#include "GPIO.hpp"
#include "SPI.hpp"
#include "FlashDevice.hpp"

#include <project_config.hpp>

#include "utility/log.hpp"
#include "utility/time.hpp"

void evaluateStatusRegInfo(FlashDevice::StatusReg status_reg);

int main()
{
    sjsu::LogInfo("STARING LAB 3");
    
    FlashDevice adesto;
    FlashDevice::StatusReg status_reg;
    const uint8_t size = 38;

    // Initialize Flash Device
    adesto.init();

    // Use SPI driver to read manufacturer ID & 16-bit device ID from Flash device
    sjsu::LogInfo("\nREADING MANUFACTURER AND DEVICE ID");
    adesto.ReadSignature();


    // Set up initial variables to write and read from Adesto device
    char group_name[size] = "git_rekt, joseph abero, udit chophla";
    char read_data[size] = {};
    uint32_t address = 0x00;

    // Initially erase information from addresses to write to
    adesto.WriteEnableCommand();
    adesto.BlockErase(address & 0xFF);
    sjsu::Delay(500ms);

    // Write each 8-bit character to each address
    for(int i = 0; i < size; i++)
    {
        adesto.WriteEnableCommand();
        adesto.WriteByte(address, group_name[i]);
        address += 2;
    }
    
    // Reset address value and Read the characters back from the Adesto device
    address = 0x00;
    for(int i = 0; i < size; i++)
    {
        read_data[i] = adesto.ReadByte(address);
        address += 2;
    }

    // Print final value read back from device
    sjsu::LogInfo("FINISHED READING WRITTEN DATA");
    sjsu::LogInfo("Data Read Back: %s\n", read_data);
    
    // 1. Read Adesto Status Register
    // 2. Evaluate each relevant bit
    // 3. Output datasheet statement based on status bit value
    // 4. Eval if WEL == 1
    // 5. Toggle WEL status register value with WriteEnableCommand or WriteDisableCommand
    while(1)
    {
        status_reg = adesto.GetStatusRegister();
        evaluateStatusRegInfo(status_reg);
        sjsu::Delay(1s);

        if(status_reg.write_enable_latch_status)
        {
            adesto.WriteDisableCommand();
        }
        else
        {
            adesto.WriteEnableCommand();
        }
    }
}

void evaluateStatusRegInfo(FlashDevice::StatusReg status_reg)
{
    sjsu::LogInfo("EVALUATING STATUS REG INFO.");
    if(status_reg.ready_busy_status)
    {
        sjsu::LogInfo("Device is busy with an internal operation.");
    }
    else
    {
        sjsu::LogInfo("Device is ready.");
    }

    if(status_reg.write_enable_latch_status)
    {
        sjsu::LogInfo("Device is write enabled.");
    }
    else
    {
        sjsu::LogInfo("Device is NOT write enabled (default).");
    }

    if(status_reg.block_protection)
    {
        sjsu::LogInfo("Entire memory array is protected.");
    }
    else
    {
        sjsu::LogInfo("Entire memory array is unprotected.");
    }

    if(status_reg.write_protect_pin_status)
    {
        sjsu::LogInfo("WP' is deasserted.");
    }
    else
    {
        sjsu::LogInfo("WP' is asserted.");
    }

    if(status_reg.erase_program_error)
    {
        sjsu::LogInfo("Erase or program error detected.");
    }
    else
    {
        sjsu::LogInfo("Erase or program operation was successful.");
    }

    if(status_reg.block_protection_locked)
    {
        sjsu::LogInfo("BP0 bit locked in current state when WP' asserted.");
    }
    else
    {
        sjsu::LogInfo("BP0 bit unlocked (default).");
    }

    if(status_reg.ready_busy_status_2)
    {
        sjsu::LogInfo("Device is busy with an internal operation.");
    }
    else
    {
        sjsu::LogInfo("Device is ready.");
    }

    if(status_reg.reset_enabled)
    {
        sjsu::LogInfo("Reset command is enabled.");
    }
    else
    {
        sjsu::LogInfo("Reset command is disabled (default).");
    }
}