#include "battery_get_status.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdint.h>
#include <fcntl.h>
#include <string.h>

#define BAT_OCV				"/sys/class/power_supply/battery/voltage_ocv"
#define BAT_PRESENT	 		"/sys/class/power_supply/battery/present"
#define BAT_TEMP	  		"/sys/class/power_supply/battery/temp"
#define BAT_STATUS			"/sys/class/power_supply/battery/status"

struct get_status {
        int getVoltageOcv;
        int getTemperature;
	int getPresent;
	char getStatus[64];

}bat_status;

int getVoltageOcv(void)
{

	int fd;
	int ret;
	char buf[8] = "";

	fd = open(BAT_OCV, O_RDONLY | O_CREAT | O_TRUNC, 0777);
	if(fd < 0) {
		printf("open %s fail",  BAT_OCV);
		return -1;
	} else {
		memset(buf, 0, sizeof(buf));
		ret = read(fd, buf, sizeof(buf));
		if(ret <= 0) {
			printf(" read failed, ret=%d", ret);
			bat_status.getVoltageOcv = -1;
		} else {
			bat_status.getVoltageOcv = atoi(buf);
		}
	}

	close(fd);
	return bat_status.getVoltageOcv;
}

int getTemperature(void)
{

	int fd;
	int ret;
	char buf[8] = "";

	fd = open(BAT_TEMP, O_RDONLY | O_CREAT | O_TRUNC, 0777);
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
	char buf[8] = "";

	fd = open(BAT_PRESENT, O_RDONLY | O_CREAT | O_TRUNC, 0777);
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


void getStatus(void)                           
{                                              
                                               
    int fd;
    int  ret;
    char buf[64];

    fd = open(BAT_STATUS, O_RDONLY | O_CREAT | O_TRUNC, 0777);
    if(fd < 0) {
        printf("open %s fail",  BAT_STATUS);
    } else {
        memset(buf, 0, sizeof(buf));
        ret = read(fd, buf, sizeof(buf));
        if(ret <= 0) {
           	printf(" read failed, ret=%d", ret);
            strcpy(bat_status.getStatus,"0");
        } else {
            strcpy(bat_status.getStatus,buf);
        }          
    }
    close(fd);
}

