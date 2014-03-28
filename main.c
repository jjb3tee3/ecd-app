#include "robotarm.h"

volatile unsigned int duty_Cycles[] = {0x3F,0x4F,0x5F,0x6F,0x7F,0x8F,0x9F};

volatile uintptr_t pwm_A;
volatile uintptr_t pwm_B;
volatile uintptr_t pwm_C;
volatile uintptr_t pwm_D;
volatile uintptr_t pwm_A_enable;
volatile uintptr_t pwm_B_enable;
volatile uintptr_t pwm_C_enable;
volatile uintptr_t pwm_D_enable;

volatile unsigned int base_Duty_Cycle_Index;
volatile unsigned int shoulder_Duty_Cycle_Index;
volatile unsigned int elbow_Duty_Cycle_Index;
volatile unsigned int gripper_Duty_Cycle_Index;

volatile unsigned int key;

void printd(unsigned int level, const char *format, ...) {
	va_list arg;

	switch(level) {
		case RA_DEBUG:
			printf("[*] ");
			break;
		case RA_INFO:
			printf("[-] ");
			break;
		case RA_ERROR:
			printf("[!] ");
			break;
		case RA_LCD:
			printf("[>] ");
			break;
		default:
			printf("[-] ");
			break;
	}

	va_start(arg, format);
	vprintf(format, arg);
	va_end(arg);

	printf("\n");
}

void sig_handler(int signo) {
		printd(RA_INFO, "Gracefully exiting application.");
		exit(0);
}

void setup_registers(uintptr_t mem_addr) {
	pwm_A = (uintptr_t)mem_addr + 0x06;
	pwm_B = (uintptr_t)mem_addr + 0x07;
	pwm_C = (uintptr_t)mem_addr + 0x08;
	pwm_D = (uintptr_t)mem_addr + 0x09;
	pwm_A_enable = (uintptr_t)mem_addr + 0x0B;
	pwm_B_enable = (uintptr_t)mem_addr + 0x0C;
	pwm_C_enable = (uintptr_t)mem_addr + 0x0D;
	pwm_D_enable = (uintptr_t)mem_addr + 0x0E;	
}

void setup_duty_cycle() {
	base_Duty_Cycle_Index = 4;
	shoulder_Duty_Cycle_Index = 4;
	elbow_Duty_Cycle_Index = 4;
	gripper_Duty_Cycle_Index = 4;
}

void configure_dio() {
    pwm_A_enable = 0x0;
    pwm_B_enable = 0x0;
    pwm_C_enable = 0x0;
    pwm_D_enable = 0x0;
}

#ifdef _NO_LCD_
/* These functions mimic the LCD screen so we can build
 * without a board. */
void writechars(char *msg) {
		printd(RA_LCD, "%s", msg);
}

void lcdinit() {
	// This is in LCD and only compiles on the board
	// so this is to stop it erroring when compiling
	printd(RA_DEBUG, "LCD initialised.");
}

void lcdwait() {
		printd(RA_DEBUG, "LCD waiting");
}

void command(uint8_t cmd) {
		printd(RA_DEBUG, "LCD got command: %02x", cmd);
}
#endif

int main(int argc, char **argv) {
	int fd;
	int page_size;
	uintptr_t mem_addr;
	uintptr_t mem_phys;

	volatile uintptr_t vga_reg;
	volatile uintptr_t enable_vga;
	
	mem_addr = (uintptr_t)NULL;
	mem_phys = (uintptr_t)MEM_PHYS_ADDR;

	signal(SIGINT, sig_handler);
	
	fd = open("/dev/mem", O_RDWR | O_SYNC);

	if(fd < 0) {
		printd(RA_ERROR, "Open /dev/mem failed");
	}

	mem_addr = (uintptr_t)mmap(
								0,
								page_size,
								PROT_READ | PROT_WRITE,
								MAP_SHARED,
								fd,
								mem_phys
							);
	
	if(mem_addr == (uintptr_t)MAP_FAILED) {
		printd(RA_ERROR, "mmap failed");
		close(fd);
		//return -1;
	}
	
	lcdinit();
	writechars("TS7300 EMU ROBOT ARM");
	lcdwait();
	command(0xa8);
	writechars("CONTROLLER ACTIVATED");
	
	setup_registers(mem_addr);
	
	vga_reg = (uintptr_t)mem_addr + 0x00;
	enable_vga = (uintptr_t)mem_addr + 0x01;
	
	setup_duty_cycle();
	
	configure_dio();
	
	key = 0;
	
	while(key != 0x01) {
		get_key(vga_reg, &key);
	}
	
	return 0;
}
