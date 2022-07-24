
# usage


```bash
$ clang -fPIC -fno-rtti -Wno-deprecated -o PTA.cpp.o -c ./PTA.cpp
$ clang  -Wl,-znodelete -fno-rtti -fPIC -shared VarAnalysisPass.cpp -o  ../bin/VarAnalysisPass.so /home/szx/Documents/tools/SVF/Release-build/lib/libSvf.a  PTA.cpp.o
$ cd ../bin/
$ opt -load ./VarAnalysisPass.so -VarAnalysis ../tests/Field.bc -enable-new-pm=0 -o /dev/null



export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:/home/szx/Documents/tools/clang+llvm-13.0.0-x86_64-linux-gnu-ubuntu-20.04/lib:/home/szx/Documents/tools/SVF/Release-build/lib
export PATH=$PATH:/home/szx/Documents/tools/clang+llvm-13.0.0-x86_64-linux-gnu-ubuntu-20.04/bin
export CPP_INCLUDE_PATH=$C_INCLUDE_PATH:/home/szx/Documents/tools/clang+llvm-13.0.0-x86_64-linux-gnu-ubuntu-20.04/include:/home/szx/Documents/tools/SVF/include:/home/szx/Documents/tools/SVF/z3.obj/include






```
