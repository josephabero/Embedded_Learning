#include <stdbool.h>
#include <stdio.h>

#include "FreeRTOS.h"
#include "semphr.h"
#include "task.h"

#include "delay.h"
#include "l5_application/gpio.h"
#include "l5_application/gpio_isr.h"
#include "lpc40xx.h"

// #####################################################
// #                                                   #
// #                 GLOBAL VARIABLES                  #
// #                                                   #
// #####################################################

static SemaphoreHandle_t button_pressed;

// #####################################################
// #                                                   #
// #               FUNCTION DECLARATIONS               #
// #                                                   #
// #####################################################
void led_task(void *pvParameters);

void button_ISR() {
  xSemaphoreGiveFromISR(button_pressed, NULL);
}

// #####################################################
// #                                                   #
// #                       MAIN                        #
// #                                                   #
// #####################################################

int main(void) {
  static gpio_s led = {1, 18};
  static gpio_s button = {0, 29};

  gpio_set_as_input(button);
  gpio_attach_interrupt(button, GPIO_INTR__RISING_EDGE, button_ISR);

  NVIC_EnableIRQ(GPIO_IRQn);

  button_pressed = xSemaphoreCreateBinary();

  xTaskCreate(led_task, "led", 2048 / sizeof(void *), &led, PRIORITY_LOW, NULL);
  // xTaskCreate(button_task, "button", 2048 / sizeof(void *), &button, PRIORITY_LOW, NULL);

  vTaskStartScheduler();
  return 0;
}

// #####################################################
// #                                                   #
// #               FUNCTION DEFINITIONS                #
// #                                                   #
// #####################################################

void led_task(void *pvParameters) {
  printf("Setting up LED Task...\n");
  gpio_s _led = *(gpio_s *)(pvParameters);
  gpio_set_as_output(_led); // Set GPIO as Output
  printf("Finished setting up LED Task!\n");

  while (1) {
    if (xSemaphoreTake(button_pressed, portMAX_DELAY)) {
      // Toggle LED
      printf("Toggling P%i_%i: %i\n", _led.port, _led.pin, gpio_get_level(_led));
      gpio_toggle(_led);
    }
    printf("LED_TASK\n");
    vTaskDelay(500);
  }
}