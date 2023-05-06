#include "IAP.h"

/** ҳ��С */
#define FLASH_PAGE_SIZE 2048

typedef void (*pFunction)(void);
pFunction Jump_To_Application;
uint32_t JumpAddress;


/** page���� */
static u8 FLASH_PAGE_BUF[FLASH_PAGE_SIZE];


/**
 * @brief Flash���, �ڲ�֧�ֺ���
 * @param
 *			SrcAdd ��Ҫд������ݵ���ʼ��ַ
 *      DstAddr��Ҫд�����ݵ�Falsh��ַ
 *			Len		 �����������ݳ���
 *
 * @return
 *			FLASH_ERR_PG������ʧ��
 *			FLASH_COMPL �������ɹ�
 */
static FLASH_STS FLASH_ProgramPage(uint32_t* SrcAddr, uint32_t DstAddr, uint16_t Len)
{
    uint32_t i;

    for (i = 0; i < Len; i += 4)
    {
        FLASH_Word_Program(DstAddr + i, *(uint32_t*)((uint32_t)SrcAddr + i));
        if (*(uint32_t*)(DstAddr + i) != *(uint32_t*)((uint32_t)SrcAddr + i))
        {
            return FLASH_ERR_PG;	// ����У��
        }
    }

    return FLASH_EOP;
}


/**
 * @brief �����ַд�����ⳤ������
 * @param
 *			addr����ʼ��ַ
 *      src ��д�������ָ��
 *			len ��д�����ݳ���(byte)
 *
 * @return
 *			FLASH_ERR_WRP������ʧ��
 *			FLASH_COMPL  �������ɹ�
 */
