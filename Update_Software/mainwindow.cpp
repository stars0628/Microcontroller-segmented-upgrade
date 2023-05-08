#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDebug>
#include <QMessageBox>

int step = 0;//记录发了多少部分bin文件
uint16_t chunk_count_u16 = 0;//生成的bin文件数
QString selected_file;//选择bin文件路径
QString srcDirPath;

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    on_pushButton_4_clicked();
    ui->BitBox->setCurrentIndex(3);
    ui->size->setCurrentIndex(2);
    ui->BaudBox->setCurrentIndex(1);

//    QString App_Dir = QCoreApplication::applicationDirPath();
//    srcDirPath = App_Dir+"/bin_out/";//分割保存路径
//    ui->label_8->setText("输出路径："+srcDirPath);
//                                     qDebug()<<srcDirPath;
//                         QDir dir(srcDirPath);
//                         dir.mkdir(srcDirPath);

    connect(&serial, SIGNAL(readyRead()), this, SLOT(serial_readyRead()));

    connect(mTime,&QTimer::timeout,this,[=]()
            {
                float schedule;
                //ui->DataReceived->setText(serial.readAll().toHex());
                QString array = serial.readAll().toHex();
                QString bin_num_str;
                qDebug()<<array;
                if(array == "0a0b01b0a0")//发送下一段
                {
                    bin_num_str = bin_num_str.number(step);
                    step++;
                    qDebug()<<step;
                    readFile(srcDirPath+"/bin_part." + bin_num_str + ".bin");
                    qDebug()<<srcDirPath+"/bin_part." + bin_num_str + ".bin";
                    schedule = ((float)step/(float)chunk_count_u16)*100.0;
                    ui->progressBar->setValue(schedule);
                }
                else if(array == "0a0b00b0a0")//重发此段
                {
                    step--;
                    bin_num_str = bin_num_str.number(step);
                    readFile(srcDirPath+"/bin_part." + bin_num_str + ".bin");
                    step++;
                }
                else if(array == "0a0b10b0a0")//升级成功
                {
                    step = 0;
                    ui->pushButton_5->setEnabled(true);
                    ui->pushButton_6->setEnabled(true);
                    ui->pushButton_2->setEnabled(true);
                    ui->pushButton->setEnabled(true);
                    ui->size->setEnabled(true);
                }
                mTime->stop();
            });


}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_pushButton_4_clicked()//刷新串口
{
    // 清除当前显示的端口号
    ui->PortBox->clear();
    //检索端口号和端口名字
    foreach(const QSerialPortInfo &info, QSerialPortInfo::availablePorts())
    {
        ui->PortBox->addItem(info.portName()+"  "+info.description());
    }
}

void MainWindow::disable_commobox()//批量失能commobox
{
    ui->BaudBox->setDisabled(true);
    ui->BitBox->setDisabled(true);
    ui->PortBox->setDisabled(true);
    ui->ParityBox->setDisabled(true);
    ui->StopBox->setDisabled(true);
}

void MainWindow::enable_commobox()//批量使能commobox
{
    ui->BaudBox->setDisabled(false);
    ui->BitBox->setDisabled(false);
    ui->PortBox->setDisabled(false);
    ui->ParityBox->setDisabled(false);
    ui->StopBox->setDisabled(false);
}
void MainWindow::on_pushButton_2_clicked()
{
    if(ui->pushButton_2->text() == "连接")
    {
        QString SerialCom = ui->PortBox->currentText();
        QString Baud = ui->BaudBox->currentText();
        QStringList SerialComList = SerialCom.split("  ");
        serial.setPortName(SerialComList[0]);
        if(Baud == "9600")
        {
            serial.setBaudRate(QSerialPort::Baud9600);
        }
        else if(Baud == "115200")
        {
            serial.setBaudRate(QSerialPort::Baud115200);
        }


        // 设置数据位、校验位和停止位
        serial.setDataBits(QSerialPort::Data8);
        serial.setParity(QSerialPort::NoParity);
        serial.setStopBits(QSerialPort::OneStop);


        if(!serial.open(QIODevice::ReadWrite)) {
            qDebug() << "Failed to open serial port";
            return;
        }
        ui->pushButton_2->setText("断开");
        serial_open();
    }
    else
    {
        serial.close();
        ui->pushButton_2->setText("连接");
        serial_close();
    }
}

void MainWindow::serial_readyRead()
{
    mTime->start(20);
}

void MainWindow::serial_open()
{
    disable_commobox();
    ui->pushButton_5->setEnabled(true);
    ui->pushButton_6->setEnabled(true);
    ui->pushButton_7->setEnabled(true);
    ui->pushButton_4->setEnabled(false);
}

void MainWindow::serial_close()
{
    enable_commobox();
    ui->pushButton_5->setEnabled(false);
    ui->pushButton_6->setEnabled(false);
    ui->pushButton_7->setEnabled(false);
    ui->pushButton_4->setEnabled(true);
}


void MainWindow::on_pushButton_clicked()//获取选择的bin文件所在的路径
{
    // 创建文件选择对话框，设置对话框标题和默认打开的目录
    QFileDialog dialog(nullptr, "Select a file", QDir::currentPath(), "Bin (*.bin)");
        // 设置对话框打开模式为选择单个文件
    dialog.setFileMode(QFileDialog::ExistingFile);
        // 显示文件选择对话框，并等待用户的选择

    if (dialog.exec() == QDialog::Accepted) {
        // 获取用户所选文件的路径
        selected_file = dialog.selectedFiles()[0];
        ui->label_7->setText("Bin文件："+selected_file);
    }
}

