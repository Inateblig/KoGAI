#!/bin/sh

n=20
fd=fifos
dd=.
aid=AIenv

cd ..

for i in $(seq $n)
do
	$dd/build/DDNet-AI -f $dd/settings_KoG.cfg -F $fd/fin$i $fd/fout$i &
done > clientlog

python3 $aid/ai.py $(
	for i in $(seq $n)
	do
		echo $fd/fin$i
		echo $fd/fout$i
	done)
