/*
 * File:   digitaloutput_main.c
 * Author: Oliver Jenkins
 *
 * Created on 14 April 2015, 07:43
 *
 * Configuration test to make each pin a digital output
 * Tested with a simple LED on the output of each pin (current sourcing) to flash  */

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
#include "ports.h"
#include "lcddisplay.h"

/** D E C L A R A T I O N S **************************************************/


/** P R O T O T Y P E S ******************************************************/
void interrupt XC8_HighISR_Handler(void);

#ifdef COMPILER_XC8
#include <GenericTypeDefs.h>
#endif

/** V A R I A B L E S *************************************************/
unsigned char PortBValue;  // 8-bit variable


/** D E C L A R A T I O N S *******************************************/

void main (void)
{
    InitPorts();
    InitInterrupts();

    InitLCD();

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
    putLCD( (unsigned char *)"Testing ");

    putIntLCD(3455);

    SetLCDDDRamAddr(0x040);         // Cursor to second line
    putLCD( (unsigned char *)"Line 000 ");

    WriteDataLCD(0);              // Write First Custom Char
    WriteDataLCD(1);              // Write Second Custom Char    

    LATD = 0;
  
    while (1)
    { // we update the port pins in our "background" loop while the interrupts
      // handle the switch and timer.

        LATDbits.LATD7 = ~LATDbits.LATD7; // toggle LATD;
        Delay1KTCYx(250);
    }
}


/** I N T E R R U P T S ***********************************************/
// -------------------- Iterrupt Service Routines --------------------------


void interrupt XC8_HighISR_Handler(void)
{
    // Check to see what caused the interrupt
    // (Necessary when more than 1 interrupt at a priority level)

    // Check for INT0 interrupt
    // RB0 / INT0 check
    if (INTCONbits.INT0IF )
    {
        // clear (reset) flag
        INTCONbits.INT0IF = 0;
        LATDbits.LATD6 = ~LATDbits.LATD6; // toggle LATD;
    }

}  // return from high-priority interrupt

#ifdef COMPILER_C18
//----------------------------------------------------------------------------
// High priority interrupt vector

#pragma code C18_HighISR_Vector = 0x08
void C18_HighISR_Vector (void)
{
  _asm
    goto C18_HighISR_Handler //jump to interrupt routine
  _endasm
}
#pragma code
#endif


void interrupt low_priority XC8_LowISR_Handler(void)
{
    // Check to see what caused the interrupt
    // (Necessary when more than 1 interrupt at a priority level)
    if (INTCON3bits.INT1IF) {
        INTCON3bits.INT1IF = 0; // Clear RB1 / INT1 flag
        LATDbits.LATD1 = ~LATDbits.LATD1; // toggle LATD;
    }

    if (INTCONbits.RBIF) {
        PortBValue = PORTB;
        INTCONbits.RBIF = 0;
        if (PortBValue & 0b00010000) {  // Check for RB4
            LATDbits.LATD4 = ~LATDbits.LATD4; // toggle LATD;
        }
        if (PortBValue & 0b00100000) {// Check for RB5
            LATDbits.LATD5 = ~LATDbits.LATD5; // toggle LATD;
        }
    }
    // Check for another interrupt, examples:
    // if (PIR1bits.TMR1IF)     // Timer 1
    // if (PIR1bits.ADIF)       // ADC
}

#ifdef COMPILER_C18
//----------------------------------------------------------------------------
// Low priority interrupt vector

#pragma code C18_LowISR_Vector = 0x18
void C18_LowISR_Vector (void)
{
  _asm
    goto C18_LowISR_Handler //jump to interrupt routine
  _endasm
}
#pragma code
#endif

