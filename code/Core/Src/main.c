/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2024 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "adc.h"
#include "comp.h"
#include "dac.h"
#include "dma.h"
#include "spi.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <stdio.h>
#include "oscilloscope.h"

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define DAC_VREF 3.3
#define DAC_RESOLUTION 4096

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
volatile uint8_t SPI1_TX_completed_flag = 1;
static int conv_done = 0;
int ready_to_draw = 0;
volatile int done_drawing = 1;
volatile uint8_t comparatorTriggeredFlag;
Oscilloscope oscilloscope;


/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
void HAL_SPI_TxCpltCallback(SPI_HandleTypeDef *hspi){
	SPI1_TX_completed_flag = 1;
}

void HAL_COMP_TriggerCallback(COMP_HandleTypeDef *hcomp){
	comparatorTriggeredFlag = 1;
	if(conv_done & done_drawing){
		conv_done = 0;
		HAL_ADC_Start_DMA(&hadc1, (uint32_t*) oscilloscope.ch1.waveform_raw_adc , MEMORY_DEPTH);
		HAL_ADC_Start_DMA(&hadc2, (uint32_t*) oscilloscope.ch2.waveform_raw_adc , MEMORY_DEPTH);
		ready_to_draw = 1;
		done_drawing = 0;
	}
	//HAL_COMP_Stop(&hcomp1);
}

/**
 * @brief function for printing using UART
 */
int _write(int file, char* ptr, int len){
	HAL_UART_Transmit(&huart2, (uint8_t *)ptr, len, HAL_MAX_DELAY);
	return len;
}

/**
 * @brief convert voltage to DAC value
 */
