CC       = gcc
CXX      = g++
CFLAGS   = -g
CXXFLAGS = $(CFLAGS)

LIBPATH = ./
LIBS = -Wl,-rpath=./:./ -lusb-1.0
SRC = ./main.c common.c conf.c debug.c CP2130_API.c nrf24l01.c
TARGET = ./spi_server

all: 
	$(CXX) $(OPTI) $(CXXFLAGS) $(SRC) -o $(TARGET) -L$(LIBPATH) $(LIBS)

.PHONY: clean
clean:
	rm -f $(TARGET)