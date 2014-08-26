.PHONY:clean  
CC=g++  
CFLAGS=-Wall -g  
LINKS=-lpthread -ltools
JBINCLUDE=-I../../include/jiebadict
INCLUDE=-I../../include/tools
LIB=-L../../lib
BIN=../../bin/DeleteRepeat
OBJS=DeleteRepeat.o Main_DeleteRepeat.o
$(BIN):$(OBJS)  
	$(CC) $(CFLAGS) $^ -o $@ $(LINKS) $(LIB)
	rm -f *.o
%.o:%.cpp  
	$(CC) $(CFLAGS) -c $< -o $@ $(JBINCLUDE) $(INCLUDE)
clean:  
	rm -f *.o  
