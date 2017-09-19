#include "threadPlayStream.h"
#include <QDebug>
//#include "mainwindow.h"
//#include "ui_mainwindow.h"
//extern MainWindow *w;


threadPlayStream::threadPlayStream(QMutex* mu, QString url, QObject *parent) : QThread(){

    mutex = mu;
    capture.open(url.toStdString());

    timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(captureImage()));
    timer->start(50);
    connectionStateNow = connectionStatePrev = false;
}

threadPlayStream::~threadPlayStream(){
    stopThread=true;
}

void threadPlayStream::run(){

    emit notConnected();
    //CvCapture *x =
    //#undef TIME
    //#define TIME 4

    if (capture.isOpened()){

        while(!stopThread) {

            cv::Mat frame;
            if (capture.read(frame)){

                connectionStateNow = true;
                if (connectionStateNow && !connectionStatePrev){
                    emit connected();
                    connectionStatePrev = connectionStateNow;
                    qDebug() << "connected";
                }

                if (frame.empty())
                    qDebug() << "empty";

                //qDebug() << frame.rows << "x" << frame.cols;
                if (captureFlag) {
                    //mutex->lock();
                    iter++;
                    //cv::Mat dest;
                    cv::cvtColor(frame, dest, CV_BGR2RGB);
                    //QImage *t = new  QImage( (uchar*) dest.data, dest.cols, dest.rows, dest.step, QImage::Format_RGB888 );
                    //imgBuffer.append( t );

                    //buffer = QImage( (const uchar*) dest.data, dest.cols, dest.rows, dest.step, QImage::Format_RGB888 );
                    //buffer.save("x.jpg");
                    //buffer.load("x.jpg");
                    //QString file="x"+QString::number(iter)+".jpg";
                    //imgBuffer.last()->save(file);

                    //w->ui->label->setPixmap(QPixmap::fromImage(*imgBuffer.last()));
                    captureFlag = false;
                    emit imageCaptured();
                    //mutex->unlock();
                }
            } else {
                connectionStateNow = false;
                if (!connectionStateNow && connectionStatePrev) {
                    emit notConnected();
                    connectionStatePrev = connectionStateNow;
                    qDebug() << "not connected 1";
                }
                //qDebug() << "1";
            }
        }
    } else {
        connectionStateNow = false;
        if (!connectionStateNow && connectionStatePrev) {
            emit notConnected();
            connectionStatePrev = connectionStateNow;
            qDebug() << "not connected 2";
        }
        qDebug() << "2";
    }
    //                    std::vector<uchar> buf(50000);
    //                    imencode(".jpg", frame, buf);
    //                    QImage *t = new  QImage((char*) &buf);
}

void threadPlayStream::captureImage(){
   captureFlag = true;
}

QImage threadPlayStream::getImage() {

    QMutexLocker locker(mutex);
    QImage img(buffer);
    return img;
}
