#include "washing_machine.h"
#include "i2c_lcd.h"
#include "button.h"
#include "fnd4digit.h"
#include "buzzer.h"
#include "ultrasonic.h"

/*
#define FORWARD_SPIN 0
#define REVERSE_SPIN 1
#define MULTIPLE_SPIN 2
*/

/*** variables declaration ***/
// flags
uint8_t forward_spin_flag;
uint8_t reverse_spin_flag;
uint8_t buzzer_flag;
uint8_t washing_machine_flag;
uint8_t alret_flag;

uint8_t laundry_flag;
uint8_t laundry_water_level_control_flag;
uint8_t laundry_timer_setting_flag;
uint8_t laundry_timer_clock_flag;
uint8_t laundry_run_for_time_set_flag;
uint8_t laundry_run_for_auto_flag;
uint8_t laundry_timer_setting_complete_flag;

uint8_t rinsing_flag;
uint8_t rinsing_timer_setting_flag;
uint8_t rinsing_water_level_control_flag;
uint8_t rinsing_timer_clock_flag;
uint8_t rinsing_run_for_time_set_flag;
uint8_t rinsing_run_for_auto_flag;
uint8_t rinsing_timer_setting_complete_flag;

uint8_t dehydration_flag;
uint8_t dehydration_speed_up_flag;
uint8_t dehydration_speed_down_flag;
uint8_t dehydration_start_flag;
uint8_t dehydration_timer_clock_flag;

// timer
uint8_t laundry_timer;
uint8_t rinsing_timer;
uint8_t dehydration_timer;
extern volatile int laundry_timer_counter_10ms;
extern volatile int rinsing_timer_counter_10ms;
extern volatile int dehydration_timer_counter_10ms;
extern volatile int fnd1ms_counter_timer; // using fnd counter
extern volatile int fnd1ms_counter_running;
extern volatile int led_blink_counter_1ms; // using led blink

// led_bit
uint8_t led_bit;

// buzzer
extern TIM_HandleTypeDef htim5;
extern int divide_freq;
extern unsigned int happy_birthday[];
extern unsigned int duration[];

// pwm motor (fan)
uint8_t pin_8_state;
uint8_t pin_7_state;
uint8_t speed_up_value;
uint8_t speed_down_value;
extern TIM_HandleTypeDef htim4;

//FND
extern uint8_t FND_forward_running[8];
extern uint8_t FND_reverse_running[8];
extern uint16_t FND_digit[4];
extern uint16_t FND[4];

// Ultra Sonic
extern int dist;

/*** end variables declaration ***/

/*** function declaration ***/
// main
void washing_machine_start_lcd(void);
void washing_machine_choice_lcd(void);
void washing_machine_processing(void);

// washing_machine
void washing_machine_init(void);
void washing_machine(void);
void washing_machine_end_lcd(void);
void pwm_init(void);
void buzzer_start(void);
void buzzer_alret(void);
void all_reset(void);

// laundry (세탁)
void laundry_flag_reset(void);
void laundry_choice_lcd(void);
void laundry_run(void);
void laundry_water_level_control(void);
void laundry_water_level_control_lcd(void);
void laundry_timer_setting_lcd(void);
void laundry_timer_setting(void);
void laundry_timer_setting_store_lcd(void);
void laundry_timer_clock(void);
void laundry_run_for_time_set_lcd(void);
void laundry_run_for_time_set(void);
void laundry_spinning_fan(void);
void laundry_run_for_time_finish_lcd(void);
void laundry_run_for_auto_lcd(void);
void laundry_run_for_auto(void);
void laundry_run_auto_finish_lcd(void);
void laundry_time_has_not_been_set_lcd(void);

// rinsing (헹굼)
void rinsing_flag_reset(void);
void rinsing_choice_lcd(void);
void rinsing_run(void);
void rinsing_water_level_control(void);
void rinsing_water_level_control_lcd(void);
void rinsing_timer_setting_lcd(void);
void rinsing_timer_setting(void);
void rinsing_timer_setting_store_lcd(void);
void rinsing_timer_clock(void);
void rinsing_run_for_time_set_lcd(void);
void rinsing_run_for_time_set(void);
void rinsing_spinning_fan(void);
void rinsing_run_for_time_finish_lcd(void);
void rinsing_run_for_auto_lcd(void);
void rinsing_run_for_auto(void);
void rinsing_run_auto_finish_lcd(void);
void rinsing_time_has_not_been_set_lcd(void);

// dehydration (탈수)
void dehydration_run(void);
void dehydration_begin_lcd(void);
void dehydration_speed_up_lcd(void);
void dehydration_speed_down_lcd(void);
void dehydration_start_lcd(void);
void dehydration_stop_lcd(void);
void dehydration_speed_up(void);
void dehydration_speed_down(void);
void dehydration_start(void);
void dehydration_stop(void);
void dehydration_speed_up_lcd(void);
void dehydration_speed_down_lcd(void);
void dehydration_store_lcd(void);
void dehydration_finish_lcd(void);
void dehydration_spinning_fan(void);
void dehydration_timer_clock(void);
void dehydration_flag_reset(void);

// buzzer
void buzzer_init(void);
extern void noTone(void);

// FND
extern void FND4digit_off(void);
extern void FND_update(unsigned int value);

// LCD
extern void i2c_lcd_init(void);
/*** end function declaration ***/

