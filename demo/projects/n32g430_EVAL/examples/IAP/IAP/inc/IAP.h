#ifndef _IAP_H__
#define _IAP_H__

#include "n32g430.h"



void Jump_To_App(uint32_t address);
FLASH_STS MID_FLASH_E2P_Write(u32 addr, const u8* src, u32 len);
void MID_FLASH_E2P_Read(u32 ReadAddr, u8* pBuffer, u32 NumToRead);
#endif
