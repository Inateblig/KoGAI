import os
import numpy as np
import gym
import glb
import math
from gym import spaces
from time import time
from stable_baselines3.common.callbacks import BaseCallback
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
	hs = 0 # hook state
	njum = 0

def getinput(strnums):
	i = iter(strnums)
	def getn():
		return int(next(i))
	inp = Input()
	inp.vel.x = getn() / 32
	inp.vel.y = getn() / 32
	inp.hp.x = getn() / 32
	inp.hp.y = getn() / 32
	inp.hs = getn()
	inp.njum = getn()
	return inp

def fifowrite(fout, dir, tx, ty, j, h, sk, pr):
	out = f"{dir} {tx} {ty} {j} {h} {sk}\n"
	fout.write(out)
	fout.flush()
	if pr:
		print(out, end='')

def getobsinprwd(fin, retrwds):
	inpstr = fin.readline().split()
	inp = getinput(inpstr[0:6])
	allrays = inpstr[10:]

	obs = []
	obs.extend([float.fromhex(x) for x in allrays])
	obs.extend([inp.vel.x, inp.vel.y])
	obs.extend([inp.hp.x, inp.hp.y])
	obs.extend([inp.hs, inp.njum])

	rwds = [int(i) for i in inpstr[6:10]] if retrwds else None
	return obs, inp, rwds

maxvel = 6000 / 32
maxhooklen = 800 / 32
firstobs = [0] * (glb.totalrays + 6)

#alow = np.array([-1, -1, 0, 0])
#ahigh = np.array([1, 1, 0, 1])
alow = np.array([-1, -1, 0])
ahigh = np.array([1, 1, 1])

olow = np.array([-1] * glb.totalrays + \
	[-maxvel, -maxvel, \
	-maxhooklen, -maxhooklen, \
	0, 0 \
	])
ohigh = np.array([1] * glb.totalrays + \
	[maxvel, maxvel, \
	maxhooklen, maxhooklen, \
	7, 3 \
	])

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
		self.file_writer = tf.summary.create_file_writer(glb.logdir + f"/Env{self.i + 1:02}")

		self.hasstarted = False
		self.hasfinished = False
		self.spdthres = 13
		self.isdone = False
		self.rwdspeed = 0
		self.rwdfreeze = 0
		self.rwdstart = 0
		self.rwdfinish = 0
		self.rwdjump = 0
		self.rwdckpnt = 0
		self.rwdhook = 0
		self.rwdtimealive = 0
		self.totalrwd = 0
		self.prevrwd = 0
		self.hook_time = 0
		self.hookstarted = False
		self.time_alive = 0
		self.reset_time = 0

	def step(self, actn):
		info = {}

		dir = int(actn[0] * 2)
		ms_distance = 200
		tx = int(math.sin(actn[1] * math.pi) * ms_distance)
		ty = int(-math.cos(actn[1] * math.pi) * ms_distance)
#		print(f"{tx:05.03f}\t{ty:05.03f}")
		jump = 0
#		jump = int(actn[2] * 2)
		hook = int(actn[2] * 2)

		if hook == 1:
			self.rwdhook += glb.hookw
			self.hookstarted = True
			self.hook_time = time()
		elif self.hookstarted:
			self.hookstarted = False
			newhook_time = time()
			hook_dt = newhook_time - self.hook_time
			if hook_dt < glb.minhooktime:
				self.rwdhook += glb.shorthookw

#		print(f"{self.n}: {self.i}: writing(1)...")
		fifowrite(self.fout, dir, tx, ty, jump, hook, 0, False)
#		print(f"{self.n} r{self.i}: reading(1)...")
		obs, inp, rwds = getobsinprwd(self.fin, True)
#		print(f"{self.n}: {self.i} done")

		if rwds[0] == 1:
			self.rwdfreeze += glb.freezew
#			print("donefreeze")
			self.isdone = True
		if rwds[1] == 1 and self.hasstarted == False:
			self.rwdstart += glb.startw
			self.hasstarted = True
		if rwds[2] == 1 and self.hasfinished == False:
			self.rwdfinish += glb.finishw
			self.hasfinished = True
#			print("finish", rwdfinish, "self.i", self.i)
			self.isdone = True
		if rwds[3] >= 1:
			self.rwdckpnt += glb.finishw * rwds[3] /25

		if (abs(math.sqrt(inp.vel.x**2 + inp.vel.y**2))) >= self.spdthres:
			self.rwdspeed += glb.speedw
		else:
			self.rwdspeed += -glb.speedw * 0.5
		if jump > 0:
			self.rwdjump += glb.jumpw

		if self.time_alive > glb.mintimealive:
			self.rwdtimealive += glb.timealivew #* max(1, self.time_alive)

		self.totalrwd = self.rwdfreeze + self.rwdstart + self.rwdfinish + self.rwdspeed + \
			self.rwdjump + self.rwdckpnt + self.rwdhook + self.rwdtimealive
		reward = self.totalrwd - self.prevrwd
		self.prevrwd = self.totalrwd

		self.time_alive = time() - self.reset_time
		if self.n % 500 == 0:
			with self.file_writer.as_default():
				tf.summary.scalar("info/time_alive", data=self.time_alive, step=self.n)
				tf.summary.scalar("individual_rewards/time_alive", data=self.rwdtimealive, step=self.n)
				tf.summary.scalar("individual_rewards/freeze", data=self.rwdfreeze, step=self.n)
				tf.summary.scalar("individual_rewards/start", data=self.rwdstart, step=self.n)
				tf.summary.scalar("individual_rewards/finish", data=self.rwdfinish, step=self.n)
				tf.summary.scalar("individual_rewards/speed", data=self.rwdspeed, step=self.n)
				tf.summary.scalar("individual_rewards/jump", data=self.rwdjump, step=self.n)
				tf.summary.scalar("individual_rewards/ckpnt", data=self.rwdckpnt, step=self.n)
				tf.summary.scalar("individual_rewards/hook", data=self.rwdhook, step=self.n)
				tf.summary.scalar("total_rewards/reward_sum", data=self.prevrwd, step=self.n)
				tf.summary.scalar("total_rewards/reward", data=reward, step=self.n)

		print(f"{self.n:6}\r", end = '')
		if self.n % glb.nstp == 0 and self.n != 0:
#			print(f"{self.n}: {self.i}: writing(2)...")
			fifowrite(self.fout, 0, 100, 0, 0, 0, 1, False)
#			print(f"{self.n}: {self.i}: reading(2)...")
			obs = getobsinprwd(self.fin, False)[0]
#			print(f"{self.n}: {self.i}: done")
			reward = 0

		self.n += 1

		done = self.isdone
		glb.totalrwd = self.totalrwd

		return np.array(obs), reward, done, info

	def reset(self):
#		print(f"{self.n}: {self.i}: writing(3)...")
		fifowrite(self.fout, 0, 100, 0, 0, 0, 1, False)
#		print(f"{self.n}: {self.i}: reading(3)...")
		obs = getobsinprwd(self.fin, False)[0]
#		print(f"{self.n}: {self.i}: done")

		self.reset_time = time()
		self.time_alive = 0
		self.isdone = False
		self.hasstarted = False
		self.hasfinished = False

		return np.array(firstobs)  # reward, done, info can't be included

#	def close(self):
