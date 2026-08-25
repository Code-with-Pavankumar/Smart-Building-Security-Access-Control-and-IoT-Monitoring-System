#include <LPC21xx.h>
#include "header.h"

// Keypad Row Pins
#define R1  9       // starting fo keypad matrix
#define R2  10
#define R3  11
#define R4  12

// Keypad Column Pins
#define C1  4      //  After that continuously connect from c1
#define C2  5
#define C3  6
#define C4  7

unsigned char key_map[4][4] =
{
    {'1', '2', '3', 'A'},
    {'4', '5', '6', 'B'},
    {'7', '8', '9', 'C'},
    {'*', '0', '#', 'D'}
};


void keypad_init(void)
{
    /*
       Configure P0.4-P0.7 as GPIO
       Configure P0.9-P0.12 as GPIO
       GPIO function = PINSEL bits = 00
    */

    PINSEL0 &= ~((3 << 8) | (3 << 10) | (3 << 12) | (3 << 14) | (3 << 18) | (3 << 20) | (3 << 22) | (3 << 24) );
    
   // Rows = OUTPUT
   // Columns = INPUT

    IODIR0 |= (1 << R1) | (1 << R2)  (1 << R3) |(1 << R4);

    IODIR0 &= ~((1 << C1) | (1 << C2) | (1 << C3) | (1 << C4));
    
   // Initially make all rows HIGH
   
   IOSET0 = (1 << R1) | (1 << R2) | (1 << R3) | (1 << R4);
}

unsigned char keypad_read(void)
{
    unsigned int row;
    unsigned int col;

    while (1)
    {
        for (row = 0; row < 4; row++)
        {
         // First make all rows HIGH
            IOSET0 = (1 << R1) | (1 << R2) | (1 << R3) | (1 << R4);
         // Make only the current row LOW
            IOCLR0 = (1 << (R1 + row));
            delay_ms(1);

         // Check all four columns
      
            for (col = 0; col < 4; col++)
            {
                if ((IOPIN0 & (1 << (C1 + col))) == 0)
                {
                    delay_ms(20); // debounce

                    if ((IOPIN0 & (1 << (C1 + col))) == 0)
                    {
                        while ((IOPIN0 & (1 << (C1 + col))) == 0);
                        return key_map[row][col];
                    }
                }
            }
        }
    }
}
 

int keypad(u8 *arr)
{
  u8 a[4] = {0};
  //char n[5]="1234";
  s32 i;
  s32 readAddr = 50;


  while(readAddr <= 57) {

  for(i = 0 ; i < 4 ; i++ , readAddr++)
  a[i] = i2c_read(0xa1 , readAddr);
  
  for(i = 0 ; i < 4 ; i++)
  if(arr[i] != a[i])
  break;
  
  if(i == 4)
  return 1;
  }
            
  return 0;
}

