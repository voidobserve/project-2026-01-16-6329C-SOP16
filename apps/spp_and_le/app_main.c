/*********************************************************************************************
    *   Filename        : app_main.c

    *   Description     :

    *   Copyright:(c)JIELI  2011-2019  @ , All Rights Reserved.
*********************************************************************************************/
#include "system/includes.h"
#include "app_config.h"
#include "app_action.h"
#include "app_main.h"
#include "update.h"
#include "update_loader_download.h"
#include "app_charge.h"
#include "app_power_manage.h"
#include "asm/charge.h"
#include "rf433.h"
#include "led_strip_sys.h"
#include "led_strand_effect.h"
#if TCFG_KWS_VOICE_RECOGNITION_ENABLE
#include "jl_kws/jl_kws_api.h"
#endif /* #if TCFG_KWS_VOICE_RECOGNITION_ENABLE */
#include "one_wire.h"

#define LOG_TAG_CONST APP
#define LOG_TAG "[APP]"
#define LOG_ERROR_ENABLE
#define LOG_DEBUG_ENABLE
#define LOG_INFO_ENABLE
/* #define LOG_DUMP_ENABLE */
#define LOG_CLI_ENABLE
#include "debug.h"

/*任务列表 */
const struct task_info task_info_table[] = {
    {"app_core", 1, 0, 640, 128},
    {"sys_event", 7, 0, 256, 0},
    {"btctrler", 4, 0, 512, 256},
    {"btencry", 1, 0, 512, 128},
    {"btstack", 3, 0, 768, 256},
    {"systimer", 7, 0, 128, 0},
    {"update", 1, 0, 512, 0},
    {"dw_update", 2, 0, 256, 128},
#if (RCSP_BTMATE_EN)
    {"rcsp_task", 2, 0, 640, 0},
#endif
#if (USER_UART_UPDATE_ENABLE)
    {"uart_update", 1, 0, 256, 128},
#endif
#if (XM_MMA_EN)
    {"xm_mma", 2, 0, 640, 256},
#endif
    {"usb_msd", 1, 0, 512, 128},
#if TCFG_AUDIO_ENABLE
    {"audio_dec", 3, 0, 768, 128},
    {"audio_enc", 4, 0, 512, 128},
#endif /*TCFG_AUDIO_ENABLE*/
#if TCFG_KWS_VOICE_RECOGNITION_ENABLE
    {"kws", 2, 0, 256, 64},
#endif /* #if TCFG_KWS_VOICE_RECOGNITION_ENABLE */
#if (TUYA_DEMO_EN)
    {"user_deal", 7, 0, 512, 512}, // 定义线程 tuya任务调度
#endif
    {"led_task", 2, 0, 512, 512},
    {0, 0},
};

APP_VAR app_var;

void app_var_init(void)
{
    app_var.play_poweron_tone = 1;

    app_var.auto_off_time = TCFG_AUTO_SHUT_DOWN_TIME;
    app_var.warning_tone_v = 340;
    app_var.poweroff_tone_v = 330;
}

__attribute__((weak))
u8
get_charge_online_flag(void)
{
    return 0;
}

void clr_wdt(void);
void check_power_on_key(void)
{
#if TCFG_POWER_ON_NEED_KEY

    u32 delay_10ms_cnt = 0;
    while (1)
    {
        clr_wdt();
        os_time_dly(1);

        extern u8 get_power_on_status(void);
        if (get_power_on_status())
        {
            log_info("+");
            delay_10ms_cnt++;
            if (delay_10ms_cnt > 70)
            {
                /* extern void set_key_poweron_flag(u8 flag); */
                /* set_key_poweron_flag(1); */
                return;
            }
        }
        else
        {
            log_info("-");
            delay_10ms_cnt = 0;
            log_info("enter softpoweroff\n");
            power_set_soft_poweroff();
        }
    }
#endif
}

