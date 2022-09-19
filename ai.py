import os
from sys import argv
import itertools
import numpy as np
import glb
import time
from stable_baselines3.common.env_util import make_vec_env
from datetime import datetime
from array import array
from stable_baselines3 import PPO
#from stable_baselines3 import A2C
from kogenv import KoGEnv

fifofnms = []
try:
	mapfnm = argv[1]
	for i in range(2, len(argv), 2):
		fifofnms.append([argv[i], argv[i+1]])
except IndexError:
	print("Not enough arguments given")
	print(f"usage: {argv[0]} <mapfile> (<fifoin> <fifoout>)...")

map = []
with open(mapfnm, 'r') as mapf:
	def maperror(em):
		print(em)
		raise KeyboardInterrupt

	line = mapf.readline()
	sp = line.split()

	w = int(sp[0])
	h = int(sp[1])

	map = [None] * h

	for y in range(h):
		line = mapf.readline()

		sp = line.split()
		if len(sp) != w:
			maperror("line no. {} has not exactly {} nums".format(y+1, w))

		map[y] = array('I', [0] * w)

		for x in range(w):
			map[y][x] = int(sp[x])
glb.map = map

print(fifofnms)
glb.fifofs = []
for fnms in fifofnms:
	print(fnms)
	glb.fifofs.append([open(fnms[0], 'r'), open(fnms[1], 'w')])
	time.sleep(2)
	print(fnms)

datetime.now()
stamp = datetime.now().strftime("%Y%m%d_%H%M%S")
models_dir = f"models/{stamp}/"
logdir = f"logs/{stamp}/"

if not os.path.exists(models_dir):
	os.makedirs(models_dir)

if not os.path.exists(logdir):
	os.makedirs(logdir)

n_envs = len(glb.fifofs)
iters = 10
nstp = 1024
lr = 0.0005
bs = 2048
TOTALTIMESTEPS = nstp * iters * n_envs

env = make_vec_env(KoGEnv, n_envs=n_envs)
model = PPO('MlpPolicy', env, verbose=1, tensorboard_log=logdir,
	learning_rate = lr, n_steps=nstp, batch_size = bs)
#model = A2C('MlpPolicy', env, verbose=1, tensorboard_log=logdir,
#	learning_rate = lr, n_steps=nstp)
model.learn(total_timesteps=TOTALTIMESTEPS)
model.save(f"{models_dir}/model0")
print("modeldone 0")

#Train loop
#for i in range(0,200):
##	model = PPO.load(f"{models_dir}/model{i}", print_system_info=True)
#	model = A2C.load(f"{models_dir}/model{i}", print_system_info=True)
#	model.set_env(env)
#
#	model.learn(total_timesteps=TOTALTIMESTEPS, reset_num_timesteps=False)
#	model.save(f"{models_dir}/model{i+1}")
#	print("modeldone", i)

#Load fromm here
#i = 25
#model = PPO.load(f"models/20220610_013518/model{i}", print_system_info=True)
#model.set_env(env)
#obs = env.reset()
#for i in range(10000):
#    action, _state = model.predict(obs)
#    obs, reward, done, info = env.step(action)
#    if done:
#    	break
#obs = env.reset()
#print("modeldone", i)
