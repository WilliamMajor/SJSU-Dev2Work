#include <cstdint>

#include "L1_Peripheral/lpc40xx/pulse_capture.hpp"
#include "L4_Testing/testing_frameworks.hpp"
#include "utility/enum.hpp"
#include "utility/status.hpp"
#include "utility/units.hpp"

namespace sjsu::lpc40xx
{
EMIT_ALL_METHODS(PulseCapture);

PulseCapture::CaptureStatus_t isr_result;
PulseCapture::CaptureIsr test_timer_isr = nullptr;

void DummyCallback(PulseCapture::CaptureStatus_t status)
{
  memcpy(&isr_result, &status, sizeof(PulseCapture::CaptureStatus_t));
}

TEST_CASE("Testing lpc40xx Pulse Capture", "[lpc40xx-pulse_capture]")
{
  LPC_TIM_TypeDef test_timer_register;

  constexpr PulseCapture::CaptureEdgeMode kModeList[] = {
    PulseCapture::CaptureEdgeMode::kNone,
    PulseCapture::CaptureEdgeMode::kRising,
    PulseCapture::CaptureEdgeMode::kFalling,
    PulseCapture::CaptureEdgeMode::kBoth
  };

  static Mock<sjsu::Pin> mock_pin0;
  static Mock<sjsu::Pin> mock_pin1;

  Fake(Method(mock_pin0, SetPinFunction), Method(mock_pin1, SetPinFunction));

  sjsu::Pin & capture0_input_pin = mock_pin0.get();
  sjsu::Pin & capture1_input_pin = mock_pin1.get();

  PulseCapture::CaptureChannelNumber test_timer_channel_number0 =
      PulseCapture::kChannel0;
  PulseCapture::CaptureChannelNumber test_timer_channel_number1 =
      PulseCapture::kChannel1;

  static const PulseCapture::CaptureChannelPartial_t kTestTimerPartial0 = {
    .timer_register = &test_timer_register,
    .power_id       = SystemController::Peripherals::kTimer0,
    .irq            = IRQn::TIMER0_IRQn,
    .user_callback  = &test_timer_isr,
    .capture_pin0   = capture0_input_pin,
    .capture_pin1   = capture1_input_pin,
    .channel_number = &test_timer_channel_number0
  };

  static const PulseCapture::CaptureChannelPartial_t kTestTimerPartial1 = {
    .timer_register = &test_timer_register,
    .power_id       = SystemController::Peripherals::kTimer0,
    .irq            = IRQn::TIMER0_IRQn,
    .user_callback  = &test_timer_isr,
    .capture_pin0   = capture0_input_pin,
    .capture_pin1   = capture1_input_pin,
    .channel_number = &test_timer_channel_number1
  };

  const PulseCapture::CaptureChannel_t kTestTimerCh0 = {
    .channel = kTestTimerPartial0,
    .isr     = PulseCapture::TimerHandler<kTestTimerPartial0>
  };
  const PulseCapture::CaptureChannel_t kTestTimerCh1 = {
    .channel = kTestTimerPartial1,
    .isr     = PulseCapture::TimerHandler<kTestTimerPartial1>
  };

  memset(&test_timer_register, 0, sizeof(test_timer_register));

  Mock<sjsu::SystemController> mock_system_controller;
  Fake(Method(mock_system_controller, PowerUpPeripheral));

  constexpr units::frequency::hertz_t kTestSystemFrequency = 4_MHz;
  constexpr int kTestPeripheralClockDivider                = 1;

  When(Method(mock_system_controller, GetSystemFrequency))
      .AlwaysReturn(kTestSystemFrequency);
  When(Method(mock_system_controller, GetPeripheralClockDivider))
      .AlwaysReturn(kTestPeripheralClockDivider);

  Mock<sjsu::InterruptController> mock_interrupt_controller;
  Fake(Method(mock_interrupt_controller, Register));

  constexpr units::frequency::hertz_t kTestFrequency = 4_MHz;

  PulseCapture test_subject0(kTestTimerCh0,
                             PulseCapture::CaptureChannelNumber::kChannel0,
                             kTestFrequency,
                             mock_system_controller.get(),
                             mock_interrupt_controller.get());

  PulseCapture test_subject1(kTestTimerCh1,
                             PulseCapture::CaptureChannelNumber::kChannel1,
                             kTestFrequency,
                             mock_system_controller.get(),
                             mock_interrupt_controller.get());

  PulseCapture * test_subjects[2] = { &test_subject0, &test_subject1 };

  SECTION("Capture Initialize Channel 0")
  {
    test_subjects[0]->Initialize(&DummyCallback);
    int prescaler =
        (kTestSystemFrequency.to<int32_t>() / kTestPeripheralClockDivider) /
        kTestFrequency.to<int32_t>();

    Verify(
        Method(mock_interrupt_controller, Register)
            .Matching([kTestTimerCh0](
                          sjsu::InterruptController::RegistrationInfo_t info) {
              return (info.interrupt_request_number ==
                      kTestTimerCh0.channel.irq) &&
                     (info.interrupt_service_routine == kTestTimerCh0.isr) &&
                     (info.enable_interrupt == true) && (info.priority == -1);
            }));

    CHECK(test_timer_register.PR == prescaler);
    CHECK(bit::Read(test_timer_register.TCR, 0) == true);
  }  // end section initialize

  SECTION("Capture Initialize Channel 1")
  {
    test_subjects[1]->Initialize(&DummyCallback);
    int prescaler =
        (kTestSystemFrequency.to<int32_t>() / kTestPeripheralClockDivider) /
        kTestFrequency.to<int32_t>();

    Verify(
        Method(mock_interrupt_controller, Register)
            .Matching([kTestTimerCh1](
                          sjsu::InterruptController::RegistrationInfo_t info) {
              return (info.interrupt_request_number ==
                      kTestTimerCh1.channel.irq) &&
                     (info.interrupt_service_routine == kTestTimerCh1.isr) &&
                     (info.enable_interrupt == true) && (info.priority == -1);
            }));

    CHECK(test_timer_register.PR == prescaler);
    CHECK(bit::Read(test_timer_register.TCR, 0) == true);
  }  // end section initialize

  SECTION("Capture Config Channel 0")
  {
    int position = 0;
    for (size_t mode = 0; mode < std::size(kModeList); mode++)
    {
      test_subjects[0]->ConfigureCapture(kModeList[mode]);
      CHECK(bit::Extract(test_timer_register.CCR, position, 2) ==
            static_cast<uint32_t>(kModeList[mode]));
    }
  }  // end section capture config

  SECTION("Capture Config Channel 0")
  {
    int position = 3;
    for (size_t mode = 0; mode < std::size(kModeList); mode++)
    {
      test_subjects[1]->ConfigureCapture(kModeList[mode]);
      CHECK(bit::Extract(test_timer_register.CCR, position, 2) ==
            static_cast<uint32_t>(kModeList[mode]));
    }
  }  // end section capture config

  SECTION("Capture Interrupt Channel 0")
  {
    int position = 2;
    test_subjects[0]->EnableCaptureInterrupt(true);
    CHECK(bit::Read(test_timer_register.CCR, position) == true);
    test_subjects[0]->EnableCaptureInterrupt(false);
    CHECK(bit::Read(test_timer_register.CCR, position) == false);
  }  // end section capture interrupt

  SECTION("Capture Interrupt Channel 1")
  {
    int position = 5;
    test_subjects[1]->EnableCaptureInterrupt(true);
    CHECK(bit::Read(test_timer_register.CCR, position) == true);
    test_subjects[1]->EnableCaptureInterrupt(false);
    CHECK(bit::Read(test_timer_register.CCR, position) == false);
  }  // end section capture interrupt

  SECTION("Capture Interrupt Handler Acknowledge (Common)")
  {
    memset(&isr_result, 0, sizeof(PulseCapture::CaptureStatus_t));
    kTestTimerCh0.isr();
    CHECK(bit::Extract(test_timer_register.IR, 4, 2) == 0b11);
  }

  SECTION("Capture Interrupt Handler Channel 0")
  {
    constexpr uint32_t kCr0TestPattern = 0x22226666;
    memset(&isr_result, 0, sizeof(PulseCapture::CaptureStatus_t));
    test_timer_register.CR0 = kCr0TestPattern;
    kTestTimerCh0.isr();
    CHECK(isr_result.count == kCr0TestPattern);
  }  // end section capture interrupt handler

  SECTION("Capture Interrupt Handler Channel 0")
  {
    constexpr uint32_t kCr1TestPattern = 0x33337777;
    memset(&isr_result, 0, sizeof(PulseCapture::CaptureStatus_t));
    test_timer_register.CR1 = kCr1TestPattern;
    kTestTimerCh1.isr();
    CHECK(isr_result.count == kCr1TestPattern);
  }  // end section capture interrupt handler
}  // end test case
}  // namespace sjsu::lpc40xx
