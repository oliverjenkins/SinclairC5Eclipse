/* 
 * File:   main.c
 * Author: Oliver Jenkins
 *
 * Created on 14 April 2015, 07:43
 */

/** C O N F I G U R A T I O N   B I T S ******************************/

#pragma config FOSC = INTIO67, FCMEN = OFF, IESO = OFF                      // CONFIG1H
#pragma config PWRT = OFF, BOREN = SBORDIS, BORV = 30                       // CONFIG2L
#pragma config WDTEN = OFF, WDTPS = 32768                                   // CONFIG2H
#pragma config LPT1OSC = ON, PBADEN = ON, CCP2MX = PORTC                    // CONFIG3H
#pragma config STVREN = ON, LVP = OFF, XINST = OFF                          // CONFIG4L
#pragma config CP0 = OFF, CP1 = OFF, CP2 = OFF, CP3 = OFF                   // CONFIG5L
#pragma config CPB = OFF, CPD = OFF                                         // CONFIG5H
#pragma config WRT0 = OFF, WRT1 = OFF, WRT2 = OFF, WRT3 = OFF               // CONFIG6L
#pragma config WRTB = OFF, WRTC = OFF, WRTD = OFF                           // CONFIG6H
#pragma config EBTR0 = OFF, EBTR1 = OFF, EBTR2 = OFF, EBTR3 = OFF           // CONFIG7L
#pragma config EBTRB = OFF                                                  // CONFIG7H

#ifdef __DEBUG
#warning MCLR is used as RESET not a GPIO input
#else
#pragma config MCLRE = OFF
#endif



/** I N C L U D E S **************************************************/
#include <xc.h>
#include <delays.h>

/** Circuit Implimentation details */
/*
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
#define LINES_5X10  0b00110100  /* 5x10 characters               */
#define LINES_5X7  0b00111000  /* 5x7 characters, multiple line */


void InitLCD(void);
void DelayFor18TCY(void);
void DelayPORLCD(void);
void DelayLCD(void);

void OpenLCD(unsigned char lcdtype);
void WriteCmdLCD(unsigned char cmd);
void SetLCDCGRamAddr(unsigned char CGaddr);
void SetLCDDDRamAddr(unsigned char DDaddr);
unsigned char BusyLCD(void);
void putLCD(char *buffer);
void WriteDataLCD(char data);

void main (void)
{    
    TRISD = 0b00000000;
    LATD = 0;
    
    InitLCD();
    // Create first custom char
    SetLCDCGRamAddr(0x00);
    WriteDataLCD(0b00000);
    WriteDataLCD(0b01111);
    WriteDataLCD(0b10001);
    WriteDataLCD(0b10001);
    WriteDataLCD(0b10001);
    WriteDataLCD(0b01111);
    WriteDataLCD(0b00111);
    WriteDataLCD(0b00111);
    
    // Second custom char
    SetLCDCGRamAddr(0x08);
    WriteDataLCD(0b11111);
    WriteDataLCD(0b10001);
    WriteDataLCD(0b10001);
    WriteDataLCD(0b10101);
    WriteDataLCD(0b10001);
    WriteDataLCD(0b10001);
    WriteDataLCD(0b10001);
    WriteDataLCD(0b11111);

    WriteCmdLCD(0b00000010);        // Cursor to home
    putLCD("Hello World9");

    SetLCDDDRamAddr(0x040);         // Cursor to second line
    putLCD("LCD Down");
    WriteDataLCD(0);              // Write First Custom Char
    WriteDataLCD(1);              // Write Second Custom Char
 
    






    while (1)
    {
	// use lookup table to output one LED on based on LED_Number value
        LATDbits.LD0 = 1;
        Delay1KTCYx(50);

        LATDbits.LD1 = 1;
        Delay1KTCYx(50);

        LATDbits.LD2 = 1;
        Delay1KTCYx(50);

        LATDbits.LD3 = 1;
        Delay1KTCYx(50);

        LATDbits.LD4 = 1;
        Delay1KTCYx(50);

        LATDbits.LD5 = 1;
        Delay1KTCYx(50);

        LATDbits.LD6 = 1;
        Delay1KTCYx(50);

        LATDbits.LD7 = 1;
        Delay1KTCYx(50);


        LATD = 0;
        Delay1KTCYx(50);
    }
}

