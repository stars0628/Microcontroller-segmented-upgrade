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
*\*\file main.h
*\*\author Nations
*\*\version v1.0.0
*\*\copyright Copyright (c) 2019, Nations Technologies Inc. All rights reserved.
**/
#ifndef __MAIN_H__
#define __MAIN_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "n32g430.h"
#include "n32g430_rcc.h"
#include "n32g430_usart.h"
#include <stdio.h>
#include "string.h"


#include "Segmented_IAP.h"
#include "IAP.h"
#include "CommUART1.h"


#define _USART1_UART4_
// #define _USART2_UART3_

#ifdef _USART1_UART4_
#define USARTy            USART1
#define USARTy_CLK        RCC_APB2_PERIPH_USART1
#define USARTy_GPIO       GPIOA
#define USARTy_GPIO_CLK   RCC_AHB_PERIPH_GPIOA
#define USARTy_RxPin      GPIO_PIN_10
#define USARTy_TxPin      GPIO_PIN_9
#define USARTy_Rx_GPIO_AF GPIO_AF5_USART1
#define USARTy_Tx_GPIO_AF GPIO_AF5_USART1
#define USARTy_APBxClkCmd RCC_APB2_Peripheral_Clock_Enable
#define USARTy_IRQn       USART1_IRQn
//#define USARTy_IRQHandler USART1_IRQHandler

#define USARTz            USART2
#define USARTz_CLK        RCC_APB1_PERIPH_USART2
#define USARTz_GPIO       GPIOA
#define USARTz_GPIO_CLK   RCC_AHB_PERIPH_GPIOA
#define USARTz_RxPin      GPIO_PIN_3
#define USARTz_TxPin      GPIO_PIN_2
#define USARTz_Rx_GPIO_AF GPIO_AF5_USART2
#define USARTz_Tx_GPIO_AF GPIO_AF5_USART2
#define USARTz_APBxClkCmd RCC_APB1_Peripheral_Clock_Enable
#define USARTz_IRQn       USART2_IRQn
#define USARTz_IRQHandler USART2_IRQHandler
#endif

	#ifdef _USART2_UART3_
#define USARTy            USART2
#define USARTy_CLK        RCC_APB1_PERIPH_USART2
#define USARTy_GPIO       GPIOA
#define USARTy_GPIO_CLK   RCC_AHB_PERIPH_GPIOA
#define USARTy_RxPin      GPIO_PIN_7
#define USARTy_TxPin      GPIO_PIN_6
#define USARTy_Rx_GPIO_AF GPIO_AF11_USART2
#define USARTy_Tx_GPIO_AF GPIO_AF11_USART2
#define USARTy_APBxClkCmd RCC_APB1_Peripheral_Clock_Enable
#define USARTy_IRQn       USART2_IRQn
#define USARTy_IRQHandler USART2_IRQHandler

#define USARTz            UART3
#define USARTz_GPIO       GPIOB
#define USARTz_CLK        RCC_APB1_PERIPH_USART2
#define USARTz_GPIO_CLK   RCC_AHB_PERIPH_GPIOB
#define USARTz_RxPin      GPIO_PIN_11
#define USARTz_TxPin      GPIO_PIN_10
#define USARTz_Rx_GPIO_AF GPIO_AF10_UART3
#define USARTz_Tx_GPIO_AF GPIO_AF10_UART3
#define USARTz_APBxClkCmd RCC_APB2_Peripheral_Clock_Enable
#define USARTz_IRQn       UART3_IRQn
#define USARTz_IRQHandler UART3_IRQHandler
#endif


#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H__ */
