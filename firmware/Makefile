CXX         := avr-gcc
CXXFLAGS    :=  -Wall -Os -DF_CPU=16500000 -DDEBUG_LEVEL=0 -mmcu=attiny85
INCLUDE     := -Ivusb
BUILD       := ./build
OBJ_DIR     := $(BUILD)/obj
OUTPUT_DIR  := ./
TARGET      := usb_firmware
TARGET_HEX  := $(TARGET).hex
TARGET_BIN  := $(TARGET).bin
SRC         := src/main.c vusb/libs-device/osccal.c vusb/usbdrvasm.S vusb/usbdrv.c


hex: build $(TARGET_HEX)


$(TARGET_BIN):
	$(CXX) $(CXXFLAGS) $(INCLUDE) $(SRC) -o $@

$(TARGET_HEX): $(TARGET_BIN)
	avr-objcopy -j .text -j .data -O ihex $< $@

flash:
	micronucleus --run $(TARGET_HEX)
