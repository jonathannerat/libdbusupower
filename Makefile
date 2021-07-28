PREFIX = /usr/local
INCS = `pkg-config --cflags dbus-1`
LIBS = -ldbus-1 -ldbusupower
CFLAGS = -fPIC $(INCS)
LDFLAGS = -L. $(LIBS)

all: options libdbusupower.so

options:
	@echo build options:
	@echo "CFLAGS  = $(CFLAGS)"
	@echo "LDFLAGS = $(LDFLAGS)"
	@echo "CC      = $(CC)"

libdbusupower.so: libdbusupower.h
	$(CC) $(CFLAGS) -shared -o $@ libdbusupower.c

main: main.o libdbusupower.so
	$(CC) $(LDFLAGS) -o $@ main.o

main.o: libdbusupower.h

.c.o:
	$(CC) $(CFLAGS) -c $<

debug: CFLAGS += -g -DDEBUG
debug: main

lsp: clean
	@bear -- make

clean:
	-rm *.o *.so main compile_commands.json

install:
	@echo "PREFIX  = $(PREFIX)"
	cp libdbusupower.h $(PREFIX)/include
	cp libdbusupower.so $(PREFIX)/lib

uninstall:
	@echo "PREFIX  = $(PREFIX)"
	rm $(PREFIX)/include/libdbusupower.h 
	rm $(PREFIX)/lib/libdbusupower.so 

.PHONY = all options lib clean install
