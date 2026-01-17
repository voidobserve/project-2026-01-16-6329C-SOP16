/****************************
@led_strand_effect.c
适用：
产品ID: yxwh27s5
品类：幻彩户外串灯-蓝牙
协议：BLE
负责幻彩灯串效果制作
*****************************/
#include "system/includes.h"
#include "led_strand_effect.h"
#include "WS2812FX.H"
#include "ws2812fx_effect.h"
#include "Adafruit_NeoPixel.H"
#include "tuya_ble_type.h"
#include "asm/mcpwm.h"

extern void printf_buf(u8 *buf, u32 len);
static void static_mode(void);
static void fc_smear_adjust(void);
static void fc_pair_effect(void);
static void ls_scene_effect(void);
void fc_set_style_custom(void);
void custom_effect(void);
static void strand_rainbow(void);
void jump_mutil_c(void) ;
void strand_breath(void) ;
void strand_twihkle(void) ;
void strand_flow_water(void) ;
void strand_chas_light(void) ;
void strand_colorful(void) ;
void mutil_seg_grandual(void) ;
void mutil_c_grandual(void);
void ls_set_colors(uint8_t n, color_t *c);

void soft_rurn_off_lights(void); //软关灯处理
 void soft_turn_on_the_light(void);   //软开灯处理


fc_effect_t fc_effect;//幻彩灯串效果数据
static u8 custom_index;
// 和通信协议对应
u8 rgb_sequence_map[6]=
{
    NEO_RGB,
    NEO_RBG,
    NEO_GRB,
    NEO_GBR,
    NEO_BRG,
    NEO_BGR,
};
// 效果数据初始化
void fc_data_init(void)
{
    u16 num;
    fc_effect.on_off_flag = DEVICE_ON;
    fc_effect.led_num = 1;
    fc_effect.Now_state = IS_STATIC;
    fc_effect.custom_index = 1;
    fc_effect.rgb.r = 255;
    fc_effect.rgb.g = 0;
    fc_effect.rgb.b = 0;
    fc_effect.dream_scene.speed = 100;
    fc_effect.sequence = NEO_BGR;
    fc_effect.b = 255;
    fc_effect.speed = 80;
    fc_effect.music.m = 1;
    fc_effect.music.s = 80;   //灵敏度
    //电机
    fc_effect.base_ins.mode = 4;   //360转
    fc_effect.base_ins.period = 8;  //速度8s
    fc_effect.base_ins.dir = 0 ;  // 0: 正转  1：
    fc_effect.base_ins.motor_on_off = 1; //电机开
}





u16 bw_speed = 0;
u16 _b = 0;
u16 _w = 10000;
u16  liangdu = 0;
u8  f_ = 1;
u8  step_ = 0;
u16 custom_delay_cnt = 0;

#define dengji 20
void bw_breath_effect(void)
{

    if(fc_effect.Now_state == ACT_CUSTOM  && fc_effect.custom_index == 1)
    {
        bw_speed += 10;
        if(f_ == 2)
        {
            custom_delay_cnt += 10;
            if(custom_delay_cnt >= 3000)
            {
                custom_delay_cnt = 0;
                f_ = 1;

            }
        }
        if(bw_speed >=  fc_effect.dream_scene.speed)
        {
            bw_speed = 0;
            if(f_ == 1)
            {
                if(liangdu < (10000 - dengji))
                {
                    liangdu += dengji;
                }
                else
                {
                    liangdu = 10000;
                    f_ = 0;
                }
            }
            else if(f_ == 0)
            {
                if(liangdu > 0 )
                {
                    liangdu -= dengji;
                }
                else
                {
                    liangdu = 0;
                    f_ = 2;

                }

            }
            // printf("liangdu = %d", liangdu);
        }


        tow_led_drive(10000,10000,liangdu);
    }

}


void bw_gradual_effect(void)
{

    if(fc_effect.Now_state == ACT_CUSTOM  && fc_effect.custom_index == 2)
    {
        bw_speed += 10;
        if(bw_speed >=  fc_effect.dream_scene.speed)
        {
            bw_speed = 0;
            if(f_)
            {
                _b += 100;
                _w -= 100;
                if(_b >= 10000)
                {
                    f_ = 0;
                }
            }
            else
            {
                _b -= 100;
                _w += 100;
                if(_w >= 10000)
                {
                    f_ = 1;
                }

            }
        }
        tow_led_drive(_b,_w,(fc_effect.b * 10000 / 255));
    }



}

//蓝色呼吸 》 白色呼吸 》 蓝白呼吸
void bw_effect3(void)
{

    if(fc_effect.Now_state == ACT_CUSTOM && fc_effect.custom_index == 3)
    {
        bw_speed += 10;
        if(f_ == 2)
        {
            custom_delay_cnt += 10;
            if(custom_delay_cnt >= 3000)
            {
                custom_delay_cnt = 0;
                f_ = 1;
                if(step_ < 2)
                {
                    step_++;
                }
                else
                {
                    step_ = 0;
                }
            }
        }
        if(bw_speed >=  fc_effect.dream_scene.speed)
        {
            bw_speed = 0;
            switch(step_)
            {
                case 0: //蓝色亮 灭
                _b = 10000;
                _w = 0;
                if(f_ == 1)
                {
                    if(liangdu < (10000 - dengji))  //亮
                    {
                        liangdu += dengji;
                    }
                    else
                    {
                        liangdu = 10000;
                        f_ = 0;
                    }
                }
                else if(f_ == 0)
                {
                    if(liangdu > 0 )  //灭
                    {
                        liangdu -= dengji;
                    }
                    else
                    {
                        liangdu = 0;
                        f_ = 2;

                    }
                }
                break;
                case 1:
                _b = 0;
                _w = 10000;
                if(f_ == 1)
                {
                    if(liangdu < (10000 - dengji))  //亮
                    {
                        liangdu += dengji;
                    }
                    else
                    {
                        liangdu = 10000;
                        f_ = 0;
                    }
                }
                else if(f_ == 0)
                {
                    if(liangdu > 0 )  //灭
                    {
                        liangdu -= dengji;
                    }
                    else
                    {
                        liangdu = 0;
                        f_ = 2;

                    }
                }
                break;
                case 2:
                _b = 10000;
                _w = 10000;
                if(f_ == 1)
                {
                    if(liangdu < (10000 - dengji))  //亮
                    {
                        liangdu += dengji;
                    }
                    else
                    {
                        liangdu = 10000;
                        f_ = 0;
                    }
                }
                else if(f_ == 0)
                {
                    if(liangdu > 0 )  //灭
                    {
                        liangdu -= dengji;
                    }
                    else
                    {
                        liangdu = 0;
                        f_ = 2;

                    }
                }
                break;
            }
        }
        tow_led_drive(_b,_w,liangdu);
    }

}



void full_color_init(void)
{
extern void read_flash_device_status_init(void);

    extern uint16_t WS2812FX_mode_static(void) ;
    read_flash_device_status_init();
    WS2812FX_init(1, fc_effect.sequence );
    WS2812FX_stop();
    WS2812FX_setBrightness( 255 );
    if(fc_effect.on_off_flag == DEVICE_ON)
    {
        soft_turn_on_the_light();
    }
    else
    {
        soft_rurn_off_lights();
    }


    //  WS2812FX_start();
    // set_fc_effect();
}





