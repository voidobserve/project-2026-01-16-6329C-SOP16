
#ifndef led_strip_drive_h
#define led_strip_drive_h

#include "board_ac632n_demo_cfg.h"
#include "asm/ledc.h"
#include "asm/gpio.h"
#define MIC_PIN     IO_PORTA_08
#define MIC_AD_CHANNEL AD_CH_PA8
#define LEDC_PIN    IO_PORTB_07

#define FAN_CTL_PIN IO_PORT_DM // 风扇控制脚

#define LED_R_PIN IO_PORTA_00 // 驱动 红色 分量的引脚
#define LED_G_PIN IO_PORTA_01 // 驱动 绿色 分量的引脚
#define LED_B_PIN IO_PORTB_07 // 驱动 蓝色 分量的引脚
#define LED_W_PIN IO_PORTA_02 // 驱动 白色 分量的引脚

typedef enum
{
    OFF,    //mic关闭
    ON,     //mic打开
}MIC_OFFON;

extern MIC_OFFON MIC_ENABLE;

void led_state_init(void);

u16 check_mic_adc(void);
void ledc_init(const struct ledc_platform_data *arg); 

void fc_rgbw_driver(u8 r, u8 g, u8 b, u8 w);

#endif







