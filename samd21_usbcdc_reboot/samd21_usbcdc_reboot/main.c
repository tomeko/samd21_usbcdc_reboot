#include <atmel_start.h>

#include "console.h"

int main(void)
{
	/* Initializes MCU, drivers and middleware */
	atmel_start_init();

	while (1)
	{
		if (usb_dat_avail)
			consoleRx(get_usb_rx());
	}
}
