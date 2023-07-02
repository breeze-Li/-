#ifndef _DS18B20_H_
#define _DS18B20_H_

#define uint unsigned int
#define uchar unsigned char  //�궨��
sbit DQ  =P1^3;  //����DS18B20�˿�DQ   
uchar presence;	   //��ʼ��DS18B20�����ɹ����أ�����ʧ�ܷ��أ�
uchar code  LEDData[]   = {0xC0,0xF9,0xA4,0xB0,0x99,0x92,0x82,0xF8,0x80,0x90,0xff,0xbf}; //����������룬���������ֱ��ǿ��ַ��͸��ű�־
uchar data  temp_data[2]= {0x00,0x00};	//����¶ȸߣ�λ�͵ͣ�λ
uint data  display[8]  = {0x00,0x00,0x00,0x00};  //����¶ȵľ������ֻ����
uchar code  ditab[16]   = {0x00,0x01,0x01,0x02,0x03,0x03,0x04,0x04,0x05,0x06,0x06,0x07,0x08,0x08,0x09,0x09}; //С��λ�Ĳ��ֵ���ο�DS18B20�����ĵ�
bit  flash=0;        //��ʾ���ر��
uchar NegativeFlag=0;//������־  
 
int ActualTemperature;//ʵ���¶�  



/*****��ʱ�ӳ���*****/
void Delay(uint num)
{
while( --num );
}

/*****��ʼ��DS18B20*****/
Init_DS18B20(void)
{
DQ = 1;          //DQ��λ
Delay(8);        //������ʱ
DQ = 0;          //��Ƭ����DQ����
Delay(90);       //��ȷ��ʱ����480us
DQ = 1;           //��������
Delay(8);
presence = DQ;       //���=0���ʼ���ɹ� =1���ʼ��ʧ��
Delay(100);
DQ = 1; 
return(presence);    //�����źţ�0=presence,1= no presence
}

/*****���ֽ��ӳ���**��Ҫ������ȡ�¶ȴ�����ת���������¶�***/
ReadOneChar(void)    //��һ���ֽ�
{
uchar i = 0;
uchar dat = 0;
for (i = 8; i > 0; i--)
   {
     DQ = 0;     // �������ź�
     dat >>= 1;
     DQ = 1;     // �������ź�
     if(DQ)		 //��λ�ȶ�����ģ�=1001��1010 ,���ȶ����ұߵģ���Ȼ�������ƶ�����ѭ�����Ŷ�
      dat |= 0x80;
     Delay(30);
   }
    return (dat);
}

/*****д�ֽ��ӳ���**��Ҫ�������¶ȴ�������������***/
WriteOneChar(uchar dat)   //дһ���ֽ�
{
uchar i = 0;
for (i = 8; i > 0; i--)
   {
     DQ = 0;
     DQ = dat&0x01;	  //��λ��д�����ֽ�Ϊ1011��1010������д���ұߵ�λ
     Delay(5);

     DQ = 1;
     dat>>=1;
  }
}

/*****���¶��ӳ���******/
Read_Temperature(void)   //��ȡ�¶�
{
   Init_DS18B20();
   if(presence==1)   	//����ʼ�����ɹ�     
  		flash=1;  		//�ر���ʾ    
    else 
    {
		flash=0;
		WriteOneChar(0xCC);  //����������кŵĲ���
		WriteOneChar(0x44);  //�����¶�ת��

        Init_DS18B20();	  	 //����ɾ��
		WriteOneChar(0xCC);  //����������кŵĲ���������ɾ��
		WriteOneChar(0xBE);  //��ȡ�¶ȼĴ���

		temp_data[0] = ReadOneChar();//�¶ȵ�8λ
		temp_data[1] = ReadOneChar();//�¶ȸ�8λ 

		if((temp_data[1]&0xf8)==0xf8)	//���Ǹ��¶ȣ���Ҫ���㷴��Ͳ���
		{
			temp_data[1]=~temp_data[1];
			temp_data[0]=~temp_data[0]+1;

			if(temp_data[0]==0x00) temp_data[1]++;
			NegativeFlag=1;	//����
		}
		else
			NegativeFlag=0;	//����
	}
}
    						 
Disp_Temperature()     		//��ʾ�¶�
{   uchar k,tt;				//tt���ڴ�ż�������е��м���
    tt=temp_data[0]&0x0f;
    display[0]=ditab[tt];   //����С��λ��ֵ������display[0]��
    tt=((temp_data[0]&0xf0)>>4)|((temp_data[1]&0x0f)<<4);  //����8λ�ĵ�4λ�͵�8λ�ĸ�4λ�ϲ���һ�������¶ȵ���������
    display[3]=tt/100;		//ȡ�¶�ֵ�İ�λ�� ��125�ȵġ�1������display[3]��
    display[2]=tt%100/10;	//ȡ�¶�ֵ��ʮλ������125�ȵġ�2��������display[2]��
    display[1]=tt%10;		//ȡ�¶�ֵ�ĸ�λ������125�ȵġ�5��������display[1]��

	if( NegativeFlag )		//���NegativeFlag=1�¶�Ϊ���������£� ʮλ����100���ϸ�λ����10����С�����һλ����1��(�������ȨλΪʮλ)
		ActualTemperature=~(display[2]*100+display[1]*10+display[0]*1)+0x01; 

	else					//�����¶�Ϊ���������ϣ� ���ڰ�λ����1000����ʮλ����100���ϸ�λ����10����С�����һλ����1.(�������ȨλΪ��λ)
		ActualTemperature=(display[3]*1000+display[2]*100+display[1]*10+display[0]*1);	
    if(!display[3])          //��λΪ0������ʾ
    {   display[3]=0x0a;              
        if(!display[2])      //�θ�λΪ0������ʾ
        display[2]=0x0a;
    }
    if( NegativeFlag )
    {   if(display[2]==0x0a)	//������¶ȵ�ʮλΪ0
	        display[2]=0x0b; 	//��ʮλ������ʾ����
	     else
	     display[3]=0x0b;  		//�����¶ȵİ�λ��ʾ����
    }
	 
	/*�������ʾ*/
    for( k=0; k<=7; k++) 		//�����ɨ��λ��0~7
    {   P0 =LEDData[display[k]]&((k==1 || k==5)?0x7f:0xff); //��ʾС��λ k==1 display[1] ��С����	 k==5 display[5] ��С����
        P2 =_cror_(0xf7,k);
        Delay(100);
        P2 = 0xff; 
    }
}

#endif