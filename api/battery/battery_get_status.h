#ifndef BATTERY_GET_STATUS_H
#define BATTERY_GET_STATUS_H

#ifdef __cplusplus
extern "C" {
#endif
int getVoltageOcv(void);
int getTemperature(void);
int getPresent(void);
void getStatus(void);
#ifdef __cplusplus
} // extern "C"
#endif
#endif
