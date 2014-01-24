TARGET = flower

all:$(TARGET)

flower:flower.cpp
	g++ -Wall -o $@ $< -lpthread

clean:
	rm -rf $(TARGET) *.o
