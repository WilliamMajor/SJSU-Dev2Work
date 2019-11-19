// Test for Pin class.
// Using a test by side effect on the Cortex M4 SysTick register
#include "config.hpp"
#include "L1_Peripheral/cortex/dwt_counter.hpp"
#include "L4_Testing/testing_frameworks.hpp"

namespace sjsu::cortex
{
EMIT_ALL_METHODS(DwtCounter);

TEST_CASE("Testing ARM Cortex Data Watchdog Trace Counter",
          "[cortex-dwt-counter]")
{
  DWT_Type local_dwt = {
    .PCSR = 0,
  };
  CoreDebug_Type local_core;

  memset(&local_dwt, 0, sizeof(local_dwt));
  memset(&local_core, 0, sizeof(local_core));

  DwtCounter::dwt  = &local_dwt;
  DwtCounter::core = &local_core;

  DwtCounter test_subject;

  SECTION("Initialize Debug Counter")
  {
    test_subject.Initialize();

    CHECK(CoreDebug_DEMCR_TRCENA_Msk == local_core.DEMCR);
    CHECK(0 == local_dwt.CYCCNT);
    CHECK(DWT_CTRL_CYCCNTENA_Msk == local_dwt.CTRL);
  }
  SECTION("Get Count")
  {
    local_dwt.CYCCNT = 0;
    CHECK(0 == test_subject.GetCount());
    local_dwt.CYCCNT = 17;
    CHECK(17 == test_subject.GetCount());
    local_dwt.CYCCNT = 1024;
    CHECK(1024 == test_subject.GetCount());
  }

  DwtCounter::dwt  = DWT;
  DwtCounter::core = CoreDebug;
}
}  // namespace sjsu::cortex
