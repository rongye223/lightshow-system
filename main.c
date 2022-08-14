#include <msp430.h> 
#include "ADC12.h"

unsigned int icnt;
void  ClockInit()
{//����MCLK:16MHz,   SMCLK:8MHz,   ACLK:32KHz
    UCSCTL6 &= ~XT1OFF;          //����XT1
     P5SEL |= BIT2 + BIT3;             //XT2���Ź���ѡ��
     UCSCTL6 &= ~XT2OFF;          //��XT2
     __bis_SR_register(SCG0);
     UCSCTL0 = DCO0+DCO1+DCO2+DCO3+DCO4;
     UCSCTL1 = DCORSEL_4;       //DCOƵ�ʷ�Χ��28.2MHZ����
     UCSCTL2 = FLLD_5 + 1;          //D=16��N=1

     //n=8,FLLREFCLKʱ��ԴΪXT2CLK��
     //DCOCLK=D*(N+1)*(FLLREFCLK/n);DCOCLKDIV=(N+1)*(FLLREFCLK/n);
     UCSCTL3 = SELREF_5 + FLLREFDIV_3;
     //ACLK��ʱ��ԴΪDCOCLKDIV,MCLK\SMCLK��ʱ��ԴΪDCOCLK
     UCSCTL4 = SELA_4 + SELS_3 +SELM_3;
     //ACLK��DCOCLKDIV��32��Ƶ�õ���SMCLK��DCOCLK��2��Ƶ�õ�
     UCSCTL5 = DIVA_5 +DIVS_1;
}

void IO_Init(void)
{
    P1DIR|=BIT5;
    P1OUT|=BIT5;
    P2DIR|=BIT4;
    P2OUT&=~BIT4;
    P2DIR|=BIT5;
    P2SEL|=BIT5;

    P8DIR |= BIT1;                          // ����P8.1��Ϊ���ģʽ  ����LED��
    P8OUT &= ~BIT1;                          // ѡ��P8.1Ϊ�����ʽ
    P3DIR |= BIT7;
    P3OUT &= ~BIT7;
    P7DIR |= BIT4;
    P7OUT &= ~BIT4;
    P6DIR |= BIT3;
    P6OUT &= ~BIT3;
    P6DIR |= BIT4;
    P6OUT &= ~BIT4;
    P3DIR |= BIT5;
    P3OUT &= ~BIT5;

}

int apInterface_Key( void )
{
   return 0;
}

/**
 * main.c
 */
int main(void)
{
    WDTCTL = WDTPW | WDTHOLD;   // stop watchdog timer
    IO_Init();
    ClockInit();
    ADInit();

    TA0CTL |= MC_1 + TASSEL_2 + TACLR;      //ʱ��ΪSMCLK,�Ƚ�ģʽ����ʼʱ���������
    TA0CCTL0 = CCIE;                        //�Ƚ����ж�ʹ��
    TA0CCR0  = 50000;                       //�Ƚ�ֵ��Ϊ50000���൱��50ms��ʱ����
    __enable_interrupt();
    // ѡ��ʱ��ACLK, ��� TAR��������
    TA2CTL = TASSEL_2 + TACLR + MC0;
    TA2CCR0 = 500;   // PWM����
    TA2CCTL2 = OUTMOD_7 ;// ���ģʽ7
    TA2CCR2=0;
    int input=1350;
    unsigned int ivalue = 0;              //�����жϱ���
    unsigned int micvalue = 0;
    while(1)
    {
          //ivalue = GetAD();               //û������˲�
        ivalue = Filter( );                 //����˲�
        micvalue=micFilter();
        double temp;
        if (TA2CCR2>0&&TA2CCR2<500)
        {
            if (ivalue>=input)
            {
                temp=((double)(ivalue-input))/4095*500;
                TA2CCR2+=(int)temp;
            }
            if (ivalue<input)
            {
                temp=((double)(input-ivalue))/4095*500;
                TA2CCR2-=(int)temp;
            }
            __delay_cycles(80000);
        }

        if(micvalue>=2500)
        {
            TA2CCR2=250;
            TA0CTL |= MC_1 + TASSEL_2 + TACLR;      //ʱ��ΪSMCLK,�Ƚ�ģʽ����ʼʱ���������
            icnt=0;
        }

        if (300<=ivalue)
        {
            P8OUT |= BIT1;
        }
        else P8OUT &= ~BIT1;
        if (600<=ivalue)
        {
            P3OUT |= BIT7;
        }
        else P3OUT &= ~BIT7;
        if (900<=ivalue)
        {
            P7OUT |= BIT4;
        }
        else P7OUT &= ~BIT4;
        if (1200<=ivalue)
        {

            P6OUT |= BIT3;
        }
        else P6OUT &= ~BIT3;
        if (1500<=ivalue)
        {

            P6OUT |= BIT4;
        }
        else P6OUT &= ~BIT4;
        if (1800<ivalue)
        {

            P3OUT |= BIT5;
        }
        else P3OUT &= ~BIT5;
    }

}

#pragma vector = TIMER0_A0_VECTOR
__interrupt void Timer_A (void)
{
    icnt++;
    if(icnt==1600)
    {//10s�ص�
       TA2CCR2=0;
       icnt=0;
    }
}
