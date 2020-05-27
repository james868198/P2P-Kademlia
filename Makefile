all: obj/main.o obj/UDP_socket.o obj/easy_file.o obj/kad_util.o
	g++ -std=c++17 -pthread -lssl -lcrypto -o output/main obj/main.o obj/UDP_socket.o obj/easy_file.o obj/kad_util.o
	-cp output/main ../node/main

obj/main.o: main.cpp
	g++ -o obj/main.o -std=c++17 -pthread -c main.cpp

obj/UDP_socket.o: UDP_socket.cpp UDP_socket.hpp
	g++ -o obj/UDP_socket.o -std=c++17 -c UDP_socket.cpp

obj/easy_file.o: easy_file.cpp easy_file.hpp
	g++ -o obj/easy_file.o -std=c++17 -c easy_file.cpp

obj/kad_util.o: kad_util.cpp kad_util.hpp
	g++ -o obj/kad_util.o -std=c++17 -c kad_util.cpp

debug:	main.o obj/UDP_socket.o obj/easy_file.o obj/kad_util.o
	g++ -std=c++17 -pthread -g -o output/main obj/main.o obj/UDP_socket.o obj/easy_file.o obj/kad_util.o

clean:
	-rm -f obj/*.o
	-rm -f output/main