/*** function definition ***/
// main
void all_reset(void)
{
	laundry_flag_reset();
	rinsing_flag_reset();
	dehydration_flag_reset();
	washing_machine_flag = 0;
	forward_spin_flag = 0;
	reverse_spin_flag = 0;
	buzzer_flag = 0;
	alret_flag=0;

	pwm_init();
	buzzer_init();
}
void washing_machine_processing(void)
{
	// wasing_machine
	washing_machine();
	buzzer_start();
	buzzer_alret();

	// laundry
	laundry_run();
	laundry_water_level_control();
	laundry_timer_setting();
	laundry_timer_clock();
	laundry_run_for_time_set();
	laundry_run_for_auto();

	// rinsing
	rinsing_run();
	rinsing_water_level_control();
	rinsing_timer_setting();
	rinsing_timer_clock();
	rinsing_run_for_time_set();
	rinsing_run_for_auto();

	// dehydration
	dehydration_run();
	dehydration_speed_up();
	dehydration_speed_down();
	dehydration_spinning_fan();
	dehydration_timer_clock();
//	dehydration_start();
}
// end main

// washing_machine
void buzzer_alret(void)
{
	if(alret_flag==1)
	{
		__HAL_TIM_SET_AUTORELOAD(&htim5, divide_freq / happy_birthday[0]);
		__HAL_TIM_SET_COMPARE(&htim5, TIM_CHANNEL_4, divide_freq / happy_birthday[0] / 2);
		if(dist > 5)
		{
			alret_flag=0;
		}
	}
}

void washing_machine_init(void)
{
	// washing machine flags
	washing_machine_flag = 1;

	// laundry flags
	laundry_flag_reset();

	// rinsing flags
	rinsing_flag_reset();

	// dehydration flags
	dehydration_flag_reset();

	// timer
	laundry_timer = 0;
	rinsing_timer = 0;
	dehydration_timer = 0;

	// led_bit
	led_bit = 0;

	// init state
	FND4digit_off();
	pwm_init();
	buzzer_init();

	// spin
	forward_spin_flag = 0;
	reverse_spin_flag = 0;

	// buzzer
	buzzer_flag = 0;
	alret_flag=0;
}

void buzzer_init(void)
{
	HAL_TIM_PWM_Stop(&htim5, TIM_CHANNEL_4);
}

void buzzer_start(void)
{
	if (buzzer_flag==1)
	{
		HAL_TIM_PWM_Start(&htim5, TIM_CHANNEL_4);

	    for (int i=0; i < 25; i++)
	    {
			__HAL_TIM_SET_AUTORELOAD(&htim5, divide_freq / happy_birthday[i]);
			__HAL_TIM_SET_COMPARE(&htim5, TIM_CHANNEL_4, divide_freq / happy_birthday[i] / 2);
			HAL_Delay(300*duration[i]);
			noTone();
	    }
	    buzzer_flag = 0;
	    HAL_TIM_PWM_Stop(&htim5, TIM_CHANNEL_4);
	    washing_machine_choice_lcd();
	}
}

void pwm_init(void)
{
	pin_8_state = 1;
	pin_7_state = 1;
	HAL_TIM_PWM_Stop(&htim4, TIM_CHANNEL_1);
}

void washing_machine(void)
{
	if (washing_machine_flag==1)
	{
		// 세탁
		if (get_button(BUTTON0_GPIO_Port, BUTTON0_Pin, 0) == BUTTON_PRESS)
		{
			laundry_choice_lcd();
			laundry_flag = 1;
			washing_machine_flag = 0;
			rinsing_flag_reset();
			dehydration_flag_reset();
		}
		// 헹굼
		if (get_button(BUTTON1_GPIO_Port, BUTTON1_Pin, 1) == BUTTON_PRESS)
		{
			rinsing_choice_lcd();
			rinsing_flag = 1;
			washing_machine_flag = 0;
			laundry_flag_reset();
			dehydration_flag_reset();
		}
		// 탈수
		if (get_button(BUTTON2_GPIO_Port, BUTTON2_Pin, 2) == BUTTON_PRESS)
		{
			dehydration_begin_lcd();
			dehydration_flag = 1;
			washing_machine_flag = 0;
			rinsing_flag_reset();
			laundry_flag_reset();
		}
		//세탁기 종료
		if (get_button(BUTTON3_GPIO_Port, BUTTON3_Pin, 3) == BUTTON_PRESS)
		{
			washing_machine_end_lcd();
			washing_machine_flag = 0;
			laundry_flag_reset();
			rinsing_flag_reset();
			dehydration_flag_reset();
		}
	}
	else
	{
		if (get_button(BUTTON3_GPIO_Port, BUTTON3_Pin, 3) == BUTTON_PRESS)
		{
			washing_machine_choice_lcd();
			washing_machine_flag = 1;
		}
	}
}
// end washing_machine

// laundry

void laundry_flag_reset(void)
{
	laundry_flag = 0;
	laundry_water_level_control_flag = 0;
	laundry_timer_setting_flag = 0;
	laundry_timer_clock_flag = 0;
	laundry_run_for_time_set_flag = 0;
	laundry_run_for_auto_flag = 0;
	laundry_timer_setting_complete_flag = 0;
	forward_spin_flag = 0;
}