void app_main()
{
    struct intent it;

    if (!UPDATE_SUPPORT_DEV_IS_NULL())
    {
        int update = 0;
        update = update_result_deal();
    }

    printf(">>>>>>>>>>>>>>>>>app_main...\n");

    if (get_charge_online_flag())
    {
#if (TCFG_SYS_LVD_EN == 1)
        vbat_check_init();
#endif
    }
    else
    {
        check_power_on_voltage();
    }

#if TCFG_POWER_ON_NEED_KEY
    check_power_on_key();
#endif

#if TCFG_AUDIO_ENABLE
    extern int audio_dec_init();
    extern int audio_enc_init();
    audio_dec_init();
    audio_enc_init();
#endif /*TCFG_AUDIO_ENABLE*/

#if TCFG_KWS_VOICE_RECOGNITION_ENABLE
    jl_kws_main_user_demo();
#endif /* #if TCFG_KWS_VOICE_RECOGNITION_ENABLE */

    init_intent(&it);

#if CONFIG_APP_SPP_LE
    it.name = "spp_le";
    it.action = ACTION_SPPLE_MAIN;

#elif CONFIG_APP_AT_COM || CONFIG_APP_AT_CHAR_COM
    it.name = "at_com";
    it.action = ACTION_AT_COM;

#elif CONFIG_APP_DONGLE
    it.name = "dongle";
    it.action = ACTION_DONGLE_MAIN;

#elif CONFIG_APP_MULTI
    it.name = "multi_conn";
    it.action = ACTION_MULTI_MAIN;

#elif CONFIG_APP_NONCONN_24G
    it.name = "nonconn_24g";
    it.action = ACTION_NOCONN_24G_MAIN;

#elif CONFIG_APP_LL_SYNC
    it.name = "ll_sync";
    it.action = ACTION_LL_SYNC;

#elif CONFIG_APP_TUYA
    it.name = "tuya";
    it.action = ACTION_TUYA;

#elif CONFIG_APP_CENTRAL
    it.name = "central";
    it.action = ACTION_CENTRAL_MAIN;

#elif CONFIG_APP_DONGLE
    it.name = "dongle";
    it.action = ACTION_DONGLE_MAIN;

#elif CONFIG_APP_BEACON
    it.name = "beacon";
    it.action = ACTION_BEACON_MAIN;

#elif CONFIG_APP_IDLE
    it.name = "idle";
    it.action = ACTION_IDLE_MAIN;

#elif CONFIG_APP_CONN_24G
    it.name = "conn_24g";
    it.action = ACTION_CONN_24G_MAIN;

#else
    while (1)
    {
        printf("no app!!!");
    }
#endif

    log_info("run app>>> %s", it.name);
    log_info("%s,%s", __DATE__, __TIME__);

    start_app(&it);

#if TCFG_CHARGE_ENABLE
    set_charge_event_flag(1);
#endif
}

/*
 * app模式切换
 */
void app_switch(const char *name, int action)
{
    struct intent it;
    struct application *app;

    log_info("app_exit\n");

    init_intent(&it);
    app = get_current_app();
    if (app)
    {
        /*
         * 退出当前app, 会执行state_machine()函数中APP_STA_STOP 和 APP_STA_DESTORY
         */
        it.name = app->name;
        it.action = ACTION_BACK;
        start_app(&it);
    }

    /*
     * 切换到app (name)并执行action分支
     */
    it.name = name;
    it.action = action;
    start_app(&it);
}

int eSystemConfirmStopStatus(void)
{
    /* 系统进入在未来时间里，无任务超时唤醒，可根据用户选择系统停止，或者系统定时唤醒(100ms) */
    // 1:Endless Sleep
    // 0:100 ms wakeup
    /* log_info("100ms wakeup"); */
    return 1;
}

__attribute__((used)) int *__errno()
{
    static int err;
    return &err;
}

/****************************************************************************************
**名称:定时器设置，定时50us中断
**功能:
**说明:
**备注:
**日期:
*****************************************************************************************/
static const u16 timer_div[] = {
    /*0000*/ 1,
    /*0001*/ 4,
    /*0010*/ 16,
    /*0011*/ 64,
    /*0100*/ 2,
    /*0101*/ 8,
    /*0110*/ 32,
    /*0111*/ 128,
    /*1000*/ 256,
    /*1001*/ 4 * 256,
    /*1010*/ 16 * 256,
    /*1011*/ 64 * 256,
    /*1100*/ 2 * 256,
    /*1101*/ 8 * 256,
    /*1110*/ 32 * 256,
    /*1111*/ 128 * 256,
};

