本实验将实现如下功能：开机后，检测字库，然后初始化USB HOST，并不断轮询。当检测并识别U盘后，在LCD上面显示U盘总容量和剩余容量，此时便可以通过USMART调用FATFS相关函数，来测试U盘数据的读写了，方法同FATFS实验一模一样。当U盘没插入的时候，DS0闪烁，提示程序运行，当U盘插入后，DS1闪烁，提示可以通过USMART测试了。


注意：
1，本实验需要用跳线帽短接P11的D+和PA12，以及D-和PA11。
2，本实验USB_SLAVE接口不要插任何设备。
3，请自备U盘/USB读卡器（配SD卡）一个。 
 

               	正点原子@ALIENTEK
               	2014-10-25
		广州市星翼电子科技有限公司
                电话：020-38271790
                传真：020-36773971
	       	购买： http://shop62057469.taobao.com
                公司网站：www.alientek.com
         	技术论坛：www.openedv.com