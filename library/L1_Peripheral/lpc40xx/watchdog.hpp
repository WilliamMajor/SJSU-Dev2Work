#pragma once

#include <cstdint>
#include <iterator>

#include "L0_Platform/lpc40xx/LPC40xx.h"
#include "L1_Peripheral/cortex/interrupt.hpp"
#include "L1_Peripheral/lpc40xx/gpio.hpp"
#include "L1_Peripheral/watchdog.hpp"
#include "third_party/units/units.h"
#include "utility/time.hpp"
#include "utility/bit.hpp"

namespace sjsu
{
namespace lpc40xx
{
class Watchdog final : public sjsu::Watchdog
{
 public:
  const uint8_t kWatchdogResetAndEnable = 0x3;
  const uint32_t kTimerWarningMax       = 0b11'1111'1111;
  static constexpr units::frequency::hertz_t kWatchdogClockFrequency = 500_kHz;
  static constexpr units::frequency::hertz_t kWatchdogClockDivider   = 4_Hz;
  inline static LPC_WDT_TypeDef * wdt_base                           = LPC_WDT;

  // Calls the default watchdog handler
  static void WatchdogIrqHandler()
  {
    DefaultWatchdogHandler();
  }

  // Default watchdog handler that halts the program is fired off
  static void DefaultWatchdogHandler()
  {
    SJ2_ASSERT_FATAL(false, "Watch Dog timer overflowed");
  }

  // This function converts duration back into watchdog ticks to write to the TC
  // register
  static uint32_t ConvertDurationToWatchdogTicks(uint32_t watchdog_ticks,
                                                 std::chrono::seconds duration)
  {
    uint32_t watchdog_clock_scalar =
        (kWatchdogClockFrequency / kWatchdogClockDivider).to<uint32_t>();
    watchdog_ticks =
        (watchdog_clock_scalar * static_cast<uint32_t>(duration.count()));
    return watchdog_ticks;
  }

  inline static const InterruptController::RegistrationInfo_t
      kWatchdogInterruptInfo = {
        .interrupt_request_number  = WDT_IRQn,
        .interrupt_service_routine = &WatchdogIrqHandler,
        .enable_interrupt          = true,
      };

  inline static const cortex::InterruptController kCortexInterruptController =
      cortex::InterruptController();

  // Initializes the watchdog timer
  Status Initialize(std::chrono::seconds duration) const override
  {
    uint32_t timer_constant = 0x0000'0000;
    timer_constant = ConvertDurationToWatchdogTicks(timer_constant, duration);

    // Insert timer_constant value into TC register
    wdt_base->TC = bit::Extract(timer_constant, { .position = 0, .width = 24 });

    // Enables the watchdog and enables the watchdog reset
    wdt_base->MOD = kWatchdogResetAndEnable;

    // Insert timer_warning value into WARNINT register
    wdt_base->WARNINT = kTimerWarningMax;

    return Status::kSuccess;
  }

  void RegisterWatchdogHandler() const override
  {
    // Register WDT_IRQ defined by the structure
    kCortexInterruptController.Register(kWatchdogInterruptInfo);
  }

  // Feeds the watchdog timer
  void FeedSequence() const override
  {
    // Insert valid feed sequence
    wdt_base->FEED = 0xAA;
    wdt_base->FEED = 0x55;
  }
  // Reads the current counter value of the watchdog timer
  uint32_t CheckTimeout() const override
  {
    return wdt_base->TV;
  }
};
}  // namespace lpc40xx
}  // namespace sjsu
