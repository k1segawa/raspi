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
#include <sys/sem.h>

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

#define SHTDWN	(30)
#define EOD		(-1)

int main(int argc, char *argv[])
{
	int rtn;
	int v;

	int i_pin_no;
	int shtdwn;

	int semid;
	key_t key;
	union semun {
		int				val;	/* SETVAL */
		struct semid_ds	*buf;	/* IPC_STAT, IPC_SET buf */
		unsigned short	*array;	/* GETALL, SETALL array */
	} ctl_arg;
	struct sembuf sops[1];

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

	key = ftok("/bin/cp", 1);
#if 1
	if((semid = semget(key, 1, 0666 | IPC_CREAT)) == -1) {
		fprintf(stderr, "semaphore get error\n");
		return 1;
	}

	sops[0].sem_num = 0;
	sops[0].sem_op = 1;
	sops[0].sem_flg = 0;
	if(semop(semid, sops, 1) == -1) {
		fprintf(stderr, "semaphore op error\n");
		semctl(semid, 0, IPC_RMID, ctl_arg);
		return 1;
	}
#else
	if((semid = semget(IPC_PRIVATE, 0, /*0600*/ IPC_CREAT | IPC_EXCL)) == -1) {
		fprintf(stderr, "semaphore get error\n");
		return 1;
	}

	ctl_arg.val = 1;
	if(semctl(semid, 0, SETVAL, ctl_arg) == -1) {
		fprintf(stderr, "semaphore ctl error\n");
		semctl(semid, 0, IPC_RMID, ctl_arg);
		return 1;
	}

	sops[0].sem_num = 0;
	sops[0].sem_op = -1;
	sops[0].sem_flg = IPC_NOWAIT;
	if(semop(semid, sops, 1) == -1) {
		fprintf(stderr, "semaphore op error\n");
		semctl(semid, 0, IPC_RMID, ctl_arg);
		return 1;
	}
#endif

	sleep(10);

	rtn = wiringPiSetupGpio();
	if( rtn == -1 ) {
		fprintf(stderr, "wiringPiSetupGpio(): Error (%d)\n", rtn);
		return 1;
	}

	pullUpDnControl(i_pin_no, PUD_UP); /* pullup */
	pinMode(i_pin_no, INPUT); 

	v = 0;  
	v = digitalRead(i_pin_no);
	fprintf(stdout, "val=%d", v);

	/*system("/sbin/shutdown");
	*/
	system("/bin/ls");
	
	return 0;
}
