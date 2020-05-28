all: main
	-cp output/main ../node/main

main: obj/main.o obj/UDP_socket.o obj/easy_file.o obj/kad_util.o
	g++ -std=c++11 -pthread \
	-o output/main obj/main.o obj/UDP_socket.o obj/easy_file.o obj/kad_util.o -lssl -lcrypto

obj/main.o: main.cpp
	g++ -std=c++11 -pthread -g -o obj/main.o -c main.cpp

obj/UDP_socket.o: UDP_socket.cpp UDP_socket.hpp
	g++ -std=c++11 -g -o obj/UDP_socket.o -c UDP_socket.cpp

obj/easy_file.o: easy_file.cpp easy_file.hpp
	g++ -std=c++11 -g -o obj/easy_file.o  -c easy_file.cpp

obj/kad_util.o: kad_util.cpp kad_util.hpp
	g++ -std=c++11 -g -o obj/kad_util.o -c kad_util.cpp

debug:	obj/main.o obj/UDP_socket.o obj/easy_file.o obj/kad_util.o
	g++ -std=c++11 -pthread \
	-g -o output/main obj/main.o obj/UDP_socket.o obj/easy_file.o obj/kad_util.o -lssl -lcrypto

clean:
	-rm -f obj/*.o
	-rm -f output/main
