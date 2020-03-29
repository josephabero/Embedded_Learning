#include "SPI.hpp"
#include <stdio.h>
#include "utility/log.hpp"

bool Spi::Initialize(uint8_t data_size_select, FrameModes format, uint8_t divide) 
{
     // Initialize SSP peripheral
    // 1. Power (PCONP register. set PCSSP2!)
    // 2. Set Peripheral Clock
    // 3. Set SSP pins
    // IOCON_P1_0 -> 100 for SSP2_SCK
    // IOCON_P1_1 -> 100 for SSP2_MOSI
    // IOCON_P1_4 -> 100 for SSP2_MISO
    if (divide < 2 || divide & (1 << 0)) //check if divide is 0 or 1 or if even number by checking LSB
    {
        sjsu::LogWarning("SSP interface FAILED to initialize: Value of divisor is invalid! Must be an even number between 2 and 254.");
        return false;
    }

    sjsu::LogInfo("Enable SSP2");
    sjsu::LogInfo("Initializing P1.0 as SSP2_SCK");
    sjsu::LogInfo("Initializing P1.1 as SSP2_MOSI");
    sjsu::LogInfo("Initializing P1.4 as SSP2_MISO");
    sjsu::lpc40xx::LPC_SC->PCONP |=  (1 << 20);                            // Enable power for SSP2
    sjsu::lpc40xx::LPC_IOCON->P1_0 = (sjsu::lpc40xx::LPC_IOCON->P1_0 & ~(0b111)) | 0b100; // Sets P1_0 as SSP2_SCK
    sjsu::lpc40xx::LPC_IOCON->P1_1 = (sjsu::lpc40xx::LPC_IOCON->P1_1 & ~(0b111)) | 0b100; // Sets P1_1 as SSP2_MOSI
    sjsu::lpc40xx::LPC_IOCON->P1_4 = (sjsu::lpc40xx::LPC_IOCON->P1_4 & ~(0b111)) | 0b100; // Sets P1_4 as SSP2_MISO

    // Initially clear CR0
    sjsu::lpc40xx::LPC_SSP2->CR0 = 0;         
    sjsu::lpc40xx::LPC_SSP2->CR0 |= (data_size_select - 1);    // Set DSS to user defined bit mode
    sjsu::lpc40xx::LPC_SSP2->CR0 |= format;                    // Sets format based on user definition
    sjsu::lpc40xx::LPC_SSP2->CR0 &= ~(1 << 6);                 // Clear CPOL, bus clock low between frames
    sjsu::lpc40xx::LPC_SSP2->CR0 &= ~(1 << 7);                 // Clear CPHA, first transition
    sjsu::lpc40xx::LPC_SSP2->CR0 &= ~(0xFFFF << 8);            // Clear SCR to 0  
    sjsu::lpc40xx::LPC_SSP2->CPSR |= divide;                   // Set CPSR to user divide, ONLY ALLOWS FOR EVEN NUMBERS TO DIVIDE

    // Set CR1
    sjsu::lpc40xx::LPC_SSP2->CR1 &= ~(1);           // Set to normal operation
    sjsu::lpc40xx::LPC_SSP2->CR1 &= ~(1 << 2);      // Set SSP2 as Master
    sjsu::lpc40xx::LPC_SSP2->CR1 |= (1 << 1);       // Enable SSP2

    return true;
}

uint8_t Spi::Transfer(uint8_t send) 
{
    uint8_t result_byte = 0;

    // Set SSP2 Data Register to send value
    sjsu::lpc40xx::LPC_SSP2->DR = send; 

    while(sjsu::lpc40xx::LPC_SSP2->SR & (1 << 4))
    {
        continue;   // BSY is set, currently sending/receiving frame
    }

    // When BSY bit is set, SSP2 Data Register holds value read from device.
    result_byte = sjsu::lpc40xx::LPC_SSP2->DR;
    return result_byte;
}

