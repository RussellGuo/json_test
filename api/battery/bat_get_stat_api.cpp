#include "battery_get_status.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdint.h>
#include <fcntl.h>
#include <string.h>

#define BAT_VOL	   		        "/sys/class/power_supply/battery/real_time_voltage"
#define BAT_PRESENT	 			"/sys/class/power_supply/battery/present"
#define BAT_TEMP	  			"/sys/class/power_supply/battery/temp"
#define BAT_STATUS				"/sys/class/power_supply/battery/status"
#define BAT_USBONLINE			"/sys/class/power_supply/battery/online"

int getBatvol(void)
{
	int fd;
	int ret;
	char buf[16];

	fd = open(BAT_VOL, O_RDONLY);
	if(fd < 0) {
		printf("open %s fail",  BAT_VOL);
		return -1;
	} else {
		memset(buf, 0, sizeof(buf));
		ret = read(fd, buf, sizeof(buf));
		if(ret <= 0) {
			printf(" read failed, ret=%d", ret);
			bat_status.getBatvol = -1;
		} else {
			bat_status.getBatvol = atoi(buf);
		}
	}
	close(fd);
	return bat_status.getBatvol;
}

int getTemperature(void)
{

	int fd;
	int ret;
	char buf[16];

	fd = open(BAT_TEMP, O_RDONLY);
	if(fd < 0) {
		printf("open %s fail",  BAT_TEMP);
		return -1;
	} else {
		memset(buf, 0, sizeof(buf));
		ret = read(fd, buf, sizeof(buf));
		if(ret <= 0) {
			printf(" read failed, ret=%d", ret);
			bat_status.getTemperature = -300;
		} else {
			bat_status.getTemperature = atoi(buf);
		}
	}
	close(fd);
	return bat_status.getTemperature;
}

int getPresent(void)
{

	int fd;
	int ret;
	char buf[16];

	fd = open(BAT_PRESENT, O_RDONLY);
	if(fd < 0) {
		printf("open %s fail",  BAT_PRESENT);
		return -1;
	} else {
		memset(buf, 0, sizeof(buf));
		ret = read(fd, buf, sizeof(buf));
		if(ret <= 0) {
			printf(" read failed, ret=%d", ret);
			bat_status.getPresent = 0;
		} else {
			bat_status.getPresent = atoi(buf);
		}
	}
	close(fd);
	return bat_status.getPresent;
}

int getUsbonline(void)
{

	int fd;
	int ret;
	char buf[8] = "";

	fd = open(BAT_USBONLINE, O_RDONLY, 0777);
	if(fd < 0) {
		printf("open %s fail",  BAT_USBONLINE);
		return -1;
	} else {
		memset(buf, 0, sizeof(buf));
		ret = read(fd, buf, sizeof(buf));
		if(ret <= 0) {
			printf(" read failed, ret=%d", ret);
			bat_status.getUsbonline = 0;
		} else {
			bat_status.getUsbonline = atoi(buf);
		}
	}
	close(fd);
	return bat_status.getUsbonline;
}

void getStatus(char *getStatus)
{
    int fd;
    int  ret;
    char buf[64];

    fd = open(BAT_STATUS, O_RDONLY);
    if(fd < 0) {
        printf("open %s fail",  BAT_STATUS);
    } else {
        memset(buf, 0, sizeof(buf));
        ret = read(fd, buf, sizeof(buf));
        if(ret <= 0) {
		printf(" read failed, ret=%d", ret);
		strcpy(getStatus,"0");
        } else {
            strcpy(getStatus,buf);
        }
    }
    close(fd);
}

