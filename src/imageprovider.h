#ifndef IMAGEPROVIDER
#define IMAGEPROVIDER

#include <QDebug>
#include <QQuickImageProvider>
#include <QSize>
#include <QMap>

class ImageProvider : public QObject, public QQuickImageProvider
{
    Q_OBJECT

public:
    ImageProvider() : QQuickImageProvider(QQuickImageProvider::Image) {}
    virtual ~ImageProvider() {}

    // return an image from the cache if it exists, otherwise a placeholder is returned
    QImage requestImage(const QString &id, QSize *size, const QSize &requestedSize)
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

public slots:
    // add an image to the cache
    void addImage(const QImage& image, const QString& sTitle)
    {
        m_mImageCache[sTitle] = image;
        emit signal_newData(sTitle);
    }

signals:
    // notify the ui that new data has been cached, let it determine what to redraw
    void signal_newData(const QString& sTitle);

private:
    QMap<QString, QImage> m_mImageCache;

    int m_iCurrentIndex = 0;
};

#endif // IMAGEPROVIDER