FLASH_STS MID_FLASH_E2P_Write(u32 addr, const u8* src, u32 len)
{
    uint32_t i;
    uint32_t pageAddr;  //��ҳ����ĵ�ַ
    uint32_t pageCount; //дFalsh��ҳ��
    uint32_t tmpAddr;   //��ҳ��������ַ
    uint32_t tmpLen;    //��ҳ��Ҫд��Flash�ĳ���
    uint32_t startAddr; //��ʼ��ַ�Ƿ�ҳ����
    uint32_t endAddr;   //������ַ�Ƿ�ҳ����
    uint32_t flag = 0;  //ҳ������ʱ,�Ƿ��ҳ

//	  if(FLASH_HSICLOCK_DISABLE == FLASH_ClockInit())
//    {
//        printf("HSI oscillator not yet ready\r\n");
//        while(1);
//    }

    FLASH_Unlock();
    startAddr = (addr % FLASH_PAGE_SIZE);					// ��ȡ��ʼҳƫ�Ƶ�ַ
    endAddr   = ((addr + len) % FLASH_PAGE_SIZE);	// ��ȡ����ҳƫ�Ƶ�ַ

    /** ��ʼ��ַҳ���� */
    if (startAddr == 0)
    {
        pageAddr = addr;
        pageCount = (len / FLASH_PAGE_SIZE);
        for (i = 0; i < pageCount; i++)
        {
            /** дFlashǰ�Ȳ���, ����Ҫ���� */
            if (FLASH_EOP != FLASH_One_Page_Erase(pageAddr))
            {
                FLASH_Lock();
                return FLASH_ERR_WRP;
            }

            /** ��ҳдFlash */
            if (FLASH_EOP != FLASH_ProgramPage((uint32_t*)(src + (i * FLASH_PAGE_SIZE)), pageAddr, FLASH_PAGE_SIZE))
            {
                FLASH_Lock();
                return FLASH_ERR_WRP;
            }
            pageAddr += FLASH_PAGE_SIZE;
        }

        /** ������ַҳ������ */
        if (endAddr != 0)
        {
            /** ����һҳ���� */
            for (i = 0; i < FLASH_PAGE_SIZE; i++)
            {
                FLASH_PAGE_BUF[i] = ((uint8_t*)pageAddr)[i];
            }

            /** ���㰴ҳ��������ַ */
            tmpAddr = (len % FLASH_PAGE_SIZE);

            /** ����Ҫд��Flash�����ݱ��浽����buf�� */
            for (i = 0; i < tmpAddr; i++)
            {
                FLASH_PAGE_BUF[i] = ((uint8_t*)(src + (pageCount * FLASH_PAGE_SIZE)))[i];
            }

            /** дFlashǰ�Ȳ��� */
            if (FLASH_EOP != FLASH_One_Page_Erase(pageAddr))
            {
                FLASH_Lock();
                return FLASH_ERR_WRP;
            }

            /** д���һҳ */
            if (FLASH_EOP != FLASH_ProgramPage((uint32_t*)FLASH_PAGE_BUF, pageAddr, FLASH_PAGE_SIZE))
            {
                FLASH_Lock();
                return FLASH_ERR_WRP;
            }
        }
    }
    else /** ��ʼ��ַҳ������ */
    {
        pageAddr = ((addr / FLASH_PAGE_SIZE) * FLASH_PAGE_SIZE);	//������ҳ��ҳ�����ַ
        tmpAddr  = (addr % FLASH_PAGE_SIZE);											//������ҳ�����ַ
        tmpLen   = (FLASH_PAGE_SIZE - tmpAddr);										//������ҳ��Ҫд��Flash�ĳ���

        if (tmpLen > len)
        {
            tmpLen = len;
            flag = 0; // ����ҳ
        }
        else
        {
            flag = 1; // ��ҳ
        }

        /** ����һҳ���� */
        for (i = 0; i < FLASH_PAGE_SIZE; i++)
        {
            FLASH_PAGE_BUF[i] = ((uint8_t*)pageAddr)[i];
        }

        /** ����Ҫд��Flash�����ݱ��浽����buf�� */
        for (i = 0; i < tmpLen; i++)
        {
            FLASH_PAGE_BUF[tmpAddr + i] = ((uint8_t*)src)[i];
        }

        /** дFlashǰ�Ȳ��� */
        if (FLASH_EOP != FLASH_One_Page_Erase(pageAddr))
        {
            FLASH_Lock();
            return FLASH_ERR_WRP;
        }

        /** д��ҳFlash */
        if (FLASH_EOP != FLASH_ProgramPage((uint32_t*)FLASH_PAGE_BUF, pageAddr, FLASH_PAGE_SIZE))
        {
            FLASH_Lock();
            return FLASH_ERR_WRP;
        }

        /** ����ʣ��дFlash��ҳ�� */
        pageCount = ((len - tmpLen) / FLASH_PAGE_SIZE);
        pageAddr += FLASH_PAGE_SIZE;
        for (i = 0; i < pageCount; i++)
        {
            /** дFlashǰ�Ȳ��� */
            if (FLASH_EOP != FLASH_One_Page_Erase(pageAddr))
            {
                FLASH_Lock();

                return FLASH_ERR_WRP;
            }

            /** ��ҳдFlash */
            if (FLASH_EOP != FLASH_ProgramPage((uint32_t*)(src + tmpLen + (i * FLASH_PAGE_SIZE)), pageAddr, FLASH_PAGE_SIZE))
            {
                FLASH_Lock();

                return FLASH_ERR_WRP;
            }
            pageAddr += FLASH_PAGE_SIZE;
        }

        /** ������ַ��ҳ */
        if ((endAddr != 0) && (flag == 1))
        {
            /** ������ַҳ�����룬��Ҫ�������һҳ���� */
            for (i = 0; i < FLASH_PAGE_SIZE; i++)
            {
                FLASH_PAGE_BUF[i] = ((uint8_t*)pageAddr)[i];
            }

            /** ���㰴ҳ��������ַ */
            tmpAddr = (len - tmpLen) % FLASH_PAGE_SIZE;

            /** ����Ҫд��Flash�����ݱ��浽buf�� */
            for (i = 0; i < tmpAddr; i++)
            {
                FLASH_PAGE_BUF[i] = ((uint8_t*)(src + tmpLen + (pageCount * FLASH_PAGE_SIZE)))[i];
            }

            /** дFlashǰ�Ȳ��� */
            if (FLASH_EOP != FLASH_One_Page_Erase(pageAddr))
            {
                FLASH_Lock();
                return FLASH_ERR_WRP;
            }

            /** д���һҳ */
            if (FLASH_EOP != FLASH_ProgramPage((uint32_t*)FLASH_PAGE_BUF, pageAddr, FLASH_PAGE_SIZE))
            {
                FLASH_Lock();
                return FLASH_ERR_WRP;
            }
        }
    }

    FLASH_Lock();
    return FLASH_EOP;
}


/**
 * @brief �����ַ��ȡ���ⳤ������
 * @param
 *			ReadAddr ��Ҫ��ȡ����ʼ��ַ
 *      pBuffer  ���洢��ȡ����buf����ʼ��ַ
 *			NumToRead��Ҫ��ȡ�����ݳ���(byte)
 *
 */
void MID_FLASH_E2P_Read(u32 ReadAddr, u8* pBuffer, u32 NumToRead)
{
    u32 i;
    for (i = 0; i < NumToRead; i++)
    {
        *((u8*)pBuffer + i) = *((u8*)ReadAddr + i);
    }
}

void Jump_To_App(uint32_t address)
{
    /* Judge whether the top of stack address is legal or not */
    if (((*(__IO uint32_t*)address) & 0x2FFE0000) == 0x20000000)
    {
        /* Jump to user application */
        JumpAddress = *(__IO uint32_t*) (address + 4);
        Jump_To_Application = (pFunction) JumpAddress;

        /* Initialize user application's Stack Pointer */
        //__set_MSP(*(__IO uint32_t*) address);

        register uint32_t __regMainStackPointer     __ASM("msp");
        __regMainStackPointer = *(__IO uint32_t*) address;

        Jump_To_Application();
    }
}











