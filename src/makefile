.PHONY:clean

VarAnalysisPass.so: PTA.cpp.o VarAnalysis.cpp.o VarAnalysisPass.cpp
	clang  -Wl,-znodelete -fno-rtti -fPIC -shared VarAnalysisPass.cpp -o  ../bin/VarAnalysisPass.so /home/szx/Documents/tools/SVF/Release-build/lib/libSvf.a PTA.cpp.o VarAnalysis.cpp.o
PTA.cpp.o: PTA.cpp
	clang -fPIC -fno-rtti -Wno-deprecated -o PTA.cpp.o -c PTA.cpp
VarAnalysis.cpp.o:  VarAnalysis.cpp
	clang -fPIC -fno-rtti -Wno-deprecated -o VarAnalysis.cpp.o -c VarAnalysis.cpp


clean:
	rm -rf *.o