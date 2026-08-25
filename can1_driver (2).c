/*can1_driver.c*/
#include <LPC21xx.H>
#include "header.h"

extern u8 flag;

void can1_init(void){
	VPBDIV=1;//PCLK=60MHz
	PINSEL1|=0x40000;//P0.25-->RD1
	C1MOD=1;//Select reset mode
	C1BTR=0x001C001D;//125Kbps baudrate
	AFMR=2;//accept all receiving msgs
	C1MOD=0;//release reset mode	
}

#define TCS ((C1GSR>>3)&1)
void can1_tx(CAN1 v){
	C1TID1=v.id;
	C1TFI1=v.dlc<<16;//set dlc,rtr=0,ff=0(std version)
	if(v.rtr==0){//if data-frame
		C1TDA1=v.byteA;
		C1TDB1=v.byteB;
	}
	else
		C1TFI1|=(1<<30);//rtr=1
	
	C1CMR=0x21;//Select Txbuf1 & start Xmission
	while(TCS==0);
}

#define RBS (C1GSR&1)
void can1_rx(CAN1 *ptr){
	while(RBS==0);//waiting for data-frame to receive
	ptr->id=C1RID;	
	ptr->dlc=(C1RFS>>16)&0xF;//extract dlc
	ptr->rtr=(C1RFS>>30)&1;//extract rtr
	ptr->ff=(C1RFS>>31)&1;//extract frame format
	if(ptr->rtr==0){//if data-frame
		ptr->byteA=C1RDA;
		ptr->byteB=C1RDB;
	}
	C1CMR=(1<<2);//*release rxbuf
}


void EINT0_Handler(void) __irq
{
flag = 1;
EXTINT = 1;

VICVectAddr = 0;
}

void config_vic_for_eint0(void)
{
PINSEL1 |= 1;

EXTMODE = 1;
EXTPOLAR = 0;

VICIntSelect = 0;
VICVectCntl0 = 14 | (1 << 5);
VICVectAddr0 = (u32) EINT0_Handler;
VICIntEnable = 1 << 14;
}
