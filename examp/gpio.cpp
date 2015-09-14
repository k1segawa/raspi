/*
 * gpio.cpp
 *
 *  Created on: 2014/05/25
 *      Author: sunaga
 */

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>
#include "gpio.h"

#define BCM_PASSWORD		0x5A000000
#define PERI_BASE     0x20000000
#define GPIO_BASE     (PERI_BASE + 0x200000)
#define GPIO_PWM		 (PERI_BASE + 0x0020C000)
#define CLOCK_BASE	 (PERI_BASE + 0x00101000)
#define BLOCK_SIZE    4096
//PWM
#define	PWM_CONTROL 0
#define	PWM_STATUS  1
#define	PWM0_RANGE  4
#define	PWM0_DATA   5
#define	PWM1_RANGE  8
#define	PWM1_DATA   9

#define	PWM0_MS_MODE    0x0080  // Run in MS mode
#define	PWM0_USEFIFO    0x0020  // Data from FIFO
#define	PWM0_REVPOLAR   0x0010  // Reverse polarity
#define	PWM0_OFFSTATE   0x0008  // Ouput Off state
#define	PWM0_REPEATFF   0x0004  // Repeat last value if FIFO empty
#define	PWM0_SERIAL     0x0002  // Run in serial mode
#define	PWM0_ENABLE     0x0001  // Channel Enable

#define	PWM1_MS_MODE    0x8000  // Run in MS mode
#define	PWM1_USEFIFO    0x2000  // Data from FIFO
#define	PWM1_REVPOLAR   0x1000  // Reverse polarity
#define	PWM1_OFFSTATE   0x0800  // Ouput Off state
#define	PWM1_REPEATFF   0x0400  // Repeat last value if FIFO empty
#define	PWM1_SERIAL     0x0200  // Run in serial mode
#define	PWM1_ENABLE     0x0100  // Channel Enable

#define	PWMCLK_CNTL	40
#define	PWMCLK_DIV	   41
#define   PWM_FREQ		19200000.0	//(Hz)


Gpio::Gpio(int deviceNo)
{
	int fd;
    //  get GPIO buffer
	fd = open("/dev/mem", O_RDWR | O_SYNC);
	if(fd < 0)
	{
       printf("error: cannot open /dev/mem (gpio_setup)\n");
		return;
	}

	m_lpnGpio = (unsigned int *)mmap(NULL, BLOCK_SIZE,
                    PROT_READ | PROT_WRITE, MAP_SHARED,
                    fd, GPIO_BASE );

	//PWM
	m_nPwmClock = 4095; m_nPwmRange = 4096; m_pwmValue = 2048;//longest pulse, 50% duty
	m_lpnPwm  = (unsigned int  *)mmap(0, BLOCK_SIZE, PROT_READ|PROT_WRITE, MAP_SHARED, fd, GPIO_PWM) ;

	//Cclock
	// Clock control (needed for PWM)
	m_lpnClock = (unsigned int   *)mmap(0, BLOCK_SIZE, PROT_READ|PROT_WRITE, MAP_SHARED, fd, CLOCK_BASE) ;
	close(fd);
}

Gpio::~Gpio()
{
}

unsigned int g_nTblPinBit[32] =
{
	0x00000001, 0x00000002, 0x00000004, 0x00000008, 0x00000010, 0x00000020, 0x00000040, 0x00000080,
	0x00000100, 0x00000200, 0x00000400, 0x00000800, 0x00001000, 0x00002000, 0x00004000, 0x00008000,
	0x00010000, 0x00020000, 0x00040000, 0x00080000, 0x00100000, 0x00200000, 0x00400000, 0x00800000,
	0x01000000, 0x02000000, 0x04000000, 0x08000000, 0x10000000, 0x20000000, 0x40000000, 0x80000000,
};

