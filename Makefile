all: main.o UDP_socket.o kad_util.o
	g++ -std=c++11 -pthread -o main main.o UDP_socket.o kad_util.o
	-cp main ../node1/main
main.o: main.cpp
	g++ -std=c++11 -pthread -c main.cpp

UDP_socket.o: UDP_socket.cpp UDP_socket.hpp
	g++ -std=c++11 -pthread -c UDP_socket.cpp

TCP_socket.o: TCP_socket.cpp TCP_socket.hpp
	g++ -std=c++11 -pthread -c TCP_socket.cpp
	
kad_util.o: kad_util.cpp kad_util.hpp
	g++ -std=c++11 -pthread -c kad_util.cpp

debug:	main.o TCP_socket.o
	g++ -std=c++11 -pthread -g -o main main main.o UDP_socket.o

clean:
	-rm -f kad_util.o
	-rm -f UDP_socket.o
	-rm -f TCP_socket.o
	-rm -f main.o
	-rm -f main