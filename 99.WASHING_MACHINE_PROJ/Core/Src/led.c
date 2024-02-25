#include "led.h"
#include "main.h"
#include "button.h"

#define LED_ALL_ON 0
#define LED_ALL_OFF 1
#define FLOWER_ON 2
#define FLOWER_OFF 3
#define LED_KEEP_ON_UP 4
#define LED_KEEP_ON_DOWN 5
#define LED_ON_UP 6
#define LED_ON_DOWN 7

void led_main(void);
void led_init(void);

// sequential progress
void led_all_on(void);
void led_all_off(void);
void led_on_up(void);
void led_on_down(void);
void led_keep_on_up(void);
void led_keep_on_down(void);
void flower_on(void);
void flower_off(void);
void button0_toggle(void);
void led_off(void);
void led_on(void);

// button press progress
void button_led_all_on(void);
void button_led_all_off(void);
void button_flower_on(void);
void button_flower_off(void);
void button_led_keep_on_up(void);
void button_led_keep_on_down(void);
void button_led_on_up(void);
void button_led_on_down(void);

extern volatile int t1ms_count;
int func_index = 0;
int i = 0;
int button0_count = 1;

void led_main(void)
{

	while(1)
	{
#if 0 // sequential progress -> 과제 1번
		void (*fp[]) ()=
		{
			led_all_on,
			led_all_off,
			flower_on,
			flower_off,
			led_keep_on_up,
			led_keep_on_down,
			led_on_up,
			led_on_down
		};

		fp[func_index] ();

#else // button press progress -> 과제 2번
		button0_toggle();
#endif
	}
}

void flower_on(void)
{
	led_all_off();

	int delay = 200;

	for (int i = 0; i < 4; i++)
	{
		HAL_GPIO_WritePin(GPIOD, 0x10 << i, 1);
		HAL_GPIO_WritePin(GPIOD, 0x08 >> i, 1);
		HAL_Delay(delay);
		delay += 200;
	}
}

void flower_off(void)
{
	int delay = 1000;

	for (int i = 0; i < 4; i++)
	{
		HAL_GPIO_WritePin(GPIOD, 0x01 << i, 0);
		HAL_GPIO_WritePin(GPIOD, 0x80 >> i, 0);
		HAL_Delay(delay);
		delay -= 200;
	}

	led_all_off();
}

void led_all_on(void)
{
	HAL_GPIO_WritePin(GPIOD, GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3 |
			GPIO_PIN_4 | GPIO_PIN_5 | GPIO_PIN_6  | GPIO_PIN_7 | GPIO_PIN_0 , 1);
}

void led_all_off(void)
{
	HAL_GPIO_WritePin(GPIOD, GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3 |
			GPIO_PIN_4 | GPIO_PIN_5 | GPIO_PIN_6  | GPIO_PIN_7 | GPIO_PIN_0 , 0);
}

void led_keep_on_up(void)
{
	led_all_off();

	for (int i = 0; i < 8; i++)
	{
		HAL_GPIO_WritePin(GPIOD, 0x01 << i, 1);

		HAL_Delay(200);
	}
}

void led_on_up(void)
{
	for (int i = 0; i < 8; i++)
	{
		led_all_off();

		HAL_GPIO_WritePin(GPIOD, 0x01 << i, 1);

		HAL_Delay(200);
	}
}

void led_keep_on_down(void)
{
	led_all_off();

	for (int i = 0; i < 8; i++)
	{
		HAL_GPIO_WritePin(GPIOD, 0x80 >> i, 1);

		HAL_Delay(200);
	}
}

void led_on_down(void)
{
	for (int i = 0; i < 8; i++)
	{
		led_all_off();

		HAL_GPIO_WritePin(GPIOD, 0x80 >> i, 1);

		HAL_Delay(200);
	}
}

