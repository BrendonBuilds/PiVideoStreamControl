#include "application.h"

int Application::run(int& argc, char** argv)
{
    // declare app, engine
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    m_pApp = new QGuiApplication(argc, argv);
    m_pEngine = new QQmlApplicationEngine();

    // link to main qml screen
    const QUrl url(QStringLiteral("qrc:/qml/main.qml"));
    QObject::connect(m_pEngine, &QQmlApplicationEngine::objectCreated,
                     m_pApp, [url](QObject *obj, const QUrl &objUrl) {
        if (!obj && url == objUrl)
            QCoreApplication::exit(-1);
    }, Qt::QueuedConnection);

    // give ui access to the backend
    m_pEngine->rootContext()->setContextProperty("app", this);

    // connect the image handler
    m_pImageProvider = new ImageProvider();
    m_pEngine->addImageProvider(QLatin1String("imageProvider"), m_pImageProvider);
    m_pEngine->rootContext()->setContextProperty("imageProvider", m_pImageProvider);

    // load the ui
    m_pEngine->load(url);

    // connect frame grabber to handle video feed
    m_pMediaPlayer = new QMediaPlayer();
    m_pMediaPlayer->setMedia(QMediaContent(QUrl("gst-pipeline: tcpclientsrc host=cm4 port=8888 ! decodebin ! videoconvert ! autovideosink")));
    m_pFrameGrabber = new VideoFrameGrabber();
    m_pMediaPlayer->setVideoOutput(m_pFrameGrabber);
    connect(m_pFrameGrabber, SIGNAL(frameAvailable(QVideoFrame)), this, SLOT(catchFrame(QVideoFrame)), Qt::DirectConnection);

    // start app
    return m_pApp->exec();
}

void Application::connectToStream()
{
    m_pMediaPlayer->setMedia(QMediaContent(QUrl("gst-pipeline: tcpclientsrc host=cm4 port=8888 ! decodebin ! videoconvert ! autovideosink")));
    m_pMediaPlayer->play();
}

void Application::stopStream()
{
    m_pMediaPlayer->stop();
}

void Application::runRemoteCommand(const QString &sRemoteAddr, const QString &sRemoteUser, const QString &sRemoteCommand)
{
    QString sCommand = "ssh " + sRemoteUser + "@" + sRemoteAddr + " " + sRemoteCommand;

    qDebug() << "Running remotely:" << sCommand;
    QProcess::startDetached("ssh", QStringList() << sRemoteUser + "@" + sRemoteAddr << sRemoteCommand);
}

void Application::catchFrame(const QVideoFrame &frame)
{
    // increment frame counter
    iFrameCount++;

    // debug track fps
    if(iFrameCount % 100 == 0)
    {
        qDebug() << "Total frames:" << iFrameCount << "," << 1000.0f / (float(timer.elapsed()) / 100.0f) << "fps";
        timer.restart();
    }

    // every frame, for now
    QVideoFrame cloneFrame(frame);
    if(cloneFrame.map(QAbstractVideoBuffer::ReadOnly))
    {
        // use open cv for the image conversion
        cv::Mat videoImage;
        videoImage = cv::Mat(cloneFrame.height() + (cloneFrame.height() / 2), cloneFrame.width(), CV_8UC1, cloneFrame.bits(), cloneFrame.bytesPerLine());

        // release the video frame
        cloneFrame.unmap();

        // convert to BGR/gray
        cv::Mat videoImageBGR, videoImageGray, laplaceOutput;

        cvtColor(videoImage.clone(), videoImageBGR, cv::COLOR_YUV2BGR_I420);
        cvtColor(videoImageBGR, videoImageGray, cv::COLOR_BGR2GRAY);

        // create a blue tinted grayscale image
        cv::Mat grayScaleTinted(videoImageGray.rows, videoImageGray.cols, CV_8UC3, cv::Scalar(0, 0, 0));
        cv::Mat splitGrayTinted[3];
        cv::split(grayScaleTinted, splitGrayTinted);
        for(int i = 0;i < videoImageGray.rows;++i)
        {
            for(int j = 0;j < videoImageGray.cols;++j)
            {
                // set the gray to the blue channel
                splitGrayTinted[2].at<uint8_t>(i, j) = videoImageGray.at<uint8_t>(i, j);
            }
        }

        cv::Mat imageTinted;
        merge(splitGrayTinted, 3, imageTinted);

        // usr original frame for now, hold onto blue tinted code
        imageTinted = videoImageBGR;

        m_pImageProvider->addImage(QImage((uchar*) imageTinted.data, imageTinted.cols, imageTinted.rows, imageTinted.step, QImage::Format_RGB888), "overlay");

        // find the focused areas
        cv::Laplacian(videoImageGray, laplaceOutput, 64);
        cv::Mat focusColor(laplaceOutput.rows, laplaceOutput.cols, CV_8UC4, cv::Scalar(0, 0, 0, 0));
        cv::Mat splitBGRA[4];
        cv::split(focusColor, splitBGRA);

        for(int i = 0;i < laplaceOutput.rows;++i)
        {
            for(int j = 0;j < laplaceOutput.cols;++j)
            {
                splitBGRA[0].at<uint8_t>(i, j) = 225;
                splitBGRA[1].at<uint8_t>(i, j) = 199;
                splitBGRA[2].at<uint8_t>(i, j) = 10;
                splitBGRA[3].at<uint8_t>(i, j) = laplaceOutput.at<uint8_t>(i, j) * 3;
            }
        }

        cv::Mat imageRescaledAll;
        merge(splitBGRA, 4, imageRescaledAll);

        m_pImageProvider->addImage(QImage((uchar*) imageRescaledAll.data, imageRescaledAll.cols, imageRescaledAll.rows, imageRescaledAll.step, QImage::Format_RGBA8888), "focusOverlay");


        // *********
        // HISTOGRAM
        // *********

        // create the histogram
        cv::Mat detectionROIHSV;
        cvtColor(videoImageBGR, detectionROIHSV, cv::COLOR_BGR2HSV);

        // hue is stored in 0-180
        int hBins = 179;
        int sBins = 1;
        int histSize[] = {hBins, sBins};
        float hRanges[] = {0.0f, (float)hBins};
        float sRanges[] = {51.0f, 254.0f};
        const float* ranges[] = {hRanges, sRanges};
        int channels[] = {0, 1};

        cv::MatND hist;
        cv::calcHist(&detectionROIHSV, 1, channels, cv::Mat(), hist, 2, histSize, ranges, true, false);

        double maxVal = 0;
        minMaxLoc(hist, 0, &maxVal, 0, 0);

        // prevent divide-by-zero
        if(maxVal == 0) {
            maxVal = 1;
        }

        cv::Mat histImg = cv::Mat::zeros(hBins, hBins * 4, CV_8UC3);

        for( int h = 0; h < hBins; h++ ) {
            for( int s = 0; s < sBins; s++ ) {
                float binVal = hist.at<float>(h, s);

                // draw a rectange for each bar
                int iHeight = cvRound(binVal / maxVal * hBins);
                if(iHeight == 0) {
                    iHeight = 1;
                }

                rectangle(histImg, cv::Point(h * 4, hBins), cv::Point((h + 1) * 4, hBins - iHeight), cv::Scalar(h, 255, 255), cv::FILLED);
            }
        }

        cvtColor(histImg, histImg, cv::COLOR_HSV2RGB);
        m_pImageProvider->addImage(QImage((uchar*) histImg.data, histImg.cols, histImg.rows, histImg.step, QImage::Format_RGB888), "histo");
    }
}
