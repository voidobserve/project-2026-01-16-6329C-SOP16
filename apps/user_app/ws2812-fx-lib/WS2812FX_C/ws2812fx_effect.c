// 利用ws2812fx库，创建效果
#include "WS2812FX.h"
#include "ws2812fx_tool.h"
#include "Adafruit_NeoPixel.h"
#include "led_strand_effect.h"
#include "system/includes.h"

extern  Segment* _seg;
extern  uint16_t _seg_len;
extern Segment_runtime* _seg_rt;

/* ****************项目专属效果******************************* */
// 调整RGB顺序效果，以红绿蓝跳变
uint16_t WS2812FX_adj_rgb_sequence(void)
{
  u32 c = BLUE;
  switch(_seg_rt->counter_mode_step )
  {
    case 0:
      c = RED;
      break;
    case 1:
      c = GREEN;
      break;
    case 2:
      c = BLUE;
      break;
  }

  Adafruit_NeoPixel_fill( c, _seg->start, _seg_len);

  if(_seg_rt->counter_mode_step < 3)
  {
    _seg_rt->counter_mode_step++;
  }
  return 1000;
}

// 亮度+最大提示效果
uint16_t max_bright_tips_blue(void)
{
  if(_seg_rt->counter_mode_step == 0)
  {
    Adafruit_NeoPixel_fill( BLUE, _seg->start, _seg_len);
  }
  _seg_rt->counter_mode_step++;
  if(_seg_rt->counter_mode_step > 1)
  {
    extern void read_flash_device_status_init(void);
    read_flash_device_status_init();
    set_fc_effect();
  }
  return 100;
}

uint16_t max_bright_tips_puple(void)
{
  if(_seg_rt->counter_mode_step == 0)
  {
    Adafruit_NeoPixel_fill( PINK, _seg->start, _seg_len);
  }
  _seg_rt->counter_mode_step++;
  if(_seg_rt->counter_mode_step > 1)
  {
    extern void read_flash_device_status_init(void);
    read_flash_device_status_init();
    set_fc_effect();
  }
  return 100;
}

// uint16_t power_on_effect(void)
// {
//   if(_seg_rt->counter_mode_step)
//   {
//     Adafruit_NeoPixel_fill( WHITE, _seg->start, _seg_len);
//   }
//   else
//   {
//     Adafruit_NeoPixel_fill( GRAY, _seg->start, _seg_len);
//   }
//   _seg_rt->counter_mode_step =!_seg_rt->counter_mode_step;
//   _seg_rt->aux_param++;
//   if(_seg_rt->aux_param > 6)
//   {
//     extern void read_flash_device_status_init(void);
//     read_flash_device_status_init();
//     set_fc_effect();
//   }
//   return (500 );
// }


uint16_t power_off_effect(void)
{

  

  _seg->colors[1] = 0;//渐变黑色，避免[1]有残留
  WS2812FX_fade_out();

  _seg_rt->counter_mode_step++;
  if(_seg_rt->counter_mode_step > 50)
    WS2812FX_pause();





  return (_seg->speed );



  #if 0
  if(_seg_rt->counter_mode_step)
  {
    Adafruit_NeoPixel_fill( BLACK, _seg->start, _seg_len);
  }
  else
  {
    Adafruit_NeoPixel_fill( RED, _seg->start, _seg_len);
  }
  _seg_rt->counter_mode_step =!_seg_rt->counter_mode_step;
  _seg_rt->aux_param++;
  if(_seg_rt->aux_param > 4)
  {
    // 硬件关机
    gpio_direction_output(IO_PORTA_08,0);
  }
  return (500 );
  #endif
}

// 解绑效果
uint16_t unbind_effect(void)
{
  if(_seg_rt->counter_mode_step)
  {
    Adafruit_NeoPixel_fill( WHITE, _seg->start, _seg_len);
  }
  else
  {
    Adafruit_NeoPixel_fill( GRAY, _seg->start, _seg_len);
  }
  _seg_rt->counter_mode_step =!_seg_rt->counter_mode_step;

  return (500 );
}

/*
 * Color wipe function，多种颜色流水效果
 * LEDs are turned on (color1) in sequence, then turned off (color2) in sequence.
 * if (uint8_t rev == true) then LEDs are turned off in reverse order
 * 颜色擦除功能，用color1依次点亮LED,再用color2依次擦除
 * rev = 0:两个颜色同向， =1color2反方向擦除
 * IS_REVERSE:流水方向
 */
uint16_t WS2812FX_multiColor_wipe(uint8_t is_reverse,uint8_t rev)
{
  static uint32_t color[2];

	if(_seg_rt->counter_mode_step == _seg_len)
  {
    // 一个循环后更换颜色
    color[_seg_rt->aux_param] = _seg->colors[_seg_rt->aux_param3];
    _seg_rt->aux_param++;
    _seg_rt->aux_param%=2;
    _seg_rt->aux_param3++;
    _seg_rt->aux_param3%=_seg->c_n;
  }
  if(_seg_rt->counter_mode_step == 0)
  {

    color[_seg_rt->aux_param] = _seg->colors[_seg_rt->aux_param3];
    _seg_rt->aux_param++;
    _seg_rt->aux_param%=2;
    _seg_rt->aux_param3++;
    _seg_rt->aux_param3%=_seg->c_n;
    SET_CYCLE;
  }

  if(_seg_rt->counter_mode_step < _seg_len) {
    uint32_t led_offset = _seg_rt->counter_mode_step;
    if(is_reverse) {
      WS2812FX_setPixelColor(_seg->stop - led_offset, color[0]);
    } else {
      WS2812FX_setPixelColor(_seg->start + led_offset, color[0]);
    }
  } else {
    uint32_t led_offset = _seg_rt->counter_mode_step - _seg_len;
    if((is_reverse && !rev) || (!is_reverse && rev)) {
      WS2812FX_setPixelColor(_seg->stop - led_offset, color[1]);
    } else {
      WS2812FX_setPixelColor(_seg->start + led_offset, color[1]);
    }
  }

	_seg_rt->counter_mode_step = (_seg_rt->counter_mode_step + 1) % (_seg_len * 2);

  return (_seg->speed );
}


// 多种颜色流水效果
// 正向流水，颜色同向
uint16_t WS2812FX_mode_multi_forward_same(void)
{
  return WS2812FX_multiColor_wipe(0,0) ;
}


// 多种颜色流水效果
// 反向流水，颜色同向
uint16_t WS2812FX_mode_multi_back_same(void)
{
  return WS2812FX_multiColor_wipe(1,0);
}

/*
在每一个LED颜色随机。并且淡出，淡入
Cycle a rainbow on each LED
 */
uint16_t WS2812FX_mode_fade_each_led(void) {
  if( _seg_rt->counter_mode_step == 0) {
    _seg_rt->counter_mode_step = 0x0f;
    for(uint16_t i=_seg->start; i <= _seg->stop; i++) {
      WS2812FX_setPixelColor(i, WS2812FX_color_wheel(WS2812FX_random8()));

    }
  }
  else{
    WS2812FX_fade_out();
  }
  _seg_rt->counter_mode_step++;
  return (_seg->speed / 8);
}




// 功能：颜色块跳变效果，多个颜色块组成背景,以块为单位步进做流水,
// _seg->c_n:有效颜色数量
// SIZE_OPTION：决定颜色块大小
// IS_REVERSE:0 反向流水 ；1正向流水，WS2812FX_setOptions(REVERSE)来设置

uint16_t WS2812FX_mode_single_block_scan(void)
{
  uint8_t size = fc_effect.dream_scene.seg_size;
  uint8_t j;
  uint32_t c;

  if(size > (_seg->stop - _seg->start))
    return 0;

  _seg_rt->counter_mode_step = 0;
  _seg_rt->aux_param = 0;
  while(_seg_rt->counter_mode_step  < _seg->stop)
  {
    for(j = 0; j < size; j++)
    {
      if(IS_REVERSE == 0) //反向流水
      {
        WS2812FX_setPixelColor( _seg->start + _seg_rt->counter_mode_step, _seg->colors[_seg_rt->aux_param]);
      }
      else
      {
        WS2812FX_setPixelColor( _seg->stop - _seg_rt->counter_mode_step, _seg->colors[_seg_rt->aux_param]);
      }
      _seg_rt->counter_mode_step++;
      if(_seg_rt->counter_mode_step  > _seg->stop)
      {
        break;
      }
    }
    _seg_rt->aux_param++;
    _seg_rt->aux_param %= _seg->c_n;
  }

    c = _seg->colors[0];
    // 重新开始，对颜色转盘
    for(j=1; j< _seg->c_n; j++)
    {
      // 把后面的颜色提前
      _seg->colors[j-1] = _seg->colors[j];
    }
    _seg->colors[j-1] = c;

  return _seg->speed*5;
}


// 多段颜色同时流水效果
// SIZE_OPTION：像素点大小
// SIZE_SMALL   1
// SIZE_MEDIUM  3
// SIZE_LARGE   5
// SIZE_XLARGE  7


uint16_t WS2812FX_mode_multi_block_scan(void)
{
  uint8_t size = (SIZE_OPTION << 1) + 1;
  uint8_t j;
  uint16_t i;
  if(size > (_seg->stop - _seg->start))
    return 0;

  /* 构建背景颜色 */
  if(_seg_rt->counter_mode_step == 0)
  {
    while(_seg_rt->counter_mode_step <= _seg->stop)
    {
      for(j = 0; (j < size) && (_seg_rt->counter_mode_step <= _seg->stop); j++)
      {
        WS2812FX_setPixelColor( _seg->start + _seg_rt->counter_mode_step, _seg->colors[_seg_rt->aux_param]);
        _seg_rt->counter_mode_step++;
      }
      _seg_rt->aux_param++;
      _seg_rt->aux_param %= _seg->c_n;
      // _seg_rt->aux_param %= MAX_NUM_COLORS;
    }
  }
  else
  {
    if(IS_REVERSE) //反向流水
    {
      WS2812FX_move_reverse(_seg->start, _seg->stop);
    }
    else
    {
      WS2812FX_move_forward(_seg->start, _seg->stop);
    }


  }

  return _seg->speed ;
}





