#ifndef _Segmented_IAP_H__
#define _Segmented_IAP_H__

#include "stdint.h"

// #error ����App�������¼��ַ�����崫��bin���ݵĴ�����������bin�޹ص����ݳ���
#define FLASH_APP1_ADDR		(0x08000000 + 20*1024)	/*��BOOTԤ��20kb�ռ䣬APP20kb֮��洢*/
#define OTHER_LENGTH 2 //bin���ݵĴ�����������bin�޹ص����ݳ���


typedef struct//�洢App��������Ϣ�Ľṹ��
{
    uint32_t bin_total_length;//App�������ܴ�С
    uint16_t bin_num;//Bin�ļ��ֶ�����
    uint16_t bin_crc;//���յ��ĸö�bin�ļ�У��ֵ
    uint8_t other_message_length;//ÿ��bin�ļ�����bin�ļ��޹���Ϣ�ĳ���
    uint32_t bin_write_flash_addr;//���յ��ĸö�bin�ļ���д��ĵ�ַ
    uint16_t bin_count;//�ѽ��յ�����Чbin�ļ�����

} App_Upgrade_Package_Data;
extern App_Upgrade_Package_Data bin_message;



typedef struct//���ڽ���������Ϣ�ṹ��
{
    uint32_t serial_get_length;//�����յ��ĸö����ݳ���
    uint32_t bin_get_length;//���յ��ĸö�bin�ļ��Ĵ�С
    uint32_t bin_get_totle_length;//���յ����������ܴ�С
    uint8_t serial_error_count;//���ڴ�������
    uint8_t retry_count;//���Դ���

} Serial_Port_Data_Information;
extern Serial_Port_Data_Information serial_message;


typedef struct
{
    uint8_t receive_bin_segment_done:1;//���ոö���ɱ�־
    uint8_t execute_app_done:1;//������ɱ�־
    uint8_t star_flag:1;//��ʼ�����־
} Iap_Flag;
extern Iap_Flag iap_flag;


void Transmission_and_Upgrade(uint8_t *serial_data, uint32_t length);//bin�ļ�����������
void cmd_or_data_deal(uint8_t *serial_data,uint32_t length);//cmd��bin�������ִ���
#endif

