#include <stdio.h>
#include "utility/log.hpp"
#include "ssd1306.hpp"

ssd1306::ssd1306() : CS(1, 22), DC(1, 25)
{
    // nothing
}

bool ssd1306::Init()
{
    CS.SetAsOutput();
    DC.SetAsOutput();

    Disable();

    SPI.Initialize(Spi::SpiPort::kPort1, 8, Spi::FrameModes::kSPI, 48);

    sjsu::LogInfo("SPI Initialized");

    sjsu::LogInfo("Configuring OLED...");
    ConfigureSoftware();
    sjsu::LogInfo("OLED Configured");

    
    return true;
}

void ssd1306::WriteCommand(uint8_t data)
{
    Enable();
    DC.SetLow();

    sjsu::LogInfo("Sending %02X", data);
    SPI.Transfer(data);

    Disable();
}

void ssd1306::WriteData(uint8_t data)
{
    Enable();
    DC.SetHigh();

    sjsu::LogInfo("SPI Transfer...");
    SPI.Transfer(data);

    sjsu::LogInfo("Transfer Completed...");
    Disable();
}

void ssd1306::Enable()
{
    sjsu::LogInfo("Enable");
    CS.SetLow();
}

void ssd1306::Disable()
{
    CS.SetHigh();
}


// ###############################################
// #                                             #
// #                OLED COMMANDS                #
// #                                             #
// ###############################################
void ssd1306::SetMemoryAddressingMode(MemoryAddressingMode address_mode)
{
    WriteCommand(OledCommand::kSetMemoryAddressingMode);
    WriteCommand(address_mode);
}

void ssd1306::SetContrastControl(uint8_t contrast)
{
    WriteCommand(OledCommand::kSetContrastControl);
    WriteCommand(contrast);
}

void ssd1306::SetMuxRatio(uint8_t ratio)
{
    WriteCommand(OledCommand::kSetMultiplexRatio);
    WriteCommand(ratio);
}

void ssd1306::SetDisplayOffset(uint8_t offset)
{
    WriteCommand(OledCommand::kSetDisplayOffset);
    WriteCommand(offset);
}

void ssd1306::SetDisplayStartLine(uint8_t line)
{
    const uint8_t MIN_START_LINE = 0x40;
    const uint8_t MAX_START_LINE = 0x7F;
    // line range: 0x40 -> 0x7F
    if(line <= (MAX_START_LINE - MIN_START_LINE)){
        WriteCommand(OledCommand::kSetDisplayStartLine | line);
    }
    else{
        LOG_ERROR("SetDisplayStartLine FAILED! Invalid start line: %i", line);
    }
}

void ssd1306::SetCOMHardwareConfig(COMHardwareConfig config)
{
    WriteCommand(OledCommand::kSetCOMHardwareConfig);
    WriteCommand(config);
}

void ssd1306::SetOscFrequency(uint8_t frequency)
{
    WriteCommand(OledCommand::kSetDivideRatioAndOscFrequency);
    WriteCommand(frequency);
}

void ssd1306::SetChargePumpSetting(ChargePumpSetting setting)
{
    WriteCommand(OledCommand::kSetChargePumpSetting);
    WriteCommand(setting);
}

void ssd1306::ConfigureSoftware()
{
    SetMuxRatio(0x3F);
    SetDisplayOffset(0x00);
    SetDisplayStartLine(0x00);
    WriteCommand(OledCommand::kSetSegmentRemapTo0);
    WriteCommand(OledCommand::kSetCOMOutputDirectionToNormal);
    SetCOMHardwareConfig(COMHardwareConfig::kAltCOMAndDisableCOMRemap);
    SetContrastControl(0x7F);
    WriteCommand(OledCommand::kDisableEntireDisplayOn);
    WriteCommand(OledCommand::kSetNormalDisplay);
    SetOscFrequency(0x80);
    SetChargePumpSetting(ChargePumpSetting::kEnable);
    WriteCommand(OledCommand::kSetDisplayOn);
}
