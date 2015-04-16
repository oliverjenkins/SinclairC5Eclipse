/* 
 * File:   xlcd.h
 * Author: Oliver
 

 * Copied and modified from xlcd.h.  However functions placed in-line as to avoid re-compilation issues
 * Header files reference functions that must be re-compiled with an unknown method
 * By putting all functions inline, and renaming constants I avoid this step
 * Its working around my ignorance to do it this way
 *
 * LCD connected to PORTA via 4-bit UPPER interface
 * LCD  Function            Pic Port.Pin
 * 1    GND
 * 2    VDD
 * 3    Contrast adjustment 10k pot, output reads 2.7v from 4.2v supply
 * 4    RS - Read Select    PortA.RA1		LCD_RS_PIN
 * 5    RW - Read/Write     PortA.RA2		LCD_RW_PIN
 * 6    E -  Enable         PortA.RA3		LCD_E_PIN
 * 7    GND
 * 8    GND
 * 9    GND
 * 10   GND
 * 11   Data                PortA.RA4
 * 12   Data                PortA.RA5
 * 13   Data                PortA.RA6
 * 14   Data                PortA.RA7

 * Created on 14 April 2015, 11:41
 */

 #define UPPER                          // Using upper four bits of porta

 #define LCD_DATA_PORT      		PORTA
 #define LCD_TRIS_DATA_PORT 		TRISA

/* CTRL_PORT defines the port where the control lines are connected.
 * These are just samples, change to match your application.
 */
 #define LCD_RW_PIN   LATAbits.LATA2   		/* PORT for RW */
 #define LCD_TRIS_RW  TRISAbits.TRISA2    	/* TRIS for RW */

 #define LCD_RS_PIN   LATAbits.LATA1   		/* PORT for RS */
 #define LCD_TRIS_RS  TRISAbits.TRISA1    	/* TRIS for RS */

 #define LCD_E_PIN    LATAbits.LATA3  		/* PORT for D  */
 #define LCD_TRIS_E   TRISAbits.TRISA3    	/* TRIS for E  */

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