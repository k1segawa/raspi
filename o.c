#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "wiringPi.h"

#define DEBUG

#ifdef DEBUG
#define DPRINT	printf
#else
#define DPRINT
#endif

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

int main(int argc, char *argv[])
{
	int rtn;
	int v;

/* OUTPUT */	
#if 1
	FILE *fp;
	unsigned char b[S_BUF+1];
	int o_pin_no;
	int limit;

	if( argc == 1 ) {
		/* default */
		o_pin_no = OUTPUT_PIN;
		limit = LIMIT;
	} else if( argc == 3 ) {
		o_pin_no = atoi(argv[1]);
		limit = atoi(argv[2]);
	} else {
		fprintf(stderr, "Usage:%s\n",argv[0]);
		fprintf(stderr, "      %s <port No.> <limit>\n",argv[0]);
		return 1;
	}

	fp = popen("cat /sys/class/thermal/thermal_zone0/temp", "r");
	if( fp == NULL ) {
		fprintf(stderr, "fopen(): Open Error\n");
		return 1;
	} else {
		fgets(b, S_BUF, fp);
		pclose(fp);
		DPRINT("temp val=%s", b);
	}
#endif

	rtn = wiringPiSetupGpio();
	if( rtn == -1 ) {
		fprintf(stderr, "wiringPiSetupGpio(): Error (%d)\n", rtn);
		return 1;
	}

#if 0
/* INPUT */
	pullUpDnControl(INPUT_PIN, PUD_UP);	/* pullup */
	pinMode(INPUT_PIN, INPUT);

	v = 0;
	v = digitalRead(INPUT_PIN);
	fprintf(stdout, "%d", v);	/* print read value */
#else
/* OUTPUT */
	pinMode(OUTPUT_PIN, OUTPUT);

	DPRINT("int size=%d\n", sizeof(int));
	if(strcmp(b,"") == 0) {
		fprintf(stderr, "b: Null Error\n");
		return 1;
	} else {
		v = atoi(b);
	}
	DPRINT("v=%d\n",v);
	if(v < LIMIT) {
		digitalWrite(OUTPUT_PIN, 0);
		DPRINT("0 output.\n");
	} else {
		digitalWrite(OUTPUT_PIN, 1);
		DPRINT("1 output.\n");
	}
#endif
	
	return 0;
}
