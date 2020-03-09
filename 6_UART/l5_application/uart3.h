#pragma once

#include "FreeRTOS.h"
#include "l3_drivers/lpc_peripherals.h"
#include "lpc40xx.h"
#include "queue.h"
#include <stdio.h>

typedef enum {
  UART2,
  UART3,
} uart_number_e;

void uart3_init(uart_number_e uart, uint32_t peripheral_clock, uint32_t baud_rate);
bool uart_polled_get(uart_number_e uart, char *input_byte);
bool uart_polled_put(uart_number_e uart, char output_byte);
void uart_enable_receive_interrupt(uart_number_e uart_number);
bool uart_get_char_from_queue(char *input_byte, uint32_t timeout);