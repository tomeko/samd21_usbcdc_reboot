/*
 * console.h
 *
 * Created: 3/18/2021 6:17:54 PM
 *  Author: tomek0
 */ 


#ifndef CONSOLE_H_
#define CONSOLE_H_

#include <stdio.h>
#include <stdlib.h>

// alright this is super weird, but for some reason, only 2 commands wouldn't get output correctly when using
// a wrapper function with a va_list, hence the macro below (which works). This workaround works, but i'm still not sure why
#define printf_cdc(...) {sprintf(&buftx[0], __VA_ARGS__); printf_cdc_int(); }

//serial stuff
#define RX_BUFLEN       128
#define TX_BUFLEN       256

extern char buftx[];

void consoleRx(uint8_t * rx);
void newline();

void printf_cdc_int();

void console_out(void * str);

#endif /* CONSOLE_H_ */