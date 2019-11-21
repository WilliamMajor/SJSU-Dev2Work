
#include "L1_Peripheral/gpio.hpp"

#include <cstdint>

namespace sjsu
{


    ///Implementations for the virtual functions defined in Gpio.hpp
    class LabGPIO : public Gpio
    {
        public:

        LabGPIO(uint8_t port, uint8_t pin);
        void SetDirection(Direction direction) const override;
        void Set(State output) const override;
        void Toggle() const override;
        bool Read() const override;
        const sjsu::Pin & GetPin() const override;
        void AttachInterrupt(IsrPointer function, Edge edge) const override;
        void DetachInterrupt() const override;

        private:
            volatile uint8_t PIN;
            volatile uint8_t PORT;
    };
}