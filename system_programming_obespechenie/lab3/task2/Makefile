CC = x86_64-w64-mingw32-gcc
CFLAGS = -O2 -Wall
LIBS = -lcomdlg32

todo.exe: todo.c
	$(CC) $(CFLAGS) -o $@ $< $(LIBS)

clean:
	rm -f todo.exe

.PHONY: clean