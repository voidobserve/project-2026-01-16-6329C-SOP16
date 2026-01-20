/*
- led_strip_drive.c
- 幻彩灯带驱动
- 麦克风驱动
 */
#include "led_strip_sys.h"
#include "led_strip_drive.h"
#include "asm/adc_api.h"
#include "asm/mcpwm.h"

// MIC_OFFON MIC_ENABLE; // 0-关闭麦克风，1-开启麦克风

const struct ledc_platform_data ledc_data =
    {
        .index = 0,       // 控制器号
        .port = LEDC_PIN, // 输出引脚
        .idle_level = 1,  // 当前帧的空闲电平，0：低电平， 1：高电平
        .out_inv = 1,     // 起始电平，0：高电平开始， 1：低电平开始
        .bit_inv = 1,     // 取数据时高低位镜像，0：不镜像，1：8位镜像，2:16位镜像，3:32位镜像
        .t_unit = t_42ns, // 时间单位
        .t1h_cnt = 21,    // 1码的高电平时间 = t1h_cnt * t_unit;21*42=882
        .t1l_cnt = 8,     // 1码的低电平时间 = t1l_cnt * t_unit;7*42=294
        .t0h_cnt = 10,    // 0码的高电平时间 = t0h_cnt * t_unit;8*42=336
        .t0l_cnt = 20,    // 0码的低电平时间 = t0l_cnt * t_unit;*/30*42=1260

        .t_rest_cnt = 20000, // 复位信号时间 = t_rest_cnt * t_unit;20000*42=840000
        .cbfun = NULL,       // 中断回调函数
};

void led_state_init(void)
{
#ifdef MY_DEBUG
    printf("-----------------led_state_init--------------------");
#endif
    ledc_init(&ledc_data);
}

void led_gpio_init(void)
{

    gpio_set_die(LED_R_PIN, 1);
    gpio_direction_output(LED_R_PIN, 0);

    gpio_set_die(LED_G_PIN, 1);
    gpio_direction_output(LED_G_PIN, 0);

    gpio_set_die(LED_B_PIN, 1);
    gpio_direction_output(LED_B_PIN, 0);

    gpio_set_die(LED_W_PIN, 1);
    gpio_direction_output(LED_W_PIN, 0);
}

/**************************  风扇IO口初始化   ***********/
void fan_gpio_init(void)
{
    gpio_set_pull_down(FAN_CTL_PIN, 0);
    gpio_set_pull_up(FAN_CTL_PIN, 0);
    gpio_direction_output(FAN_CTL_PIN, 1);
}

void open_fan(void)
{
    gpio_direction_output(FAN_CTL_PIN, 1);
}

void close_fan(void)
{
    gpio_direction_output(FAN_CTL_PIN, 0);
}

void led_pwm_init(void)
{

#if 1
    // R
    struct pwm_platform_data pwm_p_data;
    pwm_p_data.pwm_aligned_mode = pwm_edge_aligned; // 边沿对齐
    pwm_p_data.pwm_ch_num = pwm_ch0;                // 通道号
    pwm_p_data.frequency = 1000;                    // 1KHz
    pwm_p_data.duty = 0;                            // 上电输出0%占空比
    pwm_p_data.h_pin = LED_R_PIN;                   // 任意引脚
    pwm_p_data.l_pin = -1;                          // 任意引脚,不需要就填-1
    pwm_p_data.complementary_en = 0;                // 两个引脚的波形, 0: 同步,  1: 互补，互补波形的占空比体现在H引脚上
    mcpwm_init(&pwm_p_data);
    // G
    pwm_p_data.pwm_aligned_mode = pwm_edge_aligned; // 边沿对齐
    pwm_p_data.pwm_ch_num = pwm_ch1;                // 通道号
    pwm_p_data.frequency = 1000;                    // 1KHz
    pwm_p_data.duty = 0;                            // 上电输出0%占空比
    pwm_p_data.h_pin = LED_G_PIN;                   // 任意引脚
    pwm_p_data.l_pin = -1;                          // 任意引脚,不需要就填-1
    pwm_p_data.complementary_en = 0;                // 两个引脚的波形, 0: 同步,  1: 互补，互补波形的占空比体现在H引脚上
    mcpwm_init(&pwm_p_data);
    // B
    pwm_p_data.pwm_aligned_mode = pwm_edge_aligned; // 边沿对齐
    pwm_p_data.pwm_ch_num = pwm_ch2;                // 通道号
    pwm_p_data.frequency = 1000;                    // 1KHz
    pwm_p_data.duty = 0;                            // 上电输出0%占空比
    pwm_p_data.h_pin = LED_B_PIN;                   // 任意引脚
    pwm_p_data.l_pin = -1;                          // 任意引脚,不需要就填-1
    pwm_p_data.complementary_en = 0;                // 两个引脚的波形, 0: 同步,  1: 互补，互补波形的占空比体现在H引脚上
    mcpwm_init(&pwm_p_data);
    // W
    pwm_p_data.pwm_aligned_mode = pwm_edge_aligned; // 边沿对齐
    pwm_p_data.pwm_ch_num = pwm_ch3;                // 通道号
    pwm_p_data.frequency = 1000;                    // 1KHz
    pwm_p_data.duty = 0;                            // 上电输出0%占空比
    pwm_p_data.h_pin = LED_W_PIN;                   // 任意引脚
    pwm_p_data.l_pin = -1;                          // 任意引脚,不需要就填-1
    pwm_p_data.complementary_en = 0;                // 两个引脚的波形, 0: 同步,  1: 互补，互补波形的占空比体现在H引脚上
    mcpwm_init(&pwm_p_data);

#endif
}

