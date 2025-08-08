/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2025 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32l4xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define MCO_Pin GPIO_PIN_0
#define MCO_GPIO_Port GPIOA
#define VCP_TX_Pin GPIO_PIN_2
#define VCP_TX_GPIO_Port GPIOA
#define SWDIO_Pin GPIO_PIN_13
#define SWDIO_GPIO_Port GPIOA
#define SWCLK_Pin GPIO_PIN_14
#define SWCLK_GPIO_Port GPIOA
#define VCP_RX_Pin GPIO_PIN_15
#define VCP_RX_GPIO_Port GPIOA
#define GPIO_LEFT_Pin GPIO_PIN_3
#define GPIO_LEFT_GPIO_Port GPIOB
#define GPIO_RIGHT_Pin GPIO_PIN_5
#define GPIO_RIGHT_GPIO_Port GPIOB

/* USER CODE BEGIN Private defines */
#define GRID_WIDTH	16
#define GRID_HEIGHT	 5
#define TOTAL_BRICKS (GRID_WIDTH * GRID_HEIGHT)

#define BLOCK_WIDTH		8U
#define BLOCK_HEIGHT	4U

#define HALF_BLOCK_WIDTH	(BLOCK_WIDTH/2)
#define HALF_BLOCK_HEIGHT	(BLOCK_HEIGHT/2)

#define GRID_START		9

#define PADDLE_WIDTH	14
#define PADDLE_HEIGHT	4

#define HALF_PADDLE_WIDTH	(PADDLE_WIDTH/2)
#define HALF_PADDLE_HEIGHT	(PADDLE_HEIGHT/2)

#define BALL_RADIUS		2

#define FALSE			0
#define TRUE			1

#define LEFT_BOUND		4
#define UPPER_BOUND		4

#define POINTS_PER_BLOCK	10

#define LEVEL_SIZE		(TOTAL_BRICKS/8)
/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
