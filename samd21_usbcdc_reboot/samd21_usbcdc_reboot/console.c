/*
 * console.c
 *
 * Created: 3/18/2021 6:17:44 PM
 *  Author: tomek0
 */ 

#include <atmel_start.h>
#include <stdarg.h>
#include "console.h"
#include "util_reset.h"

char bufrx[RX_BUFLEN] = {0};
char buftx[TX_BUFLEN] = {0};

uint8_t bufrx_p = 0;

const char nl[] = "\r\n";
 
 void printf_cdc_int()
 {
	 // see note on printf_cdc macro
	 cdcdf_acm_write((uint8_t *)&buftx[0], strlen(buftx));
 }
 
 void newline()
 {
	 cdcdf_acm_write((uint8_t *)&nl[0], strlen(nl));
 }
 
 void resetRxBuf()
 {
	 memset(&bufrx[0], 0, TX_BUFLEN);
	 bufrx_p = 0;
 }

void consoleRx(uint8_t * rx)
{
	uint8_t len = strlen((char *)rx);

	memcpy(&bufrx[bufrx_p], rx, len);
	bufrx_p += len;
	
	char * pch = strpbrk(&bufrx[0], "\r\n");
	bool hascrlf = pch != NULL;
	
	if (hascrlf)
	{
		// newline received in bufrx, do whatever you want here
		resetRxBuf();
	} else if (bufrx_p >= TX_BUFLEN)
	{
		resetRxBuf();
	}
}
