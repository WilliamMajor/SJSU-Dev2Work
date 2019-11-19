//*****************************************************************************
// LPC407x_8x Microcontroller Startup code for use with LPCXpresso IDE
//
// Version : 150706
//*****************************************************************************
//
// Copyright(C) NXP Semiconductors, 2014-2015
// All rights reserved.
//
// Software that is described herein is for illustrative purposes only
// which provides customers with programming information regarding the
// LPC products.  This software is supplied "AS IS" without any warranties of
// any kind, and NXP Semiconductors and its licensor disclaim any and
// all warranties, express or implied, including all implied warranties of
// merchantability, fitness for a particular purpose and non-infringement of
// intellectual property rights.  NXP Semiconductors assumes no responsibility
// or liability for the use of the software, conveys no license or rights under
// any patent, copyright, mask work right, or any other intellectual property
// rights in or to any products. NXP Semiconductors reserves the right to make
// changes in the software without notification. NXP Semiconductors also makes
// no representation or warranty that such application will be suitable for the
// specified use without further testing or modification.
//
// Permission to use, copy, modify, and distribute this software and its
// documentation is hereby granted, under NXP Semiconductors' and its
// licensor's relevant copyrights in the software, without fee, provided that it
// is used in conjunction with NXP Semiconductors microcontrollers.  This
// copyright, permission, and disclaimer notice must appear in all copies of
// this code.
//*****************************************************************************
#include <FreeRTOS.h>
#include <task.h>

#include <cstdint>
#include <cstring>
#include <iterator>

#include "L0_Platform/ram.hpp"
#include "L0_Platform/startup.hpp"
#include "L1_Peripheral/cortex/dwt_counter.hpp"
#include "L1_Peripheral/cortex/fpu.hpp"
#include "L1_Peripheral/cortex/interrupt.hpp"
#include "L1_Peripheral/cortex/system_timer.hpp"
#include "L1_Peripheral/lpc40xx/system_controller.hpp"
#include "L1_Peripheral/lpc40xx/uart.hpp"
#include "newlib/newlib.hpp"
#include "utility/log.hpp"
#include "utility/macros.hpp"
#include "utility/time.hpp"

// Private namespace to make sure that these do not conflict with other globals
namespace
{
using sjsu::cortex::InterruptController;
// Create LPC40xx system controller to be used by low level initialization.
sjsu::lpc40xx::SystemController system_controller;
// Create timer0 to be used by lower level initialization for uptime calculation
sjsu::cortex::DwtCounter arm_dwt_counter;
// Uart port 0 is used to communicate back to the host computer
sjsu::lpc40xx::Uart uart0(sjsu::lpc40xx::Uart::Port::kUart0);
// System timer is used to count milliseconds of time and to run the RTOS
// scheduler.
sjsu::cortex::SystemTimer system_timer(system_controller);
// Cortex NVIC interrupt controller used to setup FreeRTOS ISRs
sjsu::cortex::InterruptController interrupt_controller;

int Lpc40xxStdOut(const char * data, size_t length)
{
  uart0.Write(reinterpret_cast<const uint8_t *>(data), length);
  return length;
}

int Lpc40xxStdIn(char * data, size_t length)
{
  uart0.Read(reinterpret_cast<uint8_t *>(data), length);
  return length;
}
}  // namespace

extern "C" uint32_t ThreadRuntimeCounter()
{
  return arm_dwt_counter.GetCount();
}

// The entry point for the C++ library startup
extern "C"
{
  // These are defined after the compilation of the FreeRTOS port for Cortex M4F
  // These will link to those definitions.
  extern void xPortPendSVHandler(void);   // NOLINT
  extern void vPortSVCHandler(void);      // NOLINT
  extern void xPortSysTickHandler(void);  // NOLINT
  // Forward declaration of the default handlers. These are aliased.
  // When the application defines a handler (with the same name), the
  // application's handler will automatically take precedence over these weak
  // definitions.
  extern void ArmHardFaultHandler(void);
  extern void ArmResetHandler(void);

  void vPortSetupTimerInterrupt(void)  // NOLINT
  {
    interrupt_controller.Register({
        .interrupt_request_number  = sjsu::cortex::SVCall_IRQn,
        .interrupt_service_routine = vPortSVCHandler,
    });
    interrupt_controller.Register({
        .interrupt_request_number  = sjsu::cortex::PendSV_IRQn,
        .interrupt_service_routine = xPortPendSVHandler,
    });
    // Set the SystemTick frequency to the RTOS tick frequency
    // It is critical that this happens before you set the system_clock,
    // since The system_timer keeps the time that the system_clock uses to
    // delay itself.
    system_timer.SetInterrupt(xPortSysTickHandler);
  }
}

SJ2_SECTION(".crp") const uint32_t kCrpWord = 0xFFFFFFFF;

