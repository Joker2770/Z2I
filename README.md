# Z2I

[![MSBuild](https://github.com/Joker2770/Z2I/actions/workflows/msbuild.yml/badge.svg)](https://github.com/Joker2770/Z2I/actions/workflows/msbuild.yml)
[![Codacy Badge](https://app.codacy.com/project/badge/Grade/a0c92665548344f49f92e0d999eb5c05)](https://app.codacy.com/gh/Joker2770/Z2I/dashboard?utm_source=gh&utm_medium=referral&utm_content=&utm_campaign=Badge_grade)

The `pbrain-Z2I` is just a gomoku engine which use the [protocol of gomocup](lastovicka.github.io/protocl2en.htm). A GUI named [qpiskvork](https://github.com/Joker2770/qpiskvork) can load this engine.

## Supported Games
Currently only Gomoku and similar games such as Tic-Tac-Toe. 

Welcome other game implementions if you want to become the contributor!

## Supported rules

- [x] free-style
- [x] standard
- [x] renju
- [x] caro

## Supported OS System
Linux/Windows (tested on Ubuntu 20 + GPU/CPU and Windows 10 + GPU/CPU)


## Supported Enviroment
Both GPU and CPU (GPU test on Tesla V100 + Cuda 11 and CPU test on Intel i5/i7)


## Language
C++ (for speed!) and python. The model is trained by pytorch (Python) and onnxruntime (C++, for selfplay), and inferenced by onnxruntime (C++).


## Dependence
gcc (Linux) or visual studio 19 (windows)

cmake 3.13+

pytorch (tested on 1.11)

onnxruntime-gpu (tested on 1.11)


## Installation
Download and install miniconda / python
and `pip install` all the dependent packages such as pytorch

train.sh: convert `/data/miniconda3/bin/python` to `python` or `python3` or your own python intepreter path

Download onnxruntime: https://github.com/microsoft/onnxruntime/releases/tag/v1.11.1

CMakefiles.txt: convert the onnxruntime path to your own path


```shell
git clone --recursive https://github.com/Joker2770/Z2I.git
# git submodule update --init --recursive
cd Z2I
mkdir build
cp ./scripts/*.sh ./build/
cd ./build
cmake ..    # (or 'cmake -A x64 ..')
cmake --build . --config Release   # (or open .sln file through visual Studio 19 and generate for win10)
```

or build with xmake toolchain(In this way, `onnruntime-1.11.1` will be installed with xrepo).

```shell
git clone --recursive https://github.com/Joker2770/Z2I.git
# git submodule update --init --recursive
cd Z2I
mkdir build
cp ./scripts/*.sh ./build/
xmake config --mode=release
xmake
```

## Train (Linux)

```shell
cd ./build/
bash train.sh
...
```

If you want to train the model on windows 10, convert `train.sh` to `train.bat` and change corresponding commands.


## Human play with AI (inference)

1.  Run `pbrain-Z2I`, for example on Linux:

```shell
./pbrain-Z2I
```

There is a better way to run `pbrain-Z2I` with the GUI program named [qpiskvork](https://github.com/Joker2770/qpiskvork) besides. 

Increase or decrease `num_mct_sims` in `config.toml` (default 100) to increase the power or speed of AI.

2.  Run mcts_test, for example on Linux:

```shell
./mcts_test ./weights/1000.onnx 1
```

Here 1(or 0) = AI play with black(or white) pieces. 

Increase or decrease `NUM_MCT_SIMS` in src/common.h (default 1600) to increase the power or speed of AI.

## weight struct

![weight struct](https://github.com/Joker2770/Z2I/blob/main/Z2I_onnx.svg)
## reference
1.  [https://onnxruntime.ai/docs/execution-providers/CUDA-ExecutionProvider.html](https://onnxruntime.ai/docs/execution-providers/CUDA-ExecutionProvider.html)
