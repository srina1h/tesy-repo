rm -r build
mkdir build
cd build
cmake ..
make
cd ..
# /opt/homebrew/opt/llvm/bin/clang  -g -O0 -emit-llvm -c tests/red_black_tree.c -o tests/red_black_tree.bc
# /opt/homebrew/opt/llvm/bin/llvm-dis tests/red_black_tree.bc -o tests/red_black_tree.ll
# /opt/homebrew/opt/llvm/bin/opt -load-pass-plugin build/libSeminalInputAnalysis.so -passes=SeminalInputAnalysis -disable-output tests/red_black_tree.bc

clang -g -o0 -fpass-plugin=build/libSeminalInputFeaturesAnalysis.so -emit-llvm -c tests/bzip2.c -o test-output/bzip2.ll