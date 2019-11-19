#include <FreeRTOS.h>
#include <task.h>

#include <cstdint>
#include <cstdio>

#include "config.hpp"
#include "L1_Peripheral/lpc40xx/gpio.hpp"
#include "L2_HAL/displays/led/onboard_led.hpp"
#include "L2_HAL/switches/button.hpp"
#include "utility/log.hpp"
#include "utility/macros.hpp"
#include "utility/rtos.hpp"
#include "utility/time.hpp"

// Using anonymous namespace so these tasks are only visible to this file
namespace
{
sjsu::OnBoardLed leds;

void LedToggle(void * parameters)
{
  LOG_INFO("Setting up task...");
  LOG_INFO("Retrieving delay amount from parameters...");
  auto delay = sjsu::rtos::RetrieveParameter(parameters);
  LOG_INFO("Initializing LEDs...");
  leds.Off(0);
  leds.On(1);
  LOG_INFO("LEDs Initialized...");
  // Loop blinks the LEDs back and forth at a rate that depends on the
  // pvParameter's value.
  LOG_INFO("Toggling LEDs...");
  while (true)
  {
    leds.Toggle(0);
    leds.Toggle(1);
    vTaskDelay(delay);
  }
}

void ButtonReader([[maybe_unused]] void * parameters)
{
  LOG_INFO("Setting up task...");
  LOG_INFO("Initializing SW3...");

  sjsu::lpc40xx::Gpio button_gpio3(1, 19);
  sjsu::Button switch3(button_gpio3);

  switch3.Initialize();
  leds.Off(3);

  LOG_INFO("SW3 Initialized...");
  LOG_INFO("Press and release SW3 to toggle LED3 state...");
  // Loop detects when the button has been released and changes the LED state
  // accordingly.
  while (true)
  {
    if (switch3.Released())
    {
      leds.Toggle(3);
    }
    vTaskDelay(50);
  }
}

}  // namespace

int main()
{
  TaskHandle_t handle = NULL;
  LOG_INFO("Starting FreeRTOS Blinker Example...");
  LOG_INFO("Initializing LEDs...");
  leds.Initialize();
  LOG_INFO("Creating Tasks...");
  sjsu::Delay(1s);
  // See https://www.freertos.org/a00125.html for the xTaskCreate API
  // See utility/rtos.hpp for the sjsu::rtos:: namespace utility functions
  xTaskCreate(LedToggle,        // Make function LedToggle a task
              "LedToggleTask",  // Give this task the name "LedToggleTask"
              sjsu::rtos::StackSize(1024),  // Size of stack allocated to task
              sjsu::rtos::PassParameter(100),  // Parameter to be passed to task
              sjsu::rtos::Priority::kLow,      // Give this task low priority
              &handle);                        // Optional feference to the task
  xTaskCreate(ButtonReader,        // Make function ButtonReader a task
              "ButtonReaderTask",  // Give this task the name "ButtonReaderTask"
              sjsu::rtos::StackSize(1024),    // Size of stack allocated to task
              sjsu::rtos::kNoParameter,       // Pass nothing to this task
              sjsu::rtos::Priority::kMedium,  // Give this task medium priority
              sjsu::rtos::kNoHandle);         // Do not supply a task handle
  LOG_INFO("Starting Scheduler...");
  vTaskStartScheduler();
  return 0;
}
