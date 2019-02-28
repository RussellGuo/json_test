#ifndef SET_LED_H
#define SET_LED_H

#ifdef __cplusplus
extern "C" {
#endif

int set_red_led_on(void);
int set_red_led_off(void);

int set_green_led_on(void);
int set_green_led_off(void);

int set_position_led_on(void);
int set_position_led_off(void);

#ifdef __cplusplus
} // extern "C"
#endif

#endif
