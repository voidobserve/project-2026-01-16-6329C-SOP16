#ifndef rf24g_h
#define rf24g_h

// 硬件按键值定义
//3V 2.4G遥控
#define RF24_K01 0x11	
#define RF24_K02 0x12	
#define RF24_K03 0x13	
#define RF24_K04 0x14	

#define RF24_K05 0x21  //声控按键 没有单击	
#define RF24_K06 0x22	
#define RF24_K07 0x23	
#define RF24_K08 0x24	

#define RF24_K09 0x31	
#define RF24_K10 0x32	
#define RF24_K11 0x33	
#define RF24_K12 0x34

#define RF24_K13 0x41	
#define RF24_K14 0x42	
#define RF24_K15 0x43	
#define RF24_K16 0x44	

#define RF24_K17 0x51	
#define RF24_K18 0x52	
#define RF24_K19 0x53	
#define RF24_K20 0x54

#define RF24_K21 0x61	
#define RF24_K22 0x62	
#define RF24_K23 0x63	
#define RF24_K24 0x64

#define RF24_K25 0x01	
#define RF24_K26 0x02	
#define RF24_K27 0x03	
#define RF24_K28 0x04	




#pragma pack (1)
typedef struct
{
    u8 header1;
    u8 header2;
    u8 key_v;
    u8 pair[3];         //客户码
    u8 dynamic_code;    //  动态码

}rf24g_ins_t;   //指令数据
#pragma pack ()

void rf24g_scan(unsigned char *pBuf);
void rf24g_long_timer(void);
u8 get_rf24g_long_state(void);


#endif

