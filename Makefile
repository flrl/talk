LDFLAGS = -framework ApplicationServices

all: talk

talk: main.o
	$(CC) $(LDFLAGS) -o $@ $^

clean:
	@rm -f talk main.o
