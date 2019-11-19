#include "L1_Peripheral/lpc17xx/adc.hpp"
#include "L1_Peripheral/lpc17xx/system_controller.hpp"
#include "L2_HAL/sensors/environment/light/temt6000x01.hpp"
#include "utility/log.hpp"

int main()
{
  LOG_INFO("Starting LPC176x/5x Light Sensor Example...");

  sjsu::lpc17xx::SystemController system_controller;
  system_controller.SetPeripheralClockDivider(
      sjsu::lpc17xx::SystemController::Peripherals::kAdc, 8);
  // The LPC176x/5x ADC has a reference voltage of 3.3V.
  constexpr units::voltage::volt_t kAdcReferenceVoltage = 3.3_V;
  // A 10kOhm pull-down resistor is used on the SJOne board.
  constexpr units::impedance::ohm_t kPullDownResistance = 10'000_Ohm;
  // The SJOne board uses ADC Channel 0.2, P0.25 for the light sensor
  sjsu::lpc17xx::Adc adc2(sjsu::lpc17xx::AdcChannel::kChannel2,
                          system_controller);
  sjsu::Temt6000x01 light_sensor(
      adc2, kAdcReferenceVoltage, kPullDownResistance);

  SJ2_ASSERT_FATAL(light_sensor.Initialize(),
                   "Failed to initialized light sensor!");
  adc2.BurstMode(true);

  while (1)
  {
    const units::illuminance::lux_t kLux = light_sensor.GetIlluminance();
    LOG_INFO("%.4f", kLux.to<double>());
    sjsu::Delay(1s);
  }

  return 0;
}
