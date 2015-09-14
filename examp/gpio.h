/*
 * gpio.h
 *
 *  Created on: 2014/05/25
 *      Author: sunaga
 */

#ifndef GPIO_H_
#define GPIO_H_

#define	INPUT			 0
#define	OUTPUT			 1
#define	PWM_OUTPUT		 2
#define	GPIO_CLOCK		 3


class Gpio
{
	unsigned int *m_lpnGpio, *m_lpnPwm, *m_lpnClock;

	int m_nPwmClock, m_nPwmRange, m_pwmValue;

public:
	Gpio(int deviceNo);
	~Gpio();

	void SetPinMode(int pinNo, int mode);
	void SetPinHigh(int pinNo);
	void SetPinLow(int pinNo);
	int GetPinData(int pinNo);
	//PWM
	int SetPwmByFreq(double freq, double duty);
	int SetPwmByMicroSec(double periodTime, double dutyTime);
	int SetPwmByMilliSec(double periodTime, double dutyTime);
	int StartPwm();
	int StopPwm();
};


#endif /* GPIO_H_ */
