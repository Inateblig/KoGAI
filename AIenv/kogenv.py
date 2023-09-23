import numpy as np
import gym
import glb
import math
from gym import spaces
import tensorflow as tf

class vec2:
	x: float
	y: float
	def __init__(self, x = 0, y = 0):
		self.x = x
		self.y = y

class Input:
	vel = vec2()
	hp = vec2() # hook pos
	pathv = vec2() #pathfinding vector
	hs = 0 # hook state

def getinput(strnums):
	i = iter(strnums)
	def getni():
		return int(next(i))
	def getnf():
		return float.fromhex(next(i))
	inp = Input()
	inp.vel.x = getnf()
	inp.vel.y = getnf()
	inp.hp.x = getnf()
	inp.hp.y = getnf()
	inp.pathv.x = getnf()
	inp.pathv.y = getnf()
	inp.hs = getnf()
	return inp

def fifowrite(fout, dir, tx, ty, j, h, sk, pr):
	out = f"{dir} {tx} {ty} {j} {h} {sk}\n"
	fout.write(out)
	fout.flush()
	if pr:
		print(out, end='')

def getobsinprwd(fin, retrwds):
	inpstr = fin.readline().split()
	inp = getinput(inpstr[0:7])
	allrays = inpstr[11:]

	obs = []
	obs.extend([float.fromhex(x) for x in allrays])
	obs.extend([inp.vel.x, inp.vel.y])
	obs.extend([inp.hp.x, inp.hp.y])
	obs.extend([inp.pathv.x, inp.pathv.y])
	obs.extend([inp.hs])

	if retrwds:
		rwds = [int(i) for i in inpstr[7:10]]
		rwds.append(float.fromhex(inpstr[10]))
	else:
		rwds = None
	return obs, inp, rwds

#alow = np.array([-1, -1, 0, 0])
#ahigh = np.array([1, 1, 0, 1])
alow = np.array([-1, -1, 0])
ahigh = np.array([1, 1, 1])

olow = np.array([-1] * glb.totalrays + \
	[-1, -1, \
	-1, -1, \
	-1, -1, \
	0])
ohigh = np.array([1] * glb.totalrays + \
	[1, 1, \
	1, 1, \
	1, 1, \
	1])

class KoGEnv(gym.Env):
	def __init__(self, id, fifofnms):
		super(KoGEnv, self).__init__()
		self.action_space = spaces.Box(alow, ahigh, dtype=np.float32)
		self.observation_space = spaces.Box(olow, ohigh, dtype=np.float32)

		self.i = id
		self.n = 0
		print(f"id: {self.i}")
		print("fifofnms:", fifofnms)
#		print("glb.fifofnms:", glb.fifofnms)
		self.fout = open(fifofnms[0], 'w')
		self.fin = open(fifofnms[1], 'r')
#		self.fout = open(glb.fifofnms[self.i][0], 'w')
#		self.fin = open(glb.fifofnms[self.i][1], 'r')
		self.file_writer = tf.summary.create_file_writer(glb.logdir + f"log_ai_rewards/Env{self.i + 1:02}")

		self.spdthres = 13
		self.isdone = False
		self.rwdfreeze = 0
		self.rwdstart = 0
		self.rwdfinish = 0
#		self.rwdhook = 0
		self.rwdcrctpath = 0
		self.totalrwd = 0
		self.prevrwd = 0
		self.hook_time = 0
		self.hookstarted = False

	def step(self, actn):
		info = {}

		dir = int(actn[0] * 2)
		ms_distance = 200
		tx = int(math.sin(actn[1] * math.pi) * ms_distance)
		ty = int(-math.cos(actn[1] * math.pi) * ms_distance)
#		print(f"{tx:05.03f}\t{ty:05.03f}")
		hook = int(actn[2] * 2)

#		if hook == 1:
#			self.rwdhook += glb.hookw
#			self.hookstarted = True
#			self.hook_time = time()
#		elif self.hookstarted:
#			self.hookstarted = False
#			newhook_time = time()
#			hook_dt = newhook_time - self.hook_time
#			if hook_dt < glb.minhooktime:
#				self.rwdhook += glb.shorthookw

#		print(f"{self.n}: {self.i}: writing(1)...")
		fifowrite(self.fout, dir, tx, ty, 0, hook, 0, False)
#		print(f"{self.n} r{self.i}: reading(1)...")
		obs, inp, rwds = getobsinprwd(self.fin, True)
#		print(f"{self.n}: {self.i} done")

		if rwds[0] == 1:
			self.rwdfreeze += glb.freezew
#			print("donefreeze")
			self.isdone = True
#		if rwds[1] == 1:
#			self.rwdstart += glb.startw
		if rwds[2] == 1:
			self.rwdfinish += glb.finishw
#			print("finish", rwdfinish, "self.i", self.i)
			self.isdone = True
		self.rwdcrctpath += rwds[3] * glb.crctpathw


#		print("pathv.x", obs[glb.totalrays + 4], "pathv.y", obs[glb.totalrays + 5])
		self.totalrwd = self.rwdfreeze + self.rwdstart + self.rwdfinish + \
			self.rwdcrctpath
			#self.rwdhook + self.rwdcrctpath
		reward = self.totalrwd - self.prevrwd
		self.prevrwd = self.totalrwd
#		print(f" step: {rwds[3] * glb.crctpathw}, sum: {self.rwdcrctpath}, step_rwds: {reward}, rwds: {self.prevrwd}")
#		print(f"obs: {obs}")

#		print(f"stp_rwd: {reward:10.7f}, sum_rwd: {self.prevrwd:10.7f}, ")

		if self.n % 5000 == 0:
			with self.file_writer.as_default():
				tf.summary.scalar("individual_rewards/freeze", data=self.rwdfreeze, step=self.n)
				tf.summary.scalar("individual_rewards/start", data=self.rwdstart, step=self.n)
				tf.summary.scalar("individual_rewards/finish", data=self.rwdfinish, step=self.n)
#				tf.summary.scalar("individual_rewards/hook", data=self.rwdhook, step=self.n)
				tf.summary.scalar("individual_rewards/correct_path", data=self.rwdcrctpath, step=self.n)
				tf.summary.scalar("total_rewards/reward_sum", data=self.prevrwd, step=self.n)
				tf.summary.scalar("step_rewards/reward", data=reward, step=self.n)
				tf.summary.scalar("step_rewards/correct_path", data=rwds[3] * glb.crctpathw, step=self.n)

#		print(f"{self.n:6}\r", end = '')

		self.n += 1
		done = self.isdone
		return np.array(obs), reward, done, info

	def reset(self):
#		print(f"{self.n}: {self.i}: writing(3)...")
		fifowrite(self.fout, 0, 100, 0, 0, 0, 1, False)
#		print(f"{self.n}: {self.i}: reading(3)...")
		obs = getobsinprwd(self.fin, False)[0]
#		print(f"{self.n}: {self.i}: done")

		self.isdone = False

		return np.array(obs)  # reward, done, info can't be included

#	def close(self):
