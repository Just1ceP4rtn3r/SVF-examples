
# usage


```bash
$ clang  -Wl,-znodelete -fno-rtti -fPIC -shared VarAnalysisPass.cpp -o  ../bin/VarAnalysisPass.so
$ cd ../bin/
$ opt -load ./VarAnalysisPass.so -VarAnalysis ../tests/Field.bc -o /dev/null

```