void laundry_run(void)
{
	if (laundry_flag==1)
	{
		// Water level control (using LED BAR)
		if (get_button(BUTTON0_GPIO_Port, BUTTON0_Pin, 0) == BUTTON_PRESS)
		{
			laundry_water_level_control_lcd();
			all_reset();
			laundry_water_level_control_flag = 1;
		}
		// Timer setting
		if (get_button(BUTTON1_GPIO_Port, BUTTON1_Pin, 1) == BUTTON_PRESS)
		{
			laundry_timer_setting_lcd();
			all_reset();
			laundry_timer_setting_flag = 1;
			laundry_timer_clock_flag = 1;
		}
		// run for time set
		if (get_button(BUTTON2_GPIO_Port, BUTTON2_Pin, 2) == BUTTON_PRESS)
		{
			if(laundry_timer_setting_complete_flag)
			{
				laundry_run_for_time_set_lcd();
				all_reset();
				laundry_run_for_time_set_flag = 1;
				laundry_timer_clock_flag = 1;
				forward_spin_flag = 1;
				laundry_spinning_fan();
				laundry_timer_counter_10ms = 0;
			}
			else
			{
				laundry_time_has_not_been_set_lcd();
				laundry_choice_lcd();
			}
		}
		// auto timer set & run
		if (get_button(BUTTON3_GPIO_Port, BUTTON3_Pin, 3) == BUTTON_PRESS)
		{
			laundry_run_for_auto_lcd();
			laundry_timer = 30;
			all_reset();
			laundry_spinning_fan();
			laundry_timer_counter_10ms = 0;
			laundry_run_for_auto_flag = 1;
			laundry_timer_clock_flag = 1;
			forward_spin_flag = 1;
		}
	}
}

void laundry_water_level_control(void)
{
	if(laundry_water_level_control_flag==1)
	{
		// +10
		if (get_button(BUTTON0_GPIO_Port, BUTTON0_Pin, 0) == BUTTON_PRESS)
		{
			if (led_bit <= 0)
			{
				led_bit = 1;
				HAL_GPIO_WritePin(GPIOD, led_bit, 1);
			}
			else if (led_bit >= 0x80)
			{
				led_bit = 0x80;
				HAL_GPIO_WritePin(GPIOD, led_bit, 1);
			}
			else
			{
				led_bit <<= 1;
				HAL_GPIO_WritePin(GPIOD, led_bit, 1);
			}
		}
		// +20
		if (get_button(BUTTON1_GPIO_Port, BUTTON1_Pin, 1) == BUTTON_PRESS)
		{
			if (led_bit <= 0)
			{
				led_bit = 2;
				HAL_GPIO_WritePin(GPIOD, led_bit, 1);
				HAL_GPIO_WritePin(GPIOD, led_bit>>1, 1);
			}
			else if (led_bit >= 0x20)
			{
				led_bit = 0x80;
				HAL_GPIO_WritePin(GPIOD, led_bit, 1);
				HAL_GPIO_WritePin(GPIOD, led_bit>>1, 1);
			}
			else
			{
				led_bit <<= 2;
				HAL_GPIO_WritePin(GPIOD, led_bit, 1);
				HAL_GPIO_WritePin(GPIOD, led_bit>>1, 1);
			}
		}
		// -10
		if (get_button(BUTTON2_GPIO_Port, BUTTON2_Pin, 2) == BUTTON_PRESS)
		{
			HAL_GPIO_WritePin(GPIOD, led_bit, 0);
			led_bit >>= 1;
			if (led_bit <= 0) led_bit = 0;
		}
		// store
		if (get_button(BUTTON3_GPIO_Port, BUTTON3_Pin, 3) == BUTTON_PRESS)
		{
			all_reset();
			laundry_flag = 1;
			laundry_choice_lcd();
		}
	}
}

void laundry_timer_setting(void)
{
	if (laundry_timer_setting_flag==1)
	{
		// +1
		if (get_button(BUTTON0_GPIO_Port, BUTTON0_Pin, 0) == BUTTON_PRESS)
		{
			laundry_timer++;
			if (laundry_timer > 99) laundry_timer = 99;
		}
		// +10
		if (get_button(BUTTON1_GPIO_Port, BUTTON1_Pin, 1) == BUTTON_PRESS)
		{
			laundry_timer += 10;
			if (laundry_timer > 99) laundry_timer = 99;
		}
		// -10
		if (get_button(BUTTON2_GPIO_Port, BUTTON2_Pin, 2) == BUTTON_PRESS)
		{
			if (laundry_timer <= 10 )
			{
				laundry_timer = 0;
			}
			else
			{
				laundry_timer -= 10;
			}
		}
		// store
		if (get_button(BUTTON3_GPIO_Port, BUTTON3_Pin, 3) == BUTTON_PRESS)
		{
			all_reset();
//			laundry_timer_setting_store_lcd();
			laundry_timer_setting_complete_flag = 1;
			laundry_flag = 1;
			laundry_choice_lcd();
		}
	}
}

void laundry_timer_clock(void)
{
	if (laundry_timer_clock_flag==1)
	{
		static unsigned int ms_counter=0;    // ms counter

		static int position=0;     // FND position indicator
		static int position_running=0;

		// SYSTICK interrupt 방식
		if (fnd1ms_counter_timer >= 2)   // 2ms reached
		{
			ms_counter += 2;   // 2ms
			fnd1ms_counter_timer=0;
			if (ms_counter > 1000)   // 1000ms reached
			{
				ms_counter = 0;
				FND_update(laundry_timer);
				position_running++;
				position_running %= 8;
			}

			FND4digit_off();
			if (position == 2 || position == 3)
			{
				HAL_GPIO_WritePin(FND_COM_PORT,FND_digit[position], GPIO_PIN_SET);
				HAL_GPIO_WritePin(FND_DATA_PORT, FND[position], GPIO_PIN_RESET);
			}
			else
			{
				if (forward_spin_flag)
				{
					if (position_running >= 1 && position_running <= 4)
					{
						HAL_GPIO_WritePin(FND_COM_PORT,FND_d1, GPIO_PIN_SET);
						HAL_GPIO_WritePin(FND_DATA_PORT, FND_forward_running[position_running], GPIO_PIN_RESET);
					}
					else
					{
						HAL_GPIO_WritePin(FND_COM_PORT,FND_d10, GPIO_PIN_SET);
						HAL_GPIO_WritePin(FND_DATA_PORT, FND_forward_running[position_running], GPIO_PIN_RESET);
					}
				}
			}
			position++;   // 다음 display할 FND를 가리킨다.
			position %= 4;
		}
	}
}

