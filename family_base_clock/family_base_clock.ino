//#include <Adafruit_Microbit.h>
#include "kirabit.h"
#include "drv_ledbar.h"
#include "drv_key.h"
#include "mode_afterimg.h"
#include "drv_l6470.h"
#include "microbit_Screen.h"
#include <Time.h> 


#define MODE_INIT  (0)  // 初期化モード
#define MODE_AUTO  (1)  // 自動描画モード
#define MODE_MANU  (2)  // マニュアルモード
#define MODE_STOP  (3)  // 停止モード


static int m_Mode = MODE_STOP;

static int counter = 0;

void setup() 
{
  Serial.begin(DEF_SERIAL_SPEED);
  Serial.println("microbit matrix demo is ready!");
  delay(1000);

  
  // ボタン
  pinMode(PIN_BUTTON_A, INPUT_PULLUP);
  pinMode(PIN_BUTTON_B, INPUT_PULLUP);
  pinMode(DEF_BTN_PIN, INPUT_PULLUP);

  // 簡易スクリーン表示
  SCREEN.begin();

  // 初期化
  m_Mode = MODE_INIT;
}

bool move_slider(int dir, int step)
{
  // 強制終了時の終了判定
  if(L6470_is_force_term()){ return false;}

  L6470_move(dir, step);//指定方向に指定数ステップする 
  delay(25);

  // 強制終了時の終了判定
  if(L6470_is_force_term()){ return false;}

  // 左右表示
  MODE_AFTERIMG.set_dir(dir == 0);
  MODE_AFTERIMG.main();
  L6470_busydelay(1);

  return true;
}

static unsigned long tm_out;

void loop() 
{
  // 初期化モード
  if(m_Mode == MODE_INIT){
    initialize();

    tm_out = millis() + 1000;
  }
  // 描画モード
  else if(m_Mode == MODE_AUTO)
  {
    if(tm_out < millis())
    {
      tm_out = millis() + 1000;
      
      if(move_slider(1, 5700) && move_slider(0, 5700))
      {
        counter += 2;
      }
      else
      {
        // 終了させる。
        counter = IMAGE_NUM;
      }
    }
  // マニュアルモード
  }else if(m_Mode == MODE_MANU){
    if(digitalRead(DEF_BTN_PIN) == 0)
    {
      if(move_slider(1, 5700) && move_slider(0, 5700))
      {
        counter += 2;
      }
      else
      {
        // 終了させる。
        counter = IMAGE_NUM;
      }
    }
  // 停止モード
  }else{
    // 何もしない。
  }

  // 停止モードへ移行
  if(counter >= IMAGE_NUM ){
    L6470_hardhiz();//回転急停止、保持トルクなし
    m_Mode == MODE_STOP;
  }

  // AUTOモードでボタン押下した場合は、マニュアルモードへ移行
  if(m_Mode == MODE_AUTO && digitalRead(DEF_BTN_PIN) == 0){
    SCREEN.showIcon(IconNames::SmallDiamond);
    m_Mode = MODE_MANU;

  // MANUモードでAボタン押下した場合は、AUTOモードへ移行
  }else if(m_Mode == MODE_MANU && digitalRead(PIN_BUTTON_A) == 0){
    SCREEN.showIcon(IconNames::SmallSquare);
    m_Mode = MODE_AUTO;

    // 但し、位置は、右側になるようにする。
//    if(counter & 0x01){
//      move_slider(0, 7000);
//    }
  }
  
  // ボタン同時押でリセットする。
  if(digitalRead(PIN_BUTTON_A) == 0 && digitalRead(PIN_BUTTON_B) == 0){
    SCREEN.showIcon(IconNames::SmallHeart);
    m_Mode = MODE_INIT;
  }
}

void initialize(void)
{
  Serial.println("initialize start");

  MODE_AFTERIMG.setup();
 
  L6470_setup();
  L6470_init();  //L6470を設定

  // 原点移動
  L6470_move(1, 50);
  L6470_busydelay(200);
  L6470_gountil(0, 0, 20000);
  L6470_busydelay(100);
  L6470_getstatus();  // アラート削除

  // 指定方向に指定数ステップする 
  L6470_move(1,100);
  L6470_busydelay(100);

  m_Mode = MODE_AUTO;
  counter = 0;
  MODE_AFTERIMG.count_reset();

  Serial.println("initialize end.");
}

void L6470_init(){
  delay(1000);
  L6470_resetdevice(); //L6470リセット

  delay(200);
 
  // アラームのクリア
  L6470_setparam_alareen(0xF7); //[R, WS]有効アラームdefault 0xff (1+1+1+1+1+1+1+1bit)
  L6470_setparam_ocdth(15); //[R, WR]過電流しきい値default 0x8 (4bit) (375*val+375[mA]) 1875
  L6470_setparam_stallth(127); //[R, WR]失速電流しきい値？default 0x40 (7bit) (31.25*val+31.25[mA])

  L6470_setparam_acc(3000); //[R, WS] 加速度default 0x08A (12bit) (14.55*val+14.55[step/s^2])
  L6470_setparam_dec(3000); //[R, WS] 減速度default 0x08A (12bit) (14.55*val+14.55[step/s^2])
  L6470_setparam_maxspeed(200); //[R, WR]最大速度default 0x041 (10bit) (15.25*val+15.25[step/s])
  L6470_setparam_minspeed(0x00); //[R, WS]最小速度default 0x000 (1+12bit) (0.238*val[step/s])
  L6470_setparam_fsspd(200); //[R, WR]μステップからフルステップへの切替点速度default 0x027 (10bit) (15.25*val+7.63[step/s])
  
  L6470_setparam_kvalhold(0x20); //[R, WR]停止時励磁電圧default 0x29 (8bit) (Vs[V]*val/256)
  L6470_setparam_kvalrun(0xFF); //[R, WR]定速回転時励磁電圧default 0x29 (8bit) (Vs[V]*val/256)
  L6470_setparam_kvalacc(0xFF); //[R, WR]加速時励磁電圧default 0x29 (8bit) (Vs[V]*val/256)
  L6470_setparam_kvaldec(0xFF); //[R, WR]減速時励磁電圧default 0x29 (8bit) (Vs[V]*val/256)
  L6470_setparam_stslp(0xFF); //[R, WH]逆起電力補償低速時勾配default 0x19 (8bit) (0.000015*val[% s/step])
  L6470_setparam_fnslpacc(0xFF); //[R, WH]逆起電力補償高速時加速勾配default 0x29 (8bit) (0.000015*val[% s/step])
  L6470_setparam_fnslpdec(0xFF); //[R, WH]逆起電力補償高速時減速勾配default 0x29 (8bit) (0.000015*val[% s/step])
  
  L6470_setparam_stepmood(0x03); //ステップモードdefault 0x07 (1+3+1+3bit)
}
