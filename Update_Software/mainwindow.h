
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

private slots:
    void on_pushButton_4_clicked();

    void on_pushButton_2_clicked();

    void on_pushButton_clicked();

    void on_pushButton_5_clicked();

    void serial_readyRead();

    void on_pushButton_6_clicked();

    void on_pushButton_7_clicked();

    void on_pushButton_3_clicked();

private:
    Ui::MainWindow *ui;
    QSerialPort serial;
    QTimer *mTime = new QTimer();

};

#endif // MAINWINDOW_H
