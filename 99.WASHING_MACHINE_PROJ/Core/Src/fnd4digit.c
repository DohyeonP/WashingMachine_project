#include "main.h"
#include "fnd4digit.h"
#include "button.h"

extern volatile int fnd1ms_counter;  // fnd1ms

uint16_t FND_digit[4] =
{
	FND_d1, FND_d10, FND_d100, FND_d1000
};

uint8_t FND_forward_running[8] =
{
	FND_a, FND_a, FND_b, FND_c, FND_d, FND_d, FND_e, FND_f
};

uint8_t FND_reverse_running[8] =
{
	FND_a, FND_f, FND_e, FND_d, FND_d, FND_c, FND_b, FND_a
};

uint32_t FND_font[10] =
{
  FND_a|FND_b|FND_c|FND_d|FND_e|FND_f,   // 0
  FND_b|FND_c,                           // 1
  FND_a|FND_b|FND_d|FND_e|FND_g,         // 2
  FND_a|FND_b|FND_c|FND_d|FND_g,         // 3
  FND_b|FND_c|FND_f|FND_g,   // 4
  FND_a|FND_c|FND_d|FND_f|FND_g,  // 5
  FND_a|FND_c|FND_d|FND_e|FND_f|FND_g,  // 6
  FND_a|FND_b|FND_c,      // 7
  FND_a|FND_b|FND_c|FND_d|FND_e|FND_f|FND_g,   // 8
  FND_a|FND_b|FND_c|FND_d|FND_f|FND_g   // 9
};

uint16_t FND[4];    // FND에 쓰기 위한 값을 준비하는 변수

void fnd4digit_sec_clock(void)
{
	static unsigned int value=0;   // 1초가 되었을때 up count
	static unsigned int msec=0;    // ms counter
	static int i=0;     // FND position indicator

	// SYSTICK interrupt 방식
	if (fnd1ms_counter >= 2)   // 2ms reached
	{
		msec += 2;   // 2ms
		fnd1ms_counter=0;
		if (msec > 1000)   // 1000ms reached
		{
			msec = 0;
			value++;       // sec count를 증가
			FND_update(value);
		}

		FND4digit_off();
#if 1 // common 애노우드  WCN4-
			HAL_GPIO_WritePin(FND_COM_PORT,FND_digit[i], GPIO_PIN_SET);
			HAL_GPIO_WritePin(FND_DATA_PORT, FND[i], GPIO_PIN_RESET);
#else // common 캐소우드 CL5642AH30
			HAL_GPIO_WritePin(FND_COM_PORT,FND_digit[i], GPIO_PIN_RESET);
			HAL_GPIO_WritePin(FND_DATA_PORT, FND[i], GPIO_PIN_SET);
#endif
		i++;   // 다음 display할 FND를 가리킨다.
		i %= 4;
	}
}

void FND_update(unsigned int value)
{
#if 0
	FND[0] = FND_font[value % 10];
	FND[1] = FND_font[value / 10 % 10];
	FND[2] = FND_font[value / 100 % 10];
	FND[3] = FND_font[value / 1000 % 10];
#else // for washing_machine
	FND[2] = FND_font[value % 10];
	FND[3] = FND_font[value / 10 % 10];
#endif

	return;
}

void FND4digit_off(void)
{
#if 1 // common 에노우드 WCN4-
	HAL_GPIO_WritePin(FND_COM_PORT, GPIO_PIN_8|GPIO_PIN_9|GPIO_PIN_10|GPIO_PIN_11, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(FND_DATA_PORT,FND_font[8]|FND_p, GPIO_PIN_SET);
#else // common 캐소우드 CL5642AH30
	HAL_GPIO_WritePin(FND_COM_PORT, GPIO_PIN_8|GPIO_PIN_9|GPIO_PIN_10|GPIO_PIN_11, GPIO_PIN_SET);
	HAL_GPIO_WritePin(FND_DATA_PORT,FND_font[8]|FND_p, GPIO_PIN_RESET);
#endif
	return;
}
void FND4digit_on(void)
{
#if 1  // common 에노우드
	HAL_GPIO_WritePin(FND_COM_PORT, GPIO_PIN_8|GPIO_PIN_9|GPIO_PIN_10|GPIO_PIN_11, GPIO_PIN_SET);
	HAL_GPIO_WritePin(FND_DATA_PORT,FND_font[8]|FND_p, GPIO_PIN_RESET);
#else  // CL5642AH30
	HAL_GPIO_WritePin(FND_COM_PORT, GPIO_PIN_8|GPIO_PIN_9|GPIO_PIN_10|GPIO_PIN_11, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(FND_DATA_PORT,FND_font[8]|FND_p, GPIO_PIN_SET);
#endif
	return;
}











