crontab -e
* * * * * /usr/bin/sudo /home/pi/tempFan --out=23 --cpu=47000 1> /dev/null 2>/dev/null
* * * * * /usr/bin/sudo /home/pi/pwoff --input=24 --time=120 1> /dev/null 2>/dev/null



tempFan2:
#* * * * * /usr/bin/sudo /home/pi/raspi/tempFan --out=23 --cpu=48000 1> /dev/null 2>>/dev/null
* * * * * /usr/bin/sudo /home/pi/raspi/tempFan2 --out=18 --cpu=39000 --power 990 --full 42000 1> /dev/null 2>>/dev/null

