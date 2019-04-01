all: microshell


microshell: microshell.o parser.o
	gcc -g -Wall -o microshell microshell.o parser.o

microshell.o: microshell.c parser.h
	gcc -g -Wall -c -o microshell.o microshell.c


parser.o: parser.c parser.h
	gcc -g -Wall -c -o parser.o parser.c



clean:
	rm -f microshell
	rm -f prueba
	rm -f *.o
	rm -f *~
