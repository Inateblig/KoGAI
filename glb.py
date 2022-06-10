import threading

map = []

freezew = 50
startw = 10
finishw = 100
speedw = 0.0005

fifoi = 0 # fifo index available for use
lock = threading.Lock()
fifofs = []
