#include "usbusr.h"
#include "usb_hcd_int.h"
#include "usbh_usr.h"
#include "usbh_core.h"
#include "usbh_msc_core.h"
#include "string.h"
#include "delay.h"

#include "tcp_server_demo.h"
u8 recbuff[TCP_SERVER_RX_BUFSIZE];
extern u8 send_number;
extern u8 glength;
int glseek=0;

int line=1600; 
short Receive_Retry = 0;
u8 sendTime=1;
u8 toggle_out=0;

u16 sendlen=32;
u8 SendMechine = 2;
int SendCount = 0;
u8 ReceiveMechine = 0;
int ReceiveCount = 0;
u32 dataReceived = 0;
u8 over_flag=1;
/*
u8 printdata[10]="printdata";
u8 printstartend[]= {0x1b,'%','-','1','2','3','4','5','X',0};
const u8 *usb_send1[5]= {
    "@PJL INITIALIZE"
    "@PJL SET PAGEPROTECT=ON",
    "@PJL SET RESOLUTION=600",
    "@PJL ENTER LANGUAGE=PCL",
    "@PJL SET PAGEPROTECT=OFF",
};
u8 printreset[]= {0x1b,'E',0};
*/
static const u8 *fxpath1="0:/config.txt";
static const u8 *fxpath="0:/next_test.txt";
FIL			fftemp;
FIL 		fftemp1;

char buf[200];
int i=0;
int j=0;
int flag;

u8 usb_send[64];
u8 usb_rec[64];
int gsend_flag=1;
u16 rec_len=0;
int toggle_out_flag=1;

int buf_num=0;
//my_debug
//12.18
u8 first_into=0;
u8 first_config=1;
u8 first_machine=1;
//mydebug_12.21
u8 config_finish=0;
u8 send_finish=0;

//mydebug_12.22
u8 gsend_status=1;
typedef struct
{
  uint8_t              hc_num_in; 
  uint8_t              hc_num_out; 
  uint8_t              MSBulkOutEp;
  uint8_t              MSBulkInEp;
  uint16_t             MSBulkInEpSize;
  uint16_t             MSBulkOutEpSize;
}myClassTypedef;
myClassTypedef myClass;

USBH_Status Init(USB_OTG_CORE_HANDLE *pdev , void *phost)
{
		u8 status;
	USBH_HOST *pphost = phost;
	f_open(&fftemp,(const TCHAR*)fxpath,FA_READ); 

	printf("%x\r\n",pphost->device_prop.Ep_Desc[0][0].bEndpointAddress);    
  printf("%x\r\n",pphost->device_prop.Ep_Desc[0][1].bEndpointAddress);        
					
	myClass.MSBulkOutEp =  pphost->device_prop.Ep_Desc[0][0].bEndpointAddress;
	myClass.MSBulkInEp =  pphost->device_prop.Ep_Desc[0][1].bEndpointAddress;
	myClass.MSBulkOutEpSize =  pphost->device_prop.Ep_Desc[0][0].wMaxPacketSize;
	myClass.MSBulkInEpSize = pphost->device_prop.Ep_Desc[0][1].wMaxPacketSize;
	
	myClass.hc_num_out = USBH_Alloc_Channel(pdev, myClass.MSBulkOutEp);
	myClass.hc_num_in = USBH_Alloc_Channel(pdev, myClass.MSBulkInEp); 
	printf("myClass.hc_num_out :%d\r\n",myClass.hc_num_out);
	printf("myClass.hc_num_in :%d\r\n",myClass.hc_num_in);
  
	status = USBH_Open_Channel  (pdev,
											myClass.hc_num_out,
											pphost->device_prop.address,
											pphost->device_prop.speed,
											EP_TYPE_BULK,
											myClass.MSBulkOutEpSize);  
	if(status == HC_OK)
	status = USBH_Open_Channel  (pdev,
											myClass.hc_num_in,
											pphost->device_prop.address,
											pphost->device_prop.speed,
											EP_TYPE_BULK,
											myClass.MSBulkInEpSize);
	if(status == HC_OK)
	//LHLFont_DebugString("USBH Open Channel Ok");
	
	
	Receive_Retry = 2;
	
	return USBH_OK;                                 
}
            
