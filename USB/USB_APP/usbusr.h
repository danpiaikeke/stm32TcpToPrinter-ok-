#ifndef _USBUSR
#define _USBUSR
#include "sys.h"

#include "usbh_core.h"
#include "usbh_stdreq.h"
#include "usb_bsp.h"
#include "usbh_ioreq.h"
#include "usbh_hcs.h"

USBH_Status My_Machine(USB_OTG_CORE_HANDLE *pdev , void *phost);
USBH_Status My_Config(USB_OTG_CORE_HANDLE *pdev,void *phost);

extern USBH_Class_cb_TypeDef Class_cb;
extern short Receive_Retry ;
extern u16 usbbufferlen;
extern u8 SendMechine ;
extern int SendCount ;
extern u8 ReceiveMechine ;
extern int ReceiveCount ;
extern u32 dataReceived ;
#endif