void fc_debug(void)
{
    printf("\n Now_state=%d", fc_effect.Now_state);
    printf("\n rgb = ");
    printf_buf(&fc_effect.rgb, 3);
    printf("\n c_n=%d", fc_effect.dream_scene.c_n);
    printf("\n fc_effect.b = %d",fc_effect.b);
    printf("\n fc_effect.w =%d",fc_effect.w);
    printf("\n fc_effect.on_off_flag=%d",fc_effect.on_off_flag);
    printf("\n fc_effect.music_mode =%d",fc_effect.music_mode);

}
/***************************************************自定义效果*****************************************************/


// extern uint16_t power_on_effect(void);
extern uint16_t power_off_effect(void);

void set_power_off(void)
{
    custom_index = 1; //关机效果
    fc_set_style_custom(); //自定义效果
    set_fc_effect();
}
void custom_effect(void)
{
    extern uint16_t WS2812FX_adj_rgb_sequence(void);
    fc_effect.w = 0;
    if(custom_index==0) //开机效果
    {
        //WS2812FX_stop();
        // WS2812FX_setSegment_colorOptions(0,0,fc_effect.led_num-1,&power_on_effect,0,0,0);
        // WS2812FX_start();
    }
    else if(custom_index == 1) //关机效果
    {
        //WS2812FX_stop();
        WS2812FX_setSegment_colorOptions(0,0,fc_effect.led_num-1,&power_off_effect,0,0,0);
        WS2812FX_start();
    }
    else if(custom_index == 2)  //调整RGB顺序效果
    {
        //WS2812FX_stop();
        WS2812FX_setSegment_colorOptions(0,0,fc_effect.led_num-1,&WS2812FX_adj_rgb_sequence,0,0,0);
        WS2812FX_start();
    }
    else if(custom_index == 3)
    {
        extern uint16_t max_bright_tips_blue(void);
        WS2812FX_setSegment_colorOptions(0,0,fc_effect.led_num-1,&max_bright_tips_blue,0,0,0);
        WS2812FX_start();
    }
    else if(custom_index == 4)
    {
        extern uint16_t max_bright_tips_puple(void);
        WS2812FX_setSegment_colorOptions(0,0,fc_effect.led_num-1,&max_bright_tips_puple,0,0,0);
        WS2812FX_start();
    }
}

/**
 * @brief 向app反馈灯开关状态
 *
 */
void fb_led_on_off_state(void)
{
    uint8_t Send_buffer[6];
    Send_buffer[0] = 0x01;
    Send_buffer[1] = 0x01;
    Send_buffer[2] = fc_effect.on_off_flag; //
    extern void zd_fb_2_app(u8 *p, u8 len);
    zd_fb_2_app(Send_buffer, 3);


}

void fb_led_bright_state(void)
{
    uint8_t Send_buffer[6];
    Send_buffer[0] = 0x04;
    Send_buffer[1] = 0x03;
    Send_buffer[2] = fc_effect.b*100/255; //
    extern void zd_fb_2_app(u8 *p, u8 len);
    zd_fb_2_app(Send_buffer, 3);


}

void fb_led_speed_state(void)
{
    uint8_t Send_buffer[6];
    Send_buffer[0] = 0x04;
    Send_buffer[1] = 0x04;
    Send_buffer[2] =  (500 - fc_effect.dream_scene.speed) / 5;
    extern void zd_fb_2_app(u8 *p, u8 len);
    zd_fb_2_app(Send_buffer, 3);


}


void fb_motor_state(u8 p)
{
    uint8_t Send_buffer[6];
    Send_buffer[0] = 0x2F;
    Send_buffer[1] = 0x08;
    Send_buffer[2] =  p;
    extern void zd_fb_2_app(u8 *p, u8 len);
    zd_fb_2_app(Send_buffer, 3);

  
}
void fb_motor_period()
{
    uint8_t Send_buffer[6];
    Send_buffer[0] = 0x2F;
    Send_buffer[1] = 0x07;
    Send_buffer[2] = fc_effect.base_ins.period;
    extern void zd_fb_2_app(u8 *p, u8 len);
    zd_fb_2_app(Send_buffer, 3);

  
}

#include "ble_trans_profile.h"
#include "btstack/le/att.h"
extern uint16_t power_off_effect(void);
extern u8 Ble_Addr[6]; //蓝牙地址
extern u8 music_trigger;
u8 jianbian_start = 0;
u8 pwr_on_effect_f=0;  //0:无需执行开机效果
u8 pwr_on_effect_f1=0;  //0:无需执行开机效果
static float bb =1;
extern u8 temp_w_bright;
/************************************** **************软件关机*****************************************************/
void soft_rurn_off_lights(void) //软关灯处理
{

    pwr_on_effect_f = 0;
    pwr_on_effect_f1 = 0;
    jianbian_start = 0;

    if(fc_effect.w == 0)
    {
        WS2812FX_setSegment_colorOptions(
        0,                                      //第0段
        0,fc_effect.led_num-1,                  //起始位置，结束位置
        &power_off_effect,                  //效果
        0,                                      //颜色，WS2812FX_setColors设置
        100,                                   //速度
        FADE_XXSLOW);                            //选项，这里像素点大小：1
        WS2812FX_start();


    }
 
    //===============  清灯状态  ===========
    fc_effect.on_off_flag = DEVICE_OFF;
    music_trigger = 0;
    fb_led_on_off_state();
   
    //===============  关风扇  ===========
    close_fan(); 

    // ==========  关电机   ===========
    one_wire_set_mode(6); //配置模式
    // os_time_dly(1);  //不能使用，会复位
    enable_one_wire();  //使用发送数据
    fb_motor_state(0);
    fc_effect.base_ins.motor_on_off = 0;


    save_user_data_area3();
    printf("soft_rurn_off_light!!\n");

}

void special_w_close(void)
{
    static u8 u1_delay = 0;
    if(fc_effect.on_off_flag == DEVICE_OFF && fc_effect.w != 0)
    {

       
        if(temp_w_bright > 0)
        {

            temp_w_bright--;
            mcpwm_set_duty(pwm_ch3, fc_effect.w*10000/255*temp_w_bright/255);

        }


    } 
    else if(fc_effect.on_off_flag == DEVICE_ON && fc_effect.w != 0)
    {

        if(pwr_on_effect_f1)
        {

        if(temp_w_bright < fc_effect.b)
        {
temp_w_bright++;
 mcpwm_set_duty(pwm_ch3, fc_effect.w*10000/255*temp_w_bright/255);

        }
        else{

            pwr_on_effect_f1 = 0;
        }

        }




    }


}




