#!/bin/sh 
find ./graph_lib/include -regex '.*\.\(cpp\|hpp\|cc\|cxx\)' -exec clang-format -i {} \;
# find ./src -regex '.*\.\(cpp\|hpp\|cc\|cxx\)' -exec clang-format -i {} \;
find ./test -regex '.*\.\(cpp\|hpp\|cc\|cxx\)' -exec clang-format -i {} \;