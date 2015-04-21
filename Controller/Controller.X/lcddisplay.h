/* 
 * File:   lcddisplay.h
 * Author: Oliver Jenkins
 *
 * Created on 21 April 2015, 08:21
 */

/* LCD connected to PORTC via 4-bit interface
 * LCD  Function            Pic Port.Pin
 * 1    GND
 * 2    VDD
 * 3    Contrast adjustment 10k pot, output reads 2.7v from 4.2v supply
 * 4    RS - Read Select    PortE.RE0		LCD_RS_PIN
 * 5    RW - Read/Write     PortE.RE1		LCD_RW_PIN
 * 6    E -  Enable         PortE.RE2		LCD_E_PIN
 * 7    GND
 * 8    GND
 * 9    GND
 * 10   GND
 * LOWER NIBBLE CONFIGURATION - DELETE #define UPPER
 * 11   Data                PortD.RD0
 * 12   Data                PortD.RD1
 * 13   Data                PortD.RD2
 * 14   Data                PortD.RD3
 * UPPER NIBBLE CONFIGURATION - Remember to define UPPER
 * 11   Data                PortD.RD4
 * 12   Data                PortD.RD5
 * 13   Data                PortD.RD6
 * 14   Data                PortD.RD7
 */

 //#define UPPER
 #define LCD_DATA_PORT      		PORTD
 #define LCD_TRIS_DATA_PORT 		TRISD

/* CTRL_PORT defines the port where the control lines are connected.
 * These are just samples, change to match your application.
 */

#define LCD_RS_PIN   LATEbits.LATE0   		/* PORT for RS */
#define LCD_TRIS_RS  TRISEbits.TRISE0    	/* TRIS for RS */

#define LCD_RW_PIN   LATEbits.LATE1   		/* PORT for RW */
#define LCD_TRIS_RW  TRISEbits.TRISE1    	/* TRIS for RW */

#define LCD_E_PIN    LATEbits.LATE2  		/* PORT for D  */
#define LCD_TRIS_E   TRISEbits.TRISE2    	/* TRIS for E  */

/* Display ON/OFF Control defines */
#define DON         0b00001111  /* Display on      */
#define DOFF        0b00001011  /* Display off     */
#define CURSOR_ON   0b00001111  /* Cursor on       */
#define CURSOR_OFF  0b00001101  /* Cursor off      */
#define BLINK_ON    0b00001111  /* Cursor Blink    */
#define BLINK_OFF   0b00001110  /* Cursor No Blink */

/* Cursor or Display Shift defines */
#define SHIFT_CUR_LEFT    0b00000100  /* Cursor shifts to the left   */
#define SHIFT_CUR_RIGHT   0b00000101  /* Cursor shifts to the right  */
#define SHIFT_DISP_LEFT   0b00000110  /* Display shifts to the left  */
#define SHIFT_DISP_RIGHT  0b00000111  /* Display shifts to the right */

/* Function Set defines */
#define FOUR_BIT   0b00101100  /* 4-bit Interface               */
#define EIGHT_BIT  0b00111100  /* 8-bit Interface               */
#define LINE_5X7   0b00110000  /* 5x7 characters, single line   */
#define LINE_5X10  0b00110100  /* 5x10 characters               */
#define LINES_5X7  0b00111000  /* 5x7 characters, multiple line */



void InitLCD(void);
void DelayFor18TCY(void);
void DelayPORLCD(void);


void OpenLCD(unsigned char lcdtype);
void WriteCmdLCD(unsigned char cmd);
void SetLCDCGRamAddr(unsigned char CGaddr);
void SetLCDDDRamAddr(unsigned char DDaddr);
unsigned char BusyLCD(void);
void putLCD(char *buffer);
void putIntLCD(short int n);
void WriteDataLCD(char data);

// Utiltity Methods
void LCDInitialDisplay(void);