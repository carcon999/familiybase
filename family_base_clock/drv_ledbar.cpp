#include "arduino.h"
#include "drv_ledbar.h"
#include "ram.h"
#include "test_image.h"

using namespace kirabit;

#define RGB32(r,g,b)  (((uint32_t)r << 16) | ((uint32_t)g <<  8) | b)

// global instance
kirabit::Ledbar LEDBAR;

static Adafruit_NeoPixel led = Adafruit_NeoPixel(8);

// 初期化処理を行う関数
// コンストラクタは　クラス名::クラス名と同じ名前で構成します
Ledbar::Ledbar()
{
}

void Ledbar::setColorTable(int index, const uint8_t* pRGB)
{
  _colororg[index] = RGB32(*pRGB, *(pRGB+1), *(pRGB+2));
  _color[index] = RGB32(*pRGB, *(pRGB+1), *(pRGB+2));
}

void Ledbar::clear(void)
{
  led.clear();
  led.show();
//  delay(1);
}

void Ledbar::setup(uint8_t numPixels, uint8_t pin, neoPixelType t)
{
  led.setPin(pin);
  led.updateLength(numPixels);
  led.updateType(t);
  led.begin();

  _colorSize = 0;
  _width = 0;
  _height = 0;
  _is_ble = false;
  _br_level = 3;
}

void Ledbar::draw(int dir)
{

  if(dir <= 0)
  {
    uint8_t len = _height >> 1;

    for (int j = 0; j < _width ; j++){
      for (int i = 0; i < len; i++) {
        uint8_t val = _share._image[j][i];
        //Serial.println(val);
        led.setPixelColor(_height - 1 - ((i << 1) + 0), _color[val >> 4]);
        led.setPixelColor(_height - 1 - ((i << 1) + 1), _color[val & 0x0F]);
      }
      led.show();
      //delay(1);
    }
  }
  else
  {
    uint8_t len = _height >> 1;

    for (int j = _width - 1; j >= 0 ; j--){
      for (int i = 0; i < len; i++) {
        uint8_t val = _share._image[j][i];
        //Serial.println(val);
        led.setPixelColor(_height - 1 - ((i << 1) + 0), _color[val >> 4]);
        led.setPixelColor(_height - 1 - ((i << 1) + 1), _color[val & 0x0F]);
      }
      led.show();
      //delay(1);
    }    
  }

  clear();
}

bool Ledbar::setImage3(const uint32_t* pColor, uint8_t colorSize, uint16_t pos, uint8_t width, uint8_t height)
{
  bool ret = false;

  if(colorSize > COLOR_MAX){
    return false;
  }

  if(width > IMG_WIDTH){
    return false;
  }

  memcpy(_colororg, pColor, sizeof(uint32_t)* colorSize);
  memcpy(_color, pColor, sizeof(uint32_t)* colorSize);
  
  _width = width;
  _height = height;

  ret = true;

//EXIT:;
  return ret;
}

bool Ledbar::setImage2(const uint32_t* pColor, uint8_t colorSize, const uint16_t* pImage, uint8_t width, uint8_t height)
{
  bool ret = false;

  if(colorSize > COLOR_MAX){
    goto EXIT;
  }

  if(width > IMG_WIDTH){
    goto EXIT;
  }

  ret = true;

EXIT:;
  return ret;
}

bool Ledbar::setImage(const uint32_t* pColor, uint8_t colorSize, const uint8_t* pImage, uint8_t width, uint8_t height)
{
  bool ret = false;

  if(colorSize > COLOR_MAX){
    goto EXIT;
  }

  if(width > IMG_WIDTH || height > IMG_HEIGHT){
    goto EXIT;
  }

  memcpy(_colororg, pColor, sizeof(uint32_t)* colorSize);
  memcpy(_color, pColor, sizeof(uint32_t)* colorSize);
  
  _width = width;
  _height = height;

  // 縦横変換＋16色の4bit形式のデータ構造に変換
  for(int j = 0 ; j < _width ; j++)
  {
    for(int i = 0 ; i < _height ; i+=2)
    {
      const uint8_t* p1 = pImage + ((i + 0) * width) + j;   // 上位４ビット
      const uint8_t* p2 = pImage + ((i + 1) * width) + j;   // 下位４ビット
      uint8_t val = (*p1 << 4) | *p2 ; 
      _share._image[j][i>>1] = val;
    }
  }

  // 明るさも再設定
  setBright(_br_level);

  _is_ble = false;

  ret = true;

EXIT:;
  return ret;
}

#define STR_WIDTH     (16)      // 16pix
#define STR_HEIGHT    (32)      // 32pix
#define CHA_BASE_WIDTH  (176)   // 
#define CHA_BASE_HEIGHT  (32)   // 
#define CHA_SIZE      (8)

#define STR_WIDTH_SM       (8)    // 16pix
#define STR_HEIGHT_SM      (16)   // 32pix
#define CHA_BASE_WIDTH_SM  (96)   // 
#define CHA_BASE_HEIGHT_SM (16)   // 
#define POS_OFFSET         (8)

