#ifndef THREADPLAYSTREAM_H
#define THREADPLAYSTREAM_H

#include <QThread>
#include <opencv2/opencv.hpp>
#include <opencv2/videoio.hpp>
#include <opencv2/imgproc/imgproc.hpp>
//#include <time.h>
//#include <QTime>
#include <QTimer>
#include <QImage>
#include <QList>
#include <QMutex>
#include <QMutexLocker>
#include "../Getsystemtime/getsystemtime.h"

class threadPlayStream : public QThread{

    Q_OBJECT

public:

    explicit threadPlayStream(QMutex* mu, QString url, QObject *parent = 0);
    ~threadPlayStream();

    void run();
    void stop();
    void connect2cam();
    void setFps(int fps);
    void startCapture();
    void measureFpsFn(int msec);
    QImage getImage();

    bool stopThread=false;
    bool starting = false;
    QString url;
    cv::VideoCapture capture;
    //cv::Mat frame;
    cv::Mat dest;
    QList<cv::Mat> frameBuffer;
    int frameBufferSize = 3;
    int frameBufferIter = 0;
    bool frameBufferReady = false;
    bool debugMsg = false;

    getSystemTime timeSystem;                   // system time class
    int firstTimeTick, secondTimeTick;          // succesive system time instances
    int tickDiff;                               // difference between 2 system times

    QTimer *timer;
    QTimer *timerFps;
    int targetFps = 20;
    int realFps = 0;
    int timerPeriod = 50;
    bool captureFlag = false;
    bool measureFps = false;
    int measureFpsDuration = 0;

    QImage buffer;
    QImage *bufferPtr;
    QList<QImage*> imgBuffer;
    int iter=0;

    bool connectionStatePrev, connectionStateNow;

    int propFps = -1;
    int propIris = -1;
    int propISO = -1;

signals:

    imageCaptured(int);
    connected();
    notConnected();

public slots:

    void captureImage();
    void measureFpsSlot();

private:

    QMutex* mutex;
};

#endif // THREADPLAYSTREAM_H
