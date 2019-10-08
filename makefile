CC = cc
CFLAGS = -O0 -g -pthread
TARGETS = main
.PHONY: all clean

all: clean $(TARGETS)

$(TARGETS):
	$(CC)  $(CFLAGS) $@.c -o $@
clean:
	rm -f $(TARGETS) *.ppm

# Old Implementation
# compile all .c files into .o files
# % matches all (like * in a command)
# $< is the source file (.c)

#%.o: %.c
#	$(CC) -c $(CFLAGS) $<

#main: main.o
#	$(CC) -o main main.o

#clean:
#	rm -f core *.o main
