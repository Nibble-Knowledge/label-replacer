#ifndef _LR4_H_
#define _LR4_H_

#define _GNU_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <stdint.h>
#include <errno.h>
#include <ctype.h>

typedef struct _replace {
	char *name;
	uint16_t addr;
} replace;

#endif /* _LR4_H_ */
