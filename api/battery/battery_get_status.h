#ifndef BATTERY_GET_STATUS_H
#define BATTERY_GET_STATUS_H

#ifdef __cplusplus
extern "C" {
#endif

struct get_status {
        int getBatvol;
        int getTemperature;
		int getPresent;
		int getUsbonline;
    	char getStatus[64];
}bat_status;

int getBatvol(void);
int getTemperature(void);
int getPresent(void);
int getUsbonline(void);
void getStatus(char *getStatus);
#ifdef __cplusplus
} // extern "C"
#endif
#endif