//void flower_on(void)
//{
//	if (t1ms_count >= 200)
//	{
//		t1ms_count = 0;
//		HAL_GPIO_WritePin(GPIOD, 0x10 << i, 1);
//		HAL_GPIO_WritePin(GPIOD, 0x08 >> i, 1);
//		i++;
//		if (i >= 4)
//		{
//			i = 0;
//			func_index = FLOWER_OFF;
//		}
//	}
//}
//
//void flower_off(void)
//{
//	if (t1ms_count >= 200)
//	{
//		t1ms_count = 0;
//		HAL_GPIO_WritePin(GPIOD, 0x01 << i, 0);
//		HAL_GPIO_WritePin(GPIOD, 0x80 >> i, 0);
//		i++;
//		if (i >= 4)
//		{
//			i = 0;
//			func_index = LED_KEEP_ON_UP;
//		}
//	}
//}
//
//void led_all_on(void)
//{
//	if (t1ms_count >= 200)
//	{
//		t1ms_count = 0;
//		HAL_GPIO_WritePin(GPIOD, 0xff, 1);
//		func_index = LED_ALL_OFF;
//	}
//}
//
//void led_all_off(void)
//{
//	if (t1ms_count >= 200)
//	{
//		t1ms_count = 0;
//		HAL_GPIO_WritePin(GPIOD, 0xff, 0);
//		func_index = FLOWER_ON;
//	}
//}
//
//void led_keep_on_up(void)
//{
//	if (t1ms_count >= 200)
//	{
//		if (i == 0) led_off();
//		t1ms_count = 0;
//		HAL_GPIO_WritePin(GPIOD, 0x01 << i, 1);
//		i++;
//		if (i >= 8)
//		{
//			i = 0;
//			func_index = LED_KEEP_ON_DOWN;
//		}
//	}
//}
//
//void led_on_up(void)
//{
//	if (t1ms_count >= 200)
//	{
//		t1ms_count = 0;
//		led_off();
//		HAL_GPIO_WritePin(GPIOD, 0x01 << i, 1);
//		i++;
//		if (i >= 8)
//		{
//			i = 0;
//			func_index = LED_ON_DOWN;
//		}
//	}
//}
//
//void led_keep_on_down(void)
//{
//	if (t1ms_count >= 200)
//	{
//		if (i == 0) led_off();
//		t1ms_count = 0;
//		HAL_GPIO_WritePin(GPIOD, 0x80 >> i, 1);
//		i++;
//		if (i >= 8)
//		{
//			i = 0;
//			func_index = LED_ON_UP;
//		}
//	}
//}
//
//void led_on_down(void)
//{
//	if (t1ms_count >= 200)
//	{
//		t1ms_count = 0;
//		led_off();
//		HAL_GPIO_WritePin(GPIOD, 0x80 >> i, 1);
//		i++;
//		if (i >= 8)
//		{
//			i = 0;
//			func_index = LED_ALL_ON;
//		}
//	}
//}

void (*fbp[]) ()=
{
	NULL,
	button_led_all_on,
	button_led_all_off,
	button_flower_on,
	button_flower_off,
	button_led_keep_on_up,
	button_led_keep_on_down,
	button_led_on_up,
	button_led_on_down
};

void button0_toggle(void)
{

	if (t1ms_count >= 200)
	{
		t1ms_count = 0;
		fbp[button0_count] ();
	}

	if (get_button(BUTTON0_GPIO_Port, BUTTON0_Pin, 0) == BUTTON_PRESS)
	{
		button0_count++;
		if (button0_count >= 9) button0_count = 1;

		if (button0_count == 3) i = 0;

		if (button0_count == 4)
		{
			i = 0;
			led_on();
		}

		if (button0_count == 5 || button0_count == 6 ||
			button0_count == 7 || button0_count == 8)
		{
			led_off();
			i = 0;
		}
	}
}

void button_led_all_on(void)
{
	HAL_GPIO_WritePin(GPIOD, 0xff, 1);
}

void button_led_all_off(void)
{
	HAL_GPIO_WritePin(GPIOD, 0xff, 0);
}

void button_flower_on(void)
{
	if (i == 0) led_off();
	HAL_GPIO_WritePin(GPIOD, 0x10 << i, 1);
	HAL_GPIO_WritePin(GPIOD, 0x08 >> i, 1);
	i++;
	if (i >= 4) i = 0;
}

void button_flower_off(void)
{
	HAL_GPIO_WritePin(GPIOD, 0x01 << i, 0);
	HAL_GPIO_WritePin(GPIOD, 0x80 >> i, 0);
	i++;
	if (i >= 4)
	{
		i = 0;
		led_on();
	}
}

void button_led_keep_on_up(void)
{
	if (i == 0) led_off();
	HAL_GPIO_WritePin(GPIOD, 0x01 << i, 1);
	i++;
	if (i >= 8) i = 0;
}

void button_led_keep_on_down(void)
{
	if (i == 0) led_off();
	HAL_GPIO_WritePin(GPIOD, 0x80 >> i, 1);
	i++;
	if (i >= 8) i = 0;
}

void button_led_on_up(void)
{
	led_off();
	HAL_GPIO_WritePin(GPIOD, 0x01 << i, 1);
	i++;
	if (i >= 8) i = 0;
}

void button_led_on_down(void)
{
	led_off();
	HAL_GPIO_WritePin(GPIOD, 0x80 >> i, 1);
	i++;
	if (i >= 8) i = 0;
}

void led_off(void)
{
	HAL_GPIO_WritePin(GPIOD, 0xff, 0);
}

void led_on(void)
{
	HAL_GPIO_WritePin(GPIOD, 0xff, 1);
}

void led_init(void)
{
	led_off();
}
