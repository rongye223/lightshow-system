/*
 * ad.c
 *
 *  Created on: 2018-2-10
 *      Author: Liu
 */
#include "ADC12.h"

//ADC ��ʼ��
void ADInit()
{
    ADC12CTL0 |= ADC12MSC;            //�Զ�ѭ������ת��
    ADC12CTL0 |= ADC12ON;               //����ADC12ģ��
    ADC12CTL1 |= ADC12CONSEQ_3 ;//ѡ������ͨ�����ѭ������ת��
    ADC12CTL1 |= ADC12SHP;             //��������ģʽ
    ADC12CTL1  |= ADC12CSTARTADD_0;
    ADC12MCTL0 |=ADC12INCH_1;    //ͨ��ѡ��
    ADC12MCTL1 |=ADC12INCH_2+ADC12EOS;
    ADC12CTL0 |= ADC12ENC;

}

#define N_POINT 13
unsigned int  GetAD()
{
	unsigned int temp = 0;			//���ñ���
	ADC12CTL0 |= ADC12SC;			//��ʼ����ת��
	temp= ADC12MEM0;				//�ѽ����������
	return temp;
}

unsigned int  GetADmic()
{
      unsigned int temp = 0;//���ñ���

      ADC12CTL0 |= ADC12SC;  //��ʼ����ת��
      temp= ADC12MEM1;           //�ѽ����������

      return temp;
}

unsigned int Filter( )
{
     char count,i,j;
     unsigned int value_buf[N_POINT];
     unsigned int temp=0;
     int sum=0;
	  for (count=0;count<N_POINT;count++)
	  {
	    	value_buf[count] = GetAD();
	  }
	  for (j=0;j<N_POINT-1;j++)
	  {
	    	for (i=0;i<N_POINT-j-1;i++)
		   {
			    if ( value_buf[i]>value_buf[i+1] )
			  {
				   temp = value_buf[i];
				   value_buf[i] = value_buf[i+1];
				   value_buf[i+1] = temp;
			   }
		   }
	}
	for(count=1;count<N_POINT-1;count++)
	sum += value_buf[count];
	//���Ϊ2��n�η��ȷ֣��������>>n�ļ��ټ�����  a=a*8;  ����Ϊ a=a<<3;
	//b=b/8; ����Ϊb=b>>3;
	return (unsigned int)(sum/(N_POINT-2));
}

unsigned int micFilter( )
{
     char count,i,j;
     unsigned int value_buf[N_POINT];
     unsigned int temp=0;
     int sum=0;
     for (count=0;count<N_POINT;count++)
     {
          value_buf[count] = GetADmic();
     }
     for (j=0;j<N_POINT-1;j++)
     {
          for (i=0;i<N_POINT-j-1;i++)
          {
                if ( value_buf[i]>value_buf[i+1] )
               {
                    temp = value_buf[i];
                    value_buf[i] = value_buf[i+1];
                    value_buf[i+1] = temp;
               }
          }
     }
     for(count=1;count<N_POINT-1;count++)
           {
                 sum += value_buf[count];
           }
           //���Ϊ2��n�η��ȷ֣��������>>n�ļ��ټ�����    a=a*8;
           //����Ϊ a=a<<3;   b=b/8;   ����Ϊb=b>>3;
           return (unsigned int)(sum/(N_POINT-2));
}

