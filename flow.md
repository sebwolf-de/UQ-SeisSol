# Flow of UQ-SeisSol

#### Very brief flow

1. `main.cpp` reads the YAML configuration file and instantiates the corresponding classes.
2. The new candidate sample is drawn according to the configuration inside `UQ::MyMIComponentFactory::Proposal`.
3. `UQ/SamplingProblem.cpp` runs SeisSol and computes the log densities.
4. The candidate sample is either accepted or rejected.
5. Repeat steps 2 to 4 until the number of samples specified inside the YAML configuration file is reached.


#### More comprehensive flow

##### 1. [main.cpp](https://github.com/sebwolf-de/UQ-SeisSol/blob/main/src/main.cpp)
- Parses the YAML configuration file passed as the first command line argument when running UQ-SeisSol.
    - Uses [IO/ParameterReader.cpp](https://github.com/sebwolf-de/UQ-SeisSol/blob/main/src/IO/ParameterReader.cpp) to assert the existence of the required parameters inside the configuration file and to read them.
    - Uses [SeisSol/UQSeisSolFactory.cpp](https://github.com/sebwolf-de/UQ-SeisSol/blob/main/src/IO/MaterialParameterWriter.cpp) to build the corresponding instances with the configuration values.
- Archives the output files from the previous run inside a directory named `output_archive`. It also makes sure that the directories that will hold the SeisSol output exist.
- Creates the `UQ/MyMIComponentFactory.cpp` instance. This is the class that configures the `MUQ` uncertainty quantification algorithm.
- Creates the `ptree` that is passed to `MUQ` for our current uncertainty quantification run. It uses the number of indexes (and the number of samples to take on each index) as specified in the configuration YAML file.
- Runs the `MUQ` algorithm.
- Writes the output to an HDF5 file.

##### 2. [SeisSol/ReceiverDB.cpp](https://github.com/sebwolf-de/UQ-SeisSol/blob/main/src/SeisSol/ReceiverDB.cpp), [SeisSol/Receiver.cpp](https://github.com/sebwolf-de/UQ-SeisSol/blob/main/src/SeisSol/Receiver.cpp), and [IO/ReceiverReader.cpp](https://github.com/sebwolf-de/UQ-SeisSol/blob/main/src/IO/ReceiverReader.cpp)
- These 3 files handle receivers.
    - `SeisSol/ReceiverDB.cpp` instances hold `SeisSol/Receiver.cpp` instances and allow you to query receivers, add receivers, and compute L1 differences.
    - `SeisSol/Receiver.cpp` instances hold the information of a receiver file. It performs the L1 difference computations, and allows you to introspect a receiver. Note that L1 differences are computed in the number of subintervals (on the time axis) desired.
    - `IO/ReceiverReader.cpp` parses receivers files from your filesystem.
- Two `SeisSol/ReceiverDB.cpp` instances are created from the start to hold the observation receivers (our reference taken from real life observations) and the simulation receivers (output from SeisSol).

##### 3. [UQ/MyMIComponentFactory.cpp](https://github.com/sebwolf-de/UQ-SeisSol/blob/main/src/UQ/MIComponentFactory.cpp)
- Specifies how to draw a new candidate sample.
    - Configures the type of sampling.
    - Configures the number of parameters to draw samples for.
    - Configures the covariance of those samples (currently dynamic with respect to the order of the initial values of the parameters). This can be modified inside the `UQ::MyMIComponentFactory::Proposal` method.
- Creates the `UQ/MySamplingProblem.cpp` instance which orchestrates the simulation runs.
- Specifies the starting values of our parameters.

##### 4. [UQ/SamplingProblem.cpp](https://github.com/sebwolf-de/UQ-SeisSol/blob/main/src/UQ/SamplingProblem.cpp)
- Updates the material parameter file that SeisSol uses using [IO/MaterialParameterWriter.cpp](https://github.com/sebwolf-de/UQ-SeisSol/blob/main/src/IO/MaterialParameterWriter.cpp).
- Prepares the filesystem for the next SeisSol run using [SeisSol/Runner.cpp](https://github.com/sebwolf-de/UQ-SeisSol/blob/main/src/SeisSol/Runner.cpp). This method moves the receiver files of the last SeisSol run from `output/current` to `output/chain` for potential introspection.
- Runs SeisSol with the candidate material parameters using `SeisSol/Runner.cpp`.
- Computes the relative norm between the observation receivers and the simulation receivers and returns it to MUQ to either accept or reject the candidate.


#### Notes about the parameter files
1. [YAML configuration file](https://github.com/sebwolf-de/UQ-SeisSol/blob/main/example/uq.yaml)
    - `SeisSolBinary` specifies the name of the SeisSol binary.
    - `MaterialFileTemplate` specifies the name of the [template file](https://github.com/sebwolf-de/UQ-SeisSol/blob/main/example/material_template.yaml) for your material parameters. What `UQ-SeisSol` does is to replace all the parameter values in the template file that have the format `@parameter_name@` with the candidate sample. The number of `@parameter_name@` tags inside this file must match the number (and names) of the initial values given in `InitialParameters`.
    - `InitialParameters` specifies the initial values for each of the material parameters.
    - `ObservationDirectory` specifies the directory where the observation receivers can be parsed from.
    - `ReceiverPrefix` specifies the root of the receiver file names.
    - `NumberOfReceivers` specifies the number of receivers.
    - `NumberOfSamples` is an array specifying the number of samples to draw per index (each element index in this array corresponds to the index of the `MUQ` index).
    - `NumberOfSubintervals` specifies the number of subintervals (on the time axis) to use when calculating the L1 difference.
2. [SeisSol parameter files](https://github.com/sebwolf-de/UQ-SeisSol/blob/main/example/parameters.par)
    - You should have one parameter file per `MUQ` index.
    - The name of the parameter files must follow the format `parameters_<Index Number>.par`. There should be one for each index specified by `NumberOfSamples` parameter in the YAML configuration file. Higher index numbers normally have a finer mesh used for the simulation.
3. [SeisSol mesh and source files](https://github.com/sebwolf-de/UQ-SeisSol/tree/main/example)
    - All required SeisSol files to run a simulations must be present.