#include<lpc21xx.h>
#include<string.h>
#include "header.h"

int rfid_init(void)
{
	char s[13] = {0};
	uart0_tx_string("\r\nRFID Initialized");
	uart0_tx_string("\r\nWaiting for RFID Card\r\n");
	uart0_rx_string(s , 12);
	uart0_tx_string("RFID Card detected\r\n");
	uart0_tx_string("\r\nRFID CARD ID: ");
	uart0_tx_string(s);
      delay_ms(200);
	if(rfid_compare(s))
		return 1;
	return 0;
}

int rfid_compare(s8 * s)
{
   //char m[13]="4900C9C08FCF";
	u8 id[12] = {0};
	s32 i;

	for(i = 0 ; i < 12 ; i++)
	id[i] = i2c_read(0xa1 ,i);
	id[i] = 0;
	uart0_tx_string(s);
      delay_ms(200);
	
      for(i = 0 ; s[i] && id[i] ; i++)
      if(s[i] != id[i])
      return 0;
      
      return 1;
}