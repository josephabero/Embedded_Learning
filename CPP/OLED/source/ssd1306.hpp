#pragma once

#include "GPIO.hpp"
#include "SPI.hpp"

class ssd1306 {
 public:
    ssd1306();
    bool Init();

 private:
    enum OledCommand : uint8_t
    {
        // Fundamental Commands
        kSetContrastControl     = 0x81,
        kDisableEntireDisplayOn = 0xA4,
        kEnableEntireDisplayOn  = 0xA5,
        kSetNormalDisplay       = 0xA6,
        kSetInverseDisplay      = 0xA7,
        kSetDisplayOff          = 0xAE,
        kSetDisplayOn           = 0xAF,

        // Address Setting Commands
        kSetMemoryAddressingMode        = 0x20,
        kSetPageAddress                 = 0x22,
        kSetPageStartAddressForPageMode = 0xB0

        // Hardware Configuration
        kSetDisplayStartLine             = 0x40,
        kSetSegmentRemapTo0              = 0xA0,
        kSetSegmentRemapTo127            = 0xA1,
        kSetMultiplexRatio               = 0xA8,
        kSetCOMOutputDirectionToNormal   = 0xC0,
        kSetCOMOutputDirectionToRemapped = 0xC8,
        kSetDisplayOffset                = 0xD3,
        kSetCOMHardwareConfig            = 0xDA,

        // Timing & Driving Setting
        kSetDivideRatioAndOscFrequency = 0xD5,

        // Charge Pump
        kSetChargePumpSetting = 0x8D
    };

    enum Page : uint8_t
    {
        kPage0 = 0,
        kPage1 = 1,
        kPage2 = 2,
        kPage3 = 3,
        kPage4 = 4,
        kPage5 = 5,
        kPage6 = 6,
    }

    enum MemoryAddressingMode : uint8_t
    {
        kSetHorizontalAddressingMode = 0x00,
        kSetVerticalAddressingMode   = 0x01,
        kSetPageAddressingMode       = 0x10
    };

    enum ContrastControl : uint8_t
    {
        kReset = 0x7F
    };

    enum COMHardwareConfig : uint8_t
    {
        kAltCOMAndDisableCOMRemap        = 0x02,
        kSequentialCOMAndEnableCOMRemap  = 0x12,
        kAltCOMAndEnableCOMRemap         = 0x22
    };

    enum ChargePumpSetting : uint8_t
    {
        kEnable  = 0x14,
        kDisable = 0x10
    };

    void WriteCommand(uint8_t data);
    void WriteData(uint8_t data);

    // R/W' tied to LOW
    uint8_t ReadStatus();
    uint8_t ReadData();

    void SetMemoryAddressingMode(MemoryAddressingMode address_mode);
    void SetPageStartAddressForPageMode(Page page);

    void SetContrastControl(uint8_t contrast);
    void SetMuxRatio(uint8_t ratio);
    void SetDisplayOffset(uint8_t offset);
    void SetDisplayStartLine(uint8_t line);
    void SetSegmentRemap(uint8_t segment);
    void SetCOMHardwareConfig(COMHardwareConfig config);
    void SetOscFrequency(uint8_t frequency);
    void SetChargePumpSetting(ChargePumpSetting setting);

    void Enable();
    void Disable();

    void ConfigureSoftware();

    Spi SPI;
    GPIO CS;
    GPIO DC;
};
