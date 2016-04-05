#include "serialthread.h"
#include <QTime>
#include <QDebug>
#include <QtSerialPort/QSerialPort>
#include <QtSerialPort/QSerialPortInfo>

QT_USE_NAMESPACE

SerialThread::SerialThread(QObject *parent)
    : QThread(parent), quit(false), requestPending(false)
{
    QMutexLocker locker(&mutex);

}

SerialThread::~SerialThread()
{
    mutex.lock();
    quit = true;
    cond.wakeOne();
    mutex.unlock();
    wait();
}

void SerialThread::run()
{
    QSerialPort serial;

    // lock thread
    mutex.lock();

    // set port name
    foreach (const QSerialPortInfo &info, QSerialPortInfo::availablePorts()){
        portName = info.portName();
        serial.setPortName(info.portName());
        break; // first in the list
   }

    // set baud rate (from device manual)
    serial.setBaudRate(QSerialPort::Baud19200);
    serial.setDataBits(QSerialPort::Data8);
    serial.setParity(QSerialPort::NoParity);
    serial.setStopBits(QSerialPort::OneStop);
    serial.setFlowControl(QSerialPort::NoFlowControl);

    // open serial port
    if (!serial.open(QIODevice::ReadWrite)) {
        emit error(tr("Can't open %1, error code %2")
                   .arg(portName).arg(serial.error()));
        return;
    }

    // unlock thread
    cond.wakeOne();
    mutex.unlock();
    //wait();

    // infinate loop for read
    while (1) {
        if (quit)
            serial.close();

        // write data
        mutex.lock();
        QString currentRequest;
        QByteArray requestData;
        if (requestPending){
           currentRequest = request;
           requestData = currentRequest.toLocal8Bit();
           serial.write(requestData);
           //qDebug() << "SERIAL WRITE:     " << request << endl;
           requestPending = false;
        }
        mutex.unlock();


        // read data
        if (serial.waitForReadyRead(100)) {
            QByteArray responseData = serial.readAll();
            while (serial.waitForReadyRead(100)){
                responseData += serial.readAll();
            }
            QString response(responseData);
            emit this->response(response);
            //qDebug() << "SERIAL READ:     " << response << endl;
        }
    }

}


void SerialThread::setRequest(int index){
    mutex.lock();
    this->request = QString::number(index);
    this->requestPending = true;
    mutex.unlock();
}
