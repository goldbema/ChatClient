CC = gcc
objects = chatclient.o network.o validate.o

chatclient: $(objects)
	$(CC) -o chatclient $(objects)

chatclient.o: network.h validate.h
network.o: network.h validate.h
validate.o: validate.h

.PHONY: clean
clean:
	rm -f *.o chatclient
