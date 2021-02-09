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

```bash
./main config.yaml
```

Also see the example folder for more details.
