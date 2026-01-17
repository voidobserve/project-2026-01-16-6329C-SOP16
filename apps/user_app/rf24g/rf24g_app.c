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

// #if TCFG_RF24GKEY_ENABLE
#if 1
extern rf24g_ins_t rf24g_ins;
#define PAIR_MAX    2

#pragma pack (1)
typedef struct
{
    u8 pair[3];
    u8 flag;    //0:表示该数组没使用，0xAA：表示改数组已配对使用
}rf24g_pair_t;

#pragma pack ()

rf24g_pair_t rf24g_pair[PAIR_MAX];        //需要写flash
/***********************************************************移植须修改****************************************************************/
extern void save_user_data_area3(void);
#define PAIR_TIME_OUT 5*1000    //3秒
static u16 pair_tc = 0;

// 配对计时，10ms计数一次
void rf24g_pair_tc(void)
{
    if(pair_tc <= PAIR_TIME_OUT)
    {
        pair_tc+=10;
    }
}

/***********************************************************移植须修改 END****************************************************************/



/***********************************************************API*******************************************************************/
#define     CFG_USER_PAIR_DATA          4   //保存2.4G遥控客户码
void save_rf24g_pair_data(void)
{
    syscfg_write(CFG_USER_PAIR_DATA, (u8 *)(&rf24g_pair[0]), sizeof(rf24g_pair_t));
}

void read_rf24g_pair_data(void)
{
    syscfg_read(CFG_USER_PAIR_DATA, (u8 *)(&rf24g_pair[0]), sizeof(rf24g_pair_t));
    printf_buf((u8 *)(&rf24g_pair[0]), sizeof(rf24g_pair_t));
}



//-------------------------------------------------效果



// -----------------------------------------------声控


// -----------------------------------------------灵敏度




/***********************************************************APP*******************************************************************/

// pair_handle是长按执行，长按时会被执行多次
// 所以执行一次后，要把pair_tc = PAIR_TIME_OUT，避免误触发2次
 extern void ls_lenght_add(u8 l);
 extern void ls_lenght_sub(u8 l);

static void pair_handle(void)
{
    extern void save_rf24g_pair_data(void);
    u8 op = 0;//1:配对，2：解码
    u8 i;
    
    // 开机3秒内
    if(pair_tc < PAIR_TIME_OUT)
    {
        // printf("\n pair_tc=%d",pair_tc);
        pair_tc = PAIR_TIME_OUT;//避免误触发2次
        memcpy((u8*)(&rf24g_pair[0].pair), (u8*)(&rf24g_ins.pair), 3);
        rf24g_pair[0].flag = 0xaa;
        save_rf24g_pair_data();
        // printf("\n pair");
        // printf_buf(&rf24g_pair[0].pair, 3);
        extern void fc_24g_pair_effect(void);  //配对效果
        fc_24g_pair_effect();
        // 查找表是否存在
       

    }
}




extern u8 ble_state;

u8 off_long_cnt=0;
extern u8 Ble_Addr[6];
extern u8 auto_set_led_num_f;
 u8 pexil_switch = 1;

u8 yaokong_or_banzai = 0;
uint16_t need_to_set_num ;
 extern void set_ls_lenght(u16 l);


#define _3V_12jian_head1   0x34
#define _3V_12jian_head2   0x12

#define _12V_18jian_head1  0x01
#define _12V_18jian_head2  0x23

extern u16 rf24_T0  ;
extern u16 rf24_T1  ;
extern u8 last_key_v;
u8 key_value = 0;

extern u8 stepmotpor_speed_cnt;
u8 long_press_f = 0;
u8 long_press_cnt = 0;

