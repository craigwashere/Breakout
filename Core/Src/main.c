/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  * @author			: Craig Beaubien
  * 				: CPE 187
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
  * Main file for Breakout on OLED display. Drivers are in SSD1306* files.
  * Collision logic came from https://learnopengl.com/In-Practice/2D-Game/Collisions/Collision-resolution
  * Math logic inspired by Game Physics Cookbook by Gabor Szauer
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <stdio.h>
#include <stdlib.h>
#include "ssd1306.h"

#include <stdlib.h>
#include <string.h>

#include "Geometry2D.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */
struct _brick
{
	uint8_t x, y;
	SSD1306_COLOR color;
} bricks[GRID_HEIGHT][GRID_WIDTH];

struct
{
	volatile uint8_t x, y, old_x, old_y;
} paddle;

struct
{
	volatile float x, y, old_x, old_y;
} ball;

vec2f ball_velocity = {2.0, -2.0 }
   , INITIAL_BALL_VELOCITY = { 2.0, -2.0 };

enum GameState
{
	  GAME_ACTIVE,
	  GAME_SERVE,
	  GAME_WIN,
	  GAME_OVER
} game_state = GAME_SERVE;

enum DIRECTION
{
	UP = 0,
	RIGHT,
	DOWN,
	LEFT
} ;

uint8_t current_level = 1;

uint16_t score = 0;
/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define EEPROM_PORT		hi2c3

#define EEPROM_DEVICE_ADDRESS 0xA0
#define EEPROM_MEMORY_ADDRESS 0X1AA0
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
I2C_HandleTypeDef hi2c1;
I2C_HandleTypeDef hi2c3;

UART_HandleTypeDef huart2;

