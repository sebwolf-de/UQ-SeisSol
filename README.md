# UQ-SeisSol

### Prerequisites
Install yaml-cpp and eigen3 e.g. through your favorite package manager

Install the muq library: http://muq.mit.edu/

### Building

```bash
cmake -DMUQ_DIR:PATH=/path/to/muq2/build/ ..
make
```

### Running

In the example folder there is a working setup.
UQ-SeisSol need `$SLURM_NTASKS` to be set.

1. Create the meshes using `gmsh` and `pumgen`.
2. Convert the source using `rconv`.
3. `./main uq.yaml`
