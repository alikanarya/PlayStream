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
        timer->start(timerPeriod);
    }
}

void threadPlayStream::run(){

    //CvCapture *x =
    connectionStateNow = connectionStatePrev = false;

    connect2cam();

    if (capture.isOpened()){

        stopThread = false;

        while(!stopThread) {

            cv::Mat frame;
            if (capture.read(frame)){

                connectionStateNow = true;
                if (connectionStateNow && !connectionStatePrev){
                    emit connected();
                    connectionStatePrev = connectionStateNow;
                    //qDebug() << "connected";
                }

                //if (frame.empty()) qDebug() << "empty";

                //qDebug() << frame.rows << "x" << frame.cols;
                if (captureFlag) {
                    //mutex->lock();
                    //iter++;
                    //cv::Mat dest;

                    cv::cvtColor(frame, dest, CV_BGR2RGB);

                    captureFlag = false;
                    emit imageCaptured();
                    //mutex->unlock();
                }

            } else {

                connectionStateNow = false;
                if (!connectionStateNow && connectionStatePrev) {
                    emit notConnected();
                    connectionStatePrev = connectionStateNow;
                    //qDebug() << "not connected 1";
                }
            }
        }
    } else {
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

QImage threadPlayStream::getImage() {

    QMutexLocker locker(mutex);
    QImage img(buffer);
    return img;
}
