#!/bin/sh

n=60
fd=fifos
dd=.
aid=AIenv

cd ..
$dd/build/DDNet-Server -f $dd/autoexecKoG.cfg -F \
	$(
		for i in $(seq $n)
		do
			echo $fd/fin$i
			echo $fd/fout$i
		done
	) & #>svlog &

python3 $aid/ai.py $(
	for i in $(seq $n)
	do
		echo $fd/fin$i
		echo $fd/fout$i
	done)