/*
 * Fades the LEDs between mutil colors
 */
#if 0
uint16_t WS2812FX_mode_mutil_fade(void)
{
  uint8_t size = 1 << SIZE_OPTION;
  uint16_t j;
  uint32_t color, color1,color0;
  int lum = _seg_rt->counter_mode_step;

  size = 5; //调试用
  _seg->c_n = 3; //调试用
  if(size > (_seg->stop - _seg->start))
    return 0;

  if(lum > 255) lum = 511 - lum; // lum = 0 -> 255 -> 0
  _seg_rt->aux_param = 0;
  _seg_rt->aux_param2 = 0;
  // if(_seg_rt->aux_param2 == 0)
  {
    while(_seg_rt->aux_param2  < _seg->stop)
    {
      color0 = _seg->colors[_seg_rt->aux_param];
      _seg_rt->aux_param++;
      _seg_rt->aux_param %= _seg->c_n;
      color1 = _seg->colors[_seg_rt->aux_param];
      _seg_rt->aux_param++;
      _seg_rt->aux_param %= _seg->c_n;
      color = WS2812FX_color_blend(color1, color0, lum);
      for(j = 0; j < size; j++)
      {
        WS2812FX_setPixelColor( _seg->start + _seg_rt->aux_param2, \
                                color);
        _seg_rt->aux_param2++;
        if(_seg_rt->aux_param2  > _seg->stop)
        {
          break;
        }
      }
    }
  }




  // uint32_t color = WS2812FX_color_blend(_seg->colors[1], _seg->colors[0], lum);
  // Adafruit_NeoPixel_fill(color, _seg->start, _seg_len);

  // 此时颜色停留在color1,把color0颜色变换
  if(_seg_rt->counter_mode_step==0)
  {

  }

  _seg_rt->counter_mode_step++;

  // 此时颜色停留在color1
  if(_seg_rt->counter_mode_step > 511) {
    _seg_rt->counter_mode_step = 0;
    SET_CYCLE;
  }
  // 此时颜色停留再color0
  if(_seg_rt->counter_mode_step == 256)
  {

  }
  return (_seg->speed / 32);
}


#endif

// 多段颜色，同时在做渐变效果，每段效果把_seg->colors轮转
// SIZE_OPTION：像素点大小
// SIZE_SMALL   1
// SIZE_MEDIUM  3
// SIZE_LARGE   5
// SIZE_XLARGE  7
// WS2812FX_set_coloQty()设置颜色数量
uint16_t WS2812FX_mode_mutil_fade(void)
{
  uint8_t size =  fc_effect.dream_scene.seg_size;
  uint16_t j;
  uint8_t cnt0=0,cnt1 = 1;
  uint32_t color, color1,color0;
  static uint32_t c1[MAX_NUM_COLORS];
  int lum = _seg_rt->counter_mode_step;


  if(size > (_seg->stop - _seg->start) && size== 0)
    return 0;

  if(lum > 255) lum = 511 - lum; // lum = 0 -> 255 -> 0
  _seg_rt->aux_param = 0;
  _seg_rt->aux_param2 = 0;

  if(_seg_rt->aux_param3 == 0)
  {
    _seg_rt->aux_param3 = 1;
    memcpy(c1, _seg->colors ,MAX_NUM_COLORS * 4);

  }

    while(_seg_rt->aux_param2  < _seg->stop)
    {
      color0 = _seg->colors[cnt0];
      cnt0++;
      cnt0 %= _seg->c_n;
      color1 = c1[cnt1];
      cnt1++;
      cnt1 %= _seg->c_n;
      color = WS2812FX_color_blend(color1, color0, lum);
      for(j = 0; j < size; j++)
      {
        WS2812FX_setPixelColor( _seg->start + _seg_rt->aux_param2, \
                                color);
        _seg_rt->aux_param2++;
        if(_seg_rt->aux_param2  > _seg->stop)
        {
          break;
        }
      }
    }

  _seg_rt->counter_mode_step+=4;

  // 此时颜色停留在color1
  if(_seg_rt->counter_mode_step > 511) {
    _seg_rt->counter_mode_step = 0;
    // color0的颜色池左移1

    SET_CYCLE;
  }

  // 此时颜色停留在color1,把color0颜色变换,color0向左转盘
  if(_seg_rt->counter_mode_step==0)
  {
    uint32_t c_tmp;
    c_tmp = _seg->colors[0];

    memmove(_seg->colors, _seg->colors + 1, (_seg->c_n-1)*4);

    _seg->colors[_seg->c_n - 1] = c_tmp;
  }

  // 此时颜色停留再color0
  if(_seg_rt->counter_mode_step == 256)
  {
    // color1的颜色池左移1
    uint32_t c_tmp;
    c_tmp = c1[0];
    memmove(&c1[0], &c1[1], (_seg->c_n-1)*4);


    c1[_seg->c_n - 1] = c_tmp;
  }
  return (_seg->speed / 32);
}



// 多段颜色构成背景色，做呼吸效果
// SIZE_OPTION：像素点大小
// SIZE_SMALL   1
// SIZE_MEDIUM  3
// SIZE_LARGE   5
// SIZE_XLARGE  7
// WS2812FX_set_coloQty()设置颜色数量

uint16_t WS2812FX_mode_mutil_breath(void)
{
  uint8_t size = fc_effect.dream_scene.seg_size;
  uint8_t j;
  uint16_t lum = _seg_rt->aux_param3;
  uint32_t color;

  if(lum>255)
  {
    lum = 511 - lum;
  }
  if(size > (_seg->stop - _seg->start))
    return 0;
  _seg_rt->counter_mode_step = 0;
  _seg_rt->aux_param = 0;
  while(_seg_rt->counter_mode_step <= _seg->stop)
  {
    for(j = 0; (j < size) && (_seg_rt->counter_mode_step <= _seg->stop); j++)
    {
      color =  WS2812FX_color_blend( 0,_seg->colors[_seg_rt->aux_param], lum);
      WS2812FX_setPixelColor( _seg->start + _seg_rt->counter_mode_step, color);
      _seg_rt->counter_mode_step++;
    }
    _seg_rt->aux_param++;
    _seg_rt->aux_param %= _seg->c_n;
  }

  _seg_rt->aux_param3+=2;
  if(_seg_rt->aux_param3 > (512 - 5))
  {
    _seg_rt->aux_param3 = 5;
  }

  return _seg->speed;
}


// 多段颜色构成背景色，做闪烁
// SIZE_OPTION：像素点大小
// SIZE_SMALL   1
// SIZE_MEDIUM  3
// SIZE_LARGE   5
// SIZE_XLARGE  7
// WS2812FX_set_coloQty()设置颜色数量

uint16_t WS2812FX_mode_mutil_twihkle(void)
{
  uint8_t size = (SIZE_OPTION << 1) + 1;
  uint8_t j;
  // size = 5;
  // _seg->c_n = 3;
  if(size > (_seg->stop - _seg->start))
    return 0;

  _seg_rt->counter_mode_step = 0;
  _seg_rt->aux_param = 0;
  if(_seg_rt->aux_param3)
  {
      while(_seg_rt->counter_mode_step <= _seg->stop)
    {
      for(j = 0; (j < size) && (_seg_rt->counter_mode_step <= _seg->stop); j++)
      {
        WS2812FX_setPixelColor( _seg->start + _seg_rt->counter_mode_step, _seg->colors[_seg_rt->aux_param]);
        _seg_rt->counter_mode_step++;
      }
      _seg_rt->aux_param++;
      _seg_rt->aux_param %= _seg->c_n;
    }
  }
  else
  {
    Adafruit_NeoPixel_fill( BLACK, _seg->start, _seg_len);

  }

  _seg_rt->aux_param3 =!_seg_rt->aux_param3;


  return _seg->speed;
}

// 多种颜色跳变
uint16_t WS2812FX_mutil_c_jump(void)
{

  Adafruit_NeoPixel_fill(_seg->colors[_seg_rt->counter_mode_step], _seg->start, _seg_len);
  _seg_rt->counter_mode_step++;
  _seg_rt->counter_mode_step %= _seg->c_n;
  return _seg->speed;
}

// 支持多颜色频闪
uint16_t WS2812FX_mutil_strobe(void)
{
  if(_seg_rt->aux_param == 0)
  {
    Adafruit_NeoPixel_fill(_seg->colors[_seg_rt->counter_mode_step], _seg->start, _seg_len);
    _seg_rt->counter_mode_step++;
    _seg_rt->counter_mode_step %= _seg->c_n;
  }
  else
  {
    Adafruit_NeoPixel_fill(BLACK, _seg->start, _seg_len);
  }
  _seg_rt->aux_param =! _seg_rt->aux_param ;

  return _seg->speed / 2;
}

