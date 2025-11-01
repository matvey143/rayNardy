LDFLAGS = '-Wl,-rpath,$$ORIGIN' -L./ -I./ -lraylib -lGL -lm -lpthread -ldl -lrt -lX11
CFLAGS =

all: nardy

nardy: raylib.h libraylib.so nardy.o
	$(CC) -o nardy nardy.o $(LDFLAGS)

nardy.o: raylib.h libraylib.so main.c
	$(CC) -c -o nardy.o $(CFLAGS) main.c
