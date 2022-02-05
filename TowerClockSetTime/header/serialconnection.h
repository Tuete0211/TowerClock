#ifndef SERIALCONNECTION_H
#define SERIALCONNECTION_H

#include <QObject>
#include <QSerialPort>
#include <QSerialPortInfo>
#include <QTimer>

class SerialConnection : public QObject
{
    Q_OBJECT
public:
    explicit SerialConnection(QObject *parent = nullptr);
    ~SerialConnection();

    enum State {
        Disconnected,
        Connected,
        Identified,
        Ready2Sync
    };

    QString getData() const;

    void writeSerial(QString str);
    bool connectToDevice(QSerialPortInfo serialPortInfo);
    void disconnectFromDevice();
    void setTimer(int time);

    State getState() const;
    void setState(State newState);

public slots:
    void readSerial();
    void timerTimeout();
    void pockeDevice();
    void deleteSerialConnection();

    void errorHandler(QSerialPort::SerialPortError error);

private:
    QSerialPort *serialDevice;
    QString m_data;
    QString m_input;
    State m_state = Disconnected;

    QTimer *m_timer;
    QTimer *m_interval;

    void setData(const QString &data);

signals:

    void deviceInfoChanged();
    void serialDataChanged();
    void deviceDisconnected();
    void towerClockConnected(SerialConnection*);
    void stateChanged();

    void statusMessage(QString, int);

};

#endif // SERIALCONNECTION_H
