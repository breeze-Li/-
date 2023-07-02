#ifndef _DS18B20_H_
#define _DS18B20_H_

#define uint unsigned int
#define uchar unsigned char  //宏定义
sbit DQ  =P1^3;  //定义DS18B20端口DQ   
uchar presence;	   //初始化DS18B20，若成功返回０，若失败返回１
uchar code  LEDData[]   = {0xC0,0xF9,0xA4,0xB0,0x99,0x92,0x82,0xF8,0x80,0x90,0xff,0xbf}; //数码管字型码，倒数两个分别是空字符和负号标志
uchar data  temp_data[2]= {0x00,0x00};	//存放温度高８位和低８位
uint data  display[8]  = {0x00,0x00,0x00,0x00};  //存放温度的具体数字或符号
uchar code  ditab[16]   = {0x00,0x01,0x01,0x02,0x03,0x03,0x04,0x04,0x05,0x06,0x06,0x07,0x08,0x08,0x09,0x09}; //小数位的查表值，参考DS18B20技术文档
bit  flash=0;        //显示开关标记
uchar NegativeFlag=0;//负数标志  
 
int ActualTemperature;//实际温度  



/*****延时子程序*****/
void Delay(uint num)
{
while( --num );
}

/*****初始化DS18B20*****/
Init_DS18B20(void)
{
DQ = 1;          //DQ复位
Delay(8);        //稍做延时
DQ = 0;          //单片机将DQ拉低
Delay(90);       //精确延时大于480us
DQ = 1;           //拉高总线
Delay(8);
presence = DQ;       //如果=0则初始化成功 =1则初始化失败
Delay(100);
DQ = 1; 
return(presence);    //返回信号，0=presence,1= no presence
}

/*****读字节子程序**主要用于提取温度传感器转换过来的温度***/
ReadOneChar(void)    //读一个字节
{
uchar i = 0;
uchar dat = 0;
for (i = 8; i > 0; i--)
   {
     DQ = 0;     // 给脉冲信号
     dat >>= 1;
     DQ = 1;     // 给脉冲信号
     if(DQ)		 //低位先读，如ＤＱ=1001　1010 ,则先读最右边的０，然后向右移动，再循环接着读
      dat |= 0x80;
     Delay(30);
   }
    return (dat);
}

/*****写字节子程序**主要用于向温度传感器发送命令***/
WriteOneChar(uchar dat)   //写一个字节
{
uchar i = 0;
for (i = 8; i > 0; i--)
   {
     DQ = 0;
     DQ = dat&0x01;	  //低位先写，若字节为1011　1010，则先写最右边的位
     Delay(5);

     DQ = 1;
     dat>>=1;
  }
}

/*****读温度子程序******/
Read_Temperature(void)   //读取温度
{
   Init_DS18B20();
   if(presence==1)   	//若初始化不成功     
  		flash=1;  		//关闭显示    
    else 
    {
		flash=0;
		WriteOneChar(0xCC);  //跳过读序号列号的操作
		WriteOneChar(0x44);  //启动温度转换

        Init_DS18B20();	  	 //不能删掉
		WriteOneChar(0xCC);  //跳过读序号列号的操作　不能删掉
		WriteOneChar(0xBE);  //读取温度寄存器

		temp_data[0] = ReadOneChar();//温度低8位
		temp_data[1] = ReadOneChar();//温度高8位 

		if((temp_data[1]&0xf8)==0xf8)	//若是负温度，则要计算反码和补码
		{
			temp_data[1]=~temp_data[1];
			temp_data[0]=~temp_data[0]+1;

			if(temp_data[0]==0x00) temp_data[1]++;
			NegativeFlag=1;	//负数
		}
		else
			NegativeFlag=0;	//正数
	}
}
    						 
Disp_Temperature()     		//显示温度
{   uchar k,tt;				//tt用于存放计算过程中的中间量
    tt=temp_data[0]&0x0f;
    display[0]=ditab[tt];   //查表得小数位的值，放在display[0]中
    tt=((temp_data[0]&0xf0)>>4)|((temp_data[1]&0x0f)<<4);  //将高8位的低4位和低8位的高4位合并到一起，这是温度的整数部分
    display[3]=tt/100;		//取温度值的百位数 如125度的“1”放在display[3]中
    display[2]=tt%100/10;	//取温度值的十位数，如125度的“2”，放在display[2]中
    display[1]=tt%10;		//取温度值的个位数，如125度的‘5’，放在display[1]中

	if( NegativeFlag )		//如果NegativeFlag=1温度为负数（零下） 十位乘以100加上个位乘以10加上小数点后一位乘以1。(零下最高权位为十位)
		ActualTemperature=~(display[2]*100+display[1]*10+display[0]*1)+0x01; 

	else					//否则温度为正数（零上） 等于百位乘以1000加上十位乘以100加上个位乘以10加上小数点后一位乘以1.(领上最高权位为百位)
		ActualTemperature=(display[3]*1000+display[2]*100+display[1]*10+display[0]*1);	
    if(!display[3])          //高位为0，不显示
    {   display[3]=0x0a;              
        if(!display[2])      //次高位为0，不显示
        display[2]=0x0a;
    }
    if( NegativeFlag )
    {   if(display[2]==0x0a)	//如果负温度的十位为0
	        display[2]=0x0b; 	//则十位用来显示负号
	     else
	     display[3]=0x0b;  		//否则负温度的百位显示负号
    }
	 
	/*数码管显示*/
    for( k=0; k<=7; k++) 		//数码管扫描位数0~7
    {   P0 =LEDData[display[k]]&((k==1 || k==5)?0x7f:0xff); //显示小数位 k==1 display[1] 加小数点	 k==5 display[5] 加小数点
        P2 =_cror_(0xf7,k);
        Delay(100);
        P2 = 0xff; 
    }
}

#endif