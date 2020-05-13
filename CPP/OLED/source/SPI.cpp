#include "SPI.hpp"
#include <stdio.h>
#include "utility/log.hpp"

bool Spi::Initialize(SpiPort spi, uint8_t data_size_select, FrameModes format, uint8_t divide) 
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

    switch (spi)
    {
        case kPort1:
            LOG_INFO("Enable SSP1");
            LOG_INFO("Initializing P0.7 as SSP1_SCK");
            LOG_INFO("Initializing P0.9 as SSP1_MOSI");
            LOG_INFO("Initializing P0.8 as SSP1_MISO");

            sjsu::lpc40xx::LPC_SC->PCONP |=  (1 << 10);  // Enable power for SSP1 (PCSSP1)
            sjsu::lpc40xx::LPC_IOCON->P0_7 = (sjsu::lpc40xx::LPC_IOCON->P0_7 & ~(0b111)) | 0b010; // Sets P0_7 as SSP1_SCK
            sjsu::lpc40xx::LPC_IOCON->P0_9 = (sjsu::lpc40xx::LPC_IOCON->P0_9 & ~(0b111)) | 0b010; // Sets P0_9 as SSP1_MOSI
            sjsu::lpc40xx::LPC_IOCON->P0_8 = (sjsu::lpc40xx::LPC_IOCON->P0_8 & ~(0b111)) | 0b010; // Sets P0_8 as SSP1_MISO

            LPC_SSPx = sjsu::lpc40xx::LPC_SSP1;
            break;
        case kPort2:
            sjsu::LogInfo("Enable SSP2");
            sjsu::LogInfo("Initializing P1.0 as SSP2_SCK");
            sjsu::LogInfo("Initializing P1.1 as SSP2_MOSI");
            sjsu::LogInfo("Initializing P1.4 as SSP2_MISO");
            sjsu::lpc40xx::LPC_SC->PCONP |=  (1 << 20);  // Enable power for SSP2
            sjsu::lpc40xx::LPC_IOCON->P1_0 = (sjsu::lpc40xx::LPC_IOCON->P1_0 & ~(0b111)) | 0b100; // Sets P1_0 as SSP2_SCK
            sjsu::lpc40xx::LPC_IOCON->P1_1 = (sjsu::lpc40xx::LPC_IOCON->P1_1 & ~(0b111)) | 0b100; // Sets P1_1 as SSP2_MOSI
            sjsu::lpc40xx::LPC_IOCON->P1_4 = (sjsu::lpc40xx::LPC_IOCON->P1_4 & ~(0b111)) | 0b100; // Sets P1_4 as SSP2_MISO

            LPC_SSPx = sjsu::lpc40xx::LPC_SSP2;
            break;
        default:
            sjsu::LogError("SPI failed to initialize! Invalid SPI index passed: %i", spi);
            return false;
            break;
    }
    // Initially clear CR0
    LPC_SSPx->CR0 = 0;         
    LPC_SSPx->CR0 |= (data_size_select - 1);    // Set DSS to user defined bit mode
    LPC_SSPx->CR0 |= format;                    // Sets format based on user definition
    LPC_SSPx->CR0 &= ~(1 << 6);                 // Clear CPOL, bus clock low between frames
    LPC_SSPx->CR0 &= ~(1 << 7);                 // Clear CPHA, first transition
    LPC_SSPx->CR0 &= ~(0xFFFF << 8);            // Clear SCR to 0  
    LPC_SSPx->CPSR |= divide;                   // Set CPSR to user divide, ONLY ALLOWS FOR EVEN NUMBERS TO DIVIDE

    // Set CR1
    LPC_SSPx->CR1 &= ~(1);           // Set to normal operation
    LPC_SSPx->CR1 &= ~(1 << 2);      // Set SSP2 as Master
    LPC_SSPx->CR1 |= (1 << 1);       // Enable SSP2

    return true;
}

uint8_t Spi::Transfer(uint8_t send) 
{
    uint8_t result_byte = 0;

    // Set SSP2 Data Register to send value
    LPC_SSPx->DR = send; 

    while(LPC_SSPx->SR & (1 << 4))
    {
        continue;   // BSY is set, currently sending/receiving frame
    }

    // When BSY bit is set, SSP2 Data Register holds value read from device.
    result_byte = LPC_SSPx->DR;
    return result_byte;
}

