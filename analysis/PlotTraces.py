import pandas as pd
import matplotlib.pyplot as plt
import numpy as np
import h5py
import re
import matplotlib
matplotlib.rcParams.update({'font.size': 22})

#import matplotlib
#
#matplotlib.rcParams.update({
#    "pgf.texsystem": "pdflatex",
#    #"font.family": "phvr7t",
#    "font.size": "20",
#    "text.usetex": True,
#    "pgf.rcfonts": False,
#    "pgf.preamble": r"\usepackage{amsmath}",
#    "text.latex.preamble": r"\usepackage{amsmath}\n \usepackage{helvet} \renewcommand{\familydefault}{\sfdefault} \fontfamily{phv}\selectfont",
#})
#

def get_sources_from_log(log_filename):
    source_re = re.compile("param_source_([xyz])_(\d)\s+:\s+([+-.e\d]+)")
    seissol_re = re.compile("Running SeisSol on index 0")
    chain_idx = 0
    sources = {}
    letter_to_id = {'x': 2, 'y': 1, 'z': 0}
    with open(log_filename) as log_file:
        for line in log_file.readlines():
            result_seissol = seissol_re.search(line)
            if result_seissol:
                chain_idx += 1
                if not chain_idx in sources.keys():
                    sources[chain_idx] = {}
            result_source = source_re.search(line)
            if result_source:
                g = result_source.groups()
                fused_idx = int(g[1])
                if not fused_idx in sources[chain_idx].keys():
                    sources[chain_idx][fused_idx] = np.zeros(3)
                dim = g[0]
                val = float(g[2])
                sources[chain_idx][fused_idx][letter_to_id[dim]] = val
    return sources

def get_samples_from_chain(chain_filename):
    f = h5py.File(chain_filename, 'r')
    samples = f["samples"]
    return samples

def find_source_ids_from_chain(chain_filename, log_filename):
    sources = get_sources_from_log(log_filename)
    samples = get_samples_from_chain(chain_filename) 
    number_of_samples = samples.shape[1]

    ids = []
    for i in range(number_of_samples):
        source_position = samples[:,i]
        for chain_idx in sources.keys():
            for fused_idx in sources[chain_idx].keys():
                if np.linalg.norm(sources[chain_idx][fused_idx] - source_position) < 1e-3:
                    ids.append((chain_idx, fused_idx))
                    break
    return ids

def read_receiver(filename):
    return pd.read_csv(filename, header=None, skiprows=5, sep="\s+")

def get_sub_receiver(receiver_df, i=0):
    indices = [0, i*9+1, i*9+2, i*9+3, i*9+4, i*9+5, i*9+6, i*9+7, i*9+8, i*9+9]
    sub_receiver = receiver_df.loc[:,indices].copy(deep=True)
    sub_receiver.columns = [0, 1, 2, 3, 4, 5, 6, 7, 8, 9]
    return sub_receiver

def read_receiver_collection(filenames):
    receivers = [read_receiver(fn) for fn in filenames]
    return receivers

def plot_receivers(receiver_dfs, reference_dfs):
    num_receivers = len(receiver_dfs)
    num_simulations = len(receiver_dfs[0])
    fig, axes = plt.subplots(num_receivers, 2, figsize=(10, 8))

    for j in range(num_receivers):
        axes[j][0].set_ylabel(f"receiver {j+1}")
    for j in range(2):
        axes[num_receivers-1][j].set_xlabel(f"time (s)")
    for ax_idx, q_idx in enumerate([0, 2]):
        axes[0][ax_idx].set_title(f"$v_{q_idx+1}$")

    for ax_idx, q_idx in enumerate([0, 2]):
        for j in range(num_receivers):
            for k in range(num_simulations):
                axes[j][ax_idx].plot(receiver_dfs[j][k][0], receiver_dfs[j][k][7+q_idx], c="tab:blue", linewidth=0.01)
            axes[j][ax_idx].plot(reference_dfs[j][0], reference_dfs[j][7+q_idx], c="tab:red")
    fig.align_ylabels(axes[:, 0])
    plt.tight_layout()
    plt.savefig("traces.png")
    plt.show()

receiver_dfs = [read_receiver_collection([f"../rapidTesting/output/chain/receiver-{receiver_idx}-chain-{chain_idx:05d}.dat" for receiver_idx in range(1, 5)]) for chain_idx in range(1, 82)]
reference_dfs = read_receiver_collection([
    "../rapidTesting/observations-short/output-receiver-00001-00000.dat",
    "../rapidTesting/observations-short/output-receiver-00002-00000.dat",
    "../rapidTesting/observations-short/output-receiver-00003-00000.dat",
    "../rapidTesting/observations-short/output-receiver-00004-00000.dat",
    ])

print(len(receiver_dfs))

log_filename = "../linux-cluster/logs/UQ-SeisSol.141047.out"
chain_filename = '../linux-cluster/test-8.h5'
ids = find_source_ids_from_chain(chain_filename, log_filename)
print(ids)
num_unique_samples = len(set(ids))
num_samples = len(ids)
print(num_unique_samples, num_samples, num_unique_samples / num_samples)

ordered_receiver_dfs = {0: [], 1: [], 2: [], 3: []}
for chain_idx, fused_idx in ids:
    for receiver_idx in range(4):
        #print(chain_idx, receiver_idx, fused_idx)
        ordered_receiver_dfs[receiver_idx].append(get_sub_receiver(receiver_dfs[chain_idx-1][receiver_idx], fused_idx-1))
print(len(ordered_receiver_dfs))
print(len(ordered_receiver_dfs[0]))

#fused_receiver_dfs = [[get_sub_receiver(r, fused_idx) for r in receiver_dfs] for fused_idx in range(8)]
plot_receivers(ordered_receiver_dfs, reference_dfs)