void InitLCD(void) {
    OpenLCD(FOUR_BIT & LINES_5X7);
    while(BusyLCD());
    WriteCmdLCD(0x06);
    WriteCmdLCD(0x0C);
}

void DelayFor18TCY(void) {
    Delay1TCYx(18);
}
void DelayPORLCD(void) {
    Delay1TCYx(30);
}
void DelayLCD(void) {
    Delay1TCYx(10);
}

/* LIBRARY FUNCTIONS */
void OpenLCD(unsigned char lcdtype)
{
        // The data bits must be either a 8-bit port or the upper or
        // lower 4-bits of a port. These pins are made into inputs

        LCD_TRIS_RW = 0;                    // All control signals made outputs
        LCD_TRIS_RS = 0;
        LCD_TRIS_E = 0;
        LCD_RW_PIN = 0;                     // R/W pin made low
        LCD_RS_PIN = 0;                     // Register select pin made low
        LCD_E_PIN = 0;                      // Clock pin made low

        // Delay for 15ms to allow for LCD Power on reset
        DelayPORLCD();
        //-------------------reset procedure through software----------------------
        WriteCmdLCD(0x30);
        Delay10KTCYx(0x05);

        WriteCmdLCD(0x30);
         Delay10KTCYx(0x01);

        WriteCmdLCD(0x32);
        while( BusyLCD() );
        //------------------------------------------------------------------------------------------


        // Set data interface width, # lines, font
        while(BusyLCD());              // Wait if LCD busy
        WriteCmdLCD(lcdtype);          // Function set cmd

        // Turn the display on then off
        while(BusyLCD());              // Wait if LCD busy
        WriteCmdLCD(DOFF&CURSOR_OFF&BLINK_OFF);        // Display OFF/Blink OFF
        while(BusyLCD());              // Wait if LCD busy
        WriteCmdLCD(DON&CURSOR_ON&BLINK_ON);           // Display ON/Blink ON

        // Clear display
        while(BusyLCD());              // Wait if LCD busy
        WriteCmdLCD(0x01);             // Clear display

        // Set entry mode inc, no shift
        while(BusyLCD());              // Wait if LCD busy
        WriteCmdLCD(SHIFT_CUR_LEFT);   // Entry Mode

        // Set DD Ram address to 0
        while(BusyLCD());              // Wait if LCD busy
        SetLCDDDRamAddr(0x80);                // Set Display data ram address to 0

        return;
}

void WriteCmdLCD(unsigned char cmd)
{
         // Lower nibble interface
        LCD_TRIS_DATA_PORT &= 0xf0;
        LCD_DATA_PORT &= 0xf0;
        LCD_DATA_PORT |= (cmd>>4)&0x0f;

        LCD_RW_PIN = 0;                     // Set control signals for command
        LCD_RS_PIN = 0;
        DelayFor18TCY();
        LCD_E_PIN = 1;                      // Clock command in
        DelayFor18TCY();
        LCD_E_PIN = 0;
                               // Lower nibble interface
        LCD_DATA_PORT &= 0xf0;
        LCD_DATA_PORT |= cmd&0x0f;

        DelayFor18TCY();
        LCD_E_PIN = 1;                      // Clock command in
        DelayFor18TCY();
        LCD_E_PIN = 0;

        LCD_TRIS_DATA_PORT |= 0x0f;

        return;
}