#define CRC16_MODBUS_POLY 0xA001
// 计算CRC16 Modbus校验码
uint16_t crc16_modbus(uint8_t *data, uint32_t len)
{
    uint16_t crc = 0xFFFF;
    for (uint32_t i = 0; i < len; i++) {
        crc ^= (uint16_t)data[i];
        for (int j = 0; j < 8; j++) {
            if (crc & 0x0001) {
                crc = (crc >> 1) ^ CRC16_MODBUS_POLY;
            } else {
                crc >>= 1;
            }
        }
    }
    return crc;
}

int MainWindow::Bin_Deal(QString file,int size,QString save_file)//对bin文件进行分割操作
{

        QByteArray input_file = file.toUtf8();
        const char *output_prefix = "bin_part";
        QByteArray output_path = save_file.toUtf8(); // 输出文件夹路径
        clearFolder(output_path);
        int chunk_size = size;
        int chunk_count = 0;
        FILE *input_fp = fopen(input_file, "rb");
        if (input_fp == NULL) {
            printf("Could not open input file.\n");
            return 1;
        }
        // 获取原始文件大小
        fseek(input_fp, 0, SEEK_END);
        uint32_t file_size = ftell(input_fp);
        rewind(input_fp);

        ui->label_9->setText("bin文件总大小：" + QString::number(file_size));

        uint8_t *buffer = (uint8_t *)malloc(chunk_size);
        uint16_t crc;
        char output_filename[100];
        FILE *output_fp;
        while (!feof(input_fp)) {
            size_t read_bytes = fread(buffer, 1, chunk_size, input_fp);
            if (read_bytes == 0) {
                break;
            }
            QByteArray bin_out_array = (save_file+"/%s.%d.bin").toUtf8();
            snprintf(output_filename, 100, bin_out_array, output_prefix, chunk_count); // 修改生成的文件名，将路径包含在内
            output_fp = fopen(output_filename, "wb");
            if (output_fp == NULL) {
                printf("Could not create output file.\n");
                return 1;
            }
            fwrite(buffer, 1, read_bytes, output_fp);
            crc = crc16_modbus(buffer, read_bytes);
            fwrite(&crc, 1, sizeof(crc), output_fp);
            fclose(output_fp);
            chunk_count++;
        }
        chunk_count_u16 = chunk_count;
        ui->label_10->setText("输出bin文件总数：" + QString::number(chunk_count));

        fclose(input_fp);
        free(buffer);
        return 0;
}

void MainWindow::clearFolder(const QString& folderPath)//清除路径下的文件和文件夹
{
        QDir folder(folderPath);
        if (folder.exists()) {
            Q_FOREACH(QFileInfo fileInfo, folder.entryInfoList(QDir::NoDotAndDotDot | QDir::AllEntries)) {
                if (fileInfo.isDir()) {
                QDir(fileInfo.absoluteFilePath()).removeRecursively();
                } else {
                QFile(fileInfo.absoluteFilePath()).remove();
                }
            }
        }
}

void MainWindow::on_pushButton_5_clicked()//开始升级
{
        if(selected_file != ""||srcDirPath != "")
        {
            step = 0;
            QString bin_size = ui->size->currentText();
            int size = bin_size.toInt()-2;
            Bin_Deal(selected_file,size,srcDirPath);

            uint8_t num_h = chunk_count_u16 >> 8;
            uint8_t num_l = chunk_count_u16 & 0xff;
            uint8_t data[] = {0x0a,0x0b,0x11,num_h,num_l,0xb0,0xa0};
            uint16_t crc_cmd = crc16_modbus(data,sizeof(data));
            uint8_t crc_h = crc_cmd&0xff;
            uint8_t crc_l = crc_cmd>>8;
            uint8_t senddata[] = {0x0a,0x0b,0x11,num_h,num_l,0xb0,0xa0,crc_h,crc_l};
            QByteArray sendData = QByteArray::fromRawData((char*)senddata, sizeof(senddata));

            serial.write(sendData);

            ui->pushButton_5->setEnabled(false);
            ui->pushButton_6->setEnabled(false);
            ui->pushButton_2->setEnabled(false);
            ui->pushButton->setEnabled(false);
            ui->pushButton_4->setEnabled(false);
            ui->size->setEnabled(false);
        }
        else
        {
            QMessageBox::warning(this, tr("警告"),  tr("未选择升级文件或设置输出路径"),QMessageBox::Close);
        }
}

// 从指定路径读入文件
QByteArray MainWindow::readFile(const QString& filePath)
{
        QFile file(filePath);
        if (!file.open(QIODevice::ReadOnly)) {
            qDebug() << "failed to open file: " << file.errorString();
            return QByteArray();
        }

        QByteArray fileData = file.readAll();
        serial.write(fileData);//发送bin文件
        file.close();
        return fileData;
}

void MainWindow::on_pushButton_6_clicked()//进入升级模式
{
        uint8_t data[] = {0x55,0x50,0x44,0x41,0x54,0x45};
        uint16_t crc_cmd = crc16_modbus(data,sizeof(data));
        uint8_t crc_h = crc_cmd&0xff;
        uint8_t crc_l = crc_cmd>>8;
        uint8_t senddata[] = {0x55,0x50,0x44,0x41,0x54,0x45,crc_h,crc_l};
        QByteArray sendData = QByteArray::fromRawData((char*)senddata, sizeof(senddata));
        serial.write(sendData);
}


void MainWindow::on_pushButton_7_clicked()//
{
        ui->pushButton_5->setEnabled(true);
        ui->pushButton_6->setEnabled(true);
        ui->pushButton_2->setEnabled(true);
        ui->pushButton->setEnabled(true);
        ui->size->setEnabled(true);
}


void MainWindow::on_pushButton_3_clicked()
{
        srcDirPath = QFileDialog::getExistingDirectory(
        this, "选择输出文件夹路径",
        "/");
        ui->label_8->setText("输出路径："+srcDirPath);

}

