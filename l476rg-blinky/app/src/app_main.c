#include <app_main.h>

void uart_transmit(uint8_t *data, uint8_t size);

void app_main() {
	HAL_Delay(500);
}

uint8_t led_state = 0;

/*
 * Interrupt turns on or off the onboard LED when the onboard button
 * is pressed. Then sends a message over UART to say the button has
 * been pressed.
 * All working inderpenedently of the mainloop.
 */
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_PIN) {
	if(GPIO_PIN == PUSH_BTN_Pin) {
		switch (led_state) {
			case 0:
				HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, GPIO_PIN_SET);
				led_state = 1;
				break;
			case 1:
				HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, GPIO_PIN_RESET);
				led_state = 0;
				break;
		}
	}
	uint8_t data[] = "Button Pressed!\r\n";
	uart_transmit(&data, sizeof(data));
}