void laundry_run_for_time_set(void)
{
	if (laundry_run_for_time_set_flag==1)
	{
		if (dist <= 5)
		{
			laundry_run_for_time_set_flag=0;
			alret_flag=1;
		}

		// led blink
		if (led_blink_counter_1ms >= 50)
		{
			led_blink_counter_1ms = 0;
			HAL_GPIO_TogglePin(GPIOF, GPIO_PIN_15);
		}

		// 1초마다 laundry_timer 1씩 감소
		if (laundry_timer_counter_10ms >= 100)
		{
			laundry_timer_counter_10ms = 0;
			laundry_timer--;
		}

		// count가 종료되면 헹굼으로 넘어간다.
		if (laundry_timer <= 0)
		{
			laundry_run_for_time_finish_lcd();
			rinsing_choice_lcd();
			all_reset();
			pwm_init();
			washing_machine_flag = 1;
			FND4digit_off();
			HAL_GPIO_WritePin(GPIOF, GPIO_PIN_15, 0);
			buzzer_flag = 1;
		}
	}
}

void laundry_run_for_auto(void)
{
	if (laundry_run_for_auto_flag==1)
	{
		// led blink
		if (led_blink_counter_1ms >= 50)
		{
			led_blink_counter_1ms = 0;
			HAL_GPIO_TogglePin(GPIOF, GPIO_PIN_15);
		}

		// 1초마다 laundry_timer 1씩 감소
		if (laundry_timer_counter_10ms >= 100)
		{
			laundry_timer_counter_10ms = 0;
			laundry_timer--;
		}

		// count가 종료되면 헹굼으로 넘어간다.
		if (laundry_timer <= 0)
		{
			laundry_run_auto_finish_lcd();
			all_reset();
			pwm_init();
			washing_machine_flag = 1;
			FND4digit_off();
			HAL_GPIO_WritePin(GPIOF, GPIO_PIN_15, 0);
			buzzer_flag = 1;
		}
	}
}

void laundry_spinning_fan(void)
{
	// forward spin
	pin_8_state = 1;
	pin_7_state = 0;

	HAL_TIM_PWM_Start(&htim4, TIM_CHANNEL_1);
	HAL_GPIO_WritePin(GPIOE, GPIO_PIN_8, pin_8_state);
	HAL_GPIO_WritePin(GPIOE, GPIO_PIN_7, pin_7_state);
}
// end laundry

// rinsing

 void rinsing_flag_reset(void)
{
	rinsing_flag = 0;
	rinsing_water_level_control_flag = 0;
	rinsing_timer_setting_flag = 0;
	rinsing_timer_clock_flag = 0;
	rinsing_run_for_time_set_flag = 0;
	rinsing_run_for_auto_flag = 0;
	rinsing_timer_setting_complete_flag = 0;
}


void rinsing_run(void)
{
	if (rinsing_flag==1)
	{
		// Water level control (using LED BAR)
		if (get_button(BUTTON0_GPIO_Port, BUTTON0_Pin, 0) == BUTTON_PRESS)
		{
			rinsing_water_level_control_lcd();
			all_reset();
			rinsing_water_level_control_flag = 1;
		}
		// Timer setting
		if (get_button(BUTTON1_GPIO_Port, BUTTON1_Pin, 1) == BUTTON_PRESS)
		{
			rinsing_timer_setting_lcd();
			all_reset();
			rinsing_timer_setting_flag = 1;
			rinsing_timer_clock_flag = 1;
		}
		// run for time set
		if (get_button(BUTTON2_GPIO_Port, BUTTON2_Pin, 2) == BUTTON_PRESS)
		{
			if(rinsing_timer_setting_complete_flag)
			{
				rinsing_run_for_time_set_lcd();
				all_reset();
				rinsing_run_for_time_set_flag = 1;
				rinsing_timer_clock_flag = 1;
				reverse_spin_flag = 1;
				rinsing_spinning_fan();
				rinsing_timer_counter_10ms = 0;
			}
			else
			{
				rinsing_time_has_not_been_set_lcd();
				rinsing_choice_lcd();
			}
		}
		// auto timer set & run
		if (get_button(BUTTON3_GPIO_Port, BUTTON3_Pin, 3) == BUTTON_PRESS)
		{
			washing_machine_init();
			rinsing_run_for_auto_lcd();
			rinsing_timer = 30;
			rinsing_run_for_auto_flag = 1;
			rinsing_timer_clock_flag = 1;
			reverse_spin_flag = 1;
			rinsing_spinning_fan();
			rinsing_timer_counter_10ms = 0;
		}
	}
}