/**************************************************软件开机*****************************************************/
 void soft_turn_on_the_light(void)   //软开灯处理
{

    pwr_on_effect_f = 1;
    pwr_on_effect_f1 = 1;
    bb = 1;
    WS2812FX_setBrightness(1);
    WS2812FX_start();
    fc_effect.on_off_flag = DEVICE_ON;

  
   //============  开风扇   ===========
    open_fan(); 

    //============  开电机   ===========
    one_wire_set_mode(4);
    // os_time_dly(1);//不能使用，会复位
    enable_one_wire();       //启动发送电机数据
    fb_motor_state(1);


    fc_effect.base_ins.motor_on_off = 1;
    fb_led_on_off_state();
    set_fc_effect();
    save_user_data_area3();

    printf("soft_turn_on_the_light!!\n");

}





// 开机效果,控制亮度渐亮,定时10ms执行
// 应用判断亮度达到最大亮度
void power_on_effect(void)
{

static u8 u_delay = 0;

  if(pwr_on_effect_f == 0){
    u_delay = 0;

    
    return;

  }

  if(u_delay < 1)
  {
    u_delay++;
  }
  else 
  {

    jianbian_start = 1;
    u_delay = 0;
    if(bb<128)
    {
        bb = bb+0.5;
       
    }
    else
    {
        bb+=1;
        
    }
  
    if(bb >= fc_effect.b)
    {
      WS2812FX_setBrightness(fc_effect.b);
    
      pwr_on_effect_f = 0;
      bb =1;
    }
    else
    {
        temp_w_bright += 1;
      WS2812FX_setBrightness(bb);
    
    }

  }
 
}




ON_OFF_FLAG get_on_off_state(void)
{
    return fc_effect.on_off_flag;
}

void set_on_off_led(u8 on_off)
{
    fc_effect.on_off_flag = on_off;
    printf("\n on_off=%d",on_off);
    if(fc_effect.on_off_flag == DEVICE_ON)
    {
        soft_turn_on_the_light();
    }
    else
    {
        soft_rurn_off_lights();
    }
}

void ls_open(void)
{

    if(fc_effect.on_off_flag == DEVICE_OFF)
    {

        soft_turn_on_the_light();
    }

}



//-----------------------------------------------------------------------------------------静态模式

#define MAX_STATIC_N    11
// 静态效果颜色map
const u32 fc_static_map[MAX_STATIC_N] =
{
    RED,    //0
    GREEN,  //1
    BLUE,   //2
    WHITE,  //3
    YELLOW, //4
    CYAN,   //5
    MAGENTA,//6
    PURPLE, //7
    ORANGE, //8
    PINK,   //9
    GRAY,
};


// 利用fc_effect结构体，构建内置效果
void fc_static_effect(u8 n)
{
    fc_effect.Now_state = IS_STATIC;
    if(fc_static_map[n] != WHITE)
    {
        fc_effect.rgb.r = (fc_static_map[n] >> 16) & 0xff;
        fc_effect.rgb.g = (fc_static_map[n] >> 8) & 0xff;
        fc_effect.rgb.b = (fc_static_map[n] ) & 0xff;
        fc_effect.w = 0;
    }
    else
    {
        printf("bai");
        fc_effect.w = 255;
    }
    set_fc_effect();
}

void set_static_mode(u8 r, u8 g, u8 b)
{
    fc_effect.Now_state = IS_STATIC;
    fc_effect.rgb.r = r;
    fc_effect.rgb.g = g;
    fc_effect.rgb.b = b;
    fc_effect.w = 0;
    set_fc_effect();
}


static void static_mode(void)
{
    //WS2812FX_stop();
    WS2812FX_setSegment_colorOptions(
        0,                                      //第0段
        0,fc_effect.led_num-1,                  //起始位置，结束位置
        &WS2812FX_mode_static,                  //效果
        0,                                      //颜色，WS2812FX_setColors设置
        100,                                    //速度
        0);                                     //选项，这里像素点大小：1
    WS2812FX_set_coloQty(0,fc_effect.dream_scene.c_n);
    ls_set_colors(1, &fc_effect.rgb);
    WS2812FX_start();
}
/******************************************************************
 * 函数：更新涂抹效果数据
 * 形参1：tool       油桶、画笔、橡皮擦
 * 形参2：colour     hsv颜色
 * 形参3：led_place  灯点位置（0~47）
 * 返回：无
 *
 * 注：若选择IS_drum油桶，led_place参数无效
 *     若选择IS_eraser橡皮擦，colour参数无效，内部将colour设为黑色
 *****************************************************************/
void effect_smear_adjust_updata(smear_tool_e tool, hsv_t *colour,unsigned short *led_place)
{
    unsigned char num = 0;
    unsigned char max;

    //更新为涂抹功能状态
    fc_effect.Now_state = IS_smear_adjust;

    //更新工具
    fc_effect.smear_adjust.smear_tool = tool;
    printf("fc_effect.smear_adjust.smear_tool = %d\r\n",(uint8_t)fc_effect.smear_adjust.smear_tool);
    printf("\r\n");

    //清除rgb[0~n]数据
    // memset(fc_effect.smear_adjust.rgb, 0, sizeof(fc_effect.smear_adjust.rgb));

    /*HSV转换RGB*/
    if(fc_effect.smear_adjust.smear_tool == IS_drum) //油桶
    {
      m_hsv_to_rgb(&fc_effect.smear_adjust.rgb[0].r,
                   &fc_effect.smear_adjust.rgb[0].g,
                   &fc_effect.smear_adjust.rgb[0].b,
                   colour->h_val,
                   colour->s_val,
                   colour->v_val);
      max = fc_effect.led_num;
      for(num = 1; num < max; ++num)
      {
        fc_effect.smear_adjust.rgb[num].r = fc_effect.smear_adjust.rgb[0].r;
        fc_effect.smear_adjust.rgb[num].g = fc_effect.smear_adjust.rgb[0].g;
        fc_effect.smear_adjust.rgb[num].b = fc_effect.smear_adjust.rgb[0].b;


      }
    }
    else if((fc_effect.smear_adjust.smear_tool == IS_pen) ||   //画笔
            (fc_effect.smear_adjust.smear_tool == IS_eraser))  //橡皮擦
    {
        m_hsv_to_rgb(&fc_effect.smear_adjust.rgb[*led_place].r,
                     &fc_effect.smear_adjust.rgb[*led_place].g,
                     &fc_effect.smear_adjust.rgb[*led_place].b,
                     colour->h_val,
                     colour->s_val,
                     colour->v_val);

    }
}
/*----------------------------------涂抹模式----------------------------------*/
extern  Segment* _seg;
extern  uint16_t _seg_len;
extern Segment_runtime* _seg_rt;


static uint16_t ls_smear_adjust_effect(void)
{
  unsigned char num;
  unsigned char max = fc_effect.led_num;
  if(max >= _seg_len) max = _seg_len;
  for(num = 0; num < max; ++num)
  {
      WS2812FX_setPixelColor_rgb(num,
        fc_effect.smear_adjust.rgb[num].r,
        fc_effect.smear_adjust.rgb[num].g,
        fc_effect.smear_adjust.rgb[num].b);
  }
  return _seg->speed;
}

// ---------------------------------------------------------------------------------------------------------动态效果

static void fc_smear_adjust(void)
{
    //WS2812FX_stop();
    WS2812FX_setSegment_colorOptions(0,0,fc_effect.led_num-1,&ls_smear_adjust_effect,BLUE,100,0);
    WS2812FX_start();
}