extern u8 pwr_on_effect_f;
extern u8 jianbian_start ;
// 整条灯带渐变，支持多种颜色之间切换
// 颜色池：fc_effect.dream_scene.rgb[]
// 颜色数量fc_effect.dream_scene.c_n
uint16_t WS2812FX_mutil_c_gradual(void)
{
  static u8 index ;
  u32 rgb;
  static u8 pp = 0;

  {

    if(!jianbian_start)return _seg->speed;

    int lum = _seg_rt->counter_mode_step;
    if(lum > 255) lum = 511 - lum; // lum = 0 -> 255 -> 0

    // _seg->colors[1]:目标颜色
    uint32_t color = WS2812FX_color_blend(_seg->colors[1], _seg->colors[0], lum);

    Adafruit_NeoPixel_fill(color, _seg->start, _seg_len);

    if(_seg_rt->counter_mode_step == 256)
    {
      pp = 2;
      index++;
      index %= fc_effect.dream_scene.c_n;

        rgb = ( (u32)fc_effect.dream_scene.rgb[index].r << 16 ) |
                ( (u32)fc_effect.dream_scene.rgb[index].g << 8 ) |
                ( (u32)fc_effect.dream_scene.rgb[index].b ) ;

        _seg->colors[0] = color;
        _seg->colors[1] = rgb;

    }

    _seg_rt->counter_mode_step++;
    if(_seg_rt->counter_mode_step > 511) {
      _seg_rt->counter_mode_step = 0;

      SET_CYCLE;
    }
    return (_seg->speed /5);
  }
}
// 整条灯带呼吸，每次呼吸切换颜色
uint16_t mutil_c_breath(void)
{
  int lum = _seg_rt->counter_mode_step;
  if(lum > 255) lum = 511 - lum; // lum = 0 -> 255 -> 0

  uint32_t color =  WS2812FX_color_blend(0,_seg->colors[_seg_rt->aux_param], lum);
  Adafruit_NeoPixel_fill(color, _seg->start, _seg_len);

  _seg_rt->counter_mode_step += 1;
  if(_seg_rt->counter_mode_step > 511) {
    _seg_rt->counter_mode_step = 0;
    _seg_rt->aux_param++;
    _seg_rt->aux_param %= _seg->c_n;
    // printf("\n _seg_rt->aux_param=%d",_seg_rt->aux_param);
    // printf("\n c=%x",_seg->colors[_seg_rt->aux_param]);
    return 3000 + _seg->speed;
  }
  else if(_seg_rt->counter_mode_step == 255)
  {
    return _seg->speed;
  }

  else{
    return 20;
  }


}


//森木光纤灯
// rgb通道呼吸
// 按照用户需求，亮5秒，灭3秒。这个时间为最小时间单位，进行调节
// 整体呼吸，亮-》灭-》亮
uint16_t breath_rgb(void)
{
  
  int lum = _seg_rt->counter_mode_step;
  if(lum > 255) lum = 511 - lum; // lum = 0 -> 255 -> 0

  uint32_t color =  WS2812FX_color_blend(_seg->colors[1], _seg->colors[0], lum);
  Adafruit_NeoPixel_fill(color, _seg->start, _seg_len);

  _seg_rt->counter_mode_step += 1;
  if(_seg_rt->counter_mode_step > 511) {
    _seg_rt->counter_mode_step = 0;

    return 3000 + _seg->speed;
  }
  else if(_seg_rt->counter_mode_step == 255)
  {
    return _seg->speed;
  }

  else{
    return 20;
  }

 

}

// w通道呼吸
uint16_t breath_w(void)
{
  int lum = _seg_rt->counter_mode_step;
  if(lum > 255) lum = 511 - lum; // lum = 0 -> 255 -> 0

  fc_effect.w =  WS2812FX_color_blend(_seg->colors[1], _seg->colors[0], lum);
  Adafruit_NeoPixel_fill(0, _seg->start, _seg_len);
  _seg_rt->counter_mode_step += 1;
  if(_seg_rt->counter_mode_step > 511)
  {
    _seg_rt->counter_mode_step = 0;

    return 3000 + _seg->speed;
  }
  else if(_seg_rt->counter_mode_step == 255)
  {
    return _seg->speed;
  }

  else{
    return 20;
  }
}

//-------------------------------------- 声控-----------------------------------------
// ----------------------------------------------------------------------------全彩音乐效果
typedef enum {E_RISE, E_TOP}FS_ACT;

typedef struct
{
  uint8_t act;//当前动作，0：上升：1：顶部效果
  uint16_t c_pos;
  uint16_t c_pos1;
  uint16_t trg;
}music_oc_t;  //oc:open/close 开合模式

music_oc_t m_oc={
  .trg = 15
};

typedef struct
{
  uint8_t act; //当前动作，0：上升：1：顶部效果
  uint16_t rise_tag; //上升的目标值
  uint16_t c_pos; //当前位置
  uint8_t fall_sp;  //下降速度
  short int top_pos;
  uint8_t top_sp; //顶端速度
  uint32_t bgc;   //背景颜色
} music_fs_t;//频谱声控

music_fs_t m_fs=
{
  .rise_tag = 40,
  .bgc = GRAY,
  .act = E_TOP,
};

// p:百分比
void set_music_oc_trg(u8 p)
{
  m_oc.trg = p*_seg_len/50;
}
// p:百分比
void set_music_fs_trg(u8 p)
{
  // if(m_fs.act == E_TOP)
  m_fs.rise_tag = p*_seg_len/30;
  // printf("\n rise_tag=%d",m_fs.rise_tag);


}

// 渐变，触发变色
uint16_t fc_music_gradual(void)
{
  extern u8 music_trigger;
  uint32_t color = WS2812FX_color_wheel(_seg_rt->counter_mode_step);

  Adafruit_NeoPixel_fill(color, _seg->start, _seg_len);

  if(music_trigger)
  {
    music_trigger = 0;
    _seg_rt->counter_mode_step += 20;
  }

  _seg_rt->counter_mode_step = (_seg_rt->counter_mode_step + 1) & 0xFF;

  if(_seg_rt->counter_mode_step == 0)  SET_CYCLE;

  return (100);
}


// 呼吸，触发渐亮-》渐暗，最后黑，每次变色
uint16_t fc_music_breath(void)
{
  extern u8 music_trigger;
  static uint32_t color1 ;
  if(music_trigger)
  {
    // if(_seg_rt->counter_mode_step == 0)
    {
      color1 = WS2812FX_color_wheel( _seg_rt->aux_param);
      _seg_rt->aux_param +=5;
    }
    music_trigger = 0;
    _seg_rt->counter_mode_step = 1;
  }

  int lum = _seg_rt->counter_mode_step;
  if(lum > 255) lum = 511 - lum; // lum = 0 -> 255 -> 0

  uint32_t color =  WS2812FX_color_blend(0, color1, lum);
  Adafruit_NeoPixel_fill(color, _seg->start, _seg_len);
  if(_seg_rt->counter_mode_step != 0)
  {
    _seg_rt->counter_mode_step += 8;
  }
  if(_seg_rt->counter_mode_step > 511) {
    _seg_rt->counter_mode_step = 0;
    music_trigger = 0;
    printf("\n fc_music_breath");
  }
  return 10;
}

// 定色，触发换颜色
uint16_t fc_music_static(void)
{
  extern u8 music_trigger;
  uint32_t color1;
  if(music_trigger)
  {
    // if(_seg_rt->counter_mode_step == 0)
    {
      color1 = WS2812FX_color_wheel( _seg_rt->aux_param);
      _seg_rt->aux_param +=20;
    }
    music_trigger = 0;
    Adafruit_NeoPixel_fill(color1, _seg->start, _seg_len);
  }

  return 0xffff;
}

// 定色，触发黑->爆闪一下，每次变色
uint16_t fc_music_twinkle(void)
{
  extern u8 music_trigger;
  uint32_t color1;
  if(music_trigger)
  {
    // if(_seg_rt->counter_mode_step == 0)
    {
      color1 = WS2812FX_color_wheel( _seg_rt->aux_param);
      _seg_rt->aux_param +=20;
    }
    music_trigger = 0;
    Adafruit_NeoPixel_fill(color1, _seg->start, _seg_len);
  }
  else
  {
    Adafruit_NeoPixel_fill(BLACK, _seg->start, _seg_len);
  }
  return 100;
}

extern u16 get_max_sp(void);
#define MAX_RATE 17
uint8_t met_trg = 0;
// 流星发射，声音触发，可以连续发射
uint16_t music_meteor(void)
{

  static uint8_t i = 0;
  uint32_t r1, g1, b1, w1 ;
  const uint8_t rate[MAX_RATE] = {100,75,45,30,20,15,10,7,5,3,2,0,0,0,0,0,0};
  int w = (_seg->colors[0] >> 24) & 0xff;
  int r = (_seg->colors[0] >> 16) & 0xff;
  int g = (_seg->colors[0] >>  8) & 0xff;
  int b =  _seg->colors[0]        & 0xff;
  if(met_trg)
  {
    _seg->colors[0] =  WS2812FX_color_wheel(_seg_rt->counter_mode_step);
    _seg_rt->counter_mode_step = _seg_rt->counter_mode_step + 33 & 0xff;
    if(i == MAX_RATE - 1)
    {
      i = 0;
    }
    met_trg = 0;
  }

  WS2812FX_copyPixels(_seg->start+1,_seg->start, _seg_len-1);
  r1 = r * rate[i] / 100;
  g1 = g * rate[i] / 100;
  b1 = b * rate[i] / 100;
  w1 = w * rate[i] / 100;
  WS2812FX_setPixelColor_rgbw(_seg->start, r1, g1, b1, w1);


  if(i < MAX_RATE-1)
    i++;

  return (30 );
}


// 彩虹色滚动，音乐触发加速
uint16_t trg_en, sp_en, en_cnt;

