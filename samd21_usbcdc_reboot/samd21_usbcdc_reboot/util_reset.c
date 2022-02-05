/*
 * util_reset.c
 *
 * Created: 2/5/2022 3:25:48 PM
 *  Author: tomek0
 */ 
#include <atmel_start.h>
#include <string.h>
#include "util_reset.h"

#define APP_START 0x00002004
#define NVM_MEMORY ((volatile uint16_t *)0x000000)
static int ticks = -1;



static inline bool nvmReady(void)
{
	return NVMCTRL->INTFLAG.reg & NVMCTRL_INTFLAG_READY;
}

int scaleRange(int val, int oldmin, int oldmax, int newmin, int newmax)
{
	return (int)((((val - oldmin) * (newmax - newmin)) / (float)((oldmax - oldmin))) + newmin);
}

__attribute__ ((long_call, section (".ramfunc")))
static void banzai() {
	// Disable all interrupts
	__disable_irq();

	// Avoid erasing the application if APP_START is < than the minimum bootloader size
	// This could happen if without_bootloader linker script was chosen
	// Minimum bootloader size in SAMD21 family is 512bytes (RM section 22.6.5)
	if (APP_START < (0x200 + 4)) {
		goto reset;
	}

	reset:
	// Reset the device
	NVIC_SystemReset() ;

	while (true);
}

void forceReset(void)
{
	BOOT_DOUBLE_TAP_DATA = DOUBLE_TAP_MAGIC;    // Start in bootloader mode after reset
	banzai();
}

void initiateReset(int _ticks) {
	ticks = _ticks;
}

void cancelReset()
{
	ticks = -1;
}

void SysTick_DefaultHandler(void)
{
	if (ticks == -1)
		return;
	ticks--;
	if (ticks == 0)
		banzai();
}