/* USER CODE BEGIN PV */
uint8_t lives = 3
	  , bricks_left = 0;

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_USART2_UART_Init(void);
static void MX_I2C1_Init(void);
static void MX_I2C3_Init(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
void FillRectangle(uint8_t start_x, uint8_t start_y, uint8_t width, uint8_t height, SSD1306_COLOR color)
{
	ssd1306_FillRectangle(start_x, start_y, start_x+width, start_y+height, color);
}

void draw_brick(uint8_t x, uint8_t y, SSD1306_COLOR color)
{
	FillRectangle(bricks[x][y].x - HALF_BLOCK_WIDTH,
				  bricks[x][y].y - HALF_BLOCK_HEIGHT,
				  BLOCK_WIDTH,
				  BLOCK_HEIGHT,
				  color);
}

void reset_ball()
{
	ball.x = paddle.x + HALF_PADDLE_WIDTH;
	ball.y = paddle.y - HALF_PADDLE_HEIGHT;

	ball_velocity = INITIAL_BALL_VELOCITY;
}

uint8_t * get_bricks(uint8_t level)
{
	uint8_t *brick = malloc(LEVEL_SIZE);
	memset(brick, 0, LEVEL_SIZE);

	HAL_I2C_Mem_Read(&EEPROM_PORT							// I2C_HandleTypeDef
			       , EEPROM_DEVICE_ADDRESS						// DevAddress
				   , EEPROM_MEMORY_ADDRESS+(LEVEL_SIZE*level)	// MemAddress
				   , I2C_MEMADD_SIZE_16BIT					// MemAddSize
				   , (uint8_t *)brick						// pData
				   , LEVEL_SIZE								// Size
				   , HAL_MAX_DELAY);						// Timeout

	return brick;
}

SSD1306_COLOR get_color(uint8_t i, uint8_t j, uint8_t *level)
{
	uint8_t byte = ((i*GRID_WIDTH)+j)/8;
	j -= (byte%2)*8;

	return (SSD1306_COLOR)((level[byte] >> (8-j-1)) & 1);
}

void setup_level(uint8_t level)
{
	uint8_t *brick = get_bricks(level-1);
	for (uint8_t i = 0; i < GRID_HEIGHT; i++)
		for (uint8_t j = 0; j < GRID_WIDTH; j++)
		{
			bricks[i][j].x = (j * BLOCK_WIDTH);
			bricks[i][j].y = (i * BLOCK_HEIGHT) + GRID_START;
			bricks[i][j].color = get_color(i, j, brick);
			if (bricks[i][j].color == White)
				bricks_left++;
		}
	free(brick);
}

void init()
{
	bricks_left = 0;

	current_level = 1;

	setup_level(current_level);

	paddle.x = 30;
	paddle.y = 60;

	reset_ball();

	ssd1306_Fill(Black);

	lives = 3;

	score = 0;
}

void draw_grid()
{
	for (uint8_t i = 0; i < GRID_HEIGHT; i++)
		for (uint8_t j = 0; j < GRID_WIDTH; j++)
			FillRectangle(bricks[i][j].x, bricks[i][j].y, BLOCK_WIDTH, BLOCK_HEIGHT, bricks[i][j].color);
}

void draw_paddle()
{
	FillRectangle(paddle.old_x,
				  paddle.old_y,
				  PADDLE_WIDTH,
				  PADDLE_HEIGHT,
				  Black);

	FillRectangle(paddle.x,
				  paddle.y,
				  PADDLE_WIDTH,
				  PADDLE_HEIGHT,
				  White);
}

void draw_ball()
{
	ssd1306_FillCircle((int)ball.old_x, (int)ball.old_y, BALL_RADIUS, Black);
	ssd1306_FillCircle((int)ball.x, 	(int)ball.y, 	 BALL_RADIUS, White);
}

void write_status_line(uint8_t max_level)
{
	  ssd1306_SetCursor(1, 1);
	  char status[20];

	  sprintf(status, "Lives: %1hu  %1hu/%1hu  %5u", lives-1, current_level, max_level, score);
	  ssd1306_WriteString(status, Font_6x8, White);
}

uint8_t read_buttons()
{
	return ((HAL_GPIO_ReadPin(GPIOB, GPIO_LEFT_Pin) << 1) | HAL_GPIO_ReadPin(GPIOB, GPIO_RIGHT_Pin));
}

void update_ball()
{
	if (GAME_SERVE == game_state)
	{
	  ball.old_x = ball.x;
	  ball.old_y = ball.y;
	  ball.x = paddle.x + HALF_PADDLE_WIDTH;
	  return;
	}

	ball.old_x = ball.x;
	ball.old_y = ball.y;

	ball.x += ball_velocity.x;
	if (ball.x <= LEFT_BOUND)
	{
		ball_velocity.x *= -1;
		ball.x = LEFT_BOUND + 1;
	}
	else if (ball.x >= (SSD1306_WIDTH - BALL_RADIUS))
		ball_velocity.x *= -1;

	ball.y += ball_velocity.y;
	if (ball.y <= UPPER_BOUND)
	{
		ball_velocity.y *= -1;
		ball.y = UPPER_BOUND + 1;
	}
	else if (ball.y >= SSD1306_HEIGHT)
	{
	  game_state = GAME_SERVE;
	  reset_ball();

	  lives -= 1;
	}
}

void update_paddle()
{
	uint8_t multiplier = 0;
	switch (read_buttons())
	{
	  case 3: game_state = GAME_ACTIVE;
			  break;
	  case 2: multiplier = -1;
			  break;
	  case 1: multiplier = 1;
			  break;
	  default:multiplier = 0;
	}
	paddle.old_x = paddle.x;
	paddle.old_y = paddle.y;
	paddle.x += (4 * multiplier);

	if (paddle.x >= SSD1306_WIDTH - PADDLE_WIDTH)
		paddle.x = SSD1306_WIDTH - PADDLE_WIDTH;
	else if (paddle.x <= LEFT_BOUND)
		paddle.x = LEFT_BOUND;
}

enum DIRECTION vector_direction(vec2f *target)
{
    vec2f compass[] = {
        make_vec2f(0.0f, 1.0f),	// up
		make_vec2f(1.0f, 0.0f),	// right
		make_vec2f(0.0f, -1.0f),	// down
		make_vec2f(-1.0f, 0.0f)	// left
    };

    float max = 0.0f;
    unsigned int best_match = -1;

    for (unsigned int i = 0; i < 4; i++)
    {
    	Normalizef(target);
        float dot_product = Dotf(target, &compass[i]);
        if (dot_product > max)
        {
            max = dot_product;
            best_match = i;
        }
    }
    return best_match;
}

uint8_t check_collision(struct _brick brick, enum DIRECTION *dir, vec2f *diff)
{
    // get center point circle first
    vec2f center;
    center.x = ball.x;
    center.y = ball.y;

    // calculate AABB info (center, half-extents)
    vec2f aabb_half_extents = make_vec2f(HALF_BLOCK_WIDTH, HALF_BLOCK_HEIGHT),
    	  aabb_half_extents2 = make_vec2f((0-(float)HALF_BLOCK_WIDTH), (0-(float)HALF_BLOCK_HEIGHT));

    vec2f aabb_center = make_vec2f(
        brick.x + HALF_BLOCK_WIDTH,
        brick.y + HALF_BLOCK_HEIGHT
    );

    // get difference vector between both centers
    *diff = Subtractf(&center, &aabb_center);
    vec2f clamped = clamp(*diff, aabb_half_extents2, aabb_half_extents);

    // add clamped value to AABB_center and we get the value of box closest to circle
    vec2f closest = Addf(&aabb_center, &clamped);

    // retrieve vector between center circle and closest point AABB and check if length <= radius
    *diff = Subtractf(&closest, &center);

    // not <= since in that case a collision also occurs when object ball exactly
    // touches object brick, which they are at the end of each collision resolution stage.
    if (Magnitudef(diff) < BALL_RADIUS)
	{
    	*dir = vector_direction(diff);
    	return TRUE;
   	}

    return FALSE;
}

void do_collisions()
{
	enum DIRECTION dir;
	vec2f diff;

	for (uint8_t row = 0; row < GRID_HEIGHT; row++)
	{
		for (uint8_t col = 0; col < GRID_WIDTH; col++)
		{
			if (bricks[row][col].color == White)
			{
				if (TRUE == check_collision(bricks[row][col], &dir, &diff))
				{
					bricks[row][col].color = Black;
					bricks_left--;
					score += 10;

                    if ((LEFT == dir) || (RIGHT == dir))
                    {
                        ball_velocity.x = -ball_velocity.x;
                        float penetration = BALL_RADIUS - abs(diff.x);
                        if (LEFT == dir)
                            ball.x += penetration;
                        else
                            ball.x -= penetration;
                    }
                    else
                    {
                    	ball_velocity.y = -ball_velocity.y;
                        float penetration = BALL_RADIUS - abs(diff.y);
                        if (UP == dir)
                            ball.y += penetration;
                        else
                            ball.y -= penetration;
                    }
				}
			}
		}
	}

	if ((ball.x >= paddle.x)
		&& (ball.x <= paddle.x + PADDLE_WIDTH)
		&& (abs(ball.y - paddle.y) <= (BALL_RADIUS)))
	{
        float centerBoard = (paddle.x + HALF_PADDLE_WIDTH);
        float distance = abs((ball.x) - centerBoard);
        float percentage = distance / (HALF_PADDLE_WIDTH);

        // then move accordingly
        float strength = 2.0f;
        vec2f oldVelocity = ball_velocity;
        ball_velocity.x = ball_velocity.x * percentage * strength;
        vec2f t = Normalizedf(&ball_velocity);

        // keep speed consistent over both axes (multiply by length of old velocity, so total strength is not changed)
        ball_velocity = Multiplyf(&t, Magnitudef(&oldVelocity));

        if (IsEqual(&ball_velocity, &oldVelocity))
        	ball_velocity = INITIAL_BALL_VELOCITY;
        else
			// fix sticky paddle
			ball_velocity.y = -1.0f * abs(oldVelocity.y);
	}
}

void write_end_message(char *text)
{
	ssd1306_SetCursor(64-((strlen(text)/2)*Font_11x18.FontWidth), 32-(Font_11x18.FontHeight/2));
	ssd1306_WriteString(text, Font_11x18, White);
}

void display_end_message()
{
	if (bricks_left == 0)
		write_end_message("YOU WIN!");
	else
		write_end_message("GAME OVER");
}

void write_levels()
{
	uint8_t levels[] = {
		//  row 1		row 2		row 3		row 4		row 5
			0xff, 0xff,	0xff, 0xff,	0xff, 0xff,	0xff, 0xff,	0xff, 0xff,  // level 1
//			0xff, 0xff, 0xce, 0x73, 0xce, 0x73, 0xce, 0x73, 0xff, 0xff,  // level 2
//			0xff, 0xff, 0xaa, 0xab, 0xd5, 0x55, 0xaa, 0xab, 0xff, 0xff,  // level 3
			0xf8, 0x1f, 0xf8, 0x1f, 0xf8, 0x1f, 0xf8, 0x1f, 0xf8, 0x1f,  // level 4
			0xbb, 0xbb, 0xa2, 0xa3, 0xa3, 0xbb, 0xa2, 0x23, 0xba, 0x3b,
	};

	HAL_I2C_Mem_Write(&EEPROM_PORT			// I2C_HandleTypeDef
		  	  	  , EEPROM_DEVICE_ADDRESS	// DevAddress
				  , EEPROM_MEMORY_ADDRESS	// MemAddress
				  , I2C_MEMADD_SIZE_16BIT	// MemAddSize
				  , (uint8_t *)levels		// pData
				  , 30			// Size
				  , HAL_MAX_DELAY);			// Timeout
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
  MX_USART2_UART_Init();
  MX_I2C1_Init();
  MX_I2C3_Init();
  /* USER CODE BEGIN 2 */
  ssd1306_Init();

//  write_levels();
  init();
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  uint8_t max_levels = 3;

  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
	  if (GAME_OVER != game_state)
	  {
		  write_status_line(max_levels);
		  update_paddle();

		  update_ball();

		  if (GAME_ACTIVE == game_state)
			  do_collisions();

		  draw_grid();
		  draw_paddle();
		  draw_ball();

		  if (bricks_left == 0)
		  {
			  if (current_level < max_levels)
			  {
				  current_level++;
				  setup_level(current_level);
				  reset_ball();
				  game_state = GAME_SERVE;
			  }
			  else
				  game_state = GAME_WIN;
		  }
		  if (lives == 0)
			  game_state = GAME_OVER;
	  }

	  if (GAME_OVER == game_state)
	  {
		  display_end_message();
		  if (read_buttons() == 3)
		  {
			  game_state = GAME_SERVE;
			  init();
		  }
	  }
	  ssd1306_UpdateScreen();
	  HAL_Delay(50);
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
  if (HAL_PWREx_ControlVoltageScaling(PWR_REGULATOR_VOLTAGE_SCALE1) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure LSE Drive Capability
  */
  HAL_PWR_EnableBkUpAccess();
  __HAL_RCC_LSEDRIVE_CONFIG(RCC_LSEDRIVE_LOW);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_LSE|RCC_OSCILLATORTYPE_MSI;
  RCC_OscInitStruct.LSEState = RCC_LSE_ON;
  RCC_OscInitStruct.MSIState = RCC_MSI_ON;
  RCC_OscInitStruct.MSICalibrationValue = 0;
  RCC_OscInitStruct.MSIClockRange = RCC_MSIRANGE_6;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_MSI;
  RCC_OscInitStruct.PLL.PLLM = 1;
  RCC_OscInitStruct.PLL.PLLN = 16;
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

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_1) != HAL_OK)
  {
    Error_Handler();
  }

  /** Enable MSI Auto calibration
  */
  HAL_RCCEx_EnableMSIPLLMode();
}

