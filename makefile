matrix : matrix.o
	gcc matrix.o -lncursesw -ltinfo -o matrix

matrix.o : matrix.c
	gcc -g -c matrix.c -D_GNU_SOURCE -D_DEFAULT_SOURCE -I/usr/include/ncursesw

clean : 
	rm matrix matrix.o
