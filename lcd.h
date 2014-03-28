#ifndef _LCD_H_
#define _LCD_H_

#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <stdlib.h>
#include <unistd.h>
#include <ctype.h>   /* toupper */
#include <signal.h>

#define GPIOBASE 	0x80840000
#define MODELBASE 	0x22000000
#define TS7300		0x03
#define PADR		0
#define PADDR		(0x10 / sizeof(unsigned int))
#define PAMASK		0x7F
#define PCDR		(0x08 / sizeof(unsigned int))
#define PCDDR		(0x18 / sizeof(unsigned int))
#define PCMASK  	0x01
#define PHDR    	(0x40 / sizeof(unsigned int))
#define PHDDR   	(0x44 / sizeof(unsigned int))

/* Delay values are calibrated for the EP9301 CPU
 * running at 166 MHz, but should work on 200MHz */
#define SETUP    	15
#define PULSE    	36
#define HOLD    	22

#define COUNTDOWN(x)    asm volatile ( \
  "1:\n"\
  "subs %1, %1, #1;\n"\
  "bne 1b;\n"\
  : "=r" ((x)) : "r" ((x)) \
);

volatile unsigned int *model_ptr;
unsigned int model;
volatile unsigned int *gpio;
volatile unsigned int *phdr;
volatile unsigned int *phddr;
volatile unsigned int *pcdr;
volatile unsigned int *pcddr;
volatile unsigned int *padr;
volatile unsigned int *paddr;

void command(unsigned int);
void writechars(unsigned char *);
unsigned int lcdwait(void);
void lcdinit(void);

#endif