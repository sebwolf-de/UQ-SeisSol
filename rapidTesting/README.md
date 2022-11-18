# Rapid Testing Example #
This is the primary testing example for UQ-SeisSol.
We use the LOH1 configuration and try to find the position of the source.
We have references for 2s and 9s runs.

# Preparations #
* Build SeisSol with `NUMBER_OF_FUSED_SIMULATIONS=4`.
* Build SeisSol-UQ
* Link SeisSol Executable and SeisSol-UQ executable in this directory.

# Run #
Run with `./main uq.yaml` or have a look at the job script.

# Possible adaptions #
* Larger mesh: Use `mesh/fine.geo` to generate a new mesh with pumgen. Adapt `parameters_0.par` accordingly.
* Longer runtime: Adapt `EndTime` in `parameters_0.par` and `ObservationDirectory` to `observations-short`.
* Different number of fused simluations: Recompile SeisSol with the wanted number of fused simulations. Adapt `NumberOfFusedSims` in `uq.yaml` and `source/source_template.dat`.
