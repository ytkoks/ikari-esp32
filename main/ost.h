#ifndef OST_H_
#define OST_H_

#include <stdint.h>

extern void ost_init(void);
extern void ost_request_value(uint8_t value);
extern void ost_task(void);

#endif // OST_H_
