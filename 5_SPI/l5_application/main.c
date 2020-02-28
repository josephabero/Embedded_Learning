#include "FreeRTOS.h"
#include "task.h"
#include <stdio.h>

#include "l5_application/gpio.h"
#include "l5_application/ssp2.h"

// TODO: Implement Adesto flash memory CS signal as a GPIO driver
void adesto_cs(void);
void adesto_ds(void);

static gpio_s FLASH_CS = {1, 10};
static gpio_s MIRROR_CS = {0, 6};

// TODO: Study the Adesto flash 'Manufacturer and Device ID' section
typedef struct {
  uint8_t manufacturer_id;
  uint8_t device_id_1;
  uint8_t device_id_2;
  uint8_t extended_device_id;
} adesto_flash_id_s;

void adesto_cs() {
  printf("FLASH_CS LOW\n");
  gpio_set_low(FLASH_CS);
  gpio_set_low(MIRROR_CS);
}

void adesto_ds() {
  printf("FLASH_CS HIGH\n");
  gpio_set_high(FLASH_CS);
  gpio_set_high(MIRROR_CS);
}

// TODO: Implement the code to read Adesto flash memory signature
// TODO: Create struct of type 'adesto_flash_id_s' and return it
adesto_flash_id_s adesto_read_signature(void) {
  adesto_flash_id_s data = {0};

  adesto_cs();
  // SEND 0x01
  ssp2_transfer(0x9F);
  data.manufacturer_id = ssp2_transfer(0xFF);
  data.device_id_1 = ssp2_transfer(0xFF);
  data.device_id_2 = ssp2_transfer(0xFF);
  data.extended_device_id = ssp2_transfer(0xFF);
  adesto_ds();

  return data;
}

// TODO: Read Adesto flash datasheet to read its 'STATUS' register
// uint8_t adesto_read_status(void) {}

void spi_task(void *p) {
  uint32_t spi_clock_mhz = 24;
  ssp2_init(spi_clock_mhz);

  // From the LPC schematics pdf, find the pin numbers connected to flash memory
  // Read table 84 from LPC User Manual and configure PIN functions for SPI2 pins
  // You can use gpio__construct_with_function() API from gpio.h
  //
  // Note: Configure only SCK2, MOSI2, MISO2.
  // CS will be a GPIO output pin(configure and setup direction)
  gpio_set_as_output(FLASH_CS);
  gpio_set_as_output(MIRROR_CS);
  adesto_ds();

  while (1) {
    adesto_flash_id_s id = adesto_read_signature();
    printf("MANF ID: %i\n", id.manufacturer_id);
    printf("DEV_ID_1: %i\n", id.device_id_1);
    printf("DEV_ID_2: %i\n", id.device_id_2);
    printf("EXT_DEV_ID: %i\n", id.extended_device_id);
    // TODO: printf the members of the 'adesto_flash_id_s' struct

    vTaskDelay(500);
  }
}

int main(void) {
  xTaskCreate(spi_task, "spi", 2048, NULL, PRIORITY_LOW, NULL);
  vTaskStartScheduler();
}