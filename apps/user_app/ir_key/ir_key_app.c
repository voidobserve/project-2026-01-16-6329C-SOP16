#include "system/includes.h"
#include "led_strip_sys.h"
#include "ir_key_app.h"
#include "task.h"
#include "event.h"
#include "my_effect.h"

#if TCFG_IRKEY_ENABLE

// @key_app.c
// 实现以下功能APP
// 物理按键
// AD按键
// 红外遥控
OS_SEM ir_task_sem;                     //遥控任务信号量

/***********************************************************移植须修改****************************************************************/
// 遥控一共10个自动mode
#define IR_MODE_QTY 10
/* 定义声控模式数量 */
#define IR_MIC_QTY  4
// 输入ir_scan_mode，用于遥控自动模式，模式+，模式-
const u8 ir_mode[IR_MODE_QTY] = {0,1,4,5,6,15,12,7,8,9};
/* 作为light_mode_change()输入形参 */
// 数组元素：MIC_ROLL_CMD ，MIC_ERENGY_CMD，MIC_SPECTRUM_CMD，MIC_ROCK_CMD
const u8 ir_mic_mode[IR_MIC_QTY] = {MODE_MIC_ROLL, MODE_MIC_ERENGY, MODE_MIC_SPECTRUM, MODE_MIC_ROCK};
/***********************************************************END****************************************************************/


//  计数器，遥控自动模式，模式+,-计数
u8 ir_mode_cnt;
ir_auto_e ir_auto_f = IR_PAUSE;
// 自动模式下，改变模式计数器
u16 ir_auto_change_tcnt = 0;
/* 定时器状态 */
ir_timer_e ir_timer_state ;
/* 定时计数器,累减,单位S */
u16 ir_time_cnt;
/* 本地声控模式index */
u8 ir_local_mic_index = 0;
/* 灵敏度：1-100 */
u8 ir_sensitive = 0;
/***********************************************************API*******************************************************************/
/* 定时器周期1秒，单位1ms */
#define IR_TIMER_UNIT   1000
#define IR_CHANGE_MODE_T    (10)     //10秒换一个模式


//--------------------------------------------------自动
/* 设置自动开与关 */
void set_ir_auto(ir_auto_e auto_f)
{
    /* 检测合法性，和有变化 */
    if(auto_f <= IR_PAUSE && ir_auto_f != auto_f)
    {
        ir_auto_f = auto_f;
        #ifdef MY_DEBUG
        printf("\r set_ir_auto OK");
        #endif
        os_sem_post(&ir_task_sem);
    }
    else
    {
        // ir_auto_f = IR_PAUSE;
    }
}

/* 获取自动状态 */
ir_auto_e get_ir_auto(void)
{
    return ir_auto_f;
}

//-------------------------------------------------效果

// 返回遥控器当前操作的模式
u8 get_ir_mode(void)
{
    return ir_mode[ir_mode_cnt];
}
// 每次调用，循环递增模式，到IR_MODE_QTY从0开始
void ir_mode_plus(void)
{
    ir_mode_cnt++;
    ir_mode_cnt %= IR_MODE_QTY;
}

// 每次调用，循环递减模式，到0从IR_MODE_QTY开始
void  ir_mode_sub(void)
{
    if(ir_mode_cnt>0)
    {
        ir_mode_cnt--;
    }
    else
    {
        ir_mode_cnt = IR_MODE_QTY;
    }
}
// -----------------------------------------------遥控开启定时功能

/* 设置定时关机，按OFF取消 */
void set_ir_timer(ir_timer_e timer)
{
    /* 检测合法性，和有变化 */
    if(timer <= IR_TIMER_120MIN && ir_timer_state != timer)
    {
        ir_timer_state = timer;
        ir_time_cnt = timer / IR_TIMER_UNIT;
        os_sem_post(&ir_task_sem);
    }
}

ir_timer_e get_ir_timer(void)
{
    return ir_timer_state;
}
// -----------------------------------------------声控
void ir_mic_mode_plus(void)
{
    ir_local_mic_index++;
    ir_local_mic_index %= IR_MIC_QTY;
}

void ir_mic_mode_sub(void)
{
    if(ir_local_mic_index > 0)
    {
        ir_local_mic_index--;
    }
    else
    {
        ir_local_mic_index = IR_MIC_QTY -1;
    }
}

/* 返回ir_mic_mode[]下标 */
u8 get_ir_mic_mode_index(void)
{
    return ir_local_mic_index;
}

u8 get_ir_mic_mode(void)
{
    return ir_mic_mode[ir_local_mic_index];
}

// -----------------------------------------------灵敏度
void ir_sen_plus(void)
{
    ir_sensitive+=10;
    if(ir_sensitive > 100)
    {
        ir_sensitive = 100;
    }
}

void ir_sen_sub(void)
{
    if(ir_sensitive >= 10)
    {
        ir_sensitive -=10;
    }
}

void set_ir_sen(u8 sen)
{
    if(sen > 100)
    {
        sen = 100;
    }
    ir_sensitive = sen;
}

u8 get_ir_sen(void)
{
    return ir_sensitive;
}





/***********************************************************APP*******************************************************************/
extern void adkey_mode_change(u8 light_mode);
static void ir_auto_change_mode(void)
{
    u32 err;
    // 循环模式
    if(ir_auto_f == IR_AUTO)
    {

        if(ir_auto_change_tcnt!=0)
        {

            ir_auto_change_tcnt--;
        }

        if(ir_auto_change_tcnt == 0)
        {
            ir_auto_change_tcnt = IR_CHANGE_MODE_T;
            effect_plus();
            switch_effect(get_effect_index());
        }
    }
}

static void ir_timer_handle(void)
{
    if(ir_timer_state != IR_TIMER_NO)
    {
        if(ir_time_cnt > 0)
        {
            ir_time_cnt--;
            #ifdef MY_DEBUG
            printf("\r ir_time_cnt--");
            #endif
        }
        else
        {
            /* 关灯 */
            set_ir_timer(IR_TIMER_NO);
            soft_rurn_off_lights();
        }
    }
}

extern void effect_speed_plus(void);
extern void effect_speed_sub(void);
void ir_key_handle(struct sys_event *event)
{
    u8 event_type = 0;
    u8 key_value = 0;
    event_type = event->u.key.event;
    key_value = event->u.key.value;
    #ifdef MY_DEBUG
        // printf("\n event->u.key.type = %d",event->u.key.type);
    #endif
    if(event->u.key.type == KEY_DRIVER_TYPE_IR)
    {


    }//if(event->u.key.type == KEY_DRIVER_TYPE_IR)
}

/***********************************************************AD_KEY*******************************************************************/

void ad_key_event_handle(u8 key_state, u8 key_value);

/***********************************************************key_event_task_handle*******************************************************************/

void key_event_task_handle(void *p)
{
    u32 err;
    while (1)
    {
        // 每“IR_TIMER_UNIT”唤醒一次任务
        err = os_sem_pend(&ir_task_sem, IR_TIMER_UNIT/10);

        ir_auto_change_mode();
        ir_timer_handle();

    }
}


#endif
