#include "application.h"

int Application::run(int& argc, char** argv)
{
    qputenv("QT_MULTIMEDIA_BACKEND", "gstreamer");
    qputenv("QT_MULTIMEDIA_PREFERRED_PLUGINS", "gstreamer");
    //qputenv("GST_PLUGIN_PATH", "C:\\gstreamer_msvc\\1.0\\msvc_x86_64\\bin\\");

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
    m_pFrameGrabber = new VideoFrameGrabber();
    m_pMediaPlayer->setVideoOutput(m_pFrameGrabber);
    connect(m_pFrameGrabber, SIGNAL(frameAvailable(QVideoFrame)), this, SLOT(catchFrame(QVideoFrame)), Qt::DirectConnection);

    // start app
    return m_pApp->exec();
}

void Application::connectToStream(const QString& sIPAddr)
{
    m_pMediaPlayer->setMedia(QMediaContent(QUrl("gst-pipeline: tcpclientsrc host=" + sIPAddr + " port=8888 ! decodebin ! videoconvert ! qtvideosink")));
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
    QVideoFrame cloneFrame(frame);
    if(cloneFrame.map(QAbstractVideoBuffer::ReadOnly))
    {
        // use open cv for the image conversion
        cv::Mat videoImage;
        videoImage = cv::Mat(cloneFrame.height() + (cloneFrame.height() / 2), cloneFrame.width(), CV_8UC1, cloneFrame.bits(), cloneFrame.bytesPerLine());

        // release the video frame
        cloneFrame.unmap();

        // pass the frame to the imageprovider
        m_pImageProvider->addVideoFrame(videoImage);
    }
}
