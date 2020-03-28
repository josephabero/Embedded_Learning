#pragma once

#include <stdbool.h>
#include <stdint.h>

volatile const LPC_GPIO_TypeDef *lpc_gpio[] = {LPC_GPIO0, LPC_GPIO1, LPC_GPIO2, LPC_GPIO3, LPC_GPIO4, LPC_GPIO5};

// #####################################################
// #                                                   #
// #                    STRUCTURES                     #
// #                                                   #
// #####################################################

typedef struct {
  uint8_t port;
  uint8_t pin;
} gpio_s;

// #####################################################
// #                                                   #
// #               FUNCTION DECLARATIONS               #
// #                                                   #
// #####################################################
static LPC_GPIO_TypeDef *gpio_get_struct(gpio_s gpio) {
  // LPC_GPIOX
  return (LPC_GPIO_TypeDef *)lpc_gpio[gpio.port];
}

void gpio_set_as_input(gpio_s gpio);
void gpio_set_as_output(gpio_s gpio);
void gpio_set_high(gpio_s gpio);
void gpio_set_low(gpio_s gpio);
void gpio_set(gpio_s gpio, bool high);
bool gpio_get_level(gpio_s gpio);

// #####################################################
// #                                                   #
// #                FUNCTION DEFINITIONS               #
// #                                                   #
// #####################################################

void gpio_set_as_input(gpio_s gpio) {
  LPC_GPIO_TypeDef *LPC_GPIO = gpio_get_struct(gpio);
  LPC_GPIO->DIR &= ~(1 << gpio.pin);
}

void gpio_set_as_output(gpio_s gpio) {
  LPC_GPIO_TypeDef *LPC_GPIO = gpio_get_struct(gpio);
  LPC_GPIO->DIR |= (1 << gpio.pin);
}

void gpio_set_high(gpio_s gpio) {
  LPC_GPIO_TypeDef *LPC_GPIO = gpio_get_struct(gpio);
  LPC_GPIO->PIN |= (1 << gpio.pin);
}

void gpio_set_low(gpio_s gpio) {
  LPC_GPIO_TypeDef *LPC_GPIO = gpio_get_struct(gpio);
  LPC_GPIO->PIN &= ~(1 << gpio.pin);
}

void gpio_set(gpio_s gpio, bool high) {
  if (high) {
    gpio_set_high(gpio);
  } else {
    gpio_set_low(gpio);
  }
}

void gpio_toggle(gpio_s gpio) {
  // Toggle
  gpio_set(gpio, !gpio_get_level(gpio));
}

bool gpio_get_level(gpio_s gpio) {
  LPC_GPIO_TypeDef *LPC_GPIO = gpio_get_struct(gpio);
  return LPC_GPIO->PIN & (1 << gpio.pin);
}