// The Interrupt vector table.
// This relies on the linker script to place at correct location in memory.
SJ2_SECTION(".isr_vector")
// NOLINTNEXTLINE(readability-identifier-naming)
const sjsu::IsrPointer kInterruptVectorTable[] = {
  // Core Level - CM4
  &StackTop,                           // 0, The initial stack pointer
  ArmResetHandler,                     // 1, The reset handler
  InterruptController::LookupHandler,  // 2, The NMI handler
  ArmHardFaultHandler,                 // 3, The hard fault handler
  InterruptController::LookupHandler,  // 4, The MPU fault handler
  InterruptController::LookupHandler,  // 5, The bus fault handler
  InterruptController::LookupHandler,  // 6, The usage fault handler
  nullptr,                             // 7, Reserved
  nullptr,                             // 8, Reserved
  nullptr,                             // 9, Reserved
  nullptr,                             // 10, Reserved
  vPortSVCHandler,                     // 11, SVCall handler
  InterruptController::LookupHandler,  // 12, Debug monitor handler
  nullptr,                             // 13, Reserved
  xPortPendSVHandler,                  // 14, FreeRTOS PendSV Handler
  InterruptController::LookupHandler,  // 15, The SysTick handler
  // Chip Level - LPC40xx
  InterruptController::LookupHandler,  // 16, 0x40 - WDT
  InterruptController::LookupHandler,  // 17, 0x44 - TIMER0
  InterruptController::LookupHandler,  // 18, 0x48 - TIMER1
  InterruptController::LookupHandler,  // 19, 0x4c - TIMER2
  InterruptController::LookupHandler,  // 20, 0x50 - TIMER3
  InterruptController::LookupHandler,  // 21, 0x54 - UART0
  InterruptController::LookupHandler,  // 22, 0x58 - UART1
  InterruptController::LookupHandler,  // 23, 0x5c - UART2
  InterruptController::LookupHandler,  // 24, 0x60 - UART3
  InterruptController::LookupHandler,  // 25, 0x64 - PWM1
  InterruptController::LookupHandler,  // 26, 0x68 - I2C0
  InterruptController::LookupHandler,  // 27, 0x6c - I2C1
  InterruptController::LookupHandler,  // 28, 0x70 - I2C2
  InterruptController::LookupHandler,  // 29, Not used
  InterruptController::LookupHandler,  // 30, 0x78 - SSP0
  InterruptController::LookupHandler,  // 31, 0x7c - SSP1
  InterruptController::LookupHandler,  // 32, 0x80 - PLL0 (Main PLL)
  InterruptController::LookupHandler,  // 33, 0x84 - RTC
  InterruptController::LookupHandler,  // 34, 0x88 - EINT0
  InterruptController::LookupHandler,  // 35, 0x8c - EINT1
  InterruptController::LookupHandler,  // 36, 0x90 - EINT2
  InterruptController::LookupHandler,  // 37, 0x94 - EINT3
  InterruptController::LookupHandler,  // 38, 0x98 - ADC
  InterruptController::LookupHandler,  // 39, 0x9c - BOD
  InterruptController::LookupHandler,  // 40, 0xA0 - USB
  InterruptController::LookupHandler,  // 41, 0xa4 - CAN
  InterruptController::LookupHandler,  // 42, 0xa8 - GP DMA
  InterruptController::LookupHandler,  // 43, 0xac - I2S
  InterruptController::LookupHandler,  // 44, 0xb0 - Ethernet
  InterruptController::LookupHandler,  // 45, 0xb4 - SD/MMC card I/F
  InterruptController::LookupHandler,  // 46, 0xb8 - Motor Control PWM
  InterruptController::LookupHandler,  // 47, 0xbc - Quadrature Encoder
  InterruptController::LookupHandler,  // 48, 0xc0 - PLL1 (USB PLL)
  InterruptController::LookupHandler,  // 49, 0xc4 - USB Activity interrupt to
                                       // wakeup
  InterruptController::LookupHandler,  // 50, 0xc8 - CAN Activity interrupt to
                                       // wakeup
  InterruptController::LookupHandler,  // 51, 0xcc - UART4
  InterruptController::LookupHandler,  // 52, 0xd0 - SSP2
  InterruptController::LookupHandler,  // 53, 0xd4 - LCD
  InterruptController::LookupHandler,  // 54, 0xd8 - GPIO
  InterruptController::LookupHandler,  // 55, 0xdc - PWM0
  InterruptController::LookupHandler,  // 56, 0xe0 - EEPROM
};

namespace sjsu
{
SJ2_WEAK(void InitializePlatform());
void InitializePlatform()
{
  // Enable FPU (F.loating P.oint U.nit)
  // System will crash if floating point instruction is executed before
  // Initializing the FPU first.
  sjsu::cortex::InitializeFloatingPointUnit();
  // Set Clock Speed
  // SetSystemClockFrequency will timeout return the offset between desire
  // clockspeed and actual clockspeed if the PLL doesn't get a frequency fix
  // within a defined timeout (see L1/system_clock.hpp:kDefaultTimeout)
  system_controller.SetSystemClockFrequency(config::kSystemClockRateMhz);
  // Enable Peripheral Clock and set its divider to 1 meaning the clock speed
  // fed to all peripherals will be 48Mhz.
  system_controller.SetPeripheralClockDivider({}, 1);
  // Set UART0 baudrate, which is required for printf and scanf to work properly
  uart0.Initialize(config::kBaudRate);
  sjsu::newlib::SetStdout(Lpc40xxStdOut);
  sjsu::newlib::SetStdin(Lpc40xxStdIn);

  system_timer.SetTickFrequency(config::kRtosFrequency);
  sjsu::Status timer_start_status = system_timer.StartTimer();

  SJ2_ASSERT_FATAL(timer_start_status == sjsu::Status::kSuccess,
                   "System Timer (used by FreeRTOS) has FAILED to start!");

  arm_dwt_counter.Initialize();
  sjsu::SetUptimeFunction(sjsu::cortex::SystemTimer::GetCount);
}
}  // namespace sjsu