uint16_t music_energy(void)
{

    for(uint16_t i=0; i < _seg_len; i++) {
      uint32_t color = WS2812FX_color_wheel(((i * 256 / _seg_len) + _seg_rt->counter_mode_step) & 0xFF);
      WS2812FX_setPixelColor(_seg->stop - i, color);
      // r = (color >> 16) & 0xFF * index / 100;
      // g = (color >>  8) & 0xFF * index / 100;
      // b =  color        & 0xFF * index / 100;
      // WS2812FX_setPixelColor_rgb(_seg->stop - i, r, g, b);
    }


  if(trg_en)
  {
    sp_en = get_max_sp();
    en_cnt++;
    _seg_rt->counter_mode_step = (_seg_rt->counter_mode_step + 20) & 0xFF;
    if(en_cnt>10)
    {
      trg_en = 0;
    }
  }
  else{
    en_cnt = 0;
    sp_en = 30;
    _seg_rt->counter_mode_step = (_seg_rt->counter_mode_step + 1) & 0xFF;
  }

  return (sp_en);
}

uint16_t music_star_sp;
// 采集声音的ADC值
void set_mss(uint16_t s)
{
  if(500 > s)
    music_star_sp = 500 -s;
  else music_star_sp = 10;
  if(music_star_sp < 10) music_star_sp = 10;


}
uint8_t music_s_m;//0:随机颜色；1：白色，2：蓝色
void set_music_s_m(u8 m)
{
  music_s_m = m;
}

// 星空效果，音乐触发，更多星星，闪烁速度更快
uint16_t music_star(void)
{
  uint8_t j=2;
  if(music_s_m == 0)
    _seg->colors[0]= WS2812FX_color_wheel(WS2812FX_random8());
  if(music_s_m == 1)  
    _seg->colors[0] = WHITE;
  if(music_s_m == 2)  
    _seg->colors[0] = BLUE;
  uint8_t w = (_seg->colors[0] >> 24) & 0xFF;
  uint8_t r = (_seg->colors[0] >> 16) & 0xFF;
  uint8_t g = (_seg->colors[0] >>  8) & 0xFF;
  uint8_t b = (_seg->colors[0]        & 0xFF);
  uint8_t lum = max(w, max(r, max(g, b))) / 2;

  if(trg_en)
  {
    sp_en = get_max_sp();
    _seg_rt->aux_param++;
    if(_seg_rt->aux_param >3) //加速持续时间
    {
      _seg_rt->aux_param = 0;
      trg_en = 0;

    }

  }
  else{
    sp_en = 500;
  }

  en_cnt+=10;
  if(en_cnt >= sp_en )
  // if(en_cnt >= music_star_sp)
  {
    en_cnt = 0;
    WS2812FX_fade_out();
    for(uint16_t i=0; i <= j ; i++)
    {
      int flicker = WS2812FX_random8_lim(lum);

      WS2812FX_setPixelColor_rgbw(WS2812FX_random16_lim(_seg_len), max(r - flicker, 0), max(g - flicker, 0), max(b - flicker, 0), max(w - flicker, 0));
    }
  }
  SET_CYCLE;
  return (10 );
}

// 彩虹闪烁，有声音两彩虹，没声音灭
uint16_t music_rainbow_flash(void) 
{
  uint16_t i;

  if(trg_en)
  {
    en_cnt++;
    if(en_cnt>10)
    {
      trg_en = 0;
    }
  }
  else{
    en_cnt = 0;
    Adafruit_NeoPixel_fill(GRAY, _seg->start, _seg_len);

  }

  if(trg_en)
  {
    for(i=0; i<_seg_len; i++) 
    {
      uint32_t color = WS2812FX_color_wheel(((i * 256 / _seg_len) + _seg_rt->counter_mode_step) & 0xFF);
      WS2812FX_setPixelColor( i, color);
        
    }

  }
      _seg_rt->counter_mode_step = (_seg_rt->counter_mode_step + 2) & 0xFF;
    if(_seg_rt->counter_mode_step == 0) SET_CYCLE;
  return (get_max_sp());
}


uint16_t music_multi_c_flow(void)
{
  uint8_t size = 5;
  uint8_t j, reverse;
  uint16_t i;

  if(trg_en)
  {
    // trg_en = 0;
    // if(sp_en >10)
    //   sp_en -= 10;
    // else sp_en = 10;
    sp_en = 10;
    _seg_rt->aux_param++;
    if(_seg_rt->aux_param >3) //加速持续时间
    {
      _seg_rt->aux_param = 0;
      trg_en = 0;

    }

  }
  else{
    sp_en = 500;
    // if(sp_en<500)
    //   sp_en +=10;
    // else sp_en = 500;
  }

  en_cnt+=10;
  // printf("%d",music_star_sp);
  // if(music_star_sp < 80)
  // {
  //   music_star_sp = music_star_sp/3;
  // }
  // else  if(music_star_sp < 100)
  // {
  //   music_star_sp = music_star_sp/2;
  // }
  // else if(music_star_sp < 150)
  // {
  //   music_star_sp = music_star_sp*3;
  // }
  // else if(music_star_sp < 200)
  // {
  //   music_star_sp = music_star_sp*4;
  // }
  // else
  // {
  //   music_star_sp = music_star_sp*5;

  // }

  if(en_cnt >= sp_en )
  {
    en_cnt = 0;
    /* 构建背景颜色 */
    if(_seg_rt->counter_mode_step == 0)
    {
      while(_seg_rt->counter_mode_step <= _seg->stop)
      {
        _seg->colors[0]= WS2812FX_color_wheel(WS2812FX_get_random_wheel_index(WS2812FX_random8()));
        for(j = 0; (j < size) && (_seg_rt->counter_mode_step <= _seg->stop); j++)
        {
          WS2812FX_setPixelColor( _seg->start + _seg_rt->counter_mode_step, _seg->colors[0]);
          _seg_rt->counter_mode_step++;
        }
      }
    }
    else
    {

      if(reverse) //反向流水
      {
        WS2812FX_move_reverse(_seg->start, _seg->stop);
      }
      else
      {
        WS2812FX_move_forward(_seg->start, _seg->stop);
      }
    }
  }

  return 10 ;
}


// oc:open clos,开合模式
uint16_t music_oc_2(void)
{
  uint8_t i,j;
  if(m_oc.trg > _seg_len/2)
  {
    m_oc.trg = _seg_len/2;
  }

  if(_seg_rt->aux_param==0)
  {
    m_oc.c_pos = _seg_len/2;
    m_oc.c_pos1 = _seg_len/2;
    _seg_rt->aux_param = 1;
  }

  for(i=_seg_len/2; i< m_oc.c_pos; i++)
  {
    uint32_t color = WS2812FX_color_wheel(((i * 256 / _seg_len) + _seg_rt->counter_mode_step) & 0xFF);
    WS2812FX_setPixelColor(  i, color);
		WS2812FX_setPixelColor(  _seg_len/2 - (i - _seg_len/2) -1, color);

  }
  _seg_rt->counter_mode_step = (_seg_rt->counter_mode_step + 1) & 0xFF;
    if(_seg_rt->counter_mode_step == 0) SET_CYCLE;

  if(m_oc.act == E_RISE)
  {
    if(m_oc.c_pos < _seg_len/2 + m_oc.trg)
    {
      m_oc.c_pos++;
    }
    else
    {
      m_oc.act = E_TOP;
      m_oc.trg = 0;
    }

    if(m_oc.c_pos1 > _seg_len/2 - m_oc.trg && m_oc.c_pos1 > 0)
    {
      m_oc.c_pos1--;
    }
  }
  else
  {
    WS2812FX_setPixelColor( m_oc.c_pos, 0);
    WS2812FX_setPixelColor( m_oc.c_pos1, 0);

    if(m_oc.c_pos > _seg_len/2)
    {
      m_oc.c_pos--;
    }
    if(m_oc.c_pos1<_seg_len/2)
    {
      m_oc.c_pos1++;
    }
  }

  if(m_oc.c_pos < _seg_len/2 + m_oc.trg)
  {
    m_oc.act = E_RISE;
  }

  return (get_max_sp());
}

// 两边往中间跑，开合效果
uint16_t music_2_side_oc(void)
{
  // 顺序上升是：从0 ~ 一半
  // 倒序上升是：最长 ~ 一半
  uint8_t i,j;
  if(m_oc.trg > _seg_len/2)
  {
    m_oc.trg = _seg_len/2;
  }
  /* 设置初值 */
  if(_seg_rt->aux_param==0)
  {
    m_oc.c_pos = 0; //顺序的位置，从0开始
    m_oc.c_pos1 = _seg_len-1;//倒叙的位置，从最后一点开始
    _seg_rt->aux_param = 1;
  }
  for(i=0; i< m_oc.c_pos; i++)
  {
    uint32_t color = WS2812FX_color_wheel(((i * 256 / _seg_len) + _seg_rt->counter_mode_step) & 0xFF);
    WS2812FX_setPixelColor( i, color);
    WS2812FX_setPixelColor( _seg_len-1-i, color);

  }

    _seg_rt->counter_mode_step = (_seg_rt->counter_mode_step + 1) & 0xFF;
    if(_seg_rt->counter_mode_step == 0) SET_CYCLE;
  /* 上升处理 */
  if(m_oc.act == E_RISE)
  {
    if(m_oc.c_pos <  m_oc.trg-1) //顺序上升位置，堆积上升
    {
      m_oc.c_pos++;
    }
    else
    {
      m_oc.act = E_TOP;   //标记来到顶端
      m_oc.trg = 0;       //清除触发长度，为下降准备
    }

    if(m_oc.c_pos1 > _seg_len - m_oc.trg && m_oc.c_pos1 > 0) //倒序上升位置，
    {
      m_oc.c_pos1--;
    }
  }
  /* 下降处理 */
  else
  {
    WS2812FX_setPixelColor( m_oc.c_pos, 0);
    WS2812FX_setPixelColor( m_oc.c_pos1, 0);

    if(m_oc.c_pos > 0)//计算顺序下降位置
    {
      m_oc.c_pos--;
    }
    if(m_oc.c_pos1<_seg_len-1)
    {
      m_oc.c_pos1++;
    }
  }

  if(m_oc.c_pos < m_oc.trg)
  {
    m_oc.act = E_RISE;
  }
  return (get_max_sp());
}
uint16_t sp;


