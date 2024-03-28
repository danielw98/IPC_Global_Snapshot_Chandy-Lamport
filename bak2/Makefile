# channel.cpp  main.cpp  message.cpp  process.cpp  snapshot.cpp processcontroller.cpp
default:
	g++ -o channel.o -c channel.cpp
	g++ -o message.o -c message.cpp
	g++ -o process.o -c process.cpp
	g++ -o snapshot.o -c snapshot.cpp
	g++ -o processcontroller.o -c processcontroller.cpp
	g++ -o main.o -c main.cpp
	g++ -o main main.o processcontroller.o snapshot.o process.o message.o channel.o
clean:
	rm *.o
	rm main