/*----------------------------------涂鸦配网效果----------------------------------*/
static void fc_pair_effect(void)
{
    extern uint16_t unbind_effect(void);
    //WS2812FX_stop();
    WS2812FX_setSegment_colorOptions(0,0,fc_effect.led_num-1,&unbind_effect,0,0,0);
    WS2812FX_start();
}



/*----------------------------------彩虹模式----------------------------------*/
static void strand_rainbow(void)
{
    // //WS2812FX_stop();
    // printf("\n fc_effect.dream_scene.c_n=%d",fc_effect.dream_scene.c_n);
    // printf("\n fc_effect.led_num=%d",fc_effect.led_num);
    // printf("\n fc_effect.dream_scene.speed=%d",fc_effect.dream_scene.speed);
    // printf("\n fc_effect.dream_scene.rgb");
    // printf_buf(fc_effect.dream_scene.rgb, fc_effect.dream_scene.c_n*sizeof(color_t));
    // printf("\n fc_effect.dream_scene.direction=%d",fc_effect.dream_scene.direction);

    WS2812FX_setSegment_colorOptions(
        0,                                      //第0段
        0,fc_effect.led_num-1,                  //起始位置，结束位置
        &WS2812FX_mode_mutil_fade,               //效果
        0,                                      //颜色，WS2812FX_setColors设置
        fc_effect.dream_scene.speed,            //速度
        SIZE_SMALL);                            //选项，这里像素点大小：1

    WS2812FX_set_coloQty(0,fc_effect.dream_scene.c_n);
    ls_set_colors(fc_effect.dream_scene.c_n, &fc_effect.dream_scene.rgb);

    WS2812FX_start();
}

/*----------------------------------跳变模式----------------------------------*/
// 多段颜色跳变
void jump_mutil_c(void)
{
    //WS2812FX_stop();
    WS2812FX_setSegment_colorOptions(
        0,                                      //第0段
        0,fc_effect.led_num-1,                  //起始位置，结束位置
        &WS2812FX_mode_single_block_scan,       //效果
        0,                                      //颜色，WS2812FX_setColors设置
        fc_effect.dream_scene.speed,            //速度
        SIZE_MEDIUM);                           //选项，这里像素点大小：3

    WS2812FX_set_coloQty(0,fc_effect.dream_scene.c_n);
    ls_set_colors(fc_effect.dream_scene.c_n, &fc_effect.dream_scene.rgb);

    WS2812FX_start();
}

// 标准跳变，整体颜色跳变
void standard_jump(void)
{
    extern uint16_t WS2812FX_mutil_c_jump(void);
    //WS2812FX_stop();
    WS2812FX_setSegment_colorOptions(
        0,                                      //第0段
        0,fc_effect.led_num-1,                  //起始位置，结束位置
        &WS2812FX_mutil_c_jump,       //效果
        0,                                      //颜色，WS2812FX_setColors设置
        fc_effect.dream_scene.speed,            //速度
        0);                           //选项，这里像素点大小：3

    WS2812FX_set_coloQty(0,fc_effect.dream_scene.c_n);
    ls_set_colors(fc_effect.dream_scene.c_n, &fc_effect.dream_scene.rgb);
    WS2812FX_start();
}

/*----------------------------------频闪模式----------------------------------*/
void ls_strobe(void)
{
    extern uint16_t WS2812FX_mutil_strobe(void);

    //WS2812FX_stop();
    WS2812FX_setSegment_colorOptions(
        0,                                      //第0段
        0,fc_effect.led_num-1,                  //起始位置，结束位置
        &WS2812FX_mutil_strobe,       //效果
        0,                                      //颜色，WS2812FX_setColors设置
        fc_effect.dream_scene.speed,            //速度
        0);                           //选项，这里像素点大小：3

    WS2812FX_set_coloQty(0,fc_effect.dream_scene.c_n);
    ls_set_colors(fc_effect.dream_scene.c_n, &fc_effect.dream_scene.rgb);
    WS2812FX_start();
}


/*----------------------------------呼吸模式----------------------------------*/
void strand_breath(void)
{
    //WS2812FX_stop();
    WS2812FX_setSegment_colorOptions(
        0,                                      //第0段
        0,fc_effect.led_num-1,                  //起始位置，结束位置
        &WS2812FX_mode_mutil_breath,            //效果
        0,                                      //颜色，WS2812FX_setColors设置
        fc_effect.dream_scene.speed,            //速度
        SIZE_MEDIUM);                           //选项，这里像素点大小：3

    WS2812FX_set_coloQty(0,fc_effect.dream_scene.c_n);
    ls_set_colors(fc_effect.dream_scene.c_n, &fc_effect.dream_scene.rgb);

    WS2812FX_start();
}



void single_c_breath(void)
{
    extern uint16_t breath_rgb(void) ;
extern uint16_t WS2812FX_mode_breath(void) ;
    WS2812FX_setSegment_colorOptions(
    0,                                      //第0段
    0,fc_effect.led_num-1,                  //起始位置，结束位置
    &breath_rgb,            //效果
    0,                                      //颜色，WS2812FX_setColors设置
    fc_effect.dream_scene.speed,            //速度
    SIZE_MEDIUM);                           //选项，这里像素点大小：3

    WS2812FX_set_coloQty(0,fc_effect.dream_scene.c_n);
    ls_set_colors(fc_effect.dream_scene.c_n, &fc_effect.dream_scene.rgb);

    WS2812FX_start();
}
/*----------------------------------闪烁模式----------------------------------*/
void strand_twihkle(void)
{
    uint8_t option;
    //WS2812FX_stop();
    WS2812FX_setSegment_colorOptions(
        0,                                      //第0段
        0,fc_effect.led_num-1,                  //起始位置，结束位置
        &WS2812FX_mode_mutil_twihkle,           //效果
        0,                                      //颜色，WS2812FX_setColors设置
        fc_effect.dream_scene.speed,            //速度
        SIZE_SMALL);                            //选项，这里像素点大小：1
    WS2812FX_set_coloQty(0,fc_effect.dream_scene.c_n);
    ls_set_colors(fc_effect.dream_scene.c_n, &fc_effect.dream_scene.rgb);

    WS2812FX_start();

}

/*----------------------------------流水模式----------------------------------*/
void strand_flow_water(void)
{
    uint8_t option;
    // 正向
    if(fc_effect.dream_scene.direction == IS_forward)
    {
        option = SIZE_MEDIUM | 0;
    }
    else{
        option = SIZE_MEDIUM | REVERSE;
    }

    //WS2812FX_stop();
    WS2812FX_setSegment_colorOptions(
        0,                                      //第0段
        0,fc_effect.led_num-1,                  //起始位置，结束位置
        &WS2812FX_mode_multi_block_scan,        //效果
        0,                                      //颜色，WS2812FX_setColors设置
        fc_effect.dream_scene.speed,            //速度
        option);                                //选项，这里像素点大小：3,反向/反向
    WS2812FX_set_coloQty(0,fc_effect.dream_scene.c_n);
    ls_set_colors(fc_effect.dream_scene.c_n, &fc_effect.dream_scene.rgb);

    WS2812FX_start();
}