#define TIME_Y_OFFSET    (24)     // 高さ位置オフセット
#define DAY_X_OFFSET    (10)
#define DAY_Y_OFFSET    (8)

uint8_t tLocation[4][2] = {
  {POS_OFFSET + 0, TIME_Y_OFFSET}, {POS_OFFSET + 16, TIME_Y_OFFSET}, {POS_OFFSET + 34, TIME_Y_OFFSET}, {POS_OFFSET + 50, TIME_Y_OFFSET}
//  {0, 0}, {16, 0}, {34, 0}, {50, 0}
};
uint8_t tLocationsm[4][2] = {
  {POS_OFFSET + 30, TIME_Y_OFFSET+16}, {POS_OFFSET + 64, TIME_Y_OFFSET+16}, {POS_OFFSET + 68, TIME_Y_OFFSET+16}, {POS_OFFSET + 76, TIME_Y_OFFSET+16}
//  {30, 16}, {64, 16}, {68, 16}, {76, 16}
};
uint8_t tLocationYMD[10][2] = {
  {DAY_X_OFFSET+0, DAY_Y_OFFSET}, {DAY_X_OFFSET+8, DAY_Y_OFFSET}, {DAY_X_OFFSET+16, DAY_Y_OFFSET}, {DAY_X_OFFSET+24, DAY_Y_OFFSET}, {DAY_X_OFFSET+32, DAY_Y_OFFSET}, {DAY_X_OFFSET+40, DAY_Y_OFFSET}, {DAY_X_OFFSET+48, DAY_Y_OFFSET}, {DAY_X_OFFSET+56, DAY_Y_OFFSET}, {DAY_X_OFFSET+64, DAY_Y_OFFSET}, {DAY_X_OFFSET+72, DAY_Y_OFFSET}   
};

bool Ledbar::setTime(int yy, uint8_t m, uint8_t dd, uint8_t hh, uint8_t mm, uint8_t ss)
{
  bool ret = false;
  
  uint8_t nums[4];
  nums[0] = hh / 10;
  nums[1] = hh % 10;
  nums[2] = mm / 10;
  nums[3] = mm % 10;

  uint8_t nums_sm[4];
  nums_sm[0] = 10;       // コロン固定
  nums_sm[1] = 10;       // コロン固定
  nums_sm[2] = ss / 10;
  nums_sm[3] = ss % 10;

  uint8_t ymd[10];
  ymd[0] = (uint8_t)(yy/1000);
  ymd[1] = (uint8_t)((yy - ymd[0]*1000)/100);
  ymd[2] = (uint8_t)((yy - ymd[0]*1000 - ymd[1]*100)/10);
  ymd[3] = yy % 10;
  ymd[4] = 11;
  ymd[5] = m/10;
  ymd[6] = m%10;
  ymd[7] = 11;
  ymd[8] = dd/10;
  ymd[9] = dd%10;

  memcpy(_colororg, tColor, sizeof(uint32_t)* 16);
  memcpy(_color, tColor, sizeof(uint32_t)* 16);

  // 表示する領域のサイズ
  _width = 100;
  _height = 70;
  // 領域クリア
  memset(&_share._image[0][0], 0x00, _width * (_height >> 1));

  for(int n = 0 ; n < 4 ; n++)
  {
    uint8_t offset = nums[n] * STR_WIDTH;
    const uint8_t* pImage = &img[0][0] + offset;

    // 縦横変換＋16色の4bit形式のデータ構造に変換
    for(int j = 0 ; j < STR_WIDTH ; j++)
    {
      uint8_t x = tLocation[n][0];
      uint8_t y = tLocation[n][1] >> 1;
      uint8_t pos = n*STR_WIDTH;
      for(int i = 0 ; i < STR_HEIGHT ; i+=2)
      {
        const uint8_t* p1 = pImage + ((i + 0) * CHA_BASE_WIDTH) + j;   // 上位４ビット
        const uint8_t* p2 = pImage + ((i + 1) * CHA_BASE_WIDTH) + j;   // 下位４ビット
        uint8_t val = (*p1 << 4) | *p2 ; 
        if(val != 0){
          _share._image[x + j][y + (i>>1)] = val;
        }
      }
    }    
  }

  for(int n = 0 ; n < 4 ; n++)
  {
    uint8_t offset = nums_sm[n] * STR_WIDTH_SM;
    const uint8_t* pImage = &img_sm[0][0] + offset;

    // 縦横変換＋16色の4bit形式のデータ構造に変換
    for(int j = 0 ; j < STR_WIDTH_SM ; j++)
    {
      uint8_t x = tLocationsm[n][0];
      uint8_t y = tLocationsm[n][1] >> 1;
      uint8_t pos = n*STR_WIDTH_SM;
      for(int i = 0 ; i < STR_HEIGHT_SM ; i+=2)
      {
        const uint8_t* p1 = pImage + ((i + 0) * CHA_BASE_WIDTH_SM) + j;   // 上位４ビット
        const uint8_t* p2 = pImage + ((i + 1) * CHA_BASE_WIDTH_SM) + j;   // 下位４ビット
        uint8_t val = (*p1 << 4) | *p2 ; 
        if(val != 0){
          _share._image[x + j][y + (i>>1)] = val;
        }
      }
    }    
  }

  for(int n = 0 ; n < 10 ; n++)
  {
    uint8_t offset = ymd[n] * STR_WIDTH_SM;
    const uint8_t* pImage = &img_sm[0][0] + offset;

    // 縦横変換＋16色の4bit形式のデータ構造に変換
    for(int j = 0 ; j < STR_WIDTH_SM ; j++)
    {
      uint8_t x = tLocationYMD[n][0];
      uint8_t y = tLocationYMD[n][1] >> 1;
      uint8_t pos = n*STR_WIDTH_SM;
      for(int i = 0 ; i < STR_HEIGHT_SM ; i+=2)
      {
        const uint8_t* p1 = pImage + ((i + 0) * CHA_BASE_WIDTH_SM) + j;   // 上位４ビット
        const uint8_t* p2 = pImage + ((i + 1) * CHA_BASE_WIDTH_SM) + j;   // 下位４ビット
        uint8_t val = (*p1 << 4) | *p2 ; 
        if(val != 0){
          _share._image[x + j][y + (i>>1)] = val;
        }
      }
    }    
  }

  // 明るさも再設定
  setBright(_br_level);

  _is_ble = false;

  ret = true;

EXIT:;
  return ret;
}

