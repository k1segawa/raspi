pi@raspberrypi ~/raspi/tool $ sudo vi m
pi@raspberrypi ~/raspi/tool $ python m
Traceback (most recent call last):
  File "m", line 93, in <module>
    send(ADDRESS, [to_addr], msg)
  File "m", line 68, in send
    smtpobj.login(ADDRESS, PASSWARD)
  File "/usr/lib/python2.7/smtplib.py", line 613, in login
    raise SMTPAuthenticationError(code, resp)
smtplib.SMTPAuthenticationError: (534, '5.7.14 <https://accounts.google.com/ContinueSignIn?sarp=1&scc=1&plt=AKgnsbvma\n5.7.14 bid7q3ZW4Br2vQ-xxnrGvKrUDq-VqfjYX7HlZ0_j14t1UEpvC74-Jf4Fm_Q8acAbikwdWj\n5.7.14 XaPp8TAGT6AfcgDrRoLsSXaTJNw-YiiJJAElBDwUb8_FNNO8_5WGPuf0tjm7IZ4NyUwpXf\n5.7.14 2BNsxzqCdc5YVWf_Oo-Mxtm7Oao_RKK1Pow1gpdQ2BXLoPHL3gSnFv0nujxqn3pyfPqcLN\n5.7.14 P1ObVvRLHjCHQDR36DeNhaMuF4cw> Please log in via your web browser and\n5.7.14 then try again.\n5.7.14  Learn more at\n5.7.14  https://support.google.com/mail/answer/78754 ku8sm9150474pab.0 - gsmtp')
pi@raspberrypi ~/raspi/tool $ 
