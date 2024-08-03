#include <LPC17xx.h>
#include <math.h>
#define PRESCALE 29999999
#define TRIG (1 << 15) // P0.15
#define ECHO (1 << 16) // P0.16
int temp, temp1, temp2 = 0, row, flag2, x, col, key;
int flag = 0, flag1;
int i, j, k, l, r, echoTime = 5000;
float distance = 0;
void clear_ports(void);
void delay(unsigned int r1);
void clearDisplay(void);
void startTimer0(void);
float stopTimer0();
void initTimer0(void);
void delayUS(unsigned int microseconds);
void delayMS(unsigned int milliseconds);
void delayUS(unsigned int microseconds) // Using Timer0
{
LPC_SC->PCLKSEL0 &= ~(0x3 << 2); // Set PCLK_TIMER0 to divide by 1
LPC_TIM0->TCR = 0x02; // Reset timer
LPC_TIM0->PR = 0; // Set prescaler to 0
LPC_TIM0->MR0 = microseconds - 1; // Set match register for 10us
LPC_TIM0->MCR = 0x01; // Interrupt on match
LPC_TIM0->TCR = 0x01; // Enable timer
while ((LPC_TIM0->IR & 0x01) == 0)
; // Wait for interrupt flag
LPC_TIM0->TCR = 0x00; // Disable timer
LPC_TIM0->IR = 0x01;
}
void delayMS(unsigned int milliseconds) // Using Timer0
{
delayUS(milliseconds * 1000);
}
void initTimer0(void)
{
// Timer for distance
LPC_TIM0->CTCR = 0x0;
LPC_TIM0->PR = 11999999;
LPC_TIM0->TCR = 0x02; // Reset Timer
}
void startTimer0()
{
LPC_TIM0->TCR = 0x02; // Reset Timer
LPC_TIM0->TCR = 0x01; // Enable timer
}
float stopTimer0()
{
LPC_TIM0->TCR = 0x0;
return LPC_TIM0->TC;
}
void delay(unsigned int r1)
{
for (r = 0; r < r1; r++);
}
int main()
{
int ledflag = 0;
float rounded_down;
SystemInit();
SystemCoreClockUpdate();
initTimer0();
//configuring trigger, echo, light, fan, buzzer
LPC_PINCON->PINSEL0 &= 0x3fffc0ff;
LPC_PINCON->PINSEL1 &= 0xfffffffc;
//configuring keyboard
LPC_PINCON->PINSEL3 &= 0;
LPC_PINCON->PINSEL4 &= 0;
LPC_GPIO1->FIODIR |= 0 << 16| 0<<23; // Direction for ECHO PIN and keyboard
LPC_GPIO0->FIODIR |= TRIG;
LPC_GPIO2->FIODIR |= 0xf<<10;
LPC_GPIO0->FIODIR |= 0x00000070; // Direction for Light, fan and buzzer
LPC_GPIO0->FIOCLR |= TRIG;
while (1)
{
LPC_GPIO0->FIOSET = 0x00000800;
// Output 10us HIGH on TRIG pin
LPC_GPIO0->FIOMASK = 0xFFFF7FFF;
LPC_GPIO0->FIOPIN |= TRIG;
delayUS(10);
LPC_GPIO0->FIOCLR |= TRIG;
LPC_GPIO0->FIOMASK = 0x0;
while (!(LPC_GPIO0->FIOPIN & ECHO))
{ // Wait for a HIGH on ECHO pin
}
startTimer0();
// LPC_GPIO0->FIOSET = LED_Pinsel << 4;
// echoTime--;
while (LPC_GPIO0->FIOPIN & ECHO)
; // Wait for a LOW on ECHO pin
echoTime = stopTimer0(); // Stop Counting
// LPC_GPIO0->FIOCLR = LED_Pinsel << 4;
distance = (0.0343 * echoTime) / 2;
if (distance < 20)
{
LPC_GPIO0->FIOSET = 0x00000030;
}
else
{
LPC_GPIO0->FIOCLR = 0x00000030;
}
for (row=0;row<4;row++)
{ flag2=0;
LPC_GPIO2->FIOPIN = 1<<(10+row);
//sending high on ith row
x = ((LPC_GPIO1->FIOPIN>>23) & 0xf); //CHECK IF ANY COLUMN LINE IS HIGH
if(x)
{
flag2=1;
//some key is pressed
if (x == 1) //finding col no.
col = 0;
else if (x ==2)
col=1;
else if (x ==4)
col =2;
else if (x ==8)
col =3;
key = 4*row + col;
}
}
if(key == 0)
{
LPC_GPIO0->FIOSET = 0x00000040;
}
if(key == 1)
{
LPC_GPIO0->FIOSET = 0x00000030;
}
if(key == 2)
{
LPC_GPIO0->FIOCLR = 0x00000030;
}
if(key == 3)
{
LPC_GPIO0->FIOCLR = 0x00000040;
}
delay(88000);
}
}