// 频谱，生长效果，到达最高位置，顶点变成白色，慢慢下降
uint16_t music_fs(void) {
  uint8_t i;
  uint32_t color_top , color;
  if(m_fs.rise_tag > _seg_len) m_fs.rise_tag = _seg_len;
  
  for(i=0; i< m_fs.c_pos; i++) //上升点亮灯带
  {
    color = WS2812FX_color_wheel(((i * 256 / _seg_len) + _seg_rt->counter_mode_step) & 0xFF);
    WS2812FX_setPixelColor( i, color);
    // if(m_fs.top_pos<i)    //点亮顶端白点
    // WS2812FX_setPixelColor( i+1, WHITE);

  }
  _seg_rt->counter_mode_step = (_seg_rt->counter_mode_step + 1) & 0xFF;
  if(_seg_rt->counter_mode_step == 0) SET_CYCLE;

  if(m_fs.act == E_RISE)
  {
    if(m_fs.c_pos < m_fs.rise_tag - 1) //-1是留一个点位置给顶端显示白点
    {
      m_fs.c_pos++;
      
    }
    else//到达顶端
    {
      m_fs.act = E_TOP;
      if(m_fs.c_pos > m_fs.top_pos)  //刷新顶端值
      {
        m_fs.top_pos = m_fs.c_pos  ;
        color_top = color;
      }
      m_fs.rise_tag = 0;
    }
    // voice_c%=voice_trg;

  }
  else
  {
    if(m_fs.fall_sp >= 3)
    {
      m_fs.fall_sp=0;
			if(m_fs.c_pos > 0) m_fs.c_pos--;
      WS2812FX_setPixelColor( m_fs.c_pos, 0);
    }
    m_fs.fall_sp++;
  }

  if(m_fs.rise_tag >= m_fs.c_pos && m_fs.rise_tag >0)
  {
    m_fs.act = E_RISE;
  }
  else
  {
    m_fs.act = E_TOP;
  }

  if(m_fs.top_pos >= m_fs.c_pos)
  {
    if(m_fs.top_sp>=10)
    {
      WS2812FX_setPixelColor( m_fs.top_pos+2, BLACK);
      WS2812FX_setPixelColor( m_fs.top_pos+1, color_top);
      WS2812FX_setPixelColor( m_fs.top_pos, color_top);
      m_fs.top_sp=0;
      m_fs.top_pos--;
    }
    m_fs.top_sp++;
  }

  return (get_max_sp());
}

// 频谱，生长效果，背景色可调
uint16_t music_fs_bc(void) {
  uint16_t i;
  if(m_fs.rise_tag > _seg_len) m_fs.rise_tag = _seg_len;
  Adafruit_NeoPixel_fill(m_fs.bgc, _seg->start, _seg_len);

  for(i=0; i< m_fs.c_pos; i++) //上升点亮灯带
  {
    uint32_t color = WS2812FX_color_wheel(((i * 256 / _seg_len) + _seg_rt->counter_mode_step) & 0xFF);
    WS2812FX_setPixelColor( i, color);

  }
  _seg_rt->counter_mode_step = (_seg_rt->counter_mode_step + 1) & 0xFF;
  if(_seg_rt->counter_mode_step == 0) SET_CYCLE;



  if(m_fs.act == E_RISE)
  {
    if(m_fs.c_pos < m_fs.rise_tag ) //-1是留一个点位置给顶端显示白点
    {
      m_fs.c_pos++;

    }
    else//到达顶端
    {
      m_fs.act = E_TOP;
      if(m_fs.rise_tag > m_fs.top_pos)  //刷新顶端值
      {
        m_fs.top_pos = m_fs.rise_tag -1 ;
      }
      m_fs.rise_tag = 0;
    }
    // voice_c%=voice_trg;

  }
  else
  {
    if(m_fs.fall_sp >= 3)
    {
      m_fs.fall_sp=0;
			if(m_fs.c_pos > 0) m_fs.c_pos--;
      // WS2812FX_setPixelColor( m_fs.c_pos, m_fs.bgc);  //下降填充背景颜色
    }
    m_fs.fall_sp++;
  }

  if(m_fs.rise_tag  > m_fs.c_pos && m_fs.rise_tag >0)
  {
    m_fs.act = E_RISE;
  }
  else
  {
    m_fs.act = E_TOP;
  }


  return (get_max_sp());
}


uint16_t music_fs_green_blue(void)
{
  uint16_t i;
  static u8 ss;
  uint8_t size = 1 << SIZE_OPTION;
  uint8_t param;
  param = _seg_len;
  // if(_seg_len > 64)
  // {
  //   param = 64;
  // }

  // uint8_t sineIncr = max(1, (256 / param) * size);
  uint8_t sineIncr = max(1, (256 / param) * size);

  if(m_fs.rise_tag > _seg_len) m_fs.rise_tag = _seg_len;
  Adafruit_NeoPixel_fill(BLACK, _seg->start, _seg_len);

  for(i=0; i< m_fs.c_pos; i++) //上升点亮灯带
  {
    int lum = (int)Adafruit_NeoPixel_sine8(((i + _seg_rt->counter_mode_step) * sineIncr));
    uint32_t color = WS2812FX_color_blend(BLUE, GREEN, lum);
    // if(IS_REVERSE)
    {
      WS2812FX_setPixelColor(_seg->start + i, color);
    }
    // else
    // {
    //   WS2812FX_setPixelColor(_seg->stop - i,  color);
    // }

  }
  ss++;
  if(ss>=3)
  {
  _seg_rt->counter_mode_step = (_seg_rt->counter_mode_step + 1) & 0xFF;
  if(_seg_rt->counter_mode_step == 0) SET_CYCLE;
  ss=0;

  }


  if(m_fs.act == E_RISE)
  {
    if(m_fs.c_pos < m_fs.rise_tag ) //-1是留一个点位置给顶端显示白点
    {
      m_fs.c_pos++;

    }
    else//到达顶端
    {
      m_fs.act = E_TOP;
      if(m_fs.rise_tag > m_fs.top_pos)  //刷新顶端值
      {
        m_fs.top_pos = m_fs.rise_tag -1 ;
      }
      m_fs.rise_tag = 0;
    }
    // voice_c%=voice_trg;

  }
  else
  {
    if(m_fs.fall_sp >= 3)
    {
      m_fs.fall_sp=0;
			if(m_fs.c_pos > 0) m_fs.c_pos--;
      // WS2812FX_setPixelColor( m_fs.c_pos, m_fs.bgc);  //下降填充背景颜色
    }
    m_fs.fall_sp++;
  }

  if(m_fs.rise_tag  > m_fs.c_pos && m_fs.rise_tag >0)
  {
    m_fs.act = E_RISE;
  }
  else
  {
    m_fs.act = E_TOP;
  }


  return (get_max_sp());
}








