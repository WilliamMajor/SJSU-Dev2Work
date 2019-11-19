// Test for Pin class.
// Using a test by side effect on the Cortex M4 SysTick register
#include "config.hpp"
#include "L0_Platform/lpc40xx/LPC40xx.h"
#include "L1_Peripheral/cortex/system_timer.hpp"
#include "L4_Testing/testing_frameworks.hpp"

namespace sjsu::cortex
{
EMIT_ALL_METHODS(SystemTimer);

static void DummyFunction(void) {}

TEST_CASE("Testing ARM Cortex SystemTimer", "[cortex-system-timer]")
{
  // Simulated local version of SysTick register to verify register
  // manipulation by side effect of Pin method calls
  // Default figure 552 page 703
  SysTick_Type local_systick = { 0x4, 0, 0, 0x000F'423F };
  // Substitute the memory mapped SysTick with the local_systick test struture
  // Redirects manipulation to the 'local_systick'
  SystemTimer::sys_tick = &local_systick;

  // Set mock for sjsu::SystemController
  constexpr units::frequency::hertz_t kDummySystemControllerClockFrequency =
      12_MHz;

  Mock<SystemController> mock_system_controller;
  When(Method(mock_system_controller, GetSystemFrequency))
      .AlwaysReturn(kDummySystemControllerClockFrequency);

  Mock<sjsu::InterruptController> mock_interrupt_controller;
  Fake(Method(mock_interrupt_controller, Register));
  Fake(Method(mock_interrupt_controller, Deregister));

  SystemTimer test_subject(mock_system_controller.get(),
                           mock_interrupt_controller.get());

  SECTION("SetTickFrequency generate desired frequency")
  {
    constexpr auto kDivisibleFrequency = 1_kHz;
    local_systick.LOAD                 = 0;

    CHECK(0 == test_subject.SetTickFrequency(kDivisibleFrequency));
    constexpr uint32_t kExpectedLoadValue =
        (kDummySystemControllerClockFrequency / kDivisibleFrequency) - 1;
    CHECK(kExpectedLoadValue == local_systick.LOAD);
  }
  SECTION("SetTickFrequency should return remainder of ticks mismatch")
  {
    constexpr auto kOddFrequency = 7_Hz;
    local_systick.LOAD           = 0;

    CHECK(kDummySystemControllerClockFrequency.to<uint32_t>() %
              kOddFrequency.to<uint32_t>() ==
          test_subject.SetTickFrequency(kOddFrequency));
    CHECK(((kDummySystemControllerClockFrequency / kOddFrequency) - 1)
              .to<uint32_t>() == local_systick.LOAD);
  }
  SECTION("Start Timer should set necessary SysTick Ctrl bits and set VAL to 0")
  {
    // Source: "UM10562 LPC408x/407x User manual" table 553 page 703
    constexpr uint8_t kEnableMask    = 0b0001;
    constexpr uint8_t kTickIntMask   = 0b0010;
    constexpr uint8_t kClkSourceMask = 0b0100;
    constexpr uint32_t kMask = kEnableMask | kTickIntMask | kClkSourceMask;
    local_systick.VAL        = 0xBEEF;
    local_systick.LOAD       = 1000;

    CHECK(Status::kSuccess == test_subject.StartTimer());
    CHECK(kMask == local_systick.CTRL);
    CHECK(0 == local_systick.VAL);
  }
  SECTION(
      "StartTimer should return false and set no bits if LOAD is set to zero")
  {
    // Source: "UM10562 LPC408x/407x User manual" table 553 page 703
    constexpr uint8_t kClkSourceMask = 0b0100;
    // Default value see above.
    local_systick.CTRL = kClkSourceMask;
    // Setting load value to 0, should return false
    local_systick.LOAD = 0;
    local_systick.VAL  = 0xBEEF;

    CHECK(Status::kInvalidSettings == test_subject.StartTimer());
    CHECK(kClkSourceMask == local_systick.CTRL);
    CHECK(0xBEEF == local_systick.VAL);
    // TODO(undef): add check for interrupt_controller.Register()
  }
  SECTION("DisableTimer should clear all bits")
  {
    // Source: "UM10562 LPC408x/407x User manual" table 553 page 703
    constexpr uint8_t kEnableMask    = 0b0001;
    constexpr uint8_t kTickIntMask   = 0b0010;
    constexpr uint8_t kClkSourceMask = 0b0100;
    constexpr uint32_t kMask = kEnableMask | kTickIntMask | kClkSourceMask;
    local_systick.CTRL       = kMask;
    local_systick.LOAD       = 1000;

    test_subject.DisableTimer();

    CHECK(0 == local_systick.CTRL);
  }
  SECTION("SetInterrupt set SystemTimer::system_timer_isr to DummyFunction")
  {
    SystemTimer::system_timer_isr = nullptr;

    test_subject.SetInterrupt(DummyFunction);

    CHECK(DummyFunction == SystemTimer::system_timer_isr);
  }

  SystemTimer::sys_tick = SysTick;
}
}  // namespace sjsu::cortex
