/*
 */
#include <stdint.h>
#include "FreeRTOS.h"
#include "task.h"
#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/gpio.h>
/* Dimensions the buffer that the task being created will use as its stack.
NOTE:  This is the number of words the stack will hold, not the number of
bytes.  For example, if each stack item is 32-bits, and this is set to 100,
then 400 bytes (100 * 32-bits) will be allocated. */
#define STACK_SIZE 200
static void
task1(void *args __attribute((unused))) {

	for (;;) {
		gpio_toggle(GPIOD,GPIO12);
		vTaskDelay(pdMS_TO_TICKS(500));
	}
}
/* task to read the user button and check it out */
static void 
task_user_button(void *args __attribute((unused))) {
	while(1){
		if(0 != gpio_get(GPIOA, GPIO0)){	
			gpio_toggle(GPIOD,GPIO12);
		}

	}
}

static void hardware_setup(void)
{
	/* Enable RCC with external crystal */
	rcc_clock_setup_hse_3v3(&rcc_hse_8mhz_3v3[RCC_CLOCK_3V3_168MHZ]);
	/* Enable GPIOD clock. (LED) */
	rcc_periph_clock_enable(RCC_GPIOD);
	gpio_mode_setup(GPIOD, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, GPIO12);
	/* Enable GPIOA clock (Button) */
	rcc_periph_clock_enable(RCC_GPIOA);
	gpio_mode_setup(GPIOA, GPIO_MODE_INPUT, GPIO_PUPD_NONE, GPIO0);

}
int main(void)
{
	hardware_setup();

	//xTaskCreate(task1,"LED",STACK_SIZE, NULL, configMAX_PRIORITIES-1,NULL);
	xTaskCreate(task_user_button,"button",STACK_SIZE, NULL, configMAX_PRIORITIES-1,NULL);
	vTaskStartScheduler();
	/* Blink the LED (PC8) on the board. */
	while (1) {

	}

	return 0;
}

extern void vApplicationStackOverflowHook(
	TaskHandle_t *pxTask,
	signed portCHAR *pcTaskName);

void
vApplicationStackOverflowHook(
  TaskHandle_t *pxTask __attribute((unused)),
  signed portCHAR *pcTaskName __attribute((unused))
) {
	for(;;);	// Loop forever here..
}


