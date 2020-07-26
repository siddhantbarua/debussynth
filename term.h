#ifndef TERM_H
#define TERM_H

#include<termios.h>
#include<unistd.h>
#include<ctype.h>
#include<stdio.h>
#include<stdlib.h>


void disableRawMode();
void enableRawMode();

#endif
