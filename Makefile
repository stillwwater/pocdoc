all:
	@clang++ -Wall -Wextra -std=c++17 -O3 pocdoc.cpp -o build/pocdoc -L/usr/include/clang-c/ -lclang
