CXX         := -c++
CXXFLAGS    :=  -Wall
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
	$(CXX) $(CXXFLAGS) -o $@ -c $<

$(OUTPUT_DIR)/$(TARGET): $(OBJECTS)
	@mkdir -p $(@D)
	$(CXX) $(CXXFLAGS) -o $(OUTPUT_DIR)/$(TARGET) $(OBJECTS) $(LDLIBS)

.PHONY: all build clean debug release

build:
	@mkdir -p $(OUTPUT_DIR)
	@mkdir -p $(OBJ_DIR)

debug: CXXFLAGS += -DDEBUG -g
debug: all
