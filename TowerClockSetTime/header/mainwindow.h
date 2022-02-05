#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSerialPortInfo>
#include "serialconnection.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

public slots:
    void registerTowerClock(SerialConnection *con);

private slots:
    void on_connectButton_clicked();

    void on_syncButton_clicked();

    void on_primeButton_clicked();

    void on_disconnectButton_clicked();

    void stateChanged();

    void on_infoButton_toggled(bool checked);

private:
    Ui::MainWindow *ui;
    QSerialPortInfo *m_serialPortInfo;
    SerialConnection *m_serialConnection = nullptr;
    QList<QSerialPortInfo> m_availablePorts;

    QTimer *m_syncLightInterval;

    void search4TowerClock();
    void setTime();
};
#endif // MAINWINDOW_H