// 两种颜色混合流水效果，渐变色流水
void tow_color_fix_flow(void)
{
    extern uint16_t WS2812FX_mode_running_lights(void) ;

    uint8_t option;
    if(fc_effect.dream_scene.direction == IS_forward)
    {
        option = 0;
    }
    else
    {
        option = REVERSE;
    }
    // WS2812FX_stop();
    WS2812FX_setSegment_colorOptions(
        0,                                      //第0段
        0,fc_effect.led_num-1,                  //起始位置，结束位置
        &WS2812FX_mode_running_lights,             //效果
        0,                                      //颜色，WS2812FX_setColors设置
        fc_effect.dream_scene.speed,            //速度
        option);                            //选项，这里像素点大小：1
    WS2812FX_set_coloQty(0,fc_effect.dream_scene.c_n);
    ls_set_colors(fc_effect.dream_scene.c_n, &fc_effect.dream_scene.rgb);

    WS2812FX_start();
}
/*----------------------------------追光模式----------------------------------*/
void strand_chas_light(void)
{
    // printf("\n fc_effect.dream_scene.c_n=%d",fc_effect.dream_scene.c_n);
    // printf("\n fc_effect.led_num=%d",fc_effect.led_num);
    // printf("\n fc_effect.dream_scene.speed=%d",fc_effect.dream_scene.speed);
    // printf("\n fc_effect.dream_scene.rgb");
    // printf_buf(fc_effect.dream_scene.rgb, fc_effect.dream_scene.c_n*sizeof(color_t));
    // printf("\n fc_effect.dream_scene.direction=%d",fc_effect.dream_scene.direction);
    //WS2812FX_stop();
    // 正向
    if(fc_effect.dream_scene.direction == IS_forward)
    {
        WS2812FX_setSegment_colorOptions(
            0,                                      //第0段
            0,fc_effect.led_num-1,                  //起始位置，结束位置
            &WS2812FX_mode_multi_forward_same,        //效果
            0,                                      //颜色，WS2812FX_setColors设置
            fc_effect.dream_scene.speed,            //速度
            0);                                     //选项
    }
    else
    {
        WS2812FX_setSegment_colorOptions(
            0,                                      //第0段
            0,fc_effect.led_num-1,                  //起始位置，结束位置
            &WS2812FX_mode_multi_back_same,        //效果
            0,                                      //颜色，WS2812FX_setColors设置
            fc_effect.dream_scene.speed,            //速度
            0);
    }
    WS2812FX_set_coloQty(0,fc_effect.dream_scene.c_n);
    ls_set_colors(fc_effect.dream_scene.c_n, &fc_effect.dream_scene.rgb);

    WS2812FX_start();
}

/*----------------------------------炫彩模式----------------------------------*/
void strand_colorful(void)
{
    uint8_t option;
    //WS2812FX_stop();
    WS2812FX_setSegment_colorOptions(
        0,                                      //第0段
        0,fc_effect.led_num-1,                  //起始位置，结束位置
        &WS2812FX_mode_multi_block_scan,        //效果
        0,                                      //颜色，WS2812FX_setColors设置
        fc_effect.dream_scene.speed,            //速度
        SIZE_SMALL);                            //选项，这里像素点大小：1
    WS2812FX_set_coloQty(0,fc_effect.dream_scene.c_n);
    ls_set_colors(fc_effect.dream_scene.c_n, &fc_effect.dream_scene.rgb);

    WS2812FX_start();
}

/*----------------------------------渐变模式----------------------------------*/
// 多段，不同颜色渐变
void mutil_seg_grandual(void)
{
    //WS2812FX_stop();
    WS2812FX_setSegment_colorOptions(
        0,                                      //第0段
        0,fc_effect.led_num-1,                  //起始位置，结束位置
        &WS2812FX_mode_mutil_fade,              //效果
        0,                                      //颜色，WS2812FX_setColors设置
        fc_effect.dream_scene.speed,            //速度
        SIZE_MEDIUM);                                //选项，这里像素点大小：3,反向/反向

    WS2812FX_set_coloQty(0,fc_effect.dream_scene.c_n);
    ls_set_colors(fc_effect.dream_scene.c_n, &fc_effect.dream_scene.rgb);

    WS2812FX_start();
}

// 整条灯带渐变，支持多种颜色之间切换
// 颜色池：fc_effect.dream_scene.rgb[]
// 颜色数量fc_effect.dream_scene.c_n
void mutil_c_grandual(void)
{
    extern uint16_t WS2812FX_mutil_c_gradual(void);

    WS2812FX_stop();
    WS2812FX_setSegment_colorOptions(
        0,                                      //第0段
        0,fc_effect.led_num-1,                  //起始位置，结束位置
        &WS2812FX_mutil_c_gradual,              //效果
        0,                                      //颜色，WS2812FX_setColors设置
        fc_effect.dream_scene.speed,            //速度
        SIZE_MEDIUM);                           //选项，这里像素点大小：3,反向/反向

    WS2812FX_set_coloQty(0,fc_effect.dream_scene.c_n);
    ls_set_colors(fc_effect.dream_scene.c_n, &fc_effect.dream_scene.rgb);
    WS2812FX_start();
}

/*----------------------------------星空----------------------------------*/

// 单点单色随机闪现
void single_c_flash_random(void)
{
    extern uint16_t WS2812FX_mode_fire_flicker_intense(void);
    WS2812FX_setSegment_colorOptions(
        0,                                      //第0段
        0,fc_effect.led_num-1,                  //起始位置，结束位置
        &WS2812FX_mode_fire_flicker_intense,    //效果
        0,                                      //颜色，WS2812FX_setColors设置
        fc_effect.dream_scene.speed,            //速度
        0);                           //选项，这里像素点大小：3,反向/反向

    WS2812FX_set_coloQty(0,fc_effect.dream_scene.c_n);
    ls_set_colors(fc_effect.dream_scene.c_n, &fc_effect.dream_scene.rgb);
    WS2812FX_start();
}

// 从的颜色池抽取颜色闪现，以段为单位，闪现位置随机
void seg_mutil_c_flash_random(void)
{
    extern uint16_t WS2812FX_mode_fireworks(void);
    WS2812FX_setSegment_colorOptions(
        0,                                      //第0段
        0,fc_effect.led_num-1,                  //起始位置，结束位置
        &WS2812FX_mode_fireworks,               //效果
        0,                                      //颜色，WS2812FX_setColors设置
        fc_effect.dream_scene.speed,            //速度
        0);                           //选项，这里像素点大小：3,反向/反向

    WS2812FX_set_coloQty(0,fc_effect.dream_scene.c_n);
    ls_set_colors(fc_effect.dream_scene.c_n, &fc_effect.dream_scene.rgb);

    WS2812FX_start();
}

