#pragma once 
#ifndef LABSPI_H_
#define LABSPI_H_

#include "../../library/L0_Platform/lpc40xx/LPC40xx.h"
#include "GPIO.hpp"
#include <cstdint>

class Spi
{
 public:
    enum SpiPort : uint8_t
    {
        kPort0      = 0,
        kPort1      = 1,
        kPort2      = 2,
    };
    enum FrameModes : uint8_t
    {
        kSPI = 0b00,
        kTI = 0b01,
        kMicrowire = 0b10
    };
    /**
     * 1) Powers on SPPn peripheral
     * 2) Set peripheral clock
     * 3) Sets pins for specified peripheral to MOSI, MISO, and SCK
     *
     * @param data_size_select transfer size data width; To optimize the code, look for a pattern in the datasheet
     * @param format is the code format for which synchronous serial protocol you want to use.
     * @param divide is the how much to divide the clock for SSP; take care of error cases such as the value of 0, 1, and odd numbers
     *
     * @return true if initialization was successful
     */
    bool Initialize(SpiPort spi, uint8_t data_size_select, FrameModes format, uint8_t divide);

    /**
     * Transfers a byte via SSP to an external device using the SSP data register.
     * This region must be protected by a mutex static to this class.
     *
     * @return received byte from external device via SSP data register.
     */
    uint8_t Transfer(uint8_t send);
    
 
 private:
    sjsu::lpc40xx::LPC_SSP_TypeDef *LPC_SSPx;
};
#endif