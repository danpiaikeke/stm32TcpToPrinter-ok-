#include "sys.h"
#include "delay.h"  
#include "usart.h"   
#include "led.h"
#include "lcd.h"
#include "key.h"  
#include "usmart.h"  
#include "sram.h"   
#include "malloc.h" 
#include "w25qxx.h"    
#include "sdio_sdcard.h"
#include "ff.h"  
#include "exfuns.h"    
#include "fontupd.h"
#include "text.h"	
#include "piclib.h"	 
#include "usbh_usr.h" 

#include "usbusr.h"    //my_debug

//mydebug_12.27
#include "lwip_comm.h"
#include "lan8720.h"
#include "timer.h"
#include "lwip/netif.h"
#include "lwip_comm.h"
#include "lwipopts.h"
#include "tcp_server_demo.h"

//ALIENTEK ̽����STM32F407������ ʵ��53
//USB U�� ʵ��-�⺯���汾 
//����֧�֣�www.openedv.com
//�Ա����̣�http://eboard.taobao.com
//������������ӿƼ����޹�˾    
//���ߣ�����ԭ�� @ALIENTEK 

USBH_HOST  USB_Host;
USB_OTG_CORE_HANDLE  USB_OTG_Core;
extern u8 config_finish;
extern u8 send_finish;

//�û�����������
//����ֵ:0,����
//       1,������
u8 USH_User_App(void)
{ 
	u32 total,free;
	u8 res=0;
	Show_Str(30,140,200,16,"�豸���ӳɹ�!.",16,0);	 
	res=exf_getfree("2:",&total,&free);
	if(res==0)
	{
		POINT_COLOR=BLUE;//��������Ϊ��ɫ	   
		LCD_ShowString(30,160,200,16,16,"FATFS OK!");	
		LCD_ShowString(30,180,200,16,16,"U Disk Total Size:     MB");	 
		LCD_ShowString(30,200,200,16,16,"U Disk  Free Size:     MB"); 	    
		LCD_ShowNum(174,180,total>>10,5,16); //��ʾU�������� MB
		LCD_ShowNum(174,200,free>>10,5,16);	
	} 
 
	while(HCD_IsDeviceConnected(&USB_OTG_Core))//�豸���ӳɹ�
	{	
		LED1=!LED1;
		delay_ms(200);
	}
	POINT_COLOR=RED;//��������Ϊ��ɫ	   
	Show_Str(30,140,200,16,"�豸������...",16,0);
	LCD_Fill(30,160,239,220,WHITE);
	return res;
} 
void lwip_test_ui(u8 mode)
{
	u8 speed;
	u8 buf[30]; 
	POINT_COLOR=RED;
	if(mode&1<<0)
	{
		LCD_Fill(30,30,lcddev.width,110,WHITE);	//�����ʾ
		LCD_ShowString(30,30,200,16,16,"Explorer STM32F4");
		LCD_ShowString(30,50,200,16,16,"Ethernet lwIP Test");
		LCD_ShowString(30,70,200,16,16,"ATOM@ALIENTEK");
		LCD_ShowString(30,90,200,16,16,"2014/8/15"); 	
	}
	if(mode&1<<1)
	{
		LCD_Fill(30,110,lcddev.width,lcddev.height,WHITE);	//�����ʾ
		LCD_ShowString(30,110,200,16,16,"lwIP Init Successed");
		if(lwipdev.dhcpstatus==2)
			sprintf((char*)buf,"DHCP IP:%d.%d.%d.%d",lwipdev.ip[0],lwipdev.ip[1],lwipdev.ip[2],lwipdev.ip[3]);//��ӡ��̬IP��ַ
		else 
			sprintf((char*)buf,"Static IP:%d.%d.%d.%d",lwipdev.ip[0],lwipdev.ip[1],lwipdev.ip[2],lwipdev.ip[3]);//��ӡ��̬IP��ַ
		LCD_ShowString(30,130,210,16,16,buf); 
		speed=LAN8720_Get_Speed();//�õ�����
		if(speed&1<<1)
			LCD_ShowString(30,150,200,16,16,"Ethernet Speed:100M");
		else 
			LCD_ShowString(30,150,200,16,16,"Ethernet Speed:10M");
		LCD_ShowString(30,170,200,16,16,"PORT: 8087");
		LCD_ShowString(30,170,200,16,16,"KEY0: Send data");
	}
}