void DeInit(USB_OTG_CORE_HANDLE *pdev , void *phost)
{
	#if 1
  if ( myClass.hc_num_out)
  {
    USB_OTG_HC_Halt(pdev, myClass.hc_num_out);
    USBH_Free_Channel  (pdev, myClass.hc_num_out);
    myClass.hc_num_out = 0;     /* Reset the Channel as Free */
  }
#endif
  if ( myClass.hc_num_in)
  {
    USB_OTG_HC_Halt(pdev, myClass.hc_num_in);
    USBH_Free_Channel  (pdev, myClass.hc_num_in);
    myClass.hc_num_in = 0;     /* Reset the Channel as Free */
  } 
	f_close(&fftemp);
}
USBH_Status Requests(USB_OTG_CORE_HANDLE *pdev , void *phost)
{
	return USBH_OK;
}
USBH_Status My_Machine(USB_OTG_CORE_HANDLE *pdev , void *phost)
{
	
	USBH_HOST *pphost = phost;
	u8 send_length=0;
	if(SendMechine==1)
	{
			SendMechine=0;
			
			if(toggle_out_flag)
				pdev->host.hc[myClass.hc_num_out].toggle_out = 1;
			else
				pdev->host.hc[myClass.hc_num_out].toggle_out=0;
				
			if(gsend_status)			//ÅÐ¶ÏsendÊÇ·ñack
			{
				if(send_number==1)
				{
					memcpy(usb_send,recbuff+glseek*64,glength);
					send_length=glength;
					glseek=0;     //Æ«ÒÆÇå0           
				}
				else
				{
					memcpy(usb_send,recbuff+glseek*64,64);
					glseek++;
					glength-=64;
					send_length=64;
				}
			}
			//printf("%d",pphost->device_prop.Ep_Desc[0][0].bEndpointAddress);
			USBH_BulkSendData (pdev,
														 (u8*)usb_send, 
														 send_length, 
														 MSC_Machine.hc_num_out);
			printf("***send_number=%d send_length=%d\r\n",send_number,send_length);
			delay_ms(20);
			toggle_out_flag^=1;
			SendCount=0;
			while((HCD_GetURB_State(pdev, MSC_Machine.hc_num_out) != URB_DONE)&&SendCount<100)
			{
				SendCount++;
				//	printf("HCD_GetURB_State=%d\r\n",pdev->host.URB_State[myClass.hc_num_out]);
			}
			//mydebug_12.22   for check TX/RX status
			if(pdev->host.HC_Status[2] == HC_NAK)
				gsend_status=0;
			else
			{
				gsend_status=1;
				--send_number;
			}	
			
			Receive_Retry = 2;
		}
	
	if(Receive_Retry >0)
	{
		
		Receive_Retry--;
		ReceiveCount=0;
		
		USBH_BulkReceiveData (pdev,
								 (u8 *)usb_rec, //usbbuffer+3
								 64, 
								 MSC_Machine.hc_num_in);
		/*
		USBH_CtlReceiveData(pdev,
									(u8 *)usb_rec,
									100,
									pphost->Control.hc_num_in
		);*/
		while(HCD_GetURB_State(pdev, MSC_Machine.hc_num_in) != URB_DONE &&ReceiveCount<100)
		{
			ReceiveCount++;
		}
		
//		if( HCD_GetURB_State(pdev, myClass.hc_num_in) == URB_DONE)	
		rec_len=HCD_GetXferCnt(pdev, MSC_Machine.hc_num_in);
		if(rec_len>0)
		{
			
			printf("***usb receive\r\n");
			printf("USBH_BulkReceiveData usbbufferlen:%d\r\n",rec_len);
			printf("USBH_BulkReceiveData=%s\r\n",usb_rec);
			dataReceived = 1;
		}
			
	}

	return USBH_OK;
}
					
