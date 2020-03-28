#include "clock.h"
#include "lpc40xx.h"
#include <stdint.h>

void ssp2_init(uint32_t max_clock_mhz) {
  // Refer to LPC User manual and setup the register bits correctly
  // a) Power on Peripheral
  // b) Setup control registers CR0 and CR1
  // c) Setup prescalar register to be <= max_clock_mhz
  printf("Enable SSP2\n");
  printf("Initializing P1.0 as SSP2_SCK\n");
  printf("Initializing P1.1 as SSP2_MOSI\n");
  printf("Initializing P1.4 as SSP2_MISO\n");
  LPC_SC->PCONP |= (1 << 20);

  // SET UP DIVIDER
  // uint8_t divider = 2;
  // const uint32_t cpu_clock_mhz = clock__get_core_clock_hz() / 1000000UL;
  // // Keep scaling down divider until calculated is higher
  // while (max_clock_mhz < (cpu_clock_mhz / divider) && divider <= 254) {
  //   divider += 2;
  // }
  LPC_SSP2->CPSR |= 48;

  // SET UP CR0
  LPC_SSP2->CR0 = 0;
  LPC_SSP2->CR0 |= 7; // 8-BIT MODE
  // LPC_SSP2->CR0 &= (0b00 << 4); // Format to SPI
  // printf("CR0: %i\n", LPC_SSP2->CR0);
  LPC_SSP2->CR0 &= ~(1 << 6); // CPOL, maintain low between frames
  LPC_SSP2->CR0 &= ~(1 << 7); // CPHA, capture on first clock

  // PCLK / (CPSDVSR * [SCR+1])) -> Clearing SCR sets divisor solely on CPSDVSR
  LPC_SSP2->CR0 &= ~(0xFFFF << 8); // Clear SCR to 0
  // printf("CR0: %i\n", LPC_SSP2->CR0);

  // SET UP CR1
  LPC_SSP2->CR1 &= ~(1 << 0); // Normal Operation
  LPC_SSP2->CR1 &= ~(1 << 2); // Set as Master
  LPC_SSP2->CR1 |= (1 << 1);  // Enable SSP2
  // printf("CR1: %i\n", LPC_SSP2->CR1);
}

uint8_t ssp2_transfer(uint8_t data_out) {
  // Configure the Data register(DR) to send and receive data by checking the status register
  LPC_SSP2->DR = data_out;
  while (LPC_SSP2->SR & (1 << 4)) {
    // Wait for BSY bit to clear
    continue;
  }
  printf("FINISHED TRANSFER...\n");
  uint8_t result = LPC_SSP2->DR; // DR holds data read from SPI exchange
  return result;
}