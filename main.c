/*
 */
#include <stdint.h>
#include <stdio.h>
#include "FreeRTOS.h"
#include "task.h"
#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/usart.h>
/* Dimensions the buffer that the task being created will use as its stack.
NOTE:  This is the number of words the stack will hold, not the number of
bytes.  For example, if each stack item is 32-bits, and this is set to 100,
then 400 bytes (100 * 32-bits) will be allocated. */
#define STACK_SIZE 400
static void
task_toggle_green(void *args __attribute((unused))) {

	for (;;) {
		gpio_toggle(GPIOD,GPIO12);
		vTaskDelay(pdMS_TO_TICKS(1000));
	}
}
static void
task_toggle_orange(void *args __attribute((unused))) {

	for (;;) {
		gpio_toggle(GPIOD,GPIO13);
		vTaskDelay(pdMS_TO_TICKS(500));
	}
}
/* task to read the user button and check it out */

static void 
task_user_button(void *args __attribute((unused))) {
	while(1){
		if(0 != gpio_get(GPIOA, GPIO0)){	
			gpio_toggle(GPIOD,GPIO12);
			vTaskDelay(pdMS_TO_TICKS(100));
		}

	}
}
#define AF_USART1 	GPIO_AF7

static void debug_uart_setup(void)
{
 	/* USART1 PA9=TX, PA19=RX*/
	gpio_mode_setup(GPIOA, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO9);
	gpio_set_af(GPIOA, AF_USART1, GPIO9);

	gpio_mode_setup(GPIOA, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO10);
	gpio_set_af(GPIOA, AF_USART1, GPIO10);
	
	rcc_periph_clock_enable(RCC_USART1);	
	usart_set_baudrate(USART1, 115200);
	usart_set_databits(USART1, 8);
	usart_set_parity(USART1, USART_PARITY_NONE);
	usart_set_stopbits(USART1, USART_CR2_STOPBITS_1);
	usart_set_mode(USART1, USART_MODE_TX_RX);
	usart_set_flow_control(USART1, USART_FLOWCONTROL_NONE);
	usart_enable(USART1);

}
/* Structure that will hold the TCB of the task being created. */
    StaticTask_t xTaskBuffer;

    /* Buffer that the task being created will use as its stack.  Note this is
    an array of StackType_t variables.  The size of StackType_t is dependent on
    the RTOS port. */
    StackType_t xStack[ STACK_SIZE ];

static void 
task_uart_send(void *args __attribute((unused))) {
	while(1){
		usart_send(USART1, 'A');
		}

}

static void 
task_uart_read(void *args __attribute((unused))) {
	static volatile uint8_t c=0;
	while(1){
		c = usart_recv(USART1);
		if(c == 'A'){	
			gpio_toggle(GPIOD,GPIO12);
			vTaskDelay(pdMS_TO_TICKS(10));
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
	gpio_mode_setup(GPIOD, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, GPIO13);
	gpio_mode_setup(GPIOD, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, GPIO14);
	gpio_mode_setup(GPIOD, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, GPIO15);
	/* Enable GPIOA clock (Button) */
	rcc_periph_clock_enable(RCC_GPIOA);
	gpio_mode_setup(GPIOA, GPIO_MODE_INPUT, GPIO_PUPD_NONE, GPIO0);

	debug_uart_setup();
	
}
int main(void)
{
	hardware_setup();
	xTaskCreate(task_toggle_green,"led_green",STACK_SIZE, NULL, configMAX_PRIORITIES-1,NULL);
	xTaskCreate(task_toggle_orange,"led_orange",STACK_SIZE, NULL, configMAX_PRIORITIES-1,NULL);
	
	//xTaskCreate(task_user_button,"button",STACK_SIZE, NULL, configMAX_PRIORITIES-1,NULL);
	xTaskCreateStatic(task_uart_send,"uart_send",STACK_SIZE, NULL, configMAX_PRIORITIES-4,xStack,&xTaskBuffer);
	xTaskCreate(task_uart_read,"uart_read",STACK_SIZE, NULL, configMAX_PRIORITIES-3,NULL);
	
	vTaskStartScheduler();
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


/* configSUPPORT_STATIC_ALLOCATION is set to 1, so the application must provide an
implementation of vApplicationGetIdleTaskMemory() to provide the memory that is
used by the Idle task. */
void vApplicationGetIdleTaskMemory( StaticTask_t **ppxIdleTaskTCBBuffer,
                                    StackType_t **ppxIdleTaskStackBuffer,
                                    uint32_t *pulIdleTaskStackSize )
{
/* If the buffers to be provided to the Idle task are declared inside this
function then they must be declared static - otherwise they will be allocated on
the stack and so not exists after this function exits. */
static StaticTask_t xIdleTaskTCB;
static StackType_t uxIdleTaskStack[ configMINIMAL_STACK_SIZE ];

    /* Pass out a pointer to the StaticTask_t structure in which the Idle task's
    state will be stored. */
    *ppxIdleTaskTCBBuffer = &xIdleTaskTCB;

    /* Pass out the array that will be used as the Idle task's stack. */
    *ppxIdleTaskStackBuffer = uxIdleTaskStack;

    /* Pass out the size of the array pointed to by *ppxIdleTaskStackBuffer.
    Note that, as the array is necessarily of type StackType_t,
    configMINIMAL_STACK_SIZE is specified in words, not bytes. */
    *pulIdleTaskStackSize = configMINIMAL_STACK_SIZE;
}
/*-----------------------------------------------------------*/

/* configSUPPORT_STATIC_ALLOCATION and configUSE_TIMERS are both set to 1, so the
application must provide an implementation of vApplicationGetTimerTaskMemory()
to provide the memory that is used by the Timer service task. */
void vApplicationGetTimerTaskMemory( StaticTask_t **ppxTimerTaskTCBBuffer,
                                     StackType_t **ppxTimerTaskStackBuffer,
                                     uint32_t *pulTimerTaskStackSize )
{
/* If the buffers to be provided to the Timer task are declared inside this
function then they must be declared static - otherwise they will be allocated on
the stack and so not exists after this function exits. */
static StaticTask_t xTimerTaskTCB;
static StackType_t uxTimerTaskStack[ configTIMER_TASK_STACK_DEPTH ];

    /* Pass out a pointer to the StaticTask_t structure in which the Timer
    task's state will be stored. */
    *ppxTimerTaskTCBBuffer = &xTimerTaskTCB;

    /* Pass out the array that will be used as the Timer task's stack. */
    *ppxTimerTaskStackBuffer = uxTimerTaskStack;

    /* Pass out the size of the array pointed to by *ppxTimerTaskStackBuffer.
    Note that, as the array is necessarily of type StackType_t,
    configTIMER_TASK_STACK_DEPTH is specified in words, not bytes. */
    *pulTimerTaskStackSize = configTIMER_TASK_STACK_DEPTH;
}