/* -------------------------------ws2812fx自带效果----------------------------------- */
/* #define	FX_MODE_STATIC			0		//静态-无闪烁。只是普通的老式静电灯。
#define	FX_MODE_BLINK			1		//眨眼-正常眨眼。50%的开/关时间。
#define	FX_MODE_BREATH			2		//呼吸-进行众所周知的i-设备的“备用呼吸”。固定速度。
#define	FX_MODE_COLOR_WIPE			3		//颜色擦除-在每个LED灯亮起后点亮所有LED灯。然后按顺序关闭它们。重复
#define	FX_MODE_COLOR_WIPE_INV			4		//颜色擦除反转-与颜色擦除相同，只是交换开/关颜色。
#define	FX_MODE_COLOR_WIPE_REV			5		//颜色擦除反转-在每个LED点亮后点亮所有LED。然后按相反的顺序关闭它们。重复
#define	FX_MODE_COLOR_WIPE_REV_INV			6		//颜色擦除反转-与颜色擦除反转相同，除了交换开/关颜色。
#define	FX_MODE_COLOR_WIPE_RANDOM			7		//颜色擦除随机-将所有LED依次切换为随机颜色。然后用另一种颜色重新开始。
#define	FX_MODE_RANDOM_COLOR			8		//随机颜色-以一种随机颜色点亮所有LED。然后将它们切换到下一个随机颜色。
#define	FX_MODE_SINGLE_DYNAMIC			9		//单一动态-以随机颜色点亮每个LED。将一个随机LED逐个更改为随机颜色。
#define	FX_MODE_MULTI_DYNAMIC			10		//多动态-以随机颜色点亮每个LED。将所有LED同时更改为新的随机颜色。
#define	FX_MODE_RAINBOW			11		//彩虹-通过彩虹同时循环所有LED。
#define	FX_MODE_RAINBOW_CYCLE			12		//彩虹循环-在整个LED串上循环彩虹。
#define	FX_MODE_SCAN			13		//扫描-来回运行单个像素。
#define	FX_MODE_DUAL_SCAN			14		//双扫描-在相反方向来回运行两个像素。
#define	FX_MODE_FADE			15		//淡入淡出-使LED灯再次淡入淡出。
#define	FX_MODE_THEATER_CHASE			16		//剧场追逐——剧场风格的爬行灯。灵感来自Adafruit的例子。
#define	FX_MODE_THEATER_CHASE_RAINBOW			17		//剧院追逐彩虹-剧院风格的彩虹效果爬行灯。灵感来自Adafruit的例子。
#define	FX_MODE_RUNNING_LIGHTS			18		//运行灯光-运行灯光效果与平滑正弦过渡。
#define	FX_MODE_TWINKLE			19		//闪烁-闪烁几个LED灯，重置，重复。
#define	FX_MODE_TWINKLE_RANDOM			20		//闪烁随机-以随机颜色闪烁多个LED，打开、重置、重复。
#define	FX_MODE_TWINKLE_FADE			21		//闪烁淡出-闪烁几个LED，淡出。
#define	FX_MODE_TWINKLE_FADE_RANDOM			22		//闪烁淡出随机-以随机颜色闪烁多个LED，然后淡出。
#define	FX_MODE_SPARKLE			23		//闪烁-每次闪烁一个LED。
#define	FX_MODE_FLASH_SPARKLE			24		//闪烁-以选定颜色点亮所有LED。随机闪烁单个白色像素。
#define	FX_MODE_HYPER_SPARKLE			25		//超级火花-像闪光火花。用更多的闪光。
#define	FX_MODE_STROBE			26		//频闪-经典的频闪效果。
#define	FX_MODE_STROBE_RAINBOW			27		//频闪彩虹-经典的频闪效果。骑自行车穿越彩虹。
#define	FX_MODE_MULTI_STROBE			28		//多重选通-具有不同选通计数和暂停的选通效果，由速度设置控制。
#define	FX_MODE_BLINK_RAINBOW			29		//闪烁彩虹-经典的闪烁效果。骑自行车穿越彩虹。
#define	FX_MODE_CHASE_WHITE			30		//追逐白色——白色上的颜色。
#define	FX_MODE_CHASE_COLOR			31		//追逐颜色-白色在颜色上奔跑。
#define	FX_MODE_CHASE_RANDOM			32		//Chase Random-白色跑步，然后是随机颜色。
#define	FX_MODE_CHASE_RAINBOW			33		//追逐彩虹——白色在彩虹上奔跑。
#define	FX_MODE_CHASE_FLASH			34		//Chase Flash（追逐闪光）-在彩色屏幕上运行的白色闪光。
#define	FX_MODE_CHASE_FLASH_RANDOM			35		//Chase Flash Random（随机闪烁）：白色闪烁，然后是随机颜色。
#define	FX_MODE_CHASE_RAINBOW_WHITE			36		//追逐彩虹白色-彩虹在白色上奔跑。
#define	FX_MODE_CHASE_BLACKOUT			37		//Chase Blackout-黑色在颜色上运行。
#define	FX_MODE_CHASE_BLACKOUT_RAINBOW			38		//追逐遮光彩虹-黑色在彩虹上奔跑。
#define	FX_MODE_COLOR_SWEEP_RANDOM			39		//颜色扫描随机-从条带开始和结束交替引入的随机颜色。
#define	FX_MODE_RUNNING_COLOR			40		//运行颜色-交替运行颜色/白色像素。
#define	FX_MODE_RUNNING_RED_BLUE			41		//运行红蓝-交替运行红/蓝像素。
#define	FX_MODE_RUNNING_RANDOM			42		//随机运行-随机彩色像素运行。
#define	FX_MODE_LARSON_SCANNER			43		//拉森扫描仪-K.I.T.T。
#define	FX_MODE_COMET			44		//彗星——从一端发射彗星。
#define	FX_MODE_FIREWORKS			45		//烟花-烟花火花。
#define	FX_MODE_FIREWORKS_RANDOM			46		//烟花随机-随机彩色烟花火花。
#define	FX_MODE_MERRY_CHRISTMAS			47		//圣诞快乐-绿色/红色像素交替运行。
#define	FX_MODE_FIRE_FLICKER			48		//火焰闪烁-火焰闪烁效果。就像在刺骨的风中。
#define	FX_MODE_FIRE_FLICKER_SOFT			49		//火焰闪烁（软）-火焰闪烁效果。跑得更慢/更柔和。
#define	FX_MODE_FIRE_FLICKER_INTENSE			50		//火焰闪烁（强烈）-火焰闪烁效果。更多颜色范围。
#define	FX_MODE_CIRCUS_COMBUSTUS			51		//Circus Combustitus-交替运行白/红/黑像素。
#define	FX_MODE_HALLOWEEN			52		//万圣节-交替运行橙色/紫色像素。
#define	FX_MODE_BICOLOR_CHASE			53		//双色追逐-背景色上运行的两个LED。
#define	FX_MODE_TRICOLOR_CHASE			54		//三色追逐-交替运行三色像素。
#define	FX_MODE_CUSTOM			55  // keep this for backward compatiblity		//闪烁福克斯-灯光随机淡入淡出。
#define	FX_MODE_CUSTOM_0			55  // custom modes need to go at the end		//通过63。自定义-最多八个用户创建的自定义效果。 */
/*
 * Random flickering.
 */
uint16_t WS2812FX_mode_fire_flicker(void) {
  return WS2812FX_fire_flicker(3);
}

/*
* Random flickering, less intensity.
*/
uint16_t WS2812FX_mode_fire_flicker_soft(void) {
  return WS2812FX_fire_flicker(6);
}

/*
* Random flickering, more intensity.
*/
uint16_t WS2812FX_mode_fire_flicker_intense(void) {
  return WS2812FX_fire_flicker(1);
}


/*
 * Random colored firework sparks.
 */
uint16_t WS2812FX_mode_fireworks_random(void) {
  return WS2812FX_fireworks(WS2812FX_color_wheel(WS2812FX_random8()));
}


/*
 * Firework sparks.
 */
uint16_t WS2812FX_mode_fireworks(void) {
  uint32_t color = BLACK;
  do { // randomly choose a non-BLACK color from the colors array
    color = _seg->colors[WS2812FX_random8_lim(MAX_NUM_COLORS)];
  } while (color == BLACK);
  return WS2812FX_fireworks(color);
}

/*
 * Firing comets from one end.
 */
uint16_t WS2812FX_mode_comet(void) {
  WS2812FX_fade_out();

  if(IS_REVERSE) {
    WS2812FX_setPixelColor(_seg->stop - _seg_rt->counter_mode_step, _seg->colors[0]);
  } else {
    WS2812FX_setPixelColor(_seg->start + _seg_rt->counter_mode_step, _seg->colors[0]);
  }

  _seg_rt->counter_mode_step = (_seg_rt->counter_mode_step + 1) % _seg_len;
  if(_seg_rt->counter_mode_step == 0) SET_CYCLE;

  return (_seg->speed / _seg_len);
}


/*
 * K.I.T.T.
 */
uint16_t WS2812FX_mode_larson_scanner(void) {
  WS2812FX_fade_out();

  if(_seg_rt->counter_mode_step < _seg_len) {
    if(IS_REVERSE) {
      WS2812FX_setPixelColor(_seg->stop - _seg_rt->counter_mode_step, _seg->colors[0]);
    } else {
      WS2812FX_setPixelColor(_seg->start + _seg_rt->counter_mode_step, _seg->colors[0]);
    }
  } else {
    uint16_t index = (_seg_len * 2) - _seg_rt->counter_mode_step - 2;
    if(IS_REVERSE) {
      WS2812FX_setPixelColor(_seg->stop - index, _seg->colors[0]);
    } else {
      WS2812FX_setPixelColor(_seg->start + index, _seg->colors[0]);
    }
  }

  _seg_rt->counter_mode_step++;
  if(_seg_rt->counter_mode_step >= (uint16_t)((_seg_len * 2) - 2)) {
    _seg_rt->counter_mode_step = 0;
    SET_CYCLE;
  }

  return (_seg->speed / (_seg_len * 2));
}


/*
 * Random colored pixels running.
 */
uint16_t WS2812FX_mode_running_random(void) {
  uint8_t size = 2 << SIZE_OPTION;
  if((_seg_rt->counter_mode_step) % size == 0) {
    _seg_rt->aux_param = WS2812FX_get_random_wheel_index(_seg_rt->aux_param);
  }

  uint32_t color = WS2812FX_color_wheel(_seg_rt->aux_param);

  return WS2812FX_running(color, color);
}


/*
 * Alternating color/white pixels running.
 */
uint16_t WS2812FX_mode_running_color(void) {
  return WS2812FX_running(_seg->colors[0], _seg->colors[1]);
}


/*
 * Alternating red/blue pixels running.
 */
uint16_t WS2812FX_mode_running_red_blue(void) {
  return WS2812FX_running(RED, BLUE);
}


/*
 * Alternating red/green pixels running.
 */
uint16_t WS2812FX_mode_merry_christmas(void) {
  return WS2812FX_running(RED, GREEN);
}

/*
 * Alternating orange/purple pixels running.
 */
uint16_t WS2812FX_mode_halloween(void) {
  return WS2812FX_running(PURPLE, ORANGE);
}

/*
 * White flashes running on _color.
 */
uint16_t WS2812FX_mode_chase_flash(void) {
  return WS2812FX_chase_flash(_seg->colors[0], WHITE);
}


/*
 * White flashes running, followed by random color.
 */
uint16_t WS2812FX_mode_chase_flash_random(void) {
  return WS2812FX_chase_flash(WS2812FX_color_wheel(_seg_rt->aux_param), WHITE);
}



/*
 * White running on rainbow.
 白色点流水效果，背景为彩虹色，才会说也跟随白色点变换花样
 */
uint16_t WS2812FX_mode_chase_rainbow(void) {
  uint8_t color_sep = 256 / _seg_len;
  uint8_t color_index = _seg_rt->counter_mode_call & 0xFF;
  uint32_t color = WS2812FX_color_wheel(((_seg_rt->counter_mode_step * color_sep) + color_index) & 0xFF);

  return WS2812FX_chase(color, WHITE, WHITE);
}


/*
 * Black running on rainbow.
 */
