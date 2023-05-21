#include "imageprovider.h"

QImage ImageProvider::requestImage(const QString &id, QSize *size, const QSize &requestedSize)
{
    // prevent compiler warnings
    Q_UNUSED(size);
    Q_UNUSED(requestedSize);

    // return cached image, ui responsible for requesting redraw
    QImage retImage;
    if(!m_mImageCache.empty() && m_mImageCache.find(id) != m_mImageCache.end())
    {
        retImage = m_mImageCache[id];
    }
    else
    {
        QImage image(QSize(4056,3040), QImage::Format_ARGB32);
        image.fill("#444444");
        retImage = image;
    }

    return retImage;
}

void ImageProvider::addVideoFrame(const cv::Mat &videoFrame)
{
    m_iFrameCount++;

    // convert to the proper color-space
    cv::Mat videoImageBGR;
#ifdef Q_OS_WIN
    cvtColor(videoFrame, videoImageBGR, cv::COLOR_YUV2RGB_NV12);
#else
    cvtColor(videoImage, videoImageBGR, cv::COLOR_YUV2RGB_I420);
#endif

    // *********
    // ORIGINAL FRAME
    // *********
    addImage(QImage((uchar*) videoImageBGR.data, videoImageBGR.cols, videoImageBGR.rows, videoImageBGR.step, QImage::Format_RGB888), "original");


    // *********
    // FOCUS OVERLAY
    // *********
    if(m_bDrawFocusOverlay)
    {
        cv::Mat videoImageGray, laplaceOutput;
        // find the focused areas
        cvtColor(videoImageBGR, videoImageGray, cv::COLOR_BGR2GRAY);
        cv::Laplacian(videoImageGray, laplaceOutput, 64);
        cv::Mat focusColor(laplaceOutput.rows, laplaceOutput.cols, CV_8UC4, cv::Scalar(0, 0, 0, 0));
        cv::Mat splitBGRA[4];
        cv::split(focusColor, splitBGRA);

        // create an overlay with the alpha layer based on the laplacian filter output
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

        // pass focus overlay to ui
        addImage(QImage((uchar*) imageRescaledAll.data, imageRescaledAll.cols, imageRescaledAll.rows, imageRescaledAll.step, QImage::Format_RGBA8888), "focusOverlay");
    }


    // *********
    // HISTOGRAM
    // *********
    if(m_bDrawHistogram)
    {
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
        addImage(QImage((uchar*) histImg.data, histImg.cols, histImg.rows, histImg.step, QImage::Format_RGB888), "histo");
    }
}
