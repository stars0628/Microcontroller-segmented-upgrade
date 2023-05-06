#include "CommUART1.h"
#include "IAP.h"
#include "main.h"

// uint8_t U1_RxBuffer[TxRxCounterleng] = {0};
// uint8_t U1_TxBuffer[100] = {0};
uint8_t U1_RxBuffer[TxRxCounterleng]; // 接收缓存
uint8_t U1_TxBuffer[100];             // 发送缓存
uint16_t RxCounter1 = 0;              /// 接收数据字节数
uint8_t rxflag = 0;                   ////接收完成标志

void UART1_GPIO_Configuration(void)
{
    GPIO_InitType GPIO_InitStructure;
    /* Enable GPIO clock */
    RCC_AHB_Peripheral_Clock_Enable(USARTy_GPIO_CLK);
    RCC_APB2_Peripheral_Clock_Enable(RCC_APB2_PERIPH_AFIO);
    /* Enable USARTy and USARTz Clock */

    /* Initialize GPIO_InitStructure */
    GPIO_Structure_Initialize(&GPIO_InitStructure);
    /* Configure USARTy Tx as alternate function push-pull */
    GPIO_InitStructure.Pin = USARTy_TxPin;
    GPIO_InitStructure.GPIO_Mode = GPIO_MODE_AF_PP;
    GPIO_InitStructure.GPIO_Pull = GPIO_PULL_UP;
    // GPIO_InitStructure.GPIO_Slew_Rate = GPIO_SLEW_RATE_SLOW;
    GPIO_InitStructure.GPIO_Alternate = USARTy_Tx_GPIO_AF;
    GPIO_Peripheral_Initialize(USARTy_GPIO, &GPIO_InitStructure);

    /* Configure USARTx Rx as alternate function push-pull */
    GPIO_InitStructure.Pin = USARTy_RxPin;
    GPIO_InitStructure.GPIO_Pull = GPIO_PULL_UP;
    GPIO_InitStructure.GPIO_Alternate = USARTy_Rx_GPIO_AF;
    GPIO_Peripheral_Initialize(USARTy_GPIO, &GPIO_InitStructure);
}

/***\*\name    DMA_Configuration.*\*\fun
Configures the DMA.*\*\param   none*\*\return  none
**/
void UART1_DMA_Configuration(void)
{
    DMA_InitType DMA_InitStructure;
    NVIC_InitType NVIC_InitStructure;
    USART_InitType USART_InitStructure;

    RCC_AHB_Peripheral_Clock_Enable(RCC_AHB_PERIPH_DMA);
    USARTy_APBxClkCmd(USARTy_CLK);

#if 1
    /* Enable the USARTy Interrupt */
    //	 NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
    NVIC_Priority_Group_Set(NVIC_PER0_SUB4_PRIORITYGROUP);
    NVIC_InitStructure.NVIC_IRQChannel = USARTy_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelCmd = (FunctionalState)ENABLE;

    NVIC_Initializes(&NVIC_InitStructure);
#endif
    /* DMA clock enable */

    /* USARTy_Tx_DMA_Channel (triggered by USARTy Tx event) Config */
    DMA_Reset(USARTy_Tx_DMA_Channel);
    DMA_InitStructure.PeriphAddr = USARTy_DAT_Base;
    DMA_InitStructure.MemAddr = (uint32_t)U1_TxBuffer;
    DMA_InitStructure.Direction = DMA_DIR_PERIPH_DST;
    DMA_InitStructure.BufSize = 100;
    DMA_InitStructure.PeriphInc = DMA_PERIPH_INC_MODE_DISABLE;
    DMA_InitStructure.MemoryInc = DMA_MEM_INC_MODE_ENABLE;
    DMA_InitStructure.PeriphDataSize = DMA_PERIPH_DATA_WIDTH_BYTE;
    DMA_InitStructure.MemDataSize = DMA_MEM_DATA_WIDTH_BYTE;
    DMA_InitStructure.CircularMode = DMA_CIRCULAR_MODE_DISABLE;
    DMA_InitStructure.Priority = DMA_CH_PRIORITY_HIGHEST;
    DMA_InitStructure.Mem2Mem = DMA_MEM2MEM_DISABLE;
    DMA_Initializes(USARTy_Tx_DMA_Channel, &DMA_InitStructure);
    DMA_Channel_Request_Remap(USARTy_Tx_DMA_Channel, USARTy_Tx_DMA_REMAP);

    /* USARTy RX DMA1 Channel (triggered by USARTy Rx event) Config */
    DMA_Reset(USARTy_Rx_DMA_Channel);
    DMA_InitStructure.PeriphAddr = USARTy_DAT_Base;
    DMA_InitStructure.MemAddr = (uint32_t)U1_RxBuffer;
    DMA_InitStructure.Direction = DMA_DIR_PERIPH_SRC;
    DMA_InitStructure.CircularMode = DMA_CIRCULAR_MODE_DISABLE;
    DMA_InitStructure.BufSize = TxRxCounterleng;
    // DMA_InitStructure.PeriphInc      = DMA_PERIPH_INC_MODE_DISABLE;
    // DMA_InitStructure.MemoryInc      = DMA_MEM_INC_MODE_ENABLE;
    // DMA_InitStructure.PeriphDataSize = DMA_PERIPH_DATA_WIDTH_BYTE;
    // DMA_InitStructure.MemDataSize    = DMA_MEM_DATA_WIDTH_BYTE;
    // DMA_InitStructure.CircularMode   = DMA_CIRCULAR_MODE_DISABLE;
    // DMA_InitStructure.Priority       = DMA_CH_PRIORITY_HIGHEST;
    // DMA_InitStructure.Mem2Mem        = DMA_MEM2MEM_DISABLE;
    DMA_Initializes(USARTy_Rx_DMA_Channel, &DMA_InitStructure);
    DMA_Channel_Request_Remap(USARTy_Rx_DMA_Channel, USARTy_Rx_DMA_REMAP);

    /* USARTy and USARTz configuration ------------------------------------------------------*/
    USART_InitStructure.BaudRate = USART1TXRXBaudRate;
    USART_InitStructure.WordLength = USART_WL_8B;
    USART_InitStructure.StopBits = USART_STPB_1;
    USART_InitStructure.Parity = USART_PE_NO;
    USART_InitStructure.HardwareFlowControl = USART_HFCTRL_NONE;
    USART_InitStructure.Mode = USART_MODE_RX | USART_MODE_TX;
    /* Configure USARTy and USARTz */
    USART_Initializes(USARTy, &USART_InitStructure);

    /* Enable USARTy DMA Rx  request */
    USART_DMA_Transfer_Enable(USARTy, USART_DMAREQ_TX);
    USART_DMA_Transfer_Enable(USARTy, USART_DMAREQ_RX);
    /* Enable USARTy TX DMA1 Channel */
    // DMA_Channel_Enable(USARTy_Tx_DMA_Channel);
    /* Enable USARTy RX DMA1 Channel */
    DMA_Channel_Enable(USARTy_Rx_DMA_Channel);

#if 1
    /* Enable USARTy Receive and Transmit interrupts */
    USART_Interrput_Enable(USARTy, USART_INT_RXDNE);
    USART_Interrput_Enable(USARTy, USART_INT_IDLEF);
#endif

    /* Enable the USARTy and USARTz */
    USART_Enable(USARTy);

    // 清除标志位，避免第一次传输出错
    DMA_Flag_Status_Clear(DMA, USARTy_Tx_DMA_FLAG);
    DMA_Flag_Status_Clear(DMA, USARTy_Rx_DMA_FLAG);
    DMA_Interrupt_Status_Clear(DMA, USARTy_Tx_DMA_Interrupts_Flag);
    DMA_Interrupt_Status_Clear(DMA, USARTy_Rx_DMA_Interrupts_Flag);
}

