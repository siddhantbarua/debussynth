test: test.c term.c
	gcc -o test test.c term.c -lsoundio -lm -lpthread -I.
