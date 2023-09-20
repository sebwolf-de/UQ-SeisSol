import pandas as pd
import matplotlib.pyplot as plt
import numpy as np

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
    fig, axes = plt.subplots(3, num_receivers)
    for i in range(3):
        for j in range(num_receivers):
            axes[i][j].plot(receiver_dfs[j][0], receiver_dfs[j][7+i])
            axes[i][j].plot(reference_dfs[j][0], reference_dfs[j][7+i])
    plt.show()

def log_density(receiver_dfs, reference_dfs):
    relative_norm = 0
    assert len(receiver_dfs) == len(reference_dfs)
    num_receivers = len(receiver_dfs)
    for receiver_idx in range(num_receivers):
        dt = reference_dfs[receiver_idx].loc[1, 0] - reference_dfs[receiver_idx].loc[0, 0]
        rec_df = receiver_dfs[receiver_idx].loc[:,[7,8,9]]
        ref_df = reference_dfs[receiver_idx].loc[:,[7,8,9]]
        l1_diff = dt * (rec_df - ref_df).abs().sum().sum()
        l1_norm = dt * (ref_df).abs().sum().sum()
        receiver_norm = 0
        if l1_norm > 1e-2:
            receiver_norm = l1_diff / l1_norm
        else:
            receiver_norm = l1_diff
        relative_norm += receiver_norm
    relative_norm /= num_receivers
    log_density = - relative_norm**4
    return log_density

receiver_dfs = read_receiver_collection([
    "../linux-cluster/output/chain/receiver-1-chain-00001.dat",
    "../linux-cluster/output/chain/receiver-2-chain-00001.dat",
    "../linux-cluster/output/chain/receiver-3-chain-00001.dat",
    "../linux-cluster/output/chain/receiver-4-chain-00001.dat",
    ])
reference_dfs = read_receiver_collection([
    "../rapidTesting/observations-short/output-receiver-00001-00000.dat",
    "../rapidTesting/observations-short/output-receiver-00002-00000.dat",
    "../rapidTesting/observations-short/output-receiver-00003-00000.dat",
    "../rapidTesting/observations-short/output-receiver-00004-00000.dat",
    ])

for fused_idx in range(8):
    fused_receiver_dfs = [get_sub_receiver(r, fused_idx) for r in receiver_dfs]
    ld = log_density(fused_receiver_dfs, reference_dfs)
    print(ld)
    plot_receivers(fused_receiver_dfs, reference_dfs)


