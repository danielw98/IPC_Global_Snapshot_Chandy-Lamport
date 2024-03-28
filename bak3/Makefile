# channel.cpp  main.cpp  message.cpp  process.cpp  snapshot.cpp processcontroller.cpp
default:
	mkdir obj
	g++ -o obj/channel.o -c src/channel.cpp
	g++ -o obj/message.o -c src/message.cpp
	g++ -o obj/process.o -c src/process.cpp
	g++ -o obj/snapshot.o -c src/snapshot.cpp
	g++ -o obj/processcontroller.o -c src/processcontroller.cpp
	g++ -o obj/main.o -c src/main.cpp
	g++ -o main obj/main.o obj/processcontroller.o obj/snapshot.o obj/process.o obj/message.o obj/channel.o

clean:
	rm -r obj
	rm main

rebuild:
	make clean && make