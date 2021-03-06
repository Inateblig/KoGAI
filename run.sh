#!/bin/sh

mapf=ddnetAI/build/AI4.txt
n=2

for i in $(seq 1 $n)
do
	(
		cd ddnetAI/build
		FIFOOUT=fout$i FIFOIN=fin$i ./DDNet -f settings_ddnet.cfg
	) &

	#./ai.py
	#python3 checkenv.py
#	python3 -tt ai.py $mapf ddnetAI/build/fin$i ddnetAI/build/fout$i

	#python3 ai.py
	#python3 train.py
done

python3 ai.py $mapf $(
	for i in $(seq 1 $n)
	do
		echo "ddnetAI/build/fout$i ddnetAI/build/fin$i"
	done)
