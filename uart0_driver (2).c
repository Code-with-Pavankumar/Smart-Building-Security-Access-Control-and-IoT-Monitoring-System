#include<lpc22xx.h>
#include "header.h"

void uart0_init(unsigned int baud)
{
int result,pclk;

if(VPBDIV==0X0)
pclk=15000000;
else if(VPBDIV==0X1)
pclk=60000000;
else if(VPBDIV==0X2)
pclk=30000000;

result=pclk/(16*baud);
PINSEL0 |= 0X05;

U0LCR=0X83;
U0DLL=result&0xff;
U0DLM=(result>>8)&0xff;
U0LCR=0X03;
}

#define THRE (U0LSR>>5&1)
void uart0_tx(unsigned char data)
{
U0THR=data;
while(THRE==0);
}

#define RDR (U0LSR&1)
unsigned char uart0_rx(void)
{
while(RDR==0);
return U0RBR;
}

void uart0_tx_string(char *s)
{
while(*s)
uart0_tx(*s++);
}

void uart0_binary(int num)
{
int pos,temp;
for(pos=31;pos>=0;pos--)
{
temp=(num>>pos&1)+48;
uart0_tx(temp);
}
}

void uart0_tx_integer(int num)
{
int a[20],i=0;
if(num==0)
{
uart0_tx('0');
return;
}
if(num<0)
{
num=-num;
uart0_tx('-');
}
while(num)
{
a[i]=num%10+48;
num/=10;
i++;
}
for(--i;i>=0;i--)
uart0_tx(a[i]);
}

void uart0_float(float f)
{
int num;
if(f==0)
{
uart0_tx_string("0.00");
return;
}
if(f<0)
{
f=-f;
uart0_tx('-');
}
num=f;
uart0_tx_integer(num);
uart0_tx('.');
num=(f-num)*1000000;
uart0_tx_integer(num);
}

int uart0_rx_integer(void)
{
int temp,num,a[15],i=0;
while(1)
{
temp=uart0_rx();
uart0_tx(temp);
if(temp==13)
break;
a[i]=temp;
i++;
}
for(--i,num=0,temp=1;i>=0;i--)
{
if(a[i]=='-')
continue;
num=(a[i]-48)*temp+num;
temp*=10;
}
if(a[0]=='-')
num=-num;
return num;
}

void uart0_rx_string(char *s,int size)
{
char i=0,temp;
while(i<size)
{
temp=uart0_rx();
uart0_tx(temp);
//lcd_data(temp);
if(temp==13)
break;
s[i]=temp;
i++;
}
s[i]=0;
}

int uart0_atoi(char *s)
{
int i=0,num;
if(s[0]=='+' || s[0]=='-')
i=1;

for(num=0;s[i];i++)
if(s[i]>='0' && s[i]<='9')
num=num*10+(s[i]-48);
else
break;
if(s[0]=='-')
num=-num;
return num;
}


unsigned int str_len(char *s)
{
int len=0;
while(*s++)
len++;
return len;
}

int check_prime(int num)
{
int n;
for(n=2;n<num;n++)
if(num%n==0)
break;
if(num==n)
return 1;
else
return 0;
}

int str_cmp(const char *s1,const char *s2)
{
int i;
for(i=0;s1[i] && s2[i];i++)
if(s1[i]!=s2[i])
return s1[i]-s2[i];
if(s1[i]==s2[i])
return 0;
else
return 1;
}