void DMA_send(uint16_t BufferLength)
{
    ;

    // DMA_EnableChannel(DMA_CH2, DISABLE);
    // DMA_Channel_Enable(USARTy_Tx_DMA_Channel, DISABLE);
    DMA_Channel_Disable(USARTy_Tx_DMA_Channel);
    DMA_Current_Data_Transfer_Number_Set(USARTy_Tx_DMA_Channel, BufferLength);
    DMA_Flag_Status_Clear(DMA, USARTy_Tx_DMA_FLAG);
    // DMA_Channel_Enable(USARTy_Tx_DMA_Channel, ENABLE);
    DMA_Channel_Enable(USARTy_Tx_DMA_Channel);
    while (USART_Flag_Status_Get(USARTy, USART_FLAG_TXDE) == RESET) {
    }
}

void DMA_Revice(void)
{
    DMA_Channel_Disable(USARTy_Rx_DMA_Channel);
    RxCounter1 = (TxRxCounterleng - DMA_Current_Data_Transfer_Number_Get(USARTy_Rx_DMA_Channel));
    //    if (0 != RxCounter1) {
    //        U1_RxBuffer[RxCounter1] = 0;
    //        rxflag = 1;
    //    }
    cmd_or_data_deal(U1_RxBuffer,RxCounter1);

    DMA_Current_Data_Transfer_Number_Set(USARTy_Rx_DMA_Channel, TxRxCounterleng);
    DMA_Interrupt_Status_Clear(DMA, USARTy_Tx_DMA_Interrupts_Flag);
    DMA_Channel_Enable(USARTy_Rx_DMA_Channel);
    USART_Flag_Clear(USARTy, USART_FLAG_IDLEF);
}

/** * @brief  This function handles USARTy global interrupt request. */
void USARTy_IRQHandler(void)
{
#if 0
    u8 ch;
    if((USARTy->STS & USART_STS_RXDNE) != RESET) //不为空
    {
        USARTy->STS &= ~USART_STS_RXDNE;
        ch=(u8)USART_Data_Receive(USARTy);
        // if(!(USARTy->CR1 & USART_CR1_IDLEIE)) {
        //  USART_Interrput_Enable(USARTy, USART_INT_IDLEF);//当没有开启IDLE中断的时候再开，不然上电会进去一次
        //  USART_Flag_Clear(USARTy, USART_FLAG_IDLEF);
        // }

    }
#endif
    if (USART_Interrupt_Status_Get(USARTy, USART_INT_IDLEF) != RESET) {

        DMA_Revice();

        (void)USARTy->STS;
        (void)USARTy->DAT;
        // USARTy->CTRL1  |=  USART_CTRL1_RXDNEIEN;
    }
}

void CommUsart_Init(void)
{

    UART1_GPIO_Configuration();
    UART1_DMA_Configuration();
}
