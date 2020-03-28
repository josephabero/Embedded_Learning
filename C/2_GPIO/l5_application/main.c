#include <stdbool.h>
#include <stdio.h>

#include "FreeRTOS.h"
#include "semphr.h"
#include "task.h"

#include "l5_application/gpio.h"
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
void button_task(void *pvParameters);

// #####################################################
// #                                                   #
// #                       MAIN                        #
// #                                                   #
// #####################################################

int main(void) {
  static gpio_s led = {0, 9};
  static gpio_s button = {0, 7};

  button_pressed = xSemaphoreCreateBinary();

  xTaskCreate(led_task, "led", 2048 / sizeof(void *), &led, PRIORITY_LOW, NULL);
  xTaskCreate(button_task, "button", 2048 / sizeof(void *), &button, PRIORITY_LOW, NULL);

  vTaskStartScheduler();
  return 0;
}

// #####################################################
// #                                                   #
// #                       TASKS                       #
// #                                                   #
// #####################################################

void led_task(void *pvParameters) {
  gpio_s led = *(gpio_s *)(pvParameters);

  gpio_set_as_output(led); // Set GPIO as Output

  while (1) {

    if (xSemaphoreTake(button_pressed, 1000)) {
      // Toggle LED
      gpio_set(led, !gpio_get_level(led));
      printf("P%i_%i: %i\n", led.port, led.pin, gpio_get_level(led));
    } else {
      puts("Timeout: No switch press indication for 1000ms");
    }
    vTaskDelay(500);
  }
}

void button_task(void *pvParameters) {
  gpio_s button = *(gpio_s *)(pvParameters);

  gpio_set_as_input(button); // Set GPIO as Input

  while (1) {
    while (gpio_get_level(button)) {
      vTaskDelay(1);
      if (!gpio_get_level(button)) {
        printf("button pressed\n");
        xSemaphoreGive(button_pressed);
      }
    }
    // if (gpio_get_level(button)) {
    //   printf("button pressed\n");
    //   xSemaphoreGive(button_pressed);
    // }
    vTaskDelay(100);
  }
}
