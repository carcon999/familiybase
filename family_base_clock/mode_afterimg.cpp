#include "kirabit.h"
#include "arduino.h"
#include "MMA8653.h"
#include "drv_ledbar.h"
#include "drv_key.h"
#include "mode_afterimg.h"
#include "test_image.h"
#include "ram.h"
#include <Wire.h>
#include "RTClib.h"
RTC_DS1307 RTC;
using namespace kirabit;

// global instance
kirabit::ModeAfterimg MODE_AFTERIMG;

#define CHANGE_COUNT   (2)

ModeAfterimg::ModeAfterimg()
{
}

void ModeAfterimg::setup(void)
{
  Wire.begin();
  RTC.begin();

  if (! RTC.isrunning()) 
  {
    Serial.println("RTC is NOT running!");
    // following line sets the RTC to the date & time this sketch was compiled
    RTC.adjust(DateTime(__DATE__, __TIME__));
  }
  // RTC.adjust(DateTime(__DATE__, __TIME__));

  LEDBAR.setup(DEF_LED_NUM, DEF_LED_PIN);
  img_index = 0;
  nextimage();
}

void ModeAfterimg::start(void)
{
  ModeBase::start();
}

void ModeAfterimg::stop(void)
{
}

void ModeAfterimg::main(void)
{
  LEDBAR.draw(_dir);

  counter++;
  if(counter >= CHANGE_COUNT)
  {
    nextimage();
    counter = 0;
  }
}

void ModeAfterimg::count_reset(void)
{
  counter = 0;
  img_index = 0;
  memset(&_share._image[0][0], 0x00, IMG_WIDTH*(IMG_HEIGHT>>1));
}

void ModeAfterimg::nextimage(void)
{
 
  DateTime now = RTC.now(); 

  uint8_t hh = now.hour();
  uint8_t mm = now.minute();
  uint8_t ss = now.second();

  int y = now.year();
  uint8_t m = now.month();
  uint8_t d = now.day();

  LEDBAR.setTime(y, m, d, hh, mm, ss);

  img_index += DRAW_SKIP;
  if(img_index >= IMAGE_NUMEX){
    img_index = 0;
  }
}
