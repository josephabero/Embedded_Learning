#include "GPIO.hpp"
#include "utility/log.hpp"

sjsu::InterruptHandler GPIO::pin_isr_map[GPIO::kPorts][GPIO::kPins] = { nullptr };

GPIO::GPIO(uint8_t port, uint32_t pin)
{
    _port = port;
    _pin = pin;

    switch (_port)
    {
        case 0:
            LPC_GPIOx = sjsu::lpc40xx::LPC_GPIO0;
            IOxIntEnR = &sjsu::lpc40xx::LPC_GPIOINT->IO0IntEnR;
            IOxIntEnF = &sjsu::lpc40xx::LPC_GPIOINT->IO0IntEnF;
            break; 
        case 1:
            LPC_GPIOx = sjsu::lpc40xx::LPC_GPIO1;
            break;
        case 2:
            LPC_GPIOx = sjsu::lpc40xx::LPC_GPIO2;
            IOxIntEnR = &sjsu::lpc40xx::LPC_GPIOINT->IO2IntEnR;
            IOxIntEnF = &sjsu::lpc40xx::LPC_GPIOINT->IO2IntEnF;
            break;
        case 3:
            LPC_GPIOx = sjsu::lpc40xx::LPC_GPIO3;
            break;
        case 4:
            LPC_GPIOx = sjsu::lpc40xx::LPC_GPIO4;
            break;
        case 5:
            LPC_GPIOx = sjsu::lpc40xx::LPC_GPIO5;
            break;
        default:
             sjsu::LogError("Invalid GPIO port: %d", _port);
            exit(1);
            break;
    }

    if(_pin > 32)
    {
         sjsu::LogError("Invalid GPIO pin: %d", _pin);
        exit(1);
    }
}

void GPIO::Init()
{
    // Enable Pull Down Resistor for SW0 (P1.19) and SW1 (P1.15)
    // CLEAR IOCON, then set to 0x01
    sjsu::lpc40xx::LPC_IOCON->P1_15 &= ~(0x3 << 3);
    sjsu::lpc40xx::LPC_IOCON->P1_15 |= (0x1 << 3);
    // CLEAR IOCON, then set to 0x01
    sjsu::lpc40xx::LPC_IOCON->P1_19 &= ~(0x3 << 3);
    sjsu::lpc40xx::LPC_IOCON->P1_19 |= (0x1 << 3);
}

void GPIO::SetAsInput()
{
    LPC_GPIOx->DIR &= ~(1 << _pin);
}

void GPIO::SetAsOutput()
{
    LPC_GPIOx->DIR |= (1 << _pin);
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
    LPC_GPIOx->PIN |= (1 << _pin); 
}

void GPIO::SetLow()
{
    LPC_GPIOx->PIN &= ~(1 << _pin); 
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
    bool result = ReadBool();
    return static_cast<State>(result);
}

bool GPIO::ReadBool()
{
    return (LPC_GPIOx->PIN >> _pin) & 1;
}

void GPIO::AttachInterruptHandler(sjsu::InterruptHandler isr, GPIO::Edge edge)
{
    GPIO::pin_isr_map[_port][_pin] = isr;
    switch(edge)
    {
        case Edge::kRising:
            *IOxIntEnR |= (1 << _pin);
            *IOxIntEnF &= ~(1 << _pin);
            break;
        case Edge::kFalling:
            *IOxIntEnR &= ~(1 << _pin);
            *IOxIntEnF |= (1 << _pin);
            break;
        case Edge::kBoth:
            *IOxIntEnR |= (1 << _pin);
            *IOxIntEnF |= (1 << _pin);
            break;
        case Edge::kNone:
        default:
            *IOxIntEnR &= ~(1 << _pin);
            *IOxIntEnF &= ~(1 << _pin);
            break;
    }
}

void GPIO::EnableInterrupts()
{
    sjsu::InterruptController::GetPlatformController().Enable({
        .interrupt_request_number = sjsu::lpc40xx::GPIO_IRQn,
        .interrupt_handler        = GpioInterruptHandler,
    });
}

void GPIO::GpioInterruptHandler()
{
    uint32_t IOxIntStatR;
    uint32_t IOxIntStatF;
    volatile uint32_t *IOxIntClr;
    uint8_t port;

     sjsu::LogInfo("Interrupt Triggered!");
    if(sjsu::lpc40xx::LPC_GPIOINT->IntStatus & 1)                  // Check bit 0 for Port 0 Interrupt Status
    {
        IOxIntStatR = sjsu::lpc40xx::LPC_GPIOINT->IO0IntStatR;
        IOxIntStatF = sjsu::lpc40xx::LPC_GPIOINT->IO0IntStatF;
        IOxIntClr = &sjsu::lpc40xx::LPC_GPIOINT->IO0IntClr;
        port = 0;
    }
    else if((sjsu::lpc40xx::LPC_GPIOINT->IntStatus >> 2) & 1)      // Check bit 2 for Port 2 Interrupt Status
    {
        IOxIntStatR = sjsu::lpc40xx::LPC_GPIOINT->IO2IntStatR;
        IOxIntStatF = sjsu::lpc40xx::LPC_GPIOINT->IO2IntStatF;
        IOxIntClr = &sjsu::lpc40xx::LPC_GPIOINT->IO2IntClr;
        port = 1;
    }

    for(uint8_t i = 0; i < 32; i++)
    {
        if((IOxIntStatR >> i) & 1)
        {
            sjsu::LogInfo("Rising Edge.");
            *IOxIntClr |= (1 << i);
            GPIO::pin_isr_map[port][i]();
            break;
        }
        if((IOxIntStatF >> i) & 1)
        {
            sjsu::LogInfo("Falling Edge.");
            *IOxIntClr |= (1 << i);
            GPIO::pin_isr_map[port][i]();
            break;
        }
    }
}
