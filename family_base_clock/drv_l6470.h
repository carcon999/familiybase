/*
 * drv_l6470.h
 * L6470 コントロール
 * 
 * 
 * 
 */
#ifndef drv_l6470_h
#define drv_l6470_h

/*ver 1.00 2013/4/24*/
/*ver 1.01 2013/12/14 コメント追記*/

/*L6470 コントロールコマンド
 引数-----------------------
 dia   1:正転 0:逆転,
 spd  (20bit)(0.015*spd[step/s])
 pos  (22bit)
 n_step (22bit)
 act   1:絶対座標をマーク  0:絶対座標リセット
 mssec ミリ秒
 val 各レジスタに書き込む値
 ---------------------------
 */
void L6470_setup(void); // SPI セットアップ
void L6470_run(int dia,long spd); //指定方向に連続回転
void L6470_stepclock(int dia); //指定方向にstepピンのクロックで回転
void L6470_move(int dia,long n_step); //指定方向に指定数ステップする 
void L6470_goto(long pos);//指定座標に最短でいける回転方向で移動
void L6470_gotodia(int dia,long pos);//回転方向を指定して指定座標に移動
void L6470_gountil(int act,int dia,long spd);//指定した回転方向に指定した速度で回転し、スイッチのONで急停止と座標処理
void L6470_relesesw(int act,int dia);//スイッチがOFFに戻るまで最低速度で回転し、停止と座標処理
void L6470_gohome();//座標原点に移動
void L6470_gomark();//マーク座標に移動
void L6470_resetpos();//絶対座標リセット
void L6470_resetdevice(); //L6470リセット
void L6470_softstop();//回転停止、保持トルクあり
void L6470_hardstop();//回転急停止、保持トルクあり
void L6470_softhiz(); //回転停止、保持トルクなし
void L6470_hardhiz(); //回転急停止、保持トルクなし
long L6470_getstatus(); //statusレジスタの値を返す （L6470_getparam_status();と同じ）
 
void L6470_busydelay(long msec);//busyフラグがHIGHになってから、指定ミリ秒待つ。
bool L6470_is_force_term(void);// 中断判定
 
// レジスタ書き込みコマンド
void L6470_setparam_abspos(long val); //[R, WS]現在座標default 0x000000 (22bit)
void L6470_setparam_elpos(long val); //[R, WS]コイル励磁の電気的位置default 0x000 (2+7bit)
void L6470_setparam_mark(long val); //[R, WR]マーク座標default 0x000000 (22bit)
 //ありませんL6470_spped //[R] 現在速度read onry  (20bit)
void L6470_setparam_acc(long val); //[R, WS] 加速度default 0x08A (12bit) (14.55*val+14.55[step/s^2])
void L6470_setparam_dec(long val); //[R, WS] 減速度default 0x08A (12bit) (14.55*val+14.55[step/s^2])
void L6470_setparam_maxspeed(long val); //[R, WR]最大速度default 0x041 (10bit) (15.25*val+15.25[step/s])
void L6470_setparam_minspeed(long val); //[R, WS]最小速度default 0x000 (1+12bit) (0.238*val+[step/s])
void L6470_setparam_fsspd(long val); //[R, WR]μステップからフルステップへの切替点速度default 0x027 (10bit) (15.25*val+7.63[step/s])
void L6470_setparam_kvalhold(long val); //[R, WR]停止時励磁電圧default 0x29 (8bit) (Vs[V]*val/256)
void L6470_setparam_kvalrun(long val); //[R, WR]定速回転時励磁電圧default 0x29 (8bit) (Vs[V]*val/256)
void L6470_setparam_kvalacc(long val); //[R, WR]加速時励磁電圧default 0x29 (8bit) (Vs[V]*val/256)
void L6470_setparam_kvaldec(long val); //[R, WR]減速時励磁電圧default 0x29 (8bit) (Vs[V]*val/256)
void L6470_setparam_intspd(long val); //[R, WH]逆起電力補償切替点速度default 0x0408 (14bit) (0.238*val[step/s])
void L6470_setparam_stslp(long val); //[R, WH]逆起電力補償低速時勾配default 0x19 (8bit) (0.000015*val[% s/step])
void L6470_setparam_fnslpacc(long val); //[R, WH]逆起電力補償高速時加速勾配default 0x29 (8bit) (0.000015*val[% s/step])
void L6470_setparam_fnslpdec(long val); //[R, WH]逆起電力補償高速時減速勾配default 0x29 (8bit) (0.000015*val[% s/step])
void L6470_setparam_ktherm(long val); //[R, WR]不明default 0x0 (4bit) (0.03125*val+1)
 //ありませんL6470_adcout //[R] read onry (5bit) ADCによる逆起電力補償の大きさかな？
void L6470_setparam_ocdth(long val); //[R, WR]過電流しきい値default 0x8 (4bit) (375*val+375[mA])
void L6470_setparam_stallth(long val); //[R, WR]失速電流しきい値？default 0x40 (7bit) (31.25*val+31.25[mA])
void L6470_setparam_stepmood(long val); //[R, WH]ステップモードdefault 0x07 (1+3+1+3bit)
void L6470_setparam_alareen(long val); //[R, WS]有効アラームdefault 0xff (1+1+1+1+1+1+1+1bit)
void L6470_setparam_config(long val); //[R, WH]各種設定default 0x2e88 (3+3+2+1+1+1+1+1+3bit)
 //L6470_status //[R]状態read onry (16bit)

// [R]:読み取り専用
// [WR]:いつでも書き換え可
// [WH]:書き込みは出力がハイインピーダンスの時のみ可
// [WS]:書き換えはモータが停止している時のみ可
 
 
// レジスタ読み込みコマンド(返り値long型)
long L6470_getparam_abspos();
long L6470_getparam_elpos();
long L6470_getparam_mark();
long L6470_getparam_speed();
long L6470_getparam_acc();
long L6470_getparam_dec();
long L6470_getparam_maxspeed();
long L6470_getparam_minspeed();
long L6470_getparam_fsspd();
long L6470_getparam_kvalhold();
long L6470_getparam_kvalrun();
long L6470_getparam_kvalacc();
long L6470_getparam_kvaldec();
long L6470_getparam_intspd();
long L6470_getparam_stslp();
long L6470_getparam_fnslpacc();
long L6470_getparam_fnslpdec();
long L6470_getparam_ktherm();
long L6470_getparam_adcout();
long L6470_getparam_ocdth();
long L6470_getparam_stallth();
long L6470_getparam_stepmood();
long L6470_getparam_alareen();
long L6470_getparam_config();
long L6470_getparam_status();

#endif  // drv_l6470_h
