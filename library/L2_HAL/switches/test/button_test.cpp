#include "L2_HAL/switches/button.hpp"
#include "L4_Testing/testing_frameworks.hpp"

namespace sjsu
{
EMIT_ALL_METHODS(Button);

TEST_CASE("Testing Button", "[button]")
{
  // Make a mock pin to work with
  Mock<sjsu::Pin> mock_pin;
  // Retrieve a reference to the Pin to be injected as the return value
  // of Gpios's GetPin() method.
  sjsu::Pin & test_pin = mock_pin.get();
  // Fake the implementation of SetAsActiveLow and SetPull to be inspected later
  Fake(Method(mock_pin, SetPull));

  // Create a mock gpio object
  Mock<sjsu::Gpio> mock_gpio;
  // Fake Read and SetAsInput so we can inspect them later
  Fake(Method(mock_gpio, Read), Method(mock_gpio, Set),
       Method(mock_gpio, SetDirection));
  // Fake implementation of GetPin() to return our mock_pin reference. So when
  // Button retrieves test_pin from GetPin() it will run our faked
  // SetAsActiveLow and SetPull methods, allowing us to change them later.
  When(Method(mock_gpio, GetPin)).AlwaysReturn(test_pin);

  // Retrieve Gpio reference ot be passed to the test subject
  sjsu::Gpio & test_gpio = mock_gpio.get();

  // Inject test_gpio into button object
  Button test_subject(test_gpio);

  SECTION("Initialize")
  {
    test_subject.Initialize();
    Verify(Method(mock_gpio, SetDirection).Using(Gpio::Direction::kInput),
           Method(mock_pin, SetPull).Using(Pin::Resistor::kPullDown));
  }
  SECTION("Button Released")
  {
    // Reset button state
    test_subject.ResetState();
    // Simulate button being idle
    When(Method(mock_gpio, Read)).AlwaysReturn(false);
    // With this check, the state of the button should be false, and since we
    // have not run the Released() method yet, the Released() method should
    // return false.
    CHECK(!test_subject.Released());
    // Simulate button being pressed
    When(Method(mock_gpio, Read)).AlwaysReturn(true);
    // Button is currently pressed but has not been released, so this method
    // should return false again.
    CHECK(!test_subject.Released());
    // Simulate button being released
    When(Method(mock_gpio, Read)).AlwaysReturn(false);
    // Button has moved from a pressed to released state, so this method should
    // return true.
    CHECK(test_subject.Released());
  }
  SECTION("Button Pressed")
  {
    // Reset button state
    test_subject.ResetState();
    // Simulate button having already been pressed
    When(Method(mock_gpio, Read)).AlwaysReturn(true);
    // With this check, the state of the button should be true, and since we
    // have not run the Pressed() method yet, the Press() method should
    // return false.
    CHECK(!test_subject.Pressed());
    // Simulate button being released
    When(Method(mock_gpio, Read)).AlwaysReturn(false);
    // Button is currently released but has not been pressed, so this method
    // should return false again.
    CHECK(!test_subject.Pressed());
    // Simulate button being pressed
    When(Method(mock_gpio, Read)).AlwaysReturn(true);
    // Button has changed from a low to high state signaling a pressed event,
    // return true.
    CHECK(test_subject.Pressed());
  }
}
}  // namespace sjsu
