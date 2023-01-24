#!/bin/sh

fd=fifos
dd=.
aid=AIenv

cd ..

$dd/build/DDNet-AI -f $dd/settings_KoG.cfg -F $fd/fin256 $fd/fout256 & #> svlog &
#echo "client 1"

python $aid/ai.py $fd/fin256 $fd/fout256
