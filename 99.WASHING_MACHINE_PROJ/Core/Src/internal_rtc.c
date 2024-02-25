#include "internal_rtc.h"
#include "main.h"
#include "i2c_lcd.h"
#include "button.h"

RTC_TimeTypeDef sTime = {0};
RTC_DateTypeDef sDate = {0};

extern uint8_t lcd_display_mode_flag;
extern RTC_TimeTypeDef mTime;

// STM32의 RTC로 부터 날짜와 시각 정보를 읽어 오는 함수
void get_rtc(void)
{
	static RTC_TimeTypeDef previousTime; // 이전 시간 정보
	char lcd_buff[40];

	HAL_RTC_GetDate(&hrtc, &sDate, RTC_FORMAT_BCD);
	HAL_RTC_GetTime(&hrtc, &sTime, RTC_FORMAT_BCD);

	if (previousTime.Seconds != sTime.Seconds)
	{
		// YYYY-MM-DD HH:mm:ss format
		printf("%04d-%02d-%02d %02d:%02d:%02d\n",
				bin2dec(sDate.Year) + 2000, bin2dec(sDate.Month), bin2dec(sDate.Date),
				bin2dec(sTime.Hours), bin2dec(sTime.Minutes), bin2dec(sTime.Seconds));
		if (lcd_display_mode_flag == 0)
		{
			sprintf(lcd_buff, "DATE: %04d-%02d-%02d",
					bin2dec(sDate.Year) + 2000, bin2dec(sDate.Month), bin2dec(sDate.Date));
			move_cursor(0, 0);
			lcd_string(lcd_buff);

			sprintf(lcd_buff, "TIME: %02d:%02d:%02d",
					bin2dec(sTime.Hours), bin2dec(sTime.Minutes), bin2dec(sTime.Seconds));
			move_cursor(1, 0);
			lcd_string(lcd_buff);
		}
	}
	previousTime.Seconds = sTime.Seconds;
}

void set_time_button_ui(void)
{
	static uint8_t hour = 0, min = 0, sec = 0;
	uint8_t lcd_buff[40];

	if (lcd_display_mode_flag == 3)
	{
		sprintf(lcd_buff, "TIME:%02d:%02d:%02d",
				bin2dec(mTime.Hours), bin2dec(mTime.Minutes), bin2dec(mTime.Seconds));
		move_cursor(0, 0);
		lcd_string(lcd_buff);

		if (get_button(BUTTON0_GPIO_Port, BUTTON0_Pin, 0) == BUTTON_PRESS)
		{
			hour++;
			if (hour > 23) hour = 0;
			sprintf(lcd_buff, "%02d", hour);
			move_cursor(1, 5);
			lcd_string(lcd_buff);
		}
		if (get_button(BUTTON1_GPIO_Port, BUTTON1_Pin, 1) == BUTTON_PRESS)
		{
			min++;
			if (min > 59) min = 0;
			sprintf(lcd_buff, "%02d", min);
			move_cursor(1, 8);
			lcd_string(lcd_buff);
		}
		if (get_button(BUTTON2_GPIO_Port, BUTTON2_Pin, 2) == BUTTON_PRESS)
		{
			sec++;
			if (sec > 59) sec = 0;
			sprintf(lcd_buff, "%02d", sec);
			move_cursor(1, 11);
			lcd_string(lcd_buff);
		}
		if (get_button(BUTTON3_GPIO_Port, BUTTON3_Pin, 3) == BUTTON_PRESS)
		{
			mTime.Hours = dec2bin(hour);
			mTime.Minutes = dec2bin(min);
			mTime.Seconds = dec2bin(sec);

			lcd_command(CLEAR_DISPLAY);
			move_cursor(0, 0);
			lcd_string("change success");
			// 완료 버튼
			HAL_RTC_SetTime(&hrtc, &mTime, RTC_FORMAT_BCD);

			sprintf(lcd_buff, "TIME: %02d:%02d:%02d",
					bin2dec(mTime.Hours), bin2dec(mTime.Minutes), bin2dec(mTime.Seconds));
			move_cursor(1, 0);
			lcd_string(lcd_buff);

			HAL_GPIO_WritePin(GPIOA, GPIO_PIN_3, 1);
			HAL_Delay(1000);
			HAL_GPIO_WritePin(GPIOA, GPIO_PIN_3, 0);
			lcd_display_mode_flag = 0;
		}
	}
}

// setrtc231016103800 <- input command
void set_rtc(unsigned char* input)
{
	char year[4], month[4], date[4]; // date
	char hour[4], min[4], sec[4]; // time

//	setrtc231016103800 <- input command
//	0123456789 <- index
	strncpy(year, input+6, 2); // year[0] = '2' yy[1] = '3' yy[2] = 0; 가 들어간다.
	strncpy(month, input+8, 2);
	strncpy(date, input+10, 2);

	strncpy(hour, input+12, 2);
	strncpy(min, input+14, 2);
	strncpy(sec, input+16, 2);

// 	ascii --> int --> bcd // conversion progress
	sDate.Year = dec2bin(atoi(year));
	sDate.Month = dec2bin(atoi(month));
	sDate.Date = dec2bin(atoi(date));

	sTime.Hours = dec2bin(atoi(hour));
	sTime.Minutes = dec2bin(atoi(min));
	sTime.Seconds = dec2bin(atoi(sec));

	HAL_RTC_SetDate(&hrtc, &sDate, RTC_FORMAT_BCD);
	HAL_RTC_SetTime(&hrtc, &sTime, RTC_FORMAT_BCD);
}

unsigned char bin2dec(unsigned char byte)
{
	unsigned char high, low;
	low = byte & 0x0f; // 하위 4bit (low nibble)
	high = (byte >> 4) * 10; // 상위 4bit (low nibble)

	return high + low;
}

unsigned char dec2bin(unsigned char byte)
{
	unsigned char high, low;

	high = (byte / 10) << 4;
	low = byte % 10;

	return high + low;
}