void rinsing_water_level_control(void)
{
	if (rinsing_water_level_control_flag==1)
	{
		// +10
		if (get_button(BUTTON0_GPIO_Port, BUTTON0_Pin, 0) == BUTTON_PRESS)
		{
			if (led_bit <= 0)
			{
				led_bit = 1;
				HAL_GPIO_WritePin(GPIOD, led_bit, 1);
			}
			else if (led_bit >= 0x80)
			{
				led_bit = 0x80;
				HAL_GPIO_WritePin(GPIOD, led_bit, 1);
			}
			else
			{
				led_bit <<= 1;
				HAL_GPIO_WritePin(GPIOD, led_bit, 1);
			}
		}
		// +20
		if (get_button(BUTTON1_GPIO_Port, BUTTON1_Pin, 1) == BUTTON_PRESS)
		{
			if (led_bit <= 0)
			{
				led_bit = 2;
				HAL_GPIO_WritePin(GPIOD, led_bit, 1);
				HAL_GPIO_WritePin(GPIOD, led_bit>>1, 1);
			}
			else if (led_bit >= 0x20)
			{
				led_bit = 0x80;
				HAL_GPIO_WritePin(GPIOD, led_bit, 1);
				HAL_GPIO_WritePin(GPIOD, led_bit>>1, 1);
			}
			else
			{
				led_bit <<= 2;
				HAL_GPIO_WritePin(GPIOD, led_bit, 1);
				HAL_GPIO_WritePin(GPIOD, led_bit>>1, 1);
			}
		}
		// -10
		if (get_button(BUTTON2_GPIO_Port, BUTTON2_Pin, 2) == BUTTON_PRESS)
		{
			HAL_GPIO_WritePin(GPIOD, led_bit, 0);
			led_bit >>= 1;
			if (led_bit <= 0) led_bit = 0;
		}
		// store
		if (get_button(BUTTON3_GPIO_Port, BUTTON3_Pin, 3) == BUTTON_PRESS)
		{
			washing_machine_init();
			rinsing_flag = 1;
			rinsing_choice_lcd();
		}
	}
}

void rinsing_timer_setting(void)
{
	if (rinsing_timer_setting_flag==1)
	{
		// +1
		if (get_button(BUTTON0_GPIO_Port, BUTTON0_Pin, 0) == BUTTON_PRESS)
		{
			rinsing_timer++;
			if (rinsing_timer > 99) rinsing_timer = 99;
		}
		// +10
		if (get_button(BUTTON1_GPIO_Port, BUTTON1_Pin, 1) == BUTTON_PRESS)
		{
			rinsing_timer += 10;
			if (rinsing_timer > 99) rinsing_timer = 99;
		}
		// -10
		if (get_button(BUTTON2_GPIO_Port, BUTTON2_Pin, 2) == BUTTON_PRESS)
		{
			if (rinsing_timer <= 10 )
			{
				rinsing_timer = 0;
			}
			else
			{
				rinsing_timer -= 10;
			}
		}
		// store
		if (get_button(BUTTON3_GPIO_Port, BUTTON3_Pin, 3) == BUTTON_PRESS)
		{
			rinsing_timer_setting_store_lcd();
			washing_machine_init();
			rinsing_timer_setting_complete_flag = 1;
			rinsing_flag = 1;
			rinsing_choice_lcd();
		}
	}
}

void rinsing_timer_clock(void)
{
	if (rinsing_timer_clock_flag==1)
	{
		static unsigned int ms_counter=0;    // ms counter

		static int position=0;     // FND position indicator
		static int position_running=0;

		// SYSTICK interrupt 방식
		if (fnd1ms_counter_timer >= 2)   // 2ms reached
		{
			ms_counter += 2;   // 2ms
			fnd1ms_counter_timer=0;
			if (ms_counter > 1000)   // 1000ms reached
			{
				ms_counter = 0;
				FND_update(rinsing_timer);
				position_running++;
				position_running %= 8;
			}

			FND4digit_off();
			if (position == 2 || position == 3)
			{
				HAL_GPIO_WritePin(FND_COM_PORT,FND_digit[position], GPIO_PIN_SET);
				HAL_GPIO_WritePin(FND_DATA_PORT, FND[position], GPIO_PIN_RESET);
			}
			else
			{
				if (reverse_spin_flag)
				{
					if (position_running <= 7 && position_running >= 4)
					{
						HAL_GPIO_WritePin(FND_COM_PORT,FND_d1, GPIO_PIN_SET);
						HAL_GPIO_WritePin(FND_DATA_PORT, FND_reverse_running[position_running], GPIO_PIN_RESET);
					}
					else
					{
						HAL_GPIO_WritePin(FND_COM_PORT,FND_d10, GPIO_PIN_SET);
						HAL_GPIO_WritePin(FND_DATA_PORT, FND_reverse_running[position_running], GPIO_PIN_RESET);
					}
				}
			}
			position++;   // 다음 display할 FND를 가리킨다.
			position %= 4;
		}
	}
}

void rinsing_run_for_time_set(void)
{
	if (rinsing_run_for_time_set_flag==1)
	{
		// led blink
		if (led_blink_counter_1ms >= 50)
		{
			led_blink_counter_1ms = 0;
			HAL_GPIO_TogglePin(GPIOE, GPIO_PIN_13);
		}

		// 1초마다 laundry_timer 1씩 감소
		if (rinsing_timer_counter_10ms >= 100)
		{
			rinsing_timer_counter_10ms = 0;
			rinsing_timer--;
		}

		// count가 종료되면 헹굼으로 넘어간다.
		if (rinsing_timer <= 0)
		{
			rinsing_run_for_time_finish_lcd();
			washing_machine_init();
			washing_machine_flag = 1;
			HAL_GPIO_WritePin(GPIOE, GPIO_PIN_13, 0);
			buzzer_flag = 1;
		}
	}
}

