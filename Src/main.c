/**
  ******************************************************************************
  * File Name          : main.c
  * Description        : Main program body
  ******************************************************************************
  *
  * COPYRIGHT(c) 2016 STMicroelectronics
  *
  * Redistribution and use in source and binary forms, with or without modification,
  * are permitted provided that the following conditions are met:
  *   1. Redistributions of source code must retain the above copyright notice,
  *      this list of conditions and the following disclaimer.
  *   2. Redistributions in binary form must reproduce the above copyright notice,
  *      this list of conditions and the following disclaimer in the documentation
  *      and/or other materials provided with the distribution.
  *   3. Neither the name of STMicroelectronics nor the names of its contributors
  *      may be used to endorse or promote products derived from this software
  *      without specific prior written permission.
  *
  * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
  * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
  * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
  * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
  * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
  * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
  * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
  * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
  * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
  * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  *
  ******************************************************************************
  */
/* Includes ------------------------------------------------------------------*/
#include "stm32f2xx_hal.h"
#include "cmsis_os.h"
#include "adc.h"
#include "crc.h"
#include "dac.h"
#include "dma.h"
#include "fatfs.h"
#include "iwdg.h"
#include "lwip.h"
#include "rtc.h"
#include "sdio.h"
#include "spi.h"
#include "tim.h"
#include "usart.h"
#include "usb_device.h"
#include "gpio.h"

/* USER CODE BEGIN Includes */
#include "ur11435_lib.h"
#include "modbus_slave.h"
#include "dcon.h"
#include "plc_image.h"
/* USER CODE END Includes */

/* Private variables ---------------------------------------------------------*/
uint8_t __attribute__ ((section(".flash_page_0"))) EEPROM_page_0[FLASH_EEPROM_PAGE_SIZE];
uint8_t __attribute__ ((section(".flash_page_1"))) EEPROM_page_1[FLASH_EEPROM_PAGE_SIZE];
/* USER CODE BEGIN PV */
/* Private variables ---------------------------------------------------------*/
extern xSemaphoreHandle myBinarySemUSBHandle;
extern xSemaphoreHandle myBinarySemCOM1_breakHandle;
extern xSemaphoreHandle myBinarySemCOM3Handle;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
void Error_Handler(void);
void MX_FREERTOS_Init(void);

/* USER CODE BEGIN PFP */
/* Private function prototypes -----------------------------------------------*/

/* USER CODE END PFP */

/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

int main(void)
{

  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration----------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  MX_GPIO_Init();
  led_on(RED);
  HAL_Init();

  /* Configure the system clock */
  SystemClock_Config();

  /* Initialize all configured peripherals */
  MX_CRC_Init();

//  if(RCC_BDCR_RTCEN_BB == DISABLE) MX_RTC_Init();
/*  MX_SDIO_SD_Init();
  MX_SPI1_Init();
  MX_TIM13_Init();
  MX_TIM14_Init();
  MX_USART3_UART_Init();
*/
  /* USER CODE BEGIN 2 */
  /* USER CODE END 2 */

  /* Call init function for freertos objects (in freertos.c) */
  MX_FREERTOS_Init();
  /* Start scheduler */
  led_off();
  osKernelStart();
  
  /* We should never get here as control is now taken by the scheduler */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
  /* USER CODE END WHILE */

  /* USER CODE BEGIN 3 */

  }
  /* USER CODE END 3 */

}

/** System Clock Configuration
*/
void SystemClock_Config(void)
{

  RCC_OscInitTypeDef RCC_OscInitStruct;
  RCC_ClkInitTypeDef RCC_ClkInitStruct;
  RCC_PeriphCLKInitTypeDef PeriphClkInitStruct;

  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_LSI|RCC_OSCILLATORTYPE_HSE
                              |RCC_OSCILLATORTYPE_LSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.LSEState = RCC_LSE_ON;
  RCC_OscInitStruct.LSIState = RCC_LSI_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 20;
  RCC_OscInitStruct.PLL.PLLN = 192;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 5;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;
  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_3) != HAL_OK)
  {
    Error_Handler();
  }

  PeriphClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_RTC;
  PeriphClkInitStruct.RTCClockSelection = RCC_RTCCLKSOURCE_LSE;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

//  HAL_RCC_MCOConfig(RCC_MCO1, RCC_MCO1SOURCE_HSE, RCC_MCODIV_1);  /* MCO 25 MHz to LAN8720 */

  HAL_SYSTICK_Config(HAL_RCC_GetHCLKFreq()/1000);

  HAL_SYSTICK_CLKSourceConfig(SYSTICK_CLKSOURCE_HCLK);

  /* SysTick_IRQn interrupt configuration */
  HAL_NVIC_SetPriorityGrouping(NVIC_PRIORITYGROUP_4);
  HAL_NVIC_SetPriority(SysTick_IRQn, 15, 0);
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
  * @brief  Period elapsed callback in non blocking mode
  * @note   This function is called  when TIM12 interrupt took place, inside
  * HAL_TIM_IRQHandler(). It makes a direct call to HAL_IncTick() to increment
  * a global variable "uwTick" used as application time base.
  * @param  htim : TIM handle
  * @retval None
  */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
