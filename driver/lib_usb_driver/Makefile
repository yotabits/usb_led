CXX         := -c++
CXXFLAGS    := -Wall -std=c++11 -pthread
INCLUDE     := -Iincludes
LDLIBS      := -lusb-1.0
BUILD       := ./build
OBJ_DIR     := $(BUILD)/obj
OUTPUT_DIR  := ./
TARGET      := lib-usb-based
SRC         := $(wildcard src/*.cpp)

OBJECTS := $(SRC:%.cpp=$(OBJ_DIR)/%.o)

all: build $(OUTPUT_DIR)/$(TARGET)

$(OBJ_DIR)/%.o: %.cpp
	@mkdir -p $(@D)
	$(CXX) $(CXXFLAGS) $(INCLUDE) -o $@ -c $<

$(OUTPUT_DIR)/$(TARGET): $(OBJECTS)
	@mkdir -p $(@D)
	$(CXX) $(CXXFLAGS) -o $(OUTPUT_DIR)/$(TARGET) $(OBJECTS) $(LDLIBS)

.PHONY: all build clean debug release

build:
	@mkdir -p $(OUTPUT_DIR)
	@mkdir -p $(OBJ_DIR)

install: all
	cp $(TARGET) /usr/bin/

clean:
	rm -rf $(BUILD)
	rm -rf $(OUTPUT_DIR)$(TARGET)

debug: CXXFLAGS += -DDEBUG -g
debug: all
