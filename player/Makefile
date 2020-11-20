CC = g++
CFLAGS = -std=c++11
TARGET = player
OBJECTS = main.o
LIBS = 'pkg-config opencv4 --libs --cflags' -lOpenCL

all : $(TARGET)
	$(CC) $(CFLAGS) -o $(TARGET) main.cpp -I. $(LIBS)

clean:
	rm -f $(TARGET)