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

#define CMDNAME	"pwoff"
#define PSNAME	"ps -C "CMDNAME" --no-headers"

#define INPUT_PORT	(24)
/* 24(BCM2835 Lib Port No.) */
/* 5(wiringPi Lib Command Port No.) */
/* 18(Hardware Header Pin No.) */

#define OUTPUT_PORT	(23)
/* 23(BCM2835 Lib Port No.) */
/* 4(wiringPi Lib Command Port No.) */
/* 16(Hardware Header Pin No.) */

#define S_BUF	(512)
#define SHTDWN	(30)
#define EOD		(-1)

int main(int argc, char *argv[])
{
	int rtn;
	int v;

	FILE *fp;
	unsigned char b[S_BUF];
	char *tp;

	int i_port_no;
	int shtdwn;

	struct option long_opts[] = {
		{"input",	1, NULL, 0},
		{"time",	1, NULL, 1},
		{0, 0, 0, 0}
	};

	int res = 0;
	int idx = 0;

	while((res = getopt_long_only(argc, argv, "it", long_opts, &idx)) != EOD) {
		switch(res) {
		case 0:	/* input */
			DPRINT("input opt\n");
			DPRINT("name=%s val=%s\n",long_opts[idx].name, optarg);
			i_port_no = atoi(optarg);
			if(i_port_no == 0) { /* 0 : not number */
				fprintf(stderr, "Parameter not number : %s\n",optarg);
			}
			break;
		case 1: /* time */
			DPRINT("time opt\n");
			DPRINT("name=%s val=%s\n",long_opts[idx].name, optarg);
			shtdwn = atoi(optarg);
			if(shtdwn == 0) { /* 0 : not number */
				fprintf(stderr, "Parameter not number : %s\n",optarg);
			}
			break;
		case 'i':
		case 't':
			break;
		}
	}

	DPRINT("idx=%d\n",idx);

	if( argc == 1 ) {
		/* default */
		i_port_no = INPUT_PORT;
		shtdwn = SHTDWN;
	} else {
		if(idx == 0 || i_port_no == 0 || shtdwn == 0) {
			fprintf(stderr, "Usage:%s\n",CMDNAME);
			fprintf(stderr, "      %s --input <port No.> --time <shutdown>\n",CMDNAME);
			fprintf(stderr, "      %s --input=<port No.> --time=<shutdown>\n",CMDNAME);
			fprintf(stderr, "  (default:--input 24 --time 30)\n");
			return 1;
		}
	}

	/* No allow dup run */
	memset(b,0,S_BUF);
	DPRINT("PSNAME=%s\n",PSNAME);
	fp = popen(PSNAME, "r");
	if(fp == NULL) {
		fprintf(stderr, "fopen():Open Error.\n");
		return 1;
	} else {
		/*fgets(b, S_BUF, fp);*/
		fread(b, 1, S_BUF, fp);
		pclose(fp);
		DPRINT("val:\n%s",b);
		tp = strstr(b, CMDNAME);
		DPRINT("tp:\n%s",tp);
		DPRINT("strlen(tp)=%d\n",strlen(tp));
		if(strlen(tp) == strlen(CMDNAME)+1) {
			DPRINT("One Process running.\n");
		} else {
			/* No allow dup run */
			fprintf(stderr, "Two Processes running Error. Stop.\n");
			return 0;	/* normal */
		}
	}

	rtn = wiringPiSetupGpio();
	if( rtn == -1 ) {
		fprintf(stderr, "wiringPiSetupGpio(): Error (%d)\n", rtn);
		return 1;
	}

	pullUpDnControl(i_port_no, PUD_UP); /* pullup */
	pinMode(i_port_no, INPUT); 

	v = 0;  
	v = digitalRead(i_port_no);
	DPRINT("val=%d\n", v);

	/* SW ON */
	if(v == 0) {	/* 0=ON 1=OFF:because pullup */
		sleep(shtdwn);
		system("/usr/bin/sudo /sbin/shutdown -h now");
	}

	return 0;
}