#define APP_TIMER_CLK (24000000) // clk_get("timer")
#define MAX_TIME_CNT 0x7fff
#define MIN_TIME_CNT 0x100
#define TIMER_UNIT 1

#define TIMER_CON JL_TIMER2->CON
#define TIMER_CNT JL_TIMER2->CNT
#define TIMER_PRD JL_TIMER2->PRD
#define TIMER_VETOR IRQ_TIME2_IDX

___interrupt
    AT_VOLATILE_RAM_CODE void
    user_timer_isr(void) // 125us
{

    TIMER_CON |= BIT(14);

    extern void one_wire_send(void);
    one_wire_send(); // steomotor

    // #if TCFG_RF433GKEY_ENABLE
    // extern void timer125us_hook(void);
    // timer125us_hook();
    // #endif
}

void user_timer_init(void)
{
    u32 prd_cnt;
    u8 index;

    //	printf("********* user_timer_init **********\n");
    for (index = 0; index < (sizeof(timer_div) / sizeof(timer_div[0])); index++)
    {
        prd_cnt = TIMER_UNIT * (APP_TIMER_CLK / 8000) / timer_div[index];
        if (prd_cnt > MIN_TIME_CNT && prd_cnt < MAX_TIME_CNT)
        {
            break;
        }
    }

    TIMER_CNT = 0;
    TIMER_PRD = prd_cnt;
    request_irq(TIMER_VETOR, 3, user_timer_isr, 0);
    TIMER_CON = (0b0001 << 10) | (index << 4) | (0x01 << 0); // 选择晶振作为时钟源，分频系数，定时器计数模式
}
__initcall(user_timer_init);

extern u16 check_mic_adc(void);
u8 music_trigger = 0;
// u32 adc,adc_av;
#define SAMPLE_N 20
u8 adc_v_n, adc_avrg_n, adc_total_n;
u32 adc_sum = 0, adc_sum_n = 0;
extern uint8_t met_trg;
extern uint8_t trg_en;
extern void set_music_oc_trg(u8 p);
u8 i, j;
u32 adc, adc_av, adc_all;
u16 adc_v[SAMPLE_N]; // 记录20个ADC值
u32 adc_avrg[10];    // 记录5个平均值
u32 adc_total[15];   // __attribute__((aligned(4)));