extern void ls_open(void);
void rf24_key_handle()
{

    if(rf24_T0 < 0xFFFF)
    rf24_T0++;
   //T0 10ms+=
    //    printf("T0 = %d", T0);
        if(rf24_T0 > 15 && rf24_T1 < 1)
        {
            key_value =  last_key_v;
            if(key_value == NO_KEY) return ;
            last_key_v = NO_KEY;
            rf24_T0 = 0;
            rf24_T1 = 0;
            printf(" duanna key_value = %d", key_value);
        
            /*CODE*/

            if(key_value == RF24_K07 )
            {



                ls_open();   //软开灯

            }

            if(key_value == RF24_K01 ||key_value == RF24_K02 || key_value == RF24_K03 || key_value == RF24_K04 )
            {

                printf("no key");
            }


            if( get_on_off_state() )
            {
                if(key_value == RF24_K05)
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
                else if(key_value == RF24_K06)
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
                else if(key_value == RF24_K25)   //电机
                {
                    extern void one_wire_set_mode(u8 m);
                    extern fb_motor_open_state(void);
                    one_wire_set_mode(4); //配置模式 360转
                    // os_time_dly(1);
                    enable_one_wire();  //使用发送数据
                    save_user_data_area3();
                    fb_motor_state(1);
  
                    fc_effect.base_ins.motor_on_off = 1;
                }
                else if(key_value == RF24_K26)
                {
                    extern void one_wire_set_mode(u8 m);
                    extern void fb_motor_close_state(void);
                    one_wire_set_mode(6); //配置模式 停止
                    // os_time_dly(1);
                    enable_one_wire();  //使用发送数据
                    save_user_data_area3();
                    fb_motor_state(0);
   
                    fc_effect.base_ins.motor_on_off = 0;
                }
                else if(key_value == RF24_K27)
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
      

                }
                else if(key_value == RF24_K28)
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
                    
                    fb_motor_period();
                }
                else
                {
                    
                    switch(key_value)
                    {
                        case RF24_K08:
                            soft_rurn_off_lights();     //软关灯
                        break;



                        case RF24_K09:
                            fc_static_effect(0);

                            break;
                        case RF24_K10:
                            fc_static_effect(1);

                            break;
                        case RF24_K11:
                            fc_static_effect(2);

                            break;
                        case RF24_K14:
                            fc_static_effect(4);

                            break;
                        case RF24_K15:
                            fc_static_effect(5);

                            break;
                        case RF24_K16:
                            fc_static_effect(9);

                            break;
                        case RF24_K13:
                            fc_static_effect(8);

                            break;
                        case RF24_K12:
                            fc_static_effect(3);

                            break;
                        case RF24_K17:
                            ls_set_color(0, BLUE);
                            ls_set_color(1, GREEN);
                            ls_set_color(2, RED);
                            fc_effect.dream_scene.change_type = MODE_JUMP;
                            fc_effect.dream_scene.c_n = 3;
                            fc_effect.Now_state = IS_light_scene;
                            set_fc_effect();
                            break;
                        case RF24_K18:
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
                        case RF24_K19:
                            printf("\n RFKEY_FADE3");
                            extern void change_breath_mode(void);

                            change_breath_mode();
                            break;
                        case RF24_K20:
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
                        case RF24_K21:
                            fc_effect.Now_state = IS_light_music;
                            fc_effect.music.m = 0;
                            set_fc_effect();

                            break;
                        case RF24_K22:
                            fc_effect.Now_state = IS_light_music;
                            fc_effect.music.m = 1;
                            set_fc_effect();
                            break;
                        case RF24_K23:
                            fc_effect.Now_state = IS_light_music;
                            fc_effect.music.m = 2;
                            set_fc_effect();
                            break;
                        case RF24_K24:
                            fc_effect.Now_state = IS_light_music;
                            fc_effect.music.m = 3;
                            set_fc_effect();
                            break;

                    }//switch
                    save_user_data_area3();
                }

            }//if(event_type == KEY_EVENT_CLICK)

        } 





else if (rf24_T0 > 15)
{
    rf24_T1 = 0;
    long_press_f = 0;
    long_press_cnt = 0;
    printf("cled");
}


   
    
        if(get_rf24g_long_state() == KEY_EVENT_LONG && rf24_T1 > 3)  //长按
        {
            long_press_f = 1;
            key_value =  last_key_v;
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

                extern u8 single_flow_flag;
                if( (get_on_off_state() == DEVICE_ON) &&  long_press_f)
                {
                    long_press_cnt++;
  



                }




}

#endif


