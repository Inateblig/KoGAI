import threading

map = []

freezew = -10
startw = 0.5
finishw = 9.0
speedw = 0.005
oldareaw = 0.01
newareaw = 0.01

fifoi = 0 # fifo index available for use
lock = threading.Lock()
fifofs = []
