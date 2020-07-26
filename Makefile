#test: test.c term.c
main: main.c term.c audio.c
	gcc -o main main.c audio.c term.c -lsoundio -lm -lpthread -I.
	#gcc -o test test.c term.c -lsoundio -lm -lpthread -I.
