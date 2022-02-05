#include "header/mainwindow.h"
#include "ui_mainwindow.h"

#include <QDateTime>
#include <QFontDatabase>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    // setup custom font
    qDebug() << QFontDatabase::addApplicationFont("://fonts/moonhouse");

    ui->setupUi(this);

    ui->Info->setHidden(true);

    ui->Light_Clock->setHidden(true);
    ui->Light_UpperDeck->setHidden(true);
    ui->Light_Static->setHidden(true);

    ui->Light_Sync_1->setHidden(true);
    ui->Light_Sync_2->setHidden(true);

    // interval timer to signal to TowerClock
    m_syncLightInterval = new QTimer(this);
    connect(m_syncLightInterval, SIGNAL(timeout()),
            this, SLOT(stateChanged()));
    m_syncLightInterval->setSingleShot(false);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::registerTowerClock(SerialConnection *con)
{
    m_serialConnection = con;

    connect(m_serialConnection, SIGNAL(stateChanged()),
            this, SLOT(stateChanged()));

    // set tower lights
    stateChanged();
    qDebug() << "TowerClock connected!";
}


void MainWindow::on_connectButton_clicked()
{
    //Alle möglichen Serial Ports in einer Liste sammeln
    m_availablePorts = m_serialPortInfo->availablePorts();

    if(m_availablePorts.size() <= 0){
        qDebug() << "[MainWindow]\t" << "No Serial Devices connected!";
        return;
    }
    // Debug
    QString str = "";
    foreach(const QSerialPortInfo &serialPortInfo, m_availablePorts){
        str += serialPortInfo.portName() + ", ";
    }
    qDebug() << "[MainWindow]\t" << "Available Ports: " << str;

    // check if any port might be the right one
    search4TowerClock();
}

void MainWindow::search4TowerClock()
{
    // check every port to be the right one
    foreach(const QSerialPortInfo &port, m_availablePorts){
        SerialConnection *con = new SerialConnection();
        // connect to Device
        if(!con->connectToDevice(port)){
            qDebug() << "[MainWindow]\t[" << port.portName() << "]\tPort nicht verbindbar";
            continue;
        }
        connect(con, SIGNAL(towerClockConnected(SerialConnection*)),
                this, SLOT(registerTowerClock(SerialConnection*)));
        connect(con, SIGNAL(statusMessage(QString, int)),
                ui->statusBar, SLOT(showMessage(QString, int)));

        con->setTimer(15000);
    }
}

void MainWindow::setTime()
{
    // erstellen des Zeitstempels
    QDateTime current = QDateTime::currentDateTimeUtc().addSecs(60*60);
    QDate date = current.date();
    QTime time = current.time();
    QString str = "CD";
    str += QString::number(date.year()).remove(0, 2);
    str += QString("%1").arg(date.month(), 2, 10, QChar('0'));
    str += QString("%1").arg(date.day(), 2, 10, QChar('0'));
    str += QString::number(date.dayOfWeek());
    str += QString("%1").arg(time.hour(), 2, 10, QChar('0'));
    str += QString("%1").arg(time.minute(), 2, 10, QChar('0'));
    str += QString("%1").arg(time.second(), 2, 10, QChar('0'));
    str += "x";
    qDebug() << "Ergebnis:" << str;

    if(m_serialConnection->getState() == SerialConnection::State::Ready2Sync)
        m_serialConnection->writeSerial(str);
}



void MainWindow::on_syncButton_clicked()
{
    if(m_serialConnection && m_serialConnection->getState() == SerialConnection::State::Ready2Sync)
        setTime();
    else
        ui->statusBar->showMessage("Unable to sync, TowerClock not yet primed", 5000);
    //qDebug() << "Unable to sync, TowerClock not primed yet";
}


void MainWindow::on_primeButton_clicked()
{
    if(m_serialConnection && m_serialConnection->getState() == SerialConnection::State::Identified)
        m_serialConnection->writeSerial("AB");
    else
        ui->statusBar->showMessage("Unable to prime, TowerClock not connected", 5000);
    //qDebug() << " PrimeButton: No TowerClock connected";
}


void MainWindow::on_disconnectButton_clicked()
{
    if(m_serialConnection && m_serialConnection->getState() >= SerialConnection::State::Connected){
        m_serialConnection->writeSerial("EF");
        QTimer *t = new QTimer();
        t->setSingleShot(true);
        connect(t, SIGNAL(timeout()),
                m_serialConnection, SLOT(deleteSerialConnection()));
        t->start(200);
    }
    else
        ui->statusBar->showMessage("Unable to disconnect, TowerClock not connected", 5000);
    //qDebug() << "DisconnectButton: No TowerClock connected";
}

void MainWindow::stateChanged()
{
    switch(m_serialConnection->getState()){
    case SerialConnection::Disconnected:
        if(m_syncLightInterval->isActive())
            m_syncLightInterval->stop();
        ui->Light_Clock->setHidden(true);
        ui->Light_UpperDeck->setHidden(true);
        ui->Light_Static->setHidden(true);

        ui->Light_Sync_1->setHidden(true);
        ui->Light_Sync_2->setHidden(true);
        break;
    case SerialConnection::Connected:
        // will never be called due to the software design
        break;
    case SerialConnection::Identified:
        if(m_syncLightInterval->isActive())
            m_syncLightInterval->stop();
        ui->Light_Clock->setHidden(false);
        ui->Light_UpperDeck->setHidden(false);
        ui->Light_Static->setHidden(false);

        ui->Light_Sync_1->setHidden(true);
        ui->Light_Sync_2->setHidden(true);
        break;
    case SerialConnection::Ready2Sync:
        if(!m_syncLightInterval->isActive())
            m_syncLightInterval->start(300);

        if(ui->Light_Sync_1->isHidden()){
            ui->Light_Sync_1->setHidden(false);
            ui->Light_Sync_2->setHidden(true);
        } else {
            ui->Light_Sync_1->setHidden(true);
            ui->Light_Sync_2->setHidden(false);
        }

        ui->Light_Clock->setHidden(false);
        ui->Light_UpperDeck->setHidden(false);
        ui->Light_Static->setHidden(false);

        break;
    }
}


void MainWindow::on_infoButton_toggled(bool checked)
{
    if(checked)
        ui->Info->setHidden(false);
    else
        ui->Info->setHidden(true);
}

