#!/bin/bash

DATE=`date +%Y.%m.%d`
BODY=mail.txt

source flower.cfg

./jpeg -o a.jpg -W $WIDTH -H $HEIGHT

for (( i=0; i<$spray_per_day; i=i+1 )) do
    ./flower 0
    ./flower 1
done

./jpeg -o a.jpg -W $WIDTH -H $HEIGHT

cat $BODY | mutt -s "$DATE 澆水照片" bsdmaillist@gmail.com -a a.jpg b.jpg
