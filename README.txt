*** 以下の2つがイメージの分岐が起こっている最初の場所になる。(0928が白浜、1011が茅ヶ崎。1011のイメージを白浜に持っていくのを忘れたため、ひとつ前の0928を白浜でベースにした)
	0928までの白浜の成果：tempFanのcronによるCPU温度の制御
	1011までの茅ヶ崎の成果：単体の温度センサLM35DZ組立、Xivelyへのデータ送信(Python loop:CPU)、IBM Bluemixのサンプルレシピ(CPU)、USB温度計をtemperで(cronには書いていない。cronは従来のtempFanのまま)
Raspbian0928.img ... no Xively img (1023の元になったものなので IBM BluemixもAE-BME280温度湿度気圧センサもI2Cも入っていない。cronはtempFanで唯一CPU温度をやっている)
Raspbian1011.img ... Xively in img (0928にXivelyを追加。単体の温度センサは回路だけ組立てた(Raspiマガジン2015春号P.51 GPIO 8,9,10,11がCE0#,MISO,MOSI,SCLKで使用予定)取込みプログラムは書いていない。Python while loopでXivelyへのデータ送信をしている。IBM Bluemixのサンプルレシピのserviceは入っているが、ソースはまだ。USB温度計忘れて購入してUSB温度計のtemperで動かしている。)


*** 上の0928の正当な継承イメージ(1023は白浜の成果 - IBM BluemixとAE-BME280温度湿度気圧センサ - を全て含む)
	1023までの白浜の成果：IBM BluemixとAE-BME280温度湿度気圧(I2C組込み)
Raspbian1023.img ... 1021に帰ってきたshirahama(no Xively) img (0928がベース。Raspi temp/pressure/hum serviceは入っている。serviceでGPIO 2,3がSDA,SCLで使用中)(Xively無しのtempFanのメイン。usbmountが入っていない)


*** 以下の3つは途中経過のイメージ
Raspberry1029.img ... 1023 img (dd copy)
Raspberry1031_rsync.img ... 1023 + usbmount+rsync img(usbmountが不要だが入っている)
Raspberry1031_tempFan2.img ... 1023 + tempFan2 img(tempFan2(PWM制御)のみ入った。cronはtempFan2で--out=23→18に変更、--power=97追加で半分のパワー。ただしpwoff --input=24なのでcronでGPIO 18,24が使用中。service GPIO 0,1も。)


*** これが白浜の正当な継承イメージにtempFan2のみ入ったXively無しのイメージ(本流)
	1031までの茅ヶ崎の成果：tempFan2(PWM制御)のcronによるCPU温度の制御、rsyncによるバックアップ用microSDカードへのOS起動中の差分同期、Linux ddによるRaspiのみでのmicroSDカード→大容量USBメモリ(32GB:ext4)へのバックアップ、
	Ext2FsdインストールによるWindowsでのext4形式ファイルの直接読込み、GitHubへのtempFan2のアップローk1segawa)、Fan用ケーブル加工、スティックコンピュータ等のWindowsUpdate
Raspbian1031_MAIN_tempFan2_rsync.img ... Raspberry1031_tempFan2.img + rsync (これからはこれがメイン。Xively無し。Raspi temp/press/hum serviceは入っている)
