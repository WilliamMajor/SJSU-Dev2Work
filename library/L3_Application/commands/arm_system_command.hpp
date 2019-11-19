#pragma once

#include <cinttypes>
#include <iterator>

#include "L1_Peripheral/cortex/interrupt.hpp"
#include "L0_Platform/ram.hpp"
#include "L1_Peripheral/system_controller.hpp"
#include "L3_Application/commandline.hpp"
#include "utility/log.hpp"
#include "utility/time.hpp"

namespace sjsu
{
extern "C" void StackTop(void);
/// The SystemInfoCommand allows the user to get runtime stats about the
// processor like, memory left, heap left, etc ...
class LpcSystemInfoCommand final : public Command
{
 public:
  static constexpr char kDescription[] =
      "Get runtime stats about the  processor like, memory left, and system "
      "speeds";

  explicit constexpr LpcSystemInfoCommand(
      const sjsu::SystemController & system_controller)
      : Command("info", kDescription), system_(system_controller)
  {
  }

  int Program(int, const char * const[]) override
  {
    uint32_t system_frequency     = system_.GetSystemFrequency() / 1_kHz;
    uint32_t peripheral_frequency = system_.GetPeripheralFrequency({}) / 1_kHz;

    intptr_t top_of_stack         = reinterpret_cast<intptr_t>(&StackTop);
    intptr_t master_stack_pointer = sjsu::cortex::__get_MSP();
    intptr_t used_stack           = top_of_stack - master_stack_pointer;
    intptr_t remaining_stack      = 0x10000 - used_stack;
    intptr_t head_used            = heap_position - &heap;
    intptr_t remaining_head       = 0x8000 - head_used;

    printf("System Information:\n");
    printf("-------------------\n");
    printf("    System Clock: %" PRIu32 " kHz\n", system_frequency);
    printf("Peripheral Clock: %" PRIu32 " kHz\n", peripheral_frequency);
    printf("       Stack Top: 0x%" PRIXPTR "\n", top_of_stack);
    printf("   Stack Pointer: 0x%" PRIXPTR "\n", master_stack_pointer);
    printf("      Stack Used: %" PRIuPTR " Bytes\n", used_stack);
    printf(" Remaining Stack: %" PRIuPTR " Bytes\n", remaining_stack);
    printf("        Heap Top: 0x%p\n", &heap);
    printf("    Heap Pointer: 0x%p\n", heap_position);
    printf("       Heap Used: %" PRIuPTR " Bytes\n", head_used);
    printf("  Remaining Heap: %" PRIuPTR " Bytes\n", remaining_head);
    printf("\n");
    return 0;
  }
  const sjsu::SystemController & system_;
};
}  // namespace sjsu
