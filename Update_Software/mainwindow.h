
#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QtSerialPort/QSerialPort>         // 提供访问串口的功能
#include <QtSerialPort/QSerialPortInfo>     // 提供系统中存在的串口信息
#include <QFileDialog>
#include <QTimer>


QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow

{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    void disable_commobox();
    void enable_commobox();
    int Bin_Deal(QString file,int size,QString save_file);
    void clearFolder(const QString& folderPath);
    QByteArray readFile(const QString& filePath);

    void serial_open();

    void serial_close();

    void Change_the_Baud_in_IAP();

private slots:
    void on_pushButton_4_clicked();

    void on_pushButton_2_clicked();

    void on_pushButton_clicked();

    void on_pushButton_5_clicked();

    void serial_readyRead();

    void on_pushButton_6_clicked();

    void on_pushButton_7_clicked();

private:
    Ui::MainWindow *ui;
    QSerialPort serial;
    QTimer *mTime = new QTimer();//用于串口接收完整数据
    QTimer *Time_wait = new QTimer();//用于发送升级数据后的等待时间

};

#endif // MAINWINDOW_H
