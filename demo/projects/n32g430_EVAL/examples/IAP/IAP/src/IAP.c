#include "IAP.h"

/** 页大小 */
#define FLASH_PAGE_SIZE 2048

typedef void (*pFunction)(void);
pFunction Jump_To_Application;
uint32_t JumpAddress;


/** page缓存 */
static u8 FLASH_PAGE_BUF[FLASH_PAGE_SIZE];


/**
 * @brief Flash编程, 内部支持函数
 * @param
 *			SrcAdd ：要写入的数据的起始地址
 *      DstAddr：要写入数据的Falsh地址
 *			Len		 ：操作的数据长度
 *
 * @return
 *			FLASH_ERR_PG：操作失败
 *			FLASH_COMPL ：操作成功
 */
static FLASH_STS FLASH_ProgramPage(uint32_t* SrcAddr, uint32_t DstAddr, uint16_t Len)
{
    uint32_t i;

    for (i = 0; i < Len; i += 4)
    {
        FLASH_Word_Program(DstAddr + i, *(uint32_t*)((uint32_t)SrcAddr + i));
        if (*(uint32_t*)(DstAddr + i) != *(uint32_t*)((uint32_t)SrcAddr + i))
        {
            return FLASH_ERR_PG;	// 读出校验
        }
    }

    return FLASH_EOP;
}


/**
 * @brief 任意地址写入任意长度数据
 * @param
 *			addr：起始地址
 *      src ：写入的数据指针
 *			len ：写入数据长度(byte)
 *
 * @return
 *			FLASH_ERR_WRP：操作失败
 *			FLASH_COMPL  ：操作成功
 */
