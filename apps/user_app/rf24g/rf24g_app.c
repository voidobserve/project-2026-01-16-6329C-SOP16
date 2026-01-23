/*
适用用2.4G遥控
基于中道版本2.4G遥控
1、app_config.h,把宏CONFIG_BT_GATT_CLIENT_NUM设置1
2、@bt_ble_init() 加入multi_client_init()
3、@le_gatt_client.c
   __resolve_adv_report（）
   HCI_EIR_DATATYPE_MORE_16BIT_SERVICE_UUIDS 加入键值处理函数
4、在key_driver.c 注册rf24g_scan_para
5、board_ac632n_demo_cfg.h 使能TCFG_RF24GKEY_ENABLE
6、@app_tuya.c tuya_key_event_handler()加入上层应用的键值处理函数
7、底层无法判断长按，需要靠上层应用实现
 */

#include "system/includes.h"

#include "task.h"
#include "event.h"
#include "rf24g.h"
#include "led_strip_sys.h"
#include "board_ac632n_demo_cfg.h"
#include "tuya_ble_type.h"
#include "led_strand_effect.h"
#include "rf24g_app.h"

#include "one_wire.h"

// #if TCFG_RF24GKEY_ENABLE
#if 1
// extern rf24g_ins_t rf24g_ins;
// #define PAIR_MAX 2

// #pragma pack(1)
// typedef struct
// {
//     u8 pair[3];
//     u8 flag; // 0:表示该数组没使用，0xAA：表示改数组已配对使用
// } rf24g_pair_t;

// #pragma pack()

// rf24g_pair_t rf24g_pair[PAIR_MAX]; // 需要写flash
/***********************************************************移植须修改****************************************************************/

// #define PAIR_TIME_OUT 5 * 1000 // 3秒
// static u16 pair_tc = 0;

// // 配对计时，10ms计数一次
// void rf24g_pair_tc(void)
// {
//     if (pair_tc <= PAIR_TIME_OUT)
//     {
//         pair_tc += 10;
//     }
// }

/***********************************************************移植须修改 END****************************************************************/

/***********************************************************API*******************************************************************/
// #define CFG_USER_PAIR_DATA 4 // 保存2.4G遥控客户码
// void save_rf24g_pair_data(void)
// {
//     syscfg_write(CFG_USER_PAIR_DATA, (u8 *)(&rf24g_pair[0]), sizeof(rf24g_pair_t));
// }

// void read_rf24g_pair_data(void)
// {
//     syscfg_read(CFG_USER_PAIR_DATA, (u8 *)(&rf24g_pair[0]), sizeof(rf24g_pair_t));
//     printf_buf((u8 *)(&rf24g_pair[0]), sizeof(rf24g_pair_t));
// }

//-------------------------------------------------效果

// -----------------------------------------------声控

// -----------------------------------------------灵敏度

/***********************************************************APP*******************************************************************/

// pair_handle是长按执行，长按时会被执行多次
// 所以执行一次后，要把pair_tc = PAIR_TIME_OUT，避免误触发2次
// extern void ls_lenght_add(u8 l);
// extern void ls_lenght_sub(u8 l);

// static void pair_handle(void)
// {
//     extern void save_rf24g_pair_data(void);
//     u8 op = 0; // 1:配对，2：解码
//     u8 i;

//     // 开机3秒内
//     if (pair_tc < PAIR_TIME_OUT)
//     {
//         // printf("\n pair_tc=%d",pair_tc);
//         pair_tc = PAIR_TIME_OUT; // 避免误触发2次
//         memcpy((u8 *)(&rf24g_pair[0].pair), (u8 *)(&rf24g_ins.pair), 3);
//         rf24g_pair[0].flag = 0xaa;
//         save_rf24g_pair_data();
//         // printf("\n pair");
//         // printf_buf(&rf24g_pair[0].pair, 3);
//         extern void fc_24g_pair_effect(void); // 配对效果
//         fc_24g_pair_effect();
//         // 查找表是否存在
//     }
// }

// extern u8 ble_state;

// u8 off_long_cnt = 0;
// extern u8 Ble_Addr[6];
// extern u8 auto_set_led_num_f;
// u8 pexil_switch = 1;

// u8 yaokong_or_banzai = 0;
// uint16_t need_to_set_num;
// extern void set_ls_lenght(u16 l);

// #define _3V_12jian_head1 0x34
// #define _3V_12jian_head2 0x12

// #define _12V_18jian_head1 0x01
// #define _12V_18jian_head2 0x23

