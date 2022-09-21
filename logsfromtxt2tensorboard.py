import os
import glb
import tensorflow as tf

from datetime import datetime
import json
from packaging import version
import tempfile

logdir = glb.logdir

print("TensorFlow version: ", tf.__version__)
assert version.parse(tf.__version__).release[0] >= 2, \
	"This notebook requires TensorFlow 2.0 or above."

# Sets up a timestamped log directory.
# Creates a file writer for the log directory.

for i in range(glb.nenvs):
	file_writer = tf.summary.create_file_writer(logdir + f"/Env{i+1:02}")
	with file_writer.as_default():
			logfile = f"{logdir}/Logs_{i+1:02}"
			with open(logfile, 'r') as file:
				ln = 1
				for line in file:
					s = line.split()
					for e in range(len(s) // 2):
						tf.summary.scalar(s[2*e], data=float(s[2*e+1]), step=ln)
					ln += 1

os.system('tensorboard --logdir ./log_ai_rewards/')

