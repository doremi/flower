TARGET = flower jpeg

all:$(TARGET)

flower:flower.cpp
	g++ -Wall -o $@ $< -lpthread

jpeg:jpeg.c
	gcc -Wall -o $@ $< -ljpeg -DIO_READ -DIO_MMAP -DIO_USERPTR

clean:
	rm -rf $(TARGET) *.o
