#include <stdbool.h>
#include <stdio.h>

#include "FreeRTOS.h"
#include "semphr.h"
#include "task.h"

#include "l5_application/gpio.h"
#include "lpc40xx.h"

#define TASK 1

// #####################################################
// #                                                   #
// #                 GLOBAL VARIABLES                  #
// #                                                   #
// #####################################################

// LPC_GPIO_TypeDef *gpio_port;
static SemaphoreHandle_t switch_press_indication;

// #####################################################
// #                                                   #
// #               FUNCTION DECLARATIONS               #
// #                                                   #
// #####################################################

void led_task(void *pvParameters);
void button_task(void *pvParameters);

// void gpio_set_as_input(LPC_GPIO_TypeDef *port, uint8_t pin);
// void gpio_set_as_output(LPC_GPIO_TypeDef *port, uint8_t pin);
// void gpio_set_high(LPC_GPIO_TypeDef *port, uint8_t pin);
// void gpio_set_low(LPC_GPIO_TypeDef *port, uint8_t pin);
// void gpio_set(LPC_GPIO_TypeDef *port, uint8_t pin, bool high);
// bool gpio_get_level(LPC_GPIO_TypeDef *port, uint8_t pin);

// #####################################################
// #                                                   #
// #                       MAIN                        #
// #                                                   #
// #####################################################

int main(void) {
  // Onboard LEDs
  static port_pin_s led3 = {1, 18};
  // static port_pin_s led2 = {1, 24};
  // static port_pin_s led1 = {1, 26};
  // static port_pin_s led0 = {2, 3};

  // Onboard Button
  // static port_pin_s sw2 = {0, 30};
  static port_pin_s sw3 = {0, 29};

  // External LED

  // External Button

  // printf("pin0: %i\n", led0.pin);
  // printf("port0: %i\n", led0.port);
  // printf("pin1: %i\n", led1.pin);
  // printf("port1: %i\n", led1.port);
  switch_press_indication = xSemaphoreCreateBinary();

  xTaskCreate(led_task, "led3", 2048 / sizeof(void *), &led3, PRIORITY_LOW, NULL);
  // xTaskCreate(led_task, "led1", 2048 / sizeof(void *), &led1, PRIORITY_LOW, NULL);

  xTaskCreate(button_task, "sw3", 2048 / sizeof(void *), &sw3, PRIORITY_LOW, NULL);
  // xTaskCreate(led_task, "led1", 2048 / sizeof(void *), &led1, PRIORITY_LOW, NULL);
  vTaskStartScheduler();
  return 0;
}

// #####################################################
// #                                                   #
// #                       TASKS                       #
// #                                                   #
// #####################################################

/// LED Task to Blink LED
void led_task(void *pvParameters) {
  port_pin_s *led = (port_pin_s *)(pvParameters);

  gpio_set_as_output(led); // Set GPIO as Output

  while (1) {

    if (xSemaphoreTake(switch_press_indication, 1000)) {
      // Turn On LED (Active Low LED)
      gpio_set_low(led);
      printf("Turning On P%i_%i: %i\n", led->port, led->pin, gpio_get_level(led));
      vTaskDelay(500);

      // Turn Off LED (Active Low LED)
      gpio_set_high(led);
      printf("Turning Off P%i_%i: %i\n", led->port, led->pin, gpio_get_level(led));
      vTaskDelay(500);
    } else {
      puts("Timeout: No switch press indication for 1000ms");
    }
  }
}

void button_task(void *pvParameters) {
  port_pin_s *button = (port_pin_s *)(pvParameters);

  gpio_set_as_input(button); // Set GPIO as Output

  while (1) {
    if (gpio_get_level(button)) {
      printf("button pressed\n");
      xSemaphoreGive(switch_press_indication);
    }
    vTaskDelay(100);
  }
}
