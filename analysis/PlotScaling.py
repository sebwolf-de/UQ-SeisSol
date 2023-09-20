import pandas as pd
import matplotlib.pyplot as plt

scaling_df = pd.read_csv("scaling.csv")
scaling_df["node-h"] = scaling_df["time"] / 3600 * scaling_df["nodes"]
print(scaling_df)

for n in [4, 8, 16, 32, 64]:
    node_df = scaling_df.loc[scaling_df["nodes"] == n]
    baseline = node_df.loc[node_df["fused"] == 1]["time"].values
    print(baseline)

    print(1 - node_df["time"] / baseline)

fig, ax = plt.subplots(figsize=(9, 5))
for f in [1, 4, 8, 16]:
    local_df = scaling_df.loc[scaling_df["fused"] == f]
    ax.loglog(local_df["nodes"], local_df["time"] / 16, label=f, linewidth=4)

ax.set_xlabel("nodes")
ax.set_ylabel("runtime per simulation (s)")
ax.set_xticks([4, 8, 16, 32, 64])
ax.set_xticklabels([4, 8, 16, 32, 64])
ax.set_yticks([100, 200, 400, 800])
ax.set_yticklabels([100, 200, 400, 800])
ax.minorticks_off()
ax.legend()
plt.tight_layout()
plt.savefig("scaling.pdf")
#plt.show()

fig, ax = plt.subplots(figsize=(9, 5))
for f in [1, 4, 8, 16]:
    local_df = scaling_df.loc[scaling_df["fused"] == f]
    ax.semilogx(local_df["nodes"], local_df["node-h"] / 16, label=f, linewidth=4)

ax.set_xlabel("nodes")
ax.set_ylabel("node-h per simulation (h)")
ax.set_xticks([4, 8, 16, 32, 64])
ax.set_xticklabels([4, 8, 16, 32, 64])
#ax.set_yticks([100, 200, 400, 800])
#ax.set_yticklabels([100, 200, 400, 800])
ax.minorticks_off()
ax.legend()
plt.tight_layout()
plt.savefig("parallel-efficiency.pdf")
plt.show()
