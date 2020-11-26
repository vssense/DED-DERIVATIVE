options = -Wall -Wextra -w

input = derivative.txt

run : derivative.exe
	derivative.exe $(input)

derivative.exe : derivative.o derivative_tree.o derivative.h
	g++ derivative.o derivative_tree.o -o derivative.exe $(options)

derivative.o : derivative.cpp derivative.h
	g++ -c derivative.cpp -o derivative.o $(options)

derivative_tree.o : derivative_tree.cpp derivative.h
	g++ -c derivative_tree.cpp -o derivative_tree.o $(options)

