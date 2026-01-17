
#include "event.h"
#include "rf433.h"
#include "key_driver.h"
#include "app_main.h"
#include "rf433_app.h"
#include "led_strand_effect.h"
#include "WS2812FX.H"
#include "one_wire.h"
#include "btstack/btstack_typedef.h"
#include "ble_trans_profile.h"
#include "btstack/le/att.h"
// 读取键值有效时间
u8 rf433_valid_t;

void rf433_valid_cnt(void)
{
    if(rf433_valid_t < 80)//80ms
    {
        rf433_valid_t += 10;
    }
}

extern void save_user_data_area3(void);

u8 ble_state = 1; //默认开启BLE模块
extern void save_user_data_area3(void);
extern  u16 T0;
extern u16 T1;
extern u8 rf_key_val;
u8 stepmotpor_speed_cnt = 0; 
extern u8 Ble_Addr[6]; //蓝牙地址

u32 RF433_CODE = 0xFFFF;

extern sys_cb_t sys_cb;
extern void fb_motor_period();
extern void fb_motor_state(u8 p);
extern void ls_open(void);
// ad_key_handle
void rf433_handle(struct sys_event *event)
{

 u8 event_type = 0;
    u8 key_value = 0;
uint8_t Send_buffer[50];        //发送缓存.
    T0++;
    // printf("T0 = %d",T0);
    // printf("T1  = %d", T1 );

//短按
    if(T0 > 15 && T1 <= 100)
    {
        key_value = rf_key_get();
        if(key_value == 255) return ;
        rf_key_val = NO_KEY;
        T0 = 0; 
        T1 = 0;


        if(sys_cb.RFAddrCode1 == 0xFFFF && RF433_CODE == 0xFFFF)  //判断客户码
        {

            if(key_value == RFKEY_ON )
            {


               ls_open();   //软开灯

            }

            if(key_value == RFKEY_SPEED_PLUS ||key_value == RFKEY_SPEED_SUB || key_value == RFKEY_MODE_ADD || key_value == RFKEY_ON_OFF )
            {

                printf("no key");
            }


            if( get_on_off_state() )
            {
                if(key_value == RFKEY_LIGHT_PLUS)
                {

                    extern void bright_plus(void);
                    extern void speed_fast(void);

                    if(fc_effect.Now_state == IS_STATIC)
                    {
                        bright_plus();
                    }
                    if(fc_effect.Now_state == IS_light_scene ||fc_effect.Now_state == ACT_CUSTOM )
                    {
                        speed_fast();
                    }
                }
                else if(key_value == RFKEY_LIGHT_SUB)
                {

                    extern void bright_sub(void);
                    extern void speed_slow(void);

                    if(fc_effect.Now_state == IS_STATIC)
                    {
                        bright_sub();
                    }
                    if(fc_effect.Now_state == IS_light_scene || fc_effect.Now_state == ACT_CUSTOM)
                    {
                        speed_slow();
                    }
                }
                else if(key_value == MOTOR_ON)   //电机
                {
                    extern void one_wire_set_mode(u8 m);
                    extern fb_motor_open_state(void);
                    one_wire_set_mode(4); //配置模式 360转
                    // os_time_dly(1);
                    enable_one_wire();  //使用发送数据
                    save_user_data_area3();
                fb_motor_state(1);
                    // memcpy(Send_buffer,Ble_Addr, 6);
                    // Send_buffer[6] = 0x2F;
                    // Send_buffer[7] = 0x08;
                    // Send_buffer[8] = 0x01;
                    // app_send_user_data(ATT_CHARACTERISTIC_fff1_01_VALUE_HANDLE, Send_buffer,9, ATT_OP_AUTO_READ_CCC);
                    fc_effect.base_ins.motor_on_off = 1;
                }
                else if(key_value == MOTOR_OFF)
                {
                    extern void one_wire_set_mode(u8 m);
                    extern void fb_motor_close_state(void);
                    one_wire_set_mode(6); //配置模式 停止
                    // os_time_dly(1);
                    enable_one_wire();  //使用发送数据
                    save_user_data_area3();
                     fb_motor_state(0);
                    // memcpy(Send_buffer,Ble_Addr, 6);
                    // Send_buffer[6] = 0x2F;
                    // Send_buffer[7] = 0x08;
                    // Send_buffer[8] = 0x00;
                    // app_send_user_data(ATT_CHARACTERISTIC_fff1_01_VALUE_HANDLE, Send_buffer,9, ATT_OP_AUTO_READ_CCC);
                    fc_effect.base_ins.motor_on_off = 0;
                }
                else if(key_value == MOTOR_SPEED_PUL)
                {

                    extern void one_wire_set_period(u8 p);
                
                    if(stepmotpor_speed_cnt > 0)
                    {
                        stepmotpor_speed_cnt--; 
                    }
                    one_wire_set_period(period[stepmotpor_speed_cnt]);
                    // os_time_dly(1);
                    enable_one_wire();
                    save_user_data_area3();
fb_motor_period();
                    // memcpy(Send_buffer,Ble_Addr, 6);
                    // Send_buffer[6] = 0x2F;
                    // Send_buffer[7] = 0x07;
                    // Send_buffer[8] = fc_effect.base_ins.period;
                    // app_send_user_data(ATT_CHARACTERISTIC_fff1_01_VALUE_HANDLE, Send_buffer,9, ATT_OP_AUTO_READ_CCC);
                

                }
                else if(key_value == MOTOR_SPEED_SUB)
                {
    

                    extern void one_wire_set_period(u8 p);
                
                    if(stepmotpor_speed_cnt < 4)
                    {
                        stepmotpor_speed_cnt++; 
                    }
                    one_wire_set_period(period[stepmotpor_speed_cnt]);
                    // os_time_dly(1);
                    enable_one_wire();
                    save_user_data_area3();
                    // memcpy(Send_buffer,Ble_Addr, 6);
                    // Send_buffer[6] = 0x2F;
                    // Send_buffer[7] = 0x07;
                    // Send_buffer[8] = fc_effect.base_ins.period;
                    // app_send_user_data(ATT_CHARACTERISTIC_fff1_01_VALUE_HANDLE, Send_buffer,9, ATT_OP_AUTO_READ_CCC);
                    fb_motor_period();
                }
                else
                {
                    
                    switch(key_value)
                    {
                        case RFKEY_OFF:
                            soft_rurn_off_lights();     //软关灯
                        break;



                        case RFKEY_R:
                            fc_static_effect(0);

                            break;
                        case RFKEY_G:
                            fc_static_effect(1);

                            break;
                        case RFKEY_B:
                            fc_static_effect(2);

                            break;
                        case RFKEY_YELLOW:
                            fc_static_effect(4);

                            break;
                        case RFKEY_CYAN:
                            fc_static_effect(5);

                            break;
                        case RFKEY_PURPLE:
                            fc_static_effect(9);

                            break;
                        case RFKEY_ORANGE:
                            fc_static_effect(8);

                            break;
                        case RFKEY_W:
                            fc_static_effect(3);

                            break;
                        case RFKEY_JUMP3:
                            ls_set_color(0, BLUE);
                            ls_set_color(1, GREEN);
                            ls_set_color(2, RED);
                            fc_effect.dream_scene.change_type = MODE_JUMP;
                            fc_effect.dream_scene.c_n = 3;
                            fc_effect.Now_state = IS_light_scene;
                            set_fc_effect();
                            break;
                        case RFKEY_JUMP7:
                            ls_set_color(0, BLUE);
                            ls_set_color(1, GREEN);
                            ls_set_color(2, RED);
                            ls_set_color(3, WHITE);
                            ls_set_color(4, YELLOW);
                            ls_set_color(5, CYAN);
                            ls_set_color(6, PURPLE);
                            fc_effect.dream_scene.change_type = MODE_JUMP;
                            fc_effect.dream_scene.c_n = 7;
                            fc_effect.Now_state = IS_light_scene;
                            set_fc_effect();
                            break;
                        case RFKEY_FADE3:
                            printf("\n RFKEY_FADE3");
                            extern void change_breath_mode(void);

                            change_breath_mode();
                            break;
                        case RFKEY_FADE7:
                            // ls_set_color(0, BLUE);
                            // ls_set_color(1, GREEN);
                            // ls_set_color(2, RED);
                            // ls_set_color(3, WHITE);
                            // ls_set_color(4, YELLOW);
                            // ls_set_color(5, CYAN);
                            // ls_set_color(6, PURPLE);
                            fc_effect.dream_scene.change_type = MODE_GRADUAL;
                            // fc_effect.dream_scene.c_n = 7;
                            fc_effect.Now_state = IS_light_scene;
                            set_fc_effect();
                            break;
                        case RFKEY_MUSIC1:
                            fc_effect.Now_state = IS_light_music;
                            fc_effect.music.m = 0;
                            set_fc_effect();

                            break;
                        case RFKEY_MUSIC2:
                            fc_effect.Now_state = IS_light_music;
                            fc_effect.music.m = 1;
                            set_fc_effect();
                            break;
                        case RFKEY_MUSIC3:
                            fc_effect.Now_state = IS_light_music;
                            fc_effect.music.m = 2;
                            set_fc_effect();
                            break;
                        case RFKEY_MUSIC4:
                            fc_effect.Now_state = IS_light_music;
                            fc_effect.music.m = 3;
                            set_fc_effect();
                            break;

                    }//switch
                    save_user_data_area3();
                }

            }//if(event_type == KEY_EVENT_CLICK)

        }  //判断客户码




    }
    else  if(T0 > 15)  //松手
    {
        T1 = 0;
        key_value = rf_key_get();
        rf_key_val = NO_KEY;  //
        printf("clead.......................");
    }
//长按
    if(T1 > 100)  // 100*50
    {

     
        key_value = rf_key_get();

        if(key_value == 255) return ;
        rf_key_val = NO_KEY;
        T0 = 0; 


        if(sys_cb.RFAddrCode1 == 0xFFFF && RF433_CODE == 0xFFFF)  //判断客户码 原本遥控
        {
            if(key_value == RFKEY_SPEED_PLUS)
            {
                printf("open ble");
                ble_state =1;
                bt_ble_init();
               
            }
            else if(key_value == RFKEY_SPEED_SUB)
            {
                //printf(" LONG  RFKEY_SPEED_SUB");
                printf("close ble");
                bt_ble_exit();
                ble_state = 0;
               
            }
    
            save_user_data_area3();
        }


    
    }






























#if 0
    u8 event_type = 0;
    u8 key_value = 0;
    rf433_valid_cnt();
    key_value = rf_key_get();
   // printf("key_value = %d", key_value);
    if(key_value == 255) return;
    // 时间无效，返回
    if(rf433_valid_t < 80)
    {
        rf433_valid_t = 0;
        return;
    }
    rf433_valid_t = 0;
    
    event_type = KEY_EVENT_CLICK;

    // event_type = event->u.key.event;
    // key_value = event->u.key.value;

    
    printf("\n rf433_handle");
    // if(event->u.key.type == KEY_DRIVER_TYPE_RF433)
    {
        printf("\n KEY_DRIVER_TYPE_RF433");
        if(key_value == RFKEY_ON)
        {
            soft_turn_on_the_light();   //软开灯

        }

        if(key_value != RFKEY_ON &&
            key_value != RFKEY_OFF &&
            key_value != RFKEY_LIGHT_PLUS &&
            key_value != RFKEY_LIGHT_SUB &&
            key_value != RFKEY_MUSIC1 &&
            key_value != RFKEY_MUSIC2 &&
            key_value != RFKEY_MUSIC3 &&
            key_value != RFKEY_MUSIC4 &&
            key_value != 128 &&     //其他功能按键
            key_value != 64 &&
            key_value != 192 &&
            key_value != 32 &&
            key_value != 152 &&
            key_value != 88 &&
            key_value != 216 &&
            key_value != 56


        )
        {
            // dream_mic.mic_switch = OFF;

        }
 

        if(event_type == KEY_EVENT_CLICK && get_on_off_state() )
        {
            if(key_value == RFKEY_LIGHT_PLUS)
            {

                extern void bright_plus(void);
                extern void speed_fast(void);

                if(fc_effect.Now_state == IS_STATIC)
                {
                    bright_plus();
                }
                if(fc_effect.Now_state == IS_light_scene)
                {
                    speed_fast();
                }
            }
            else if(key_value == RFKEY_LIGHT_SUB)
            {

                extern void bright_sub(void);
                extern void speed_slow(void);

                if(fc_effect.Now_state == IS_STATIC)
                {
                    bright_sub();
                }
                if(fc_effect.Now_state == IS_light_scene)
                {
                    speed_slow();
                }
            }
            else
            {
                
                switch(key_value)
                {
                    case RFKEY_OFF:
                        soft_rurn_off_lights();     //软关灯
                    break;



                    case RFKEY_R:
                        fc_static_effect(0);

                        break;
                    case RFKEY_G:
                        fc_static_effect(1);

                        break;
                    case RFKEY_B:
                        fc_static_effect(2);

                        break;
                    case RFKEY_YELLOW:
                        fc_static_effect(4);

                        break;
                    case RFKEY_CYAN:
                        fc_static_effect(5);

                        break;
                    case RFKEY_PURPLE:
                        fc_static_effect(9);

                        break;
                    case RFKEY_ORANGE:
                        fc_static_effect(8);

                        break;
                    case RFKEY_W:
                        fc_static_effect(3);

                        break;
                    case RFKEY_JUMP3:
                        ls_set_color(0, BLUE);
                        ls_set_color(1, GREEN);
                        ls_set_color(2, RED);
                        fc_effect.dream_scene.change_type = MODE_JUMP;
                        fc_effect.dream_scene.c_n = 3;
                        fc_effect.Now_state = IS_light_scene;
                        set_fc_effect();
                        break;
                    case RFKEY_JUMP7:
                        ls_set_color(0, BLUE);
                        ls_set_color(1, GREEN);
                        ls_set_color(2, RED);
                        ls_set_color(3, WHITE);
                        ls_set_color(4, YELLOW);
                        ls_set_color(5, CYAN);
                        ls_set_color(6, PURPLE);
                        fc_effect.dream_scene.change_type = MODE_JUMP;
                        fc_effect.dream_scene.c_n = 7;
                        fc_effect.Now_state = IS_light_scene;
                        set_fc_effect();
                        break;
                    case RFKEY_FADE3:
                        printf("\n RFKEY_FADE3");
                        extern void change_breath_mode(void);

                        change_breath_mode();
                        break;
                    case RFKEY_FADE7:
                        // ls_set_color(0, BLUE);
                        // ls_set_color(1, GREEN);
                        // ls_set_color(2, RED);
                        // ls_set_color(3, WHITE);
                        // ls_set_color(4, YELLOW);
                        // ls_set_color(5, CYAN);
                        // ls_set_color(6, PURPLE);
                        fc_effect.dream_scene.change_type = MODE_GRADUAL;
                        // fc_effect.dream_scene.c_n = 7;
                        fc_effect.Now_state = IS_light_scene;
                        set_fc_effect();
                        break;
                    case RFKEY_MUSIC1:
                        fc_effect.Now_state = IS_light_music;
                        fc_effect.music.m = 0;
                        set_fc_effect();

                        break;
                    case RFKEY_MUSIC2:
                        fc_effect.Now_state = IS_light_music;
                        fc_effect.music.m = 1;
                        set_fc_effect();
                        break;
                    case RFKEY_MUSIC3:
                        fc_effect.Now_state = IS_light_music;
                        fc_effect.music.m = 2;
                        set_fc_effect();
                        break;
                    case RFKEY_MUSIC4:
                        fc_effect.Now_state = IS_light_music;
                        fc_effect.music.m = 3;
                        set_fc_effect();
                        break;

                }//switch
                save_user_data_area3();
            }

        }//if(event_type == KEY_EVENT_CLICK)

    }//if(event->u.key.type == KEY_DRIVER_TYPE_AD)
#endif 
}//end


