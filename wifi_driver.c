 #include"header.h"
 #include<lpc21xx.h>

 void esp_read(void){
 	unsigned char ch;
	while(1){
		ch=uart0_rx();
		uart0_tx(ch);
	}
}


void wifi_init(void)
{
    uart0_tx_string("\r\nESP-01 Init\r\n");

    uart0_tx_string("AT\r\n");
    delay_ms(1000);
	uart0_tx_string("AT+CWMODE=1\r\n");
	delay_ms(1000);
    uart0_tx_string("AT+CWJAP=\"BSNL3G\",\"12345678\"\r\n");
    delay_ms(1000);
	uart0_tx_string("AT+CIPMUX=0\r\n");
    delay_ms(1000);  
    uart0_tx_string("WiFi Ready\r\n");
}

   //wifi_print — call after authentication result is known
   //field1 = 1 (access granted) or 0 (access denied)
   //field2 = 1 (door open)      or 0 (door closed)
 
void wifi_print(int field1, int field2)
{
    uart0_tx_string("\r\nUploading to ThingSpeak\r\n");

    uart0_tx_string("AT+CIPSTART=\"TCP\",\"api.thingspeak.com\",80\r\n");
	delay_ms(3000);

    uart0_tx_string("AT+CIPSEND=128\r\n");
    delay_ms(1000);

    /* HTTP GET — everything on one line, no breaks inside URL */
    uart0_tx_string("GET /update?api_key=P3NMKR3M8JKHDZFJ&field1=");
    uart0_tx_integer(field1);
    uart0_tx_string("&field2=");
    uart0_tx_integer(field2);
    uart0_tx_string(" HTTP/1.1\r\n");
    uart0_tx_string("Host: api.thingspeak.com\r\n");
    uart0_tx_string("Connection: close\r\n");
    uart0_tx_string("\r\n");

    delay_ms(3000);

    uart0_tx_string("AT+CIPCLOSE\r\n");
    
	delay_ms(1000);

    uart0_tx_string("Upload Done\r\n");
}
