#include "stdio.h"
#include "wiringPi.h"
/* 24(BCM2835 Lib Pin No.) */
/* not 5(wiringPi Lib Command Pin No.) */
/* 18(Hardware Header Pin No.) */
int main()
{
	int rtn;
	int v;

	rtn = wiringPiSetupGpio();
	if( rtn == -1 ) {
		fprintf(stderr, "wiringPiSetupGpio(): Error (%d)\n", rtn);
		return 1;
	}

	pinMode(24, INPUT);
	pullUpDnControl(24, PUD_UP);	/* pullup */
	v = 0;
	v = digitalRead(24);
	fprintf(stdout, "%d", v);
	
	return 0;
}
