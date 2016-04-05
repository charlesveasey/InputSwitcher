#ifndef WINDOW_H
#define WINDOW_H

#include <QMainWindow>
#include <QSignalMapper>

#include "serialthread.h"

QT_BEGIN_NAMESPACE

class QLabel;
class QLineEdit;
class QSpinBox;
class QPushButton;
class QComboBox;

QT_END_NAMESPACE

class Window : public QWidget
{
    Q_OBJECT

public:
    Window(QWidget *parent = 0);

private slots:
    void processError(const QString &s);
    void processTimeout(const QString &s);
    void buttonClicked(const int &index);
    void dataRead(const QString &s);

private:
    QSignalMapper* signalMapper;
    int current;
    void start();
    void update(const int &i);

private:
    QPushButton *input[4];
    QLabel *statusLabel;
    SerialThread serialThread;
};

#endif // WINDOW_H
