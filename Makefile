CC = gcc

CFLAGS = -Wall

TARGET = jpg_polyglot

all: $(TARGET)

$(TARGET): jpg_polyglot.c
	$(CC) $(CFLAGS) -o $(TARGET) $(TARGET).c

clean: 
	rm $(TARGET)

uninstall: 
	rm /opt/$(TARGET)

install: $(TARGET)
	install -o root -g root -m 555 $(TARGET) /opt/$(TARGET)

