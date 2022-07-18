all:
	g++ -g -c classChannel.cpp
	g++ -g -c server.cpp
	g++ -g -o server.exe classChannel.o server.o -lpthread -std=c++11
	g++ -g client.cpp -o client.exe -pthread -std=c++11
	rm *.o

s:
	./server.exe

c:
	./client.exe

clean:
	rm *.exe *.o