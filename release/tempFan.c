/*
* Created by kouichi Segawa
* 2015/09/15
*/
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <getopt.h>
#include "wiringPi.h"

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
#define EOD		(-1)

int main(int argc, char *argv[])
{
	int rtn;
	int v;

	FILE *fp;
	unsigned char b[S_BUF+1];
	int o_pin_no;
	int limit;

	struct option long_opts[] = {
		{"out",		1, NULL, 0},
		{"cpu",		1, NULL, 1},
		{0, 0, 0, 0}
	};

	int res = 0;
	int idx = 0;

	while((res = getopt_long_only(argc, argv, "oc", long_opts, &idx)) != EOD) {
		switch(res) {
		case 0:	/* out */
			DPRINT("out opt\n");
			DPRINT("name=%s val=%s\n",long_opts[idx].name, optarg);
			o_pin_no = atoi(optarg);
			if(o_pin_no == 0) { /* 0 : not number */
				fprintf(stderr, "Parameter not number : %s\n",optarg);
			}
			break;
		case 1: /* cpu */
			DPRINT("cpu opt\n");
			DPRINT("name=%s val=%s\n",long_opts[idx].name, optarg);
			limit = atoi(optarg);
			if(limit == 0) { /* 0 : not number */
				fprintf(stderr, "Parameter not number : %s\n",optarg);
			}
			break;
		case 'o':
		case 'c':
			break;
		}
	}

	DPRINT("idx=%d\n",idx);

	if( argc == 1 ) {
		/* default */
		o_pin_no = OUTPUT_PIN;
		limit = LIMIT;
	} else {
		if(idx == 0 || o_pin_no == 0 || limit == 0) {
			fprintf(stderr, "Usage:%s\n",argv[0]);
			fprintf(stderr, "      %s --out=<port No.> --cpu=<limit>\n",argv[0]);
			fprintf(stderr, "      %s --out <port No.> --cpu <limit>\n",argv[0]);
			return 1;
		}
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

	rtn = wiringPiSetupGpio();
	if( rtn == -1 ) {
		fprintf(stderr, "wiringPiSetupGpio(): Error (%d)\n", rtn);
		return 1;
	}

	pinMode(o_pin_no, OUTPUT);

	DPRINT("int size=%d\n", sizeof(int));
	if(strcmp(b,"") == 0) {
		fprintf(stderr, "b: Null Error\n");
		return 1;
	} else {
		v = atoi(b);
	}
	DPRINT("v=%d\n",v);
	if(v < limit) {
		digitalWrite(o_pin_no, 0);
		DPRINT("0 output.\n");
	} else {
		digitalWrite(o_pin_no, 1);
		DPRINT("1 output.\n");
	}
	
	return 0;
}
