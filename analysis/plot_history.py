import argparse
import h5py
import matplotlib.pyplot as plt
from matplotlib.ticker import NullFormatter
import numpy as np

cmdLineParser = argparse.ArgumentParser()
cmdLineParser.add_argument('filename')
cmdLineArgs = cmdLineParser.parse_args()

hf = h5py.File(cmdLineArgs.filename, 'r')
samples_raw = hf.get('model_0_subchain_0_samples/samples')
samples = np.array(samples_raw)
log_target_raw = hf.get('model_0_subchain_0_samples/LogTarget')
log_target = np.array(log_target_raw)
n_samples = samples.shape[1]

quantity_names = ["rho", "mu", "lambda", "v_p", "v_s"]
expected_value = [2600, 1.04e10, 2.08e10, 4000, 2000]
fig,ax = plt.subplots(nrows=samples.shape[0]+3, ncols=1, sharex=True)
ax[0].plot(log_target[0])
ax[0].set_title("logDensity")
#plot material parameters
for i, s in enumerate(samples):
    ax[i+1].plot(s)
    ax[i+1].plot(np.ones(n_samples) * expected_value[i])
    ax[i+1].set_title(quantity_names[i])
#plot velocities
v_p = np.sqrt(np.divide(samples[2,:] + 2*samples[1,:], samples[0,:]))
ax[-2].plot(v_p)
ax[-2].plot(np.ones(n_samples) * expected_value[-2])
ax[-2].set_title(quantity_names[-2])
v_s = np.sqrt(np.divide(samples[1,:], samples[0,:]))
ax[-1].plot(v_s)
ax[-1].plot(np.ones(n_samples) * expected_value[-1])
ax[-1].set_title(quantity_names[-1])

plt.show()