/*----------------------------------流星效果----------------------------------*/
// 单色流星效果
void single_meteor(void)
{
    uint8_t option;
    if(fc_effect.dream_scene.direction == IS_forward)
    {
        option = 0;
    }
    else
    {
        option = REVERSE;
    }
    extern uint16_t WS2812FX_mode_comet(void);
    WS2812FX_setSegment_colorOptions(
        0,                                      //第0段
        0,fc_effect.led_num-1,                  //起始位置，结束位置
        &WS2812FX_mode_comet,                   //效果
        0,                                      //颜色，WS2812FX_setColors设置
        fc_effect.dream_scene.speed,            //速度
        option);                                //选项，这里像素点大小：3,反向/反向

    WS2812FX_set_coloQty(0,fc_effect.dream_scene.c_n);
    ls_set_colors(fc_effect.dream_scene.c_n, &fc_effect.dream_scene.rgb);
    WS2812FX_start();
}

/*----------------------------------自定义 - 三段流星追逐----------------------------------*/
void three_seg_meteor(void)
{
	WS2812FX_setSegment_colorOptions(
			0,                                      //第0段
			0,59,                  //起始位置，结束位置
			&WS2812FX_mode_larson_scanner,               //效果
			WHITE,                                      //颜色，WS2812FX_setColors设置
			2000,            //速度
			FADE_FAST);                            //选项，这里像素点大小：1

	WS2812FX_setSegment_colorOptions(
			1,                                      //第0段
			0,59,                  //起始位置，结束位置
			&WS2812FX_mode_color_sweep_random,               //效果
			WHITE,                                      //颜色，WS2812FX_setColors设置
			800,            //速度
			0);                            //选项，这里像素点大小：1

	WS2812FX_setSegment_colorOptions(
			2,                                      //第0段
			0,59,                  //起始位置，结束位置
			&WS2812FX_mode_color_sweep_random,               //效果
			WHITE,                                      //颜色，WS2812FX_setColors设置
			800,            				//速度
			0);                            //选项，这里像素点大小：1

    WS2812FX_start();
	set_seg_forward_out(2,800);
}

/*----------------------------------情景效果实现----------------------------------*/
static void ls_scene_effect(void)
{

    switch (fc_effect.dream_scene.change_type)
    {

        case MODE_MUTIL_RAINBOW:      //彩虹
            strand_rainbow();
            // printf("\n IS_SCENE_RAINBOW");
        break;
        case MODE_MUTIL_JUMP://跳变模式
            jump_mutil_c();
            // printf("\n IS_SCENE_JUMP_CHANGE");
            break;

        case MODE_MUTIL_BRAETH://呼吸模式
            strand_breath();
            break;

        case MODE_MUTIL_TWIHKLE://闪烁模式
            strand_twihkle();
            printf("\n IS_SCENE_TWIHKLE");
            break;

        case MODE_MUTIL_FLOW_WATER://流水模式
            strand_flow_water();
            printf("\n IS_SCENE_FLOW_WATER");
            break;

        case MODE_CHAS_LIGHT://追光模式
            strand_chas_light();
            printf("\n IS_SCENE_CHAS_LIGHT");
            break;

        case MODE_MUTIL_COLORFUL://炫彩模式
            strand_colorful();
            break;

        case MODE_MUTIL_SEG_GRADUAL://渐变模式
            mutil_seg_grandual();
            printf("\n IS_SCENE_GRADUAL_CHANGE");
            break;

        case MODE_JUMP:     //标准跳变
            standard_jump();
            break;

        case MODE_STROBE:   //标准频闪
            ls_strobe();
            break;

        case MODE_MUTIL_C_GRADUAL:  //多段同时渐变
            mutil_c_grandual();
            break;
        case MODE_2_C_FIX_FLOW:      //两种颜色混色流水
            tow_color_fix_flow();
            break;
        case MODE_SINGLE_FLASH_RANDOM:
            single_c_flash_random();
            break;
        case MODE_SEG_FLASH_RANDOM:
            seg_mutil_c_flash_random();
            break;
        case MODE_SINGLE_METEOR:
            single_meteor();
            break;

        case MODE_SINGLE_C_BREATH:
            single_c_breath();
            break;
        case MODE_GRADUAL:
            WS2812FX_setSegment_colorOptions(
            0,                                      //第0段
            0,fc_effect.led_num-1,                  //起始位置，结束位置
            &WS2812FX_mode_rainbow,                   //效果
            0,                                      //颜色，WS2812FX_setColors设置
            fc_effect.dream_scene.speed,            //速度
            0);                                     //选项，这里像素点大小：3,反向/反向
            WS2812FX_start();
            break;
        case MODE_BREATH_W:
            printf("\n MODE_BREATH_W");
            extern uint16_t breath_w(void) ;

            WS2812FX_setSegment_colorOptions(
            0,                                      //第0段
            0,fc_effect.led_num-1,                  //起始位置，结束位置
            &breath_w,                              //效果
            WHITE,                                      //颜色，WS2812FX_setColors设置
            fc_effect.dream_scene.speed,            //速度
            0);                                     //选项，这里像素点大小：3,反向/反向
            WS2812FX_start();
            break;
        case MODE_MUTIL_C_BREATH:

            extern uint16_t mutil_c_breath(void) ;

            WS2812FX_setSegment_colorOptions(
            0,                                      //第0段
            0,fc_effect.led_num-1,                  //起始位置，结束位置
            &mutil_c_breath,                              //效果
            0,                                      //颜色，WS2812FX_setColors设置
            fc_effect.dream_scene.speed,            //速度
            0);                                     //选项，这里像素点大小：3,反向/反向

            WS2812FX_set_coloQty(0,fc_effect.dream_scene.c_n);
            ls_set_colors(6, &fc_effect.dream_scene.rgb);
            WS2812FX_start();
            break;
        default:
            break;
    }
}

// ---------------------------------------------------------------------------------------------------------音乐效果

void set_local_mic_mode(u8 m)
{
    fc_effect.music_mode = m;
    fc_effect.Now_state = IS_light_music;
    set_fc_effect();
}

/*-------------------------------------------API------------------------------------------------*/
//-------------------------------------------------speed
u16 get_max_sp(void)
{
    u16 s;
    s = fc_effect.led_num*30/1000;
    if(s<10) s=10;
    return s; //每个LED30us
}

extern u16 bw_speed;
// 最慢500ms一帧,s:1-100%
void ls_set_speed(uint8_t s)
{
    bw_speed = 0;
    fc_effect.dream_scene.speed =  500 - (500 * s / 100);
    if(fc_effect.dream_scene.speed < 10)
    {
        fc_effect.dream_scene.speed = 10;
    }
    // ls_scene_effect();
     set_fc_effect();
}

void speed_fast(void)
{
    if(fc_effect.dream_scene.speed > 100 )
    {
        fc_effect.dream_scene.speed -= 100;
    }
    else{
        fc_effect.dream_scene.speed = 10;
    }
    if(fc_effect.dream_scene.speed <= 10)
    {
        fc_effect.dream_scene.speed = 10;
        // custom_index = 4;
        // save_user_data_area3(); //只能放在fc_set_style_custom前
        // fc_set_style_custom(); //自定义效果
        // printf("\n speed_fast");
    }
    // printf("fc_effect.dream_scene.speed = %d\n",fc_effect.dream_scene.speed);
    fb_led_speed_state();
    set_fc_effect();

}

