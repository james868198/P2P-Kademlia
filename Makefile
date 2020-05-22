all: main.o UDP_socket.o easy_file.o kad_util.o
	g++ -std=c++11 -pthread -o main main.o UDP_socket.o easy_file.o kad_util.o
	-cp main ../node1/main

main.o: main.cpp
	g++ -std=c++11 -pthread -c main.cpp

UDP_socket.o: UDP_socket.cpp UDP_socket.hpp
	g++ -std=c++11 -pthread -c UDP_socket.cpp

easy_file.o: easy_file.cpp easy_file.hpp
	g++ -std=c++11 -pthread -c easy_file.cpp

kad_util.o: kad_util.cpp kad_util.hpp
	g++ -std=c++11 -pthread -c kad_util.cpp

debug:	main.o TCP_socket.o
	g++ -std=c++11 -pthread -g -o main main.o UDP_socket.o easy_file.o kad_util.o

clean:
	-rm -f kad_util.o
	-rm -f easy_file.o
	-rm -f UDP_socket.o
	-rm -f main.o
	-rm -f main