uint16_t WS2812FX_mode_chase_blackout_rainbow(void) {
  uint8_t color_sep = 256 / _seg_len;
  uint8_t color_index = _seg_rt->counter_mode_call & 0xFF;
  uint32_t color = WS2812FX_color_wheel(((_seg_rt->counter_mode_step * color_sep) + color_index) & 0xFF);

  return WS2812FX_chase(color, BLACK, BLACK);
}


/*
 * White running followed by random color.
 */
uint16_t WS2812FX_mode_chase_random(void) {
  if(_seg_rt->counter_mode_step == 0) {
    _seg_rt->aux_param = WS2812FX_get_random_wheel_index(_seg_rt->aux_param);
  }
  return WS2812FX_chase(WS2812FX_color_wheel(_seg_rt->aux_param), WHITE, WHITE);
}


/*
 * Rainbow running on white.
 */
uint16_t WS2812FX_mode_chase_rainbow_white(void) {
  uint16_t n = _seg_rt->counter_mode_step;
  uint16_t m = (_seg_rt->counter_mode_step + 1) % _seg_len;
  uint32_t color2 = WS2812FX_color_wheel(((n * 256 / _seg_len) + (_seg_rt->counter_mode_call & 0xFF)) & 0xFF);
  uint32_t color3 = WS2812FX_color_wheel(((m * 256 / _seg_len) + (_seg_rt->counter_mode_call & 0xFF)) & 0xFF);

  return WS2812FX_chase(WHITE, color2, color3);
}

/*
 * Bicolor chase mode
 */
uint16_t WS2812FX_mode_bicolor_chase(void) {
  return WS2812FX_chase(_seg->colors[0], _seg->colors[1], _seg->colors[2]);
}


/*
 * White running on _color.
 */
uint16_t WS2812FX_mode_chase_color(void) {
  return WS2812FX_chase(_seg->colors[0], WHITE, WHITE);
}


/*
 * Black running on _color.
 */
uint16_t WS2812FX_mode_chase_blackout(void) {
  return WS2812FX_chase(_seg->colors[0], BLACK, BLACK);
}


/*
 * _color running on white.
 */
uint16_t WS2812FX_mode_chase_white(void) {
  return WS2812FX_chase(WHITE, _seg->colors[0], _seg->colors[0]);
}


/*
 * Strobe effect with different strobe count and pause, controlled by speed.
  颜色爆闪
 */
uint16_t WS2812FX_mode_multi_strobe(void) {
  Adafruit_NeoPixel_fill(_seg->colors[1], _seg->start, _seg_len);

  uint16_t delay = 200 + ((9 - (_seg->speed % 10)) * 100);
  uint16_t count = 2 * ((_seg->speed / 100) + 1);
  if(_seg_rt->counter_mode_step < count) {
    if((_seg_rt->counter_mode_step & 1) == 0) {
      Adafruit_NeoPixel_fill(_seg->colors[0], _seg->start, _seg_len);
      delay = 20;
    } else {
      delay = 50;
    }
  }

  _seg_rt->counter_mode_step = (_seg_rt->counter_mode_step + 1) % (count + 1);
  if(_seg_rt->counter_mode_step == 0) SET_CYCLE;
  return delay;
}

/*
 * Blinks one LED at a time.
 * Inspired by www.tweaking4all.com/hardware/arduino/arduino-led-strip-effects/
 */
uint16_t WS2812FX_mode_sparkle(void) {
  return WS2812FX_sparkle(_seg->colors[1], _seg->colors[0]);
}


/*
 * Lights all LEDs in the color. Flashes white pixels randomly.
 * Inspired by www.tweaking4all.com/hardware/arduino/arduino-led-strip-effects/
 */
uint16_t WS2812FX_mode_flash_sparkle(void) {
  return WS2812FX_sparkle(_seg->colors[0], WHITE);
}


/*
 * Like flash sparkle. With more flash.
 * Inspired by www.tweaking4all.com/hardware/arduino/arduino-led-strip-effects/
 */
uint16_t WS2812FX_mode_hyper_sparkle(void) {
  Adafruit_NeoPixel_fill(_seg->colors[0], _seg->start, _seg_len);

  uint8_t size = 1 << SIZE_OPTION;
  for(uint8_t i=0; i<8; i++) {
    Adafruit_NeoPixel_fill(WHITE, _seg->start + WS2812FX_random16_lim(_seg_len - size), size);
  }

  SET_CYCLE;
  return (_seg->speed / 32);
}


/*
 * Blink several LEDs on, fading out.
 */
uint16_t WS2812FX_mode_twinkle_fade(void) {
  return WS2812FX_twinkle_fade(_seg->colors[0]);
}


/*
 * Blink several LEDs in random colors on, fading out.
 */
uint16_t WS2812FX_mode_twinkle_fade_random(void) {
  return WS2812FX_twinkle_fade(WS2812FX_color_wheel(WS2812FX_random8()));
}


// 所有LED当前颜色淡出，弹出最终颜色为_seg->colors[1]
uint16_t WS2812FX_mode_fade_single(void)
{

  WS2812FX_fade_out();
  return (_seg->speed / 8);
}

/*
 * Blink several LEDs on, reset, repeat.
 * Inspired by www.tweaking4all.com/hardware/arduino/arduino-led-strip-effects/
 */
uint16_t WS2812FX_mode_twinkle(void) {
  return WS2812FX_twinkle(_seg->colors[0], _seg->colors[1]);
}

/*
 * Blink several LEDs in random colors on, reset, repeat.
 * Inspired by www.tweaking4all.com/hardware/arduino/arduino-led-strip-effects/
 */
uint16_t WS2812FX_mode_twinkle_random(void) {
  return WS2812FX_twinkle(WS2812FX_color_wheel(WS2812FX_random8()), _seg->colors[1]);
}


/*
 * Theatre-style crawling lights with rainbow effect.
 * Inspired by the Adafruit examples.
 */
uint16_t WS2812FX_mode_theater_chase_rainbow(void) {
  _seg_rt->counter_mode_step = (_seg_rt->counter_mode_step + 1) & 0xFF;
  uint32_t color = WS2812FX_color_wheel(_seg_rt->counter_mode_step);
  return WS2812FX_tricolor_chase(color, _seg->colors[1], _seg->colors[1]);
}


/*
 * Running lights effect with smooth sine transition.
 */
uint16_t WS2812FX_mode_running_lights(void) {
  uint8_t size = 1 << SIZE_OPTION;
  uint8_t param;
  param = _seg_len;
  if(_seg_len > 64)
  {
    param = 64;
  }

  uint8_t sineIncr = max(1, (256 / param) * size);
  for(uint16_t i=0; i < _seg_len; i++) {
    int lum = (int)Adafruit_NeoPixel_sine8(((i + _seg_rt->counter_mode_step) * sineIncr));
    uint32_t color = WS2812FX_color_blend(_seg->colors[0], _seg->colors[1], lum);
    if(IS_REVERSE) {
      WS2812FX_setPixelColor(_seg->start + i, color);
    } else {
      WS2812FX_setPixelColor(_seg->stop - i,  color);
    }
  }
  _seg_rt->counter_mode_step = (_seg_rt->counter_mode_step + 1) % 256;
  if(_seg_rt->counter_mode_step == 0) SET_CYCLE;
  return (_seg->speed );
}



/*
 * Tricolor chase mode
 */
uint16_t WS2812FX_mode_tricolor_chase(void) {
  return WS2812FX_tricolor_chase(_seg->colors[0], _seg->colors[1], _seg->colors[2]);
}


/*
 * Alternating white/red/black pixels running.
 */
uint16_t WS2812FX_mode_circus_combustus(void) {
  return WS2812FX_tricolor_chase(RED, WHITE, BLACK);
}


/*
 * Theatre-style crawling lights.
 * Inspired by the Adafruit examples.
 */
uint16_t WS2812FX_mode_theater_chase(void) {
  return WS2812FX_tricolor_chase(_seg->colors[0], _seg->colors[1], _seg->colors[1]);
}

/*
 * Cycles a rainbow over the entire string of LEDs.
 彩虹渐变颜色流水效果
 */
uint16_t WS2812FX_mode_rainbow_cycle(void) {
  for(uint16_t i=0; i < _seg_len; i++) {
	  uint32_t color = WS2812FX_color_wheel(((i * 256 / _seg_len) + _seg_rt->counter_mode_step) & 0xFF);
    WS2812FX_setPixelColor(_seg->stop - i, color);
  }

  _seg_rt->counter_mode_step = (_seg_rt->counter_mode_step + 1) & 0xFF;

  if(_seg_rt->counter_mode_step == 0) SET_CYCLE;

  return (_seg->speed / 256);
}

/*
 * Cycles all LEDs at once through a rainbow.
 整体渐变
 */
// uint16_t WS2812FX_mode_rainbow(void) {
//   uint32_t color = WS2812FX_color_wheel(_seg_rt->counter_mode_step);
//   Adafruit_NeoPixel_fill(color, _seg->start, _seg_len);

//   _seg_rt->counter_mode_step = (_seg_rt->counter_mode_step + 1) & 0xFF;

//   if(_seg_rt->counter_mode_step == 0)  SET_CYCLE;

//   return (_seg->speed / 10);
// }


