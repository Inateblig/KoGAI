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
	allrays = inpstr[11:]

	obs = []
	obs.extend([float.fromhex(x) for x in allrays])
	obs.extend([inp.vel.x, inp.vel.y])
	obs.extend([inp.hp.x, inp.hp.y])
	obs.extend([inp.hs, inp.njum])

	rwds = [int(i) for i in inpstr[6:11]] if retrwds else None
	return obs, inp, rwds

maxvel = 6000 / 32
maxhooklen = 800 / 32
firstobs = [0] * (glb.totalrays + 6)

alow = np.array([-1, -1, 0, 0])
ahigh = np.array([1, 1, 1, 1])

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
	fin = 0
	fout = 0
	hasstarted = False
	hasfinished = False
	spdthres = 13
	isdone = False
	rwdspeed = 0
	rwdfreeze = 0
	rwdstart = 0
	rwdfinish = 0
	rwdoldarea = 0
	rwdnewarea = 0
	rwdcurarea = 0
	rwdjump = 0
	rwdckpnt = 0
	rwdhook = 0
	rwdtimealive = 0
	totalrwd = 0
	prevrwd = 0
	hook_time = 0
	hookstarted = False
	time_alive = 0
	reset_time = 0
	n = 0
	i = 0
	def __init__(self):
		super(KoGEnv, self).__init__()
		self.action_space = spaces.Box(alow, ahigh, dtype=np.float32)
		self.observation_space = spaces.Box(olow, ohigh, dtype=np.float32)

		glb.lock.acquire()
		self.i = glb.fifoi
		self.fout = glb.fifofs[self.i][0]
		self.fin = glb.fifofs[self.i][1]
		glb.fifoi += 1
		glb.lock.release()
		if not os.path.exists(glb.logdir):
			os.makedirs(glb.logdir)
		self.file_writer = tf.summary.create_file_writer(glb.logdir + f"/Env{self.i + 1:02}")

	def step(self, actn):
		info = {}

		dir = int(actn[0] * 2)
		ms_distance = 200
#		tx = int(actn[1] * ms_distance)
#		ty = int(actn[2] * ms_distance)
		tx = int(math.sin(actn[1] * math.pi) * ms_distance)
		ty = int(-math.cos(actn[1] * math.pi) * ms_distance)
#		print(f"{tx:05.03f}\t{ty:05.03f}")
		jump = int(actn[2] * 2)
		hook = int(actn[3] * 2)
#		jump = int(actn[3] * 2)
#		hook = int(actn[4] * 2)

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

		fifowrite(self.fout, dir, tx, ty, jump, hook, 0, False)

		obs, inp, rwds = getobsinprwd(self.fin, True)

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
		if rwds[3] < 0:
			self.rwdoldarea += glb.oldareaw * (rwds[3] - 1)
		elif rwds[3] > 0:
			self.rwdnewarea += glb.newareaw
		else:
			self.rwdcurarea += glb.curareaw

		if (abs(math.sqrt(inp.vel.x**2 + inp.vel.y**2))) >= self.spdthres:
			self.rwdspeed += glb.speedw
		else:
			self.rwdspeed += -glb.speedw * 0.5
		if jump > 0:
			self.rwdjump += glb.jumpw

		if rwds[4] == 1:
			self.rwdckpnt += glb.ckpntw

		if self.time_alive > glb.mintimealive:
			self.rwdtimealive += glb.timealivew #* max(1, self.time_alive)

		self.totalrwd = self.rwdfreeze + self.rwdstart + self.rwdfinish + self.rwdspeed + \
			self.rwdoldarea + self.rwdnewarea + self.rwdjump + self.rwdckpnt + \
			self.rwdhook + self.rwdtimealive
		reward = self.totalrwd - self.prevrwd
		self.prevrwd = self.totalrwd

		self.time_alive = time() - self.reset_time
		if self.n % 100 == 0:
			with self.file_writer.as_default():
				tf.summary.scalar("info/time_alive", data=self.time_alive, step=self.n)
				tf.summary.scalar("individual_rewards/time_alive", data=self.rwdtimealive, step=self.n)
				tf.summary.scalar("individual_rewards/freeze", data=self.rwdfreeze, step=self.n)
				tf.summary.scalar("individual_rewards/start", data=self.rwdstart, step=self.n)
				tf.summary.scalar("individual_rewards/finish", data=self.rwdfinish, step=self.n)
				tf.summary.scalar("individual_rewards/oldarea", data=self.rwdoldarea, step=self.n)
				tf.summary.scalar("individual_rewards/newarea", data=self.rwdnewarea, step=self.n)
				tf.summary.scalar("individual_rewards/curarea", data=self.rwdcurarea, step=self.n)
				tf.summary.scalar("individual_rewards/speed", data=self.rwdspeed, step=self.n)
				tf.summary.scalar("individual_rewards/jump", data=self.rwdjump, step=self.n)
				tf.summary.scalar("individual_rewards/ckpnt", data=self.rwdckpnt, step=self.n)
				tf.summary.scalar("individual_rewards/hook", data=self.rwdhook, step=self.n)
				tf.summary.scalar("total_rewards/reward_sum", data=self.prevrwd, step=self.n)
				tf.summary.scalar("total_rewards/reward", data=reward, step=self.n)

		print(f"{self.n:6}\r", end = '')
		if self.n % glb.nstp == 0 and self.n != 0:
			fifowrite(self.fout, 0, 100, 0, 0, 0, 1, False)
			obs = getobsinprwd(self.fin, False)[0]
			reward = 0 # ??

		self.n += 1

		done = self.isdone
		glb.totalrwd = self.totalrwd

		return np.array(obs), reward, done, info

	def reset(self):
		fifowrite(self.fout, 0, 100, 0, 0, 0, 1, False)
		obs = getobsinprwd(self.fin, False)[0]

		self.reset_time = time()
		self.time_alive = 0
		self.isdone = False
		self.hasstarted = False
		self.hasfinished = False

		return np.array(firstobs)  # reward, done, info can't be included

#	def close(self):
