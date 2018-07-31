#ifndef LED_H_
#define LED_H_

#include <stdint.h>

extern void led_init(void);
extern void led_request_value(uint8_t value);
extern void led_task(void);

#endif // LED_H_
