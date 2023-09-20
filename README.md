# UQ-SeisSol - FusedGMHKernel Version
This is the `UQ_SeisSol` version that takes an `FusedGMHKernel` approach to integrate fused SeisSol simulations into the UQ wrapper.
The program starts only a single MCMC chain.
It proposes N samples and integrates M in each iteration into the chain.
M and N can be adapted in `main.cpp`.

This version requires the changes of the muq library that can be found on the fused-simulations branch of: https://github.com/sebwolf-de/MUQ/commits/fused-simulations

The fused simulation feature of SeisSol is not always perfectly supported, see https://github.com/SeisSol/SeisSol/pull/385. If in doubt do not compile with the master branch but with `fix-multiple-simulations`.

### Additional steps for FusedGMHKernel version
Use a SeisSol executable compiled with the same number of fused simulations as specified in `uq.yaml`. Therefore, use the `uq.yaml` file in `rapidTesting` as a template.
Also, the number of sources in `source/source_template.dat` needs to match the number in `uq.yaml`.
For each fused simulation, the total number of sample points given in `source/source_template.dat` (20,000 in rapidTesting) needs to be appended to the `source/source_template.dat` file (e.g., for 8 fused simulations a total of 160,000 sample points are needed). 
The inputs in `uq.yaml`and `source/source_template.dat` of `rapidTesting` are configured to run 8 fused simulations with three coordinates each.
To reduce the number of coordinates, remove the entry in `uq.yaml` and replace all respective entries in `source_template.dat` by 0.0.
The first value of the initial paramters in `uq.yaml` (e.g., [2000, 40000]) gives the mean (2000 in this case) and the second the variance (40000 in this case) for the coordinate.

### Running

In the `rapidTesting` folder there is a working setup.
UQ-SeisSol need `$SLURM_NTASKS` to be set.

1. Create the meshes using `gmsh` and `pumgen`.
2. `./main uq.yaml`
