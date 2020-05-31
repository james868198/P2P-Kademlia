all: print config main
	-cp output/main ../node/main

main: obj/main.o obj/UDP_socket.o obj/easy_file.o obj/kad_util.o obj/kad_bucket.o
	g++ -std=c++11 -pthread -o output/main \
	obj/main.o obj/UDP_socket.o obj/easy_file.o obj/kad_bucket.o  obj/kad_util.o -lssl -lcrypto

obj/main.o: main.cpp
	g++ -std=c++11 -pthread -o obj/main.o -c main.cpp

obj/UDP_socket.o: UDP_socket.cpp UDP_socket.hpp
	g++ -std=c++11 -o obj/UDP_socket.o -c UDP_socket.cpp

obj/easy_file.o: easy_file.cpp easy_file.hpp
	g++ -std=c++11 -o obj/easy_file.o  -c easy_file.cpp

obj/kad_util.o: kad_util.cpp kad_util.hpp
	g++ -std=c++11 -o obj/kad_util.o -c kad_util.cpp

obj/kad_bucket.o: kad_bucket.cpp kad_bucket.hpp
	g++ -std=c++11 -o obj/kad_bucket.o -c kad_bucket.cpp


debug:	config debug/main.o debug/UDP_socket.o debug/easy_file.o debug/kad_util.o debug/kad_bucket.o
	g++ -std=c++11 -pthread -g -o output/main \
	debug/main.o debug/UDP_socket.o debug/easy_file.o debug/kad_util.o debug/kad_bucket.o -lssl -lcrypto
	-cp output/main ../node/main
	
debug/main.o: main.cpp
	g++ -std=c++11 -pthread -g -o debug/main.o -c main.cpp

debug/UDP_socket.o: UDP_socket.cpp UDP_socket.hpp
	g++ -std=c++11 -g -o debug/UDP_socket.o -c UDP_socket.cpp

debug/easy_file.o: easy_file.cpp easy_file.hpp
	g++ -std=c++11 -g -o debug/easy_file.o  -c easy_file.cpp

debug/kad_util.o: kad_util.cpp kad_util.hpp
	g++ -std=c++11 -g -o debug/kad_util.o -c kad_util.cpp

debug/kad_bucket.o: kad_bucket.cpp kad_bucket.hpp
	g++ -std=c++11 -g -o debug/kad_bucket.o -c kad_bucket.cpp

print:
	-@echo ===============================================
	-@echo Start Compiling
	-@echo ===============================================

config:
	-mkdir -p obj
	-mkdir -p output
	-mkdir -p debug

clean:
	-rm -f obj/*.o
	-rm -f output/main
	-rm -f debug/*.o
