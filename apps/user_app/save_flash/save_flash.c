
#include "system/includes.h"
#include "syscfg_id.h"
#include "save_flash.h"
// #include "rf433.h"

#define CFG_USER_LED_LEDGTH_DATA 3

/*
    大致功能：
    需要写入flash时，调用：
    os_taskq_post("msg_task", 1, MSG_USER_SAVE_INFO);

    用户消息处理线程：
    save_user_data_enable() 使能延时写入flash的操作

    主循环：
    save_user_data_time_count_down() 倒计时
    save_user_data_handle() 延时写入flash的操作使能，并且倒计时到来，执行写入flash操作
*/
#define FLASH_CRC_DATA 0xC5
static volatile u16 time_count_down = 0;          // 存放当前的倒计时
static volatile u8 flag_is_enable_count_down = 0; // 标志位，是否使能了保存，进入倒计时
static volatile u8 flag_is_enable_to_save = 0;    // 标志位，是否使能保存，从而进行写flash操作

// u8 ble_state; // 默认开启BLE模块

void read_flash_device_status_init(void)
{

    int ret = 0;
    save_flash_t save_flash3;

    memset((u8 *)&save_flash3, 0, sizeof(save_flash_t));

    ret = syscfg_read(CFG_USER_LED_LEDGTH_DATA, (u8 *)(&save_flash3), sizeof(save_flash_t));

    os_time_dly(1);

    if (save_flash3.header != FLASH_CRC_DATA) // 第一次上电
    {
        fc_data_init();
        // ble_state = 1; // 默认开启BLE模块
    }
    else
    {
        printf("\n flash_size=%d", sizeof(fc_effect_t));
        memcpy((u8 *)(&fc_effect), (u8 *)(&save_flash3.fc_save), sizeof(fc_effect_t));

        // ble_state = save_flash3.ble_state;
    }
}

// 把用户数据写到区域3
void save_user_data_area3(void)
{
    int ret = 0;

    save_flash_t save_data;
    save_data.header = FLASH_CRC_DATA;
    // save_data.ble_state = ble_state;
    memcpy((u8 *)(&save_data.fc_save), (u8 *)(&fc_effect), sizeof(fc_effect_t));
    os_time_dly(1); // 让出CPU，防止后续写flash时，占用时间过长，导致芯片复位
    ret = syscfg_write(CFG_USER_LED_LEDGTH_DATA, (u8 *)(&save_data), sizeof(save_flash_t));
    if (ret != sizeof(save_flash_t))
    {
        // 如果实际写入的数据与配置的参数不一致
    }

    printf("save user data \n");
}

/**
 * @brief 写入flash倒计时
 *      10ms调用一次，不需要特别准确
 *
 *      如果 flag_is_enable_count_down == 1，表示使能倒计时
 *      如果 flag_is_enable_count_down == 0，表示未使能倒计时
 *
 *      计时结束，将 flag_is_enable_to_save 置一
 */
void save_user_data_time_count_down(void)
{
    if (0 == flag_is_enable_count_down)
    {
        return;
    }

    if (time_count_down > 0)
    {
        time_count_down--;
    }

    if (0 == time_count_down)
    {
        flag_is_enable_count_down = 0;
        flag_is_enable_to_save = 1;
        // save_user_data_area3();
    }
}

/**
 * @brief 使能延时写入flash操作
 *       由用户消息处理线程调用
 *      
 */
void save_user_data_enable(void)
{
    flag_is_enable_count_down = 0;
    time_count_down = DELAY_SAVE_FLASH_TIMES / 10; // DELAY_SAVE_FLASH_TIMES / 10 ms计时，实现 DELAY_SAVE_FLASH_TIMES ms延时
    flag_is_enable_count_down = 1;
}

/**
 * @brief 延时写入flash操作
 *       由主循环调用
 * 
 */
void save_user_data_handle(void)
{
    if (flag_is_enable_to_save)
    {
        flag_is_enable_to_save = 0;
        save_user_data_area3();
    }
}
