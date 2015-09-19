crontab -e
* * * * * /usr/bin/sudo /home/pi/tempFan --out=23 --cpu=47000 1> /dev/null 2>/dev/null
* * * * * /usr/bin/sudo /home/pi/pwoff --input=24 --time=120 1> /dev/null 2>/dev/null
