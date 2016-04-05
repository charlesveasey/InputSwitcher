#ifndef SERIALTHREAD_H
#define SERIALTHREAD_H

#include <QThread>
#include <QMutex>
#include <QWaitCondition>

class SerialThread : public QThread
{
    Q_OBJECT

public:
    SerialThread(QObject *parent = 0);
    ~SerialThread();

    QString portName;

    void transaction(const QString &portName, int waitTimeout, const QString &request);
    void run();
    void setRequest(int index);

signals:
    void response(const QString &s);
    void error(const QString &s);
    void timeout(const QString &s);

private:
    QString request;
    QMutex mutex;
    QWaitCondition cond;
    bool quit;
    bool requestPending;
};

#endif // SERIALTHREAD_H
