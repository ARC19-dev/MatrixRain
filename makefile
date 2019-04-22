LIBS = $(shell ncursesw5-config --libs)
FLAGS = $(shell ncursesw5-config --cflags)

matrix : matrix.o
	gcc matrix.o $(LIBS) -o matrix

matrix.o : matrix.c
	gcc -c matrix.c $(FLAGS)

clean : 
	rm matrix matrix.o
