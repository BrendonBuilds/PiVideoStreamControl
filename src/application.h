#ifndef APPLICATION_H
#define APPLICATION_H

#include <QObject>
#include <QDebug>
#include <QCoreApplication>
#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QProcess>
#include <QQmlContext>
#include <QMediaPlayer>
#include <QElapsedTimer>

#include "imageprovider.h"
#include "videoframegrabber.h"

#include <opencv2/opencv.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>

class Application : public QObject
{
    Q_OBJECT
public:
    explicit Application(QObject *parent = nullptr) : QObject(parent) {
    }

    virtual ~Application() {
        delete m_pImageProvider;

        m_pMediaPlayer->stop();
        delete m_pMediaPlayer;
        delete m_pFrameGrabber;

        delete m_pEngine;
        delete m_pApp;
    }

    // run the application
    int run(int &argc, char **argv);

public slots:
    // connect probe to mediaplayer
    void connectToStream();

    void stopStream();

    // run a remote command on the device
    void runRemoteCommand(const QString& sRemoteAddr, const QString& sRemoteUser, const QString& sRemoteCommand);

    // catch a video frame from the stream
    void catchFrame(const QVideoFrame& frame);

private:
    QGuiApplication *m_pApp;
    QQmlApplicationEngine *m_pEngine;

    QElapsedTimer timer;
    int iFrameCount = 0;

    ImageProvider *m_pImageProvider;
    QMediaPlayer *m_pMediaPlayer;
    VideoFrameGrabber *m_pFrameGrabber;
};

#endif // APPLICATION_H
