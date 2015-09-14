#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "wiringPi.h"

#define INPUT_PIN	(24)
/* 24(BCM2835 Lib Pin No.) */
/* 5(wiringPi Lib Command Pin No.) */
/* 18(Hardware Header Pin No.) */

#define OUTPUT_PIN	(23)
/* 23(BCM2835 Lib Pin No.) */
/* 4(wiringPi Lib Command Pin No.) */
/* 16(Hardware Header Pin No.) */

#define S_BUF	(512)
#define LIMIT	(40000)

int main()
{
	int rtn;
	int v;
#if 1
	FILE *fp;
	unsigned char b[S_BUF];

	memset(b,0,S_BUF);
	fp = popen("cat /sys/class/thermal/thermal_zone0/temp", "r");
	if(fp == NULL) {
		fprintf(stderr, "fopen(): Open Error\n");
		return 1;
	} else {
		fgets(b, S_BUF, fp);
		pclose(fp);
		fprintf(stdout, "temp val=%s", b);
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

	fprintf(stdout, "int size=%d\n", sizeof(int));
	if(strcmp(b,"") == 0) {
		fprintf(stderr, "b: Null Error\n");
		return 1;
	} else {
		v = atoi(b);
	}
	fprintf(stdout, "v=%d\n",v);
	if(v < LIMIT) {
		digitalWrite(OUTPUT_PIN, 0);
		fprintf(stdout, "0 output.\n");
	} else {
		digitalWrite(OUTPUT_PIN, 1);
		fprintf(stdout, "1 output.\n");
	}
#endif
	
	return 0;
}
