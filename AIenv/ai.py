import os
from sys import argv
import itertools
import numpy as np
import glb
import time
from stable_baselines3.common.env_util import make_vec_env
#from datetime import datetime
from array import array
from stable_baselines3 import PPO
import kogenv

try:
	fifofnms = argv[1:]
	if len(fifofnms) < 2 or len(fifofnms) % 2 != 0:
		raise IndexError
except IndexError:
	print("Not enough arguments given")
	print(f"usage: {argv[0]} <fifoout1>... <fifoin1>...")
	exit(1)

print(fifofnms)
glb.fifofs = []
for i in range(0, len(fifofnms), 2):
	wfnm, rfnm = fifofnms[i:i+2]
	print(wfnm, rfnm)
	glb.fifofs.append([open(wfnm, 'w'), open(rfnm, 'r')])

stamp = glb.stamp
models_dir = f"models/{stamp}/"
logdir = f"logs/{stamp}/"

if not os.path.exists(models_dir):
	os.makedirs(models_dir)

if not os.path.exists(logdir):
	os.makedirs(logdir)

n_envs = len(glb.fifofs)
iters = glb.iters
nstp = glb.nstp
lr = glb.lr
bs = glb.bs
TOTALTIMESTEPS = nstp * iters * n_envs

env = make_vec_env(kogenv.KoGEnv, n_envs=n_envs)
model = PPO('MlpPolicy', env, verbose=2, tensorboard_log=logdir,
	learning_rate = lr, n_steps=nstp, batch_size = bs)
print(model.policy)

model.learn(total_timesteps=TOTALTIMESTEPS)
model.save(f"{models_dir}/model0")
print("modeldone 0")


startfrom = 0
#models_dir = "models/20220921_003524"
#Train loop
for i in range(startfrom,200):
	model = PPO.load(f"{models_dir}/model{i}", print_system_info=True)
	model.set_env(env)

	model.learn(total_timesteps=TOTALTIMESTEPS, reset_num_timesteps=False)
	model.save(f"{models_dir}/model{i+1}")
	print("modeldone", i)

#Load fromm here
#model = PPO.load(f"models/20220924_042653/model{startfrom}", print_system_info=True)
#model.set_env(env)
#obs = env.reset()
#while True:
#	action, _state = model.predict(obs)
#	obs, reward, done, info = env.step(action)
#	if done[0]:
#		obs = env.reset()
#obs = env.reset()
#print("modeldone", i)
