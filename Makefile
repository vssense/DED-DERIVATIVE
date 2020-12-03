options = -Wall -Wextra -w

src = src
bin = bin

input = $(src)\derivative.txt

run : $(bin)\derivative.exe
	$(bin)\derivative.exe $(input)

$(bin)\derivative.exe : $(bin)\derivative.o $(bin)\derivative_tree.o $(bin)\expr_loader.o $(src)\derivative.h
	g++ $(bin)\derivative.o $(bin)\derivative_tree.o $(bin)\expr_loader.o -o $(bin)\derivative.exe $(options)

$(bin)\derivative.o : $(src)\derivative.cpp $(src)\derivative.h
	g++ -c $(src)\derivative.cpp -o $(bin)\derivative.o $(options)

$(bin)\derivative_tree.o : $(src)\derivative_tree.cpp $(src)\derivative.h
	g++ -c $(src)\derivative_tree.cpp -o $(bin)\derivative_tree.o $(options)

$(bin)\expr_loader.o : $(src)\expression_loader.cpp $(src)\expression_loader.h
	g++ -c $(src)\expression_loader.cpp -o $(bin)\expr_loader.o 
