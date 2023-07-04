/******************************************************************
����ʱ��DS18B20���м�⣬���DS18B20��ⲻ������ر���ʾ��
��λ�������ʾ�¶ȣ�����λΪ0�����أ���Χ-55��+128
����KEY3�л�����ģʽ,LED3ָʾ����״̬(�����ڹ���,���򲻹���,����ģʽ�����Ʊ�ʾ��������,����ģʽ�±�ʾ��������..)
******************************************************************/
#include <reg51.h>
#include <intrins.h>
#include "ds18b20.h"

sbit LED1=P1^0;	 //����ģʽָʾ��
sbit LED2=P1^1;	 //����ģʽָʾ��
sbit LED3=P1^2;	 //�¶�δ�ﵽ�趨�¶ȣ�����/���乤��ָʾ��
sbit KEY1=P1^4;	 //�����¶�����
sbit KEY2=P1^5;	 //�����¶ȼ���
sbit KEY3=P1^6;	 //����/����ģʽ�л�
sbit COD1=P3^6;  //����������
sbit HOT1=P3^7;  //���ȿ������  

//  ActualTemperature��ʾʵ���¶ȣ�ShowTemperature������ʾ�¶ȣ� modeָʾģʽ״̬ 0: COD 1: HOT   TargetTemperatureΪ�趨�¶�ֵ
int ShowTemperature, mode, TargetTemperature; 
unsigned char b1,b2,b3;	//��������
TargetTemperature = 24; //�����¶�24

void main(void)
{   
  while(1)
  {   Read_Temperature();
    if(flash==0)             
       Disp_Temperature();
    else
       P2 =0xff ;        //DS18B20���������ر���ʾ ���������,��1����ʾ


	if(!KEY1) b1++;
    if((KEY1==1)&&(b1>0)) {TargetTemperature = (TargetTemperature+1); b1=0;}	//�¶�����
	if(b1>100) TargetTemperature = (TargetTemperature+1);  //���һֱ��ס���Զ�ѭ����

	if(!KEY2) b2++;
    if((KEY2==1)&&(b2>0)) {TargetTemperature = (TargetTemperature-1);b2=0;}	//�¶ȼ���
	if(b2>100) TargetTemperature = (TargetTemperature-1);  //���һֱ��ס���Զ�ѭ����

  //mode=0:���ʵ��ActualTemperature�����趨TargetTemperature   LED3=0
	if( ActualTemperature > TargetTemperature) LED3=0;else LED3=1;    
  //mode=1:���Ϊ����ģʽ��������Ĺ���״̬ȡ��(������Ҫ����/������,��������Ҫ������/����)  ����Ǽ���ģʽ,�ٰ�LED3����. �ڶϵ�1�����ж�
	if( mode == 1 ) {LED3=(!LED3);}    //�����а�����ģʽ * �����¶� = 4 �������������,���ܽ���Ƿ���Ҫ����,��LED3ָʾ.

	if(!KEY3) b3++;
    if((KEY3==1)&&(b3>0)) {mode=(mode+1);b3=0;}	//����/�����л�

  //����ģʽ����LED
	if(mode==0) {LED1=0;LED2=1;}     //����ָʾ�ƿ���
	if(mode==1) {LED1=1;LED2=0;}     //����ָʾ�ƿ��� �ϵ�1
	
	if(mode>=2) mode=0;		  //ģʽ�л������������� mode %= 2;
  //LED3==0��ʾ��Ҫ����,��LED1��LED2��������ģʽ
	if((LED1==0)&&(LED3==0)) COD1=0;else COD1=1; //���俪��
	if((LED2==0)&&(LED3==0)) HOT1=0;else HOT1=1; //���ȿ���

  //�¶��޷�
	if(TargetTemperature>600) TargetTemperature=600; //�趨����¶�����
	if(TargetTemperature<-200) TargetTemperature=-200; //�趨����¶�����

  //���TargetTemperatureС��0 ShowTemperature����TargetTemperature�Ĳ����0X01
	if(TargetTemperature<0) ShowTemperature =~ TargetTemperature+0x01; else ShowTemperature=TargetTemperature;  

   	display[7]=ShowTemperature/1000;    //�����¶ȵİ�λ��
    display[6]=ShowTemperature%1000/100;//�����¶�ֵ��ʮλ�� 
    display[5]=ShowTemperature%100/10;  //�����¶�ֵ�ĸ�λ��
    display[4]=ShowTemperature%10;      //�����¶�ֵ��С�����һλ
    if(!display[7]){                    //��λΪ0������ʾ
        display[7]=0x0a;              
        if(!display[6])                 //�θ�λΪ0������ʾ
          display[6]=0x0a;
    }
    if(TargetTemperature < 0){        //���TargetTemperatureС��0��Ϊ������
      if(display[6]==0x0a)            //������¶ȵ�ʮλΪ0
        display[6]=0x0b;              //��ʮλ������ʾ����
    else
    display[7]=0x0b;                  //�����¶ȵİ�λ��ʾ����
    } 
  }
}

