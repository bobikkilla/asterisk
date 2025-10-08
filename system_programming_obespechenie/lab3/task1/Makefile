CC = x86_64-w64-mingw32-gcc
CFLAGS = -O2 -Wall
LIBS = -lcomdlg32 -lcomctl32 -lole32

text_editor.exe: main.c
	$(CC) $(CFLAGS) -o $@ $< $(LIBS)

clean:
	rm -f text_editor.exe

.PHONY: clean