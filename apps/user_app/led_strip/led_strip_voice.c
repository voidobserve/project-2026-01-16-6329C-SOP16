#include "led_strip_voice.h"
#include "led_strand_effect.h"
 

static volatile u8 flag_sound_triggered_in_motor = 0; // 标志位，电机声控模式下，触发声控，0--未触发，1--触发
static volatile u8 flag_sound_triggered_in_colorful_light = 0;

// 电机声控模式下，获取声控结果
u8 get_sound_triggered_by_motor(void)
{
    return flag_sound_triggered_in_motor;
}

void clear_sound_triggered_by_motor(void)
{
    flag_sound_triggered_in_motor = 0;
}

void set_sound_triggered_by_motor(void)
{
    flag_sound_triggered_in_motor = 1;
}

// 七彩灯声控模式下，获取声控结果
u8 get_sound_triggered_by_colorful_light(void)
{
    return flag_sound_triggered_in_colorful_light;
}

void clear_sound_triggered_by_colorful_light(void)
{
    flag_sound_triggered_in_colorful_light = 0;
}

void set_sound_triggered_by_colorful_light(void)
{
    flag_sound_triggered_in_colorful_light = 1;
}

void sound_handle(void)
{

#if 1 // 移植其他项目的声控程序

    if (fc_effect.on_off_flag != DEVICE_ON)
    {
        return;
    }

#define SAMPLE_N 20
    static volatile u32 adc_sum = 0;
    static volatile u32 adc_sum_n = 0;
    static volatile u8 adc_v_n = 0;
    static volatile u8 adc_avrg_n = 0;
    static volatile u16 adc_v[SAMPLE_N] = {0};
    static volatile u32 adc_avrg[10] = {0}; // 记录5个平均值
    static volatile u32 adc_total[15] = {0};
    // u8 trg = 0;
    u8 trg_v = 0;
    volatile u16 adc = 0;
    u32 adc_all = 0;
    u32 adc_ttl = 0;

    // 记录adc值
    adc = check_mic_adc(); // 每次进入，采集一次ad值（即使不在声控模式，也会占用一些时间）

    // printf("adc = %d", adc);

    if (adc >= 1000)
    {
        return;
    }

    // if (adc < 1000) // 当ADC值大于1000，说明硬件电路有问题
    if (adc_sum_n < 2000)
    {
        // 从0开始，一直加到2000，每10ms加一，总共要20s
        adc_sum_n++;
    }

    if (adc_sum_n == 2000)
    {
        if (adc / (adc_sum / adc_sum_n) > 3)
            return; // adc突变，大于平均值的3倍，丢弃改值

        adc_sum = adc_sum - adc_sum / adc_sum_n;
    }

    adc_sum += adc; // 累加adc值

    adc_v_n %= SAMPLE_N;
    adc_v[adc_v_n] = adc;
    adc_v_n++;
    adc_all = 0;

    // 计算ad值总和
    for (u8 i = 0; i < SAMPLE_N; i++)
    {
        adc_all += adc_v[i];
    }

    // 获取ad值平均值
    adc_avrg_n %= 10;
    adc_avrg[adc_avrg_n] = adc_all / SAMPLE_N;
    adc_avrg_n++;
    adc_ttl = 0;

    // 在平均值的基础上，再求总和
    for (u8 i = 0; i < 10; i++)
    {
        adc_ttl += adc_avrg[i];
    }

    memmove((u8 *)adc_total, (u8 *)adc_total + 4, 14 * 4); // 将 src 指向的内存区域中的前 n 个字节复制到 dest 指向的内存区域（能够安全地处理内存重叠的情况）

    adc_total[14] = adc_ttl / 10; // 总数平均值
    // trg = 0;

    if (adc_sum_n != 0)
    {
        if (adc * fc_effect.music.s / 100 > adc_sum / adc_sum_n)
        {
            if (fc_effect.on_off_flag == DEVICE_ON &&
                fc_effect.Now_state == IS_light_music)
            {
                set_sound_triggered_by_colorful_light();

                /*
                    让主循环扫描到立刻更新动画
                    注意：只能在只接了七彩灯的时候这么用，如果同时接了七彩灯和流星灯，
                    但是流星灯不在声控模式，会导致流星灯的动画提前跳到下一步
                    如果要在接了七彩灯和流星灯的设备上都使用，需要修改主循环的WS2812FX_service(),
                    在内部判断相应的标志位
                */
                WS2812FX_trigger();
            }
        }

        if (adc * fc_effect.music.s / 100 > adc_sum / adc_sum_n)
        {
            if (fc_effect.base_ins.motor_on_off == DEVICE_ON &&
                5 == fc_effect.base_ins.mode)
            {
                // 如果电机已经开启,并且电机处于声控模式
                set_sound_triggered_by_motor();
            }
        }
    }

#endif // 移植其他项目的声控程序
}