uint16_t WS2812FX_mode_rainbow(void) {


  if(!jianbian_start)return _seg->speed;
  
  uint32_t color = WS2812FX_color_wheel(_seg_rt->counter_mode_step);
  Adafruit_NeoPixel_fill(color, _seg->start, _seg_len);  // 85一个色

  _seg_rt->counter_mode_step = (_seg_rt->counter_mode_step + 1) & 0xFF;

  if(_seg_rt->counter_mode_step == 0)  SET_CYCLE;
// printf("color = 0x%4X\n",color);
// if(color == 0xff0000) printf("\n===================================\n");
// if(color == 0x00ff00) printf("\n===================================\n");
// if(color == 0x0000ff) printf("\n===================================\n");
// if(color == 0xffff00) printf("\n===================================\n");
// if(color == 0xff00ff) printf("\n===================================\n");
// if(color == 0x00ffff) printf("\n===================================\n");
// if(color == 0xffffff) printf("\n===================================\n");
// printf("_seg_rt->counter_mode_step = %d\n",_seg_rt->counter_mode_step);
  if(_seg->speed <= 10)
  {
        return 50; //4s  计算公式 4000 / 85 
  }
  else if(_seg->speed > 10 && _seg->speed <= 110)
  {
        return 70;  //6
  }
  else if(_seg->speed > 110 && _seg->speed <= 210)
  {
        return 80; //7

  }
  else if(_seg->speed > 210 && _seg->speed <= 310)
  {
        return 100; // 8

  }
  else if(_seg->speed > 310 && _seg->speed <= 410)
  {
        return 110; //9

  }
  else if(_seg->speed > 410 && _seg->speed <= 500)
  {
        return 120; //10
  }
}

/*
 * Runs a block of pixels back and forth.
 来回运动像素块
 */
uint16_t WS2812FX_mode_scan(void) {
  return WS2812FX_scan(_seg->colors[0], _seg->colors[1], false);
}


/*
 * Runs two blocks of pixels back and forth in opposite directions.
 */
uint16_t WS2812FX_mode_dual_scan(void) {
  return WS2812FX_scan(_seg->colors[0], _seg->colors[1], true);
}


/*
 * Fades the LEDs between two colors
 */
uint16_t WS2812FX_mode_fade(void) {
  int lum = _seg_rt->counter_mode_step;
  if(lum > 255) lum = 511 - lum; // lum = 0 -> 255 -> 0

  uint32_t color = WS2812FX_color_blend(_seg->colors[1], _seg->colors[0], lum);
  Adafruit_NeoPixel_fill(color, _seg->start, _seg_len);

  _seg_rt->counter_mode_step += 4;
  if(_seg_rt->counter_mode_step > 511) {
    _seg_rt->counter_mode_step = 0;
    SET_CYCLE;
  }
  return (_seg->speed / 128);
}

/*
 * Does the "standby-breathing" of well known i-Devices. Fixed Speed.
 * Use mode "fade" if you like to have something similar with a different speed.
 * _seg->colors[1]，和_seg->colors[0]渐变，若_seg->colors[1]为很色就是呼吸功能
 * lum最小值决定两种颜色混合最小比例。典型值15，若为红色呼吸，LED最暗到15
 */
uint16_t WS2812FX_mode_breath(void) {
  int lum = _seg_rt->counter_mode_step;
  if(lum > 255) lum = 511 - lum; // lum = 15 -> 255 -> 15

  // uint16_t delay;
  // if(lum == 15) delay = 970; // 970 pause before each breath
  // else if(lum <=  25) delay = 38; // 19
  // else if(lum <=  50) delay = 36; // 18
  // else if(lum <=  75) delay = 28; // 14
  // else if(lum <= 100) delay = 20; // 10
  // else if(lum <= 125) delay = 14; // 7
  // else if(lum <= 150) delay = 11; // 5
  // else delay = 10; // 4

  uint32_t color =  WS2812FX_color_blend(_seg->colors[1], _seg->colors[0], lum);
  Adafruit_NeoPixel_fill(color, _seg->start, _seg_len);
   if(_seg_rt->counter_mode_step < 35)
  {
    _seg_rt->counter_mode_step += 1;
  }
  else
  _seg_rt->counter_mode_step += 2;    //不能修改+2，否则呼吸有明显的不流畅
  if(_seg_rt->counter_mode_step > (512-5)) {
    _seg_rt->counter_mode_step = 5;
    SET_CYCLE;
  }
return _seg->speed;
}


/*
 * Lights every LED in a random color. Changes all LED at the same time
 * to new random colors.
 * 每次以随机颜色变换所有LED
 */
uint16_t WS2812FX_mode_multi_dynamic(void) {
  for(uint16_t i=_seg->start; i <= _seg->stop; i++) {
    WS2812FX_setPixelColor(i, WS2812FX_color_wheel(WS2812FX_random8()));
  }
  SET_CYCLE;
  return _seg->speed;
}



/*
 * Lights all LEDs in one random color up. Then switches them
 * to the next random color.
 * 彩虹跳变
 */
uint16_t WS2812FX_mode_random_color(void) {
  _seg_rt->aux_param = WS2812FX_get_random_wheel_index(_seg_rt->aux_param); // aux_param will store our random color wheel index
  uint32_t color = WS2812FX_color_wheel(_seg_rt->aux_param);
  Adafruit_NeoPixel_fill(color, _seg->start, _seg_len);
  SET_CYCLE;
  return _seg->speed;
}


/*
 * Lights every LED in a random color. Changes one random LED after the other
 * to another random color.
 * 以随机颜色点亮每个LED。依次更改一个随机LED
 * 另一种随机颜色。
 */
uint16_t WS2812FX_mode_single_dynamic(void) {
  if(_seg_rt->counter_mode_call == 0) {
    for(uint16_t i=_seg->start; i <= _seg->stop; i++) {
      WS2812FX_setPixelColor(i, WS2812FX_color_wheel(WS2812FX_random8()));
    }
  }

  WS2812FX_setPixelColor(_seg->start + WS2812FX_random16_lim(_seg_len), WS2812FX_color_wheel(WS2812FX_random8()));
  SET_CYCLE;
  return _seg->speed;
}


/*
 * Turns all LEDs after each other to a random color.
 * Then starts over with another color.
 * 彩虹随机颜色依次流水
 */
uint16_t WS2812FX_mode_color_wipe_random(void) {
  if(_seg_rt->counter_mode_step % _seg_len == 0) { // aux_param will store our random color wheel index
    _seg_rt->aux_param = WS2812FX_get_random_wheel_index(_seg_rt->aux_param);
  }
  uint32_t color = WS2812FX_color_wheel(_seg_rt->aux_param);
  return WS2812FX_color_wipe(color, color, false) * 2;
}


/*
 * Random color introduced alternating from start and end of strip.
 彩虹颜色往返流水，每次到达起点/终点，变换颜色
 */
uint16_t WS2812FX_mode_color_sweep_random(void) {
  if(_seg_rt->counter_mode_step % _seg_len == 0) { // aux_param will store our random color wheel index
    _seg_rt->aux_param = WS2812FX_get_random_wheel_index(_seg_rt->aux_param);
  }
  uint32_t color = WS2812FX_color_wheel(_seg_rt->aux_param);
  return WS2812FX_color_wipe(color, color, true) * 2;
}


/*
 * Lights all LEDs one after another.
 */
uint16_t WS2812FX_mode_color_wipe(void) {
  return WS2812FX_color_wipe(_seg->colors[0], _seg->colors[1], false);
}

uint16_t WS2812FX_mode_color_wipe_inv(void) {
  return WS2812FX_color_wipe(_seg->colors[1], _seg->colors[0], false);
}

uint16_t WS2812FX_mode_color_wipe_rev(void) {
  return WS2812FX_color_wipe(_seg->colors[0], _seg->colors[1], true);
}

uint16_t WS2812FX_mode_color_wipe_rev_inv(void) {
  return WS2812FX_color_wipe(_seg->colors[1], _seg->colors[0], true);
}


/*
 * Normal blinking. 50% on/off time.
 */
uint16_t WS2812FX_mode_blink(void) {
  return WS2812FX_blink(_seg->colors[0], _seg->colors[1], false);
}


/*
 * Classic Blink effect. Cycling through the rainbow.
 彩虹颜色和_seg->colors[1]交替闪烁，彩虹颜色一直在变换
 */
uint16_t WS2812FX_mode_blink_rainbow(void) {
  return WS2812FX_blink(WS2812FX_color_wheel(_seg_rt->counter_mode_call & 0xFF), _seg->colors[1], false);
}


/*
 * Classic Strobe effect.
 两个颜色爆闪，_seg->colors[1]下突然爆闪一下_seg->colors[0]，
_seg->colors[0]时间很多眼睛都没察觉
 */
uint16_t WS2812FX_mode_strobe(void) {
  return WS2812FX_blink(_seg->colors[0], _seg->colors[1], true);
}


/*
 * Classic Strobe effect. Cycling through the rainbow.
 彩虹色爆闪
 */
uint16_t WS2812FX_mode_strobe_rainbow(void) {
  return WS2812FX_blink(WS2812FX_color_wheel(_seg_rt->counter_mode_call & 0xFF), _seg->colors[1], true);
}

/*
 * No blinking. Just plain old static light.
 */
uint16_t WS2812FX_mode_static(void)
{
  if(_seg_rt->counter_mode_step == 0)
  {
    _seg_rt->counter_mode_step = 1;
    // printf("\n WS2812FX_mode_static");
    // printf("\n _seg->colors[0] = %x",_seg->colors[0]);
    Adafruit_NeoPixel_fill(_seg->colors[0], _seg->start, _seg->stop);
  }
  return 0xFFFF;
}



// 提示效果,白光闪烁
uint16_t white_tips(void)
{
  if(_seg_rt->counter_mode_step)
  {
    Adafruit_NeoPixel_fill( GRAY, _seg->start, _seg_len);
  }
  else
  {
    Adafruit_NeoPixel_fill( 0, _seg->start, _seg_len);
  }
  _seg_rt->counter_mode_step =!_seg_rt->counter_mode_step;
  _seg_rt->aux_param++;
  if(_seg_rt->aux_param > 3)
  {
    extern void read_flash_device_status_init(void);
    read_flash_device_status_init();
    set_fc_effect();
  }
  return (300);
}




