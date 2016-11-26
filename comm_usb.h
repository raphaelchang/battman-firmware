#ifndef _COMM_USB_H_
#define _COMM_USB_H_

extern const USBConfig usbcfg;
extern SerialUSBConfig serusbcfg;
extern SerialUSBDriver SDU1;

void comm_usb_init(void);
void comm_usb_deinit(void);
void comm_usb_send(unsigned char *buffer, unsigned int len);
int comm_usb_is_active(void);

#endif  /* _COMM_USB_H_ */

/** @} */
