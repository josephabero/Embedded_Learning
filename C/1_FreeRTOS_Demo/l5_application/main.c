#include <stdio.h>

#include "FreeRTOS.h"
#include "task.h"

static void task_one(void *task_parameter);
static void task_two(void *task_parameter);

int main(void) {
  /**
   * Observe and explain the following scenarios:
   *
   * 1) Same Priority:      task_one = 1, task_two = 1
   * 2) Different Priority: task_one = 2, task_two = 1
   * 3) Different Priority: task_one = 1, task_two = 2
   *
   * Note: Priority levels are defined at FreeRTOSConfig.h
   * Higher number = higher priority
   *
   * Turn in screen shots of what you observed
   * as well as an explanation of what you observed
   */
  xTaskCreate(task_one, "TASK_ONE", 4096 / sizeof(void *), NULL, 1, NULL);
  xTaskCreate(task_two, "TASK_TWO", 4096 / sizeof(void *), NULL, 2, NULL);
  // xTaskCreate(task_three, "TASK_TWO", 4096 / sizeof(void *), NULL, 2, NULL);
  /* Start Scheduler - This will not return, and your tasks will start to run their while(1) loop */
  vTaskStartScheduler();

  return 0;
}

static void task_one(void *task_parameter) {
  while (1) {
    // Read existing main.c regarding when we should use fprintf(stderr...) in place of printf()
    // For this lab, we will use fprintf(stderr, ...)
    fprintf(stderr, "AAAAAAAAAAAA");

    // Sleep for 100ms
    vTaskDelay(100);
  }
}

static void task_two(void *task_parameter) {
  while (1) {
    fprintf(stderr, "bbbbbbbbbbbb");
    // vTaskDelay(100);
  }
}