CEREAL_ROOT ?= $(PWD)/../../../cereal

CC=g++
CFLAGS=-Wall -Wextra -g -O3 -std=c++11 -I$(CEREAL_ROOT)/include
LFLAGS=`pkg-config opencv --libs`

BUILD_DIR ?= build
EXE_NAME = detector
EXE_PATH = $(BUILD_DIR)/$(EXE_NAME)

SOURCE_DIR = source
OBJS=main rectangle marker communication gui video_controller detector_controller color_chooser_gui

all: $(BUILD_DIR) compile

compile: $(addsuffix .o, $(addprefix $(BUILD_DIR)/, $(OBJS)))
	$(CC) $^ -o $(EXE_PATH) $(LFLAGS)

$(BUILD_DIR):
	mkdir $(BUILD_DIR)

$(BUILD_DIR)/%.o: $(SOURCE_DIR)/%.cpp
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -rf $(BUILD_DIR)/*

run:
	./$(EXE_PATH)

stop:
	killall -9 $(EXE_NAME)

check:
	valgrind --leak-check=full $(BUILD_REP)$(TARGET)
