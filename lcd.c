#include "lcd.h"
#ifdef _NO_LCD_
/* These functions mimic the LCD screen so we can build
 * without a board. */
void writechars(char *msg) {
	printd(RA_LCD, "%s", msg);
}

void lcdinit() {
	printd(RA_DEBUG, "LCD initialised.");
}

void lcdwait() {
	printd(RA_DEBUG, "LCD waiting");
}

void command(uint8_t cmd) {
	printd(RA_DEBUG, "LCD got command: %02x", cmd);
}

#else

void lcdinit(void) {
	int fd = open("/dev/mem", O_RDWR|O_SYNC);

	gpio = (unsigned int *)mmap(0, getpagesize(),
	PROT_READ|PROT_WRITE, MAP_SHARED, fd, GPIOBASE);
	model_ptr = (unsigned int *)mmap(0, getpagesize(),
	PROT_READ|PROT_WRITE, MAP_SHARED, fd, MODELBASE);

	model = *model_ptr & 0x03;

	phdr = &gpio[PHDR];
	padr = &gpio[PADR];
	paddr = &gpio[PADDR];

	if(model == TS7300) {
		pcdr = &gpio[PCDR];
		pcddr = &gpio[PCDDR];
	}

	phddr = &gpio[PHDDR];

	if(model == TS7300) {
		*paddr & ~PAMASK;  // port A to inputs
		*pcddr & ~PCMASK;  // port C to inputs
	} else {
		*paddr = 0x0;  // All of port A to inputs
	}

	*phddr |= 0x38; // bits 3:5 of port H to outputs
	*phdr &= ~0x18; // de-assert EN, de-assert RS
	
	usleep(15000);
	command(0x38); // two rows, 5x7, 8 bit
	usleep(4100);
	command(0x38); // two rows, 5x7, 8 bit
	usleep(100);
	command(0x38); // two rows, 5x7, 8 bit
	command(0x6); // cursor increment mode
	lcdwait();
	command(0x1); // clear display
	lcdwait();
	command(0xc); // display on, blink off, cursor off
	lcdwait();
	command(0x2); // return home
}

unsigned int lcdwait(void) {
	int i, dat, tries = 0;
	unsigned int ctrl = *phdr;

	if(model == TS7300) {
		*paddr = 0x00;//*paddr & ~PAMASK;  // port A to inputs
		*pcddr = 0x00;//*pcddr & ~PCMASK;  // port C to inputs
	} else {
		*paddr = 0x0;  // All of port A to inputs
	}

	ctrl = *phdr;

	do {
		// step 1, apply RS & WR
		ctrl |= 0x30; // de-assert WR
		ctrl &= ~0x10; // de-assert RS
		*phdr = ctrl;

		// step 2, wait
		i = SETUP;
		COUNTDOWN(i);

		// step 3, assert EN
		ctrl |= 0x8;
		*phdr = ctrl;

		// step 4, wait
		i = PULSE;
		COUNTDOWN(i);

		// step 5, de-assert EN, read result

		if(model == TS7300) {
			dat = (*padr & PAMASK) | ((*pcdr & PCMASK) << 7);
		} else {
			dat = *padr;
		}


		ctrl &= ~0x8; // de-assert EN
		*phdr = ctrl;

		// step 6, wait
		i = HOLD;
		COUNTDOWN(i);
	} while (dat & 0x80 && tries++ < 1000);
	
	return dat;
}

void command(unsigned int cmd) {
	int i;
	unsigned int ctrl = *phdr;

	if(model == TS7300) {
		*paddr = *paddr | PAMASK; //set port A to outputs
		*pcddr = *pcddr | PCMASK; //set port C to outputs
	//printf("SET, *paddr = 0x%x, *pcddr = 0x%x\n", *paddr, *pcddr);
	} else {
		*paddr = 0xff; // set port A to outputs
	}

	// step 1, apply RS & WR, send data
	if(model == TS7300) {
	//printf("*padr = 0x%x, *pcdr = 0x%x\n", *padr, *pcdr);
		*padr = *padr & ~PAMASK;
		*pcdr = *pcdr & ~PCMASK;
		*padr = (*padr & ~PAMASK) | (cmd & PAMASK);
	//if bit 7 of cmd is set the set bit 0 of pcdr
	//if bit 7 of cmd is clear then clear bit 0 of pcdr
		*pcdr = *pcdr | (cmd >> 7);


	//printf("cmd = 0x%x, *padr = 0x%x, *pcdr = 0x%x\n", cmd, *padr, *pcdr);
	} else {
		*padr = cmd;
	}

	ctrl &= ~0x30; // de-assert RS, assert WR
	*phdr = ctrl;

	// step 2, wait
	i = SETUP;
	COUNTDOWN(i);

	// step 3, assert EN
	ctrl |= 0x8;
	*phdr = ctrl;

	// step 4, wait
	i = PULSE;
	COUNTDOWN(i);

	// step 5, de-assert EN
	ctrl &= ~0x8; // de-assert EN
	*phdr = ctrl;

	// step 6, wait
	i = HOLD;
	COUNTDOWN(i);
}

void writechars(unsigned char *dat) {
	int i;
	unsigned int ctrl = *phdr;

	do {
		lcdwait();

	if(model == TS7300) {
		*paddr = *paddr | PAMASK; //set port A to outputs
		*pcddr = *pcddr | PCMASK; //set port C to outputs
	//printf("SET, *paddr = 0x%x, *pcddr = 0x%x\n", *paddr, *pcddr);
	} else {
		*paddr = 0xff; // set port A to outputs
	}

	// step 1, apply RS & WR, send data
	if(model == TS7300) {

		*padr = *padr & ~PAMASK;
		*pcdr = *pcdr & ~PCMASK;
		*padr = *padr | (*dat & PAMASK);
		*pcdr = *pcdr | ((*dat >> 7) & PCMASK);
	//printf("dat = 0x%x, *padr = 0x%x, *pcdr = 0x%x\n", *dat, *padr, *pcdr);
		*dat++;
	} else {
		*padr = *dat++;
	}

		ctrl |= 0x10; // assert RS
		ctrl &= ~0x20; // assert WR
		*phdr = ctrl;

		// step 2
		i = SETUP;
		COUNTDOWN(i);

		// step 3, assert EN
		ctrl |= 0x8;
		*phdr = ctrl;

		// step 4, wait 800 nS
		i = PULSE;
		COUNTDOWN(i);

		// step 5, de-assert EN
		ctrl &= ~0x8; // de-assert EN
		*phdr = ctrl;

		// step 6, wait
		i = HOLD;
		COUNTDOWN(i);
	} while(*dat);
}
#endif