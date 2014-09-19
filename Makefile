CC 		= g++
CFLAGS 		= -Wall -g -Iinclude
LIBRARIES 	= -lGL -lGLU -lglut -lpng -lm -ljpeg
OBJECTS 	= $(patsubst %.cpp, %.o, $(wildcard src/*.cpp))
BINARY    	= diashow

all: $(OBJECTS) Makefile
	$(CC) -o $(BINARY) $(LIBRARIES) $(OBJECTS)

%.o: %.cpp Makefile
	$(CC) -c -o $@ $(CFLAGS) $<

run:	all
	./$(BINARY)

debug: all
	gdb $(BINARY)

bt: all
	echo "run\nbt" | gdb $(BINARY)

clean:
	rm -f ./$(BINARY)
	rm -f src/*.o
	rm -f src/*~
	rm -f *~
