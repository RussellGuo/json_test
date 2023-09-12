#ifndef __BEEP_H__
#define __BEEP_H__

#define BEEP_DEFAULT_FREQ 2000

void BeepDisable(void);
void BeepEnable(unsigned long freq);

int BeepSuspend(void);
int BeepResume(void);

void BEEP_ON(void);
void BEEP_OFF(void);

#endif