void rinsing_run_for_auto(void)
{
	if (rinsing_run_for_auto_flag==1)
	{
		// led blink
		if (led_blink_counter_1ms >= 50)
		{
			led_blink_counter_1ms = 0;
			HAL_GPIO_TogglePin(GPIOE, GPIO_PIN_13);
		}

		// 1초마다 laundry_timer 1씩 감소
		if (rinsing_timer_counter_10ms >= 100)
		{
			rinsing_timer_counter_10ms = 0;
			rinsing_timer--;
		}

		// count가 종료되면 헹굼으로 넘어간다.
		if (rinsing_timer <= 0)
		{
			rinsing_run_auto_finish_lcd();
			washing_machine_init();
			washing_machine_flag = 1;
			HAL_GPIO_WritePin(GPIOE, GPIO_PIN_13, 0);
			buzzer_flag = 1;
		}
	}
}

void rinsing_spinning_fan(void)
{
	// reverse spin
	pin_8_state = 0;
	pin_7_state = 1;

	HAL_TIM_PWM_Start(&htim4, TIM_CHANNEL_1);
	HAL_GPIO_WritePin(GPIOE, GPIO_PIN_8, pin_8_state);
	HAL_GPIO_WritePin(GPIOE, GPIO_PIN_7, pin_7_state);
}
// end rinsing

// dehydration

void dehydration_flag_reset(void)
{
	dehydration_flag = 0;
	dehydration_start_flag = 0;
	dehydration_speed_up_flag = 0;
	dehydration_speed_down_flag = 0;
	dehydration_timer_clock_flag = 0;
	reverse_spin_flag = 0;
}


void dehydration_run(void)
{
	if (dehydration_flag==1)
	{
		// speed up
		if (get_button(BUTTON0_GPIO_Port, BUTTON0_Pin, 0) == BUTTON_PRESS)
		{
			dehydration_speed_up_lcd();
			all_reset();
			dehydration_speed_up_flag = 1;
		}
		// speed down
		if (get_button(BUTTON1_GPIO_Port, BUTTON1_Pin, 1) == BUTTON_PRESS)
		{
			dehydration_speed_down_lcd();
			all_reset();
			dehydration_speed_down_flag = 1;
		}
		// start
		if (get_button(BUTTON2_GPIO_Port, BUTTON2_Pin, 2) == BUTTON_PRESS)
		{
			dehydration_start_lcd();
			dehydration_spinning_fan();
			all_reset();
			dehydration_timer = 10;
			reverse_spin_flag = 1;
			dehydration_timer_clock_flag = 1;
			dehydration_start_flag = 1;
		}
		// end dehydration
		if (get_button(BUTTON3_GPIO_Port, BUTTON3_Pin, 3) == BUTTON_PRESS)
		{
			dehydration_stop_lcd();
			all_reset();
			washing_machine_flag = 1;
			washing_machine_choice_lcd();
		}
	}
}

void dehydration_speed_up(void)
{
	if (dehydration_speed_up_flag==1)
	{
		// +1
		if (get_button(BUTTON0_GPIO_Port, BUTTON0_Pin, 0) == BUTTON_PRESS)
		{
			speed_up_value = __HAL_TIM_GET_COMPARE(&htim4, TIM_CHANNEL_1);
			speed_up_value++;
			HAL_GPIO_TogglePin(GPIOB, GPIO_PIN_0); // LED1
			if (speed_up_value > 100) speed_up_value = 100;
		}

		// +5
		if (get_button(BUTTON1_GPIO_Port, BUTTON1_Pin, 1) == BUTTON_PRESS)
		{
			speed_up_value = __HAL_TIM_GET_COMPARE(&htim4, TIM_CHANNEL_1);
			speed_up_value += 5;
			HAL_GPIO_TogglePin(GPIOB, GPIO_PIN_7); // LED2
			if (speed_up_value > 100) speed_up_value = 100;
		}

		// +10
		if (get_button(BUTTON2_GPIO_Port, BUTTON2_Pin, 2) == BUTTON_PRESS)
		{
			speed_up_value = __HAL_TIM_GET_COMPARE(&htim4, TIM_CHANNEL_1);
			speed_up_value += 5;
			HAL_GPIO_TogglePin(GPIOB, GPIO_PIN_14); // LED3
			if (speed_up_value > 100) speed_up_value = 100;
		}

		// store
		if (get_button(BUTTON3_GPIO_Port, BUTTON3_Pin, 3) == BUTTON_PRESS)
		{
			dehydration_store_lcd();
			__HAL_TIM_SET_COMPARE(&htim4, TIM_CHANNEL_1, speed_up_value);
			all_reset();
			dehydration_flag = 1;
			dehydration_begin_lcd();
		}
	}
}

