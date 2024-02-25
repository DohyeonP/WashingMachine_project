#define BUTTON_RELEASE 1 // 버튼을 뗀 상태 == high
#define BUTTON_PRESS 0 // 버튼을 누른 상태 == active low
#define BUTTON_NUMBER 5 // 버튼 갯수 : 5

extern RTC_HandleTypeDef hrtc;

int get_button(GPIO_TypeDef *GPIO, uint16_t GPIO_PIN, uint8_t button_number);
void lcd_display_mode_select(void);
