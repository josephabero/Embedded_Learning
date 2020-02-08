#pragma once

#include <stdbool.h>
#include <stdint.h>

static LPC_GPIO_TypeDef *lpc_gpio[] = {LPC_GPIO0, LPC_GPIO1, LPC_GPIO2, LPC_GPIO3, LPC_GPIO4, LPC_GPIO5};

// #####################################################
// #                                                   #
// #                    STRUCTURES                     #
// #                                                   #
// #####################################################

typedef struct {
  uint8_t port;
  uint8_t pin;
} port_pin_s;

// #####################################################
// #                                                   #
// #                     FUNCTIONS                     #
// #                                                   #
// #####################################################

void gpio_set_as_input(port_pin_s *gpio) {
  LPC_GPIO_TypeDef *port = lpc_gpio[gpio->port];
  port->DIR &= ~(1 << gpio->pin);
}

void gpio_set_as_output(port_pin_s *gpio) {
  LPC_GPIO_TypeDef *port = lpc_gpio[gpio->port];
  port->DIR |= (1 << gpio->pin);
}

void gpio_set_high(port_pin_s *gpio) {
  LPC_GPIO_TypeDef *port = lpc_gpio[gpio->port];
  port->PIN |= (1 << gpio->pin);
}

void gpio_set_low(port_pin_s *gpio) {
  LPC_GPIO_TypeDef *port = lpc_gpio[gpio->port];
  port->PIN &= ~(1 << gpio->pin);
}

void gpio_set(port_pin_s *gpio, bool high) {
  //
  if (high) {
    gpio_set_high(gpio);
  } else {
    gpio_set_low(gpio);
  }
}

bool gpio_get_level(port_pin_s *gpio) {
  LPC_GPIO_TypeDef *port = lpc_gpio[gpio->port];
  return port->PIN & (1 << gpio->pin);
}
