#include "L1_Peripheral/lpc40xx/gpio.hpp"
#include "L1_Peripheral/lpc40xx/spi.hpp"
#include "L2_HAL/displays/oled/ssd1306.hpp"
#include "L3_Application/graphics.hpp"
#include "utility/log.hpp"
#include "utility/time.hpp"

int main()
{
  sjsu::lpc40xx::Spi ssp1(sjsu::lpc40xx::Spi::Bus::kSpi1);
  sjsu::lpc40xx::Gpio cs_gpio(1, 22);
  sjsu::lpc40xx::Gpio dc_gpio(1, 25);

  sjsu::Ssd1306 oled_display(ssp1, cs_gpio, dc_gpio);
  oled_display.Initialize();
  sjsu::Graphics graphics(oled_display);
  LOG_INFO("Starting OLED Hardware Test in 5s...");
  sjsu::Delay(5000ms);

  LOG_INFO("Initializing Graphics library along with OLED Hardware...");
  graphics.Initialize();
  sjsu::Delay(1s);

  LOG_INFO("Clearing Screen...");
  graphics.Clear();
  graphics.Update();
  sjsu::Delay(1s);

  LOG_INFO("Clearing Screen finished. Waiting 1 second...");
  sjsu::Delay(1s);

  LOG_INFO("Drawing Some Shapes...");

  graphics.DrawHorizontalLine(0, sjsu::Ssd1306::kHeight / 2,
                              sjsu::Ssd1306::kWidth);
  graphics.Update();

  graphics.DrawVerticalLine(sjsu::Ssd1306::kWidth / 2, 0,
                            sjsu::Ssd1306::kHeight);
  graphics.Update();

  graphics.DrawRectangle(sjsu::Ssd1306::kWidth / 2 - 10,
                         sjsu::Ssd1306::kHeight / 2 - 10, 20, 20);
  graphics.Update();

  graphics.DrawRectangle(sjsu::Ssd1306::kWidth / 2 - 20,
                         sjsu::Ssd1306::kHeight / 2 - 20, 40, 40);
  graphics.Update();

  graphics.DrawLine(0, 0, sjsu::Ssd1306::kWidth, sjsu::Ssd1306::kHeight);
  graphics.Update();

  graphics.DrawLine(0, sjsu::Ssd1306::kHeight, sjsu::Ssd1306::kWidth, 0);
  graphics.Update();

  graphics.DrawCircle(sjsu::Ssd1306::kWidth / 2, sjsu::Ssd1306::kHeight / 2,
                      20);
  graphics.Update();

  graphics.DrawCircle(sjsu::Ssd1306::kWidth / 2, sjsu::Ssd1306::kHeight / 2,
                      30);
  graphics.Update();

  graphics.DrawCircle(sjsu::Ssd1306::kWidth / 2, sjsu::Ssd1306::kHeight / 2,
                      40);
  graphics.Update();

  graphics.DrawCircle(sjsu::Ssd1306::kWidth / 2, sjsu::Ssd1306::kHeight / 2,
                      60);
  graphics.Update();

  LOG_INFO("Drawing Some names...");
  const char * names[] = { "Name1", "Name2", "Name3", "Name4" };
  for (size_t i = 0; i < std::strlen(names[0]); i++)
  {
    graphics.DrawCharacter(8 * i, 0, names[0][i]);
  }
  graphics.Update();
  for (size_t i = 0; i < std::strlen(names[1]); i++)
  {
    graphics.DrawCharacter(8 * i, sjsu::Ssd1306::kHeight - 8, names[1][i]);
  }
  graphics.Update();
  for (size_t i = 0; i < std::strlen(names[2]); i++)
  {
    int x_pos = (sjsu::Ssd1306::kWidth - (std::strlen(names[2]) * 8)) + (8 * i);
    graphics.DrawCharacter(x_pos, 0, names[2][i]);
  }
  graphics.Update();
  for (size_t i = 0; i < std::strlen(names[3]); i++)
  {
    int x_pos = (sjsu::Ssd1306::kWidth - (std::strlen(names[3]) * 8)) + (8 * i);
    graphics.DrawCharacter(x_pos, sjsu::Ssd1306::kHeight - 8, names[3][i]);
  }
  graphics.Update();

  while (1)
  {
    LOG_INFO("Inverting Screen Colors...");
    oled_display.InvertScreenColor();
    sjsu::Delay(5000ms);
    LOG_INFO("Normalizing Screen Colors...");
    oled_display.NormalScreenColor();
    sjsu::Delay(5000ms);
  }
  return 0;
}
