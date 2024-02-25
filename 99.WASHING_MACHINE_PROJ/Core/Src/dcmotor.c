#include "dcmotor.h"
#include "main.h"
#include "button.h"
#include "led.h"

// BUTTON0 : start/stop	: LED1 (toggle)
// BUTTON1 : SPEED-UP	: LED2 (toggle)
// BUTTON2 : SPEED-DOWN	: LED3 (toggle)

uint8_t pwm_start_flag = 1; // dc motor start indicator flag
uint8_t pin_8 = 1;
uint8_t pin_7 = 0;
uint16_t CCR_UP_value; // CCR : Channel Control Register // PWM UP Control
uint16_t CCR_DOWN_value; // CCR : Channel Control Register // PWM DOWN Control

extern TIM_HandleTypeDef htim4;

void dcmotor_pwm_control(void)
{
	if (get_button(BUTTON0_GPIO_Port, BUTTON0_Pin, 0) == BUTTON_PRESS)
	{
		HAL_GPIO_TogglePin(GPIOB, GPIO_PIN_0); // LED1

		if (pwm_start_flag)
		{
			pwm_start_flag = 0;
			HAL_TIM_PWM_Start(&htim4, TIM_CHANNEL_1);
		}
		else
		{
			pwm_start_flag = 1;
			HAL_TIM_PWM_Stop(&htim4, TIM_CHANNEL_1);
		}
	}

	// speed_up
	if (get_button(BUTTON1_GPIO_Port, BUTTON1_Pin, 1) == BUTTON_PRESS)
	{
		HAL_GPIO_TogglePin(GPIOB, GPIO_PIN_7); // LED2
		CCR_UP_value = __HAL_TIM_GET_COMPARE(&htim4, TIM_CHANNEL_1);
		CCR_UP_value += 10;

		if (CCR_UP_value > 100) CCR_UP_value = 100;

		__HAL_TIM_SET_COMPARE(&htim4, TIM_CHANNEL_1, CCR_UP_value);
	}

	// speed_down
	if (get_button(BUTTON2_GPIO_Port, BUTTON2_Pin, 2) == BUTTON_PRESS)
	{
		HAL_GPIO_TogglePin(GPIOB, GPIO_PIN_14); // LED3
		CCR_DOWN_value = __HAL_TIM_GET_COMPARE(&htim4, TIM_CHANNEL_1);
		CCR_DOWN_value -= 10;

		if (CCR_DOWN_value < 60) CCR_DOWN_value = 60;

		__HAL_TIM_SET_COMPARE(&htim4, TIM_CHANNEL_1, CCR_DOWN_value);
	}

	if (get_button(BUTTON3_GPIO_Port, BUTTON3_Pin, 3) == BUTTON_PRESS)
	{
		// led 전부 다 토글
		HAL_GPIO_TogglePin(GPIOB, GPIO_PIN_0);
		HAL_GPIO_TogglePin(GPIOB, GPIO_PIN_7);
		HAL_GPIO_TogglePin(GPIOB, GPIO_PIN_14);

		if (pin_8 == 1 && pin_7 == 0)
		{
			pin_8 = 0;
			pin_7 = 1;
			HAL_GPIO_WritePin(GPIOE, GPIO_PIN_8, pin_8);
			HAL_GPIO_WritePin(GPIOE, GPIO_PIN_7, pin_7);
		}

		else if (pin_8 == 0 && pin_7 == 1)
		{
			pin_8 = 1;
			pin_7 = 0;
			HAL_GPIO_WritePin(GPIOE, GPIO_PIN_8, pin_8);
			HAL_GPIO_WritePin(GPIOE, GPIO_PIN_7, pin_7);
		}
	}
}

void init_dcmotor_pwm(void)
{
	HAL_GPIO_WritePin(GPIOE, GPIO_PIN_8, pin_8);
	HAL_GPIO_WritePin(GPIOE, GPIO_PIN_7, pin_7);
}
