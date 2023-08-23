#!/bin/bash
#adb=/home/ubuntu/android-sdk-linux/platform-tools/adb
tempdir=signedapk
if [ ! -d $tempdir ];then
mkdir $tempdir
fi
out=system/app/RuntimeTest/
#if[! -d $out];then
#mkdir $out
#fi
pk=platform.pk8
pem=platform.x509.pem
echo $1
if [ ! -z $1 ];then
echo "change pk file"
pk=$1"/"$pk
pem=$1"/"$pem
fi
echo $pk
echo $pem
srcapk=../bin/RuntimeTest.apk
echo $2
if [ ! -z $2 ];then
echo "change src apk file"
srcapk=../bin/$2.apk
fi
echo $srcapk
java -jar signapk.jar $pem $pk $srcapk signedapk/RuntimeTest.apk
adb remount
adb shell mkdir $out
adb push signedapk/RuntimeTest.apk $out
if [ "$3" = "reboot" ];then
echo "reboot"
adb reboot
fi
echo sucess 
sleep 3