// 声控
void sound_handle(void)
{
    extern u32 adc_get_value(u32 ch);
    extern void WS2812FX_trigger();
    u16 adc;
    u8 i, trg, trg_v;
    u32 adc_all, adc_ttl;

    extern u32 adc_sample(u32 ch);
    // 记录adc值
    adc = adc_get_value(AD_CH_PA8);

    // adc = adc_sample(AD_CH_PA8);
    // printf("adc = %d", adc);
    if (adc < 1000) // 当ADC值大于1000，说明硬件电路有问题
    {

        if (adc_sum_n < 2000)
        {
            adc_sum_n++;
        }
        if (adc_sum_n == 2000)
        {
            if (adc / (adc_sum / adc_sum_n) > 3)
                return; // adc突变，大于平均值的3倍，丢弃改值
            adc_sum = adc_sum - adc_sum / adc_sum_n;
        }
        adc_sum += adc;

        adc_v_n %= SAMPLE_N;
        adc_v[adc_v_n] = adc;
        adc_v_n++;
        adc_all = 0;
        for (i = 0; i < SAMPLE_N; i++)
        {
            adc_all += adc_v[i];
        }

        adc_avrg_n %= 10;
        adc_avrg[adc_avrg_n] = adc_all / SAMPLE_N;
        adc_avrg_n++;
        // printf("%d,",adc_all / SAMPLE_N);
        adc_ttl = 0;
        for (i = 0; i < 10; i++)
        {
            adc_ttl += adc_avrg[i];
        }
        memmove((u8 *)adc_total, (u8 *)adc_total + 4, 14 * 4);
        adc_total[14] = adc_ttl / 10; // 总数平均值

        // 查找峰值
        trg = 0;
        // if( adc_total[7] >= adc_total[6] &&
        //     adc_total[7] >= adc_total[5] &&
        //     adc_total[7] > adc_total[4] &&
        //     adc_total[7] > adc_total[3] &&
        //     adc_total[7] > adc_total[2] &&
        //     adc_total[7] > adc_total[1] &&
        //     adc_total[7] > adc_total[0] &&
        //     adc_total[7] >= adc_total[8] &&
        //     adc_total[7] >= adc_total[9] &&
        //     adc_total[7] > adc_total[10] &&
        //     adc_total[7] > adc_total[11] &&
        //     adc_total[7] > adc_total[12] &&
        //     adc_total[7] > adc_total[13] &&
        //     adc_total[7] > adc_total[14]

        //     )
        {
            if (adc_sum_n != 0)
            {
                extern void set_mss(uint16_t s);
                set_mss(adc + (adc)*fc_effect.music.s / 100);
                if (adc * fc_effect.music.s / 100 > adc_sum / adc_sum_n)
                {
                    // printf("\n adc=%d",adc);
                    // printf("\n adc_sum/adc_sum_n=%d",adc_sum/adc_sum_n);

                    // set_music_oc_trg((adc - adc_sum/adc_sum_n)*100 * fc_effect.music.s / 100/(adc_sum/adc_sum_n));

                    extern void WS2812FX_trg(void);
                    if (fc_effect.led_num < 90) // 太多点数处理不过来
                                                //  WS2812FX_trg();
                        extern void set_music_fs_trg(u8 p);
                    // set_music_fs_trg((adc - adc_sum/adc_sum_n)*100 * fc_effect.music.s / 100/(adc_sum/adc_sum_n));

                    trg = 200;
                    met_trg = 1;
                    trg_en = 1;
                    music_trigger = 1;
                    if (fc_effect.on_off_flag == DEVICE_ON && fc_effect.Now_state == IS_light_music)
                        WS2812FX_trigger();
                }

                if (adc > adc_sum / adc_sum_n)
                {
                    set_music_oc_trg((adc - adc_sum / adc_sum_n) * 100 * fc_effect.music.s / 100 / (adc_sum / adc_sum_n));
                    extern void set_music_fs_trg(u8 p);
                    set_music_fs_trg((adc - adc_sum / adc_sum_n) * 100 * fc_effect.music.s / 100 / (adc_sum / adc_sum_n));
                }
            }
        }
    }
}

extern int ct_uart_init_a(u32 baud);
extern void run_tick_per_10ms(void);
extern void WS2812FX_service();
void clr_wdt(void);
void check_mic_sound(void);
extern void WS2812FX_trigger();
extern void bw_breath_effect(void);
extern void bw_effect3(void);
extern void bw_gradual_effect(void);
extern void rf24_key_handle(void);
extern void uart_key_handle(void);
extern void power_on_effect(void);
extern void test_uart_a(void);
extern void special_w_close(void);
// 10ms调用一次
void main_while(viod)
{ 
 
    rf24_key_handle();
    power_on_effect(); // 开机慢慢亮
    special_w_close(); // w灯关机慢慢开，关灯比较特殊
    bw_breath_effect();
    bw_gradual_effect();
    bw_effect3();

    uart_key_handle();

    rf24g_long_timer();

    sound_handle();
    run_tick_per_10ms();
    WS2812FX_service();
}

// OS_SEM LED_TASK_SEM;

void my_main(void)
{
    printf("\n my_main");
    extern void fc_data_init(void);
    extern void full_color_init(void);

    extern void read_flash_device_status_init(void);
    extern int mic_adc_init(void);
    led_gpio_init(); // RGB控制脚初始化
    fan_gpio_init();
    led_pwm_init();

    user_timer_init(); // 定时器2设置
    mic_adc_init();
    // mcu_com_init(); //电机GPIO初始化

    ct_uart_init_a(9600);
    full_color_init();

    sys_s_hi_timer_add(NULL, main_while, 10);
}