void dehydration_speed_down(void)
{
	if (dehydration_speed_down_flag==1)
	{
		// -1
		if (get_button(BUTTON0_GPIO_Port, BUTTON0_Pin, 0) == BUTTON_PRESS)
		{
			speed_down_value = __HAL_TIM_GET_COMPARE(&htim4, TIM_CHANNEL_1);
			speed_down_value--;
			HAL_GPIO_TogglePin(GPIOB, GPIO_PIN_0); // LED1
			if (speed_down_value < 30) speed_down_value = 30;
		}

		// -5
		if (get_button(BUTTON1_GPIO_Port, BUTTON1_Pin, 1) == BUTTON_PRESS)
		{
			speed_down_value = __HAL_TIM_GET_COMPARE(&htim4, TIM_CHANNEL_1);
			speed_down_value -= 5;
			HAL_GPIO_TogglePin(GPIOB, GPIO_PIN_7); // LED2
			if (speed_down_value < 30) speed_down_value = 30;
		}

		// -10
		if (get_button(BUTTON2_GPIO_Port, BUTTON2_Pin, 2) == BUTTON_PRESS)
		{
			speed_down_value = __HAL_TIM_GET_COMPARE(&htim4, TIM_CHANNEL_1);
			speed_down_value -= 10;
			HAL_GPIO_TogglePin(GPIOB, GPIO_PIN_14); // LED3
			if (speed_down_value < 30) speed_down_value = 30;
		}

		// store
		if (get_button(BUTTON3_GPIO_Port, BUTTON3_Pin, 3) == BUTTON_PRESS)
		{
			dehydration_store_lcd();
			__HAL_TIM_SET_COMPARE(&htim4, TIM_CHANNEL_1, speed_up_value);
			all_reset();
			dehydration_flag = 1;
			dehydration_begin_lcd();
		}
	}
}

void dehydration_spinning_fan(void)
{
	// reverse spin
	pin_8_state = 0;
	pin_7_state = 1;

	HAL_TIM_PWM_Start(&htim4, TIM_CHANNEL_1);
	HAL_GPIO_WritePin(GPIOE, GPIO_PIN_8, pin_8_state);
	HAL_GPIO_WritePin(GPIOE, GPIO_PIN_7, pin_7_state);
}

void dehydration_timer_clock(void)
{
	if (dehydration_timer_clock_flag==1)
	{
		static unsigned int ms_counter=0;    // ms counter

		static int position=0;     // FND position indicator
		static int position_running=0;

		// SYSTICK interrupt 방식
		if (fnd1ms_counter_timer >= 2)   // 2ms reached
		{
			ms_counter += 2;   // 2ms
			fnd1ms_counter_timer=0;
			if (ms_counter > 1000)   // 1000ms reached
			{
				ms_counter = 0;
				FND_update(dehydration_timer);
				position_running++;
				position_running %= 8;
			}

			FND4digit_off();
			if (position == 2 || position == 3)
			{
				HAL_GPIO_WritePin(FND_COM_PORT,FND_digit[position], GPIO_PIN_SET);
				HAL_GPIO_WritePin(FND_DATA_PORT, FND[position], GPIO_PIN_RESET);
			}
			else
			{
				if (reverse_spin_flag)
				{
					if (position_running <= 7 && position_running >= 4)
					{
						HAL_GPIO_WritePin(FND_COM_PORT,FND_d1, GPIO_PIN_SET);
						HAL_GPIO_WritePin(FND_DATA_PORT, FND_reverse_running[position_running], GPIO_PIN_RESET);
					}
					else
					{
						HAL_GPIO_WritePin(FND_COM_PORT,FND_d10, GPIO_PIN_SET);
						HAL_GPIO_WritePin(FND_DATA_PORT, FND_reverse_running[position_running], GPIO_PIN_RESET);
					}
				}
			}
			position++;   // 다음 display할 FND를 가리킨다.
			position %= 4;
		}
	}
}

void dehydration_start(void)
{
	if (dehydration_start_flag==1)
	{
		// led blink
		if (led_blink_counter_1ms >= 50)
		{
			led_blink_counter_1ms = 0;
			HAL_GPIO_TogglePin(GPIOF, GPIO_PIN_14);
		}
		// 1초마다 dehydration_timer 1씩 감소
		if (dehydration_timer_counter_10ms >= 100)
		{
			dehydration_timer_counter_10ms = 0;
			dehydration_timer--;
		}
		// count가 종료되면 끝.
		if (dehydration_timer <= 0)
		{
			dehydration_finish_lcd();
			all_reset();
			pwm_init();
			washing_machine_flag = 1;
			FND4digit_off();
			HAL_GPIO_WritePin(GPIOF, GPIO_PIN_14, 0);
			washing_machine_choice_lcd();
			buzzer_flag = 1;
		}
	}
}
// end dehydration

/*** lcd output ***/


// washing machine
void washing_machine_end_lcd(void)
{
	i2c_lcd_init();
	move_cursor(0, 0);
	lcd_string("Washing Machine");
	move_cursor(1, 0);
	lcd_string("OFF");
}

void washing_machine_choice_lcd(void)
{
	i2c_lcd_init();
	move_cursor(0, 0);
	lcd_string("0 laund 1 rinse");
	move_cursor(1, 0);
	lcd_string("2 dehyd 3 ON/OFF");
}
// end washing machine

// laundry
void laundry_choice_lcd(void)
{
	i2c_lcd_init();
	move_cursor(0, 0);
	lcd_string("0 waterLev 1 TIM");
	move_cursor(1, 0);
	lcd_string("2 RUN 3 RUN AUTO");
}

void laundry_water_level_control_lcd(void)
{
	i2c_lcd_init();
	move_cursor(0, 0);
	lcd_string("0 +10  1 +20");
	move_cursor(1, 0);
	lcd_string("2 -10  3 store");
}

void laundry_timer_setting_lcd(void)
{
	i2c_lcd_init();
	move_cursor(0, 0);
	lcd_string("0 +1  1 +10");
	move_cursor(1, 0);
	lcd_string("2 -1  3 store");
}

