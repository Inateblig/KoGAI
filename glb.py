import threading
from datetime import datetime

stamp = datetime.now().strftime("%Y%m%d_%H%M%S")

nenvs = 25

freezew = -1
startw = 0.5
finishw = 9.0
speedw = -0.0001
oldareaw = 0.0
newareaw = 0.0
jumpw = -0.01
ckpnt = 0.1
hookw = 0.0015
shorthookw = -0.01
timealivew = 0.01

env_speed = 1
minhooktime = 0.1 / env_speed
mintimealive = 1 / env_speed

totalrwd = 0

nrays = 1
totalrays = nrays * 2
logdir = f"logs/{stamp}/log_ai_rewards"

fifoi = 0 # fifo index available for use
lock = threading.Lock()
fifofs = []
