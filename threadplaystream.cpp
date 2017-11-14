#include "threadPlayStream.h"
#include <QDebug>
//#include "mainwindow.h"
//#include "ui_mainwindow.h"
//extern MainWindow *w;


threadPlayStream::threadPlayStream(QMutex* mu, QString _url, QObject *parent) : QThread(){

    mutex = mu;
    url = _url;

    connectionStateNow = connectionStatePrev = false;

    timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(captureImage()));

    timerFps = new QTimer(this);
    connect(timerFps, SIGNAL(timeout()), this, SLOT(measureFpsSlot()));

}

threadPlayStream::~threadPlayStream(){

    stopThread=true;
}

void threadPlayStream::connect2cam(){

    if ( capture.open(url.toStdString()) ){
        //emit connected();
    } else
        emit notConnected();
}

void threadPlayStream::setFps(int fps){

    targetFps = fps;
    if (targetFps != 0)
        timerPeriod = 1000.0 / targetFps;
    else
        timerPeriod = 0;
}

void threadPlayStream::startCapture(){

    if (capture.isOpened()){
        iter = 0;
        timer->start(timerPeriod);
    }
}

void threadPlayStream::run(){

    //CvCapture *x =
    starting = true;
    debugMsg = false;
    frameBufferIter = 0;
    frameBufferReady = false;
    frameBuffer.clear();

    connectionStateNow = connectionStatePrev = false;

    connect2cam();

    if (capture.isOpened()){

        if (debugMsg) qDebug() << "stream opened";

        stopThread = false;

        while(!stopThread) {

           cv::Mat frame;

            firstTimeTick = timeSystem.getSystemTimeMsec();

            if (capture.read(frame)){
                //secondTimeTick = timeSystem.getSystemTimeMsec();
                //tickDiff = secondTimeTick - firstTimeTick;

                if (debugMsg) qDebug() << "stream read";


                connectionStateNow = true;
                if (connectionStateNow && !connectionStatePrev){
                    starting = false;
                    emit connected();
                    connectionStatePrev = connectionStateNow;
                    if (debugMsg) qDebug() << "connected";
                }

                //propFps = capture.get( cv::CAP_PROP_FPS );
                //propIris = capture.get( cv::CAP_PROP_IRIS );
                //propISO = capture.get( cv::CAP_PROP_ISO_SPEED );
                if (measureFps) iter++;

                //if (frame.empty()) qDebug() << "empty";

                //qDebug() << frame.rows << "x" << frame.cols;
                //if (captureFlag) {
                    //mutex->lock();
                    //cv::Mat dest;

// **                   cv::cvtColor(frame, dest, CV_BGR2RGB);
                    if (frameBufferIter < frameBufferSize)
                        frameBufferIter++;
                    else
                        frameBufferReady = true;

                    frameBuffer.append(frame);

                    captureFlag = false;
                    if (frameBufferReady) emit imageCaptured(firstTimeTick);

                    //if (frameBufferIter == frameBufferSize) frameBuffer.removeFirst();
                    //mutex->unlock();
                //}

            } else {

                connectionStateNow = false;
                if (!connectionStateNow && connectionStatePrev || starting) {
                    starting = false;
                    emit notConnected();
                    connectionStatePrev = connectionStateNow;
                    if (debugMsg) qDebug() << "not connected 1";
                }
            }
        }
    } else {
        if (debugMsg) qDebug() << "stream cant opened";
    }

    /*QImage *t = new  QImage( (uchar*) dest.data, dest.cols, dest.rows, dest.step, QImage::Format_RGB888 );
    imgBuffer.append( t );

    buffer = QImage( (const uchar*) dest.data, dest.cols, dest.rows, dest.step, QImage::Format_RGB888 );
    buffer.save("x.jpg");
    buffer.load("x.jpg");
    QString file="x"+QString::number(iter)+".jpg";
    imgBuffer.last()->save(file);
    w->ui->label->setPixmap(QPixmap::fromImage(*imgBuffer.last()));
                        std::vector<uchar> buf(50000);
                        imencode(".jpg", frame, buf);
                        QImage *t = new  QImage((char*) &buf);*/
}

void threadPlayStream::stop(){

   stopThread = false;
}

void threadPlayStream::captureImage(){

   captureFlag = true;
}

void threadPlayStream::measureFpsFn(int msec){

    iter = 0;
    measureFpsDuration = msec;
    if (measureFpsDuration == 0) {
        if (timerFps->isActive()) timerFps->stop();
        measureFps = false;
    } else {
        timerFps->start(msec);
        measureFps = true;
    }
}

void threadPlayStream::measureFpsSlot(){

    if (measureFpsDuration != 0) {
        realFps = iter * 1000.0/ measureFpsDuration;
        iter = 0;
    } else
        realFps = -1;

    if (!this->isRunning()) {
        measureFps = false;
        timerFps->stop();
    }
}

QImage threadPlayStream::getImage() {

    QMutexLocker locker(mutex);
    QImage img(buffer);
    return img;
}