unsigned char BusyLCD(void)
{
        LCD_RW_PIN = 1;                     // Set the control bits for read
        LCD_RS_PIN = 0;
        DelayFor18TCY();
        LCD_E_PIN = 1;                      // Clock in the command
        DelayFor18TCY();
                           // Lower nibble interface
        if(LCD_DATA_PORT&0x08)
        {
                LCD_E_PIN = 0;              // Reset clock line
                DelayFor18TCY();
                LCD_E_PIN = 1;              // Clock out other nibble
                DelayFor18TCY();
                LCD_E_PIN = 0;
                LCD_RW_PIN = 0;             // Reset control line
                return 1;               // Return TRUE
        }
        else                            // Busy bit is low
        {
                LCD_E_PIN = 0;              // Reset clock line
                DelayFor18TCY();
                LCD_E_PIN = 1;              // Clock out other nibble
                DelayFor18TCY();
                LCD_E_PIN = 0;
                LCD_RW_PIN = 0;             // Reset control line
                return 0;               // Return FALSE
        }
}


void SetLCDDDRamAddr(unsigned char DDaddr)
{
        // Lower nibble interface
        LCD_TRIS_DATA_PORT &= 0xf0;                 // Make port output
        LCD_DATA_PORT &= 0xf0;                      // and write upper nibble
        LCD_DATA_PORT |= (((DDaddr | 0b10000000)>>4) & 0x0f);

        LCD_RW_PIN = 0;                             // Set control bits
        LCD_RS_PIN = 0;
        DelayFor18TCY();
        LCD_E_PIN = 1;                              // Clock the cmd and address in
        DelayFor18TCY();
        LCD_E_PIN = 0;
                                    // Lower nibble interface
        LCD_DATA_PORT &= 0xf0;                      // Write lower nibble
        LCD_DATA_PORT |= (DDaddr&0x0f);

        DelayFor18TCY();
        LCD_E_PIN = 1;                              // Clock the cmd and address in
        DelayFor18TCY();
        LCD_E_PIN = 0;
                                    // Lower nibble interface
        LCD_TRIS_DATA_PORT |= 0x0f;                 // Make port input

        return;
}

void SetLCDCGRamAddr(unsigned char CGaddr)
{

                                       // Lower nibble interface
        LCD_TRIS_DATA_PORT &= 0xf0;                 // Make nibble input
        LCD_DATA_PORT &= 0xf0;                      // and write upper nibble
        LCD_DATA_PORT |= (((CGaddr |0b01000000)>>4) & 0x0f);

        LCD_RW_PIN = 0;                             // Set control signals
        LCD_RS_PIN = 0;
        DelayFor18TCY();
        LCD_E_PIN = 1;                              // Clock cmd and address in
        DelayFor18TCY();
        LCD_E_PIN = 0;
                                       // Lower nibble interface
        LCD_DATA_PORT &= 0xf0;                      // Write lower nibble
        LCD_DATA_PORT |= (CGaddr&0x0f);

        DelayFor18TCY();
        LCD_E_PIN = 1;                              // Clock cmd and address in
        DelayFor18TCY();
        LCD_E_PIN = 0;
                                        // Lower nibble interface
        LCD_TRIS_DATA_PORT |= 0x0f;                 // Make inputs
        return;
}

void putLCD(char *buffer)
{
        while(*buffer)                  // Write data to LCD up to null
        {
                while(BusyLCD());      // Wait while LCD is busy
                WriteDataLCD(*buffer); // Write character to LCD
                buffer++;               // Increment buffer
        }
        return;
}

void WriteDataLCD(char data)
{
        // Lower nibble interface
        LCD_TRIS_DATA_PORT &= 0xf0;
        LCD_DATA_PORT &= 0xf0;
        LCD_DATA_PORT |= ((data>>4)&0x0f);

        LCD_RS_PIN = 1;                     // Set control bits
        LCD_RW_PIN = 0;
        DelayFor18TCY();
        LCD_E_PIN = 1;                      // Clock nibble into LCD
        DelayFor18TCY();
        LCD_E_PIN = 0;
                              // Lower nibble interface
        LCD_DATA_PORT &= 0xf0;
        LCD_DATA_PORT |= (data&0x0f);

        DelayFor18TCY();
        LCD_E_PIN = 1;                      // Clock nibble into LCD
        DelayFor18TCY();
        LCD_E_PIN = 0;
                             // Lower nibble interface
        LCD_TRIS_DATA_PORT |= 0x0f;

        return;
}