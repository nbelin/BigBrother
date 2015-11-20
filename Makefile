CC=g++
CFLAGS=-Wall -O2 -std=c++11 -ltfo
LFLAGS=`pkg-config opencv --libs`
TARGET=exec
OBJ=main rectangle marker

all: $(addsuffix .o, $(OBJ))
	$(CC) $(addsuffix .o,$(OBJ)) -o $(TARGET) $(LFLAGS)

%.o: %.cpp
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f *~ *.o $(TARGET)

run:
	./$(TARGET)

stop:
	killall -9 $(TARGET)

check:
	valgrind --leak-check=full ./$(TARGET)
