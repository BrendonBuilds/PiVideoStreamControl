#ifndef IMAGEPROVIDER
#define IMAGEPROVIDER

#include <QDebug>
#include <QQuickImageProvider>
#include <QVideoFrame>
#include <QTimer>
#include <QElapsedTimer>
#include <QSize>
#include <QMap>

#include <opencv2/opencv.hpp>
#include <opencv2/core/core.hpp>

class ImageProvider : public QObject, public QQuickImageProvider
{
    Q_OBJECT

    Q_PROPERTY(QString status READ getStatus WRITE setStatus NOTIFY signal_newStatus)

public:
    ImageProvider() : QQuickImageProvider(QQuickImageProvider::Image) {
        m_pFrameCounterTimer = new QTimer(this);

        connect(m_pFrameCounterTimer, SIGNAL(timeout()), this, SLOT(updateFPS()));

        m_pFrameCounterTimer->start(1000);
        m_framerateTimer.restart();
    }

    virtual ~ImageProvider() {
        delete m_pFrameCounterTimer;
    }

    // return an image from the cache if it exists, otherwise a placeholder is returned
    QImage requestImage(const QString &id, QSize *size, const QSize &requestedSize);

public slots:
    // add an image to the cache
    void addImage(const QImage& image, const QString& sTitle)
    {
        m_mImageCache[sTitle] = image;
        emit signal_newData(sTitle);
    }

    // add a frame to the cache
    void addVideoFrame(const cv::Mat& videoFrame);

    // control output, only take performance hit when viewing the data
    void setOutputOptions(const bool& bDrawFocusOverlay, const bool& bDrawHistogram) {
        m_bDrawFocusOverlay = bDrawFocusOverlay;
        m_bDrawHistogram = bDrawHistogram;
    }

    // get/set status
    QString getStatus() const { return m_sStatus; }
    void setStatus(const QString& sStatus) {
        m_sStatus = sStatus;
        emit signal_newStatus(m_sStatus);
    }

private slots:
    void updateFPS() {
        float fFrameRate = 1000.0f / (float(m_framerateTimer.elapsed()) / float(m_iFrameCount));

        qDebug() << "Total frames:" << m_iFrameCount << "," << fFrameRate << "fps";
        setStatus(QString::number(fFrameRate) + " fps");

        m_iFrameCount = 0;
        m_framerateTimer.restart();
    }

signals:
    // notify the ui that new data has been cached, let it determine what to redraw
    void signal_newData(const QString& sTitle);

    // notify the ui that there is a new status message (fps)
    void signal_newStatus(const QString& sStatus);

private:
    QMap<QString, QImage> m_mImageCache;

    bool m_bDrawFocusOverlay = false;
    bool m_bDrawHistogram = false;

    QTimer *m_pFrameCounterTimer;
    QString m_sStatus;
    QElapsedTimer m_framerateTimer;
    int m_iFrameCount = 0;
};

#endif // IMAGEPROVIDER
