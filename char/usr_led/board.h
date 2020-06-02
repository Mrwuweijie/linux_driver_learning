#ifndef __BOARD_H__
#define __BOARD_H__

#include <linux/compiler.h>

typedef struct {
	int led_num_max;
	int (*init) (int led_index);
	int (*control) (int led_index, int led_onoff);
	int (*breathe) (int led_index, int cycle);
	int (*blink) (int led_index, int on_time_in_ms, int off_time_in_ms);
} BOARD_LED_OPERATIONS_T;

BOARD_LED_OPERATIONS_T *board_get_led_info(void);

#endif /* END __BOARD_H__ */

