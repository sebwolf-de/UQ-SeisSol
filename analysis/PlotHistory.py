import argparse
import h5py
import matplotlib.pyplot as plt
import numpy as np
import re

cmdLineParser = argparse.ArgumentParser()
cmdLineParser.add_argument('filename')
args = cmdLineParser.parse_args()

hf = h5py.File(args.filename, 'r')
samples_raw = hf.get(f'/samples')
samples = np.array(samples_raw)
log_target_raw = hf.get(f'/LogTarget')
log_target = np.array(log_target_raw)
n_samples = samples.shape[1]

quantity_names = ["z", "y", "x", "strike"]
expected_value = [2000, 0, 0, 0]
fig,ax = plt.subplots(nrows=samples.shape[0]+1, ncols=1, sharex=True)
ax[0].plot(log_target[0])
ax[0].set_title("logDensity")

for i, s in enumerate(samples):
    ax[i+1].plot(s)
    ax[i+1].plot(np.ones(n_samples) * expected_value[i])
    ax[i+1].set_title(quantity_names[i])
plt.tight_layout()
plt.show()

