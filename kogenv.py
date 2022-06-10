import numpy as np
import gym
import glb
import math
from gym import spaces

class vec2:
	x: float
	y: float
	def __init__(self, x = 0, y = 0):
		self.x = x
		self.y = y


#class Input:
#	pp: vec2
#	vel: vec2
#	hp: vec2 # hook pos
#	hs: int # hook state
#	dir: int
#	njum: int
#	def __init__(self):
#		pp = vec2()
#		vel = vec2()
#		hp = vec2()
#		hs = 0
#		dir = 0
#		njum = 0

class Input:
	pp = vec2()
	vel = vec2()
	hp = vec2() # hook pos
	hs = 0 # hook state
	dir = 0
	njum = 0


def v2tolist(v: vec2):
	return [v.x, v.y]

def getinput(strnums):
	i = iter(strnums)
	def getn():
		return int(next(i))
	inp = Input()
	inp.pp.x = getn()
	inp.pp.y = getn()
	inp.vel.x = getn()
	inp.vel.y = getn()
	inp.hp.x = getn()
	inp.hp.y = getn()
	inp.hs = getn()
	inp.dir = getn()
	inp.njum = getn()
	return inp

def getmaptiles(appto, map, px, py, r):
	r2 = r // 2
	fy = int(py) - r2
	fx = int(px) - r2

	for y in range(r):
		my = fy + y
		for x in range(r):
			mx = fx + x
			if my < 0 or my >= len(map) or \
			mx < 0 or mx >= len(map[my]):
				t = 0 # air tile
			else:
				t = map[my][mx]
			appto.append(t)

def fifowrite(fout, dir, tx, ty, j, h, sk, pr):
	out = ("{} {} {} {} {} {}\n".format(
		dir, # dir
		tx, # trgx
		ty, # trgy
		j, # jump
		h, # hook
		sk)) # selfkill
	fout.write(out)
	fout.flush()
	if pr:
		print(out)

maxvel = 6000
maxhooklen = 1600
nangles = 180
vision = 32
firstobs = [0] * (vision **2 + 9)

class KoGEnv(gym.Env):
#	Custom Environment that follows gym interface
#	metadata = {'render.modes': ['human']}
	fin = 0
	fout = 0
	hasstarted = False
	hasfinished = False
	spdthres = 11
	rwdsum = 0
	rwdfreeze = 0
	rwdstart = 0
	rwdfinish = 0
	rwdspeed = 0
	isdone = False
	i = 0
	def __init__(self):
		super(KoGEnv, self).__init__()
		# Define action and observation space
		# They must be gym.spaces objects
		# Example when using discrete actions:
		self.action_space = spaces.MultiDiscrete([3,nangles,2,2])
		# Example for using image as input (channel-first; channel-last also works):
		self.observation_space = spaces.MultiDiscrete([256] * vision**2 +
		[
		# ppos
		32, 32,
		# pvel
		maxvel * 2, maxvel * 2,
		# hpos
		maxhooklen * 2, maxhooklen * 2,
		# hookstate, dir, njum
		7, 3, 3
		])

		print("woho")
		glb.lock.acquire()
		self.i = glb.fifoi
		print(f"i: {glb.fifoi}")
		self.fin, self.fout = glb.fifofs[glb.fifoi]
		glb.fifoi += 1
		glb.lock.release()

	def step(self, actn):
#		global hasstarted, hasfinished, rwdsum, rwdfreeze, rwdstart, rwdfinish, rwdspeed, isdone

		info = {}

		angle = actn[1] / 180 * math.pi * 360 / nangles
		ms_distance = 200
		tx = int(math.sin(angle) * ms_distance)
		ty = int(math.cos(angle) * ms_distance)
		fifowrite(self.fout, actn[0] - 1, tx, ty, actn[2], actn[3], 0, False)

		inputs = self.fin.readline().split()
#		print(inputs)
		input = inputs[0:9]
		rwds = inputs[9:12]
		inp = getinput(input)
		print(inp.vel.x)

		obs = []
		getmaptiles(obs, glb.map, inp.pp.x // 32, inp.pp.y // 32, vision)
		obs.extend([c % 32 for c in v2tolist(inp.pp)])
		obs.extend([c + maxvel for c in v2tolist(inp.vel)])
		obs.extend([inp.hp.x - inp.pp.x + maxhooklen, inp.hp.y - inp.pp.x + maxhooklen])
		obs.extend([inp.hs + 1, inp.dir + 1, inp.njum])
		print(obs[vision**2:])

		if (int(rwds[0]) == 1):
			self.rwdfreeze -= glb.freezew
			print("donefreeze")
			self.isdone = True
		if (int(rwds[1]) == 1 and self.hasstarted == False):
			self.rwdstart += glb.startw
			self.hasstarted = True
		if (int(rwds[2]) == 1 and self.hasfinished == False):
			self.rwdfinish += glb.finishw
			self.hasfinished = True
			self.isdone = True
		if ((abs(math.sqrt(inp.vel.x**2 + inp.vel.y**2))) >= self.spdthres):
			self.rwdspeed += glb.speedw
		else:
			self.rwdspeed -= glb.speedw * 0.5
		self.rwdsum = self.rwdfreeze + self.rwdstart + self.rwdfinish + self.rwdspeed
		print("freeze", self.rwdfreeze, \
		"start", self.rwdstart, \
		"finish", self.rwdfinish, \
		"speed", self.rwdspeed, \
		"rwdsum", self.rwdsum,
		"self.i", self.i)

		reward = self.rwdsum

		done = self.isdone
#		print(v2tolist(inp.pp), v2tolist(inp.vel), v2tolist(inp.hp), inp.hs + 1, inp.dir + 1, inp.njum)
		return np.array(obs), reward, done, info

	def reset(self):
#		global hasstarted, hasfinished
		fifowrite(self.fout, 0, 100, 0, 0, 0, 1, False)
		self.isdone = False
		self.hasstarted = False
		self.hasfinished = False
#		return np.array(obs)  # reward, done, info can't be included
		return np.array(firstobs)  # reward, done, info can't be included

#	def close(self):
