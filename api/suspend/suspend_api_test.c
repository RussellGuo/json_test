#include <stdio.h>
#include <unistd.h>

#include "suspend_api.h"

int main(int argc, char **argv)
{
	int ret;

	sleep(10);
	ret = suspend_enable();

	if(ret) {
		printf("failed set suspend\n");
	} else {
		printf("success set suspend\n");
	}

	return 0;
}