int main(void)
{        
	//u8 t;
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);//����ϵͳ�ж����ȼ�����2
	delay_init(168);  //��ʼ����ʱ����
	uart_init(115200);		//��ʼ�����ڲ�����Ϊ115200
	LED_Init();				//��ʼ����LED���ӵ�Ӳ���ӿ�
	KEY_Init();				//����
  LCD_Init();				//��ʼ��LCD 
	W25QXX_Init();			//SPI FLASH��ʼ��
	usmart_dev.init(84); 	//��ʼ��USMART	 
	//mydebug_12.27
	FSMC_SRAM_Init();
	my_mem_init(SRAMEX);		//��ʼ���ⲿ�ڴ��
	my_mem_init(SRAMCCM);	//��ʼ��CCM�ڴ��	
	
	my_mem_init(SRAMIN);	//��ʼ���ڲ��ڴ��	
 	exfuns_init();			//Ϊfatfs��ر��������ڴ� 
	piclib_init();			//��ʼ����ͼ
  f_mount(fs[0],"0:",1); 	//����SD��  
  f_mount(fs[1],"1:",1); 	//����SD��  
  f_mount(fs[2],"2:",1); 	//����U��
	POINT_COLOR=RED;      
 	while(font_init()) 				//����ֿ�
	{	    
		LCD_ShowString(60,50,200,16,16,"Font Error!");
		delay_ms(200);				  
		LCD_Fill(60,50,240,66,WHITE);//�����ʾ	     
		delay_ms(200);				  
	}
	Show_Str(30,50,200,16,"̽����STM32F407������",16,0);				    	 
	Show_Str(30,70,200,16,"USB U��ʵ��",16,0);					    	 
	Show_Str(30,90,200,16,"2014��7��22��",16,0);	    	 
	Show_Str(30,110,200,16,"����ԭ��@ALIENTEK",16,0); 
	Show_Str(30,140,200,16,"�豸������...",16,0);			 		
	//��ʼ��USB����
  USBH_Init(&USB_OTG_Core,USB_OTG_FS_CORE_ID,&USB_Host,&USBH_MSC_cb,&USR_Callbacks);  
	USB_OTG_Core.config=1;
	/*while(1)
	{
		if(config_finish)
		{
			delay_ms(9000);
			USB_OTG_Core.config=0;
			config_finish=0;
			printf("config over\r\n");
		}
		USBH_Process(&USB_OTG_Core, &USB_Host);
		SendMechine=1;
		Receive_Retry=2;
		delay_ms(1);
		t++;
		if(t==200)
		{
			LED0=!LED0;
			t=0;
		}
	}	*/
	//����printer����	
	while(!config_finish)
	{
		USBH_Process(&USB_OTG_Core,&USB_Host);
		SendMechine=1;
		Receive_Retry=2;
		delay_ms(1);
	}
	USB_OTG_Core.config=0;    //����´η�������
	delay_ms(3000);
	//tcp����
	lwip_test_ui(1);		//����ǰ�벿��UI
	TIM3_Int_Init(999,839); //100khz��Ƶ��,����1000Ϊ10ms
	while(lwip_comm_init()) //lwip��ʼ��
	{
		LCD_ShowString(30,150,200,20,16,"LWIP Init Falied!");
		delay_ms(1200);
		LCD_Fill(30,110,230,130,WHITE); //�����ʾ
		LCD_ShowString(30,110,200,16,16,"Retrying...");  
	}
	LCD_ShowString(30,110,200,20,16,"LWIP Init Success!");
	#if LWIP_DHCP
	while((lwipdev.dhcpstatus!=2)&&(lwipdev.dhcpstatus!=0XFF))//�ȴ�DHCP��ȡ�ɹ�/��ʱ���
	{
		lwip_periodic_handle();
	}
	#endif
	lwip_test_ui(2);		//���غ�벿��UI 
	delay_ms(500);			//��ʱ1s
	delay_ms(500);
	tcp_server_test();  	//TCP Serverģʽ
	
	/*while(1)
	{
		key = KEY_Scan(0);
		if(key == KEY1_PRES)		//��KEY1����������
		{
			if((tcp_server_flag & 1<<6)) printf("TCP�����Ѿ�����,�����ظ�����\r\n");	//������ӳɹ�,�����κδ���
			else tcp_server_test();		//���Ͽ����Ӻ�,����tcp_server_test()����
		}
		delay_ms(10);
	}
	*/
	
}










