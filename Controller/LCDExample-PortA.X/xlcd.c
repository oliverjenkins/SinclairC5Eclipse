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
        // The data bits must be either a 8-bit port or the upper or
        // lower 4-bits of a port. These pins are made into inputs
#ifdef BIT8                             // 8-bit mode, use whole port
        LCD_DATA_PORT = 0;
        LCD_TRIS_DATA_PORT = 0x00;
#else                                   // 4-bit mode
#ifdef UPPER                            // Upper 4-bits of the port
        LCD_DATA_PORT &= 0x0f;
        LCD_TRIS_DATA_PORT &= 0x0F;
#else                                   // Lower 4-bits of the port
        LCD_DATA_PORT &= 0xf0;
        LCD_TRIS_DATA_PORT &= 0xF0;
#endif
#endif
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
    #ifdef BIT8                             // 8-bit interface
            LCD_TRIS_DATA_PORT = 0;             // Data port output
            LCD_DATA_PORT = cmd;                // Write command to data port
            LCD_RW_PIN = 0;                     // Set the control signals
            LCD_RS_PIN = 0;                     // for sending a command
            DelayFor18TCY();
            LCD_E_PIN = 1;                      // Clock the command in
            DelayFor18TCY();
            LCD_E_PIN = 0;
            DelayFor18TCY();
            LCD_TRIS_DATA_PORT = 0xff;          // Data port input
    #else                                   // 4-bit interface
    #ifdef UPPER                            // Upper nibble interface
            TRIS_DATA_PORT &= 0x0f;
            DATA_PORT &= 0x0f;
            DATA_PORT |= cmd&0xf0;
    #else                                   // Lower nibble interface
            LCD_TRIS_DATA_PORT &= 0xf0;
            LCD_DATA_PORT &= 0xf0;
            LCD_DATA_PORT |= (cmd>>4)&0x0f;
    #endif
            LCD_RW_PIN = 0;                     // Set control signals for command
            LCD_RS_PIN = 0;
            DelayFor18TCY();
            LCD_E_PIN = 1;                      // Clock command in
            DelayFor18TCY();
            LCD_E_PIN = 0;
    #ifdef UPPER                            // Upper nibble interface
            LCD_DATA_PORT &= 0x0f;
            LCD_DATA_PORT |= (cmd<<4)&0xf0;
    #else                                   // Lower nibble interface
            LCD_DATA_PORT &= 0xf0;
            LCD_DATA_PORT |= cmd&0x0f;
    #endif
            DelayFor18TCY();
            LCD_E_PIN = 1;                      // Clock command in
            DelayFor18TCY();
            LCD_E_PIN = 0;
    #ifdef UPPER                            // Make data nibble input
            LCD_TRIS_DATA_PORT |= 0xf0;
    #else
            LCD_TRIS_DATA_PORT |= 0x0f;
    #endif
    #endif

    return;
}

unsigned char BusyLCD(void)
{
        LCD_RW_PIN = 1;                     // Set the control bits for read
        LCD_RS_PIN = 0;
        DelayFor18TCY();
        LCD_E_PIN = 1;                      // Clock in the command
        DelayFor18TCY();
#ifdef BIT8                             // 8-bit interface
        if(LCD_DATA_PORT&0x80)                      // Read bit 7 (busy bit)
        {                               // If high
                LCD_E_PIN = 0;              // Reset clock line
                LCD_RW_PIN = 0;             // Reset control line
                return 1;               // Return TRUE
        }
        else                            // Bit 7 low
        {
                LCD_E_PIN = 0;              // Reset clock line
                LCD_RW_PIN = 0;             // Reset control line
                return 0;               // Return FALSE
        }
#else                                   // 4-bit interface
#ifdef UPPER                            // Upper nibble interface
        if(LCD_DATA_PORT&0x80)
#else                                   // Lower nibble interface
        if(LCD_DATA_PORT&0x08)
#endif
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
#endif
}


void SetLCDDDRamAddr(unsigned char DDaddr)
{
    #ifdef BIT8                                     // 8-bit interface
            LCD_TRIS_DATA_PORT = 0;                     // Make port output
            LCD_DATA_PORT = DDaddr | 0b10000000;        // Write cmd and address to port
            LCD_RW_PIN = 0;                             // Set the control bits
            LCD_RS_PIN = 0;
            DelayFor18TCY();
            LCD_E_PIN = 1;                              // Clock the cmd and address in
            DelayFor18TCY();
            LCD_E_PIN = 0;
            DelayFor18TCY();
            LCD_TRIS_DATA_PORT = 0xff;                  // Make port input
    #else                                           // 4-bit interface
    #ifdef UPPER                                    // Upper nibble  interface
            LCD_TRIS_DATA_PORT &= 0x0f;                 // Make port output
            LCD_DATA_PORT &= 0x0f;                      // and write upper nibble
            LCD_DATA_PORT |= ((DDaddr | 0b10000000) & 0xf0);
    #else                                           // Lower nibble interface
            LCD_TRIS_DATA_PORT &= 0xf0;                 // Make port output
            LCD_DATA_PORT &= 0xf0;                      // and write upper nibble
            LCD_DATA_PORT |= (((DDaddr | 0b10000000)>>4) & 0x0f);
    #endif
            LCD_RW_PIN = 0;                             // Set control bits
            LCD_RS_PIN = 0;
            DelayFor18TCY();
            LCD_E_PIN = 1;                              // Clock the cmd and address in
            DelayFor18TCY();
            LCD_E_PIN = 0;
    #ifdef UPPER                                    // Upper nibble interface
            LCD_DATA_PORT &= 0x0f;                      // Write lower nibble
            LCD_DATA_PORT |= ((DDaddr<<4)&0xf0);
    #else                                           // Lower nibble interface
            LCD_DATA_PORT &= 0xf0;                      // Write lower nibble
            LCD_DATA_PORT |= (DDaddr&0x0f);
    #endif
            DelayFor18TCY();
            LCD_E_PIN = 1;                              // Clock the cmd and address in
            DelayFor18TCY();
            LCD_E_PIN = 0;
    #ifdef UPPER                                    // Upper nibble interface
            LCD_TRIS_DATA_PORT |= 0xf0;                 // Make port input
    #else                                           // Lower nibble interface
            LCD_TRIS_DATA_PORT |= 0x0f;                 // Make port input
    #endif
    #endif
    return;
}

