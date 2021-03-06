/*
 * Code generated from Atmel Start.
 *
 * This file will be overwritten when reconfiguring your Atmel Start project.
 * Please copy examples or other code you want to keep to a separate file or main.c
 * to avoid loosing it when reconfiguring.
 */
#include "atmel_start.h"
#include "usb_start.h"
#include "util_reset.h"

#if CONF_USBD_HS_SP
static uint8_t single_desc_bytes[] = {
    /* Device descriptors and Configuration descriptors list. */
    CDCD_ACM_HS_DESCES_LS_FS};
static uint8_t single_desc_bytes_hs[] = {
    /* Device descriptors and Configuration descriptors list. */
    CDCD_ACM_HS_DESCES_HS};
#define CDCD_ECHO_BUF_SIZ CONF_USB_CDCD_ACM_DATA_BULKIN_MAXPKSZ_HS
#else
static uint8_t single_desc_bytes[] = {
    /* Device descriptors and Configuration descriptors list. */
    CDCD_ACM_DESCES_LS_FS};
#define CDCD_ECHO_BUF_SIZ CONF_USB_CDCD_ACM_DATA_BULKIN_MAXPKSZ
#endif

static struct usbd_descriptors single_desc[]
    = {{single_desc_bytes, single_desc_bytes + sizeof(single_desc_bytes)}
#if CONF_USBD_HS_SP
       ,
       {single_desc_bytes_hs, single_desc_bytes_hs + sizeof(single_desc_bytes_hs)}
#endif
};

/** Buffers to receive and echo the communication bytes. */
static uint32_t usbd_cdc_buffer[CDCD_ECHO_BUF_SIZ / 4];

/** Ctrl endpoint buffer */
static uint8_t ctrl_buffer[64];

uint8_t usb_rx_buf[64] = {0};		// local rx buffer
static bool cond_reset_baud = false, cond_reset_dtr = false;

// checks whether reset baud & dtr condition is true, reset if so
static void check_reset()
{
	if (cond_reset_baud && cond_reset_dtr)
		forceReset();
}

static bool usb_device_cb_line_coding(struct usb_cdc_line_coding * lconf)
{
	if (lconf->dwDTERate == 1200)
		cond_reset_baud = true;		// if reset and baud at 1200, set flag possibly reset
	check_reset();
	return true;
}

uint8_t * get_usb_rx()
{
	cdcdf_acm_read((uint8_t *)usbd_cdc_buffer, sizeof(usbd_cdc_buffer));
	memset(usb_rx_buf, 0, sizeof(usb_rx_buf));
	memcpy(usb_rx_buf, usbd_cdc_buffer, usb_dat_avail);
	usb_dat_avail = 0;
	return &usb_rx_buf[0];
}

/**
 * \brief Callback invoked when bulk OUT data received
 */
static bool usb_device_cb_bulk_out(const uint8_t ep, const enum usb_xfer_code rc, const uint32_t count)
{
	usb_dat_avail = count;		// tps: flag/counter of rx chars

	/* No error. */
	return false;
}

/**
 * \brief Callback invoked when bulk IN data received
 */
static bool usb_device_cb_bulk_in(const uint8_t ep, const enum usb_xfer_code rc, const uint32_t count)
{
	/* Echo data. */
	cdcdf_acm_read((uint8_t *)usbd_cdc_buffer, sizeof(usbd_cdc_buffer));

	/* No error. */
	return false;
}

/**
 * \brief Callback invoked when Line State Change
 */
static bool usb_device_cb_state_c(usb_cdc_control_signal_t state)
{
	if (state.rs232.DTR) {
		/* Callbacks must be registered after endpoint allocation */
		cdcdf_acm_register_callback(CDCDF_ACM_CB_READ, (FUNC_PTR)usb_device_cb_bulk_out);
		cdcdf_acm_register_callback(CDCDF_ACM_CB_WRITE, (FUNC_PTR)usb_device_cb_bulk_in);
		cond_reset_dtr = true;
		/* Start Rx */
		cdcdf_acm_read((uint8_t *)usbd_cdc_buffer, sizeof(usbd_cdc_buffer));
	}

	/* No error. */
	return false;
}

/**
 * \brief CDC ACM Init
 */
void cdc_device_acm_init(void)
{
	/* usb stack init */
	usbdc_init(ctrl_buffer);

	/* usbdc_register_funcion inside */
	cdcdf_acm_init();

	usbdc_start(single_desc);
	usbdc_attach();
}

void usb_init(void)
{
	cdc_device_acm_init();
	usb_dat_avail = 0;
	while (!cdcdf_acm_is_enabled()); // wait cdc acm to be installed
	
	// register callbacks
	cdcdf_acm_register_callback(CDCDF_ACM_CB_STATE_C, (FUNC_PTR)usb_device_cb_state_c);
	cdcdf_acm_register_callback(CDCDF_ACM_CB_LINE_CODING_C, (FUNC_PTR)usb_device_cb_line_coding);
}
