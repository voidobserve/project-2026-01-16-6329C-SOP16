// @ ir_key.c
/* ir_key的底层驱动 */
#include "system/includes.h"
#include "app_config.h"
#include "app_action.h"
#include "app_main.h"
#include "update.h"
#include "update_loader_download.h"
//#include "app_charge.h"
#include "app_power_manage.h"
#include "system/includes.h"
#include "system/event.h"
#include "asm/mcpwm.h"
//#include "le_trans_data.h"
#include "btstack/bluetooth.h"
#include "irkey.h"
#include "ir_key_app.h"

#if TCFG_IRKEY_ENABLE
//按键驱动扫描参数列表
struct key_driver_para irkey_scan_para = {
    .scan_time 	  	  = 10,				//按键扫描频率, 单位: ms
    .last_key 		  = NO_KEY,  		//上一次get_value按键值, 初始化为NO_KEY;
    .filter_time  	  = 1,				//按键消抖延时;
    .long_time 		  = 75,  			//按键判定长按数量
    .hold_time 		  = (75 + 15),  	//按键判定HOLD数量
    .click_delay_time = 2,//20,				//按键被抬起后等待连击延时数量
    .key_type		  = KEY_DRIVER_TYPE_IR,
    .get_value 		  = ir_get_key_value,
};

typedef struct _IR_KEY {
    u32 ir_addr;
	u16 ir_code;
	bool ir_flag;
	u32 ir_timeout;
}IR_KEY;

static IR_KEY ir_key;

//___interrupt
AT_VOLATILE_RAM_CODE
void ir_detect_isr(void)
{
	static bool last_status = 1,ir_data_bit;
	static u8 soft_ir_cnt = 0, time_cnt = 250;
	static u32 ir_data_temp;
	bool new_status;

	if(gpio_read(TCFG_IRKEY_PORT))
	{
		new_status = 1;
	}
	else
	{
		new_status = 0;
	}


	if(new_status!=last_status)
	{
		last_status = new_status;
		if(new_status==0)
		{
			//if((time_cnt>100)&&(time_cnt<250))
			//printf("%d\n",time_cnt);
			if(((time_cnt>100)&&(time_cnt<150))//13.5ms 红外头 //11.25ms 连发码
			// ||((time_cnt>112)&&(time_cnt<123))
			)
			{
				soft_ir_cnt = 0;
				time_cnt = 1;
				ir_data_temp = 0;
//				printf("-");
				ir_key.ir_timeout = 0;
				return;
			}
			else if((time_cnt>8)&&(time_cnt<15))// 1.125ms	 bit0
			{
				ir_data_bit = 0;
				//printf("0");
			}
			else if((time_cnt>18)&&(time_cnt<30))// 2.25ms	 bit1
			{
				ir_data_bit = 1;
				//printf("1");
			}
			else
			{
				time_cnt =1;
				return;
			}
			time_cnt = 1;

			ir_data_temp >>= 1;
			soft_ir_cnt++;

			if(ir_data_bit)
			{
				ir_data_temp |= 0x80000000;
				//printf("1");
			}
			else
			{
				//printf("0");
			}

//			printf("soft_ir_cnt = %d\n",soft_ir_cnt);
			if (soft_ir_cnt == 32)
			{
				ir_key.ir_addr= ir_data_temp&0xffff;
				ir_key.ir_code= (ir_data_temp>>16)&0xff;
				ir_key.ir_flag = 1;
				printf("code = %x\n",ir_key.ir_code);
				// printf("%02x ",ir_key.ir_addr);
			}
		}
	}
	else
	{
		if(time_cnt<250)
		{
			time_cnt++;
		}
	}
}

void user_ir_init(u8 port)
{
	gpio_set_die(port, 1);
	gpio_set_direction(port, 1);
	gpio_set_pull_up(port, 1);
}

/*----------------------------------------------------------------------------*/
/**@brief   ir按键初始化
   @param   void
   @param   void
   @return  void
   @note    void ir_key_init(void)
*/
/*----------------------------------------------------------------------------*/
extern const struct irkey_platform_data irkey_data;

