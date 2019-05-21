#include <stdio.h>
#include <stdlib.h>
#include <termio.h>
#include <unistd.h>
#include <fcntl.h>
#include <cutils/log.h>
#include <string.h>
#include "battery_get_status.h"


int main()
{
	int Batvol,Temperature,Present,Usbonline;

	Batvol=getBatvol();
	Temperature=getTemperature();
	Present=getPresent();
	Usbonline=getUsbonline();
	getStatus(bat_status.getStatus);

	printf("bat_status.getBatvol=%d\n", Batvol);
  	printf("bat_status.getTemperature=%d\n", Temperature);
 	printf("bat_status.getPresent=%d\n", Present);
 	printf("bat_status.getUsbonline=%d\n", Usbonline);
 	printf("bat_status.getStatus=%s\n",bat_status.getStatus);
	return 0;
}
