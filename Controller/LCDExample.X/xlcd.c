#include <xc.h>
#include "xlcd.h"
#include <stdlib.h>


void InitLCD(void) {
    OpenLCD(FOUR_BIT & LINES_5X7);
    while(BusyLCD());
    WriteCmdLCD(0x06);
    WriteCmdLCD(0x0C);
}

void DelayFor18TCY(void) {
    // Delay of 18 Cycles
    Delay1TCYx(18);
}

/* LIBRARY FUNCTIONS */
void OpenLCD(unsigned char lcdtype)
{

        // Data must be in Lower 4-bits of a port.
        LCD_TRIS_RW = 0;                    // All control signals made outputs
        LCD_TRIS_RS = 0;
        LCD_TRIS_E = 0;
        LCD_RW_PIN = 0;                     // R/W pin made low
        LCD_RS_PIN = 0;                     // Register select pin made low
        LCD_E_PIN = 0;                      // Clock pin made low

        // Delay for 15ms to allow for LCD Power on reset
        // Cycles = (TimeDelay * Fosc) / 4
        // Cycles = (15ms * 1MHz) / 4
        // Cycles = (0.015 *1 000 000) /4
        // Cycles = 3750
        Delay100TCYx(38);  // 3800 clock cycles

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

// Writes out a number as a series of three ASCII charactors
// utoa is a standard c lib function
void putIntLCD(short int n) {
    unsigned char result[4];
    utoa((char*)result, n, 10);
    putLCD(result);
    //
//    unsigned char d3, d2, d1, d0, q;
//
//    if (n < 0) {
//        WriteDataLCD( '-' );
//        n = -n;
//    }
//
//    d1 = (n>>4)  & 0xF;
//    d2 = (n>>8)  & 0xF;
//    d3 = (n>>12) & 0xF;
//
//    d0 = 6*(d3 + d2 + d1) + (n & 0xF);
//    q = (d0 * 0xCD) >> 11;
//    d0 = d0 - 10*q;
//
//    d1 = q + 9*d3 + 5*d2 + d1;
//    q = (d1 * 0xCD) >> 11;
//    d1 = d1 - 10*q;
//
//    d2 = q + 2*d2;
//    q = (d2 * 0x1A) >> 8;
//    d2 = d2 - 10*q;
//
//    d3 = q + 4*d3;
////    d4 = (d3 * 0x1A) >> 8;
////    d3 = d3 - 10*d4;
//
//    WriteDataLCD( d3 + '0' );
//    WriteDataLCD( d2 + '0' );
//    WriteDataLCD( d1 + '0' );
//    WriteDataLCD( d0 + '0' );
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
