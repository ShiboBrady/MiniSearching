.PHONY:clean
CC=g++
CFLAGS=-Wall -g
BIN=libtools.a
INCLUDE=tools
OBJS=GetConfig.o Rio.o Tools.o
$(BIN):$(OBJS)
	ar -crv $@ -o $^
%.o:%.cpp
	$(CC) $(CFLAGS) -c $< -o $@ $(STARD)
clean:
	rm -rf *.o