int irkey_init(const struct irkey_platform_data *irkey_data)
{
    printf("------------irkey_init---------------");
	user_ir_init(irkey_data->port);
    return 0;
}
const u8 ir_tbl_FF00[][2] =
{
	//{0x50,IRKEY_AUTO},
	//{0x51,IRKEY_PAUSE},
	//{0x40,IRKEY_ON},
	//{0x41,IRKEY_OFF},
	//{0x5c,IRKEY_LIGHT_ADD},
	//{0x4d,IRKEY_QUICK},
	//{0x1e,IRKEY_MODE_ADD},
	//{0x5d,IRKEY_LIGHT_DEC},
	//{0x4c,IRKEY_SLOW},
	//{0x1f,IRKEY_MODE_DEC},
	//{0x58,IRKEY_R},
	//{0x59,IRKEY_G},
	//{0x45,IRKEY_B},
	//{0x54,IRKEY_YELLOW},
	//{0x49,IRKEY_CYAN},
	//{0x48,IRKEY_PARPLE},
	//{0x55,IRKEY_ORANGE},
	//{0x44,IRKEY_W},
	//{0x1c,IRKEY_MODEL1},
	//{0x1d,IRKEY_MODEL2},
	//{0x18,IRKEY_MODEL3},
	//{0x19,IRKEY_MODEL4},
	//{0x1a,IRKEY_MODEL5},
	//{0x1b,IRKEY_MODEL6},
	//{0x14,IRKEY_MODEL7},
	//{0x15,IRKEY_MODEL8},
	//{0x16,IRKEY_MODEL9},
	//{0x17,IRKEY_MODEL10},
	//{0x10,IRKEY_MUSIC1},
	//{0x11,IRKEY_MUSIC2},
	//{0x0c,IRKEY_MUSIC3},
	//{0x0d,IRKEY_MUSIC4},
	//{0x12,IRKEY_MUSIC_ADD},

	//{0x13,IRKEY_MUSIC_DEC},
	//{0x0e,IRKEY_SENSITIVE_ADD},
	//{0x0F,IRKEY_SENSITIVE_DEC},
	//{0x08,IRKEY_30_MIN},
	//{0x0a,IRKEY_90_MIN},

	//{0x0b,IRKEY_2H},
	//{0x09,IRKEY_1H},

    //{0xff,NO_KEY},	//遍历结束条件，不能删除，必须放素组最后

    { 0x45,IRKEY_ON        },
    { 0x46,IRKEY_AUTO      },
    { 0x47,IRKEY_OFF       },
    { 0x44,IRKEY_SPEED_ADD     },
    { 0x40,IRKEY_MODE_ADD     },
    { 0x43,IRKEY_LIGHT_ADD },
    { 0x07,IRKEY_SPEED_DEC     },
    { 0x15,IRKEY_MODE_DEC     },
    { 0x09,IRKEY_LIGHT_DEC },
    { 0x16,IRKEY_RED },
    { 0x19,IRKEY_GREEN1 },
    { 0x0d,IRKEY_BLUE1 },
    { 0x0c,IRKEY_YELLOW1    },
    { 0x18,IRKEY_YELLOW2 },
    { 0x5e,IRKEY_ORANGE1 },
    { 0x08,IRKEY_ORANGE2 },
    { 0x1c,IRKEY_BLUE2 },
    { 0x5a,IRKEY_BLUE3 },
    { 0x42,IRKEY_PINK },
    { 0x52,IRKEY_GREEN2 },
    { 0x4a,IRKEY_WHITE },
	{0xff,NO_KEY},	//遍历结束条件，不能删除，必须放素组最后
};

u8 ir_key_get(const u8 ir_table[][2], u8 ir_data)
{

    u8 keyval = NO_KEY;
    for(u8 i=0;;i++)
    {
        if((ir_table[i][0] == ir_data) || (ir_table[i][0] == 0xFF))
        {
            keyval = ir_table[i][1];
            break;
        }
    }
    return keyval;
}

u8 ir_get_key_value(void)
{
    u8 tkey = NO_KEY;

	if(ir_key.ir_timeout< 14)
	{
		ir_key.ir_timeout++;
	}
	else
	{
		ir_key.ir_addr= 0;
		ir_key.ir_code = 0;
		ir_key.ir_timeout = 0xff;//time out
		ir_key.ir_flag = 0;
	}

	// printf("ir_key.ir_flag = %d\n",ir_key.ir_flag);
	if(ir_key.ir_flag)
	{
		tkey = ir_key_get(ir_tbl_FF00, ir_key.ir_code);
//		printf("tkey = %d\n",tkey);
	}

    return tkey;
}

#endif
