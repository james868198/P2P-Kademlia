all: main.o TCP_socket.o kad_util.o
	g++ -std=c++11 -pthread -o main main.o TCP_socket.o kad_util.o
	
main.o: main.cpp
	g++ -std=c++11 -pthread -c main.cpp

TCP_socket.o: TCP_socket.cpp TCP_socket.hpp
	g++ -std=c++11 -pthread -c TCP_socket.cpp
	
kad_util.o: kad_util.cpp kad_util.hpp
	g++ -std=c++11 -pthread -c kad_util.cpp

debug:	main.o TCP_socket.o
	g++ -std=c++11 -pthread -g -o main main main.o TCP_socket.o

clean:
	-rm -f kad_util.o
	-rm -f TCP_socket.o
	-rm -f main.o
	-rm -f main