#include "stdio.h"
#include "wiringPi.h"
/* 24(BCM2835 Lib Pin No.) */
/* not 5(wiringPi Lib Command Pin No.) */
/* 18(Hardware Header Pin No.) */
#define INPUT_PIN	(24)
int main()
{
	int rtn;
	int v;

	rtn = wiringPiSetupGpio();
	if( rtn == -1 ) {
		fprintf(stderr, "wiringPiSetupGpio(): Error (%d)\n", rtn);
		return 1;
	}

	pullUpDnControl(INPUT_PIN, PUD_UP);	/* pullup */
	pinMode(INPUT_PIN, INPUT);
	v = 0;
	v = digitalRead(INPUT_PIN);
	fprintf(stdout, "%d", v);
	
	return 0;
}
