#include <stdbool.h>
#include <stdio.h>

#include "FreeRTOS.h"
#include "event_groups.h"
#include "queue.h"
#include "task.h"

#include "ff.h"
#include <string.h>

#include "l4_io/sensors/acceleration.h"
#include "l5_application/gpio.h"
#include "l5_application/gpio_isr.h"
#include "lpc40xx.h"
#include <stdlib.h>

// #####################################################
// #                                                   #
// #                 GLOBAL VARIABLES                  #
// #                                                   #
// #####################################################
#define PRODUCER_BIT (1 << 0)
#define CONSUMER_BIT (1 << 1)

static QueueHandle_t SENSOR_QUEUE;
static EventGroupHandle_t WATCHDOG_EVENT_BITS;

const char *SENSOR_FILENAME = "sensor_data.csv";
const char *WATCHDOG_FILENAME = "watchdog.txt";

// #####################################################
// #                                                   #
// #               FUNCTION DECLARATIONS               #
// #                                                   #
// #####################################################
void sensorProducerTask(void *pvParameters);
void sensorConsumerTask(void *pvParameters);
void watchdogTask(void *pvParameters);

void initSDCard();
void createFile(const char *filename);
void writeToSDCard(const char *filename, const char *message);

// #####################################################
// #                                                   #
// #                       MAIN                        #
// #                                                   #
// #####################################################

int main(void) {
  initSDCard();

  SENSOR_QUEUE = xQueueCreate(1, sizeof(acceleration__axis_data_s));
  WATCHDOG_EVENT_BITS = xEventGroupCreate();

  xTaskCreate(sensorProducerTask, "PRODUCER", 2048 / sizeof(void *), NULL, PRIORITY_MEDIUM, NULL);
  xTaskCreate(sensorConsumerTask, "RECEIVER", 2048 / sizeof(void *), NULL, PRIORITY_MEDIUM, NULL);
  xTaskCreate(watchdogTask, "WATCHDOG", 2048 / sizeof(void *), NULL, PRIORITY_HIGH, NULL);

  vTaskStartScheduler();
  return 0;
}

// #####################################################
// #                                                   #
// #               FUNCTION DEFINITIONS                #
// #                                                   #
// #####################################################
void sensorProducerTask(void *pvParameters) {
  acceleration__axis_data_s reading;
  acceleration__axis_data_s avg_value;
  uint32_t sum_x;
  uint32_t sum_y;
  uint32_t sum_z;

  while (1) {
    sum_x = 0;
    sum_y = 0;
    sum_z = 0;

    for (uint8_t i = 0; i < 100; i++) {
      reading = acceleration__get_data();
      sum_x += reading.x;
      sum_y += reading.y;
      sum_z += reading.z;
    }

    avg_value.x = sum_x / 100;
    avg_value.y = sum_y / 100;
    avg_value.z = sum_z / 100;

    if (xQueueSend(SENSOR_QUEUE, &avg_value, 0)) {
      printf("Sent!\n");
    }
    xEventGroupSetBits(WATCHDOG_EVENT_BITS, PRODUCER_BIT);
    vTaskDelay(100);
  }
}

void sensorConsumerTask(void *pvParameters) {
  char sensor_message[64];

  acceleration__axis_data_s received_value;
  received_value.x = 0;
  received_value.y = 0;
  received_value.z = 0;

  while (1) {
    if (xQueueReceive(SENSOR_QUEUE, &received_value, portMAX_DELAY)) {
      printf("Received: %i %i %i\n", received_value.x, received_value.y, received_value.z);

      sprintf(sensor_message, "%i, %i, %i\n", received_value.x, received_value.y, received_value.z);
      writeToSDCard(SENSOR_FILENAME, sensor_message);
    }
    xEventGroupSetBits(WATCHDOG_EVENT_BITS, CONSUMER_BIT);
  }
}

void watchdogTask(void *pvParameters) {

  char watchdog_message[64];

  EventBits_t result_bits;
  while (1) {
    result_bits = xEventGroupWaitBits(WATCHDOG_EVENT_BITS, (PRODUCER_BIT | CONSUMER_BIT), pdTRUE, pdTRUE, 1100);
    if (!(result_bits & PRODUCER_BIT)) {
      printf("Producer Task failed to run!\n");
      sprintf(watchdog_message, "Producer Task failed to run!\n");
      writeToSDCard(WATCHDOG_FILENAME, watchdog_message);
    }
    if (!(result_bits & CONSUMER_BIT)) {
      printf("Consumer Task failed to run!\n");
      sprintf(watchdog_message, "Consumer Task failed to run!\n");
      writeToSDCard(WATCHDOG_FILENAME, watchdog_message);
    }
  }
}

void initSDCard() {
  printf("Initializing SD Card!\n");
  createFile(SENSOR_FILENAME);
  writeToSDCard(SENSOR_FILENAME, "X, Y, Z\n");
  createFile(WATCHDOG_FILENAME);
}

void createFile(const char *filename) {
  printf("Creating File: %s...\n", filename);
  FIL file;
  FRESULT result = f_open(&file, filename, FA_CREATE_ALWAYS);
  if (result == FR_OK) {
    f_close(&file);
    printf("Finished creating file: %s\n", filename);
  } else {
    printf("Failed to initialize: %s\n", filename);
  }
}

void writeToSDCard(const char *filename, const char *message) {
  FIL file;
  UINT bytes_written = 0;
  FRESULT result = f_open(&file, filename, (FA_WRITE | FA_OPEN_APPEND));

  if (result == FR_OK) {
    if (FR_OK == f_write(&file, message, strlen(message), &bytes_written)) {
    } else {
      printf("ERROR: Failed to write data to file\n");
    }
    f_close(&file);
  } else {
    printf("ERROR: Failed to open: %s\n", filename);
  }
}