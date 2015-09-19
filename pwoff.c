/*
* Created by kouichi Segawa
* 2015/09/15
*/
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <getopt.h>
#include "wiringPi.h"
#include <sys/ipc.h>
#include <sys/shm.h>
#include <errno.h>
#include <sys/time.h>
#include <time.h>

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
#define SHTDWN	(30)
#define EOD		(-1)

int main(int argc, char *argv[])
{
	int rtn;
	int v;

	FILE *fp;
	unsigned char b[S_BUF];
	char *tp;

	int i_pin_no;
	int shtdwn;

	key_t key;
	int shmid;
	struct shmid_ds shmbuf;

	struct timeval tv_old;
	struct timeval tv_new;

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
			i_pin_no = atoi(optarg);
			if(i_pin_no == 0) { /* 0 : not number */
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
		i_pin_no = INPUT_PIN;
		shtdwn = SHTDWN;
	} else {
		if(idx == 0 || i_pin_no == 0 || shtdwn == 0) {
			fprintf(stderr, "Usage:%s\n",argv[0]);
			fprintf(stderr, "      %s --input=<port No.> --time=<shutdown>\n",argv[0]);
			fprintf(stderr, "      %s --input <port No.> --time <shutdown>\n",argv[0]);
			return 1;
		}
	}

	/* No allow dup run */
	memset(b,0,S_BUF);
	fp = popen("ps -C pwoff --no-headers", "r");
	if(fp == NULL) {
		fprintf(stderr, "fopen():Open Error.\n");
		return 1;
	} else {
		/*fgets(b, S_BUF, fp);*/
		fread(b, 1, S_BUF, fp);
		pclose(fp);
		DPRINT("val:\n%s",b);
		tp = strstr(b, "pwoff");
		DPRINT("tp:\n%s",tp);
		DPRINT("strlen(tp)=%d\n",strlen(tp));
		if(strlen(tp) == strlen("pwoff")+1) {
			DPRINT("One Process running.\n");
		} else {
			/* No allow dup run */
			fprintf(stderr, "Two Processes running Error. Stop.\n");
			return 0;	/* normal */
		}
	}

#if 0
	/* shard memory create */
	/* No allow dup run */
	key = ftok("/bin/cp", 901288363);
	if((shmid = shmget(key, 1, IPC_CREAT | IPC_EXCL)) == -1) {
		if(errno == EEXIST) {
			fprintf(stderr, "EEXIST.\n");
		}
		fprintf(stderr, "Not overlap run.\n");
		return 1;
	}
#endif

	rtn = wiringPiSetupGpio();
	if( rtn == -1 ) {
		fprintf(stderr, "wiringPiSetupGpio(): Error (%d)\n", rtn);
		return 1;
	}

	pullUpDnControl(i_pin_no, PUD_UP); /* pullup */
	pinMode(i_pin_no, INPUT); 

	v = 0;  
	v = digitalRead(i_pin_no);
	DPRINT("val=%d\n", v);

	if(v == 1) {
		sleep(shtdwn);
#if 0
		timerclear(&tv_old);
		timerclear(&tv_new);
		DPRINT("tv_old=%d,tv_new=%d\n",
			(timerisset(&tv_old)),
			(timerisset(&tv_new)));
		if(gettimeofday(&tv_old, NULL)) {
			fprintf(stderr, "gettimeofday Error\n");
			return 1;
		}
#endif
		system("/usr/bin/sudo /sbin/shutdown -k");
		/*system("/bin/ls");*/
	}

#if 0
	/* shard memory destory */
	if(shmctl(shmid, IPC_RMID, &shmbuf) == -1) {
		fprintf(stderr, "shmctl Error\n");
		return 1;
	}
#endif
	
	return 0;
}
