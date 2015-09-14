/*
http://www.sadaji.net/Firmware/rp_pwm/index.htm
*/
#include <unistd.h>
#include "gpio.h"
int main()
{
      Gpio gpio(0);

      gpio.SetPwmByMilliSec(20, 1);//-60 deg
      gpio.StartPwm();

      usleep(500000);
      gpio.SetPwmByMilliSec(20, 1.5);// 0 deg
      gpio.StartPwm();

      usleep(500000);
      gpio.SetPwmByMilliSec(20, 2);// 60 deg
      gpio.StartPwm();

      return 0;
}