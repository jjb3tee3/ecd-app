#ifndef _ROBOT_ARM_H_
#define _ROBOT_ARM_H_

#include <stdio.h>
#include <stdarg.h> /* va functions */
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include <ctype.h>
#include <signal.h>
#include <errno.h>
#include <string.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <sys/types.h>

#ifndef _NO_LCD_
#include "lcd.h"
#endif

#include "keypad.h"

#define RA_DEBUG 	0
#define RA_INFO 	1
#define RA_ERROR	2
#define RA_LCD		3

#define MEM_PHYS_ADDR 0x72A00000

#endif