void Gpio::SetPinMode(int pinNo, int mode)
{
    int index = pinNo / 10;
    unsigned int mask = ~(0x7 << ((pinNo % 10) * 3));
    m_lpnGpio[index] = (m_lpnGpio[index] & mask) | ((mode & 0x7) << ((pinNo % 10) * 3));
}

void Gpio::SetPinHigh(int pinNo)
{
	unsigned int pinBit = g_nTblPinBit[pinNo];
	m_lpnGpio[7] &= ~pinBit;
	m_lpnGpio[7] |= pinBit;
}

void Gpio::SetPinLow(int pinNo)
{
	unsigned int pinBit = g_nTblPinBit[pinNo];
	m_lpnGpio[10] &= ~pinBit;
	m_lpnGpio[10] |= pinBit;
}


int Gpio::GetPinData(int pinNo)
{
	unsigned int pinBit = g_nTblPinBit[pinNo];
	if( (m_lpnGpio[13] & pinBit) != 0)
	{
		return 1;
	}
	return 0;
}

int Gpio::SetPwmByFreq(double freq, double duty)
{
	if(freq  >  PWM_FREQ / 4.0)//4.8MHz
	{
		return -1;
	}
	if(freq < PWM_FREQ / (4096.0 * 4095.0))//1.14Hz
	{
		return -2;
	}
	//m_nPwmClock >= 2, get data
	int d = (int)(PWM_FREQ / freq);
	if(d < 4096)
	{
			m_nPwmClock = 2;
			m_nPwmRange = d / m_nPwmClock;
	}
	else
	{
		m_nPwmClock = (d / 4096 / 2);
		m_nPwmRange = (d / m_nPwmClock);
		if(m_nPwmRange > 4096)
		{
			m_nPwmRange /= 2;
			m_nPwmClock *=2;
		}

	}
	m_pwmValue = m_nPwmRange * duty / 100;
	return 0;
}


int Gpio::SetPwmByMicroSec(double periodTime, double dutyTime)
{
	if(periodTime == 0.0)
	{
		return -1;
	}
	if(dutyTime == 0.0)
	{
		return -2;
	}
	if(dutyTime >= periodTime)
	{
		return -3;
	}
	double freq = 1.0 / periodTime * 1000000;//usec
	double duty = dutyTime / periodTime * 100;
	return SetPwmByFreq(freq, duty);
}

int Gpio::SetPwmByMilliSec(double periodTime, double dutyTime)
{
	return SetPwmByMicroSec(periodTime * 1000.0, dutyTime * 1000.0);
}

int Gpio::StartPwm()
{
	unsigned int pwm_ctrl;

	//set PWM mode  -> gpio mode 1 pwm
	SetPinMode(18, PWM_OUTPUT);
	// gpio pwm-ms
    *(m_lpnPwm + PWM_CONTROL) = PWM0_ENABLE | PWM0_MS_MODE;
    //gpio pwmc 4095
    pwm_ctrl = *(m_lpnPwm + PWM_CONTROL) ;
    *(m_lpnPwm + PWM_CONTROL) = 0 ;// Stop PWM
    *(m_lpnClock + PWMCLK_CNTL) = BCM_PASSWORD | 0x01 ;	// Stop PWM Clock
    usleep(110) ;

    while ((*(m_lpnClock + PWMCLK_CNTL) & 0x80) != 0)	// Wait for clock to be !BUSY
    {
    	usleep (1) ;
    }

    *(m_lpnClock + PWMCLK_DIV)  = BCM_PASSWORD | (m_nPwmClock << 12) ;

    *(m_lpnClock + PWMCLK_CNTL) = BCM_PASSWORD | 0x11 ;	// Start PWM clock
    *(m_lpnPwm + PWM_CONTROL) = pwm_ctrl;		// restore PWM_CONTROL

    // gpio pwmr 4096
    *(m_lpnPwm + PWM0_RANGE) = m_nPwmRange ;
    usleep (10) ;
    *(m_lpnPwm + PWM0_DATA) = m_pwmValue ;

	return 0;
}


int Gpio::StopPwm()
{
	return 0;
}
