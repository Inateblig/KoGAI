import os
from sys import argv
import itertools
import numpy as np
import glb
import time
from stable_baselines3.common.env_util import make_vec_env
from stable_baselines3.common.vec_env import DummyVecEnv, SubprocVecEnv, VecEnv, VecFrameStack, VecNormalize
from array import array
from stable_baselines3 import PPO
import kogenv

if __name__ == '__main__':
	try:
		fifofnms = argv[1:]
		if len(fifofnms) < 2 or len(fifofnms) % 2 != 0:
			raise IndexError
	except IndexError:
		print("Not enough arguments given")
		print(f"usage: {argv[0]} <fifoout1>... <fifoin1>...")
		exit(1)

	glb.fifofnms = [fifofnms[i:i+2] for i in range(0, len(fifofnms), 2)]

	stamp = glb.stamp
	models_dir = f"models/{stamp}/"
	logdir = f"logs/{stamp}/"

	if not os.path.exists(models_dir):
		os.makedirs(models_dir)

	if not os.path.exists(logdir):
		os.makedirs(logdir)

	nenvs = len(glb.fifofnms)
	iters = glb.iters
	nstp = glb.nstp
	lr = glb.lr
	bs = glb.bs
	TOTALTIMESTEPS = nstp * iters * nenvs

	class EnvMaker:
		def __init__(self, i):
			self.i = i
			self.fifofnms = glb.fifofnms[i].copy()
		def __call__(self):
			return kogenv.KoGEnv(self.i, self.fifofnms)
	env = SubprocVecEnv([EnvMaker(i) for i in range(nenvs)]) #, start_method='fork')
#	env = make_vec_env(kogenv.KoGEnv, n_envs=nenvs, env_kwargs={"id": id},
#		vec_env_cls=SubprocVecEnv)
	model = PPO('MlpPolicy', env, verbose=2, tensorboard_log=logdir,
		learning_rate = lr, n_steps=nstp, batch_size = bs)
	print(model.policy)


	model.learn(total_timesteps=TOTALTIMESTEPS)
	model.save(f"{models_dir}/model0")
	print("modeldone 0")


	loadfrom = 0
#	models_dir = "models/20221003_102445"
#	Train loop
	for i in range(loadfrom,200):
		model = PPO.load(f"{models_dir}/model{i}", print_system_info=True)
		model.set_env(env)

		model.learn(total_timesteps=TOTALTIMESTEPS, reset_num_timesteps=False)
		model.save(f"{models_dir}/model{i+1}")
		print("modeldone", i)

#	Load fromm here
#	model = PPO.load(f"{models_dir}/model{loadfrom}", print_system_info=True)
#	model.set_env(env)
#	obs = env.reset()
#	while True:
#		action, _state = model.predict(obs)
#		obs, reward, done, info = env.step(action)
#		if done[0]:
#			obs = env.reset()
