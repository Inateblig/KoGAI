import os
from datetime import datetime

freezew = -0.7
startw = 0.7
finishw = 10.0
hookw = 0.0015
shorthookw = -0.01
crctpathw = 0.00007

env_speed = 1
minhooktime = 0.1 / env_speed

totalrwd = 0

nrays = 8
totalrays = nrays * 2

iters = 128
nstp = 4096
bs = 2048
lr = 0.0003
ContinueTraining = True

models_dir = "models/"
logdir= "logs/"

stamp = datetime.now().strftime("%Y%m%d_%H%M%S")
loadfrom = 0

if ContinueTraining:
	stamp = "2048_4096_x10_128" #used for overwriting the previous declared stamp for loading
	loadfrom = 35 #used for continuing training

f = open(os.open("/tmp/kogai_timestamp", os.O_RDWR|os.O_CREAT, 0o600), 'r+')
try:
	os.lockf(f.fileno(), os.F_TLOCK, 0)
	f.write(stamp)
	f.flush()
except BlockingIOError:
	stamp = f.read()
	f.close()

logdir += stamp + "/"
models_dir += stamp + "/"

fifofnms = []
