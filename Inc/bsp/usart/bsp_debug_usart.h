#ifndef __BSP_DEBUG_USART_H__
#define __BSP_DEBUG_USART_H__

/* ～邦o?赤﹞???t ----------------------------------------------------------------*/
#include "stm32f4xx_hal.h"
#include <stdio.h>

/* 角角D赤?“辰? ------------------------------------------------------------------*/
/* o那?“辰? --------------------------------------------------------------------*/
#define DEBUG_USARTx                                 USART1
#define DEBUG_USARTx_BAUDRATE                        115200
#define DEBUG_USART_RCC_CLK_ENABLE()                 __HAL_RCC_USART1_CLK_ENABLE()
#define DEBUG_USART_RCC_CLK_DISABLE()                __HAL_RCC_USART1_CLK_DISABLE()

#define DEBUG_USARTx_GPIO_ClK_ENABLE()               __HAL_RCC_GPIOB_CLK_ENABLE()
#define DEBUG_USARTx_Tx_GPIO_PIN                     GPIO_PIN_6
#define DEBUG_USARTx_Tx_GPIO                         GPIOB
#define DEBUG_USARTx_Rx_GPIO_PIN                     GPIO_PIN_7
#define DEBUG_USARTx_Rx_GPIO                         GPIOB

#define DEBUG_USARTx_AFx                             GPIO_AF7_USART1

#define DEBUG_USART_IRQn                             USART1_IRQn
#define DEBUG_USARTx_IRQHANDLER                      USART1_IRQHandler


#define USARTx                                 UART5
#define USARTx_BAUDRATE                        115200
#define USART_RCC_CLK_ENABLE()                 __HAL_RCC_UART5_CLK_ENABLE()
#define USART_RCC_CLK_DISABLE()                __HAL_RCC_UART5_CLK_DISABLE()

#define USARTx_GPIO_ClK_ENABLE()               {__HAL_RCC_GPIOC_CLK_ENABLE();__HAL_RCC_GPIOD_CLK_ENABLE();}
#define USARTx_Tx_GPIO_PIN                     GPIO_PIN_12
#define USARTx_Tx_GPIO                         GPIOC
#define USARTx_Rx_GPIO_PIN                     GPIO_PIN_2
#define USARTx_Rx_GPIO                         GPIOD

#define USARTx_AFx                             GPIO_AF8_UART5


#define USARTx_IRQHANDLER                      UART5_IRQHandler
#define USARTx_IRQn                            UART5_IRQn

extern UART_HandleTypeDef husart_debug;
extern UART_HandleTypeDef husartx;

void MX_DEBUG_USART_Init(void);
void MX_USARTx_Init(void);

#endif  /* __BSP_DEBUG_USART_H__ */

/******************* (C) COPYRIGHT 2015-2020 車2那‘??豕?那??a﹞⊿赤??車 *****END OF FILE****/
