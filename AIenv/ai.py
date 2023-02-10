import os
from sys import argv
import glb
from stable_baselines3.common.vec_env import SubprocVecEnv
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

	models_dir = glb.models_dir
	logdir = glb.logdir

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

	if not glb.ContinueTraining:
		model = PPO('MlpPolicy', env, verbose=2, tensorboard_log=logdir,
			learning_rate = lr, n_steps=nstp, batch_size = bs)
		print(model.policy)

		model.learn(total_timesteps=TOTALTIMESTEPS, progress_bar=True)
		model.save(f"{models_dir}/model0")
		print("modeldone 0")


#	Train loop
	for i in range(glb.loadfrom,200):
		model = PPO.load(f"{models_dir}/model{i}", print_system_info=True, env=env)
		model.set_env(env)

		model.learn(total_timesteps=TOTALTIMESTEPS, reset_num_timesteps=False, progress_bar=True)
		model.save(f"{models_dir}/model{i+1}")
		print("modeldone", i + 1)

#	Load fromm here
#	model = PPO.load(f"{models_dir}/model{glb.loadfrom}", print_system_info=True, env=env)
#	obs = env.reset()
#	while True:
#		action, _state = model.predict(obs)
#		obs, reward, done, info = env.step(action)
#		if done[0]:
#			obs = env.reset()