void SetLCDCGRamAddr(unsigned char CGaddr)
{
#ifdef BIT8                                     // 8-bit interface
        LCD_TRIS_DATA_PORT = 0;                     // Make data port ouput
        LCD_DATA_PORT = CGaddr | 0b01000000;        // Write cmd and address to port
        LCD_RW_PIN = 0;                             // Set control signals
        LCD_RS_PIN = 0;
        DelayFor18TCY();
        LCD_E_PIN = 1;                              // Clock cmd and address in
        DelayFor18TCY();
        LCD_E_PIN = 0;
        DelayFor18TCY();
        LCD_TRIS_DATA_PORT = 0xff;                  // Make data port inputs
#else                                           // 4-bit interface
#ifdef UPPER                                    // Upper nibble interface
        LCD_TRIS_DATA_PORT &= 0x0f;                 // Make nibble input
        LCD_DATA_PORT &= 0x0f;                      // and write upper nibble
        LCD_DATA_PORT |= ((CGaddr | 0b01000000) & 0xf0);
#else                                           // Lower nibble interface
        LCD_TRIS_DATA_PORT &= 0xf0;                 // Make nibble input
        LCD_DATA_PORT &= 0xf0;                      // and write upper nibble
        LCD_DATA_PORT |= (((CGaddr |0b01000000)>>4) & 0x0f);
#endif
        LCD_RW_PIN = 0;                             // Set control signals
        LCD_RS_PIN = 0;
        DelayFor18TCY();
        LCD_E_PIN = 1;                              // Clock cmd and address in
        DelayFor18TCY();
        LCD_E_PIN = 0;
#ifdef UPPER                                    // Upper nibble interface
        LCD_DATA_PORT &= 0x0f;                      // Write lower nibble
        LCD_DATA_PORT |= ((CGaddr<<4)&0xf0);
#else                                           // Lower nibble interface
        LCD_DATA_PORT &= 0xf0;                      // Write lower nibble
        LCD_DATA_PORT |= (CGaddr&0x0f);
#endif
        DelayFor18TCY();
        LCD_E_PIN = 1;                              // Clock cmd and address in
        DelayFor18TCY();
        LCD_E_PIN = 0;
#ifdef UPPER                                    // Upper nibble interface
        LCD_TRIS_DATA_PORT |= 0xf0;                 // Make inputs
#else                                           // Lower nibble interface
        LCD_TRIS_DATA_PORT |= 0x0f;                 // Make inputs
#endif
#endif
        return;
}

// Writes out a number as a series of three ASCII charactors
// utoa is a standard c lib function
void putIntLCD(short int n) {
    unsigned char result[4];
    utoa((char*)result, n, 10);
    putLCD(result);
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
    #ifdef BIT8                             // 8-bit interface
            LCD_TRIS_DATA_PORT = 0;             // Make port output
            LCD_DATA_PORT = data;               // Write data to port
            LCD_RS_PIN = 1;                     // Set control bits
            LCD_RW_PIN = 0;
            DelayFor18TCY();
            LCD_E_PIN = 1;                      // Clock data into LCD
            DelayFor18TCY();
            LCD_E_PIN = 0;
            LCD_RS_PIN = 0;                     // Reset control bits
            LCD_TRIS_DATA_PORT = 0xff;          // Make port input
    #else                                   // 4-bit interface
    #ifdef UPPER                            // Upper nibble interface
            LCD_TRIS_DATA_PORT &= 0x0f;
            LCD_DATA_PORT &= 0x0f;
            LCD_DATA_PORT |= data&0xf0;
    #else                                   // Lower nibble interface
            LCD_TRIS_DATA_PORT &= 0xf0;
            LCD_DATA_PORT &= 0xf0;
            LCD_DATA_PORT |= ((data>>4)&0x0f);
    #endif
            LCD_RS_PIN = 1;                     // Set control bits
            LCD_RW_PIN = 0;
            DelayFor18TCY();
            LCD_E_PIN = 1;                      // Clock nibble into LCD
            DelayFor18TCY();
            LCD_E_PIN = 0;
    #ifdef UPPER                            // Upper nibble interface
            LCD_DATA_PORT &= 0x0f;
            LCD_DATA_PORT |= ((data<<4)&0xf0);
    #else                                   // Lower nibble interface
            LCD_DATA_PORT &= 0xf0;
            LCD_DATA_PORT |= (data&0x0f);
    #endif
            DelayFor18TCY();
            LCD_E_PIN = 1;                      // Clock nibble into LCD
            DelayFor18TCY();
            LCD_E_PIN = 0;
    #ifdef UPPER                            // Upper nibble interface
            LCD_TRIS_DATA_PORT |= 0xf0;
    #else                                   // Lower nibble interface
            LCD_TRIS_DATA_PORT |= 0x0f;
    #endif
    #endif
    return;
}
