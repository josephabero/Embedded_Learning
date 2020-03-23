#include "l5_application/uart3.h"

// #####################################################
// #                                                   #
// #                 GLOBAL VARIABLES                  #
// #                                                   #
// #####################################################
static QueueHandle_t UART3_RX_QUEUE;

void uart3_init(uart_number_e uart, uint32_t peripheral_clock, uint32_t baud_rate) {
  // Refer to LPC User manual and setup the register bits correctly
  // The first page of the UART chapter has good instructions
  // a) Power on Peripheral
  // b) Setup DLL, DLM, FDR, LCR registers

  switch (uart) {
  case UART2:
    // LPC_SC->PCONP |= (1 << 24); // Power on UART2
    // LPC_UART2->LCR |= (1 << 7); // Enable Divisor Lab Access Bit (DLAB)

    // LPC_UART2->DLL = 0x39;
    // LPC_UART2->DLM = 0x01;

    // LPC_UART2->DLL |= baud_rate & 0xFF;        // Set LSB to first 8 bits
    // LPC_UART2->DLM |= (baud_rate >> 8) & 0xFF; // Set MSB to upper 8 bits

    // LPC_UART2->LCR &= ~(1 << 7); // Enable Divisor Lab Access Bit (DLAB)
    // LPC_UART2->LCR |= 0b11;      // Set UART to 8-bit mode

    // LPC_UART2->FCR &= ~(1 << 0);
    break;
  case UART3:
    LPC_SC->PCONP |= (1 << 25); // Power on UART2
    LPC_UART3->LCR |= (1 << 7); // Enable Divisor Lab Access Bit (DLAB)

    LPC_UART3->DLL |= baud_rate & 0xFF;        // Set LSB to first 8 bits
    LPC_UART3->DLM |= (baud_rate >> 8) & 0xFF; // Set MSB to upper 8 bits

    LPC_UART3->LCR &= ~(1 << 7); // Disable Divisor Lab Access Bit (DLAB)
    LPC_UART3->LCR |= 0b11;      // Set UART to 8-bit mode
    break;
  }
}

bool uart_polled_get(uart_number_e uart, char *input_byte) {
  bool status = false;
  // Check Status Register (LSR) for Receive Data Ready
  if (LPC_UART3->LSR & (1 << 0)) {
    *input_byte = LPC_UART3->RBR;
    status = true;
  }
  return status;
}

bool uart_polled_put(uart_number_e uart, char output_byte) {
  bool status = false;
  // Check Status Register (LSR) for Transmit Hold Register Empty
  if (LPC_UART3->LSR & (1 << 5)) {
    LPC_UART3->THR = output_byte;
    while (!(LPC_UART3->LSR & (1 << 5))) {
    }
    status = true;
  }
  return status;
}

// Private function of our uart_lab.c
static void UART_Receive_ISR(void) {
  // TODO: Read the IIR register to figure out why you got interrupted
  // TODO: Based on IIR status, read the LSR register to confirm if there is data to be read
  if ((((LPC_UART3->IIR >> 1) & 0b111) == 0x2) && (LPC_UART3->LSR & (1 << 0))) {
    // TODO: Based on LSR status, read the RBR register and input the data to the RX Queue
    const char byte = LPC_UART3->RBR;
    xQueueSendFromISR(UART3_RX_QUEUE, &byte, NULL);
  }
  // fprintf(stderr, "Print in ISR.\n");
}

// Public function to enable UART interrupt
// TODO Declare this at the header file
void uart_enable_receive_interrupt(uart_number_e uart_number) {
  // TODO: Use lpc_peripherals.h to attach your interrupt
  lpc_peripheral__enable_interrupt(LPC_PERIPHERAL__UART3, UART_Receive_ISR);

  // TODO: Enable UART receive interrupt by reading the LPC User manual
  // Hint: Read about the IER register
  LPC_UART3->LCR &= ~(1 << 7); // Disable Divisor Lab Access Bit (DLAB) to access IER
  LPC_UART3->IER |= (1 << 0);  // Enable Interrupt for RDR

  LPC_UART3->FCR |= (1 << 0);     // Enable RX/TX FIFO
  LPC_UART3->FCR &= ~(0b11 << 6); // RX Trigger Level 0 (1 Character received before INTR)

  // TODO: Create your RX queue
  UART3_RX_QUEUE = xQueueCreate(1, sizeof(char));
}

// Public function to get a char from the queue (this function should work without modification)
// TODO: Declare this at the header file
bool uart_get_char_from_queue(char *input_byte, uint32_t timeout) {
  return xQueueReceive(UART3_RX_QUEUE, input_byte, timeout);
}