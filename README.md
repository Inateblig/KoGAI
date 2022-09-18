# KoGAI
0. Apply the patch `ddnetpatchforai.diff` to a ddnet repo cloned at ddnetAI
1. You'll need to create the fifo files with `mkfifo fin1 fout1` and create as many as envs you want to run simultanously.
2. Change the number of envs with the `n=number` variable in run.sh.
3. Also change the `mapf=map.txt` with the map you want to train the AI to.
4. Generate the map.txt file by running `map\_diff map`
5. The ddnet code is on commit `926d158abcb1a11d7817411b5a4950b51ade6c86`

# This is an unfinished project due to the lack of a dgpu so I can't test it properly.
