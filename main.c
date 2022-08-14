#include <msp430.h> 
#include "ADC12.h"

unsigned int icnt;
void  ClockInit()
{//最终MCLK:16MHz,   SMCLK:8MHz,   ACLK:32KHz
    UCSCTL6 &= ~XT1OFF;          //启动XT1
     P5SEL |= BIT2 + BIT3;             //XT2引脚功能选择
     UCSCTL6 &= ~XT2OFF;          //打开XT2
     __bis_SR_register(SCG0);
     UCSCTL0 = DCO0+DCO1+DCO2+DCO3+DCO4;
     UCSCTL1 = DCORSEL_4;       //DCO频率范围在28.2MHZ以下
     UCSCTL2 = FLLD_5 + 1;          //D=16，N=1

     //n=8,FLLREFCLK时钟源为XT2CLK；
     //DCOCLK=D*(N+1)*(FLLREFCLK/n);DCOCLKDIV=(N+1)*(FLLREFCLK/n);
     UCSCTL3 = SELREF_5 + FLLREFDIV_3;
     //ACLK的时钟源为DCOCLKDIV,MCLK\SMCLK的时钟源为DCOCLK
     UCSCTL4 = SELA_4 + SELS_3 +SELM_3;
     //ACLK由DCOCLKDIV的32分频得到，SMCLK由DCOCLK的2分频得到
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

    P8DIR |= BIT1;                          // 设置P8.1口为输出模式  控制LED灯
    P8OUT &= ~BIT1;                          // 选中P8.1为输出方式
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

    TA0CTL |= MC_1 + TASSEL_2 + TACLR;      //时钟为SMCLK,比较模式，开始时清零计数器
    TA0CCTL0 = CCIE;                        //比较器中断使能
    TA0CCR0  = 50000;                       //比较值设为50000，相当于50ms的时间间隔
    __enable_interrupt();
    // 选择时钟ACLK, 清除 TAR，增计数
    TA2CTL = TASSEL_2 + TACLR + MC0;
    TA2CCR0 = 500;   // PWM周期
    TA2CCTL2 = OUTMOD_7 ;// 输出模式7
    TA2CCR2=0;
    int input=1350;
    unsigned int ivalue = 0;              //设置判断变量
    unsigned int micvalue = 0;
    while(1)
    {
          //ivalue = GetAD();               //没有软件滤波
        ivalue = Filter( );                 //软件滤波
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
            TA0CTL |= MC_1 + TASSEL_2 + TACLR;      //时钟为SMCLK,比较模式，开始时清零计数器
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
    {//10s关灯
       TA2CCR2=0;
       icnt=0;
    }
}
