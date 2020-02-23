#include <stdbool.h>
#include <stdio.h>

#include "FreeRTOS.h"
#include "queue.h"
#include "task.h"

#include "delay.h"
#include "l3_drivers/adc.h"
#include "l3_drivers/pwm1.h"
#include "l5_application/gpio.h"
#include "l5_application/gpio_isr.h"
#include "lpc40xx.h"

#define MAX_ADC_VALUE 4100

// #####################################################
// #                                                   #
// #                 GLOBAL VARIABLES                  #
// #                                                   #
// #####################################################

static QueueHandle_t adc_to_pwm_task_queue;

// #####################################################
// #                                                   #
// #               FUNCTION DECLARATIONS               #
// #                                                   #
// #####################################################
void pwm_task(void *pvParameters);
void adc_task(void *pvParameters);

// #####################################################
// #                                                   #
// #                       MAIN                        #
// #                                                   #
// #####################################################

int main(void) {
  static gpio_s PWM_PIN = {2, 0};
  static gpio_s ADC_PIN = {0, 25};

  adc_to_pwm_task_queue = xQueueCreate(1, sizeof(uint16_t));

  xTaskCreate(pwm_task, "pwm", 2048, &PWM_PIN, PRIORITY_LOW, NULL);
  xTaskCreate(adc_task, "adc", 2048, &ADC_PIN, PRIORITY_LOW, NULL);

  vTaskStartScheduler();
  return 0;
}

// #####################################################
// #                                                   #
// #               FUNCTION DEFINITIONS                #
// #                                                   #
// #####################################################
void pwm_task(void *pvParameters) {
  gpio_s _led = *(gpio_s *)(pvParameters);

  printf("Setting up PWM Task...\n");
  pwm1__init_single_edge(1000);

  // Set P2_0 to PWM1 (001)
  LPC_IOCON->P2_0 &= ~(0b111);
  LPC_IOCON->P2_0 |= (1 << 0);
  gpio_set_as_output(_led);

  uint16_t adc_reading = 0;
  uint8_t percent = 0;
  printf("Finished setting up PWM task...\n");
  while (1) {
    if (xQueueReceive(adc_to_pwm_task_queue, &adc_reading, 100)) {
      printf("ADC CONSUMED: %i\n", adc_reading);
      percent = (100 * adc_reading) / MAX_ADC_VALUE;
      printf("PERCENT: %i\n", percent);
      pwm1__set_duty_cycle(PWM1__2_0, percent);
    }
  }
}

void adc_task(void *pvParameters) {
  gpio_s _adc = *(gpio_s *)(pvParameters);

  adc__initialize();

  // Set P0_25 to ADC2 (001)
  LPC_IOCON->P0_25 &= ~(0b111);
  LPC_IOCON->P0_25 |= (1 << 0);
  LPC_IOCON->P0_25 &= ~(1 << 7); // Set to Analog mode
  gpio_set_as_input(_adc);

  uint16_t adc_reading;

  while (1) {
    adc_reading = adc__get_channel_reading_with_burst_mode(ADC__CHANNEL_2);
    printf("ADC SENT: %i\n", adc_reading);
    xQueueSend(adc_to_pwm_task_queue, &adc_reading, 0);
    vTaskDelay(100);
  }
}