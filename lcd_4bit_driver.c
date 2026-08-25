#include<lpc21xx.h>
#include "header.h"

void lcd_data(unsigned char data)
{
	unsigned int temp;
	//HIGHER NIBBLE
	IOCLR1=0XFE<<16;
	temp=((data&0xf0)<<16);
	IOSET1=temp;
	IOSET1=1<<17;
	IOCLR1=1<<18;
	IOSET1=1<<19;
	delay_ms(2);
	IOCLR1=1<<19;
	
	//LOWER NIBBLE
	IOCLR1=0XFE<<16;
	temp=((data&0x0f)<<20);
	IOSET1=temp;
	IOSET1=1<<17;
	IOCLR1=1<<18;
	IOSET1=1<<19;
	delay_ms(2);
	IOCLR1=1<<19;
}

void lcd_cmd(unsigned char cmd)
{
	unsigned int temp;
	//HIGHER NIBBLE
	IOCLR1=0XFE<<16;
	temp=((cmd&0xf0)<<16);
	IOSET1=temp;
	IOCLR1=1<<17;
	IOCLR1=1<<18;
	IOSET1=1<<19;
	delay_ms(2);
	IOCLR1=1<<19;
	
	//LOWER NIBBLE
	IOCLR1=0XFE<<16;
	temp=((cmd&0x0f)<<20);
	IOSET1=temp;
	IOCLR1=1<<17;
	IOCLR1=1<<18;
	IOSET1=1<<19;
	delay_ms(2);
	IOCLR1=1<<19;
}

void lcd_init(void)
{
	IODIR1|=0XFE<<16;
	PINSEL2|=0X0;
	lcd_cmd(0x02);
	lcd_cmd(0x28);
	lcd_cmd(0x0e);
	lcd_cmd(0x01);
}
	
void lcd_string(char *s)
{
	while(*s)
		lcd_data(*s++);
}

void lcd_integer(int num)
{
	int a[10],i=0;
	if(num==0)
	{
		lcd_data('0');
		return;
	}
	if(num<0)
	{
		num=-num;
		lcd_data('-');
	}
	while(num)
	{
		a[i]=num%10+48;
		num/=10;
		i++;
	}
	for(--i;i>=0;i--)
	lcd_data(a[i]);
}


void lcd_float(float f)
{
	int num;
	if(f==0)
	{
		lcd_string("0.00");
		return;
	}
	if(f<0)
	{
		f=-f;
		lcd_data('-');
	}
	num=f;
	lcd_integer(num);
	lcd_data('.');
	num=(f-num)*1000000;
	lcd_integer(num);
}
