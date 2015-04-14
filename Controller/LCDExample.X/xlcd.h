/* 
 * File:   xlcd.h
 * Author: Oliver
 

 * Copied and modified from xlcd.h.  However functions placed in-line as to avoid re-compilation issues
 * Header files reference functions that must be re-compiled with an unknown method
 * By putting all functions inline, and renaming constants I avoid this step
 * Its working around my ignorance to do it this way
 *
 * LCD connected to PORTC via 4-bit interface
 * LCD  Function            Pic Port.Pin
 * 1    GND
 * 2    VDD
 * 3    Contrast adjustment 10k pot, output reads 2.7v from 4.2v supply
 * 4    RS - Read Select    PortC.RC4		LCD_RS_PIN
 * 5    RW - Read/Write     PortC.RC5		LCD_RW_PIN
 * 6    E -  Enable         PortC.RC6		LCD_E_PIN
 * 7    GND
 * 8    GND
 * 9    GND
 * 10   GND
 * 11   Data                PortC.RC0
 * 12   Data                PortC.RC1
 * 13   Data                PortC.RC2
 * 14   Data                PortC.RC3

 * Created on 14 April 2015, 11:41
 */


 #define LCD_DATA_PORT      		PORTC
 #define LCD_TRIS_DATA_PORT 		TRISC

/* CTRL_PORT defines the port where the control lines are connected.
 * These are just samples, change to match your application.
 */
 #define LCD_RW_PIN   LATCbits.LATC5   		/* PORT for RW */
 #define LCD_TRIS_RW  TRISCbits.TRISC5    	/* TRIS for RW */

 #define LCD_RS_PIN   LATCbits.LATC4   		/* PORT for RS */
 #define LCD_TRIS_RS  TRISCbits.TRISC4    	/* TRIS for RS */

 #define LCD_E_PIN    LATCbits.LATC6  		/* PORT for D  */
 #define LCD_TRIS_E   TRISCbits.TRISC6    	/* TRIS for E  */

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