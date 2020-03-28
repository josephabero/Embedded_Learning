#pragma once 

#include <cstdint>
#include "../../library/L0_Platform/lpc40xx/LPC40xx.h"
  
class GPIO
{
    public:
        enum class Direction : uint8_t
        {
          kInput  = 0,
          kOutput = 1
        };
        enum class State : uint8_t
        {
          kLow  = 0,
          kHigh = 1
        };
        /// @param port - port number between 0 and 5
        /// @param pin - pin number between 0 and 32
        GPIO(uint8_t port, uint32_t pin);
        /// Sets this GPIO as an input
        void SetAsInput();
        /// Sets this GPIO as an output
        void SetAsOutput();
        /// Sets this GPIO as an input
        /// @param direction - true => output, false => set pin to input
        void SetDirection(Direction direction);
        /// Set voltage of pin to HIGH
        void SetHigh();
        /// Set voltage of pin to LOW 
        void SetLow();
        /// @param state - State::kHigh => set pin high, State::kLow => set pin low
        void set(State state);
        /// @return level of pin high => true, low => false
        State Read();
        /// @return level of pin high => true, low => false
        bool ReadBool();
    private:
        sjsu::lpc40xx::LPC_GPIO_TypeDef *gpio_port;
        uint8_t _port;
        uint32_t _pin;
};
