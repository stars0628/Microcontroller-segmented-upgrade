/*****************************************************************************
 * Copyright (c) 2019, Nations Technologies Inc.
 *
 * All rights reserved.
 * ****************************************************************************
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * - Redistributions of source code must retain the above copyright notice,
 * this list of conditions and the disclaimer below.
 *
 * Nations' name may not be used to endorse or promote products derived from
 * this software without specific prior written permission.
 *
 * DISCLAIMER: THIS SOFTWARE IS PROVIDED BY NATIONS "AS IS" AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT ARE
 * DISCLAIMED. IN NO EVENT SHALL NATIONS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA,
 * OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 * ****************************************************************************/


/**
*\*\file main.c
*\*\author Nations
*\*\version v1.0.0
*\*\copyright Copyright (c) 2019, Nations Technologies Inc. All rights reserved.
**/

#include "main.h"


/**
*\*\name    main.
*\*\fun     Main program.
*\*\param   none
*\*\return  none
**/

uint8_t IAP_Flag[2];//获取是否需要升级标志

int main(void)
{
    MID_FLASH_E2P_Read(0x0800fffe, IAP_Flag, 2);//标志写在最后0x0800fffe，16字节
    CommUsart_Init();

    while (1) {
        //if(1){
        if (IAP_Flag[0] == 0x4B && IAP_Flag[1] == 0x4F) {//不需升级
            Jump_To_App(FLASH_APP1_ADDR);
        } else {
            if (iap_flag.execute_app_done) {
                iap_flag.execute_app_done = 0;
                if (((*(vu32 *)(FLASH_APP1_ADDR + 4)) & 0xFF000000) == 0x08000000) {
                    uint8_t write_flash[2] = "KO"; // 不需升级标志
                    MID_FLASH_E2P_Write(0x0800FFFE, write_flash, 2);
                    Jump_To_App(FLASH_APP1_ADDR); // 跳转到APP起始地址，期间不能被其他中断打断，否则会跳转失败
                } else {
                    ;
                }
            }
        }
    }
}



#ifdef USE_FULL_ASSERT


/**
*\*\name    assert_failed.
*\*\fun     Reports the name of the source file and the source line number
*\*\        where the assert_param error has occurred.
*\*\param   file pointer to the source file name
*\*\param   line assert_param error line source number
*\*\return  none
**/
void assert_failed(const uint8_t* expr, const uint8_t* file, uint32_t line)
{
    /* User can add his own implementation to report the file name and line number,
       ex: ////printf("Wrong parameters value: file %s on line %d\r\n", file, line) */

    /* Infinite loop */
    while (1)
    {
    }
}

#endif


