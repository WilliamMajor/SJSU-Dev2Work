#include "newlib/newlib.hpp"

namespace sjsu
{
namespace newlib
{
int DoNothingStdOut(const char *, size_t)
{
  return 1;
}
int DoNothingStdIn(char *, size_t)
{
  return 0;
}
void SetStdout(Stdout stdout_handler)
{
  out = stdout_handler;
}
void SetStdin(Stdin stdin_handler)
{
  in = stdin_handler;
}
void StdinEchoBack(bool enable_echo)
{
  echo_back_is_enabled = enable_echo;
}
}  // namespace newlib
}  // namespace sjsu

// Needed by third party "printf" library
extern "C" void _putchar(char character)  // NOLINT
{
  sjsu::newlib::out(&character, 1);
}
