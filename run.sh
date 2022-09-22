#!/bin/sh

mapf=ddnetAI/build/AipGoresAI2checkpoints.txt
n=1

for i in $(seq 1 $n)
do
	(
		cd ddnetAI/build
		FIFOOUT=fout$i FIFOIN=fin$i ./DDNet-AI -f settings_ddnet.cfg #> hiha$i
	) &

	#./ai.py
	#python3 checkenv.py
#	python3 -tt ai.py $mapf ddnetAI/build/fin$i ddnetAI/build/fout$i

	#python3 ai.py
	#python3 train.py
done

python3 ai.py $(
	for i in $(seq 1 $n)
	do
		echo "ddnetAI/build/fout$i ddnetAI/build/fin$i"
	done)
