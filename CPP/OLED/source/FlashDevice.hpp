#pragma once

#ifndef FLASHDEVICE_H_
#define FLASHDEVICE_H_

#include "GPIO.hpp"
#include "SPI.hpp"

class FlashDevice
{
 public:
    typedef union
    {
        struct
        {
            uint8_t byte1;
            uint8_t byte2;
        };
        struct
        {
            uint8_t ready_busy_status: 1;
            uint8_t write_enable_latch_status: 1;
            uint8_t block_protection: 1;
            uint8_t : 1;  // Reserved for future use
            uint8_t write_protect_pin_status: 1;
            uint8_t erase_program_error: 1;
            uint8_t : 1;  // Reserved for future use
            uint8_t block_protection_locked: 1;

            uint8_t ready_busy_status_2 : 1;
            uint8_t : 1;  // Reserved for future use
            uint8_t : 1;  // Reserved for future use
            uint8_t : 1;  // Reserved for future use
            uint8_t reset_enabled: 1;
            uint8_t : 1;  // Reserved for future use
            uint8_t : 1;  // Reserved for future use
            uint8_t : 1;  // Reserved for future use
        } __attribute__((packed));
    } __attribute__((packed)) StatusReg;

    FlashDevice();
    void init();
    void ReadSignature();
    void WriteEnableCommand();
    void WriteDisableCommand();
    StatusReg GetStatusRegister();

    void WriteByte(uint32_t address, uint8_t data);
    uint8_t ReadByte(uint32_t address);
    void PageErase(uint8_t address);
    void BlockErase(uint8_t address);

 private:
    GPIO FLASH_CS;
    GPIO MIRROR_CS;
    Spi SPI;
    void CS();
    void DS();
};
#endif
