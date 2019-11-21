#include "LabGPIO.hpp"
//#include "L0_Platform/lpc40xx/LPC40xx.h"
#include "utility/log.hpp"
#include "L0_Platform/lpc17xx/LPC17xx.h"

#include <cstdint>
using namespace sjsu;

LabGPIO::LabGPIO(uint8_t port, uint8_t pin)
{
    PIN = pin;
    PORT = port;
}

void LabGPIO::SetDirection(Direction direction) const
{
    switch(PORT)
    {
        case 0:
        {
            if(direction == sjsu::Gpio::kInput)
                lpc17xx::LPC_GPIO0->FIODIR &= ~(1 << PIN);
            else
                lpc17xx::LPC_GPIO0->FIODIR |= (1 << PIN);
            break;
        }
        case 1:
        {
            if(direction == sjsu::Gpio::kInput)
                lpc17xx::LPC_GPIO1->FIODIR &= ~(1 << PIN);
            else
                lpc17xx::LPC_GPIO1->FIODIR |= (1 << PIN);
                
            break;
        }
            
        case 2:
        {
            if(direction == sjsu::Gpio::kInput)
                lpc17xx::LPC_GPIO2->FIODIR &= ~(1 << PIN);
            else
                lpc17xx::LPC_GPIO2->FIODIR |= (1 << PIN);
            break;
        }
        default:
        {
            // printf("Invalide port passed...\n");
            // fflush(stdout);
            break;
        }


    }
}

void LabGPIO::Set(State output) const
{
    switch(PORT)
    {
        case 0:
        {
            if(output == sjsu::Gpio::kLow)
                lpc17xx::LPC_GPIO0->FIOPIN &= ~(1 << PIN);
                
            else
                lpc17xx::LPC_GPIO0->FIOPIN |= (1 << PIN);
            break;
        }
        case 1:
        {
            if(output == sjsu::Gpio::kLow)
                lpc17xx::LPC_GPIO1->FIOPIN &= ~(1 << PIN);
            else
                lpc17xx::LPC_GPIO1->FIOPIN |= (1 << PIN);;
            break;
        }   
        case 2:
        {
            if(output == sjsu::Gpio::kLow)
                lpc17xx::LPC_GPIO2->FIOPIN &= ~(1 << PIN);
            else
                lpc17xx::LPC_GPIO2->FIOPIN |= (1 << PIN);
            break;
        }
        default:
        {
            // printf("Invalide port passed...\n");
            // fflush(stdout);
            break;
        }
    }
}

void LabGPIO::Toggle() const
{
    switch(PORT)
    {
        case 0:
        {
            lpc17xx::LPC_GPIO0->FIOPIN ^= (1 << PIN);
            break;
        }
        case 1:
        {
            lpc17xx::LPC_GPIO1->FIOPIN ^= (1 << PIN);
            break;
        }   
        case 2:
        {
            lpc17xx::LPC_GPIO2->FIOPIN ^= (1 << PIN);
            break;
        }
        default:
        {
            // printf("Invalide port passed...\n");
            // fflush(stdout);
            break;
        }
    }
}

bool LabGPIO::Read() const
{
    switch(PORT)
    {
        case 0:
        {
            return(static_cast<bool>(lpc17xx::LPC_GPIO0->FIOPIN & (1 << PIN)));
            break;
        }
        case 1:
        {
            return(static_cast<bool>(lpc17xx::LPC_GPIO1->FIOPIN & (1 << PIN)));
            break;
        }   
        case 2:
        {
            return(static_cast<bool>(lpc17xx::LPC_GPIO2->FIOPIN & (1 << PIN)));
            break;
        }
    }
    return false;
}

const sjsu::Pin & LabGPIO::GetPin() const
{
    //Not implemented yet
}

void LabGPIO::AttachInterrupt(IsrPointer function, Edge edge) const
{
    IsrPointer test = function;
    Edge testedge = edge;
    test = test;
    testedge = testedge;
}

void LabGPIO::DetachInterrupt() const
{   
    //Not implemented yet
}
