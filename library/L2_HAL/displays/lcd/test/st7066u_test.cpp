// Tests for the St7066u Parallel LCD Driver class.
#include "L2_HAL/displays/lcd/st7066u.hpp"
#include "L4_Testing/testing_frameworks.hpp"

namespace sjsu
{
EMIT_ALL_METHODS(St7066u);

TEST_CASE("Testing St7066u Parallel LCD Driver", "[st70668]")
{
  Mock<Gpio> mock_rs;  // RS:    Register Select
  Mock<Gpio> mock_rw;  // RW:    Read / Write
  Mock<Gpio> mock_e;   // E      Chip Enable
  Mock<Gpio> mock_d7;  // D7-D0: Parallel Data Pins
  Mock<Gpio> mock_d6;
  Mock<Gpio> mock_d5;
  Mock<Gpio> mock_d4;
  Mock<Gpio> mock_d3;
  Mock<Gpio> mock_d2;
  Mock<Gpio> mock_d1;
  Mock<Gpio> mock_d0;

  Fake(Method(mock_rs, SetDirection), Method(mock_rs, Set));
  Fake(Method(mock_rw, SetDirection), Method(mock_rw, Set));
  Fake(Method(mock_e, SetDirection), Method(mock_e, Set));
  Fake(Method(mock_d7, SetDirection), Method(mock_d7, Set));
  Fake(Method(mock_d6, SetDirection), Method(mock_d6, Set));
  Fake(Method(mock_d5, SetDirection), Method(mock_d5, Set));
  Fake(Method(mock_d4, SetDirection), Method(mock_d4, Set));
  Fake(Method(mock_d3, SetDirection), Method(mock_d3, Set));
  Fake(Method(mock_d2, SetDirection), Method(mock_d2, Set));
  Fake(Method(mock_d1, SetDirection), Method(mock_d1, Set));
  Fake(Method(mock_d0, SetDirection), Method(mock_d0, Set));

  St7066u::ControlPins_t pins = { mock_rs.get(), mock_rw.get(), mock_e.get(),
                                  mock_d7.get(), mock_d6.get(), mock_d5.get(),
                                  mock_d4.get(), mock_d3.get(), mock_d2.get(),
                                  mock_d1.get(), mock_d0.get() };

  SECTION("4-bit Bus Initialization")
  {
    St7066u lcd =
        St7066u(St7066u::BusMode::kFourBit, St7066u::DisplayMode::kSingleLine,
                St7066u::FontStyle::kFont5x8, pins);
    lcd.Initialize();
    Verify(Method(mock_rs, SetDirection).Using(Gpio::Direction::kOutput),
           Method(mock_rw, SetDirection).Using(Gpio::Direction::kOutput),
           Method(mock_e, SetDirection).Using(Gpio::Direction::kOutput),
           Method(mock_d7, SetDirection).Using(Gpio::Direction::kOutput),
           Method(mock_d6, SetDirection).Using(Gpio::Direction::kOutput),
           Method(mock_d5, SetDirection).Using(Gpio::Direction::kOutput),
           Method(mock_d4, SetDirection).Using(Gpio::Direction::kOutput));
  }

  SECTION("8-bit Bus Initialization")
  {
    St7066u lcd =
        St7066u(St7066u::BusMode::kEightBit, St7066u::DisplayMode::kMultiLine,
                St7066u::FontStyle::kFont5x8, pins);
    lcd.Initialize();
    // All control pins RS, RW, and D7-D0 pin direction should be set to
    // output.
    Verify(Method(mock_rs, SetDirection).Using(Gpio::Direction::kOutput),
           Method(mock_rw, SetDirection).Using(Gpio::Direction::kOutput),
           Method(mock_e, SetDirection).Using(Gpio::Direction::kOutput),
           Method(mock_d7, SetDirection).Using(Gpio::Direction::kOutput),
           Method(mock_d6, SetDirection).Using(Gpio::Direction::kOutput),
           Method(mock_d5, SetDirection).Using(Gpio::Direction::kOutput),
           Method(mock_d4, SetDirection).Using(Gpio::Direction::kOutput),
           Method(mock_d3, SetDirection).Using(Gpio::Direction::kOutput),
           Method(mock_d2, SetDirection).Using(Gpio::Direction::kOutput),
           Method(mock_d1, SetDirection).Using(Gpio::Direction::kOutput),
           Method(mock_d0, SetDirection).Using(Gpio::Direction::kOutput))
        .Exactly(1);
  }

  SECTION("Write Nibble")
  {
    constexpr uint8_t kNibble = 0b1010;

    St7066u lcd =
        St7066u(St7066u::BusMode::kFourBit, St7066u::DisplayMode::kMultiLine,
                St7066u::FontStyle::kFont5x8, pins);
    lcd.Initialize();
    lcd.WriteNibble(St7066u::WriteOperation::kCommand, kNibble);
    // pins RS, RW, and D7-D4 should all be set before the Chip Enable
    // falling edge write trigger.
    Verify(Method(mock_e, Set), Method(mock_e, Set).Using(Gpio::State::kLow),
           Method(mock_e, Set));
    Verify(Method(mock_rs, Set)
               .Using(Gpio::State(St7066u::WriteOperation::kCommand)),
           Method(mock_rw, Set).Using(Gpio::State::kLow),
           Method(mock_d7, Set).Using(Gpio::State((kNibble >> 3) & 0x01)),
           Method(mock_d6, Set).Using(Gpio::State((kNibble >> 2) & 0x01)),
           Method(mock_d5, Set).Using(Gpio::State((kNibble >> 1) & 0x01)),
           Method(mock_d4, Set).Using(Gpio::State((kNibble >> 0) & 0x01)),
           Method(mock_e, Set).Using(Gpio::State::kLow));

    // Pins D3-D0 should never be used when writing a nibble.
    Verify(Method(mock_d3, Set), Method(mock_d2, Set), Method(mock_d1, Set),
           Method(mock_d0, Set))
        .Never();
  }

  SECTION("Write Byte")
  {
    constexpr uint8_t kCommandByte = 0b1010'1010;
    constexpr uint8_t kDataByte    = static_cast<uint8_t>(~kCommandByte);

    St7066u lcd =
        St7066u(St7066u::BusMode::kEightBit, St7066u::DisplayMode::kMultiLine,
                St7066u::FontStyle::kFont5x8, pins);
    lcd.Initialize();

    lcd.WriteByte(St7066u::WriteOperation::kCommand, kCommandByte);
    // pins RS, RW, and D7-D0 should all be set before the Chip Enable
    // falling edge write trigger.
    Verify(Method(mock_e, Set), Method(mock_e, Set).Using(Gpio::State::kLow),
           Method(mock_e, Set));
    Verify(Method(mock_rs, Set)
               .Using(Gpio::State(St7066u::WriteOperation::kCommand)),
           Method(mock_rw, Set).Using(Gpio::State::kLow),
           Method(mock_d7, Set).Using(Gpio::State((kCommandByte >> 7) & 0x01)),
           Method(mock_d6, Set).Using(Gpio::State((kCommandByte >> 6) & 0x01)),
           Method(mock_d5, Set).Using(Gpio::State((kCommandByte >> 5) & 0x01)),
           Method(mock_d4, Set).Using(Gpio::State((kCommandByte >> 4) & 0x01)),
           Method(mock_d3, Set).Using(Gpio::State((kCommandByte >> 3) & 0x01)),
           Method(mock_d2, Set).Using(Gpio::State((kCommandByte >> 2) & 0x01)),
           Method(mock_d1, Set).Using(Gpio::State((kCommandByte >> 1) & 0x01)),
           Method(mock_d0, Set).Using(Gpio::State((kCommandByte >> 0) & 0x01)),
           Method(mock_e, Set).Using(Gpio::State::kLow));

    lcd.WriteByte(St7066u::WriteOperation::kData, kDataByte);
    Verify(Method(mock_e, Set), Method(mock_e, Set).Using(Gpio::State::kLow),
           Method(mock_e, Set));
    Verify(
        Method(mock_rs, Set).Using(Gpio::State(St7066u::WriteOperation::kData)),
        Method(mock_rw, Set).Using(Gpio::State::kLow),
        Method(mock_d7, Set).Using(Gpio::State((kDataByte >> 7) & 0x01)),
        Method(mock_d6, Set).Using(Gpio::State((kDataByte >> 6) & 0x01)),
        Method(mock_d5, Set).Using(Gpio::State((kDataByte >> 5) & 0x01)),
        Method(mock_d4, Set).Using(Gpio::State((kDataByte >> 4) & 0x01)),
        Method(mock_d3, Set).Using(Gpio::State((kDataByte >> 3) & 0x01)),
        Method(mock_d2, Set).Using(Gpio::State((kDataByte >> 2) & 0x01)),
        Method(mock_d1, Set).Using(Gpio::State((kDataByte >> 1) & 0x01)),
        Method(mock_d0, Set).Using(Gpio::State((kDataByte >> 0) & 0x01)),
        Method(mock_e, Set).Using(Gpio::State::kLow));
  }

  SECTION("Write Command / Data on 4-bit Bus")
  {
    constexpr uint8_t kByte = 0b0101'1010;

    St7066u lcd =
        St7066u(St7066u::BusMode::kFourBit, St7066u::DisplayMode::kMultiLine,
                St7066u::FontStyle::kFont5x8, pins);
    lcd.Initialize();

    lcd.WriteCommand(kByte);
    // The RS pin should be set to write to the instruction register.
    // The upper nibble should be sent first through D7-D4 followed
    // by the lower nibble on the same data lines.
    // The enable pin should ouput low after each nibble is set.
    Verify(Method(mock_rs, Set)
               .Using(Gpio::State(St7066u::WriteOperation::kCommand)),
           Method(mock_rw, Set).Using(Gpio::State::kLow),
           Method(mock_d7, Set).Using(Gpio::State((kByte >> 7) & 0x01)),
           Method(mock_d6, Set).Using(Gpio::State((kByte >> 6) & 0x01)),
           Method(mock_d5, Set).Using(Gpio::State((kByte >> 5) & 0x01)),
           Method(mock_d4, Set).Using(Gpio::State((kByte >> 4) & 0x01)),
           Method(mock_e, Set).Using(Gpio::State::kLow),
           Method(mock_rs, Set)
               .Using(Gpio::State(St7066u::WriteOperation::kCommand)),
           Method(mock_rw, Set).Using(Gpio::State::kLow),
           Method(mock_d7, Set).Using(Gpio::State((kByte >> 3) & 0x01)),
           Method(mock_d6, Set).Using(Gpio::State((kByte >> 2) & 0x01)),
           Method(mock_d5, Set).Using(Gpio::State((kByte >> 1) & 0x01)),
           Method(mock_d4, Set).Using(Gpio::State((kByte >> 0) & 0x01)),
           Method(mock_e, Set).Using(Gpio::State::kLow));

    lcd.WriteData(kByte);
    // The RS pin should be set to write to the data register.
    // The upper nibble should be sent first through D7-D4 followed
    // by the lower nibble on the same data lines.
    // The enable pin should ouput low after each nibble is set.
    Verify(
        Method(mock_rs, Set).Using(Gpio::State(St7066u::WriteOperation::kData)),
        Method(mock_rw, Set).Using(Gpio::State::kLow),
        Method(mock_d7, Set).Using(Gpio::State((kByte >> 7) & 0x01)),
        Method(mock_d6, Set).Using(Gpio::State((kByte >> 6) & 0x01)),
        Method(mock_d5, Set).Using(Gpio::State((kByte >> 5) & 0x01)),
        Method(mock_d4, Set).Using(Gpio::State((kByte >> 4) & 0x01)),
        Method(mock_e, Set).Using(Gpio::State::kLow),
        Method(mock_rs, Set).Using(Gpio::State(St7066u::WriteOperation::kData)),
        Method(mock_rw, Set).Using(Gpio::State::kLow),
        Method(mock_d7, Set).Using(Gpio::State((kByte >> 3) & 0x01)),
        Method(mock_d6, Set).Using(Gpio::State((kByte >> 2) & 0x01)),
        Method(mock_d5, Set).Using(Gpio::State((kByte >> 1) & 0x01)),
        Method(mock_d4, Set).Using(Gpio::State((kByte >> 0) & 0x01)),
        Method(mock_e, Set).Using(Gpio::State::kLow));
  }

  SECTION("Write Command / Data on 8-bit Bus")
  {
    constexpr uint8_t kByte = 0b0101'1010;

    St7066u lcd =
        St7066u(St7066u::BusMode::kEightBit, St7066u::DisplayMode::kMultiLine,
                St7066u::FontStyle::kFont5x8, pins);
    lcd.Initialize();

    lcd.WriteCommand(kByte);
    // The RS pin should be set to write to the instruction register.
    // The enable pin should output low after D7-D0 are set.
    Verify(Method(mock_rs, Set)
               .Using(Gpio::State(St7066u::WriteOperation::kCommand)),
           Method(mock_rw, Set).Using(Gpio::State::kLow),
           Method(mock_d7, Set).Using(Gpio::State((kByte >> 7) & 0x01)),
           Method(mock_d6, Set).Using(Gpio::State((kByte >> 6) & 0x01)),
           Method(mock_d5, Set).Using(Gpio::State((kByte >> 5) & 0x01)),
           Method(mock_d4, Set).Using(Gpio::State((kByte >> 4) & 0x01)),
           Method(mock_d3, Set).Using(Gpio::State((kByte >> 3) & 0x01)),
           Method(mock_d2, Set).Using(Gpio::State((kByte >> 2) & 0x01)),
           Method(mock_d1, Set).Using(Gpio::State((kByte >> 1) & 0x01)),
           Method(mock_d0, Set).Using(Gpio::State((kByte >> 0) & 0x01)),
           Method(mock_e, Set).Using(Gpio::State::kLow));

    lcd.WriteData(kByte);
    // The RS pin should be set to write to the data register.
    // The enable pin should output low after D7-D0 are set.
    Verify(
        Method(mock_rs, Set).Using(Gpio::State(St7066u::WriteOperation::kData)),
        Method(mock_rw, Set).Using(Gpio::State::kLow),
        Method(mock_d7, Set).Using(Gpio::State((kByte >> 7) & 0x01)),
        Method(mock_d6, Set).Using(Gpio::State((kByte >> 6) & 0x01)),
        Method(mock_d5, Set).Using(Gpio::State((kByte >> 5) & 0x01)),
        Method(mock_d4, Set).Using(Gpio::State((kByte >> 4) & 0x01)),
        Method(mock_d3, Set).Using(Gpio::State((kByte >> 3) & 0x01)),
        Method(mock_d2, Set).Using(Gpio::State((kByte >> 2) & 0x01)),
        Method(mock_d1, Set).Using(Gpio::State((kByte >> 1) & 0x01)),
        Method(mock_d0, Set).Using(Gpio::State((kByte >> 0) & 0x01)),
        Method(mock_e, Set).Using(Gpio::State::kLow));
  }

  SECTION("Clearing the Display")
  {
    constexpr uint8_t kClearDisplay =
        static_cast<uint8_t>(St7066u::Command::kClearDisplay);

    St7066u lcd =
        St7066u(St7066u::BusMode::kEightBit, St7066u::DisplayMode::kSingleLine,
                St7066u::FontStyle::kFont5x8, pins);

    lcd.ClearDisplay();

    Verify(Method(mock_e, Set), Method(mock_e, Set).Using(Gpio::State::kLow),
           Method(mock_e, Set));
    Verify(Method(mock_rs, Set)
               .Using(Gpio::State(St7066u::WriteOperation::kCommand)),
           Method(mock_rw, Set).Using(Gpio::State::kLow),
           Method(mock_d7, Set).Using(Gpio::State((kClearDisplay >> 7) & 0x01)),
           Method(mock_d6, Set).Using(Gpio::State((kClearDisplay >> 6) & 0x01)),
           Method(mock_d5, Set).Using(Gpio::State((kClearDisplay >> 5) & 0x01)),
           Method(mock_d4, Set).Using(Gpio::State((kClearDisplay >> 4) & 0x01)),
           Method(mock_d3, Set).Using(Gpio::State((kClearDisplay >> 3) & 0x01)),
           Method(mock_d2, Set).Using(Gpio::State((kClearDisplay >> 2) & 0x01)),
           Method(mock_d1, Set).Using(Gpio::State((kClearDisplay >> 1) & 0x01)),
           Method(mock_d0, Set).Using(Gpio::State((kClearDisplay >> 0) & 0x01)),
           Method(mock_e, Set).Using(Gpio::State::kLow));
  }

  SECTION("Turning Display ON / OFF")
  {
    const uint8_t kDisplayOn =
        static_cast<uint8_t>(St7066u::Command::kTurnDisplayOn);
    const uint8_t kDisplayOff =
        static_cast<uint8_t>(St7066u::Command::kTurnDisplayOff);

    St7066u lcd =
        St7066u(St7066u::BusMode::kEightBit, St7066u::DisplayMode::kMultiLine,
                St7066u::FontStyle::kFont5x8, pins);
    lcd.Initialize();

    lcd.SetDisplayOn();
    // The driver should send the kDisplayOn byte to turn on the display.
    Verify(Method(mock_e, Set), Method(mock_e, Set).Using(Gpio::State::kLow),
           Method(mock_e, Set));
    Verify(Method(mock_rs, Set)
               .Using(Gpio::State(St7066u::WriteOperation::kCommand)),
           Method(mock_rw, Set).Using(Gpio::State::kLow),
           Method(mock_d7, Set).Using(Gpio::State((kDisplayOn >> 7) & 0x01)),
           Method(mock_d6, Set).Using(Gpio::State((kDisplayOn >> 6) & 0x01)),
           Method(mock_d5, Set).Using(Gpio::State((kDisplayOn >> 5) & 0x01)),
           Method(mock_d4, Set).Using(Gpio::State((kDisplayOn >> 4) & 0x01)),
           Method(mock_d3, Set).Using(Gpio::State((kDisplayOn >> 3) & 0x01)),
           Method(mock_d2, Set).Using(Gpio::State((kDisplayOn >> 2) & 0x01)),
           Method(mock_d1, Set).Using(Gpio::State((kDisplayOn >> 1) & 0x01)),
           Method(mock_d0, Set).Using(Gpio::State((kDisplayOn >> 0) & 0x01)),
           Method(mock_e, Set).Using(Gpio::State::kLow));

    lcd.SetDisplayOn(false);
    // The driver should send the kDisplayOff byte to turn off the display.
    Verify(Method(mock_e, Set), Method(mock_e, Set).Using(Gpio::State::kLow),
           Method(mock_e, Set));
    Verify(Method(mock_rs, Set)
               .Using(Gpio::State(St7066u::WriteOperation::kCommand)),
           Method(mock_rw, Set).Using(Gpio::State::kLow),
           Method(mock_d7, Set).Using(Gpio::State((kDisplayOff >> 7) & 0x01)),
           Method(mock_d6, Set).Using(Gpio::State((kDisplayOff >> 6) & 0x01)),
           Method(mock_d5, Set).Using(Gpio::State((kDisplayOff >> 5) & 0x01)),
           Method(mock_d4, Set).Using(Gpio::State((kDisplayOff >> 4) & 0x01)),
           Method(mock_d3, Set).Using(Gpio::State((kDisplayOff >> 3) & 0x01)),
           Method(mock_d2, Set).Using(Gpio::State((kDisplayOff >> 2) & 0x01)),
           Method(mock_d1, Set).Using(Gpio::State((kDisplayOff >> 1) & 0x01)),
           Method(mock_d0, Set).Using(Gpio::State((kDisplayOff >> 0) & 0x01)),
           Method(mock_e, Set).Using(Gpio::State::kLow));
  }

  SECTION("Show / Hide Cursor")
  {
    const uint8_t kShowCursor =
        static_cast<uint8_t>(St7066u::Command::kTurnCursorOn);
    const uint8_t kHideCursor =
        static_cast<uint8_t>(St7066u::Command::kTurnDisplayOn);

    St7066u lcd =
        St7066u(St7066u::BusMode::kEightBit, St7066u::DisplayMode::kMultiLine,
                St7066u::FontStyle::kFont5x8, pins);
    lcd.Initialize();

    lcd.SetCursorHidden(false);
    // The driver should send the kShowCursor byte to show the cursor.
    Verify(Method(mock_e, Set), Method(mock_e, Set).Using(Gpio::State::kLow),
           Method(mock_e, Set));
    Verify(Method(mock_rs, Set)
               .Using(Gpio::State(St7066u::WriteOperation::kCommand)),
           Method(mock_rw, Set).Using(Gpio::State::kLow),
           Method(mock_d7, Set).Using(Gpio::State((kShowCursor >> 7) & 0x01)),
           Method(mock_d6, Set).Using(Gpio::State((kShowCursor >> 6) & 0x01)),
           Method(mock_d5, Set).Using(Gpio::State((kShowCursor >> 5) & 0x01)),
           Method(mock_d4, Set).Using(Gpio::State((kShowCursor >> 4) & 0x01)),
           Method(mock_d3, Set).Using(Gpio::State((kShowCursor >> 3) & 0x01)),
           Method(mock_d2, Set).Using(Gpio::State((kShowCursor >> 2) & 0x01)),
           Method(mock_d1, Set).Using(Gpio::State((kShowCursor >> 1) & 0x01)),
           Method(mock_d0, Set).Using(Gpio::State((kShowCursor >> 0) & 0x01)),
           Method(mock_e, Set).Using(Gpio::State::kLow));

    lcd.SetCursorHidden();
    // The driver should send the kHideCursor byte to hide the cursor.
    Verify(Method(mock_e, Set), Method(mock_e, Set).Using(Gpio::State::kLow),
           Method(mock_e, Set));
    Verify(Method(mock_rs, Set)
               .Using(Gpio::State(St7066u::WriteOperation::kCommand)),
           Method(mock_rw, Set).Using(Gpio::State::kLow),
           Method(mock_d7, Set).Using(Gpio::State((kHideCursor >> 7) & 0x01)),
           Method(mock_d6, Set).Using(Gpio::State((kHideCursor >> 6) & 0x01)),
           Method(mock_d5, Set).Using(Gpio::State((kHideCursor >> 5) & 0x01)),
           Method(mock_d4, Set).Using(Gpio::State((kHideCursor >> 4) & 0x01)),
           Method(mock_d3, Set).Using(Gpio::State((kHideCursor >> 3) & 0x01)),
           Method(mock_d2, Set).Using(Gpio::State((kHideCursor >> 2) & 0x01)),
           Method(mock_d1, Set).Using(Gpio::State((kHideCursor >> 1) & 0x01)),
           Method(mock_d0, Set).Using(Gpio::State((kHideCursor >> 0) & 0x01)),
           Method(mock_e, Set).Using(Gpio::State::kLow));
  }

  SECTION("Setting Cursor Direction")
  {
    constexpr uint8_t kCursorDirectionBackward =
        static_cast<uint8_t>(St7066u::Command::kCursorDirectionBackward);
    constexpr uint8_t kCursorDirectionForward =
        static_cast<uint8_t>(St7066u::Command::kCursorDirectionForward);

    St7066u lcd =
        St7066u(St7066u::BusMode::kEightBit, St7066u::DisplayMode::kMultiLine,
                St7066u::FontStyle::kFont5x8, pins);
    lcd.Initialize();

    lcd.SetCursorDirection(St7066u::CursorDirection::kBackward);
    // The driver should send the kShowCursor byte to show the cursor.
    Verify(Method(mock_e, Set), Method(mock_e, Set).Using(Gpio::State::kLow),
           Method(mock_e, Set));
    Verify(Method(mock_rs, Set)
               .Using(Gpio::State(St7066u::WriteOperation::kCommand)),
           Method(mock_rw, Set).Using(Gpio::State::kLow),
           Method(mock_d7, Set)
               .Using(Gpio::State((kCursorDirectionBackward >> 7) & 0x01)),
           Method(mock_d6, Set)
               .Using(Gpio::State((kCursorDirectionBackward >> 6) & 0x01)),
           Method(mock_d5, Set)
               .Using(Gpio::State((kCursorDirectionBackward >> 5) & 0x01)),
           Method(mock_d4, Set)
               .Using(Gpio::State((kCursorDirectionBackward >> 4) & 0x01)),
           Method(mock_d3, Set)
               .Using(Gpio::State((kCursorDirectionBackward >> 3) & 0x01)),
           Method(mock_d2, Set)
               .Using(Gpio::State((kCursorDirectionBackward >> 2) & 0x01)),
           Method(mock_d1, Set)
               .Using(Gpio::State((kCursorDirectionBackward >> 1) & 0x01)),
           Method(mock_d0, Set)
               .Using(Gpio::State((kCursorDirectionBackward >> 0) & 0x01)),
           Method(mock_e, Set).Using(Gpio::State::kLow));

    lcd.SetCursorDirection(St7066u::CursorDirection::kForward);
    // The driver should send the kHideCursor byte to hide the cursor.
    Verify(Method(mock_e, Set), Method(mock_e, Set).Using(Gpio::State::kLow),
           Method(mock_e, Set));
    Verify(Method(mock_rs, Set)
               .Using(Gpio::State(St7066u::WriteOperation::kCommand)),
           Method(mock_rw, Set).Using(Gpio::State::kLow),
           Method(mock_d7, Set)
               .Using(Gpio::State((kCursorDirectionForward >> 7) & 0x01)),
           Method(mock_d6, Set)
               .Using(Gpio::State((kCursorDirectionForward >> 6) & 0x01)),
           Method(mock_d5, Set)
               .Using(Gpio::State((kCursorDirectionForward >> 5) & 0x01)),
           Method(mock_d4, Set)
               .Using(Gpio::State((kCursorDirectionForward >> 4) & 0x01)),
           Method(mock_d3, Set)
               .Using(Gpio::State((kCursorDirectionForward >> 3) & 0x01)),
           Method(mock_d2, Set)
               .Using(Gpio::State((kCursorDirectionForward >> 2) & 0x01)),
           Method(mock_d1, Set)
               .Using(Gpio::State((kCursorDirectionForward >> 1) & 0x01)),
           Method(mock_d0, Set)
               .Using(Gpio::State((kCursorDirectionForward >> 0) & 0x01)),
           Method(mock_e, Set).Using(Gpio::State::kLow));
  }

  SECTION("Setting Cursor Position")
  {
    constexpr uint8_t kLineNumber = 2;
    constexpr uint8_t kPosition   = 0x12;
    // Address should be 0x94 + 0x12 = 0xA6
    constexpr uint8_t kCursorAddress =
        static_cast<uint8_t>(St7066u::Command::kDisplayLineAddress2) +
        kPosition;

    St7066u lcd =
        St7066u(St7066u::BusMode::kEightBit, St7066u::DisplayMode::kMultiLine,
                St7066u::FontStyle::kFont5x8, pins);
    lcd.Initialize();

    lcd.SetCursorPosition(St7066u::CursorPosition_t{ kLineNumber, kPosition });
    // The driver should send the kCursorAddress byte to set the cursor
    // position address.
    Verify(Method(mock_e, Set), Method(mock_e, Set).Using(Gpio::State::kLow),
           Method(mock_e, Set));
    Verify(
        Method(mock_rs, Set)
            .Using(Gpio::State(St7066u::WriteOperation::kCommand)),
        Method(mock_rw, Set).Using(Gpio::State::kLow),
        Method(mock_d7, Set).Using(Gpio::State((kCursorAddress >> 7) & 0x01)),
        Method(mock_d6, Set).Using(Gpio::State((kCursorAddress >> 6) & 0x01)),
        Method(mock_d5, Set).Using(Gpio::State((kCursorAddress >> 5) & 0x01)),
        Method(mock_d4, Set).Using(Gpio::State((kCursorAddress >> 4) & 0x01)),
        Method(mock_d3, Set).Using(Gpio::State((kCursorAddress >> 3) & 0x01)),
        Method(mock_d2, Set).Using(Gpio::State((kCursorAddress >> 2) & 0x01)),
        Method(mock_d1, Set).Using(Gpio::State((kCursorAddress >> 1) & 0x01)),
        Method(mock_d0, Set).Using(Gpio::State((kCursorAddress >> 0) & 0x01)),
        Method(mock_e, Set).Using(Gpio::State::kLow));
  }

  SECTION("Reset Cursor Position")
  {
    constexpr uint8_t kClearDisplay =
        static_cast<uint8_t>(St7066u::Command::kClearDisplay);

    St7066u lcd =
        St7066u(St7066u::BusMode::kEightBit, St7066u::DisplayMode::kMultiLine,
                St7066u::FontStyle::kFont5x8, pins);
    lcd.Initialize();

    lcd.ResetCursorPosition();
    // The driver should send the kCursorAddress byte to set the cursor
    // position address.
    Verify(Method(mock_e, Set), Method(mock_e, Set).Using(Gpio::State::kLow),
           Method(mock_e, Set));
    Verify(Method(mock_rs, Set)
               .Using(Gpio::State(St7066u::WriteOperation::kCommand)),
           Method(mock_rw, Set).Using(Gpio::State::kLow),
           Method(mock_d7, Set).Using(Gpio::State((kClearDisplay >> 7) & 0x01)),
           Method(mock_d6, Set).Using(Gpio::State((kClearDisplay >> 6) & 0x01)),
           Method(mock_d5, Set).Using(Gpio::State((kClearDisplay >> 5) & 0x01)),
           Method(mock_d4, Set).Using(Gpio::State((kClearDisplay >> 4) & 0x01)),
           Method(mock_d3, Set).Using(Gpio::State((kClearDisplay >> 3) & 0x01)),
           Method(mock_d2, Set).Using(Gpio::State((kClearDisplay >> 2) & 0x01)),
           Method(mock_d1, Set).Using(Gpio::State((kClearDisplay >> 1) & 0x01)),
           Method(mock_d0, Set).Using(Gpio::State((kClearDisplay >> 0) & 0x01)),
           Method(mock_e, Set).Using(Gpio::State::kLow));
  }

  SECTION("Display Text")
  {
    const char kText[] = "Text String";

    St7066u lcd =
        St7066u(St7066u::BusMode::kEightBit, St7066u::DisplayMode::kMultiLine,
                St7066u::FontStyle::kFont5x8, pins);
    lcd.Initialize();

    lcd.DisplayText(kText);
    for (uint8_t i = 0; i < strlen(kText); i++)
    {
      Verify(Method(mock_rs, Set)
                 .Using(Gpio::State(St7066u::WriteOperation::kCommand)),
             Method(mock_rw, Set).Using(Gpio::State::kLow),
             Method(mock_d7, Set).Using(Gpio::State((kText[i] >> 7) & 0x01)),
             Method(mock_d6, Set).Using(Gpio::State((kText[i] >> 6) & 0x01)),
             Method(mock_d5, Set).Using(Gpio::State((kText[i] >> 5) & 0x01)),
             Method(mock_d4, Set).Using(Gpio::State((kText[i] >> 4) & 0x01)),
             Method(mock_d3, Set).Using(Gpio::State((kText[i] >> 3) & 0x01)),
             Method(mock_d2, Set).Using(Gpio::State((kText[i] >> 2) & 0x01)),
             Method(mock_d1, Set).Using(Gpio::State((kText[i] >> 1) & 0x01)),
             Method(mock_d0, Set).Using(Gpio::State((kText[i] >> 0) & 0x01)),
             Method(mock_e, Set).Using(Gpio::State::kLow));
    }
  }
}
}  // namespace sjsu
