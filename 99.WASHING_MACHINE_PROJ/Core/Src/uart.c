#include "uart.h"
#include "main.h"

extern uint8_t rx_data;
extern uint8_t bt_rx_data;

extern UART_HandleTypeDef huart3; // pc
extern UART_HandleTypeDef huart6; // bt

extern void led_all_on(void);
extern void led_all_off(void);
extern void led_on_up(void);
extern void led_on_down(void);
extern void led_keep_on_up(void);
extern void led_keep_on_down(void);
extern void flower_on(void);
extern void flower_off(void);

// HW 와 SW가 만나는 장소 : call back function

/**
  * @brief  Tx Transfer completed callbacks.
  * @param  huart  Pointer to a UART_HandleTypeDef structure that contains
  *                the configuration information for the specified UART module.
  * @retval None
  */
// move from HAL_UART_RxCpltCallback of stm32f4xx_hal_uart to here
// UART로 1 byte가 수신되면 H/W가 call을 해준다.
// comport mater
// ledallon\n
// ledalloff\n

#define COMMAND_NUMBER 20
#define COMMAND_LENGTH 40

volatile unsigned char queue_rx_buff[COMMAND_NUMBER][COMMAND_LENGTH];
volatile int rear = 0;
volatile int front = 0;

volatile int rx_index = 0;
volatile unsigned char rx_buff[COMMAND_LENGTH];

volatile int newline_detect_flag = 0; // new line을 만났을 때의 indicator ex) ledallon\n 에서의 \n을 체크

volatile unsigned char bt_rx_buff[COMMAND_LENGTH]; // UART3으로부터 수신된 char를 저장하는 공간(\n을 만날때 까지)
volatile int bt_rx_index = 0; // rx_buff의 save위치
volatile int bt_newline_detect_flag = 0; // new line을 만났을 때의 indicator ex) ledallon\n 에서의 \n을 체크

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart);
void pc_command_processing(void);
void bt_command_processing(void);

extern volatile int DHT11time;
extern volatile int DHT11time_on_flag;
extern volatile int ULTRASONIC_on_flag;

extern void set_rtc(unsigned char* input);

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
	if (huart == &huart3) // comport master와 연결된 uart
	{
#if 1
		if (((rear + 1) % COMMAND_NUMBER != front))
		{
			if (rx_index < COMMAND_LENGTH)
			{
				if (rx_data == '\n' || rx_data == '\r')
				{
					queue_rx_buff[rear][rx_index] = 0; // '\0'
					newline_detect_flag = 1; // new line을 만났다는 flag를 set한다.
					rx_index = 0; // 초기화, 다음 message를 저장하기 위해서
					rear = (rear + 1) % COMMAND_NUMBER;
				}
				else
				{
					queue_rx_buff[rear][rx_index++] = rx_data;
				}
			}
			else
			{
				rx_index = 0;
				printf("Message Overflow !!!\n");
			}
		}
		// 주의: 반드시 HAL_UART_Receive_IT를 call 해줘야 다음 inturrupt가 발생이 된다.
		HAL_UART_Receive_IT(&huart3, &rx_data, 1);
#else
		if (rx_index < COMMAND_LENGTH) // 현재까지 들어온 byte가 40byte를 넘지 않으면
		{
			if (rx_data == '\n' || rx_data == '\r')
			{
				rx_buff[rx_index] = 0; // '\0'
				newline_detect_flag = 1; // new line을 만났다는 flag를 set한다.
				rx_index = 0; // 초기화, 다음 message를 저장하기 위해서
			}
			else
			{
				rx_buff[rx_index++] = rx_data;
			}
		}
		else
		{
			rx_index = 0;
			printf("Message Overflow !!!\n");
		}
		// 주의: 반드시 HAL_UART_Receive_IT를 call 해줘야 다음 inturrupt가 발생이 된다.
		HAL_UART_Receive_IT(&huart3, &rx_data, 1);
#endif
	}
#if 0
	if (huart == &huart6) // BT와 연결된 uart
	{

		if (bt_rx_index < COMMAND_LENGTH) // 현재까지 들어온 byte가 40byte를 넘지 않으면
		{
			if (bt_rx_data == '\n' || bt_rx_data == '\r')
			{
				bt_rx_buff[bt_rx_index] = 0; // '\0'
				bt_newline_detect_flag = 1; // new line을 만났다는 flag를 set한다.
				bt_rx_index = 0; // 초기화, 다음 message를 저장하기 위해서
			}
			else
			{
				bt_rx_buff[bt_rx_index++] = bt_rx_data;
			}
		}
		else
		{
			bt_rx_index = 0;
			printf("Message Overflow !!!\n");
		}
		// 주의: 반드시 HAL_UART_Receive_IT를 call 해줘야 다음 inturrupt가 발생이 된다.
		HAL_UART_Receive_IT(&huart6, &bt_rx_data, 1);
	}
#endif
}

