/*
* Created by kouichi Segawa
* 2015/09/15
* 2015/10/28
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

#define CMDNAME	"tempFan2"

#define INPUT_PORT	(24)
/* 24(BCM2835 Lib Port No.) */
/* 5(wiringPi Lib Command Port No.) */
/* 18(Hardware Header Pin No.) */

#define OUTPUT_PORT	(23)
/* 23(BCM2835 Lib Port No.) */
/* 4(wiringPi Lib Command Port No.) */
/* 16(Hardware Header Pin No.) */

#define S_BUF	(512)
#define LIMIT	(47000)
#define PWMMAX	(1023)
#define POWER	(100)
#define EOD		(-1)

int main(int argc, char *argv[])
{
	int rtn;
	int v;

	FILE *fp;
	unsigned char b[S_BUF+1];
	int o_port_no;
	int limit;
	int power;

	struct option long_opts[] = {
		{"out",		1, NULL, 0},
		{"cpu",		1, NULL, 1},
		{"power",	1, NULL, 2},
		{0, 0, 0, 0}
	};

	int res = 0;
	int idx = 0;

	while((res = getopt_long_only(argc, argv, "ocp", long_opts, &idx)) != EOD) {
		switch(res) {
		case 0:	/* out */
			DPRINT("out opt\n");
			DPRINT("name=%s val=%s\n",long_opts[idx].name, optarg);
			o_port_no = atoi(optarg);
			if(o_port_no == 0) { /* 0 : not number */
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
		case 2: /* power */
			DPRINT("power opt\n");
			DPRINT("name=%s val=%s\n",long_opts[idx].name, optarg);
			power = atoi(optarg);
			if(power == 0) { /* 0 : not number */
				fprintf(stderr, "Parameter not number : %s\n",optarg);
			}
			break;
		case 'o':
		case 'c':
		case 'p':
			break;
		}
	}

	DPRINT("idx=%d\n",idx);

	if( argc == 1 ) {
		/* default */
		o_port_no = OUTPUT_PORT;
		limit = LIMIT;
		power = POWER;
	} else {
		if(idx == 0 || o_port_no == 0 || limit == 0 || power == 0) {
			fprintf(stderr, "Usage:%s\n",CMDNAME);
			fprintf(stderr, "      %s --out <port No.> --cpu <limit> --power <parcent>\n",CMDNAME);
			fprintf(stderr, "      %s --out=<port No.> --cpu=<limit> --power=<parcent>\n",CMDNAME);
			fprintf(stderr, "  (default:--out 23 --cpu 47000 --power 100)\n");
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

	switch(o_port_no) {
		case 12:
		case 13:
		case 18:
		case 19:
			pinMode(o_port_no, PWM_OUTPUT);
			pwmWrite(o_port_no, (int)(PWMMAX/100*power));
		break;
		default:
#if 1
			pinMode(o_port_no, OUTPUT);
#else
			softPwmCreate(o_port_no, 0/*100*/, 100);
#endif
		break;
	}

	DPRINT("int size=%d\n", sizeof(int));
	if(strcmp(b,"") == 0) {
		fprintf(stderr, "b: Null Error\n");
		return 1;
	} else {
		v = atoi(b);
	}
	DPRINT("v=%d\n",v);
	if(v < limit) {
		//digitalWrite(o_port_no, 0);
		switch(o_port_no) {
			case 12:
			case 13:
			case 18:
			case 19:
				pwmWrite(o_port_no, 0);
			break;
			default:
#if 1
				digitalWrite(o_port_no, 0);
#else
				softPwmWrite(o_port_no, 0);
#endif
			break;
		}
		DPRINT("0 output.\n");
	} else {
		//digitalWrite(o_port_no, 1);
		switch(o_port_no) {
			case 12:
			case 13:
			case 18:
			case 19:
				pwmWrite(o_port_no, PWMMAX/100*power);
			break;
			default:
#if 1
				digitalWrite(o_port_no, 1);
#else
				softPwmWrite(o_port_no, power);
#endif
			break;
		}
		DPRINT("1 output.\n");
	}
	
	return 0;
}
