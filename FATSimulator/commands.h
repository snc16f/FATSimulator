/* All of the function declarations for all the functions of the program */
#ifndef COMMANDS_H
#define COMMANDS_H

//we can delete which ones we don't need
#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <time.h>
#include <math.h>

//14 main commands
void exit();
void info();
bool size();
bool ls();
bool cd();
bool creat();
bool mkdir();
bool mv();
bool open();
bool close();
bool read();
bool write();
bool rm();
bool cp();


//2 extra credit commands
bool rmdir();
bool cp_r();

#endif
