#ifndef ES_LIB_H
#define ES_LIB_H

#include "TM4C129.h" 
#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>


extern void msDelay(int msec) ;

// serial functions
extern void ES_Serial(int number, char line_format[20]) ;  // eg "9600,5,N,1" is 9600 baud, 5 bit word, no parity, 1 stop bit
extern char ES_getchar(int number) ;
extern void ES_putchar(int number, char c) ;
extern void ES_printf(const char *format, ...) ;

#endif /* ES_LIB_H */