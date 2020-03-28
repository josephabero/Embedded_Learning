#include "GPIO.hpp"

GPIO::GPIO(uint8_t port, uint32_t pin)
{
    _port = port;
    _pin = pin;

    switch (port)
    {
        case 0:
            gpio_port = sjsu::lpc40xx::LPC_GPIO0;
            break; 
        case 1:
            gpio_port = sjsu::lpc40xx::LPC_GPIO1;

            // Enable Pull Down Resistor for SW0 (P1.19) and SW1 (P1.15)
            if(pin == 15)
            {
                // CLEAR IOCON, then set to 0x01
                sjsu::lpc40xx::LPC_IOCON->P1_15 &= ~(0x3 << 3);
                sjsu::lpc40xx::LPC_IOCON->P1_15 |= (0x1 << 3);
            }
            else if(pin == 19)
            {
                // CLEAR IOCON, then set to 0x01
                sjsu::lpc40xx::LPC_IOCON->P1_19 &= ~(0x3 << 3);
                sjsu::lpc40xx::LPC_IOCON->P1_19 |= (0x1 << 3);
            }
            break;
        case 2:
            gpio_port = sjsu::lpc40xx::LPC_GPIO2;
            break;
        case 3:
            gpio_port = sjsu::lpc40xx::LPC_GPIO3;
            break;
        case 4:
            gpio_port = sjsu::lpc40xx::LPC_GPIO4;
            break;
        case 5:
            gpio_port = sjsu::lpc40xx::LPC_GPIO5;
            break;
    }
}

void GPIO::SetAsInput()
{
    gpio_port->DIR &= ~(1 << _pin);
}

void GPIO::SetAsOutput()
{
    gpio_port->DIR |= (1 << _pin);
}

void GPIO::SetDirection(Direction direction)
{
    if(direction == Direction::kOutput)
    {
        SetAsOutput();
    }
    else
    {
        SetAsInput();
    }
}

void GPIO::SetHigh()
{
    gpio_port->PIN |= (1 << _pin); 
}

void GPIO::SetLow()
{
    gpio_port->PIN &= ~(1 << _pin); 
}

void GPIO::set(State state)
{
    if(state == State::kHigh)
    {
        SetHigh();
    }
    else
    {
        SetLow();
    }
}

GPIO::State GPIO::Read()
{
    GPIO::State result_state;
    if(gpio_port->PIN & (1 << _pin))
    {
        result_state = GPIO::State::kHigh;
    }
    else
    {
        result_state = GPIO::State::kLow;
    }
    return result_state;
}

bool GPIO::ReadBool()
{
    bool result;
    if(gpio_port->PIN & (1 << _pin))
    {
        result = true;
    }
    else
    {
        result = false;
    }
    return result;
}
