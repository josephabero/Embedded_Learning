#pragma once
#include "gpio_isr.h"

// #####################################################
// #                                                   #
// #                    STRUCTURES                     #
// #                                                   #
// #####################################################

typedef enum {
  GPIO_INTR__FALLING_EDGE,
  GPIO_INTR__RISING_EDGE,
} gpio_interrupt_e;

// Function pointer type (demonstrated later in the code sample)
typedef void (*function_pointer_t)(void);

// Note: You may want another separate array for falling vs. rising edge callbacks
static function_pointer_t gpio_callbacks_rising[3][32];
static function_pointer_t gpio_callbacks_falling[3][32];

// #####################################################
// #                                                   #
// #               FUNCTION DECLARATIONS               #
// #                                                   #
// #####################################################
void gpio_clear_interrupt(const gpio_s gpio);

// Allow the user to attach their callbacks
void gpio_attach_interrupt(gpio_s gpio, gpio_interrupt_e interrupt_type, function_pointer_t callback);
void gpio_interrupt_dispatcher(void);

gpio_s check_interrupt_status();

// #####################################################
// #                                                   #
// #               FUNCTION DEFINITIONS                #
// #                                                   #
// #####################################################

void gpio_attach_interrupt(gpio_s gpio, gpio_interrupt_e interrupt_type, function_pointer_t callback) {
  // 2) Configure GPIO 0 pin for rising or falling edge
  printf("ATTACH P%i_%i: %i\n", gpio.port, gpio.pin, interrupt_type);
  switch (interrupt_type) {
  case GPIO_INTR__FALLING_EDGE:
    gpio_callbacks_rising[gpio.port][gpio.pin] = callback;
    if (gpio.port == 0) {
      LPC_GPIOINT->IO0IntEnR |= (1 << gpio.pin);
    } else if (gpio.port == 2) {
      LPC_GPIOINT->IO2IntEnR |= (1 << gpio.pin);
    }
    break;
  case GPIO_INTR__RISING_EDGE:
    gpio_callbacks_rising[gpio.port][gpio.pin] = callback;
    if (gpio.port == 0) {
      LPC_GPIOINT->IO0IntEnF |= (1 << gpio.pin);
    } else if (gpio.port == 2) {
      LPC_GPIOINT->IO2IntEnF |= (1 << gpio.pin);
    }
    break;
  };
}

// We wrote some of the implementation for you
void gpio_interrupt_dispatcher(void) {
  // Check which pin generated the interrupt

  gpio_s gpio_intr = check_interrupt_status();
  function_pointer_t attached_user_handler = gpio_callbacks_rising[gpio_intr.port][gpio_intr.pin];

  // Invoke the user registered callback, and then clear the interrupt
  attached_user_handler();

  // Clear the interrupt
  if (gpio_intr.port == 0) {
    LPC_GPIOINT->IO0IntClr |= (1 << gpio_intr.pin);
  } else if (gpio_intr.port == 2) {
    LPC_GPIOINT->IO2IntClr |= (1 << gpio_intr.pin);
  }
  // gpio_clear_interrupt(gpio_intr);
}

gpio_s check_interrupt_status() {
  gpio_s gpio_intr;

  if (LPC_GPIOINT->IntStatus & (1 << 0)) {
    gpio_intr.port = 0;
    // PORT 0 Interrupt
    // Loop through Falling Port 0 status register, check if a status bit is set
    for (uint8_t f0_pin = 0; f0_pin < 32; f0_pin++) {
      if (LPC_GPIOINT->IO0IntEnF & (1 << f0_pin)) {
        gpio_intr.pin = f0_pin;
        return gpio_intr;
      }
    }

    // Loop through Falling Port 0 status register, return if a status bit is set
    for (uint8_t r0_pin = 0; r0_pin < 32; r0_pin++) {
      if (LPC_GPIOINT->IO0IntEnR & (1 << r0_pin)) {
        gpio_intr.pin = r0_pin;
        return gpio_intr;
      }
    }
  } // end_if Port 0
  else if (LPC_GPIOINT->IntStatus & (1 << 2)) {
    gpio_intr.port = 2;
    // PORT 2 Interrupt
    // Loop through Falling Port 2 status register, check if a status bit is set
    for (uint8_t f2_pin = 0; f2_pin < 32; f2_pin++) {
      if (LPC_GPIOINT->IO0IntEnF & (1 << f2_pin)) {
        gpio_intr.pin = f2_pin;
        return gpio_intr;
      }
    }

    // Loop through Falling Port 2 status register, return if a status bit is set
    for (uint8_t r2_pin = 0; r2_pin < 32; r2_pin++) {
      if (LPC_GPIOINT->IO0IntEnR & (1 << r2_pin)) {
        gpio_intr.pin = r2_pin;
        return gpio_intr;
      }
    }
  } // end_if Port 2
  return gpio_intr;
}

void gpio_clear_interrupt(gpio_s gpio) {
  if (gpio.port == 0) {
    LPC_GPIOINT->IO0IntClr |= (1 << gpio.pin);
  } else if (gpio.port == 2) {
    LPC_GPIOINT->IO2IntClr |= (1 << gpio.pin);
  }
}