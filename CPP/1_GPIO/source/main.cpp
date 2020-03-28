#include <cstdint>

#include "utility/log.hpp"
#include "utility/time.hpp"

#include "GPIO.hpp"

void initializeGPIO(GPIO switches[], GPIO LEDs[]);
void buttonPress(GPIO switch_pin, GPIO LED_pin);
void toggleLEDState(GPIO gpio_pin, GPIO::State prevGPIOState);

int main()
{
    sjsu::LogInfo("Starting Lab 1");
    GPIO gpio_LED[] = 
    {
        GPIO(1, 18),
        GPIO(1, 24),
        GPIO(1, 26),
        GPIO(2, 3)
    };

    GPIO gpio_SW[] = 
    {
        GPIO(0, 29),
        GPIO(0, 30),
        GPIO(1, 15),
        GPIO(1, 19)
    };
    
    initializeGPIO(gpio_SW, gpio_LED);

    while(1)
    {
        buttonPress(gpio_SW[3], gpio_LED[3]);
        buttonPress(gpio_SW[2], gpio_LED[2]);
        buttonPress(gpio_SW[1], gpio_LED[1]);
        buttonPress(gpio_SW[0], gpio_LED[0]);
    }
}

void initializeGPIO(GPIO switches[], GPIO LEDs[])
{
    sjsu::LogInfo("SETTING SWITCHES AS INPUT");
    for(uint8_t i = 0; i < 4; i++)
    {
        switches[i].SetAsInput();
    }

    sjsu::LogInfo("SETTING LEDs AS OUTPUT");
    for(uint8_t i=0; i < 4; i++)
    {
        LEDs[i].SetAsOutput();
    }
}

void buttonPress(GPIO switch_pin, GPIO LED_pin)
{
    if(switch_pin.ReadBool())  // Button Pressed
    {
        GPIO::State currButtonState = switch_pin.Read();
        GPIO::State prevButtonState = currButtonState;
        GPIO::State currLEDState = LED_pin.Read();
        sjsu::Delay(5ms);
        currButtonState = switch_pin.Read();
        
        if(currButtonState != prevButtonState) 
        {
            sjsu::LogInfo("Button pressed, TOGGLE");
            currButtonState = switch_pin.Read();
            toggleLEDState(LED_pin, currLEDState);
        }
    }
}


void toggleLEDState(GPIO gpio_pin, GPIO::State prevGPIOState)
{
    if(prevGPIOState == GPIO::State::kHigh) //current sstate is kHigh
    {
        gpio_pin.SetLow();
    }
    else
    {
        gpio_pin.SetHigh();
    }
}