FLASH_STS MID_FLASH_E2P_Write(u32 addr, const u8* src, u32 len)
{
    uint32_t i;
    uint32_t pageAddr;  //按页对齐的地址
    uint32_t pageCount; //写Falsh的页数
    uint32_t tmpAddr;   //按页对齐的余地址
    uint32_t tmpLen;    //首页需要写入Flash的长度
    uint32_t startAddr; //起始地址是否页对齐
    uint32_t endAddr;   //结束地址是否页对齐
    uint32_t flag = 0;  //页不对齐时,是否跨页

//	  if(FLASH_HSICLOCK_DISABLE == FLASH_ClockInit())
//    {
//        printf("HSI oscillator not yet ready\r\n");
//        while(1);
//    }

    FLASH_Unlock();
    startAddr = (addr % FLASH_PAGE_SIZE);					// 获取起始页偏移地址
    endAddr   = ((addr + len) % FLASH_PAGE_SIZE);	// 获取结束页偏移地址

    /** 起始地址页对齐 */
    if (startAddr == 0)
    {
        pageAddr = addr;
        pageCount = (len / FLASH_PAGE_SIZE);
        for (i = 0; i < pageCount; i++)
        {
            /** 写Flash前先擦除, 不需要备份 */
            if (FLASH_EOP != FLASH_One_Page_Erase(pageAddr))
            {
                FLASH_Lock();
                return FLASH_ERR_WRP;
            }

            /** 按页写Flash */
            if (FLASH_EOP != FLASH_ProgramPage((uint32_t*)(src + (i * FLASH_PAGE_SIZE)), pageAddr, FLASH_PAGE_SIZE))
            {
                FLASH_Lock();
                return FLASH_ERR_WRP;
            }
            pageAddr += FLASH_PAGE_SIZE;
        }

        /** 结束地址页不对齐 */
        if (endAddr != 0)
        {
            /** 备份一页数据 */
            for (i = 0; i < FLASH_PAGE_SIZE; i++)
            {
                FLASH_PAGE_BUF[i] = ((uint8_t*)pageAddr)[i];
            }

            /** 计算按页对齐的余地址 */
            tmpAddr = (len % FLASH_PAGE_SIZE);

            /** 把需要写入Flash的数据保存到备份buf中 */
            for (i = 0; i < tmpAddr; i++)
            {
                FLASH_PAGE_BUF[i] = ((uint8_t*)(src + (pageCount * FLASH_PAGE_SIZE)))[i];
            }

            /** 写Flash前先擦除 */
            if (FLASH_EOP != FLASH_One_Page_Erase(pageAddr))
            {
                FLASH_Lock();
                return FLASH_ERR_WRP;
            }

            /** 写最后一页 */
            if (FLASH_EOP != FLASH_ProgramPage((uint32_t*)FLASH_PAGE_BUF, pageAddr, FLASH_PAGE_SIZE))
            {
                FLASH_Lock();
                return FLASH_ERR_WRP;
            }
        }
    }
    else /** 起始地址页不对齐 */
    {
        pageAddr = ((addr / FLASH_PAGE_SIZE) * FLASH_PAGE_SIZE);	//计算首页的页对齐地址
        tmpAddr  = (addr % FLASH_PAGE_SIZE);											//计算首页的余地址
        tmpLen   = (FLASH_PAGE_SIZE - tmpAddr);										//计算首页需要写入Flash的长度

        if (tmpLen > len)
        {
            tmpLen = len;
            flag = 0; // 不跨页
        }
        else
        {
            flag = 1; // 跨页
        }

        /** 备份一页数据 */
        for (i = 0; i < FLASH_PAGE_SIZE; i++)
        {
            FLASH_PAGE_BUF[i] = ((uint8_t*)pageAddr)[i];
        }

        /** 把需要写入Flash的数据保存到备份buf中 */
        for (i = 0; i < tmpLen; i++)
        {
            FLASH_PAGE_BUF[tmpAddr + i] = ((uint8_t*)src)[i];
        }

        /** 写Flash前先擦除 */
        if (FLASH_EOP != FLASH_One_Page_Erase(pageAddr))
        {
            FLASH_Lock();
            return FLASH_ERR_WRP;
        }

        /** 写首页Flash */
        if (FLASH_EOP != FLASH_ProgramPage((uint32_t*)FLASH_PAGE_BUF, pageAddr, FLASH_PAGE_SIZE))
        {
            FLASH_Lock();
            return FLASH_ERR_WRP;
        }

        /** 计算剩余写Flash的页数 */
        pageCount = ((len - tmpLen) / FLASH_PAGE_SIZE);
        pageAddr += FLASH_PAGE_SIZE;
        for (i = 0; i < pageCount; i++)
        {
            /** 写Flash前先擦除 */
            if (FLASH_EOP != FLASH_One_Page_Erase(pageAddr))
            {
                FLASH_Lock();

                return FLASH_ERR_WRP;
            }

            /** 按页写Flash */
            if (FLASH_EOP != FLASH_ProgramPage((uint32_t*)(src + tmpLen + (i * FLASH_PAGE_SIZE)), pageAddr, FLASH_PAGE_SIZE))
            {
                FLASH_Lock();

                return FLASH_ERR_WRP;
            }
            pageAddr += FLASH_PAGE_SIZE;
        }

        /** 结束地址跨页 */
        if ((endAddr != 0) && (flag == 1))
        {
            /** 结束地址页不对齐，需要备份最后一页数据 */
            for (i = 0; i < FLASH_PAGE_SIZE; i++)
            {
                FLASH_PAGE_BUF[i] = ((uint8_t*)pageAddr)[i];
            }

            /** 计算按页对齐的余地址 */
            tmpAddr = (len - tmpLen) % FLASH_PAGE_SIZE;

            /** 把需要写入Flash的数据保存到buf中 */
            for (i = 0; i < tmpAddr; i++)
            {
                FLASH_PAGE_BUF[i] = ((uint8_t*)(src + tmpLen + (pageCount * FLASH_PAGE_SIZE)))[i];
            }

            /** 写Flash前先擦除 */
            if (FLASH_EOP != FLASH_One_Page_Erase(pageAddr))
            {
                FLASH_Lock();
                return FLASH_ERR_WRP;
            }

            /** 写最后一页 */
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
 * @brief 任意地址读取任意长度数据
 * @param
 *			ReadAddr ：要读取的起始地址
 *      pBuffer  ：存储读取数据buf的起始地址
 *			NumToRead：要读取的数据长度(byte)
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











