CC=g++
CFLAGS=-Wall -Wextra -O3 -std=c++11 -ltfo
LFLAGS=`pkg-config opencv --libs`
TARGET=exec
SOURCE_REP=source/
BUILD_REP=build/
OBJ=main rectangle marker communication

all: $(addsuffix .o, $(addprefix $(BUILD_REP), $(OBJ)))
	$(CC) $^ -o $(BUILD_REP)$(TARGET) $(LFLAGS)

$(BUILD_REP)%.o: $(SOURCE_REP)%.cpp
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(BUILD_REP)*.o $(BUILD_REP)$(TARGET)

run:
	$(BUILD_REP)$(TARGET)

stop:
	killall -9 $(TARGET)

check:
	valgrind --leak-check=full $(BUILD_REP)$(TARGET)
