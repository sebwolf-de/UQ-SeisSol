import h5py
import re
import numpy as np

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

log_filename = "../linux-cluster/logs/UQ-SeisSol.141047.out"
chain_filename = '../linux-cluster/test-8.h5'
ids = find_source_ids_from_chain(chain_filename, log_filename)
print(ids)

