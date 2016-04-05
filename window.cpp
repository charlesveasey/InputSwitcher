#include "window.h"

#include <QLabel>
#include <QPushButton>
#include <QGridLayout>
#include <QtSerialPort/QSerialPortInfo>
#include <QDebug>

QT_USE_NAMESPACE

Window::Window(QWidget *parent)
    : QWidget(parent)
    , statusLabel(new QLabel(tr("Status: Not running.")))
    , current(-1)
{

    setWindowTitle(tr("Input Switcher"));

    Qt::WindowFlags flags = 0;
    flags = Qt::Window;
    flags |= Qt::WindowCloseButtonHint;

    setWindowFlags(flags);
    setFixedSize(450,75);

    // create buttons
    input[0] = new QPushButton("Mac");
    input[1] = new QPushButton("PC");
    input[2] = new QPushButton("Stream");
    input[3] = new QPushButton("Laptop");
    for (int i=0; i<4; i++){
        input[i]->setCheckable(true);
    }

    // create layout
    QGridLayout *mainLayout = new QGridLayout;
    for (int i=0; i<4; i++){
        mainLayout->addWidget(input[i], 0, i);
    }
    mainLayout->addWidget(statusLabel, 1, 0, 1, 4);
    setLayout(mainLayout);

    // assign signals
    connect(&serialThread, &SerialThread::error, this, &Window::processError);
    connect(&serialThread, &SerialThread::timeout, this, &Window::processTimeout);
    connect(&serialThread, &SerialThread::started, this, &Window::start);
    connect(&serialThread, &SerialThread::response, this, &Window::dataRead);


    signalMapper = new QSignalMapper (this) ;

    for (int i=0; i<4; i++){
        connect(input[i], SIGNAL(clicked()), signalMapper, SLOT(map()));
        signalMapper->setMapping(input[i], i+1);
    };

    connect(signalMapper, SIGNAL(mapped(int)),
            this, SLOT(buttonClicked(int)));

    serialThread.start();

    // set to input 1 on startup
    update(1);
    serialThread.setRequest(1);
    input[0]->setChecked(true);
}

void Window::start()
{
    statusLabel->setText(tr("Connected to Port: %1")
                         .arg(serialThread.portName));
}

void Window::processError(const QString &s)
{
    statusLabel->setText(tr("Not Connected %1").arg(s));
}

void Window::processTimeout(const QString &s)
{
    statusLabel->setText(tr("Timeout %1").arg(s));
}


void Window::buttonClicked(const int &index)
{
    if (current != index){
        update(index);
        serialThread.setRequest(index);
    }
}

void Window::dataRead(const QString &s){
    QString str = s;
    if (str.contains("Route to input", Qt::CaseInsensitive)){
        QStringList list = str.split("Route to input ");
        if (list.length() > 1){
            QString indexStr = QString(list[1].at(0));
            int index = indexStr.toInt();
            if (current != index){
                input[index-1]->setChecked(true);
                update(index);
            }
        }
    }
}

void Window::update(const int &index){
    current = index;

    for (int i=0; i<4; i++){
        if (i != index-1){
           input[i]->setChecked(false);
        }
    }
    statusLabel->setText(tr("Input: %1").arg(index));
}
