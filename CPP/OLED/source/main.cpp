#include "GPIO.hpp"
#include "SPI.hpp"
#include "FlashDevice.hpp"
#include "ssd1306.hpp"

#include <project_config.hpp>

#include "utility/log.hpp"
#include "utility/time.hpp"

void evaluateStatusRegInfo(FlashDevice::StatusReg status_reg);

int main()
{
    sjsu::LogInfo("Booted Up...");

    ssd1306 OLED;

    sjsu::LogInfo("Initializing OLED...");
    OLED.Init();
}
