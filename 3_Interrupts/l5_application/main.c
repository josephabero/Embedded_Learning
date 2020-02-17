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

void button_ISR();

void led_task(void *pvParameters);

// #####################################################
// #                                                   #
// #                       MAIN                        #
// #                                                   #
// #####################################################

int main(void) {
  static gpio_s led = {2, 9};
  static gpio_s button = {2, 7};

  gpio_set_as_input(button);
  gpio_attach_interrupt(button, GPIO_INTR__RISING_EDGE, button_ISR);

  NVIC_EnableIRQ(GPIO_IRQn);

  // while (1) {
  //   delay__ms(100);
  // }

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

void button_ISR() {
  // to prevent formatting
  xSemaphoreGiveFromISR(button_pressed, (BaseType_t *)led_task);
  // fprintf(stderr, "ISR%s\n");
}

void led_task(void *pvParameters) {
  printf("Setting up LED Task...\n");
  gpio_s _led = *(gpio_s *)(pvParameters);
  gpio_set_as_output(_led); // Set GPIO as Output
  printf("Finished setting up LED Task!\n");

  while (1) {
    if (xSemaphoreTakeFromISR(button_pressed, portMAX_DELAY)) {
      // Toggle LED
      gpio_toggle(_led);
      printf("Toggling P%i_%i: %i\n", _led.port, _led.pin, gpio_get_level(_led));
    }
  }
}