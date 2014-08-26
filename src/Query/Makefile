.PHONY:clean  
CC=g++  
CFLAGS=-Wall -g  
LINKS=-lpthread -ltools
JBINCLUDE=-I../../include/jiebadict 
INCLUDE=-I../../include/tools
LIB=-L../../lib
BIN=../../bin/Query 
OBJS=Query.o Main_Query.o
$(BIN):$(OBJS)  
	$(CC) $(CFLAGS) $^ -o $@ $(LINKS) $(LIB) 
	rm -f *.o
%.o:%.cpp  
	$(CC) $(CFLAGS) -c $< -o $@ $(JBINCLUDE) $(INCLUDE)
clean:  
	rm -f *.o  
