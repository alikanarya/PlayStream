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
    QImage getImage();

    bool stopThread=false;
    QString url;
    cv::VideoCapture capture;
    cv::Mat dest;

    QTimer *timer;
    int targetFps = 20;
    int timerPeriod = 50;
    bool captureFlag = false;

    QImage buffer;
    QImage *bufferPtr;
    QList<QImage*> imgBuffer;
    int iter=0;

    bool connectionStatePrev, connectionStateNow;

signals:

    imageCaptured();
    connected();
    notConnected();

public slots:

    void captureImage();

private:

    QMutex* mutex;
};

#endif // THREADPLAYSTREAM_H
