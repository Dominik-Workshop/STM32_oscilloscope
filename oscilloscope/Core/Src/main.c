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
#include "dma.h"
#include "spi.h"
#include "tim.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "ili9488.h"
#include "xpt2046.h"
#include "stdio.h"
#include "oscilloscope.h"
#include "008_Open_Sans_Bold.h"
#include "009_Open_Sans_Bold.h"
#include "010_Open_Sans_Bold.h"
#include "012_Open_Sans_Bold.h"
#include "014_Open_Sans_Bold.h"
#include "016_Open_Sans_Bold.h"
#include "018_Open_Sans_Bold.h"
#include "020_Open_Sans_Bold.h"
#include "022_Open_Sans_Bold.h"
#include "024_Open_Sans_Bold.h"
#include "026_Open_Sans_Bold.h"
#include "028_Open_Sans_Bold.h"
#include "036_Open_Sans_Bold.h"
#include "048_Open_Sans_Bold.h"
#include "072_Open_Sans_Bold.h"
#include "096_Open_Sans_Bold.h"
#include "112_Open_Sans_Bold.h"
#include "128_Open_Sans_Bold.h"

#define _Open_Sans_Bold_8      &Open_Sans_Bold_8
#define _Open_Sans_Bold_9      &Open_Sans_Bold_9
#define _Open_Sans_Bold_10     &Open_Sans_Bold_10
#define _Open_Sans_Bold_11     &Open_Sans_Bold_11
#define _Open_Sans_Bold_12      &Open_Sans_Bold_12
#define _Open_Sans_Bold_14      &Open_Sans_Bold_14
#define _Open_Sans_Bold_16      &Open_Sans_Bold_16
#define _Open_Sans_Bold_18      &Open_Sans_Bold_18
#define _Open_Sans_Bold_20      &Open_Sans_Bold_20
#define _Open_Sans_Bold_22      &Open_Sans_Bold_22
#define _Open_Sans_Bold_24      &Open_Sans_Bold_24
#define _Open_Sans_Bold_26      &Open_Sans_Bold_26
#define _Open_Sans_Bold_28      &Open_Sans_Bold_28
#define _Open_Sans_Bold_36      &Open_Sans_Bold_36
#define _Open_Sans_Bold_48      &Open_Sans_Bold_48
#define _Open_Sans_Bold_72      &Open_Sans_Bold_72
#define _Open_Sans_Bold_96      &Open_Sans_Bold_96
#define _Open_Sans_Bold_112      &Open_Sans_Bold_112
#define _Open_Sans_Bold_128      &Open_Sans_Bold_128

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */


/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
volatile uint8_t SPI1_TX_completed_flag = 1;




static int conv_done = 0;

//uint8_t  image [90000];
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
void HAL_SPI_TxCpltCallback(SPI_HandleTypeDef *hspi)
{
	SPI1_TX_completed_flag = 1;
}



