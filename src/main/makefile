hashtable_dir = ../hashtable/
holdall_dir = ../holdall/
options_dir = ../options/
reader_dir = ../reader/
shword_dir = ../shword/

CC = gcc
CFLAGS = -std=c18 \
  -Wall -Werror -Wfatal-errors -Wconversion -Wpedantic -Wextra -Wwrite-strings \
  -O2 \
  -I$(hashtable_dir) -I$(holdall_dir) -I$(options_dir) -I$(reader_dir) \
  -I$(shword_dir)
LDFLAGS =
vpath %.c $(hashtable_dir):$(holdall_dir):$(options_dir):$(reader_dir) \
  :$(shword_dir)
vpath %.h $(hashtable_dir):$(holdall_dir):$(options_dir):$(reader_dir) \
  :$(shword_dir)
objects = main.o hashtable.o holdall.o options.o reader.o shword.o
executable = ws

all: $(executable)

$(executable): $(objects)
	$(CC) $(LDFLAGS) -o $(executable) $(objects)

clean:
	$(RM) $(objects) $(executable)

hashtable.o: hashtable.c hashtable.h
holdall.o: holdall.c holdall.h
options.o: options.c options.h shword.h
reader.o: reader.c reader.h
shword.o: shword.c shword.h
main.o: main.c hashtable.h holdall.h options.h reader.h shword.h
