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

#include "mybuf.h"
#include "uart_buf.h"
#include "morse.h"

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

#define DEBOUNCE_DELAY 50 // ms
#define LONG_PRESS_DELAY 200 // ms
#define SHOW_SAVED_DELAY 900 // ms

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
GPIO_PinState button_state = GPIO_PIN_SET;
GPIO_PinState last_reading = GPIO_PIN_SET;
GPIO_PinState led_state = GPIO_PIN_RESET;

uint32_t last_deb_time = 0;
uint32_t last_move_time = 0;

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

void light_up(led_color led) {
	switch (led) {
	case RED:
		HAL_GPIO_WritePin(GPIOD, GPIO_PIN_15, GPIO_PIN_SET);
		break;
	case ORANGE:
		HAL_GPIO_WritePin(GPIOD, GPIO_PIN_14, GPIO_PIN_SET);
		break;
	case GREEN:
		HAL_GPIO_WritePin(GPIOD, GPIO_PIN_13, GPIO_PIN_SET);
		break;
	}
}

void light_down(led_color led) {
	switch (led) {
	case RED:
		HAL_GPIO_WritePin(GPIOD, GPIO_PIN_15, GPIO_PIN_RESET);
		break;
	case ORANGE:
		HAL_GPIO_WritePin(GPIOD, GPIO_PIN_14, GPIO_PIN_RESET);
		break;
	case GREEN:
		HAL_GPIO_WritePin(GPIOD, GPIO_PIN_13, GPIO_PIN_RESET);
		break;
	}
}

bool is_pressed() {
	return !button_state;
}

bool is_long_press() {
	return is_pressed() && HAL_GetTick() - last_move_time > LONG_PRESS_DELAY;
}

int is_changed(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin) {
	GPIO_PinState reading = HAL_GPIO_ReadPin(GPIOx, GPIO_Pin);

	if (reading != last_reading) {
		last_deb_time = HAL_GetTick();
		last_reading = reading;
	}

	if ((HAL_GetTick() - last_deb_time) > DEBOUNCE_DELAY) {
		if (reading != button_state) {
			button_state = reading;
			last_move_time = HAL_GetTick();

			if (is_pressed()) {
				return 1;
			} else {
				return -1;
			}
		}
	}

	return 0;
}

bool uart6_transmit_ongoing;
bool uart6_receive_finished;
uint8_t uart6_buf;

//HAL_StatusTypeDef uart6_transmit_it(uint8_t *buf, int len) {
//	while (uart6_transmit_ongoing);
//
//	uart6_transmit_ongoing = 1;
//	return HAL_UART_Transmit_IT(&huart6, buf, len);
//}

HAL_StatusTypeDef uart6_start_receive_char_it() {
	uart6_receive_finished = 0;
	return HAL_UART_Receive_IT(&huart6, &uart6_buf, 1);
}

bool uart6_try_push_received_char(uart_buf* buffer) {
	if (uart6_receive_finished) {
		uart_buf_push(buffer, (char) uart6_buf);
		return true;
	}

	return false;
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart) {
	if (huart->Instance == USART6) {
		uart6_receive_finished = 1;
	}
}

//void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart) {
//	if (huart->Instance == USART6) {
//		uart6_transmit_ongoing = 0;
//	}
//}

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

  my_buf buf = (my_buf) {};
  my_buf_init(&buf);

//  bool is_showing_buf = false;
//  bool was_long_press = false;
//  bool is_printing = false;
//
//  uint32_t duration_start_time = 0;
//
//  morse sym;

  uart_buf buffer;

  uart6_start_receive_char_it();
  char c;
  bool is_showing = false;
  bool is_printing = false;

  uint32_t duration_start_time;
  uint32_t letter_pause_start_time = HAL_GetTick();;
  morse sym;
  int i = 0;

  uart_buf_reset(&buffer);
  bool was_long_press = false;

  while (1)
  {
	  if (uart6_try_push_received_char(&buffer)) {
		  uart6_start_receive_char_it();
	  }

	  if (!is_showing && uart_buf_pop(&buffer, &c)) {
		  duration_start_time = HAL_GetTick();
		  is_showing = true;
	  }

	  if (is_showing && HAL_GetTick() - letter_pause_start_time > LETTER_PAUSE_DURATION) {
		  if (!is_printing && HAL_GetTick() - duration_start_time > PAUSE_DURATION) {
				  if (!morse_get_sym(c, i++, &sym)) {
					  i = 0;
					  is_showing = false;
				  } else {
					  is_printing = true;
					  light_up(GREEN);
					  duration_start_time = HAL_GetTick();
				  }
		  } else if (
				(sym == MORSE_DASH && HAL_GetTick() - duration_start_time > DASH_DURATION)
				|| (sym == MORSE_DOT && HAL_GetTick() - duration_start_time > DOT_DURATION)
			  ) {
				  light_down(GREEN);
				  is_printing = false;
				  duration_start_time = HAL_GetTick();
				  if (!morse_get_sym(c, i, &sym)) {
					  i = 0;
					  is_showing = false;
					  letter_pause_start_time = HAL_GetTick();
				  }
		  }
	  }

	  if (!is_showing) {
	  		  if (is_long_press()) {
	  			  was_long_press = true;
	  		  }

	  		  switch (is_changed(GPIOC, GPIO_PIN_15)) {
	  		  	  case 1:
	  		  		  break;
	  		  	  case -1:
	  		  		  if (was_long_press) {
	  		  			  light_up(RED);
	  					  HAL_Delay(5);
	  					  light_down(RED);
	  		  			  my_buf_write(&buf, MORSE_DASH);
	  		  			  was_long_press = false;
	  		  		  } else {
	  		  			  light_up(ORANGE);
	  		  			  HAL_Delay(5);
	  		  			  light_down(ORANGE);
	  		  			  my_buf_write(&buf, MORSE_DOT);
	  		  		  }
	  		  		  break;
	  		  	  default:
	  		  		  break;
	  		  }

	  		  if (
	  				  (!my_buf_empty(&buf) && HAL_GetTick() - last_move_time > SHOW_SAVED_DELAY && !is_pressed())
	  				  || my_buf_full(&buf)
	  		  ) {
	  			  char ch = decode_char(&buf);
	  			  my_buf_reset(&buf);
	  			  if (ch != '\0') {
	  				HAL_StatusTypeDef ok = HAL_UART_Transmit(&huart6, (uint8_t *) &ch, 1, 100);
	  				if (ok == HAL_OK) {
	  					light_up(RED);
					  HAL_Delay(500);
					  light_down(RED);
	  				}
	  			  }
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
