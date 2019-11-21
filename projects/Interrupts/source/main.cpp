#include "L1_Peripheral/gpio.hpp"
#include "LabGPIO.hpp"
#include "L0_Platform/lpc17xx/LPC17xx.h"
#include "L1_Peripheral/cortex/interrupt.hpp"
#include "utility/log.hpp"
#include <cstdint>

using namespace sjsu;


void GpioInterruptCallback();

int main(void)
{

  uint8_t testPin = 0;
  uint8_t testPort = 0;
  //Setup Gpio on LED Port 0. Pin 0
  LabGPIO gpioTest0(testPort, testPin);
  LabGPIO gpioTest1(1, 0);
  LabGPIO gpioTest2(2,0);
  LabGPIO gpioTest3(1,1);
  
  //Set Direction of GPIO to be output
  gpioTest0.SetDirection(sjsu::Gpio::kOutput);
  gpioTest1.SetDirection(sjsu::Gpio::kOutput);
  gpioTest2.SetDirection(sjsu::Gpio::kOutput);
  gpioTest3.SetDirection(sjsu::Gpio::kOutput);
  //Set Pin 0 to a low state
  gpioTest0.Set(sjsu::Gpio::kLow);
  gpioTest1.Set(sjsu::Gpio::kLow);
  gpioTest2.Set(sjsu::Gpio::kLow);
  gpioTest3.Set(sjsu::Gpio::kLow);
  sjsu::Delay(100ms);
  
  //create Interrupt controller
  sjsu::cortex::InterruptController IntCont;

  //initialize struct for registering interrupt
  InterruptController::RegistrationInfo_t gpioInterrupt = {
    sjsu::lpc17xx::EINT3_IRQn, 
    GpioInterruptCallback,
    true,
   -1,
  };
  //enable interrupt on and falling edge...
  lpc17xx::LPC_GPIOINT->IO0IntEnR |= 0x1;
  lpc17xx::LPC_GPIOINT->IO0IntEnF |= 0x1;
  lpc17xx::LPC_GPIOINT->IO2IntEnR |= 0x1;
  lpc17xx::LPC_GPIOINT->IO2IntEnF |= 0x1;

  //Register Interrupt
  IntCont.Register(gpioInterrupt);
  


  while(true)
  {
    printf("toggling 1\n");
    gpioTest0.Toggle();
    sjsu::Delay(100ms);
    printf("toggling 2\n");
    gpioTest2.Toggle();
    sjsu::Delay(100ms);

    
  }
  return 0;
}
void GpioInterruptCallback()
{
  // printf("Test... Rising: 0x%08x, Falling: 0x%08x\n", lpc17xx::LPC_GPIOINT->IO0IntStatR, lpc17xx::LPC_GPIOINT->IO0IntStatF);
  // printf("Test... Rising: 0x%08x, Falling: 0x%08x\n", lpc17xx::LPC_GPIOINT->IO2IntStatR, lpc17xx::LPC_GPIOINT->IO2IntStatF);
  if(lpc17xx::LPC_GPIOINT->IO0IntStatR || lpc17xx::LPC_GPIOINT->IO0IntStatF)
  {
    lpc17xx::LPC_GPIO1->FIOPIN ^= (1 << 0);
    lpc17xx::LPC_GPIOINT->IO0IntClr |= 1;
  }
  if(lpc17xx::LPC_GPIOINT->IO2IntStatF || lpc17xx::LPC_GPIOINT->IO2IntStatR)
  {
    lpc17xx::LPC_GPIO1->FIOPIN ^= (1 << 1);
    lpc17xx::LPC_GPIOINT->IO2IntClr |= 1;
  }
  
  
}
