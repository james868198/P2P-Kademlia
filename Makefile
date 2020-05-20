all: main.o TCP_socket.o
	g++ -std=c++11 -pthread -o main main.o TCP_socket.o
	
main.o: main.cpp main.hpp
	g++ -std=c++11 -pthread -c main.cpp
	
TCP_socket.o: TCP_socket.cpp TCP_socket.hpp
	g++ -std=c++11 -pthread -c TCP_socket.cpp
	
debug:	main.o TCP_socket.o
	g++ -std=c++11 -pthread -g -o main main main.o TCP_socket.o

clean:
	-rm -f TCP_socket.o
	-rm -f main.o
	-rm -f main