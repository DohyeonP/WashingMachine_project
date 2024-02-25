#include "servo_motor.h"
#include "main.h"

extern TIM_HandleTypeDef htim2;

void servo_motor_test_main(void);

// 84mHz/1680 = 50kHz
// T = 1/f = 1/50000 = 20us
// 2ms(180도) 	: 1주기(20us)를 100개를 count하면 2ms가 나온다
// 1.5ms(90도)	: 1주기(20us)를 75개 count하면 1.5ms가 나온다.
// 1ms(0도)		: 1주기(20us)를 50개 count하면 1ms가 나온다.
void servo_motor_test_main(void)
{
	while(1)
	{
		//180도 회전
		__HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_1, 120); // 180도
		HAL_Delay(1000);

//		//90도 회전
		__HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_1, 90); // 90도
		HAL_Delay(1000);
//
		//0도 회전
		__HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_1, 40); // 0도
		HAL_Delay(1000);
	}
}
