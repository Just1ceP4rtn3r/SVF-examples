# env

> https://github.com/SVF-tools/SVF
> 
> https://github.com/SVF-tools/SVF-example

环境配置过程参考[SVF-example](https://github.com/SVF-tools/SVF-example)

```
## 0. Install npm, zlib, unzip, cmake, gcc, nodejs (skip this step if you machine has these libs)

sudo apt-get install zlib1g-dev unzip cmake gcc g++ nodejs
git clone https://github.com/SVF-tools/SVF.git
cd SVF
source ./build.sh

## 1. Install SVF and its dependence (LLVM pre-built binary) via npm

npm i --silent svf-lib --prefix ${HOME}


## 2. Clone repository

git clone https://github.com/SVF-tools/SVF-example.git


## 3. Setup SVF environment and build your project 

source ./env.sh
cmake . && make

## 4. Analyze a bc file using svf-ex executable

clang -S -c -g -fno-discard-value-names -emit-llvm example.c -o example.ll
./bin/svf-ex example.ll



## 相关库、include配置，主要包含LLVM、SVF两部分

export LLVM_DIR=/home/szx/Documents/tools/clang+llvm-14.0.0-x86_64-linux-gnu-ubuntu-18.04
export Z3_DIR=/home/szx/Documents/tools/SVF/z3.obj
export SVF_DIR=/home/szx/Documents/tools/SVF/
export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:/home/szx/Documents/tools/clang+llvm-14.0.0-x86_64-linux-gnu-ubuntu-18.04/lib:/home/szx/Documents/tools/SVF/Release-build/lib
export PATH=$PATH:/home/szx/Documents/tools/clang+llvm-14.0.0-x86_64-linux-gnu-ubuntu-18.04/bin
export CPLUS_INCLUDE_PATH=/home/szx/Documents/tools/clang+llvm-14.0.0-x86_64-linux-gnu-ubuntu-18.04/include:/home/szx/Documents/tools/SVF/include:/home/szx/Documents/tools/SVF/z3.obj/include
export C_INCLUDE_PATH=/home/szx/Documents/tools/clang+llvm-14.0.0-x86_64-linux-gnu-ubuntu-18.04/include:/home/szx/Documents/tools/SVF/include:/home/szx/Documents/tools/SVF/z3.obj/include

```



# usage

在使用前先熟悉上述官方SVF-example。

* Type-analysis.cpp

简单的SVF-tools的指针分析使用，获取名为`key_var`变量的Points-to集合。使用参考SVF-tools

* VarAnalysisPass.cpp

结合LLVM pass与SVF-tools的指针分析，输入变量名 (e.g., `Class::v1`)，输出变量名对应llvm::Value的指针分析的结果。

```bash

# VarAnalysisPass.cpp

$ cd src/
$ clang -fPIC -fno-rtti -Wno-deprecated -o PTA.cpp.o -c ./PTA.cpp
$ clang  -Wl,-znodelete -fno-rtti -fPIC -shared VarAnalysisPass.cpp -o  ../bin/VarAnalysisPass.so /home/szx/Documents/tools/SVF/Release-build/lib/libSvf.a  PTA.cpp.o
$ cd ../bin/
$ opt -load ./VarAnalysisPass.so -VarAnalysisPass ../tests/Field.bc -enable-new-pm=0 -o /dev/null


# Type-analysis.cpp

$ cmake . && make


```
