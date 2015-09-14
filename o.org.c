#include "stdio.h"
#include "wiringPi.h"
#define INPUT_PIN	(24)
/* 24(BCM2835 Lib Pin No.) */
/* 5(wiringPi Lib Command Pin No.) */
/* 18(Hardware Header Pin No.) */
#define OUTPUT_PIN	(14)
/* 14(BCM2835 Lib Pin No.) */
/* 15(wiringPi Lib Command Pin No.) */
/* 8(Hardware Header Pin No.) */
#define S_BUF	(512)
int main()
{
	int rtn;
	int v;
#if 1
	FILE *fp;
	unsigned char b[S_BUF];

	memset(buf,0,S_BUF);
	fp = popen("/cat /sys/class/thermal/thermal_zone0/temp", "r");
	if(fp == NULL) {
		fprintf(stderr, "fopen(): Open Error\n");
		return 1;
	} else {
		fgets(b, S_BUF, fp);
		pclose(fp);
		fprintf(stdout, "temp val=%s\n", b);
	}
#endif

	rtn = wiringPiSetupGpio();
	if( rtn == -1 ) {
		fprintf(stderr, "wiringPiSetupGpio(): Error (%d)\n", rtn);
		return 1;
	}

#if 0
	pullUpDnControl(INPUT_PIN, PUD_UP);	/* pullup */
	pinMode(INPUT_PIN, INPUT);

	v = 0;
	v = digitalRead(INPUT_PIN);
	fprintf(stdout, "%d", v);
#else
	pinMode(OUTPUT_PIN, OUTPUT);
	digitalWrite(OUTPUT_PNI);
#endif
	
	return 0;
}
