CC = gcc
CFLAGS = `pkg-config --cflags gtk+-3.0 sqlite3` -rdynamic -Wall -Wextra -O2
LIBS = `pkg-config --libs gtk+-3.0 sqlite3`

SOURCES = main.c db.c
TARGET = sqlite-gtk-wrapper

all: $(TARGET)

$(TARGET): $(SOURCES)
	@if [ ! -f zoo.db ]; then sqlite3 zoo.db < zoo_create.sql; fi
	$(CC) $(CFLAGS) -o $@ $^ $(LIBS)

clean:
	rm -f $(TARGET) zoo.db

.PHONY: all clean