void speed_slow(void)
{
    if(fc_effect.dream_scene.speed < 500 - 100)
    {
        fc_effect.dream_scene.speed += 100;
    }
    else{
        fc_effect.dream_scene.speed = 500;
    }
    if(fc_effect.dream_scene.speed >= 500)
    {
        fc_effect.dream_scene.speed = 500;
        // custom_index = 4;
        // save_user_data_area3(); //只能放在fc_set_style_custom前
        // fc_set_style_custom(); //自定义效果
        // // set_fc_effect();
        // printf("\n speed_slow");

    }
    // printf("fc_effect.dream_scene.speed = %d\n",fc_effect.dream_scene.speed);
     fb_led_speed_state();
    set_fc_effect();
}

// ------------------------------------------------亮度
// 0-100
void set_bright(u8 b)
{
    if(b == 0) b = 10;
    fc_effect.b = 255*b/100;

    WS2812FX_setBrightness(fc_effect.b);
}

const u8 bright_jst[8]={255,180,150,120,90,70,50,25};
void bright_plus(void)
{

    if(fc_effect.b >= bright_jst[1])
    {
        // if(fc_effect.rgb.r == 0 && fc_effect.rgb.g == 0 )
        // {
        //     custom_index = 4;
        // }
        // else{
        //     custom_index = 3;
        // }
        fc_effect.b = 255;
        WS2812FX_setBrightness(fc_effect.b);
        save_user_data_area3(); //只能放在fc_set_style_custom前
        // fc_set_style_custom(); //自定义效果
        // set_fc_effect();
    }
    else if(fc_effect.b >= bright_jst[2])
    {
        fc_effect.b = bright_jst[1];
        WS2812FX_setBrightness(fc_effect.b);
        save_user_data_area3();
    }
    else if(fc_effect.b >= bright_jst[3])
    {
        fc_effect.b = bright_jst[2];
        WS2812FX_setBrightness(fc_effect.b);
        save_user_data_area3();
    }
    else if(fc_effect.b >= bright_jst[4])
    {
        fc_effect.b = bright_jst[3];
        WS2812FX_setBrightness(fc_effect.b);
        save_user_data_area3();
    }
    else if(fc_effect.b >= bright_jst[5])
    {
        fc_effect.b = bright_jst[4];
        WS2812FX_setBrightness(fc_effect.b);
        save_user_data_area3();
    }
    else if(fc_effect.b >= bright_jst[6])
    {
        fc_effect.b = bright_jst[5];
        WS2812FX_setBrightness(fc_effect.b);
        save_user_data_area3();
    }
    else if(fc_effect.b >= bright_jst[7])
    {
        fc_effect.b = bright_jst[6];
        WS2812FX_setBrightness(fc_effect.b);
        save_user_data_area3();
    }
    fb_led_bright_state();

    printf("\n fc_effect.b = %d",fc_effect.b);
}

void bright_sub(void)
{

    if(fc_effect.b <= bright_jst[6])
    {
        // custom_index = 4;
        fc_effect.b = bright_jst[7];
        WS2812FX_setBrightness(fc_effect.b);
        save_user_data_area3();
        // fc_set_style_custom(); //自定义效果
        // set_fc_effect();
    }
    else if(fc_effect.b <= bright_jst[5])
    {
        fc_effect.b = bright_jst[6];
        WS2812FX_setBrightness(fc_effect.b);
        save_user_data_area3();
    }
    else if(fc_effect.b <= bright_jst[4])
    {
        fc_effect.b = bright_jst[5];
        WS2812FX_setBrightness(fc_effect.b);
        save_user_data_area3();
    }
    else if(fc_effect.b <= bright_jst[3])
    {
        fc_effect.b = bright_jst[4];
        WS2812FX_setBrightness(fc_effect.b);
        save_user_data_area3();
    }
    else if(fc_effect.b <= bright_jst[2])
    {
        fc_effect.b = bright_jst[3];
        WS2812FX_setBrightness(fc_effect.b);
        save_user_data_area3();
    }
    else if(fc_effect.b <= bright_jst[1])
    {
        fc_effect.b = bright_jst[2];
        WS2812FX_setBrightness(fc_effect.b);
        save_user_data_area3();
    }
    else if(fc_effect.b <= bright_jst[0])
    {
        fc_effect.b = bright_jst[1];
        WS2812FX_setBrightness(fc_effect.b);
        save_user_data_area3();
    }
    fb_led_bright_state();
    printf("\n fc_effect.b = %d",fc_effect.b);

}

void max_bright(void)
{
    fc_effect.b = bright_jst[0];
    WS2812FX_setBrightness(fc_effect.b);
    fb_led_bright_state();
}

void min_bright(void)
{
    fc_effect.b = bright_jst[7];
    WS2812FX_setBrightness(fc_effect.b);
    fb_led_bright_state();

}

/* *********************************样式 */
// 自定义样式
void fc_set_style_custom(void)
{
    fc_effect.Now_state = ACT_CUSTOM;
}

// 涂鸦配对样式
void fc_set_style_ty_pair(void)
{
    fc_effect.Now_state = ACT_TY_PAIR;
}

// ------------------------------------------------RGB顺序
// s:0-5
void set_rgb_sequence(u8 s)
{
    if(s < 6)
    {
        fc_effect.sequence = rgb_sequence_map[s];
        //WS2812FX_stop();
        WS2812FX_init(fc_effect.led_num,fc_effect.sequence);
        custom_index = 2;       //调整RGB顺序效果
        fc_set_style_custom();  //自定义效果
        set_fc_effect();
    }
}


// 设置各段颜色
// n颜色数量
// c颜色池指针
void ls_set_colors(uint8_t n, color_t *c)
{
    uint32_t colors[MAX_NUM_COLORS];
    uint8_t i;
    for(i=0; i < n; i++)
    {
        colors[i] = c[i].r << 16 | c[i].g << 8 | c[i].b;
    }
    WS2812FX_setColors(0,colors);
}

// ------------------------------------------------W通道
void set_w(u8 w)
{
    fc_effect.w = w;
    fc_effect.Now_state = IS_STATIC;
    fc_effect.rgb.r = 0;
    fc_effect.rgb.g = 0;
    fc_effect.rgb.b = 0;
    set_fc_effect();
}

// 设置fc_effect.dream_scene.rgb的颜色池
// n:0-MAX_NUM_COLORS
// c:WS2812FX颜色系，R<<16,G<<8,B在低8位
void ls_set_color(uint8_t n, uint32_t c)
{
    if(n < MAX_NUM_COLORS)
    {
        fc_effect.dream_scene.rgb[n].r = (c>>16) & 0xff;
        fc_effect.dream_scene.rgb[n].g = (c>>8) & 0xff;
        fc_effect.dream_scene.rgb[n].b = c & 0xff;
    }
}

// ------------------------------------------------灯带长度
void set_ls_lenght(u16 l)
{
    if(l>2048)
        l=2048;
    fc_effect.led_num = l;
    WS2812FX_stop();
    WS2812FX_init(fc_effect.led_num,fc_effect.sequence);
    WS2812FX_start();
    set_fc_effect();

}

