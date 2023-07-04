/******************************************************************
开机时对DS18B20进行检测，如果DS18B20检测不正常则关闭显示；
四位数码管显示温度，若高位为0则隐藏；范围-55～+128
按键KEY3切换工作模式,LED3指示工作状态(亮则在工作,灭则不工作,制冷模式下亮灯表示正在制冷,制热模式下表示正在制热..)
******************************************************************/
#include <reg51.h>
#include <intrins.h>
#include "ds18b20.h"

sbit LED1=P1^0;	 //制冷模式指示灯
sbit LED2=P1^1;	 //加热模式指示灯
sbit LED3=P1^2;	 //温度未达到设定温度，加热/制冷工作指示灯
sbit KEY1=P1^4;	 //设置温度增加
sbit KEY2=P1^5;	 //设置温度减少
sbit KEY3=P1^6;	 //加热/制冷模式切换
sbit COD1=P3^6;  //制冷控制输出
sbit HOT1=P3^7;  //加热控制输出  

//  ActualTemperature表示实际温度，ShowTemperature用于显示温度， mode指示模式状态 0: COD 1: HOT   TargetTemperature为设定温度值
int ShowTemperature, mode, TargetTemperature; 
unsigned char b1,b2,b3;	//按键防抖
TargetTemperature = 24; //开机温度24

void main(void)
{   
  while(1)
  {   Read_Temperature();
    if(flash==0)             
       Disp_Temperature();
    else
       P2 =0xff ;        //DS18B20不正常，关闭显示 共阳数码管,置1不显示


	if(!KEY1) b1++;
    if((KEY1==1)&&(b1>0)) {TargetTemperature = (TargetTemperature+1); b1=0;}	//温度增加
	if(b1>100) TargetTemperature = (TargetTemperature+1);  //如果一直按住，自动循环加

	if(!KEY2) b2++;
    if((KEY2==1)&&(b2>0)) {TargetTemperature = (TargetTemperature-1);b2=0;}	//温度减少
	if(b2>100) TargetTemperature = (TargetTemperature-1);  //如果一直按住，自动循环减

  //mode=0:如果实测ActualTemperature大于设定TargetTemperature   LED3=0
	if( ActualTemperature > TargetTemperature) LED3=0;else LED3=1;    
  //mode=1:如果为加热模式，对制冷的工作状态取反(制冷需要工作/不工作,则这里需要不工作/工作)  如果是加热模式,再把LED3点亮. 在断点1出做判断
	if( mode == 1 ) {LED3=(!LED3);}    //这两行把两种模式 * 两种温度 = 4 种情况都攘括了,并总结出是否需要工作,由LED3指示.

	if(!KEY3) b3++;
    if((KEY3==1)&&(b3>0)) {mode=(mode+1);b3=0;}	//加热/制冷切换

  //工作模式关联LED
	if(mode==0) {LED1=0;LED2=1;}     //制冷指示灯控制
	if(mode==1) {LED1=1;LED2=0;}     //加热指示灯控制 断点1
	
	if(mode>=2) mode=0;		  //模式切换按键计数归零 mode %= 2;
  //LED3==0表示需要工作,由LED1和LED2决定工作模式
	if((LED1==0)&&(LED3==0)) COD1=0;else COD1=1; //制冷开启
	if((LED2==0)&&(LED3==0)) HOT1=0;else HOT1=1; //加热开启

  //温度限幅
	if(TargetTemperature>600) TargetTemperature=600; //设定最高温度限制
	if(TargetTemperature<-200) TargetTemperature=-200; //设定最低温度限制

  //如果TargetTemperature小于0 ShowTemperature等于TargetTemperature的补码加0X01
	if(TargetTemperature<0) ShowTemperature =~ TargetTemperature+0x01; else ShowTemperature=TargetTemperature;  

   	display[7]=ShowTemperature/1000;    //设置温度的百位数
    display[6]=ShowTemperature%1000/100;//设置温度值的十位数 
    display[5]=ShowTemperature%100/10;  //设置温度值的个位数
    display[4]=ShowTemperature%10;      //设置温度值的小数点后一位
    if(!display[7]){                    //高位为0，不显示
        display[7]=0x0a;              
        if(!display[6])                 //次高位为0，不显示
          display[6]=0x0a;
    }
    if(TargetTemperature < 0){        //如果TargetTemperature小于0（为负数）
      if(display[6]==0x0a)            //如果负温度的十位为0
        display[6]=0x0b;              //则十位用来显示负号
    else
    display[7]=0x0b;                  //否则负温度的百位显示负号
    } 
  }
}

