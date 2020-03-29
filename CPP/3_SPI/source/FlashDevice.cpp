#include "FlashDevice.hpp"
#include <stdio.h>
#include "utility/log.hpp"

FlashDevice::FlashDevice() : FLASH_CS(1, 10), MIRROR_CS(0, 6)
{
    // nothing
}

void FlashDevice::CS()
{
    FLASH_CS.SetLow();
    MIRROR_CS.SetLow();
}

void FlashDevice::DS()
{
    FLASH_CS.SetHigh();
    MIRROR_CS.SetHigh();
}

void FlashDevice::init()
{
    FLASH_CS.SetAsOutput();
    MIRROR_CS.SetAsOutput();
    DS();
    SPI.Initialize(8, Spi::FrameModes::kSPI, 48);
}

FlashDevice::StatusReg FlashDevice::GetStatusRegister() 
{
    StatusReg status_reg;
    CS();
    SPI.Transfer(0x05);                         // Send opcode 0x05 (Read Status Register)
    status_reg.byte1 = SPI.Transfer(0xFF);      // Read SR Byte 1
    status_reg.byte2 = SPI.Transfer(0xFF);      // Read SR Byte 2
    DS();

    return status_reg;
} 

void FlashDevice::WriteEnableCommand()
{
    CS();
    SPI.Transfer(0x06);
    DS();
}

void FlashDevice::WriteDisableCommand()
{
    CS();
    SPI.Transfer(0x04);
    DS();
}

void FlashDevice::ReadSignature()
{
    uint8_t data[3];
    CS();
    SPI.Transfer(0x9F);
    data[0] = SPI.Transfer(0xFF);
    data[1] = SPI.Transfer(0xFF);
    data[2] = SPI.Transfer(0xFF);
    DS();
  
    sjsu::LogInfo("Manufacturer ID: %02x Device ID: %02x%02x\n", data[0], data[1], data[2]);
}

void FlashDevice::WriteByte(uint32_t address, uint8_t data)
{
    StatusReg SR = GetStatusRegister();
    if(SR.write_enable_latch_status)
    {
        CS();
        SPI.Transfer(0x02);                     // send opcode
        SPI.Transfer((address >> 16) & 0xFF);   // send A23 - A16
        SPI.Transfer((address >> 8) & 0xFF);    // send A15 - A8
        SPI.Transfer((address >> 0) & 0xFF);    // send A7 - A0
        SPI.Transfer(data);                     // send data to write
        DS();
    }
    else
    {
        sjsu::LogWarning("Write operation FAILED. Write Enable Command must be issued before writing.");
    }
}

uint8_t FlashDevice::ReadByte(uint32_t address)
{
    uint8_t data_read;
    CS();
    SPI.Transfer(0x03);                     // send opcode
    SPI.Transfer((address >> 16) & 0xFF);   // send A23 - A16
    SPI.Transfer((address >> 8) & 0xFF);    // send A15 - A8
    SPI.Transfer((address >> 0) & 0xFF);    // send A7 - A0
    data_read = SPI.Transfer(0xFF);                     // store read data
    DS();
    return data_read;
}

void FlashDevice::PageErase(uint8_t address)
{
    CS();
    SPI.Transfer(0x81);
    SPI.Transfer(0xFF);
    SPI.Transfer(address);
    SPI.Transfer(0xFF);
    DS();
}

void FlashDevice::BlockErase(uint8_t address)
{
    StatusReg SR = GetStatusRegister();
    if(SR.write_enable_latch_status)
    {
        CS();
        SPI.Transfer(0x20);
        SPI.Transfer((address >> 16) & 0xFF);   // send A23 - A16
        SPI.Transfer((address >> 8) & 0xFF);    // send A15 - A8
        SPI.Transfer((address >> 0) & 0xFF);    // send A7 - A0
        DS();
    }
    else
    {
        sjsu::LogWarning("Block Erase operation FAILED. Write Enable Command must be issued beforehand.");        
    }
}
