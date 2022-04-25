/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
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
 /**
  * 1、串口中断接收总是溢出，清除溢出标志要先读SR在读DR，官方提供的clear方法已经完成了这两个动作；
  * 2、换用DMA接收目前没有遇到错误，也不会卡死，后续继续观察。使用hal库dma除了需要开启dma中断，
  *     还要开启串口中断，hal库会打开串口的错误中断，
  * 3、通信过程不要使用0，因为0作为字符串的结尾标志可能会造成麻烦。
  * 4、操作字符串切记最后有个\0结尾，在分配空间的时候不要忘记给它留个位置，否则会覆盖后边的代码，
  *     导致未知的错误发生。也就是说最少要分配[最大有用字符+1]个字节。
  * 
  * 
  * 
  * 00：保留   
  * 01：从机id      回传'f' for failed
  * 02：操作舵机    回传's' for successed
  * 
 */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/

#include "main.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"
#include "i2c.h"
#include "dma.h"
#include <stdio.h>
#include <string.h>

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */


#define SET_SLAVE_ID 0x01
#define CTRL_SERVO   0x02
/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */
void enable_servo(uint8_t en);
/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
uint8_t recv_ok=1;
uint8_t slave_id=0;
uint8_t trans_lens = 0;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_DMA_Init();
  MX_USART1_UART_Init();
  MX_USART2_UART_Init();
  MX_I2C1_Init();
  /* USER CODE BEGIN 2 */

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
 // enable_servo(1);
  HAL_UART_Receive_DMA(&huart1,uart_rx_buff,6);
  while (1)
  {
    /* USER CODE END WHILE */
    // // HAL_GPIO_TogglePin(GPIOA,GPIO_PIN_1);
    // printf("i2c:%x %x %x %x %x %x\n",i2cTxData[0],i2cTxData[1],i2cTxData[2],i2cTxData[3],i2cTxData[4],i2cTxData[5]);
    // printf("uart:%x %x %x %x %x %x\n",uart_tx_buff[0],uart_tx_buff[1],uart_tx_buff[2],uart_tx_buff[3],uart_tx_buff[4],uart_tx_buff[5]);
    // printf("strlen %d\n",trans_lens);
    //  HAL_Delay(1000);
    //HAL_PWR_EnterSLEEPMode(PWR_LOWPOWERREGULATOR_ON,PWR_SLEEPENTRY_WFI);
    // HAL_PWR_EnterSTOPMode(PWR_MAINREGULATOR_ON,PWR_SLEEPENTRY_WFE);
    /* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */

#if defined (STM32F103xE)
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};
  RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL9;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }
  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_USB;
  PeriphClkInit.UsbClockSelection = RCC_USBCLKSOURCE_PLL_DIV1_5;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
  {
    Error_Handler();
  }
}
#elif defined(STM32F446xE)
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Configure the main internal regulator output voltage
  */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);
  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLM = 8;
  RCC_OscInitStruct.PLL.PLLN = 180;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 2;
  RCC_OscInitStruct.PLL.PLLR = 2;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
  /** Activate the Over-Drive mode
  */
  if (HAL_PWREx_EnableOverDrive() != HAL_OK)
  {
    Error_Handler();
  }
  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_5) != HAL_OK)
  {
    Error_Handler();
  }
}
#endif 

HAL_StatusTypeDef TransmitAndReceiveI2cPacket(uint8_t _id)
{
    HAL_StatusTypeDef state = HAL_ERROR;
    //printf("id:%d,put %f\n",_id,*((float*) (i2cTxData + 1)));

    state = HAL_I2C_Master_Transmit(&hi2c1, _id, i2cTxData, 5, 1000);      
    if(state != HAL_OK)
        return state;   
 
    state = HAL_I2C_Master_Receive(&hi2c1, _id, i2cRxData, 5, 1000);
    //printf("id:%d,get %f\n",_id,*((float*) (i2cRxData + 1)));
    return state;
    
}

void enable_servo(uint8_t en)
{
    i2cTxData[0] = 0xff;
    i2cTxData[1] = en;
    //printf("put %x %x %x %x %x\n",i2cTxData[0],i2cTxData[1],i2cTxData[2],i2cTxData[3],i2cTxData[4]);
    TransmitAndReceiveI2cPacket(slave_id);
   // printf("get %x %x %x %x %x\n",i2cRxData[0],i2cRxData[1],i2cRxData[2],i2cRxData[3],i2cRxData[4]);
}

/* USER CODE BEGIN 4 */
void HAL_UART_RxCpltCallback(UART_HandleTypeDef * huart)
{
    if(huart->Instance==USART1)
    {
        recv_ok =1;
        switch(uart_rx_buff[0])
        {
            case SET_SLAVE_ID:
                slave_id = uart_rx_buff[1];
                uart_tx_buff[0]=SET_SLAVE_ID;
                uart_tx_buff[1]='s';
                trans_lens = 2;
            break;                
            case CTRL_SERVO:
                switch (uart_rx_buff[1])
                {
                case 0x01:  //设置角度
                break;
                case 0x02:  //设置速度
                break;
                case 0x03:  //设置转矩
                break;
                case 0x11:  //读取角度
                break;
                case 0x12:  //读取速度
                break;
                case 0x21:  //设置id
                break;
                case 0x22:  //设置kp
                break;
                case 0x23:  //设置ki
                break;
                case 0x24:  //设置kv
                break;
                case 0x25:  //设置kd
                break;
                case 0x26:  //设置转矩并保存
                break;
                case 0x27:  //初始角度
                break;
                case 0xff:  //使能/失能
                break;        
                default:
                    recv_ok =0;
                    break;
                }  
                memcpy(i2cTxData,uart_rx_buff+1,5) ;
                uart_tx_buff[0]=CTRL_SERVO;    
                uart_tx_buff[1]='f';    
                               
                if(recv_ok&&(TransmitAndReceiveI2cPacket(slave_id)==HAL_OK))
                {                    
                    uart_tx_buff[1]='s';                    
                    if(uart_rx_buff[1]==0x11)
                    {                   
                        float valF = *((float*) (i2cRxData + 1));
                        sprintf((char*)(uart_tx_buff+2),"%.2f",valF);   //0.00-180.00理论上最多6位，但实际上还要加一个\0结尾。
                        trans_lens = strlen((char *)uart_tx_buff);
                    }
                    else
                    {
                        trans_lens = 2;
                    }
                }
                else{
                    trans_lens = 2;
                }                
            break;
            default:
                uart_tx_buff[0]=uart_rx_buff[0];
                uart_tx_buff[1]='e';
                trans_lens = 2;
            break;
        }
        HAL_UART_Transmit_DMA(&huart1,uart_tx_buff,trans_lens);
    }
    
}
uint8_t err=0;
void HAL_UART_ErrorCallback(UART_HandleTypeDef *huart)
{
    switch(HAL_UART_GetError(huart))
    {
        case HAL_UART_ERROR_PE:
         __HAL_UART_CLEAR_PEFLAG(huart);
         err =1;
        break;
        case HAL_UART_ERROR_NE:
         __HAL_UART_CLEAR_NEFLAG(huart);
          err =2;
        break;
        case HAL_UART_ERROR_FE:
         __HAL_UART_CLEAR_FEFLAG(huart);
    
          err =3;
        break;
        case HAL_UART_ERROR_ORE:
         __HAL_UART_CLEAR_OREFLAG(huart);
          err =4;
        break;
    }   
    HAL_GPIO_TogglePin(GPIOA,GPIO_PIN_1); 
    HAL_UART_Receive_DMA(huart,uart_rx_buff,6);    
}
/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
