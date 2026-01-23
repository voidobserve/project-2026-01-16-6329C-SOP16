#include "system/app_core.h"
#include "system/includes.h"
#include "server/server_core.h"
#include "app_config.h"
#include "app_action.h"
#include "os/os_api.h"
#include "btcontroller_config.h"
#include "btctrler/btctrler_task.h"
#include "config/config_transport.h"
#include "btstack/avctp_user.h"
#include "btstack/btstack_task.h"
#include "bt_common.h"
#include "rcsp_bluetooth.h"
#include "rcsp_user_update.h"
#include "app_charge.h"
#include "app_chargestore.h"
#include "app_power_manage.h"
#include "app_comm_bt.h"
#include "app_main.h"

//******************************************************************************//
//
//
//
//
//******************************************************************************//
#define UART_CBUF_SIZE 0x100 // �������������С(�������ݳ���cbuf, �������ᶪʧ����)
#define UART_FRAM_SIZE 0x100 // 单次数据包最大值(每次cbuf缓存达到fram或串口收到一次数据, 就会起一次中断)

static u8 devBuffer_static[UART_CBUF_SIZE] __attribute__((aligned(4))); // dev DMA memory

const uart_bus_t *uart_bus;
extern void printf_buf(u8 *buf, u32 len);
extern void ble_send_user_data(u8 *buffer, u8 buffer_size);
u16 uart_dev_test_send_api(uint8_t *data, u16 len);

void uart_rx_handler(void)
{
    extern void co2_rx_data(u8 * pBuf);
    extern void pm25_rx_data(u8 * pBuf);

    if (uart_bus)
    {
        printf("uart_rxxxx");
        // app_var.rx_len = uart_bus->read(app_var.rx_buff, UART_FRAM_SIZE, 0);
        // if(app_var.rx_len>UART_FRAM_SIZE)
        // {
        // 	app_var.rx_len = 0;
        // 	printf("uart overflow\n");
        // }
        // //uart_bus->write("ssss",4);
        // co2_rx_data(app_var.rx_buff);
        // pm25_rx_data(app_var.rx_buff);
    }
}

static void user_uart_isr_cb(void *ut_bus, u32 status)
{
    struct sys_event e;
    printf("\n $");
    if (status == UT_RX || status == UT_RX_OT)
    {
#if FLOW_CONTROL
        uart1_flow_ctl_rts_suspend();
#endif
        e.type = SYS_DEVICE_EVENT;
        e.arg = (void *)DEVICE_EVENT_FROM_CI_UART;
        e.u.dev.event = 0;
        e.u.dev.value = 0;
        sys_event_notify(&e);
    }
}

int ct_uart_init(u32 baud)
{
    struct uart_platform_data_t u_arg = {0};
    u_arg.tx_pin = IO_PORT_DM; // IO_PORT_DP;
    u_arg.rx_pin = IO_PORT_DP;
    // u_arg.tx_pin = IO_PORTB_06;
    // u_arg.rx_pin = IO_PORTB_07;
    u_arg.rx_cbuf = devBuffer_static;
    u_arg.rx_cbuf_size = UART_CBUF_SIZE; //>=
    u_arg.frame_length = UART_FRAM_SIZE; // Э�����ݰ�
    u_arg.rx_timeout = 5;                // ms
    u_arg.isr_cbfun = user_uart_isr_cb;
    u_arg.baud = baud;
    u_arg.is_9bit = 0;

    uart_bus = uart_dev_open(&u_arg);
    gpio_set_pull_up(IO_PORT_DP, 1);
    gpio_set_pull_down(IO_PORT_DP, 0);
    if (uart_bus != NULL)
    {
        printf("uart_dev_open() success\n");
        return 1;
    }
    return 0;
}

void uart_close(void)
{
    extern u32 uart_dev_close(uart_bus_t * ut);
    uart_dev_close(uart_bus);
}

void tuya_UT1_putbyte(char a)
{
    if (JL_UART1->CON0 & BIT(0))
    {
        JL_UART1->BUF = a;
        __asm__ volatile("csync");
        while ((JL_UART1->CON0 & BIT(15)) == 0)
            ;
        JL_UART1->CON0 |= BIT(13);
    }
}

/*
u16 uart_dev_test_send_api(uint8_t *data, u16 len)
{
    int i = 0;
    while (len--) {
        tuya_UT1_putbyte(data[i++]);
    }
    return len;
}
*/

u16 uart_dev_test_send_api(uint8_t *data, u16 len)
{
    uart_bus->write(data, len);
    return len;
}

bool uart_dev_test_init_api(void)
{
    bool ret;

    ret = ct_uart_init(115200);
    return ret;
}
