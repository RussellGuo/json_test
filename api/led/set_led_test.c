#include "set_led_api.h"
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char **argv)
{
	int times;
	int i;

	if (argc != 3) {
		printf("usage: %s [red|green|pos] [times]\n", __func__);
		exit(-1);
	}
	if((strcmp(argv[1], "red")) && (strcmp(argv[1], "green")) && (strcmp(argv[1], "pos"))) {
		printf("usage: %s [red|green|pos] [times]\n", __func__);
		exit(-1);
	}

	times = atoi(argv[2]);
	if(!strcmp(argv[1], "red")) {
		for(i = 0; i < times; i++) {
			set_red_led_on();
			sleep(1);
			set_red_led_off();
			sleep(1);
		}
	}
	if(!strcmp(argv[1], "green")) {
		for(i = 0; i < times; i++) {
			set_green_led_on();
			sleep(1);
			set_green_led_off();
			sleep(1);
		}
	}
	if(!strcmp(argv[1], "pos")) {
		for(i = 0; i < times; i++) {
			set_position_led_on();
			sleep(1);
			set_position_led_off();
			sleep(1);
		}
	}
	return 0;
}