#include "led_strand_effect.h"
extern u8 pwr_on_effect_f1;
u8 temp_w_bright = 0;
void fc_driver(u8 r, u8 g, u8 b)
{
    u32 duty1, duty2, duty3;
    duty1 = r * 10000 / 255; // 占空比转为 0 ~ 10000
    duty2 = b * 10000 / 255; // 占空比转为 0 ~ 10000
    duty3 = g * 10000 / 255; // 占空比转为 0 ~ 10000
    // printf("\n duty1=%d",duty1);

    if (fc_effect.w == 0)
    {
        mcpwm_set_duty(pwm_ch0, duty1);
        mcpwm_set_duty(pwm_ch1, duty2);
        mcpwm_set_duty(pwm_ch2, duty3);
        mcpwm_set_duty(pwm_ch3, 0);
    }
    else
    {
        mcpwm_set_duty(pwm_ch0, 0);
        mcpwm_set_duty(pwm_ch1, 0);
        mcpwm_set_duty(pwm_ch2, 0);

        if (fc_effect.on_off_flag == DEVICE_ON)
        {

            if (!pwr_on_effect_f1)
            {
                mcpwm_set_duty(pwm_ch3, fc_effect.w * 10000 / 255 * fc_effect.b / 255);
                temp_w_bright = fc_effect.b;
            }
        }
    }
}

void tow_led_drive(u16 b, u16 w, u16 bright)
{
    u32 duty2, duty3;

    duty2 = b * bright / 10000;
    duty3 = w * bright / 10000;

    if (get_on_off_state())
    {
        mcpwm_set_duty(pwm_ch0, duty2); //
        mcpwm_set_duty(pwm_ch1, 0);     //
        mcpwm_set_duty(pwm_ch2, 0);     //
        mcpwm_set_duty(pwm_ch3, duty3); // W
    }
    else
    {
        mcpwm_set_duty(pwm_ch0, 0); //
        mcpwm_set_duty(pwm_ch1, 0); //
        mcpwm_set_duty(pwm_ch2, 0); //
        mcpwm_set_duty(pwm_ch3, 0); // W
    }
}

/*********************************mic脚IO口初始化***************************************************************/

int mic_adc_init(void)
{
    adc_add_sample_ch(AD_CH_PA8); // 注意：初始化AD_KEY之前，先初始化ADC
    gpio_set_die(IO_PORTA_08, 0);
    gpio_set_direction(IO_PORTA_08, 1);
    gpio_set_pull_down(IO_PORTA_08, 0);

    return 0;
}

u16 check_mic_adc(void)
{
    return adc_get_value(AD_CH_PA8);
}

#define MAX_SOUND 10

u32 sound_v; // 平均值
u8 sound_cnt = 0;

typedef struct
{
    int buf[MAX_SOUND];
    int v;        // 平均值
    int c_v;      // 当前值
    u8 valid;     // 数据有效
    u8 sensitive; // 灵敏度 0~100
} sound_t;

sound_t sound =
    {
        .c_v = 0,
        .v = 0,
        .valid = 0,
        .sensitive = 20,
};

void check_mic_sound(void)
{
    u8 i;
    sound.buf[sound_cnt] = check_mic_adc();
    sound.c_v = sound.buf[sound_cnt]; // 记录当前值
    sound_cnt++;
    if (sound_cnt > (MAX_SOUND - 1) || sound.valid == 1)
    {
        sound_cnt = 0;
        sound.valid = 1;
        sound.v = 0;
        for (i = 0; i < MAX_SOUND; i++)
        {
            sound.v += sound.buf[i];
        }
        sound.v = sound.v / MAX_SOUND; // 计算平均值
    }
}

void set_sensitive(u8 s)
{
    sound.sensitive = s;
    printf("\n sound.sensitive=%d", sound.sensitive);
}
// 获取声控结果
// 触发条件：（（当前声音大小 - 平均值）* 100 ）/ 平均值 > 灵敏度（0~100）
// 0:没触发
// 1:触发
u8 get_sound_result(void)
{

    if (sound.valid == 1)
    {
        if (sound.v > sound.c_v)
        {

            // if( (sound.v - sound.c_v) * 100 / sound.v > sound.sensitive)
            if ((sound.v - sound.c_v) > sound.sensitive * 10)
            {
                // printf("\n sound.v =%d",sound.v);
                // printf("\n sound.c_v =%d",sound.c_v);

                return 1;
            }
        }
    }
    else
    {
        return 0;
    }
    return 0;
}
