TARGET = flower jpeg

all:$(TARGET)

flower:flower.cpp
	g++ -Wall -o $@ $< -lpthread

jpeg:jpeg.c
	gcc -Wall -c -o $@ $< -DIO_READ -DIO_MMAP -DIO_USERPTR

clean:
	rm -rf $(TARGET) *.o
