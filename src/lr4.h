#ifndef _LR4_H_
#define _LR4_H_

#define _GNU_SOURCE
/* This is the label replacer for the Nibble Knowledge computer. */
/* Hacked together by Ryan Harvey Oct 24 2015 - for a minicompter design that should be from the 60s! */
/* This simply replaces certain labels with calculated offsets based on a simple definition file. */
/* This essentially functions as a cheap and dirty linker, and trims whitespace and comments */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <stdint.h>
#include <errno.h>
#include <ctype.h>

/* Macros to determine if we are using strange, macro-generated hex values (no 0x) or standard. */
#define STDHEX 0
#define NSTDHEX 1

/* The singular global variable - what line of the assembly file we are on. Helps with error messages */
extern unsigned long long FILELINE;

/* This is the data structure used to define label replacements */
typedef struct _replace {
	char *name;
	uint16_t addr;
} replace;
/* This is the data structure used to define .data sizes. */
typedef struct datasize {
	uint16_t size;
	uint16_t num;
} datasize;

/* estrtol (extended strtol) and estrtoul (extended strtoul) are functions to intelligently determine the base of the number to be converted by the strtol functions and return them. */
/* They function the same as strtol except for the final parameter they accept a type parameter which switches them from expecting hex values in the form 0xF (STDHEX or 0) to just F (NSTDHEX or 1). This is to accomodate the macro assembler. */
/* Switching to NSTDHEX means the default assumption for numbers is hex instead of decimal, so 12 will be assume to be 0x12, or 1. Decimal values will be accepted in this mode in the form 0d9. */
long estrtol(char *str, char **endptr, uint8_t type);

unsigned long estrtoul(char *str, char **endptr, uint8_t type);
/* Trim whitespace */
char *trim(char *str);
/* Find replacable labels. */
char *findreplace(char *str, replace *replaces, unsigned long numreplaces, uint16_t dotdatasize, unsigned char dotdata);
/* Add a .data's size to the size list. */
static void addsize(uint16_t size, uint16_t *currsize, datasize **datasizes, unsigned long *numdatasizes);

#endif /* _LR4_H_ */
