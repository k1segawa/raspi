/*
* Created by kouichi Segawa
* 2015/09/15
* 2015/11/27
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

#define OUTPUT_PORT	(18)
/* 18(BCM2835 Lib Port No.) */
/* 1(wiringPi Lib Command Port No.) */
/* 12(Hardware Header Pin No.) */

#define S_BUF	(512)
#define LIMIT	(47000)
#define MAXLIMIT	(75000)
#define PWMMAX	(1023)
#define POWER	(990)
#define EOD		(-1)

void o_port_power(int, int);

int main(int argc, char *argv[])
{
	int rtn;
	int v;

	FILE *fp;
	unsigned char b[S_BUF+1];
	int o_port_no;
	int limit;
	int power;
	int maxlimit;

	struct option long_opts[] = {
		{"out",		1, NULL, 0},
		{"cpu",		1, NULL, 1},
		{"power",	1, NULL, 2},
		{"full",	1, NULL, 3},
		{0, 0, 0, 0}
	};

	int res = 0;
	int idx = 0;

	while((res = getopt_long_only(argc, argv, "ocpf", long_opts, &idx)) != EOD) {
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
#if 0
			if(power == 0) { /* 0 : not number */
				fprintf(stderr, "Parameter not number : %s\n",optarg);
			}
#endif
			break;
		case 3: /* full */
			DPRINT("full opt\n");
			DPRINT("name=%s val=%s\n",long_opts[idx].name, optarg);
			maxlimit = atoi(optarg);
			if(maxlimit == 0) { /* 0 : not number */
				fprintf(stderr, "Parameter not number : %s\n",optarg);
			}
			break;
		case 'o':
		case 'c':
		case 'p':
		case 'f':
			break;
		}
	}

	DPRINT("idx=%d\n",idx);

	if( argc == 1 ) {
		/* default */
		o_port_no = OUTPUT_PORT;
		limit = LIMIT;
		power = POWER;
		maxlimit = MAXLIMIT;

	} else {
		if(idx == 0 || o_port_no == 0 || limit == 0 /*|| power == 0*/ || maxlimit == 0) {
			fprintf(stderr, "Usage:%s\n",CMDNAME);
			fprintf(stderr, "      %s --out <port No.> --cpu <limit(1~)> --power <pwm(0~1023)> --full <limit(1~):pwm=1023>\n",CMDNAME);
			fprintf(stderr, "      %s --out=<port No.> --cpu=<limit(1~)> --power=<pwm(0~1023)> --full=<limit(1~):pwm=1023>\n",CMDNAME);
			fprintf(stderr, "  default:\n");
			fprintf(stderr, "  (=%s --out 18 --cpu 47000 --power 990 --full 75000)\n",CMDNAME);
			fprintf(stderr, "  (47~74.999C:little power, 75C~:full power)\n");
			fprintf(stderr, "  (little power=1~1022(not need:990))\n");
			fprintf(stderr, "  pwm port:12,13,18,19\n");
			fprintf(stderr, "  example:\n");
			fprintf(stderr, "  (47C~:little power)\n");
			fprintf(stderr, "  (=%s --out 18 --cpu 47000 --power 990 --full 47000)\n",CMDNAME);
			fprintf(stderr, "  (47C~:full power)\n");
			fprintf(stderr, "  (=%s --out 18 --cpu 47000 --power 1023 --full 47000)\n",CMDNAME);
			fprintf(stderr, "  (fan stop)\n");
			fprintf(stderr, "  (=%s --out 18 --cpu 47000 --power 0 --full 47000)\n",CMDNAME);
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
		break;
		default:
			pinMode(o_port_no, OUTPUT);
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

#if 1
	if(limit < maxlimit) {
		if(v < limit) {
			o_port_power(o_port_no, 0);
		} else if(limit <= v && v < maxlimit) {
			o_port_power(o_port_no, power);
		} else {
			o_port_power(o_port_no, PWMMAX);
		}
	} else if(limit == maxlimit) {
		/* power active */
		if(v < limit) {
			o_port_power(o_port_no, 0);
		} else {
			o_port_power(o_port_no, power);
		}
	} else {
		if(v < maxlimit) {
			o_port_power(o_port_no, 0);
		} else if(maxlimit <= v && v < limit) {
			o_port_power(o_port_no, PWMMAX);
		} else {
			o_port_power(o_port_no, power);
		}
	}
#else
	if(v < limit) {
		switch(o_port_no) {
			case 12:
			case 13:
			case 18:
			case 19:
				pwmWrite(o_port_no, 0);
			break;
			default:
				digitalWrite(o_port_no, 0);
			break;
		}
		DPRINT("0 output.\n");
	} else {
		switch(o_port_no) {
			case 12:
			case 13:
			case 18:
			case 19:
				pwmWrite(o_port_no, power);
				DPRINT("%d output.\n",power);
			break;
			default:
				digitalWrite(o_port_no, 1);
				DPRINT("1 output.\n");
			break;
		}
	}
#endif	
	return 0;
}

void o_port_power(int port_no, int p_val)
{
	if(p_val == 0) {
		switch(port_no) {
			case 12:
			case 13:
			case 18:
			case 19:
				pwmWrite(port_no, 0);
			break;
			default:
				digitalWrite(port_no, 0);
			break;
		}
		DPRINT("0 output.\n");
	} else {
		switch(port_no) {
			case 12:
			case 13:
			case 18:
			case 19:
				pwmWrite(port_no, p_val);
				DPRINT("%d output.\n",p_val);
			break;
			default:
				digitalWrite(port_no, 1);
				DPRINT("1 output.\n");
			break;
		}
	}
}
