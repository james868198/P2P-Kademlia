all: obj/main.o obj/UDP_socket.o obj/easy_file.o obj/kad_util.o
	g++ -std=c++17 -pthread \
	-o output/main obj/main.o obj/UDP_socket.o obj/easy_file.o obj/kad_util.o -lssl -lcrypto
	-cp output/main ../node/main

obj/main.o: main.cpp
	g++ -std=c++17 -pthread -o obj/main.o -c main.cpp

obj/UDP_socket.o: UDP_socket.cpp UDP_socket.hpp
	g++ -std=c++17 -o obj/UDP_socket.o -c UDP_socket.cpp

obj/easy_file.o: easy_file.cpp easy_file.hpp
	g++ -std=c++17 -o obj/easy_file.o  -c easy_file.cpp

obj/kad_util.o: kad_util.cpp kad_util.hpp
	g++ -mmacosx-version-min=10.15 -std=c++17 -o obj/kad_util.o -c kad_util.cpp

debug:	main.o obj/UDP_socket.o obj/easy_file.o obj/kad_util.o
	g++ -std=c++17 -pthread -g -o output/main obj/main.o obj/UDP_socket.o obj/easy_file.o obj/kad_util.o

clean:
	-rm -f obj/*.o
	-rm -f output/main
