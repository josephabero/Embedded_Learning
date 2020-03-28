#include <stdbool.h>
#include <stdio.h>

#include "FreeRTOS.h"
#include "queue.h"
#include "task.h"

#include "l5_application/gpio.h"
#include "l5_application/gpio_isr.h"
#include "l5_application/uart3.h"
#include "lpc40xx.h"
#include <stdlib.h>

// #####################################################
// #                                                   #
// #                 GLOBAL VARIABLES                  #
// #                                                   #
// #####################################################

// #####################################################
// #                                                   #
// #               FUNCTION DECLARATIONS               #
// #                                                   #
// #####################################################
void UART_Transmitter_Task(void *pvParameters);
void UART_Receiver_Task(void *pvParameters);
void board_1_sender_task(void *p);
void board_2_receiver_task(void *p);

// #####################################################
// #                                                   #
// #                       MAIN                        #
// #                                                   #
// #####################################################

int main(void) {
  uart3_init(UART3, 24000000, 38400);

  LPC_IOCON->P2_8 &= ~(0b111);
  LPC_IOCON->P2_8 |= 0b010; // Set Pin Mode to TX3 (010)

  LPC_IOCON->P2_9 &= ~(0b111);
  LPC_IOCON->P2_9 |= 0b010; // Set Pin Mode to RX3 (010)

  LPC_IOCON->P0_0 &= ~(0b111);
  LPC_IOCON->P0_0 |= 0b010; // Set Pin Mode to TX3 (010)

  LPC_IOCON->P0_1 &= ~(0b111);
  LPC_IOCON->P0_1 |= 0b010; // Set Pin Mode to RX3 (010)

  uart_enable_receive_interrupt(UART3);

  xTaskCreate(UART_Transmitter_Task, "TRANSMITTER", 2048 / sizeof(void *), NULL, PRIORITY_HIGH, NULL);
  xTaskCreate(UART_Receiver_Task, "RECEIVER", 2048 / sizeof(void *), NULL, PRIORITY_LOW, NULL);

  vTaskStartScheduler();
  return 0;
}

// #####################################################
// #                                                   #
// #               FUNCTION DEFINITIONS                #
// #                                                   #
// #####################################################
#if 0
void UART_Transmitter_Task(void *pvParameters) {
  char t_value = 0;
  while (1) {
    if (uart_polled_put(UART3, t_value)) {
      printf("SENT...: %i\n", t_value);
      t_value++;
    }
    vTaskDelay(500);
  }
}

void UART_Receiver_Task(void *pvParameters) {
  char r_value;
  while (1) {
    // printf("ATTEMPTING TO RECEIVE...: %i\n", *r_value);
    if (uart_get_char_from_queue(&r_value, 100)) {
      printf("RECEIVED: %i\n", r_value);
    }
    vTaskDelay(500);
  }
}
#else
// This task is done for you, but you should understand what this code is doing
void UART_Transmitter_Task(void *pvParameters) {
  char number_as_string[16] = {0};

  while (1) {
    const int number = rand();
    sprintf(number_as_string, "%i", number);

    // Send one char at a time to the other board including terminating NULL char
    for (int i = 0; i <= strlen(number_as_string); i++) {
      uart_polled_put(UART3, number_as_string[i]);
      printf("Sent: %c\n", number_as_string[i]);
    }

    printf("Sent: %i over UART to the other board\n", number);
    vTaskDelay(3000);
  }
}

void UART_Receiver_Task(void *pvParameters) {
  char number_as_string[16] = {0};
  int counter = 0;

  while (1) {
    char byte = 0;
    uart_get_char_from_queue(&byte, portMAX_DELAY);
    printf("Received: %c\n", byte);

    // This is the last char, so print the number
    if ('\0' == byte) {
      number_as_string[counter] = '\0';
      counter = 0;
      printf("Received this number from the other board: %s\n", number_as_string);
    }
    // We have not yet received the NULL '\0' char, so buffer the data
    else if (counter < 16){
      // TODO: Store data to number_as_string[] array one char at a time
      // Hint: Use counter as an index, and increment it as long as we do not reach max value of 16
      number_as_string[counter++] = byte;
    }
  }
}
#endif