USBH_Class_cb_TypeDef Class_cb=
{
	Init,
	DeInit,
	Requests,
	My_Machine
};


USBH_Status My_Config(USB_OTG_CORE_HANDLE *pdev , void *phost)
{
	
	USBH_HOST *pphost = phost;
	u8  UrbStatus=0;
	
	if(first_config)
	{
		f_open(&fftemp1,(const TCHAR*)fxpath1,FA_READ);
		first_config=0;
	}

	if(SendMechine==1)
	{
		SendMechine=0;
		f_gets(buf,line,&fftemp1);
		if(strlen(buf)!=0)
		{
			//if(buf[0]=='4'||buf[0]=='C')
			//{
			//printf("%s\n",buf);
			printf("bufnum=%d\r\n",buf_num++);
			i=0;
			j=0;
			flag=1;
			while(flag)
			{
				char h1,h2;
				u8   s1,s2;
				//	if(buf[i*3+2]!=' '||buf[i*3+2]=='\0')
				if(buf[i*3+2]!=' ')
				{	
					flag=0;
				}
					
				h1=buf[i*3];
				h2=buf[i*3+1];
				if(h1>='0'&&h1<='9')
					s1=h1-0x30;
				else if(h1>='A'&&h1<='F')
					s1=h1-0x37;
					
				if(h2>='0'&&h2<='9')
					s2=h2-0x30;
				else if(h2>='A'&&h2<='F')
					s2=h2-0x37;
					
				usb_send[j]=s1*16+s2;
				i++;
				j++;
			}
				//printf("number=%d\n",j);
				//printf("usbsend[0]=0x%x,0x%x\n",usb_send[0],usb_send[j-1]);
				//printf("***length=%d\n",j);
			if(toggle_out_flag)
				pdev->host.hc[myClass.hc_num_out].toggle_out = 1;
			else
				pdev->host.hc[myClass.hc_num_out].toggle_out=0;
				
				//printf("%d",pphost->device_prop.Ep_Desc[0][0].bEndpointAddress);
			USBH_BulkSendData (pdev,
														 (u8*)usb_send, 
														 j, 
														 MSC_Machine.hc_num_out);
				
				//delay_ms(20);
				//memset(usb_send,0x0,j);
			toggle_out_flag^=1;
				
			SendCount=0;
			while((HCD_GetURB_State(pdev, MSC_Machine.hc_num_out) != URB_DONE)&&SendCount<100)
			{
				SendCount++;
				//	printf("HCD_GetURB_State=%d\r\n",pdev->host.URB_State[myClass.hc_num_out]);
			}
		}	
		else
		{
			f_close(&fftemp1);
			config_finish=1;
		}
		
		//Receive_Retry = 2;
	
	}
	if(Receive_Retry >0)
	{
		
		Receive_Retry--;
		ReceiveCount=0;
		
		USBH_BulkReceiveData (pdev,
								 (u8 *)usb_rec, //usbbuffer+3
								 64, 
								 MSC_Machine.hc_num_in);
		
		while(HCD_GetURB_State(pdev, MSC_Machine.hc_num_in) != URB_DONE &&ReceiveCount<100)
		{
			ReceiveCount++;
		}
		
//		if( HCD_GetURB_State(pdev, myClass.hc_num_in) == URB_DONE)	
		rec_len=HCD_GetXferCnt(pdev, MSC_Machine.hc_num_in);
		if(rec_len>0)
		{
			
			//printf("***usb receive\r\n");
			//printf("USBH_BulkReceiveData usbbufferlen:%d\r\n",rec_len);
			//printf("USBH_BulkReceiveData=%s\r\n",usb_rec);
			dataReceived = 1;
		}
			
	}

	return USBH_OK;
}
/*					
USBH_Class_cb_TypeDef Class_cb=
{
	Init,
	DeInit,
	Requests,
	My_Machine
};
*/

