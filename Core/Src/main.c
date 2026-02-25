/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2026 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <stdbool.h>

#include "morse_buf.h"
#include "led.h"
#include "timing.h"
#include "button_driver.h"
#include "uart_buf.h"
#include "morse.h"
#include "uart_driver.h"

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

#define DURATION_TIMER 0
#define LETTER_PAUSE_TIMER 1

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */

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
  MX_USART6_UART_Init();
  /* USER CODE BEGIN 2 */

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */

  morse_buf button_buf;

  bool is_showing = false;
  bool is_printing = false;
  bool was_long_press = false;

  char received_char;
  morse curr_morse_sym;
  int curr_morse_sym_i = 0;

  uart_buf_reset(&buffer_rx);
  uart_buf_reset(&buffer_tx);
  morse_buf_reset(&button_buf);

  timer_reset(DURATION_TIMER);
  timer_reset(LETTER_PAUSE_TIMER);

  while (1)
  {
	  receive_char_into_buffer();
	  transmit_char_from_buffer();

	  if (!is_showing && uart_buf_pop(&buffer_rx, &received_char)) {
		  if (received_char == '+') {
			  uart6_switch_interrupt_mode();
		  } else {
			  if (morse_get_sym(received_char, 0, &curr_morse_sym)) {
				  timer_reset(DURATION_TIMER);
				  is_showing = true;
			  }
		  }
	  }

	  start_receive_char();

	  if (is_showing && is_timer_expired(LETTER_PAUSE_TIMER, LETTER_PAUSE_DURATION)) {
		  if (!is_printing && is_timer_expired(DURATION_TIMER, PAUSE_DURATION)) {
			  curr_morse_sym_i++;
			  is_printing = true;
			  light_up(GREEN);
			  timer_reset(DURATION_TIMER);
		  } else if (
				(is_printing && curr_morse_sym == MORSE_DASH && is_timer_expired(DURATION_TIMER, DASH_DURATION))
				|| (is_printing && curr_morse_sym == MORSE_DOT && is_timer_expired(DURATION_TIMER, DOT_DURATION))
			  ) {
				  light_down(GREEN);
				  is_printing = false;
				  timer_reset(DURATION_TIMER);
				  if (!morse_get_sym(received_char, curr_morse_sym_i, &curr_morse_sym)) {
					  curr_morse_sym_i = 0;
					  is_showing = false;
					  timer_reset(LETTER_PAUSE_TIMER);
				  }
		  }
	  }

	  if (is_long_press()) {
		  was_long_press = true;
	  }

	  switch (update_button_state()) {
		  case 1:
			  break;
		  case -1:
			  if (was_long_press) {
				  light_blink(RED);
				  morse_buf_write(&button_buf, MORSE_DASH);
				  was_long_press = false;
			  } else {
				  light_blink(ORANGE);
				  morse_buf_write(&button_buf, MORSE_DOT);
			  }
			  break;
		  default:
			  break;
	  }

	  if (
			  (!morse_buf_empty(&button_buf) && is_long_unpress()) || morse_buf_full(&button_buf)
	  ) {
		  char ch = decode_char(&button_buf);
		  morse_buf_reset(&button_buf);
		  if (ch != '\0') {
			  uart_buf_push(&buffer_tx, ch);
		  }
	  }
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

  /** Configure the main internal regulator output voltage
  */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 15;
  RCC_OscInitStruct.PLL.PLLN = 216;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 4;
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

/* USER CODE BEGIN 4 */

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
#ifdef USE_FULL_ASSERT
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