extern u16 rf24_T0;
extern u16 rf24_T1;
extern u8 last_key_v;
u8 key_value = 0;

u8 long_press_f = 0;
u8 long_press_cnt = 0;

extern void ls_open(void);
void rf24_key_handle(void)
{
    if (rf24_T0 < 0xFFFF)
        rf24_T0++;
    if (rf24_T0 > 15 && rf24_T1 < 1)
    {
        key_value = last_key_v;
        if (key_value == NO_KEY)
            return;
        last_key_v = NO_KEY;
        rf24_T0 = 0;
        rf24_T1 = 0;
        // printf("key_value = %u\n", (u16)key_value);

        /*CODE*/

        if (key_value == RF24_K07)
        {
            soft_turn_on_the_light();
        }
        else if (key_value == RF24_K08)
        {
            // 关灯，同时关电机（电机和七彩灯是装在一起的）
            soft_rurn_off_lights(); // 软关灯
        }

        if (get_on_off_state() == DEVICE_OFF)
        {
            return;
        }

        if (key_value == RF24_K05)
        {
            /*
                静态模式下，增加 亮度
                动态模式下，增加 速度
                声控模式下，增加 灵敏度（USER_TO_DO 原本并没有调节灵敏度的功能，还不知道要不要加）
            */

            extern void bright_plus(void);
            extern void speed_fast(void);

            if (fc_effect.Now_state == IS_STATIC)
            {
                bright_plus();
                fb_led_bright_state();
            }
            else if (fc_effect.Now_state == IS_light_scene || fc_effect.Now_state == ACT_CUSTOM)
            {
                speed_fast();
                fb_led_speed_state();
                set_fc_effect();
            }
            else if (fc_effect.Now_state == IS_light_music)
            {

            }
        }
        else if (key_value == RF24_K06)
        {
            /*
                静态模式下，减小 亮度
                动态模式下，减小 速度
                声控模式下，减小 灵敏度（USER_TO_DO 原本并没有调节灵敏度的功能，还不知道要不要加）
            */
            extern void bright_sub(void);
            extern void speed_slow(void);

            if (fc_effect.Now_state == IS_STATIC)
            {
                bright_sub();
                fb_led_bright_state();
            }
            else if (fc_effect.Now_state == IS_light_scene || fc_effect.Now_state == ACT_CUSTOM)
            {
                speed_slow();
                fb_led_speed_state();
                set_fc_effect();
            }
            else if (fc_effect.Now_state == IS_light_music)
            {

            }
        }
        else if (key_value == RF24_K25) // 电机
        {
            // 打开电机
            fc_effect.base_ins.motor_on_off = DEVICE_ON;
            if (6 == fc_effect.base_ins.mode)
            {
                // 如果原来的电机模式是关闭的，改成模式4：360度旋转
                fc_effect.base_ins.mode = 4;
            }

            printf("base_ins.mode = %u\n", (u16)fc_effect.base_ins.mode);
            printf("motor period == %u\n", (u16)fc_effect.base_ins.period);
            os_taskq_post("msg_task", 1, MSG_SEQUENCER_ONE_WIRE_SEND_INFO);
            os_taskq_post("msg_task", 1, MSG_USER_SAVE_INFO);
            fb_motor_state(1);
        }
        else if (key_value == RF24_K26)
        {
            // 关闭电机
            fc_effect.base_ins.motor_on_off = DEVICE_OFF;
            one_wire_set_mode(6); // 停止电机
            printf("motor period == %u\n", (u16)fc_effect.base_ins.period);

            os_taskq_post("msg_task", 1, MSG_SEQUENCER_ONE_WIRE_SEND_INFO);
            os_taskq_post("msg_task", 1, MSG_USER_SAVE_INFO);
            fb_motor_state(0);
        }
        else if (key_value == RF24_K27)
        {
            // 电机速度加
            if (fc_effect.base_ins.motor_on_off == DEVICE_ON)
            {
                // 速度值索引越小,速度越快
                if (fc_effect.base_ins.period_index > 0)
                {
                    fc_effect.base_ins.period_index--;
                }

                one_wire_set_period(motor_period[fc_effect.base_ins.period_index]);
                os_taskq_post("msg_task", 1, MSG_SEQUENCER_ONE_WIRE_SEND_INFO);
                os_taskq_post("msg_task", 1, MSG_USER_SAVE_INFO);
                fb_motor_period();
            }
        }
        else if (key_value == RF24_K28)
        {
            // 电机速度减
            if (fc_effect.base_ins.motor_on_off == DEVICE_ON)
            {
                if (fc_effect.base_ins.period_index < ARRAY_SIZE(motor_period) - 1)
                {
                    fc_effect.base_ins.period_index++;
                }

                one_wire_set_period(motor_period[fc_effect.base_ins.period_index]);
                os_taskq_post("msg_task", 1, MSG_SEQUENCER_ONE_WIRE_SEND_INFO);
                os_taskq_post("msg_task", 1, MSG_USER_SAVE_INFO);
                fb_motor_period();
            }
        }
        else
        {
            switch (key_value)
            {
            case RF24_K09:
                colorful_light_set_static_color(RED);
                break;
                // ===============================================================
            case RF24_K10:
                colorful_light_set_static_color(GREEN);
                break;
                // ===============================================================
            case RF24_K11:
                colorful_light_set_static_color(BLUE);
                break;
                // ===============================================================
            case RF24_K12:
                colorful_light_set_static_color(PURE_WHITE);
                break;
                // ===============================================================
            case RF24_K13:
                colorful_light_set_static_color(ORANGE);
                break;
                // ===============================================================
            case RF24_K14:
                colorful_light_set_static_color(YELLOW);
                break;
                // ===============================================================
            case RF24_K15:
                colorful_light_set_static_color(CYAN);
                break;
            // ===============================================================
            case RF24_K16:
                colorful_light_set_static_color(PURPLE);
                break;
                // ===============================================================
            case RF24_K17:
                // 三色跳变
                ls_set_color(0, RED);
                ls_set_color(1, GREEN);
                ls_set_color(2, BLUE);
                fc_effect.dream_scene.change_type = MODE_JUMP;
                fc_effect.dream_scene.c_n = 3;
                fc_effect.Now_state = IS_light_scene;
                set_fc_effect();
                break;
                // ===============================================================
            case RF24_K18:
                // 七色跳变
                ls_set_color(0, RED);
                ls_set_color(1, GREEN);
                ls_set_color(2, BLUE);
                ls_set_color(3, YELLOW);
                ls_set_color(4, CYAN);
                ls_set_color(5, MAGENTA);
                ls_set_color(6, PURE_WHITE);
                fc_effect.dream_scene.change_type = MODE_JUMP;
                fc_effect.dream_scene.c_n = 7;
                fc_effect.Now_state = IS_light_scene;
                set_fc_effect();
                break;
                // ===============================================================
            case RF24_K19:
                // 5种呼吸
                extern void change_breath_mode(void);
                change_breath_mode();
                break;
                // ===============================================================
            case RF24_K20:
                // 七色渐变
                fc_effect.dream_scene.change_type = MODE_GRADUAL;
                fc_effect.Now_state = IS_light_scene;
                set_fc_effect();
                break;
                // ===============================================================
            case RF24_K21:
                fc_effect.Now_state = IS_light_music;
                fc_effect.music.m = 0;
                set_fc_effect();
                break;
                // ===============================================================
            case RF24_K22:
                fc_effect.Now_state = IS_light_music;
                fc_effect.music.m = 1;
                set_fc_effect();
                break;
                // ===============================================================
            case RF24_K23:
                fc_effect.Now_state = IS_light_music;
                fc_effect.music.m = 2;
                set_fc_effect();
                break;
                // ===============================================================
            case RF24_K24:
                fc_effect.Now_state = IS_light_music;
                fc_effect.music.m = 3;
                set_fc_effect();
                break;
                // ===============================================================

            } // switch

            os_taskq_post("msg_task", 1, MSG_USER_SAVE_INFO);

        } // if(event_type == KEY_EVENT_CLICK)
    }
    else if (rf24_T0 > 15)
    {
        rf24_T1 = 0;
        long_press_f = 0;
        long_press_cnt = 0;
        printf("cled\n");
    }

    if (get_rf24g_long_state() == KEY_EVENT_LONG && rf24_T1 > 3) // 长按
    {
        long_press_f = 1;
        key_value = last_key_v;
        last_key_v = NO_KEY;

        // if(key_value == RF24_K03)
        // {
        //     printf("open ble");
        //     ble_state =1;
        //     bt_ble_init();
        //     save_user_data_area3();
        // }
        // else if(key_value == RF24_K04)
        // {
        //     //printf(" LONG  RFKEY_SPEED_SUB");
        //     printf("close ble");
        //     bt_ble_exit();
        //     ble_state = 0;
        //     save_user_data_area3();
        // }
    }
}

void long_press_handle(void)
{
    if ((get_on_off_state() == DEVICE_ON) && long_press_f)
    {
        long_press_cnt++;
    }
}

#endif