/**
  * @brief I2C1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_I2C1_Init(void)
{

  /* USER CODE BEGIN I2C1_Init 0 */

  /* USER CODE END I2C1_Init 0 */

  /* USER CODE BEGIN I2C1_Init 1 */

  /* USER CODE END I2C1_Init 1 */
  hi2c1.Instance = I2C1;
  hi2c1.Init.Timing = 0x00300F35;
  hi2c1.Init.OwnAddress1 = 0;
  hi2c1.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
  hi2c1.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
  hi2c1.Init.OwnAddress2 = 0;
  hi2c1.Init.OwnAddress2Masks = I2C_OA2_NOMASK;
  hi2c1.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
  hi2c1.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
  if (HAL_I2C_Init(&hi2c1) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure Analogue filter
  */
  if (HAL_I2CEx_ConfigAnalogFilter(&hi2c1, I2C_ANALOGFILTER_ENABLE) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure Digital filter
  */
  if (HAL_I2CEx_ConfigDigitalFilter(&hi2c1, 0) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN I2C1_Init 2 */

  /* USER CODE END I2C1_Init 2 */

}

/**
  * @brief I2C3 Initialization Function
  * @param None
  * @retval None
  */
static void MX_I2C3_Init(void)
{

  /* USER CODE BEGIN I2C3_Init 0 */

  /* USER CODE END I2C3_Init 0 */

  /* USER CODE BEGIN I2C3_Init 1 */

  /* USER CODE END I2C3_Init 1 */
  hi2c3.Instance = I2C3;
  hi2c3.Init.Timing = 0x00300F38;
  hi2c3.Init.OwnAddress1 = 0;
  hi2c3.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
  hi2c3.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
  hi2c3.Init.OwnAddress2 = 0;
  hi2c3.Init.OwnAddress2Masks = I2C_OA2_NOMASK;
  hi2c3.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
  hi2c3.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
  if (HAL_I2C_Init(&hi2c3) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure Analogue filter
  */
  if (HAL_I2CEx_ConfigAnalogFilter(&hi2c3, I2C_ANALOGFILTER_ENABLE) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure Digital filter
  */
  if (HAL_I2CEx_ConfigDigitalFilter(&hi2c3, 0) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN I2C3_Init 2 */

  /* USER CODE END I2C3_Init 2 */

}

/**
  * @brief USART2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART2_UART_Init(void)
{

  /* USER CODE BEGIN USART2_Init 0 */

  /* USER CODE END USART2_Init 0 */

  /* USER CODE BEGIN USART2_Init 1 */

  /* USER CODE END USART2_Init 1 */
  huart2.Instance = USART2;
  huart2.Init.BaudRate = 115200;
  huart2.Init.WordLength = UART_WORDLENGTH_8B;
  huart2.Init.StopBits = UART_STOPBITS_1;
  huart2.Init.Parity = UART_PARITY_NONE;
  huart2.Init.Mode = UART_MODE_TX_RX;
  huart2.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart2.Init.OverSampling = UART_OVERSAMPLING_16;
  huart2.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
  huart2.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
  if (HAL_UART_Init(&huart2) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART2_Init 2 */

  /* USER CODE END USART2_Init 2 */

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};
  /* USER CODE BEGIN MX_GPIO_Init_1 */

  /* USER CODE END MX_GPIO_Init_1 */

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pins : GPIO_LEFT_Pin GPIO_RIGHT_Pin */
  GPIO_InitStruct.Pin = GPIO_LEFT_Pin|GPIO_RIGHT_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /* USER CODE BEGIN MX_GPIO_Init_2 */

  /* USER CODE END MX_GPIO_Init_2 */
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
