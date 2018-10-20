#include "ost.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "driver/gpio.h"
#include "rom/ets_sys.h"

#define delay(ms) (vTaskDelay(ms/portTICK_RATE_MS))
#define delayMicroseconds(us) (ets_delay_us(us))


// Output Pins
#define GPIO_OUTPUT_IO_0    (34)
#define GPIO_OUTPUT_IO_1    (35)
#define GPIO_OUTPUT_PIN_SEL ((1ULL<<GPIO_OUTPUT_IO_0) | (1ULL<<GPIO_OUTPUT_IO_1))
#define DIN_H				GPIO_OUTPUT_IO_0
#define DIN_L				GPIO_OUTPUT_IO_1
#define LOW                 (0)
#define HIGH                (1)

static uint8_t s_current_value = 0;
static uint8_t s_requested_value = 0;

static void ost_set_(uint8_t value);

static void LED_Init();
static void LED_Color_RGB(uint8_t led_r, uint8_t led_g, uint8_t led_b);

void ost_init(void)
{
    gpio_config_t io_conf;
    //disable interrupt
    io_conf.intr_type = GPIO_PIN_INTR_DISABLE;
    //set as output mode
    io_conf.mode = GPIO_MODE_OUTPUT;
    //bit mask of the pins that you want to set,e.g.GPIO18/19
    io_conf.pin_bit_mask = GPIO_OUTPUT_PIN_SEL;
    //disable pull-down mode
    io_conf.pull_down_en = 0;
    //disable pull-up mode
    io_conf.pull_up_en = 0;
    //configure GPIO with the given settings
    gpio_config(&io_conf);

    /*
     *  init led
     */
    LED_Init();
    // ost_set_(0x00);
    ost_set_(0xFF);
}

void ost_request_value(uint8_t value)
{
    s_requested_value = value;
}

void ost_task(void)
{
    if (s_current_value != s_requested_value) {
        ost_set_(s_requested_value);
        s_current_value = s_requested_value;
    }
}

void ost_set_(uint8_t value)
{
    uint32_t duty = (value << 2) | 0x03;

    LED_Color_RGB(duty, 128, 128);
}


//
//-------------------------------------------------------------------------------------------------
//

//=============================================================================//
//接続したＬＥＤの数を指定
int LED_MAX = 1;

//=============================================================================//

static void digitalWrite(gpio_num_t gpio_num, uint32_t level)
{
    gpio_set_level(gpio_num, level);
}

//=========================================================//
// 初期化信号送出
//=========================================================//
void LED_Init() {
  digitalWrite(DIN_H, LOW);
  digitalWrite(DIN_L, HIGH);
  delay(10);
}
//=========================================================//
// 値固定(Set)信号送出
//=========================================================//
void LED_Set() {
  digitalWrite(DIN_H, LOW);
  digitalWrite(DIN_L, HIGH);
  delay(1); 
}

//=========================================================//
// H(1)信号送出
//=========================================================//
void LED_Hi_Bit() {
  digitalWrite(DIN_H, HIGH);
  digitalWrite(DIN_L, HIGH);
  delayMicroseconds(1);
  digitalWrite(DIN_H, LOW);
  digitalWrite(DIN_L, HIGH);
  delayMicroseconds(1);
}
//=========================================================//
// L(0)信号送出
//=========================================================//
void LED_Low_Bit() {
  digitalWrite(DIN_H, LOW);
  digitalWrite(DIN_L, LOW);
  delayMicroseconds(1);
  digitalWrite(DIN_H, LOW);
  digitalWrite(DIN_L, HIGH);
  delayMicroseconds(1);
}

//=========================================================//
// LED用にＲＧＢカラー値を変換して指定する関数 
//=========================================================//
void LED_Color_RGB(uint8_t led_r, uint8_t led_g, uint8_t led_b) {
    uint8_t mask;

    // blue
    mask = 0x80;
    while (mask) {
        if (led_b & mask)
            LED_Hi_Bit();
        else
            LED_Low_Bit();
        mask >>= 1;
    }
    // green
    mask = 0x80;
    while (mask) {
        if (led_g & mask)
            LED_Hi_Bit();
        else
            LED_Low_Bit();
        mask >>= 1;
    }
    // red
    mask = 0x80;
    while (mask) {
        if (led_r & mask)
            LED_Hi_Bit();
        else
            LED_Low_Bit();
        mask >>= 1;
    }
}
//=============================================================================//
