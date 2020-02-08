#include <stdbool.h>
#include <stdio.h>

#include "FreeRTOS.h"
#include "task.h"

#include "lpc40xx.h"

#define TASK 1

// #####################################################
// #                                                   #
// #               FUNCTION DECLARATIONS               #
// #                                                   #
// #####################################################

void led_task(void *pvParameters);
void gpio0__set_as_input(uint8_t pin_num);
void gpio0__set_as_output(uint8_t pin_num);
void gpio0__set_high(uint8_t pin_num);
void gpio0__set_low(uint8_t pin_num);
void gpio0__set(uint8_t pin_num, bool high);
bool gpio0__get_level(uint8_t pin_num);

void gpio1__set_as_input(uint8_t pin_num);
void gpio1__set_as_output(uint8_t pin_num);
void gpio1__set_high(uint8_t pin_num);
void gpio1__set_low(uint8_t pin_num);
void gpio1__set(uint8_t pin_num, bool high);
bool gpio1__get_level(uint8_t pin_num);

// #####################################################
// #                                                   #
// #                    STRUCTURES                     #
// #                                                   #
// #####################################################

typedef struct {
  // First get gpio0 driver to work only, and if you finish it
  // you can do the extra credit to also make it work for other Ports
  // uint8_t port;
  uint8_t pin;
} port_pin_s;

// #####################################################
// #                                                   #
// #                       MAIN                        #
// #                                                   #
// #####################################################

int main(void) {
  static port_pin_s led0 = {18};
  static port_pin_s led1 = {24};

  printf("%i", led0.pin);

  xTaskCreate(led_task, "led0", 2048 / sizeof(void *), &led0, PRIORITY_LOW, NULL);
  xTaskCreate(led_task, "led1", 2048 / sizeof(void *), &led1, PRIORITY_LOW, NULL);
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

  // LPC_IOCON->P1_26 &= ~(0b111);   // Set GPIO to function as GPIO
  gpio1__set_as_output(led->pin); // Set GPIO as Output

  while (1) {
    // Turn On LED (Active Low LED)
    gpio1__set_low(led->pin);
    printf("Turning On: %i\n", gpio1__get_level(led->pin));
    vTaskDelay(500);

    // Turn Off LED (Active Low LED)
    gpio1__set_high(led->pin);
    printf("Turning Off: %i\n", gpio1__get_level(led->pin));
    vTaskDelay(500);
  }
}

// #####################################################
// #                                                   #
// #                     FUNCTIONS                     #
// #                                                   #
// #####################################################

void gpio1__set_as_input(uint8_t pin_num) { LPC_GPIO1->DIR &= ~(1 << pin_num); }

/// Should alter the hardware registers to set the pin as output
void gpio1__set_as_output(uint8_t pin_num) { LPC_GPIO1->DIR |= (1 << pin_num); }

/// Should alter the hardware registers to set the pin as high
void gpio1__set_high(uint8_t pin_num) { LPC_GPIO1->PIN |= (1 << pin_num); }

/// Should alter the hardware registers to set the pin as low
void gpio1__set_low(uint8_t pin_num) { LPC_GPIO1->PIN &= ~(1 << pin_num); }

/**
 * Should alter the hardware registers to set the pin as low
 *
 * @param {bool} high - true => set pin high, false => set pin low
 */
void gpio1__set(uint8_t pin_num, bool high) {
  if (high) {
    gpio1__set_high(pin_num);
  } else {
    gpio1__set_low(pin_num);
  }
}

/**
 * Should return the state of the pin (input or output, doesn't matter)
 *
 * @return {bool} level of pin high => true, low => false
 */
bool gpio1__get_level(uint8_t pin_num) { return LPC_GPIO1->PIN & (1 << pin_num); }