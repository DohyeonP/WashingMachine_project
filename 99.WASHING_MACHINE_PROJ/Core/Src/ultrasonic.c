#include "ultrasonic.h"
#include "main.h"
#include "i2c_lcd.h"

volatile int distance; //거리를 측정한 펄스 갯수를 저장하는 변수
volatile int ic_finish_flag = 0; // 초음파 거리 측정 완료 indicator 변수
volatile uint8_t is_first_capture = 0; // 0은 상승엣지, 1은 하강엣지
volatile int ULTRASONIC_on_flag = 1;
extern volatile int TIM10_10ms_ultrasonic;
extern uint8_t lcd_display_mode_flag;

// Drivers/stm32f4xx_HAL_Driver/Src/stm32f4xx_hal_tim.c
// HAL_TIM_IC_CaptureCallback룰 잘라내기 해서 이 곳으로 가져온다.
// 초음파 센서의 ECHO 핀의 상승 엣지와 하강 엣지 발생 시 이 곳으로 들어온다.
void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef *htim);

void ultrasonic_processing(void);
extern void delay_us(unsigned long us);
extern void lcd_string(uint8_t *str);
extern void move_cursor(uint8_t row, uint8_t column);

void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef *htim)
{
	if (htim->Instance == TIM3)
	{
		if (is_first_capture == 0) // 상승엣지
		{
			__HAL_TIM_SET_COUNTER(htim, 0); // clear H/W counter
			is_first_capture = 1;
		}

		else // 하강엣지
		{
			is_first_capture = 0;
			distance = HAL_TIM_ReadCapturedValue(htim, TIM_CHANNEL_1); // 현재까지 count한 펄스 수를 읽어온다.
			ic_finish_flag = 1;
		}
	}
}

int dist;
char ultra_buff[20];

void ultrasonic_processing(void)
{
	if (ULTRASONIC_on_flag)
	{
		if (TIM10_10ms_ultrasonic >= 100) // 1초가 되면
		{
			TIM10_10ms_ultrasonic = 0;
			make_trigger();
			if (ic_finish_flag) // 초음파 측정이 완료가 되면
			{
				ic_finish_flag = 0;
				dist = distance;
				dist = dist * 0.034 / 2; // 1us 가 0.034cm 를 이동하기 때문
										// 나누기 2는 왕복거리를 측정하기 때문에 편도 거리로 만들기 위해서 2로 나눔
//				printf("dist : %dcm\n", dist);
/*
				if (lcd_display_mode_flag == 2)
				{
					lcd_command(CLEAR_DISPLAY);
					move_cursor(0, 0);
					lcd_string("Ultra Sonic");

					sprintf(ultra_buff, "%dcm         ", dist);
					move_cursor(1, 0);
					lcd_string(ultra_buff);
				}
*/
			}
		}
	}
}

void make_trigger(void)
{
	HAL_GPIO_WritePin(ULTRASONIC_TRIGGER_GPIO_Port, ULTRASONIC_TRIGGER_Pin, GPIO_PIN_RESET);
	delay_us(2);
	HAL_GPIO_WritePin(ULTRASONIC_TRIGGER_GPIO_Port, ULTRASONIC_TRIGGER_Pin, GPIO_PIN_SET);
	delay_us(10);
	HAL_GPIO_WritePin(ULTRASONIC_TRIGGER_GPIO_Port, ULTRASONIC_TRIGGER_Pin, GPIO_PIN_RESET);
}
