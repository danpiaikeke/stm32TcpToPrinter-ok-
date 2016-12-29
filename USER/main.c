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

//ALIENTEK 探索者STM32F407开发板 实验53
//USB U盘 实验-库函数版本 
//技术支持：www.openedv.com
//淘宝店铺：http://eboard.taobao.com
//广州市星翼电子科技有限公司    
//作者：正点原子 @ALIENTEK 

USBH_HOST  USB_Host;
USB_OTG_CORE_HANDLE  USB_OTG_Core;
extern u8 config_finish;
extern u8 send_finish;

//用户测试主程序
//返回值:0,正常
//       1,有问题
u8 USH_User_App(void)
{ 
	u32 total,free;
	u8 res=0;
	Show_Str(30,140,200,16,"设备连接成功!.",16,0);	 
	res=exf_getfree("2:",&total,&free);
	if(res==0)
	{
		POINT_COLOR=BLUE;//设置字体为蓝色	   
		LCD_ShowString(30,160,200,16,16,"FATFS OK!");	
		LCD_ShowString(30,180,200,16,16,"U Disk Total Size:     MB");	 
		LCD_ShowString(30,200,200,16,16,"U Disk  Free Size:     MB"); 	    
		LCD_ShowNum(174,180,total>>10,5,16); //显示U盘总容量 MB
		LCD_ShowNum(174,200,free>>10,5,16);	
	} 
 
	while(HCD_IsDeviceConnected(&USB_OTG_Core))//设备连接成功
	{	
		LED1=!LED1;
		delay_ms(200);
	}
	POINT_COLOR=RED;//设置字体为红色	   
	Show_Str(30,140,200,16,"设备连接中...",16,0);
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
		LCD_Fill(30,30,lcddev.width,110,WHITE);	//清除显示
		LCD_ShowString(30,30,200,16,16,"Explorer STM32F4");
		LCD_ShowString(30,50,200,16,16,"Ethernet lwIP Test");
		LCD_ShowString(30,70,200,16,16,"ATOM@ALIENTEK");
		LCD_ShowString(30,90,200,16,16,"2014/8/15"); 	
	}
	if(mode&1<<1)
	{
		LCD_Fill(30,110,lcddev.width,lcddev.height,WHITE);	//清除显示
		LCD_ShowString(30,110,200,16,16,"lwIP Init Successed");
		if(lwipdev.dhcpstatus==2)
			sprintf((char*)buf,"DHCP IP:%d.%d.%d.%d",lwipdev.ip[0],lwipdev.ip[1],lwipdev.ip[2],lwipdev.ip[3]);//打印动态IP地址
		else 
			sprintf((char*)buf,"Static IP:%d.%d.%d.%d",lwipdev.ip[0],lwipdev.ip[1],lwipdev.ip[2],lwipdev.ip[3]);//打印静态IP地址
		LCD_ShowString(30,130,210,16,16,buf); 
		speed=LAN8720_Get_Speed();//得到网速
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
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);//设置系统中断优先级分组2
	delay_init(168);  //初始化延时函数
	uart_init(115200);		//初始化串口波特率为115200
	LED_Init();				//初始化与LED连接的硬件接口
	KEY_Init();				//按键
  LCD_Init();				//初始化LCD 
	W25QXX_Init();			//SPI FLASH初始化
	usmart_dev.init(84); 	//初始化USMART	 
	//mydebug_12.27
	FSMC_SRAM_Init();
	my_mem_init(SRAMEX);		//初始化外部内存池
	my_mem_init(SRAMCCM);	//初始化CCM内存池	
	
	my_mem_init(SRAMIN);	//初始化内部内存池	
 	exfuns_init();			//为fatfs相关变量申请内存 
	piclib_init();			//初始化画图
  f_mount(fs[0],"0:",1); 	//挂载SD卡  
  f_mount(fs[1],"1:",1); 	//挂载SD卡  
  f_mount(fs[2],"2:",1); 	//挂载U盘
	POINT_COLOR=RED;      
 	while(font_init()) 				//检查字库
	{	    
		LCD_ShowString(60,50,200,16,16,"Font Error!");
		delay_ms(200);				  
		LCD_Fill(60,50,240,66,WHITE);//清除显示	     
		delay_ms(200);				  
	}
	Show_Str(30,50,200,16,"探索者STM32F407开发板",16,0);				    	 
	Show_Str(30,70,200,16,"USB U盘实验",16,0);					    	 
	Show_Str(30,90,200,16,"2014年7月22日",16,0);	    	 
	Show_Str(30,110,200,16,"正点原子@ALIENTEK",16,0); 
	Show_Str(30,140,200,16,"设备连接中...",16,0);			 		
	//初始化USB主机
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
	//发送printer配置	
	while(!config_finish)
	{
		USBH_Process(&USB_OTG_Core,&USB_Host);
		SendMechine=1;
		Receive_Retry=2;
		delay_ms(1);
	}
	USB_OTG_Core.config=0;    //标记下次发送数据
	delay_ms(3000);
	//tcp配置
	lwip_test_ui(1);		//加载前半部分UI
	TIM3_Int_Init(999,839); //100khz的频率,计数1000为10ms
	while(lwip_comm_init()) //lwip初始化
	{
		LCD_ShowString(30,150,200,20,16,"LWIP Init Falied!");
		delay_ms(1200);
		LCD_Fill(30,110,230,130,WHITE); //清除显示
		LCD_ShowString(30,110,200,16,16,"Retrying...");  
	}
	LCD_ShowString(30,110,200,20,16,"LWIP Init Success!");
	#if LWIP_DHCP
	while((lwipdev.dhcpstatus!=2)&&(lwipdev.dhcpstatus!=0XFF))//等待DHCP获取成功/超时溢出
	{
		lwip_periodic_handle();
	}
	#endif
	lwip_test_ui(2);		//加载后半部分UI 
	delay_ms(500);			//延时1s
	delay_ms(500);
	tcp_server_test();  	//TCP Server模式
	
	/*while(1)
	{
		key = KEY_Scan(0);
		if(key == KEY1_PRES)		//按KEY1键建立连接
		{
			if((tcp_server_flag & 1<<6)) printf("TCP连接已经建立,不能重复连接\r\n");	//如果连接成功,不做任何处理
			else tcp_server_test();		//当断开连接后,调用tcp_server_test()函数
		}
		delay_ms(10);
	}
	*/
	
}










