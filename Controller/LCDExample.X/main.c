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
#include "xlcd.h"


/** M A I N   F U N C T I O N ***************************************/
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
    putLCD("Testing Message");

    SetLCDDDRamAddr(0x040);         // Cursor to second line
    putLCD("LCD Line 2 ");
    WriteDataLCD(0);              // Write First Custom Char
    WriteDataLCD(1);              // Write Second Custom Char
    WriteDataLCD(2);              // Write Third Custom Char
    WriteDataLCD(3);              // Write Forth Custom Char


    while (1)
    {
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
