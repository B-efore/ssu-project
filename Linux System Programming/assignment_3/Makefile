OBJECTS = ssu_sfinder.o
TARGET = ssu_sfinder
CC = gcc

$(TARGET) : $(OBJECTS)
	$(CC) -o $(TARGET) $(OBJECTS) -lcrypto -pthread

ssu_sfinder.o : ssu_sfinder.c
	$(CC) -c ssu_sfinder.c
