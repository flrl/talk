LDFLAGS = -framework ApplicationServices
CFLAGS = -g -O0

all: talk

speak_osx.o: speak_osx.c speak.h

talk: main.o speak_osx.o buffer.o
	$(CC) $(LDFLAGS) -o $@ $^

clean:
	@rm -f talk *.o
