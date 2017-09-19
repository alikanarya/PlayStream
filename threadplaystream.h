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
    QImage getImage();

    bool stopThread=false;
    cv::VideoCapture capture;
    QTimer *timer;
    bool captureFlag = false;
    QImage buffer;
    QImage *bufferPtr;
    QList<QImage*> imgBuffer;
    int iter=0;
    cv::Mat dest;


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
