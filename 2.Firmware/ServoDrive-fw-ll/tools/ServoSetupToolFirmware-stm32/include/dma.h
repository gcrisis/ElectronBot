/**
  ******************************************************************************
  * @file    dma.h
  * @brief   This file contains all the function prototypes for
  *          the dma.c file
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
#ifndef __DMA_H__
#define __DMA_H__

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* DMA memory to memory transfer handles -------------------------------------*/

/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* USER CODE BEGIN Private defines */

#if defined (STM32F103xE)
#define DMA_CLK_ENABLE()                    __HAL_RCC_DMA1_CLK_ENABLE()                                       
#define USART1_DMA_TX_IRQn                DMA1_Channel4_IRQn
#define USART1_DMA_RX_IRQn                DMA1_Channel5_IRQn
#endif

#if defined (STM32F446xE)
#define DMA_CLK_ENABLE()                   __HAL_RCC_DMA2_CLK_ENABLE()
#define USART1_DMA_TX_IRQn                DMA2_Stream7_IRQn
#define USART1_DMA_RX_IRQn                DMA2_Stream2_IRQn
#endif

/* USER CODE END Private defines */

void MX_DMA_Init(void);

/* USER CODE BEGIN Prototypes */

/* USER CODE END Prototypes */

#ifdef __cplusplus
}
#endif

#endif /* __DMA_H__ */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
