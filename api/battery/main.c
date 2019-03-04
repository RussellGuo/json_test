#include <stdio.h>
#include <stdlib.h>
#include <termio.h>
#include <unistd.h>
#include <fcntl.h>
#include <cutils/log.h>
#include <string.h>

#include "battery_get_status.h"

extern struct get_status{
        int getVoltageOcv;
        int getTemperature;
	int getPresent;
        char getStatus[64];
};
extern struct get_status bat_status;

int main()
{

	int ocv,temp,present;
	ocv = getVoltageOcv();
	temp = getTemperature();
	present = getPresent();
	getStatus();

	printf("bat_status.getVoltageOcv=%d\n", ocv);
  	printf("bat_status.getTemperature=%d\n", temp);
 	printf("bat_status.getPresent=%d\n", present);
 	printf("bat_status.getStatus=%s\n",bat_status.getStatus);
	return 0;
}