/* USER CODE BEGIN Callback 0 */
	static signed portBASE_TYPE xHigherPriorityTaskWoken;
	xHigherPriorityTaskWoken = pdFALSE;

  switch((uint32_t)htim->Instance)
  {
  case (uint32_t)TIM12:
    HAL_IncTick();
    vPLC_TimersCallback(&xHigherPriorityTaskWoken);
    break;
  case (uint32_t)TIM7:
	  usb_stop_break();
	  xSemaphoreGiveFromISR( myBinarySemUSBHandle, &xHigherPriorityTaskWoken );
	  break;
  case (uint32_t)TIM6:
	  com1_stop_break();
	  xSemaphoreGiveFromISR( myBinarySemCOM1_breakHandle, &xHigherPriorityTaskWoken );
	  break;
  case (uint32_t)TIM9:
	  /* Timer 9 Modbus Over DCON Master BreakTime */
      com3_stop_break();
      xSemaphoreGiveFromISR( myBinarySemCOM3Handle, &xHigherPriorityTaskWoken );
    break;
  case (uint32_t)TIM10:                              /* Timer 10 - CANOpen events timer */
    break;
  case (uint32_t)TIM11:                              /* Timer 11 - Monoblocks Net timer */
    break;
  }
  if( xHigherPriorityTaskWoken ) portYIELD_FROM_ISR( xHigherPriorityTaskWoken );
/* USER CODE END Callback 1 */

}
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
    static signed portBASE_TYPE xHigherPriorityTaskWoken;
    xHigherPriorityTaskWoken = pdFALSE;

    switch((uint32_t)huart->Instance)
    {
        case (uint32_t)USART1:
          if(com1_RxTx.counter >=  MODBUS_BUF_SZ) com1_RxTx.counter = 0;    /* Test buffer overflow */
          com1_RxTx.buffer[com1_RxTx.counter++] = com1_RxByte;              /* Received byte to buffer */
          HAL_UART_Receive_IT(&huart1, &com1_RxByte,1);                     /* Wait next byte */
          com1_start_break();                                               /* Start next 3.5 time break */
          break;
        case (uint32_t)USART2:
//          break;
//        case (uint32_t)USART3:
          if(com3_RxTx.counter >=  MODBUS_BUF_SZ) com3_RxTx.counter = 0;    /* Test buffer overflow */
          com3_RxTx.buffer[com3_RxTx.counter++] = com3_RxByte;              /* Received byte to buffer */
          switch(com3_Mode)
          {
              case DCON_MODE:
                if(com3_RxByte == DCON_CR)                                        /* End of DCON frame */
                {
                    com3_RxTx.buffer[com3_RxTx.counter++] = '\0';                 /* Add end of string */
                    xSemaphoreGiveFromISR( myBinarySemCOM3Handle, &xHigherPriorityTaskWoken ); /* Semaphore - Answer received */
                }
                else
                    HAL_UART_Receive_IT(&hDCON_PORT, &com3_RxByte,1);
                break;
              case MODBUS_MODE:
                HAL_UART_Receive_IT(&hDCON_PORT, &com3_RxByte,1);
                com3_start_break();
                break;
          }
          break;
    }
    if( xHigherPriorityTaskWoken ) portYIELD_FROM_ISR( xHigherPriorityTaskWoken );
}
void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart)
{
    switch((uint32_t)huart->Instance)
    {
        case (uint32_t)USART1:
          com1_RxTx.counter = 0;                                          /* Clear counter of receiver */
          break;
        case (uint32_t)USART2:
//          set_MY_TX_flag();
//          break;
//        case (uint32_t)USART3:
          if(huart->gState == HAL_UART_STATE_READY)
          {
        	  DCON_TE_OFF;
        	  HAL_UART_Receive_IT(&hDCON_PORT, &com3_RxByte,1);
        	  com3_RxTx.counter = 0;
          }
          break;
    }
}

void HAL_UART_ErrorCallback(UART_HandleTypeDef *huart)
{
    switch((uint32_t)huart->Instance)
    {
        case (uint32_t)USART2:
          break;
        default:
          break;
    }
}
void HAL_ADC_ConvCpltCallback( ADC_HandleTypeDef * hadc){
/*    HAL_ADC_Stop_DMA(hadc);
    HAL_ADC_Stop(hadc);
    led_spark(GREEN,10);*/
}
/**
  * @brief  This function is executed in case of error occurrence.
  * @param  None
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler */
  /* User can add his own implementation to report the HAL error return state */
  while(1) 
  {
	  led_on(RED);
  }
  /* USER CODE END Error_Handler */ 
}
extern __IO uint32_t uwTick;
void vApplicationMallocFailedHook( void )
{
	for(;;)
	{
		led_on(RED);
	}
}
#ifdef USE_FULL_ASSERT

/**
   * @brief Reports the name of the source file and the source line number
   * where the assert_param error has occurred.
   * @param file: pointer to the source file name
   * @param line: assert_param error line source number
   * @retval None
   */
void assert_failed(uint8_t* file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
    ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */

}

#endif

/**
  * @}
  */ 

/**
  * @}
*/ 

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
