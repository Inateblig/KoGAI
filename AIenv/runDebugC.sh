#!/bin/bash

fd=fifos
dd=.
sd=/content/drive/MyDrive/KoGAI/
aid=AIenv

cd ..

$dd/build/DDNet-AI -f $sd/settings_KoGC.cfg -F $fd/fin256 $fd/fout256 & #> svlog &
#echo "client 1"

python $aid/ai.py $fd/fin256 $fd/fout256
