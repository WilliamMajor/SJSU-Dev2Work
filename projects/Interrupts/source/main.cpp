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
  //Setup Gpio on LED Port 0. Pin 0
  LabGPIO gpioTest0(0, 0);
  
  //Set Direction of GPIO to be output
  gpioTest0.SetDirection(sjsu::Gpio::kOutput);
  //Set Pin 0 to a low state

  printf("got here0\n");
  gpioTest0.Set(sjsu::Gpio::kLow);
  printf("got here1\n");\
  sjsu::Delay(100ms);
  
  //create Interrupt controller
  sjsu::cortex::InterruptController IntCont;

  //initialize struct for registering interrupt
  InterruptController::RegistrationInfo_t gpioInterrupt = {
    sjsu::lpc17xx::EINT3_IRQn, 
    GpioInterruptCallback,
    true,
    1,
  };
  //enable interrupt on and falling edge...
  lpc17xx::LPC_GPIOINT->IO0IntEnR |= 0x1;
  //lpc17xx::LPC_GPIOINT->IO0IntEnF |= 0x1;

  printf("got here2 \n");
  //Register Interrupt
  IntCont.Register(gpioInterrupt);
  printf("got here3 \n");
  


  while(true)
  {

    printf("test: %d\n",lpc17xx::LPC_GPIOINT->IO0IntEnR);
    fflush(stdout);
    sjsu::Delay(1s);

    //Toggle LED this should cause the interrupt to fire off...
    gpioTest0.Toggle();

    ///printf("test2: %d\n", lpc17xx::LPC_GPIOINT->IntStatus);
    sjsu::Delay(100ms);

    
  }
  return 0;
}
void GpioInterruptCallback()
{
  printf("test2: %d\n", lpc17xx::LPC_GPIOINT->IntStatus);
  
  lpc17xx::LPC_GPIOINT->IO0IntClr &= ~(0x1);
  printf("test3: %d\n", lpc17xx::LPC_GPIOINT->IntStatus);
  for (int i = 0; i < 1000000; i++)
  {

  }
}
