#include <LPC21xx.H>
#include "header.h"

// PIR 
#define MOTION      (1 << 15)       // P0.15

// LEDs 
#define GREEN       (1 << 13)       // P0.13
#define RED         (1 << 14)       // P0.14

// PIR status 
#define PIR_STATUS  MOTION

u8 flag = 0;
CAN1 m1;

int main(void)
{
    s32 c = 0;
    s32 f = 0;
    s32 i;
    s32 addr = 50;

    char ch;

    u8 a[4] = {0};

   //    P0.15 = PIR INPUT
   //    P0.13 = GREEN LED OUTPUT
   //    P0.14 = RED LED OUTPUT
    
    IODIR0 &= ~MOTION;

    IODIR0 |= GREEN | RED;

    IOCLR0 = GREEN | RED;

    uart0_init(9600);
    lcd_init();
    i2c_init();
    wifi_init();
    can1_init();
    // Servo  P0.8 -> PWM4
    servo_init();

    m1.id = 0x123;
    m1.rtr = 0;           // Data frame 
    m1.dlc = 1;
    m1.byteA = 0x00;
    m1.byteB = 0x00;

    uart0_tx_string("\r\n           SMART BUILDING SECURITY\r\n");
    lcd_string("Smart Building");
    uart0_tx_string("\r\nWriting data on EEPROM");
    write();
    uart0_tx_string("\r\nPIR MOTION INITIALIZED");

/*  Displaying stored RFID ID and Password
    uart0_tx_string("\r\nRFID UID :  ");

    for(i = 0; i < 12; i++)
    {
        ch = i2c_read(0xa1, i);

        uart0_tx(ch);
    }

    uart0_tx_string("\r\n\r\nPassword :  ");

    for(i = 0; i < 4; i++, addr++)
    {
        ch = i2c_read(0xa1, addr);

        uart0_tx(ch);
     }
*/
   while(1)
    {
        uart0_tx_string("\r\nMonitoring for Motion");

        lcd_cmd(0x01);

        lcd_string("Monitoring");

        delay_sec(6);
check:
        if(IOPIN0 & PIR_STATUS)
        {
            uart0_tx_string("\r\nHUMAN DETECTED");
            lcd_cmd(0x01);
            lcd_string("Human Detected");
            c = 0;
rfid:
            if(rfid_init())
            {
                uart0_tx_string("\r\nRFID CARD Verified Successfully");
                lcd_cmd(0x01);
                lcd_string("RFID verified");
                f = 1;
                goto save;
            }
            else
            {
                c++;
                lcd_cmd(0x01);
                lcd_string("RFID Failed");
                uart0_tx_string("\r\nRFID CARD failed");
                uart0_tx_string("\r\nScan Again");
                lcd_cmd(0x01);
                lcd_string("Scan RFID");
                if(c <= 1)
                {
                    goto rfid;
                }
                else
                {
                    lcd_cmd(0x01);
                    lcd_string("RFID Failed");
                    uart0_tx_string("\r\nRFID Authentication failed");
                }
            }
            // Password Authentication
            c = 0;
            uart0_tx_string("\r\nWaiting for Password");
            
            lcd_cmd(0x01);
            lcd_string("Enter Password");
            keypad_init();
pass:
            uart0_tx_string("\r\nEnter a password : ");
            for(i = 0; i < 4; i++)
            {
                a[i] = keypad_read();
                uart0_tx(a[i]);
            }

            // Check password against EEPROM
            if(keypad(a))
            {
                lcd_cmd(0x01);
                lcd_string("Pass verified");
               
                uart0_tx_string("\r\nPassword verified successfully");
                f = 1;
                goto save;
            }
            else
            {
                c++;
                lcd_cmd(0x01);
                lcd_string("Wrong Pass");
               
                uart0_tx_string("\r\nPassword wrong");
                uart0_tx_string("\r\nLast chance enter correct password");
                if(c <= 1)
                {
                    goto pass;
                }
            }
save:
            if(f == 1)
            {
                m1.byteA = 0x10;
                can1_tx(m1);
                saveSuccessData();
                wifi_print(1 , 0);
            }
            else
            {
                m1.byteA = 0x11;
                can1_tx(m1);
                lcd_cmd(0x01);
                lcd_string("Pass Failed");
               
                uart0_tx_string("\r\nPassword authentication Failed");
                saveFailedData();
                wifi_print(0 , 0);
                goto check;
            }
            f = 0;
            uart0_tx_string("\r\nPassword Authentication Success");
            lcd_cmd(0x01);
            lcd_string("Pass Verified");
            
            servo_90deg_open();
            delay_sec(4);
            uart0_tx_string("\r\nDoor Opened");
            
            delay_sec(3);
            lcd_cmd(0x01);

            servo_0deg_close();
            delay_ms(2000);
            uart0_tx_string("\r\nDoor Closed");
        }
        delay_ms(4000);

        if(flag == 1)
        {
            flag = 0;
            uart0_tx_string("\r\nPrint success Data\r\n");
            lcd_cmd(0x01);

            lcd_string("Print Data");

            printSuccessData();
            delay_ms(2000);

            uart0_tx_string("\r\nPrint Failed data\r\n");
            printFailedData();

            delay_ms(2000);
        }
    }
}