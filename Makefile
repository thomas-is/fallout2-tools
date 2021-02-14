CC = gcc
#CC = /usr/bin/i686-w64-mingw32-gcc-win32
CFLAGS = -Wall -ansi -g -O3
LIBS = -lz

all: inventory special critical dat2dump save_info perks traits

dat2dump: src/dat2dump.o src/io.o
	$(CC) $(CFLAGS) $(LIBS) -o bin/dat2dump \
	src/dat2dump.o \
	src/io.o

save_info: src/save_info.o src/io.o src/offset.o
	$(CC) $(CFLAGS) $(LIBS) -o bin/save_info \
	src/save_info.o \
	src/io.o \
	src/offset.o

perks: src/perks.o src/io.o src/offset.o
	$(CC) $(CFLAGS) $(LIBS) -o bin/perks \
	src/perks.o \
	src/io.o \
	src/offset.o

traits: src/traits.o src/io.o src/offset.o
	$(CC) $(CFLAGS) $(LIBS) -o bin/traits \
	src/traits.o \
	src/io.o \
	src/offset.o

critical: src/critical.o src/io.o src/offset.o
	$(CC) $(CFLAGS) $(LIBS) -o bin/critical \
	src/critical.o \
	src/io.o \
	src/offset.o


special: src/special.o src/io.o src/offset.o
	$(CC) $(CFLAGS) $(LIBS) -o bin/special \
	src/special.o \
	src/io.o \
	src/offset.o

inventory: src/inventory.o src/io.o src/offset.o
	$(CC) $(CFLAGS) $(LIBS) -o bin/inventory \
	src/inventory.o \
	src/io.o \
	src/offset.o

clean:
	rm -f \
  src/*.o \
  bin/inventory \
  bin/special \
  bin/critical \
  bin/dat2dump \
  bin/save_info \
  bin/perks \
  bin/traits