void laundry_timer_setting_store_lcd(void)
{
	i2c_lcd_init();
	move_cursor(0, 0);
	lcd_string("Timer Setting");
	move_cursor(1, 0);
	lcd_string("Complete!!!");
	HAL_Delay(1000);
}

void laundry_run_for_time_set_lcd(void)
{
	i2c_lcd_init();
	move_cursor(0, 0);
	lcd_string("Laundry");
	move_cursor(1, 0);
	lcd_string("Running!!!");
}

void laundry_run_for_time_finish_lcd(void)
{
	i2c_lcd_init();
	move_cursor(0, 0);
	lcd_string("Laundry Set Time");
	move_cursor(1, 0);
	lcd_string("Finish!!!");
	HAL_Delay(2000);
}

void laundry_run_for_auto_lcd(void)
{
	i2c_lcd_init();
	move_cursor(0, 0);
	lcd_string("Laundry");
	move_cursor(1, 0);
	lcd_string("AUTO Running!!!");
}

void laundry_run_auto_finish_lcd(void)
{
	i2c_lcd_init();
	move_cursor(0, 0);
	lcd_string("Laundry AutoTime");
	move_cursor(1, 0);
	lcd_string("Finish!!!");
	HAL_Delay(2000);
}

void laundry_time_has_not_been_set_lcd(void)
{
	i2c_lcd_init();
	move_cursor(0, 0);
	lcd_string("Time has not");
	move_cursor(1, 0);
	lcd_string("been set!!!");
	HAL_Delay(2000);
}
// end laundry //

// rinsing
void rinsing_choice_lcd(void)
{
	i2c_lcd_init();
	move_cursor(0, 0);
	lcd_string("0 waterLev 1 TIM");
	move_cursor(1, 0);
	lcd_string("2 RUN 3 RUN AUTO");
}

void rinsing_water_level_control_lcd(void)
{
	i2c_lcd_init();
	move_cursor(0, 0);
	lcd_string("0 +10  1 +20");
	move_cursor(1, 0);
	lcd_string("2 -10  3 store");
}

void rinsing_timer_setting_lcd(void)
{
	i2c_lcd_init();
	move_cursor(0, 0);
	lcd_string("0 +1  1 +10");
	move_cursor(1, 0);
	lcd_string("2 -1  3 store");
}

void rinsing_timer_setting_store_lcd(void)
{
	i2c_lcd_init();
	move_cursor(0, 0);
	lcd_string("Timer Setting");
	move_cursor(1, 0);
	lcd_string("Complete!!!");
	HAL_Delay(2000);
}

void rinsing_run_for_time_set_lcd(void)
{
	i2c_lcd_init();
	move_cursor(0, 0);
	lcd_string("Rinsing");
	move_cursor(1, 0);
	lcd_string("Running!!!");
}

void rinsing_run_for_time_finish_lcd(void)
{
	i2c_lcd_init();
	move_cursor(0, 0);
	lcd_string("rinsing Set Time");
	move_cursor(1, 0);
	lcd_string("Finish!!!");
	HAL_Delay(2000);
}

void rinsing_run_for_auto_lcd(void)
{
	i2c_lcd_init();
	move_cursor(0, 0);
	lcd_string("rinsing");
	move_cursor(1, 0);
	lcd_string("AUTO Running!!!");
}

void rinsing_run_auto_finish_lcd(void)
{
	i2c_lcd_init();
	move_cursor(0, 0);
	lcd_string("rinsing AutoTime");
	move_cursor(1, 0);
	lcd_string("Finish!!!");
	HAL_Delay(2000);
}

void rinsing_time_has_not_been_set_lcd(void)
{
	i2c_lcd_init();
	move_cursor(0, 0);
	lcd_string("Time has not");
	move_cursor(1, 0);
	lcd_string("been set!!!");
	HAL_Delay(2000);
}
// end rinsing //

// dehydration
void dehydration_begin_lcd(void)
{
	i2c_lcd_init();
	move_cursor(0, 0);
	lcd_string("SPEED 0UP 1DOWN");
	move_cursor(1, 0);
	lcd_string("2 RUN 3 STOP");
}

void dehydration_speed_up_lcd(void)
{
	i2c_lcd_init();
	move_cursor(0, 0);
	lcd_string("0 +1  1 +5");
	move_cursor(1, 0);
	lcd_string("2 +10  3 store");
}

void dehydration_speed_down_lcd(void)
{
	i2c_lcd_init();
	move_cursor(0, 0);
	lcd_string("0 -1  1 -5");
	move_cursor(1, 0);
	lcd_string("2 -10  3 store");
}

void dehydration_start_lcd(void)
{
	i2c_lcd_init();
	move_cursor(0, 0);
	lcd_string("dehydration");
	move_cursor(1, 0);
	lcd_string("Start!!!");
}

void dehydration_stop_lcd(void)
{
	i2c_lcd_init();
	move_cursor(0, 0);
	lcd_string("dehydration");
	move_cursor(1, 0);
	lcd_string("Stop!!!");
	HAL_Delay(2000);
}

void dehydration_store_lcd(void)
{
	i2c_lcd_init();
	move_cursor(0, 0);
	lcd_string("Speed Setting");
	move_cursor(1, 0);
	lcd_string("Complete!!!");
	HAL_Delay(2000);
}

void dehydration_finish_lcd(void)
{
	i2c_lcd_init();
	move_cursor(0, 0);
	lcd_string("dehydration");
	move_cursor(1, 0);
	lcd_string("Finish!!!");
	HAL_Delay(2000);
}
/*** end lcd output ***/

/*** end function definition ***/
