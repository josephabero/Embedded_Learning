#include <project_config.hpp>

#include <cstdint>
#include "utility/log.hpp"
#include "utility/time.hpp"
#include "GPIO.hpp"

// Declared globally to be used in ISR functions
GPIO LED0(1, 18);
GPIO LED1(1, 24);

void toggleLEDState(GPIO *gpio_pin);
void button0ISR();
void button1ISR();

void toggleLEDState(GPIO *gpio_pin)
{
    if(gpio_pin->ReadBool())
    {
        gpio_pin->SetLow();
    }
    else
    {
        gpio_pin->SetHigh();
    }
}

void button0ISR()
{
    LOG_INFO("button 0 ISR!");
    toggleLEDState(&LED0);
}

void button1ISR()
{
    LOG_INFO("button 1 ISR!");
    toggleLEDState(&LED1);
}

int main(void)
{
    GPIO::Init();
    GPIO::EnableInterrupts();
    LED0.SetAsOutput();
    LED1.SetAsOutput();

    GPIO button0(0, 29);
    button0.SetAsInput();
    button0.AttachInterruptHandler(button0ISR, GPIO::Edge::kFalling);

    GPIO button1(0, 30);
    button1.SetAsInput();
    button1.AttachInterruptHandler(button1ISR, GPIO::Edge::kBoth);

    while (1) 
    {
        continue; // Empty loop just to test the interrupt
    }
}
