#ifndef key_app_h
#define key_app_h

#include "board_ac632n_demo_cfg.h"
// @ key_app.h
#if TCFG_IRKEY_ENABLE
#include "led_strip_sys.h"

// ----------------------------------------------硬件配置，根据PCB实际修改
// 到@board_ac632n_demo_cfg.h 完成板级配置
#define IR_PIN      IO_PORTA_00



// ---------------------------------------------定义IR_KEY键值
//#define IRKEY_PLAY      0x25
//#define IRKEY_AUTO      0x50        //自动
//#define IRKEY_PAUSE     0x51        //暂停
//#define IRKEY_ON        0x40        //开机
//#define IRKEY_OFF       0x41        //关机
//#define IRKEY_LIGHT_ADD 0x5c        //亮度+
//#define IRKEY_QUICK     0x4d        //速度加
//#define IRKEY_MODE_ADD  0x1e        //效果+
//#define IRKEY_LIGHT_DEC 0x5d        //亮度-
//#define IRKEY_SLOW      0x4c        //减速
//#define IRKEY_MODE_DEC  0x1f        //效果-
//#define IRKEY_R         0x58        //r
//#define IRKEY_G         0x59        //g
//#define IRKEY_B         0x45        //b
//#define IRKEY_YELLOW    0x54        //yellow
//#define IRKEY_CYAN      0x49        //青色
//#define IRKEY_PARPLE    0x48        //紫色
//#define IRKEY_ORANGE    0x55        //橙色
//#define IRKEY_W         0x44        //w
//#define IRKEY_MODEL1    0x1c        //mode1
//#define IRKEY_MODEL2    0x1d        //mode2
//#define IRKEY_MODEL3    0x18        //mode3
//#define IRKEY_MODEL4    0x19        //mode4
//#define IRKEY_MODEL5    0x1a        //mode5
//#define IRKEY_MODEL6    0x1b        //mode6
//#define IRKEY_MODEL7    0x14        //mode7
//#define IRKEY_MODEL8    0x15        //mode8
//#define IRKEY_MODEL9    0x16        //mode9
//#define IRKEY_MODEL10   0x17        //mode10
//#define IRKEY_MUSIC1    0x10        //音乐模式1
//#define IRKEY_MUSIC2    0x11        //音乐模式2
//#define IRKEY_MUSIC3    0x0c        //音乐模式3
//#define IRKEY_MUSIC4    0x0d        //音乐模式4
//#define IRKEY_MUSIC_ADD 0x12        //音乐模式+
//#define IRKEY_MUSIC_DEC 0x13        //音乐模式-
//#define IRKEY_SENSITIVE_ADD 0x0e    //灵敏度+
//#define IRKEY_SENSITIVE_DEC 0x0F    //灵敏度-
//#define IRKEY_30_MIN    0x08        //30分钟
//#define IRKEY_90_MIN    0x0a        //90分钟
//#define IRKEY_2H        0x0b
//#define IRKEY_1H        0x09
// ---------------------------------------------定义响益IR_KEY键值
#define  IRKEY_ON       	  0x45     //开机
#define  IRKEY_AUTO     	  0x46     //自动
#define  IRKEY_OFF            0x47     //关机
#define  IRKEY_SPEED_ADD      0x44     //速度+
#define  IRKEY_MODE_ADD       0x40     //模式+
#define  IRKEY_LIGHT_ADD	  0x43     //亮度+
#define  IRKEY_SPEED_DEC      0x07     //速度-
#define  IRKEY_MODE_DEC       0x15     //模式-
#define  IRKEY_LIGHT_DEC	  0x09     //亮度-
#define  IRKEY_RED	          0x16     //红色
#define  IRKEY_GREEN1	      0x19     //深绿色
#define  IRKEY_BLUE1	      0x0d     //深蓝色
#define  IRKEY_YELLOW1        0x0c     //黄色
#define  IRKEY_YELLOW2	      0x18     //深黄色
#define  IRKEY_ORANGE1	      0x5e     //橙色
#define  IRKEY_ORANGE2	      0x08     //浅橙色
#define  IRKEY_BLUE2	      0x1c     //浅蓝色
#define  IRKEY_BLUE3	      0x5a     //蓝色
#define  IRKEY_PINK	          0x42     //粉红色
#define  IRKEY_GREEN2	      0x52     //绿色
#define  IRKEY_WHITE	      0x4a     //白色

#define IRKEY_SNOW      0x03
#define IRKEY_HEART     0x04
#define IRKEY_METEOR    0x05


typedef enum
{
    IR_AUTO,
    IR_PAUSE,
} ir_auto_e;

typedef enum
{
    IR_TIMER_NO = 0,        //无定时
    IR_TIMER_30MIN = 30*60*1000,
    IR_TIMER_60MIN = 60*60*1000,
    IR_TIMER_90MIN = 90*60*1000,
    IR_TIMER_120MIN = 120*60*1000,
}ir_timer_e;



//--------------------------------------------------自动
/* 设置自动开与关 */
void set_ir_auto(ir_auto_e auto_f);
/* 获取自动状态 */
ir_auto_e get_ir_auto(void);

//-------------------------------------------------效果
// 返回遥控器当前操作的模式
u8 get_ir_mode(void);
// 每次调用，循环递增模式，到IR_MODE_QTY从0开始
void ir_mode_plus(void);
// 每次调用，循环递减模式，到0从IR_MODE_QTY开始
void  ir_mode_sub(void);

// -----------------------------------------------遥控开启定时功能
/* 设置定时关机，按OFF取消 */
void set_ir_timer(ir_timer_e timer);
ir_timer_e get_ir_timer(void);
// -----------------------------------------------声控
void ir_mic_mode_plus(void);
void ir_mic_mode_sub(void);
// -----------------------------------------------灵敏度
void ir_sen_plus(void);
void ir_sen_sub(void);
void set_ir_sen(u8 sen);
u8 get_ir_sen(void);

/***********************************************************APP*******************************************************************/
void switch_ir_tack_sem(void);
void key_event_task_handle(void *p);
void ir_key_handle(struct sys_event *event);
void tuya_data_init();

#endif

#endif