// --------------------------------------------呼吸模式
void change_breath_mode(void)
{
    fc_effect.breath_mode++;
    fc_effect.breath_mode %= 5;
    switch(fc_effect.breath_mode)
    {
        case 0:
            ls_set_color(0, RED);
            ls_set_color(1, BLACK);
            fc_effect.dream_scene.change_type = MODE_SINGLE_C_BREATH;
            fc_effect.dream_scene.c_n = 2;
        break;

        case 1:
            ls_set_color(0, GREEN);
            ls_set_color(1, BLACK);
            fc_effect.dream_scene.change_type = MODE_SINGLE_C_BREATH;
            fc_effect.dream_scene.c_n = 2;
        break;

        case 2:
            ls_set_color(0, BLUE);
            ls_set_color(1, BLACK);
            fc_effect.dream_scene.change_type = MODE_SINGLE_C_BREATH;
            fc_effect.dream_scene.c_n = 2;
        break;

        case 3:
            fc_effect.dream_scene.change_type = MODE_BREATH_W;
        break;

        case 4:
            fc_effect.w = 0;
            ls_set_color(0, BLUE);
            ls_set_color(1, GREEN);
            ls_set_color(2, RED);
            ls_set_color(3, YELLOW);
            ls_set_color(4, CYAN);
            ls_set_color(5, MAGENTA);
            fc_effect.dream_scene.c_n = 6;
            fc_effect.dream_scene.change_type = MODE_MUTIL_C_BREATH;
        break;
    }


    fc_effect.Now_state = IS_light_scene;
    set_fc_effect();
}


/*-------------------------------------------声控----------------------------------------*/
void music_mode_plus(void)
{

    fc_effect.music.m++;
    fc_effect.music.m %= 12;
    fc_effect.Now_state = IS_light_music;
    set_fc_effect();
}

void music_mode_sub(void)
{
    if(fc_effect.music.m > 0)
        fc_effect.music.m--;
    else fc_effect.music.m = 11;
    fc_effect.Now_state = IS_light_music;
    set_fc_effect();
}

void set_music_mode(u8 m)
{
    printf("\n set_music_mode = %d", m);
    fc_effect.music.m = m;
    fc_effect.Now_state = IS_light_music;
    set_fc_effect();
}

void set_music_sensitive(u8 s)
{
    printf("\n sensitive = %d", s);

    fc_effect.music.s = s;

}
void set_music_type(u8 ty)
{
    fc_effect.music.m_type = ty;

}
void fc_music(void)
{

/*******   4个效果的  ****/
    extern uint16_t fc_music_gradual(void);
    extern uint16_t fc_music_breath(void) ;
    extern uint16_t fc_music_static(void) ;
    extern uint16_t fc_music_twinkle(void) ;

    void *p;
    p = &fc_music_gradual;
    switch(fc_effect.music.m)
    {
        case 0: //能量
            p = &fc_music_gradual;
            break;
        case 1: //节凑
            p = &fc_music_breath;
            break;
        case 2: //频谱
            p = &fc_music_static;
            break;
        case 3://滚动
            p = &fc_music_twinkle;
            break;
    }

/****  12个效果的  *************/
# if 0
    extern uint16_t music_meteor(void);

    //频谱
    extern uint16_t music_fs(void) ;
    extern uint16_t music_fs_bc(void) ;
    extern uint16_t music_fs_green_blue(void) ;
    // 节奏
    extern uint16_t music_2_side_oc(void) ;
    extern uint16_t music_oc_2(void);
    extern uint16_t music_rainbow_flash(void);


    // 滚动
    extern uint16_t music_energy(void);
    extern uint16_t music_multi_c_flow(void) ;
    extern uint16_t music_meteor(void);


    // 能量
    extern uint16_t music_star(void) ; //七彩

    extern void set_music_s_m(u8 m);

    void *p;
    switch(fc_effect.music.m)
    {
        case 0: //能量1
            set_music_s_m(0);
            p = &music_star;
            break;
        case 1: //能量2
            set_music_s_m(1);

            p = &music_star;
            break;
        case 2: //能量3
            set_music_s_m(2);

            p = &music_star;
            break;

        case 3://节奏1
            p = &music_2_side_oc;
            break;

        case 4://节奏2
            p = &music_oc_2;
            break;

        case 5://节奏3
            p = &music_rainbow_flash;
            break;

        case 6://频谱1
            p = &music_fs;
            break;
        case 7://频谱2
            p = &music_fs_bc;
            break;
        case 8://频谱3
            p = &music_fs_green_blue;
            break;

        case 9://滚动1
            p = &music_energy;
            break;
        case 10://滚动2
            p = &music_multi_c_flow;
            break;
        case 11://滚动3
            p = &music_meteor;
            break;

    }
#endif
    WS2812FX_stop();
    WS2812FX_setSegment_colorOptions(
        0,                          //第0段
        0,fc_effect.led_num-1,                       //起始位置，结束位置
        p,              //效果
        WHITE,                      //颜色，WS2812FX_setColors设置
        100,                        //速度
        SIZE_MEDIUM|FADE_XSLOW);               //选项，这里像素点大小：3,反向/反向
    WS2812FX_start();

}

/**************************************************效果调度函数*****************************************************/

void set_fc_effect(void)
{
    printf("\n set_fc_effect");
    if(fc_effect.on_off_flag == DEVICE_ON)
    {
        switch (fc_effect.Now_state)
        {
            case IS_light_scene:
                fc_effect.w = 0;
                set_bright(100);
                ls_scene_effect();
                /* code */
                break;
            case ACT_TY_PAIR:
                printf("\n ACT_TY_PAIR");

                // 配对完成，要恢复fc_effect.Now_state
                fc_pair_effect();
                /* code */
                break;
            case ACT_CUSTOM:

                bw_speed = 0; //重新计时
                f_ = 1;
                if( fc_effect.custom_index == 1) //蓝白呼吸
                {
                    liangdu = 0;
                    custom_delay_cnt = 0;
                }
                else if( fc_effect.custom_index == 2)  //蓝白渐变
                {
                    _b = 0;
                    _w = 10000;
                }
                else if( fc_effect.custom_index == 3)  //蓝色呼吸 》 白色呼吸 》 蓝白呼吸
                {
                    step_ = 0;
                    liangdu = 0;
                    custom_delay_cnt = 0;
                }


                // custom_effect();
                /* code */
                break;
            case IS_light_music:
                fc_effect.w = 0;
                set_bright(100);
                /* code */
                fc_music();
                break;

            case IS_smear_adjust:
                printf("\n IS_smear_adjust");
                fc_smear_adjust();
                break;
            case IS_STATIC:
                static_mode();
                break;

            default:
                break;
        }

    } //if(fc_effect.on_off_flag == DEVICE_ON)
}


/*--------------------------------------API-----------------------------------*/
// 触发提示效果，白光闪烁
void run_white_tips(void)
{
    extern uint16_t white_tips(void);
    WS2812FX_setSegment_colorOptions(0,0,fc_effect.led_num-1,&white_tips,0,0,0);
    WS2812FX_start();
}

