#include <LPC21xx.h>
#include "header.h"

/*  
   EEPROM Write Address
   Block 1:
   Global address 256 - 511
   Write SA = 0xA2
   Read  SA = 0xA3

   Block 2:
   Global address 512 - 767
   Write SA = 0xA4
   Read  SA = 0xA5
  */


s32 successWriteAddr = 256;
s32 failWriteAddr    = 512;

RTC t;

void print_bcd_lcd(u8 value)
{
    lcd_data((value >> 4) + '0');
    lcd_data((value & 0x0F) + '0');
}


/*
   READ RTC
   DS1307

   00H = Seconds
   01H = Minutes
   02H = Hours
   03H = Day
   04H = Date
   05H = Month
   06H = Year
*/

void read_rtc(void)
{
    t.sec   = i2c_read(0xD1, 0x00);
    t.min   = i2c_read(0xD1, 0x01);
    t.hr    = i2c_read(0xD1, 0x02);
    t.day   = i2c_read(0xD1, 0x03);
    t.date  = i2c_read(0xD1, 0x04);
    t.month = i2c_read(0xD1, 0x05);
    t.year  = i2c_read(0xD1, 0x06);

    t.sec = t.sec & 0x7F;

    t.hr = t.hr & 0x3F;
}


/* 
   SAVE SUCCESSFUL AUTHENTICATION

   Each record = 7 bytes

   +0 = Seconds
   +1 = Minutes
   +2 = Hours
   +3 = Day
   +4 = Date
   +5 = Month
   +6 = Year

   Block 1 = 256 to 511
*/

void saveSuccessData(void)
{
    u8 data[7];
    int i;

    /*
       One record = 7 bytes.
       Block 1 ends at 511.
    */
    if((successWriteAddr + 7) > 512)
        return;

    read_rtc();

    data[0] = t.sec;
    data[1] = t.min;
    data[2] = t.hr;
    data[3] = t.day;
    data[4] = t.date;
    data[5] = t.month;
    data[6] = t.year;

    for(i = 0; i < 7; i++)
    {
        i2c_write(
            0xA2,
            successWriteAddr,
            data[i]
        );

        delay_ms(30);

        successWriteAddr++;
    }
}


/* 
   SAVE FAILED AUTHENTICATION

   Block 2 = 512 to 767
*/

void saveFailedData(void)
{
    u8 data[7];
    int i;

    if((failWriteAddr + 7) > 768)
        return;

    read_rtc();

    data[0] = t.sec;
    data[1] = t.min;
    data[2] = t.hr;
    data[3] = t.day;
    data[4] = t.date;
    data[5] = t.month;
    data[6] = t.year;

    for(i = 0; i < 7; i++)
    {
        i2c_write(
            0xA4,
            failWriteAddr,
            data[i]
        );

        delay_ms(30);

        failWriteAddr++;
    }
}


/*
   PRINT ONE RTC RECORD

   Record:

   a[0] = seconds
   a[1] = minutes
   a[2] = hours
   a[3] = day
   a[4] = date
   a[5] = month
   a[6] = year
*/

void printTime(u8 *a)
{
    u8 sec;
    u8 min;
    u8 hr;
    u8 day;
    u8 date;
    u8 month;
    u8 year;


    sec   = a[0] & 0x7F;
    min   = a[1] & 0x7F;
    hr    = a[2] & 0x3F;
    day   = a[3];
    date  = a[4];
    month = a[5];
    year  = a[6];

    uart0_tx_string("\r\nDATE : ");
    print_bcd_uart(date);
    uart0_tx('/');
    print_bcd_uart(month);
    uart0_tx('/');
    print_bcd_uart(year);
    uart0_tx_string("   TIME : ");
    print_bcd_uart(hr);
    uart0_tx(':');
    print_bcd_uart(min);
    uart0_tx(':');
    print_bcd_uart(sec);

    lcd_cmd(0x01);
    lcd_string("DATE:");
    print_bcd_lcd(date);
    lcd_data('/');
    print_bcd_lcd(month);
    lcd_data('/');
    print_bcd_lcd(year);
    lcd_cmd(0xC0);
    lcd_string("TIME:");
    print_bcd_lcd(hr);
    lcd_data(':');
    print_bcd_lcd(min);
    lcd_data(':');
    print_bcd_lcd(sec);
}


/* 
   PRINT SUCCESS LOGS

   Block 1
   Read SA = 0xA3

   Start = 256
   End   = successWriteAddr - 1
*/

void printSuccessData(void)
{
    unsigned char a[7];

    u32 readAddr;
    u32 i;
    readAddr = 256;

    uart0_tx_string("\r\n           SUCCESS LOG \r\n");

    while(readAddr < successWriteAddr)
    {
        for(i = 0; i < 7; i++)
        {
            a[i] = i2c_read(
                0xA3,
                readAddr
            );

            readAddr++;
        }
        printTime(a);
        delay_ms(500);
    }
}


/* 
   PRINT FAILED LOGS

   Block 2
   Read SA = 0xA5
*/

void printFailedData(void)
{
    unsigned char a[7];

    u32 readAddr;
    u32 i;
    readAddr = 512;
    uart0_tx_string("\r\n========== FAILED LOG ==========\r\n");

    while(readAddr < failWriteAddr)
    {
        for(i = 0; i < 7; i++)
        {
            a[i] = i2c_read(
                0xA5,
                readAddr
            );
            readAddr++;
        }
        printTime(a);
        delay_ms(500);
    }
}


/* 
   INITIAL EEPROM DATA

   Block 0
   Write SA = 0xA0

   RFID:
   0 - 11

   Password 1:
   50 - 53

   Password 2:
   54 - 57
*/

void write(void)
{
    char s[13] = "4900C9C08FCF";

    u8 a1[5] = "1234";
    u8 a2[5] = "1111";

    int i;
    int addr;

    for(i = 0; i < 12; i++)
    {
        i2c_write(0xA0,i,s[i]);

        delay_ms(30);
    }

    addr = 50;

    for(i = 0; i < 4; i++)
    {
        i2c_write(0xA0, addr,a1[i]);

        delay_ms(30);

        addr++;
    }

    for(i = 0; i < 4; i++)
    {
        i2c_write(0xA0, addr,a2[i]);
        delay_ms(30);
        addr++;
    }
}