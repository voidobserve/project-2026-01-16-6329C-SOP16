#ifndef __ONE_WIRE_H__
#define __ONE_WIRE_H__

#include "system/includes.h"

// 驱动电机ic的控制脚：
#define MOTOR_CTL_PIN IO_PORTB_05

typedef struct
{
    // 000:回正
    // 001:区域1摇摆
    // 010:区域2摇摆
    // 011:区域1和区域2摇摆
    // 100:360°正转
    // 101:音乐律动
    u8 mode;           //电机模式 
    u8 period;          //000:  8S; 001:  13S; 010:  18S ;011:  21S ;100:  26S  //转速  
    u8 dir;            //1:反转 0:正转  仅音乐律动模式有效
    u8 music_mode;     //音乐律动下的转动模式
    u8 motor_on_off;   //0:关闭电机  1：开启电机
}base_ins_t;


extern u8 period[6];



#endif