// 明るさの変更
// 色テーブルを変更する。
void Ledbar::setBright(uint8_t level)
{
  _br_level = level;

  // オリジナル色を丸コピー
  memcpy(_color, _colororg, sizeof(uint32_t)*COLOR_MAX);

  float offset[] = {0.9f, 0.7f, 0.5f, 0.3f, 0.1f};

  for(int i = 0 ; i < COLOR_MAX ; i++)
  {
      // 色付のみ処理する。
      if(_color[i] != 0)
      {
        float H, S, V;
        rgb_to_hsv(_color[i], &H, &S, &V);
      
        // HSVのBrightnessを変更
        V -= offset[level];
        if(V < 0){
          // 0だと黒になるので多少の色がでるようにする。
          V = 0.01;
        }
    
        // HSVをRGB変換
        hsv_to_rgb(H, S, V, &_color[i]);
      }
  }
}

void Ledbar::rgb_to_hsv(uint32_t color, float* H, float* S, float* V)
{
  float max;
  float min;
  float val;
  float R, G, B;

  // 0.0 - 1.0（255分率）
  R = (float)((color >> 16) & 0x000000FF) / 255;
  G = (float)((color >> 8) & 0x000000FF) / 255;
  B = (float)(color & 0x000000FF) / 255;

  // 最大値・最小値
  if (R >= G && R >= B) {
    max = R;
    min = (G < B) ? G : B;
  } else if (G >= R && G >= B) {
    max = G;
    min = (R < B) ? R : B;
  } else {
    max = B;
    min = (R < G) ? R : G;
  }

  val = max - min;
  
  if(val == 0){
    *H = 0;
  }else{
    // Hue（色相）
    if (max == R) {
      *H = 60 * (G - B) / val;
    } else if (max == G){
      *H = 60 * (B - R) / val + 120;
    } else {
      *H = 60 * (R - G) / val + 240;
    }
    if (*H < 0.0) {
      *H += 360.0;
    }
  }

  // Saturation（彩度）
  if(max != 0){
    *S = val / max;
  }else{
    *S = 0;
  }
  // Value（明度）
  *V = max;
}

void Ledbar::hsv_to_rgb(float H, float S, float V, uint32_t* color)
{
  int Hi;
  float f, p, q, t;

  Hi = ((int)(H / 60)) % 6;
  f = H / 60 - Hi;
  p = V * (1 - S);
  q = V * (1 - f * S);
  t = V * (1 - (1 - f) * S);

  // 256階調に戻す
  V *= 255;
  p *= 255;
  q *= 255;
  t *= 255;

  uint8_t r,g,b;
  switch (Hi) {
    case 0: r = (uint8_t)V; g = (uint8_t)t; b = (uint8_t)p; break;
    case 1: r = (uint8_t)q; g = (uint8_t)V; b = (uint8_t)p; break;
    case 2: r = (uint8_t)p; g = (uint8_t)V; b = (uint8_t)t; break;
    case 3: r = (uint8_t)p; g = (uint8_t)q; b = (uint8_t)V; break;
    case 4: r = (uint8_t)t; g = (uint8_t)p; b = (uint8_t)V; break;
    case 5: r = (uint8_t)V; g = (uint8_t)p; b = (uint8_t)q; break;
  }

  *color = RGB32(r,g,b);
}
