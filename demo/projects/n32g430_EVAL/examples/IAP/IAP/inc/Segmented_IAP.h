#ifndef _Segmented_IAP_H__
#define _Segmented_IAP_H__

#include "stdint.h"

// #error 定义App程序的烧录地址，定义传输bin数据的串口数据中与bin无关的数据长度
#define FLASH_APP1_ADDR		(0x08000000 + 20*1024)	/*给BOOT预留20kb空间，APP20kb之后存储*/
#define OTHER_LENGTH 2 //bin数据的串口数据中与bin无关的数据长度


typedef struct//存储App升级包信息的结构体
{
    uint32_t bin_total_length;//App升级包总大小
    uint16_t bin_num;//Bin文件分段总数
    uint16_t bin_crc;//接收到的该段bin文件校验值
    uint8_t other_message_length;//每段bin文件中与bin文件无关信息的长度
    uint32_t bin_write_flash_addr;//接收到的该段bin文件该写入的地址
    uint16_t bin_count;//已接收到的有效bin文件数量

} App_Upgrade_Package_Data;
extern App_Upgrade_Package_Data bin_message;



typedef struct//串口接收数据信息结构体
{
    uint32_t serial_get_length;//串口收到的该段数据长度
    uint32_t bin_get_length;//接收到的该段bin文件的大小
    uint32_t bin_get_totle_length;//接收到的升级包总大小
    uint8_t serial_error_count;//串口错误总数
    uint8_t retry_count;//重试次数

} Serial_Port_Data_Information;
extern Serial_Port_Data_Information serial_message;


typedef struct
{
    uint8_t receive_bin_segment_done:1;//接收该段完成标志
    uint8_t execute_app_done:1;//升级完成标志
    uint8_t star_flag:1;//开始传输标志
} Iap_Flag;
extern Iap_Flag iap_flag;


void Transmission_and_Upgrade(uint8_t *serial_data, uint32_t length);//bin文件传输与升级
void cmd_or_data_deal(uint8_t *serial_data,uint32_t length);//cmd或bin数据区分处理
#endif