/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */
  oscilloscope_channel CH1;
  oscilloscope_channel_init(&CH1);

  char buf[20];						// buffer for measurements to be displayed
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
  /* USER CODE BEGIN 2 */
  //HAL_ADC_Start_DMA(&hadc1, (uint32_t*) CH1.waveform , MEMORY_DEPTH);
  HAL_TIM_Encoder_Start(&htim1, TIM_CHANNEL_ALL);
  HAL_TIM_PWM_Start(&htim3, TIM_CHANNEL_4);
  HAL_TIM_PWM_Start(&htim3, TIM_CHANNEL_3);
  __HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_4, LCD_BRIGHTNESS); // 0-1000
  __HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_3, 200); // 0-1000
  ILI9488_Init();
  setRotation(1);
  ILI9341_Fill_Screen(ILI9488_BLACK);
  drawGrid();

  setAddrWindow(463, 1, 463+13-1, 1+18-1);
  ILI9341_Draw_Colour_Burst(YELLOW, 35 * 18);
  LCD_Font(466, 15, "2", _Open_Sans_Bold_12  , 1, BLACK);

  HAL_Delay(500);

  setAddrWindow(463, 1, 463+13-1, 1+18-1);
  ILI9341_Draw_Colour_Burst(GREEN, 35 * 18);
  LCD_Font(440, 15, "Ch:", _Open_Sans_Bold_12  , 1, WHITE);
  LCD_Font(466, 15, "1", _Open_Sans_Bold_12  , 1, BLACK);



  //uint16_t touchX = 0, touchY = 0;
  /*for(int i = 0; i < 90000; ++i){
	  image[i] = 0xF;
	  ++i;
	  image[i] = 0xA;
  }

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  int faza = 0;
  while (1)
  {
	  /*//test drawing method speed
	  fillRect(0, 0, 480, 320, RED);
	  ILI9341_Fill_Screen(ILI9488_BLACK);
	  for(int i = 0; i < 480; i+=1){
	  		for(int j = 0; j < 320; j+=1)
	  			drawPixel(i, j, ILI9488_DARKGREY);
	  	}
	  	*/
	  /*
	  drawImage(image, 10, 10, 300, 150);
	  HAL_Delay(1000);
	  setAddrWindow(10, 10, 10+300-1, 10+150-1);
	  ILI9341_Draw_Colour_Burst(RED, 300 * 150);
	  HAL_Delay(1000);
	  */

	  for(int i = 0; i < 480; ++i){
	  	CH1.waveform[i] = 2000*sin(0.05*i + faza*0.1) + 2000;
	    }
	  faza++;
	  draw_waveform(& CH1);

	  sprintf(buf,"Vpp=%d", calculate_peak_to_peak(CH1.waveform));
	  setAddrWindow(39, 1, 39+35-1, 1+18-1);
	  ILI9341_Draw_Colour_Burst(RED, 35 * 18);
	  LCD_Font(5, 15, buf, _Open_Sans_Bold_12  , 1, WHITE);

	  sprintf(buf,"Vrms=%d", calculate_RMS(CH1.waveform));
	  setAddrWindow(122, 1, 122+35-1, 1+18-1);
	  ILI9341_Draw_Colour_Burst(RED, 35 * 18);
	  LCD_Font(80, 15, buf, _Open_Sans_Bold_12  , 1, WHITE);
	  //HAL_Delay(1);



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




	  //CODE FOR DMA
	  /*
	  ILI9341_Fill_Screen(ILI9488_GREEN);
	  LCD_Font(20, 100, "ISMAIL", _Open_Sans_Bold_112  , 1, WHITE);
	  LCD_Font(10, 250, "STM32", _Open_Sans_Bold_112  , 1, WHITE);HAL_Delay(1000);
	  ILI9341_Fill_Screen(ILI9488_BLUE);
	  LCD_Font(20, 100, "ISMAIL", _Open_Sans_Bold_112  , 1, WHITE);
	  LCD_Font(10, 250, "STM32", _Open_Sans_Bold_112  , 1, WHITE);
	  HAL_Delay(1000);

	  */
	  //    CODE FOR TOUCH
	  /*
	  //char buf[20];
	  //LCD_Font(25, 25, buf1, _Open_Sans_Bold_24  , 1, WHITE);
	  touchX = getX();
	  touchY = getY();
	  //sprintf(buf,"x=%3d y=%3d",touchX,touchY);
	  //sprintf(buf1,"x=%3d y=%3d",touchX,touchY);
	  //LCD_Font(25, 25, buf, _Open_Sans_Bold_24  , 1, BLACK);
	  fillRect(touchX, touchY,7,7, RED);
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
  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_ADC;
  PeriphClkInit.AdcClockSelection = RCC_ADCCLKSOURCE_PLLSAI1;
  PeriphClkInit.PLLSAI1.PLLSAI1Source = RCC_PLLSOURCE_HSI;
  PeriphClkInit.PLLSAI1.PLLSAI1M = 1;
  PeriphClkInit.PLLSAI1.PLLSAI1N = 8;
  PeriphClkInit.PLLSAI1.PLLSAI1P = RCC_PLLP_DIV7;
  PeriphClkInit.PLLSAI1.PLLSAI1Q = RCC_PLLQ_DIV2;
  PeriphClkInit.PLLSAI1.PLLSAI1R = RCC_PLLR_DIV2;
  PeriphClkInit.PLLSAI1.PLLSAI1ClockOut = RCC_PLLSAI1_ADC1CLK;
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
