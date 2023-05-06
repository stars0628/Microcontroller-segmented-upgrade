
#ifndef __COMMUART1_H__
#define __COMMUART1_H__

#ifdef __cplusplus
extern "C" {
#endif
	
#include "n32g430.h"
#include "main.h"

#define TxRxCounterleng  1024*5




#define USARTy                  USART1
#define USARTy_CLK              RCC_APB2_PERIPH_USART1
#define USARTy_GPIO             GPIOA
#define USARTy_GPIO_CLK         RCC_AHB_PERIPH_GPIOA
#define USARTy_RxPin            GPIO_PIN_10
#define USARTy_TxPin            GPIO_PIN_9
#define USARTy_Rx_GPIO_AF       GPIO_AF5_USART1
#define USARTy_Tx_GPIO_AF       GPIO_AF5_USART1
#define USARTy_APBxClkCmd       RCC_APB2_Peripheral_Clock_Enable
#define USARTy_DAT_Base         (USART1_BASE + 0x04)

#define USARTy_Tx_DMA_Channel   DMA_CH1
#define USARTy_Tx_DMA_FLAG      DMA_CH1_TXCF
#define USARTy_Tx_DMA_REMAP     DMA_REMAP_USART1_TX
#define USARTy_Tx_DMA_Interrupts_Flag      DMA_CH1_INT_TXC

#define USARTy_Rx_DMA_Channel   DMA_CH2
#define USARTy_Rx_DMA_FLAG      DMA_CH2_TXCF
#define USARTy_Rx_DMA_REMAP     DMA_REMAP_USART1_RX
#define USARTy_Rx_DMA_Interrupts_Flag      DMA_CH2_INT_TXC

//#define USARTy_Tx_DMA_Channel   DMA_CH4
//#define USARTy_Tx_DMA_FLAG      DMA_CH4_TXCF
//#define USARTy_Tx_DMA_REMAP     DMA_REMAP_USART1_TX
#define USARTy_IRQn             USART1_IRQn
#define USARTy_IRQHandler       USART1_IRQHandler

#define USART1TXRXBaudRate           9600


/////接收发送数据供数据处理文件使用
extern uint8_t U1_RxBuffer[TxRxCounterleng];
extern uint8_t U1_TxBuffer[100];
extern uint16_t RxCounter1; ///接收数据字节数
extern uint8_t rxflag;     ////接收完成标志


void CommUsart_Init(void);
void DMA_send(uint16_t BufferLength);


#endif


























