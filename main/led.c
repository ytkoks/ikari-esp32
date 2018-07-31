#include "led.h"

#include "driver/ledc.h"

/*
 * About this example
 *
 * 1. Start with initializing LEDC module:
 *    a. Set the timer of LEDC first, this determines the frequency
 *       and resolution of PWM.
 *    b. Then set the LEDC channel you want to use,
 *       and bind with one of the timers.
 *
 * 2. You need first to install a default fade function,
 *    then you can use fade APIs.
 *
 * 3. You can also set a target duty directly without fading.
 *
 * 4. This example uses GPIO18/19/4/5 as LEDC output,
 *    and it will change the duty repeatedly.
 *
 * 5. GPIO18/19 are from high speed channel group.
 *    GPIO4/5 are from low speed channel group.
 *
 */
#define LEDC_HS_TIMER          LEDC_TIMER_0
#define LEDC_HS_MODE           LEDC_HIGH_SPEED_MODE
#define LEDC_HS_CH0_GPIO       (23)
#define LEDC_HS_CH0_CHANNEL    LEDC_CHANNEL_0
#define LEDC_HS_CH1_GPIO       (19)
#define LEDC_HS_CH1_CHANNEL    LEDC_CHANNEL_1

#define LEDC_LS_TIMER          LEDC_TIMER_1
#define LEDC_LS_MODE           LEDC_LOW_SPEED_MODE
#define LEDC_LS_CH2_GPIO       (4)
#define LEDC_LS_CH2_CHANNEL    LEDC_CHANNEL_2
#define LEDC_LS_CH3_GPIO       (5)
#define LEDC_LS_CH3_CHANNEL    LEDC_CHANNEL_3

#define LEDC_TEST_CH_NUM       (4)
#define LEDC_TEST_DUTY         (4000)
#define LEDC_TEST_FADE_TIME    (3000)

#define WITH_FADE              (1)

/*
 * Prepare individual configuration
 * for each channel of LED Controller
 * by selecting:
 * - controller's channel number
 * - output duty cycle, set initially to 0
 * - GPIO number where LED is connected to
 * - speed mode, either high or low
 * - timer servicing selected channel
 *   Note: if different channels use one timer,
 *         then frequency and bit_num of these channels
 *         will be the same
 */
static ledc_channel_config_t s_ledc_channel[LEDC_TEST_CH_NUM] = {
    {
        .channel    = LEDC_HS_CH0_CHANNEL,
        .duty       = 0,
        .gpio_num   = LEDC_HS_CH0_GPIO,
        .speed_mode = LEDC_HS_MODE,
        .timer_sel  = LEDC_HS_TIMER
    },
    {
        .channel    = LEDC_HS_CH1_CHANNEL,
        .duty       = 0,
        .gpio_num   = LEDC_HS_CH1_GPIO,
        .speed_mode = LEDC_HS_MODE,
        .timer_sel  = LEDC_HS_TIMER
    },
    {
        .channel    = LEDC_LS_CH2_CHANNEL,
        .duty       = 0,
        .gpio_num   = LEDC_LS_CH2_GPIO,
        .speed_mode = LEDC_LS_MODE,
        .timer_sel  = LEDC_LS_TIMER
    },
    {
        .channel    = LEDC_LS_CH3_CHANNEL,
        .duty       = 0,
        .gpio_num   = LEDC_LS_CH3_GPIO,
        .speed_mode = LEDC_LS_MODE,
        .timer_sel  = LEDC_LS_TIMER
    },
};

static uint8_t s_current_value = 0;
static uint8_t s_requested_value = 0;

static void led_set_(uint8_t value);

void led_init(void)
{
    int ch;
    
    /*
     * Prepare and set configuration of timers
     * that will be used by LED Controller
     */
    ledc_timer_config_t ledc_timer = {
        // .duty_resolution = LEDC_TIMER_13_BIT, // resolution of PWM duty
        .duty_resolution = LEDC_TIMER_10_BIT, // resolution of PWM duty
        .freq_hz = 5000,                      // frequency of PWM signal
        .speed_mode = LEDC_HS_MODE,           // timer mode
        .timer_num = LEDC_HS_TIMER            // timer index
    };
    // Set configuration of timer0 for high speed channels
    ledc_timer_config(&ledc_timer);

    // Prepare and set configuration of timer1 for low speed channels
    ledc_timer.speed_mode = LEDC_LS_MODE;
    ledc_timer.timer_num = LEDC_LS_TIMER;
    ledc_timer_config(&ledc_timer);

    // Set LED Controller with previously prepared configuration
    for (ch = 0; ch < LEDC_TEST_CH_NUM; ch++) {
        ledc_channel_config(&s_ledc_channel[ch]);
    }

    // Initialize fade service.
    ledc_fade_func_install(0);

    // init led
    led_set_(0x00);
}

void led_request_value(uint8_t value)
{
    s_requested_value = value;
}

void led_task(void)
{
    if (s_current_value != s_requested_value) {
        led_set_(s_requested_value);
        s_current_value = s_requested_value;
    }
}

void led_set_(uint8_t value)
{
    uint32_t duty = (value << 2) | 0x03;
#if (WITH_FADE == 1)
    ledc_set_fade_with_time(s_ledc_channel[0].speed_mode,
                            s_ledc_channel[0].channel, duty, LEDC_TEST_FADE_TIME);
    ledc_fade_start(s_ledc_channel[0].speed_mode,
                    s_ledc_channel[0].channel, LEDC_FADE_NO_WAIT);
#else
    ledc_set_duty(s_ledc_channel[0].speed_mode, s_ledc_channel[0].channel, duty);
    ledc_update_duty(s_ledc_channel[0].speed_mode, s_ledc_channel[0].channel);
#endif
}
