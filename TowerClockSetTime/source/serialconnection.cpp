#include "header/serialconnection.h"
#include <QDebug>
#include <QRegularExpression>

SerialConnection::SerialConnection(QObject *parent)
    : QObject{parent}
{
    serialDevice = new QSerialPort();
}

SerialConnection::~SerialConnection()
{
    disconnectFromDevice();
    qDebug() << "[SerialConnection]\t" << "Delete SerialConnection of port " << serialDevice->portName();
}

QString SerialConnection::getData() const
{
    return m_data;
}

void SerialConnection::writeSerial(QString str)
{
    qDebug() << "Write to device: " << (str + "\r\n").toLatin1();
    if(getState() >= State::Connected)serialDevice->write((str + "\r\n").toLatin1());
}

bool SerialConnection::connectToDevice(QSerialPortInfo serialPortInfo)
{
    if(serialPortInfo.hasProductIdentifier() && !serialPortInfo.isNull() && (serialPortInfo.description().contains("CH340", Qt::CaseInsensitive) || serialPortInfo.description().contains("Arduino", Qt::CaseInsensitive) || serialPortInfo.description().contains("USB", Qt::CaseInsensitive))) {
        serialDevice->setPort(serialPortInfo);
        if(!serialDevice->open(QSerialPort::ReadWrite)){
            qDebug() << "[SerialConnection]\t[" << serialPortInfo.portName() << "]\tPort konnte nicht geöffnet werden";
            setState(State::Disconnected);
            return false;
        }
        serialDevice->setBaudRate(QSerialPort::Baud9600);
        serialDevice->setDataBits(QSerialPort::Data8);
        serialDevice->setFlowControl(QSerialPort::NoFlowControl);
        serialDevice->setParity(QSerialPort::NoParity);
        serialDevice->setStopBits(QSerialPort::OneStop);
        serialDevice->setDataTerminalReady(false);
        QObject::connect(serialDevice, SIGNAL(readyRead()), this, SLOT(readSerial()));
        QObject::connect(serialDevice, SIGNAL(errorOccurred(QSerialPort::SerialPortError)), this, SLOT(errorHandler(QSerialPort::SerialPortError)));
        setState(State::Connected);
        return true;
    }
    qDebug() << "[SerialConnection]\t[" << serialPortInfo.portName() << "]\tPort is Null";
    setState(State::Disconnected);
    return false;
}

void SerialConnection::disconnectFromDevice( )
{
    serialDevice->close();
    setState(State::Disconnected);
    emit deviceDisconnected();
}

void SerialConnection::setTimer(int time)
{
    // timer for whole connection process
    m_timer = new QTimer(this);
    connect(m_timer, SIGNAL(timeout()),
            this, SLOT(timerTimeout()));
    m_timer->setSingleShot(true);
    m_timer->start(time);

    // interval timer to signal to TowerClock
    m_interval = new QTimer(this);
    connect(m_interval, SIGNAL(timeout()),
            this, SLOT(pockeDevice()));
    m_interval->setSingleShot(false);
    m_interval->setInterval(500);
    m_interval->start(500);

}

void SerialConnection::readSerial()
{
    QByteArray serialData = serialDevice->readAll();
    m_input.append(serialData);
    //qDebug() << QString::fromLatin1(serialData) << "-->" << m_input;
    if(m_input.endsWith("\r\n")){
        if(getState() < State::Identified) qDebug() << "Input:" << m_input;
        if(getState() >= State::Identified){
            // listen for other commands
            if(m_input.contains("RTS"))
                setState(State::Ready2Sync);
            if(m_input.contains("TCS"))
                setState(State::Identified);
            setData(m_input.remove(m_input.length()-2, 2));
        } else {
            if(m_input.contains("TowerClock")){
            //string.replace(QRegularExpression("[^a-zA-Z0-9_]"), "");                          // filter unknown characters.
                setState(State::Identified);
                qDebug() << "[SerialConnection]\t" << "Device identified: " << "TowerClock";
                emit statusMessage("TowerClock identified", 2000);
                // disable timer
                m_timer->stop();
                delete m_timer;
                m_interval->stop();
                delete m_interval;

                emit towerClockConnected(this);
                emit deviceInfoChanged();
            } else{
                qDebug() << "[SerialConnection]\t" << "Wrong name (" << m_input.remove(m_input.length()-2, 2) << ") on port:" << serialDevice->portName();
            }
        }
        m_input = "";
    }
}

void SerialConnection::timerTimeout()
{
    delete m_timer;
    m_interval->stop();
    delete m_interval;
    emit statusMessage("Failed to connect " + serialDevice->portName() + " (timeout)", 2000);
    delete this;
}

void SerialConnection::pockeDevice()
{
    static bool v = true;
    if(v)
        emit statusMessage("Connecting...", 500);
    else
        emit statusMessage("Connecting", 500);
    v=!v;
    writeSerial("TCTS");
}

void SerialConnection::deleteSerialConnection()
{
    delete this;
}

void SerialConnection::errorHandler(QSerialPort::SerialPortError error)
{
    qDebug() << "[SerialConnection]\t" << "Serial Port Error: " << error;
    emit statusMessage("Device Disconnected: Serial Port Error", 5000);
    if(error == QSerialPort::SerialPortError::ResourceError || error == QSerialPort::SerialPortError::PermissionError){
        setState(State::Disconnected);
        emit deviceDisconnected();
    }
}

SerialConnection::State SerialConnection::getState() const
{
    return m_state;
}

void SerialConnection::setState(State newState)
{
    if (m_state == newState)
        return;
    m_state = newState;
    emit stateChanged();
}

void SerialConnection::setData(const QString &data)
{
    m_data = data;
    emit serialDataChanged();
}
