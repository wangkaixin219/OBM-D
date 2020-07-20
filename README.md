# Adaptive Holding for Online Bottleneck Matching with Delays

This repository stores the source codes of the proposed reinforcement learning based algorithm to solve the *online bottleneck matching with delays* (OBM-D) problem.

## Usage of the algorithms

### Environment

gcc/g++ version: 10.1.0

OS: Ubuntu

### Hyperparameters selection

The hyperparameters for different algorithm has been provided in corresponding header files. 

### Compile and run the algorithms

#### 1. Experiments on synthetic datasets

- Compile the codes

First modify the forth line of Makefile

```Makefile
CFLAGS = -Wall -O3 -g
```

And then run the following command lines

```bash
cd algorithms/
make clean
make
```

- Run the algorithms

In synthetic datasets, we need to provide three arguments: (1) the cardinality of requests (workers); (2) the maximum time steps; (3) distribution setting. 
In our paper, the default value of the cardinality of requests (workers) is 1000; the default value of the maximum time steps is 2000; the default value of the 
distribution setting is 0. 

The value of the cardinality has 5 different values in our paper: 200, 500, 1000, 2000 and 5000; the value of the maximum time steps also has 5 different values: 
200, 500, 1000, 2000 and 5000; the distribution setting has 3 values: 0, 1 and 2, which maps to the default setting, distribution (1) and distribution (2) in our
paper. 

Thus, the command lines for running the algorithms under the different configurations are shown below:

```
Default configuration: ./OBM-D 1000 2000 0
Varying cardinality: ./OBM-D 5000 2000 0
Varying maximum time steps: ./OBM-D 1000 5000 0
Varying distribution settings: ./OBM-D 1000 2000 2
```

#### 2. Experiments on Didi datasets

- Compile the codes

First modify the forth line of Makefile

```Makefile
CFLAGS = -Wall -O3 -g -DDIDI
```

And then run the following command lines

```bash
cd algorithms/
make clean
make
```

- Run the algorithms

We need to provide the absolute path of a directory in which there are two files: (1) train.txt; (2) test.txt. We first use the data in train.txt to train our model,
and then we use the test.txt to test the performance. 

```
Run Didi dataset: ./OBM-D /PATH_TO_OBM-D/datasets/didi
```

#### 3. Experiments on Olist datasets

- Compile the codes

First modify the forth line of Makefile

```Makefile
CFLAGS = -Wall -O3 -g -DOLIST
```

And then run the following command lines

```bash
cd algorithms/
make clean
make
```

- Run the algorithms

The procedures are same as those of running on Didi datasets. We also need to provide two significant files.

```
Run Olist dataset: ./OBM-D /PATH_TO_OBM-D/datasets/olist
```

### Collect the running statistics

The code will return several lines, each line follows the following format

```
algorithm name: max cost, running time
```

