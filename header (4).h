typedef unsigned int u32;
typedef int s32;
typedef char s8;
typedef unsigned char u8;

//uart declarations
void uart0_init(unsigned int);
void uart0_tx(unsigned char);
unsigned char uart0_rx(void);
void uart0_tx_string(char *); 
int uart0_rx_integer(void);
void uart0_tx_integer(int);
void uart0_tx_float(float);
void uart0_rx_string(char *,int);
void uart0_binary(int);


//Delay declarations
void delay_ms(unsigned int);
void delay_sec(unsigned int);

//Lcd-4bit driver 
void lcd_init(void);
void lcd_data(unsigned char);
void lcd_cmd(unsigned char);
void lcd_integer(int);
void lcd_string(char *);
void lcd_float(float);

// RFID declarations
int rfid_compare(s8 *);

// I2C DRIVER DECLARATIONS
void i2c_init(void);
void i2c_write(u8 sa, u8 mr, u8 data);
u8 i2c_read(u8 sa,u8 mr);

typedef struct DATA
{
	u8 sec;
	u8 min;
	u8 hr;
	u8 day;
	u8 date;
	u8 month;
	u8 year;
	
}RTC;

// Save data declarations
void write(void);
void saveFailedData(void);
void saveSuccessData(void);
void printSuccessData(void);
void printFailedData(void);
void printTime(u8 *);

// Keypad declarations
void keypad_init(void);
unsigned char keypad_read(void);
int keypad(u8 *);

// RFID declarations
int rfid_init(void);
int rfid_compare(s8 *);

// EINT0 declarations
void EINT0_Handler(void) __irq;
void config_vic_for_eint0(void);

typedef struct CAN1_MSG{
	u32 id;
	u32 byteA;
	u32 byteB;
	u8 rtr;
	u8 dlc;
	u8 ff;
}CAN1;

extern void can1_tx(CAN1 v);
extern void can1_init(void);

void esp_read(void);
void wifi_init(void);
void wifi_print(void);


void servo_init(void);
void servo_0deg_close(void);
void servo_90deg_open(void);
