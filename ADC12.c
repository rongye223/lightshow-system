/*
 * ad.c
 *
 *  Created on: 2018-2-10
 *      Author: Liu
 */
#include "ADC12.h"

//ADC 初始化
void ADInit()
{
    ADC12CTL0 |= ADC12MSC;            //自动循环采样转换
    ADC12CTL0 |= ADC12ON;               //启动ADC12模块
    ADC12CTL1 |= ADC12CONSEQ_3 ;//选择序列通道多次循环采样转换
    ADC12CTL1 |= ADC12SHP;             //采样保持模式
    ADC12CTL1  |= ADC12CSTARTADD_0;
    ADC12MCTL0 |=ADC12INCH_1;    //通道选择
    ADC12MCTL1 |=ADC12INCH_2+ADC12EOS;
    ADC12CTL0 |= ADC12ENC;

}

#define N_POINT 13
unsigned int  GetAD()
{
	unsigned int temp = 0;			//设置变量
	ADC12CTL0 |= ADC12SC;			//开始采样转换
	temp= ADC12MEM0;				//把结果赋给变量
	return temp;
}

unsigned int  GetADmic()
{
      unsigned int temp = 0;//设置变量

      ADC12CTL0 |= ADC12SC;  //开始采样转换
      temp= ADC12MEM1;           //把结果赋给变量

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
	//如果为2的n次方等分，则可以用>>n的减少计算量  a=a*8;  可以为 a=a<<3;
	//b=b/8; 可以为b=b>>3;
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
           //如果为2的n次方等分，则可以用>>n的减少计算量    a=a*8;
           //可以为 a=a<<3;   b=b/8;   可以为b=b>>3;
           return (unsigned int)(sum/(N_POINT-2));
}

