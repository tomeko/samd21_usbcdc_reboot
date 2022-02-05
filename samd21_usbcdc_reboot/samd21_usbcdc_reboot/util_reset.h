/*
 * util_reset.h
 *
 * Created: 2/5/2022 3:25:38 PM
 *  Author: tomek0
 */ 


#ifndef UTIL_RESET_H_
#define UTIL_RESET_H_

// Code to programmatically initiate bootloader mode after reset
#define BOOT_DOUBLE_TAP_ADDRESS           (0x20007FFCul)
#define BOOT_DOUBLE_TAP_DATA              (*((volatile uint32_t *) BOOT_DOUBLE_TAP_ADDRESS))
#define DOUBLE_TAP_MAGIC 0x07738135

void console_out(void * str);
void initiateReset(int _ticks);
void forceReset(void);

#endif /* UTIL_RESET_H_ */