int conv_voltage_to_DAC(float voltage){
	int dac = voltage*DAC_RESOLUTION/DAC_VREF;
	if(dac > DAC_RESOLUTION){
		dac = DAC_RESOLUTION;
	}
	return (int) dac;
}

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim){
	if(htim == &htim4){
		if(!comparatorTriggeredFlag && !oscilloscope.stop){
			HAL_ADC_Start_DMA(&hadc1, (uint32_t*) oscilloscope.ch1.waveform_raw_adc , MEMORY_DEPTH);
			HAL_ADC_Start_DMA(&hadc2, (uint32_t*) oscilloscope.ch2.waveform_raw_adc , MEMORY_DEPTH);

			done_drawing = 0;
		}
	}
}
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
  MX_SPI1_Init();
  MX_SPI2_Init();
  MX_TIM3_Init();
  MX_ADC1_Init();
  MX_TIM1_Init();
  MX_USART2_UART_Init();
  MX_DAC1_Init();
  MX_COMP1_Init();
  MX_ADC2_Init();
  MX_TIM4_Init();
  /* USER CODE BEGIN 2 */


  HAL_TIM_Base_Start_IT(&htim4);
  HAL_DAC_Start(&hdac1, DAC_CHANNEL_1);
  HAL_COMP_Start(&hcomp1);
  //HAL_COMP_Stop(&hcomp1);
  HAL_ADC_Start_DMA(&hadc1, (uint32_t*) oscilloscope.ch1.waveform_raw_adc , MEMORY_DEPTH);
  HAL_ADC_Start_DMA(&hadc2, (uint32_t*) oscilloscope.ch2.waveform_raw_adc , MEMORY_DEPTH);
  HAL_TIM_Encoder_Start(&htim1, TIM_CHANNEL_ALL);
  HAL_TIM_PWM_Start(&htim3, TIM_CHANNEL_4);
  HAL_TIM_PWM_Start(&htim3, TIM_CHANNEL_3);
  __HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_4, LCD_BRIGHTNESS); // 0-1000
  __HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_3, 200); // 0-1000
  ILI9488_Init();
  setRotation(1);
  ILI9341_Fill_Screen(ILI9488_BLACK);

  oscilloscopeInit(&oscilloscope);

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */

  while (1){
	  clearScreen();
	  drawGrid();

	  //__HAL_DMA_GET_COUNTER()
	  displayTimeBase(&oscilloscope);
	  drawMainMenuButton();
	  displayHorizontallOffset(&oscilloscope);


	  switch (oscilloscope.timeBase_us){
	  case 10:
		  hadc1.Init.ClockPrescaler = ADC_CLOCK_ASYNC_DIV1;
		  break;
	  case 20:
		  hadc1.Init.ClockPrescaler = ADC_CLOCK_ASYNC_DIV2;
		  break;
	  case 40:
		  hadc1.Init.ClockPrescaler = ADC_CLOCK_ASYNC_DIV4;
		  break;
	  case 80:
		  hadc1.Init.ClockPrescaler = ADC_CLOCK_ASYNC_DIV8;
		  break;
	  case 160:
		  hadc1.Init.ClockPrescaler = ADC_CLOCK_ASYNC_DIV16;
		  break;
	  case 320:
		  hadc1.Init.ClockPrescaler = ADC_CLOCK_ASYNC_DIV32;
		  break;
	  case 640:
		  hadc1.Init.ClockPrescaler = ADC_CLOCK_ASYNC_DIV64;
		  break;
	  case 1280:
		  hadc1.Init.ClockPrescaler = ADC_CLOCK_ASYNC_DIV128;
		  break;
	  case 2560:
		  hadc1.Init.ClockPrescaler = ADC_CLOCK_ASYNC_DIV256;
		  break;
	  }
	  // Initialize the ADC with the configured settings
	  if (HAL_ADC_Init(&hadc1) != HAL_OK){
		  // Initialization Error
		  Error_Handler();
	  }

	  if(oscilloscope.stop){
		  HAL_COMP_Stop(&hcomp1);
	  }else{
		  HAL_COMP_Start(&hcomp1);
	  }
	  drawChannels0Vmarkers(&oscilloscope.ch1);
	  drawChannels0Vmarkers(&oscilloscope.ch2);

	  if(ready_to_draw){
		  if(oscilloscope.ch1.isOn)
			  draw_waveform(& oscilloscope.ch1, oscilloscope.timeBase_us, oscilloscope.x_offset, oscilloscope.stop);
		  if(oscilloscope.ch2.isOn)
			  draw_waveform(& oscilloscope.ch2, oscilloscope.timeBase_us, oscilloscope.x_offset, oscilloscope.stop);
		  ready_to_draw = 0;
		  done_drawing = 1;
		  comparatorTriggeredFlag = 0;
	  }

	  else if(oscilloscope.stop || !comparatorTriggeredFlag){
		  if(oscilloscope.ch1.isOn)
			  draw_waveform(& oscilloscope.ch1, oscilloscope.timeBase_us, oscilloscope.x_offset, oscilloscope.stop);
		  if(oscilloscope.ch2.isOn)
			  draw_waveform(& oscilloscope.ch2, oscilloscope.timeBase_us, oscilloscope.x_offset, oscilloscope.stop);
	  }

	  serveTouchScreen(&oscilloscope);
	  serveEncoder(&oscilloscope);

	  drawChanellVperDev(0, & oscilloscope.ch1);
	  drawChanellVperDev(110, & oscilloscope.ch2);
	  uint32_t sampling_frequency = 4210526/(oscilloscope.timeBase_us/10);
	  if(oscilloscope.ch1.isOn){
		  calculateFFT(&oscilloscope.ch1, sampling_frequency);
	  }
	  if(oscilloscope.ch2.isOn){
		  calculateFFT(&oscilloscope.ch2, sampling_frequency);
	  }
	  drawMeasurements(&oscilloscope);
	  drawRunStop(&oscilloscope);
	  drawTriggerIcon(&oscilloscope);
	  HAL_DAC_SetValue(&hdac1, DAC_CHANNEL_1, DAC_ALIGN_12B_R, conv_voltage_to_DAC(oscilloscope.triggerLevel_mV/1000.0));

	  imageRender();

/*
	  //ILI9341_Fill_Screen(ILI9488_BLACK);
	  //drawImage(image, 10, 10, 200, 1);
	  if(conv_done){
		  conv_done = 0;
		  draw_waveform(& CH1);
		  //HAL_Delay(500);
		  sprintf(buf1,"Vpp=%d", calculate_peak_to_peak(CH1.waveform_display));
		  fillRect(39, 1, 35, 18, RED);
		  LCD_Font(5, 15, buf1, _Open_Sans_Bold_12  , 1, WHITE);
		  HAL_ADC_Start_DMA(&hadc1, (uint32_t*) CH1.waveform, MEMORY_DEPTH);
		  //HAL_Delay(1);
	  }
	  */

    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};
  RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLM = 1;
  RCC_OscInitStruct.PLL.PLLN = 10;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV7;
  RCC_OscInitStruct.PLL.PLLQ = RCC_PLLQ_DIV2;
  RCC_OscInitStruct.PLL.PLLR = RCC_PLLR_DIV2;
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
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_4) != HAL_OK)
  {
    Error_Handler();
  }
  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_USART2|RCC_PERIPHCLK_ADC;
  PeriphClkInit.Usart2ClockSelection = RCC_USART2CLKSOURCE_PCLK1;
  PeriphClkInit.AdcClockSelection = RCC_ADCCLKSOURCE_SYSCLK;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
  {
    Error_Handler();
  }
  /** Configure the main internal regulator output voltage
  */
  if (HAL_PWREx_ControlVoltageScaling(PWR_REGULATOR_VOLTAGE_SCALE1) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */
void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef* hadc){
	conv_done = 1;
	HAL_ADC_Stop_DMA(hadc);
	ready_to_draw = 1;
	//HAL_COMP_Start(&hcomp1);
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
     tex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
