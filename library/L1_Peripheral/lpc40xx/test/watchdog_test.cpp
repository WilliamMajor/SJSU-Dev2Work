#include <cstdint>

#include "L0_Platform/lpc40xx/LPC40xx.h"
#include "L1_Peripheral/lpc40xx/watchdog.hpp"
#include "L4_Testing/testing_frameworks.hpp"
#include "third_party/units/units.h"

namespace sjsu
{
namespace lpc40xx
{
EMIT_ALL_METHODS(Watchdog);

TEST_CASE("Testing lpc40xx watchdog", "[lpc40xx-watchdog]")
{
  constexpr std::chrono::seconds kTimerConstantSeconds = 10s;
  constexpr uint32_t kTimerConstant                    = 1250000;
  constexpr uint8_t kTimerMode                         = 0x3;
  constexpr uint32_t kTimerWarning                     = 1023;
  constexpr uint8_t kFeedSequence                      = 0x55;
  constexpr uint32_t kNewTimerValue                    = 150;

  // Create local version of LPC_WDT
  LPC_WDT_TypeDef local_wdt;

  // Clear local wdt registers
  memset(&local_wdt, 0, sizeof(local_wdt));

  // Set base registers to local registers
  Watchdog::wdt_base = &local_wdt;

  Watchdog watchdog_test;

  SECTION("Initialization")
  {
    watchdog_test.Initialize(kTimerConstantSeconds);
    CHECK(kTimerConstant == local_wdt.TC);
    CHECK(kTimerMode == local_wdt.MOD);
    CHECK(kTimerWarning == local_wdt.WARNINT);
  }

  SECTION("Send Feed Sequence")
  {
    watchdog_test.FeedSequence();
    CHECK(kFeedSequence == local_wdt.FEED);
  }

  SECTION("Read watchdog timer value")
  {
    local_wdt.TV = kNewTimerValue;
    CHECK(kNewTimerValue == watchdog_test.CheckTimeout());
  }
}
}  // namespace lpc40xx
}  // namespace sjsu
