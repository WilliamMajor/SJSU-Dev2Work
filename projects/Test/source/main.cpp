#include "L1_Peripheral/gpio.hpp"
#include "LabGPIO.hpp"
#include "L0_Platform/lpc17xx/LPC17xx.h"
#include "L1_Peripheral/cortex/interrupt.hpp"
#include "utility/log.hpp"
#include <cstdint>

using namespace sjsu;

void testRun(LabGPIO testObject, uint8_t testedPin);

void GpioInterruptCallback();

int main(void)
{
  uint8_t testPin = 0;
  uint8_t testPort = 1;

  LabGPIO gpioTest0(testPort, testPin);
  testPin = 1;
  LabGPIO gpioTest1(testPort, testPin);
  testPin = 4;
  LabGPIO gpioTest2(testPort, testPin);
  testPin = 8;
  LabGPIO gpioTest3(testPort, testPin);

  sjsu::cortex::InterruptController IntCont;
  InterruptController::RegistrationInfo_t test = {
    sjsu::lpc17xx::EINT3_IRQn, 
    GpioInterruptCallback,
    true,
    1,
  };
  //Register Interrupt
  IntCont.Register(test);

  while(true)
  {
    testRun(gpioTest0, testPin);
    testRun(gpioTest1, testPin);
    testRun(gpioTest2, testPin);
    testRun(gpioTest3, testPin);
  }
  return 0;
}

void GpioInterruptCallback()
{
  printf("hello world\n");
}

void testRun(LabGPIO testObject, uint8_t testedPin)
{

  testObject.SetDirection(sjsu::Gpio::kOutput);
  bool pinState;
  
  testObject.Set(sjsu::Gpio::kLow);
  if(testObject.Read())
  {
    printf("Pin not set low\n");
    printf("pin state: 0x%08x\n", lpc17xx::LPC_GPIO1->FIOPIN & (1 << testedPin));
    //sjsu::Delay(1ms);
  }

  sjsu::Delay(100ms);
  testObject.Set(sjsu::Gpio::kHigh);
  if(!testObject.Read())
  {
    printf("Pin not set high\n");
    printf("pin state: 0x%08x\n", lpc17xx::LPC_GPIO1->FIOPIN & (1 << testedPin));
    sjsu::Delay(5s);
  }
  //sjsu::Delay(1ms);

  pinState = testObject.Read();
  testObject.Toggle();
  if(testObject.Read() == pinState)
    printf("Pin was not toggled");
  pinState = testObject.Read();

  //sjsu::Delay(1ms);
  
  testObject.Toggle();
  if(testObject.Read() == pinState)
    printf("Pin was not toggled");
  //sjsu::Delay(1ms);
}