void pc_command_processing(void)
{
	if ((front != rear))
	{
#if 1
		if (!strncmp(queue_rx_buff[front], "setrtc", strlen("setrtc")))
		{
			set_rtc(queue_rx_buff[front]);
		}
#else
		if (!strncmp(queue_rx_buff[front], "led_all_on", strlen("led_all_on")))
		{
			led_all_on();
		}
		else if (!strncmp(queue_rx_buff[front], "led_all_off", strlen("led_all_off")))
		{
			led_all_off();
		}
		else if (!strncmp(queue_rx_buff[front], "led_on_up", strlen("led_on_up")))
		{
			led_on_up();
		}
		else if (!strncmp(queue_rx_buff[front], "led_on_down", strlen("led_on_down")))
		{
			led_on_down();
		}
		else if (!strncmp(queue_rx_buff[front], "led_keep_on_up", strlen("led_keep_on_up")))
		{
			led_keep_on_up();
		}
		else if (!strncmp(queue_rx_buff[front], "led_keep_on_down", strlen("led_keep_on_down")))
		{
			led_keep_on_down();
		}
		else if (!strncmp(queue_rx_buff[front], "flower_on", strlen("flower_on")))
		{
			flower_on();
		}
		else if (!strncmp(queue_rx_buff[front], "flower_off", strlen("flower_off")))
		{
			flower_off();
		}
#endif
		front = (front+1)%COMMAND_NUMBER;
		return;
	}

#if 0
	if (newline_detect_flag) // comport master로 부터 완전한 문장이 들어 오면 (new line을 만나면)
	{
		newline_detect_flag = 0;
		printf("%s\n", rx_buff);

		if (!strncmp(rx_buff, "led_all_on", strlen("led_all_on")))
		{
			led_all_on();
			return;
		}
		if (!strncmp(rx_buff, "led_all_off", strlen("led_all_off")))
		{
			led_all_off();
			return;
		}
		if (!strncmp(rx_buff, "led_on_up", strlen("led_on_up")))
		{
			led_on_up();
			return;
		}
		if (!strncmp(rx_buff, "led_on_down", strlen("led_on_down")))
		{
			led_on_down();
			return;
		}
		if (!strncmp(rx_buff, "led_keep_on_up", strlen("led_keep_on_up")))
		{
			led_keep_on_up();
			return;
		}
		if (!strncmp(rx_buff, "led_keep_on_down", strlen("led_keep_on_down")))
		{
			led_keep_on_down();
			return;
		}
		if (!strncmp(rx_buff, "flower_on", strlen("flower_on")))
		{
			flower_on();
			return;
		}
		if (!strncmp(rx_buff, "flower_off", strlen("flower_off")))
		{
			flower_off();
			return;
		}
	}
#endif

#if 0
	if (newline_detect_flag) // comport master로 부터 완전한 문장이 들어 오면 (new line을 만나면)
	{
		newline_detect_flag = 0;
//		printf("%s\n", rx_buff);

		if (!strncmp(rx_buff, "DHT11time", strlen("DHT11time")))
		{
			DHT11time = atoi(rx_buff + strlen("DHT11time"));
		}
		else if (!strncmp(rx_buff, "DHT11on", strlen("DHT11on")))
		{
			DHT11time_on_flag = 1;
		}
		else if (!strncmp(rx_buff, "DHT11off", strlen("DHT11off")))
		{
			DHT11time_on_flag = 0;
		}
		else if (!strncmp(rx_buff, "ULTRAon", strlen("ULTRAon")))
		{
			ULTRASONIC_on_flag = 1;
		}
		else if (!strncmp(rx_buff, "ULTRAoff", strlen("ULTRAoff")))
		{
			ULTRASONIC_on_flag = 0;
		}
		return;
	}
#endif
}

#if 1
void bt_command_processing(void)
{
	if (bt_newline_detect_flag) // bt로 부터 완전한 문장이 들어 오면 (new line을 만나면)
	{
		bt_newline_detect_flag = 0;
		printf("%s\n", bt_rx_buff);

		if (!strncmp(bt_rx_buff, "led_all_on", strlen("led_all_on")))
		{
			led_all_on();
			return;
		}
		if (!strncmp(bt_rx_buff, "led_all_off", strlen("led_all_off")))
		{
			led_all_off();
			return;
		}
		if (!strncmp(bt_rx_buff, "led_on_up", strlen("led_on_up")))
		{
			led_on_up();
			return;
		}
		if (!strncmp(bt_rx_buff, "led_on_down", strlen("led_on_down")))
		{
			led_on_down();
			return;
		}
		if (!strncmp(bt_rx_buff, "led_keep_on_up", strlen("led_keep_on_up")))
		{
			led_keep_on_up();
			return;
		}
		if (!strncmp(bt_rx_buff, "led_keep_on_down", strlen("led_keep_on_down")))
		{
			led_keep_on_down();
			return;
		}
		if (!strncmp(bt_rx_buff, "flower_on", strlen("flower_on")))
		{
			flower_on();
			return;
		}
		if (!strncmp(bt_rx_buff, "flower_off", strlen("flower_off")))
		{
			flower_off();
			return;
		}
	}
}
#endif
