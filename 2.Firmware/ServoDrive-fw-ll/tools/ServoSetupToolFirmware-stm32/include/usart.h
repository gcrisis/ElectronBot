/**
  ******************************************************************************
  * @file    usart.h
  * @brief   This file contains all the function prototypes for
  *          the usart.c file
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2022 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __USART_H__
#define __USART_H__

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* USER CODE BEGIN Includes */
#define UART_TXBUFF_SIZE 16
#define UART_RXBUFF_SIZE 8

#if defined (STM32F103xE)
#define USART1_TX_DMA_CHANNEL             DMA1_Channel4
#define USART1_RX_DMA_CHANNEL             DMA1_Channel5
#endif
#if defined (STM32F446xE)
#define USART1_TX_DMA_CHANNEL             DMA2_Stream7
#define USART1_RX_DMA_CHANNEL             DMA2_Stream2
#endif
/* USER CODE END Includes */

extern UART_HandleTypeDef huart1;
extern UART_HandleTypeDef huart2;
extern uint8_t uart_tx_buff[UART_TXBUFF_SIZE];
extern uint8_t uart_rx_buff[UART_RXBUFF_SIZE];
/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

void MX_USART1_UART_Init(void);
void MX_USART2_UART_Init(void);

/* USER CODE BEGIN Prototypes */

/* USER CODE END Prototypes */

#ifdef __cplusplus
}
#endif

#endif /* __USART_H__ */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
