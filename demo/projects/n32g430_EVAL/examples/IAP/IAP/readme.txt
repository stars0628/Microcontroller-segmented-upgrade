此工程是将一个程序bin大于rom空间的分段升级的简单示例
此工程为Bootloader工程，下面简称本工程

此工程默认收到的数据只有两种，一是命令字，二是bin文件分段，在此简单实现中每个bin数据包只有crc16校验，未追加帧头帧位，数据包长度，数据包id等信息

此工程包含两个比较重要的内容
IAP相关文件是对flash读写，跳转的相关内容
Segmented_IAP是对收到的分段bin文件处理的相关内容，针对移植到其他单片机做bin分段接受处理，只需移植此文件并修改相关内容

注意在Segmented_IAP.h修改两个宏定义的值，在此工程中传输bin数据时，一帧与bin文件无关的数据长度只为2，APP烧录地址是(0x08000000 + 20*1024)

单片机复位运行后，首先设置并初始化IAP用到的片上外设。
在while中检查是否需要升级，即获得的标志是否满足跳转到APP函数，或者是否升级完成，跳转到APP


在Segmented_IAP.c文件中，定义了三个结构体类型
App_Upgrade_Package_Data	App升级包的信息
Serial_Port_Data_Information	串口收到的数据信息
Iap_Flag	相关标志信息




在Segmented_IAP中有以下两个函数
void Transmission_and_Upgrade(uint8_t *serial_data, uint32_t length);//对收到的bin数据段处理
void cmd_or_data_deal(uint8_t *serial_data,uint32_t length);//判断收到的数据是bin还是控制字
以上两个函数可以根据自己的实际需要去补充完善，移植需要根据单片机修改相关内容

本工程在串口接收中断中调用cmd_or_data_deal函数，在其中判断收到的数据是否为控制字（即有帧头帧尾），如果不是则认为收到的数据为bin文件分段
命令内容包含帧头，功能码，数据值，帧尾，crc16校验值

1.如果串口收到0a 0b 11 datah datal a0 b0 crch crcl，意味着开始传输bin文件，datah和datal为bin文件总数 ，crch 和 crcl为crc16校验码
随后发送0a 0b 01 b0 a0，上位机软件收到数据后开始发送第一段bin数据包，后续串口收到校验通过的数据包则发送0a 0b 01 b0 a0继续请求下一数据包，否则发送0a 0b 11 datah datal a0 b0 crch crcl请求重发本段数据包
直到收到的有效数据包数量等于datah<<8 | datal，则认为升级完成，发送0a 0b 10  a0 b0 crch crcl到上位机并将相关标志置位，跳转到App地址

2.如果串口收到0a 0b 00 a0 b0 crch crcl，则将所有数据恢复初始状态，并重新等待升级