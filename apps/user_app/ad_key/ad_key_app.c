#include "board_ac632n_demo_cfg.h"
#include "event.h"
#include "key_driver.h"
#include "led_strip_sys.h"
#include "ir_key_app.h"
#include "led_strand_effect.h"
#include "tuya_ble_type.h"
#include "system/includes.h"

#if TCFG_ADKEY_ENABLE
// ---------------------------------------------定义AD_KEY键值
// 要和board_ac632n_demo.c的定义：adkey_platform_data adkey_data[] 匹配
typedef enum
{
    AD_KEY_PWR = 0
}ad_key_value_e;


void ad_key_handle(struct sys_event *event)
{
    u8 event_type = 0;
    u8 key_value = 0;
    static u16 timeout = 0, pw_cnt = 0;;
    event_type = event->u.key.event;
    key_value = event->u.key.value;
    printf("\n key_value=%d",key_value);
    extern u8 pwr_state;
    if(event_type == KEY_EVENT_HOLD && key_value == AD_KEY_PWR && pwr_state == 1) //长按5秒，设备主动解绑
    {
        timeout+=300;
        if(timeout > 7000)
        {
            timeout = 0;
            extern void ty_start_pair(void);
            if(tuya_ble_connect_status_get() != UNBONDING_UNCONN)
            {
                tuya_ble_device_unbind();
            }
            
            printf("device unbind sucess");
            os_time_dly(30);
            ty_start_pair();
        }
    }

    if(event_type == KEY_EVENT_DOUBLE_CLICK && key_value == AD_KEY_PWR ) 
    {
        printf("\n power off");
        soft_turn_on_the_light();
        extern void set_power_off(void);
        set_power_off();
        
    }


}//end

#endif


