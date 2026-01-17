#ifndef rf24g_app_h
#define rf24g_app_h

#if TCFG_RF24GKEY_ENABLE
void rf24_key_handle(struct sys_event *event);


// #pragma pack (1)
// typedef struct
// {
//     u8 pair[2];
//     u8 flag;    //0:表示该数组没使用，0xAA：表示改数组已配对使用
// }rf24g_pair_t;
// #pragma pack ()





#endif

#endif
