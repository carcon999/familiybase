/*
 * kirabit.h
 * kirabit共通マクロ定義
 * 
 */
#ifndef kirabit_h
#define kirabit_h

#include "arduino.h"

namespace kirabit {

//#define DEF_BEEP_PIN  (2)     // 音声出力PIN番号
#define DEF_LED_PIN   (1)     // LED出力PIN番号
#define DEF_BTN_PIN   (2)     // ボタンPIN番号

#define DEF_LED_NUM   (72)    // LED個数
#define LED_RGB       (3)     // RGB数

#define COLOR_MAX     (16)    // 最大色数
#define IMG_WIDTH     (120)    // 最大幅
#define IMG_HEIGHT    (72)    // 最大高さ

#define IMAGE_NUMEX   (1)
#define IMAGE_NUM     (IMAGE_NUMEX / DRAW_SKIP)    // 
#define DRAW_SKIP     (1)     // 画像スキップ

#define BMP_WIDTH    (96)
#define BMP_HEIGHT   (72)

#define BMP_H_BYTE   (BMP_HEIGHT >> 3)  // 縦のデータバイト数

#define DEF_SERIAL_SPEED 115200

} // namespace kirabit

#endif  // kirabit_h
