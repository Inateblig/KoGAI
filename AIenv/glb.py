import threading
from datetime import datetime

stamp = datetime.now().strftime("%Y%m%d_%H%M%S")

freezew = -1
startw = 0.7
finishw = 10.0
speedw = -0.0001
oldareaw = 0.01
newareaw = 0.01
curareaw = -0.001
jumpw = -0.01
ckpntw = 0.7
hookw = 0.0015
shorthookw = -0.01
timealivew = 0.00

env_speed = 1
minhooktime = 0.1 / env_speed
mintimealive = 1 / env_speed

totalrwd = 0

nrays = 8
totalrays = nrays * 2
logdir = f"../logs/{stamp}/log_ai_rewards"

fifoi = 0 # fifo index available for use
lock = threading.Lock()
fifofs = []
