#include <xc.h>
#include <delays.h>
#include <stdlib.h>
#include "lcddisplay.h"
#include <stdio.h>
#include <string.h>


// Various Layouts and display options
void LCDInitialDisplay(void) {
//
//    // Create first custom char
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
    putLCD( (unsigned char *)" 00mph   0000rpm");

    //putIntLCD(3455);

    SetLCDDDRamAddr(0x040);         // Cursor to second line
    putLCD( (unsigned char *)"000pwr   000 pwm");

    //WriteDataLCD(0);              // Write First Custom Char
    //WriteDataLCD(1);              // Write Second Custom Char
}

void LCDUpdate(unsigned char Speed, unsigned short RPM, unsigned char Throttle, unsigned char PWM) {
    WriteCmdLCD(0x02);  // Cursor to home

    // Speed
    SetLCDDDRamAddr(0x01);  // Cursor to first Char
    putIntLCD(Speed,2);

    // RPM
    SetLCDDDRamAddr(0x09);  // Cursor to first Char
    putIntLCD(RPM,4);

    // Throttle
    SetLCDDDRamAddr(0x040);         // Cursor to second line
    putIntLCD(Throttle,3);

    // PWM
    SetLCDDDRamAddr(0x49);
    putIntLCD(PWM,3);
}


void InitLCD(void) {
    OpenLCD(FOUR_BIT & LINES_5X7);
    while(BusyLCD());
    WriteCmdLCD(0x06);
    while(BusyLCD());
    WriteCmdLCD(0x0C);
    while(BusyLCD());
    WriteCmdLCD(0x01);//Clear screen
    while(BusyLCD());
    WriteCmdLCD(66);    // Back light one
}

void DelayFor18TCY(void) {
    // Delay of 18 Cycles
    Delay1TCYx(18);
}

/* LIBRARY FUNCTIONS */
void OpenLCD(unsigned char lcdtype)
{

        Delay100TCYx(38);  // 3800 clock cycles
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
        Delay100TCYx(38);  // 3800 clock cycles
 //-------------------reset procedure through software----------------------

        WriteCmdLCD(0x30);
        Delay10KTCYx(0x05);

        WriteCmdLCD(0x30);
        Delay10KTCYx(0x01);


        WriteCmdLCD(0x32);
        while( BusyXLCD() );
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
    // These are in HEX
    //1 = Clear screen.
    //2 = Send cursor to top-left position.
    //8 = Blank without clearing.
    //12 = Make cursor invisible/restore display if blanked.
    //13 = Turn on visible blinking cursor.
    //14 = Turn on visible underline cursor.
    //16 = Move cursor one character left.
    //20 = Move cursor one character right.
    //24 = Scroll display one character left (all lines).
    //26 = Poll Keypad
    //28 = Scroll display one character right (all lines).
    //35 = Place Large Digit
    //58 = Enter Buffer Return Status Mode
    //59 = Exit Buffer Return Status Mode
    //61 = Make Vertical Bar Graph
    //65 = Auto Transmit Keypresses on
    //66 = Backlight On
    //67 = Auto Line wrapping on.................(default).
    //68 = Auto Line wrapping off.
    //69 = Clear Key Buffer
    //70 = Backlight Off
    //71 = Go to position.
    //72 = Go to top left.
    //74 = Cursor on.
    //75 = Cursor off.................................(default).
    //76 = Cursor left.
    //77 = Cursor right.
    //78 = Create Custom Character
    //79 = Auto Transmit Keypresses Off
    //80 = Contrast
    //81 = Auto scroll on.
    //82 = Auto scroll off..........................(default).
    //83 = Blink on.
    //84 = Blink off (default).
    //85 = Set Debounce Time
    //86 = General purpose output on.
    //87 = General purpose output off........(default).
    //88 = Clear display.
    //96 = Auto Repeat Mode Off
    //104 = Initialize Horizontal Bar Graph
    //110 = Initialize Large Digits
    //115 = Initialize Thin Bar Vertical Graph
    //118 = Initialize Thick Vertical Bar Graph
    //124 = Make Horizontal Bar Graph
    //126 = Auto Repeat Mode On
    //192 = Move cursor to first position on second line.
    #ifdef BIT8                             // 8-bit interface
        LCD_TRIS_DATA_PORT = 0;             // Data port output
        LCD_DATA_PORT = cmd;                // Write command to data port
        LCD_RW_PIN = 0;                     // Set the control signals
        LCD_RS_PIN = 0;                     // for sending a command
        LCD_DelayFor18TCY();
        LCD_E_PIN = 1;                      // Clock the command in
        LCD_DelayFor18TCY();
        LCD_E_PIN = 0;
        LCD_DelayFor18TCY();
        LCD_TRIS_DATA_PORT = 0xff;          // Data port input
    #else                                   // 4-bit interface
        #ifdef UPPER                            // Upper nibble interface
                LCD_TRIS_DATA_PORT &= 0x0f;
                LCD_DATA_PORT &= 0x0f;
                LCD_DATA_PORT |= cmd&0xf0;
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
void putIntLCD(short int n,short int pad) {
    
    if (pad == 2) {
        unsigned char result[2];
        sprintf(result, "%02d", n);
        putLCD(result);
    } else if (pad == 3) {
        unsigned char result[3];
        sprintf(result, "%03d", n);
        putLCD(result);
    } else if (pad == 4) {
        unsigned char result[4];
        sprintf(result, "%04d", n);
        putLCD(result);
    } else {
        unsigned char result[3];
        sprintf(result, "%d", n);
        putLCD(result);
    }
    
    //result = sprintf("%3d", n);
    //utoa((char*)result, n, 10);
   // Throttle = printf("%3d", Throttle);
    

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
        TRIS_DATA_PORT = 0xff;          // Make port input
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