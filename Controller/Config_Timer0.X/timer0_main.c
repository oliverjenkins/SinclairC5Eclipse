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


/** M A I N   F U N C T I O N ***************************************/
void main (void)
{

    TRISD = 0;
    LATD = 0b00000000;  

    // Set up global interrupts
    RCONbits.IPEN = 1;          // Enable priority levels on interrupts
    INTCONbits.GIEL = 1;        // Low priority interrupts allowed
    INTCONbits.GIEH = 1;        // Interrupting enabled.

    // Set up Interrupts for timer
    INTCONbits.TMR0IF = 0;          // clear roll-over interrupt flag
    INTCON2bits.TMR0IP = 0;         // Timer0 is low priority interrupt
    INTCONbits.TMR0IE = 1;          // enable the Timer0 interrupt.

    // Set up timer itself
    T0CON = 0b00000000;   // 16-bit timer prescale 1:2 - about 1 second maximum delay.
    //T0CON = 0b01000111;     // 8-bit timer, 1:256 prescaler - about 0.25 seconds
     //   T0CON = 0b00001000;     //16-bit  1:1 Pres=caler

    TMR0H = 0x00;             // Clear upper byte first
    TMR0L = 0x00;             // Clear lower byte
    T0CONbits.TMR0ON = 1;           // start timer

    LATD = 0b00001111;
    while (1)
    {        
        
        LATDbits.LATD0 = ~LATDbits.LATD0; // toggle LATD
        Delay1KTCYx(50);

        
    }
}


/** I N T E R R U P T S ***********************************************/
// -------------------- Iterrupt Service Routines --------------------------


void interrupt XC8_HighISR_Handler(void)
{
    // Check to see what caused the interrupt
    // (Necessary when more than 1 interrupt at a priority level)

    // Check for INT0 interrupt
    if (INTCONbits.INT0IF)
    {
        // clear (reset) flag
        INTCONbits.INT0IF = 0;
     
    }

    // Check for another interrupt, examples:
    // if (PIR1bits.TMR1IF)     // Timer 1
    // if (PIR1bits.ADIF)       // ADC

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

    // Check for Timer0 Interrupt
    if  (INTCONbits.TMR0IF)
    {
        INTCONbits.TMR0IF = 0;          // clear (reset) flag
        TMR0H = 0x00;                      // clear timer - always write upper byte first
        TMR0L = 0x00;
        LATDbits.LATD7 = ~LATDbits.LATD7; // toggle LATD
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

