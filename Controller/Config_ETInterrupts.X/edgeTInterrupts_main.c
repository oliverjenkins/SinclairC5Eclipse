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
    TRISD = 0b00000000;         // PORTD bits 7:0 are all outputs (0)
    LATD = 0b10000000;            // initialize

    // Done in a verbose way to show clearly to show how each input is configured
    // Init I/O
    INTCON2bits.RBPU = 0;       // enable PORTB internal pullups
    WPUBbits.WPUB0 = 1;         // Pull up on RB0
    WPUBbits.WPUB1 = 1;         // Pull up on RB1
    WPUBbits.WPUB2 = 1;         // Pull up on RB2
    WPUBbits.WPUB4 = 1;         // Pull up on RB4
    WPUBbits.WPUB5 = 1;         // Pull up on RB5


    // Configure the PortB and pints
    // RBO
    TRISBbits.TRISB0 = 1;       // RB0 (connected to switch) is input (1)
    ANSELHbits.ANS12 = 0;       // RB0/AN12 needs to have its analogue input turned off
    //RB1
    TRISBbits.TRISB1 = 1;       // RB1 (connected to switch) is input (1)
    ANSELHbits.ANS10 = 0;       // RB1/AN10 needs to have its analogue input turned off
    // RB2
    TRISBbits.TRISB2 = 1;       // RB2 (connected to switch) is input (1)
    ANSELHbits.ANS8 = 0;        // RB2/AN8 needs to have its analogue input turned off
    // RB4
    TRISBbits.TRISB4 = 1;       // RB4 (connected to switch) is input (1)
    ANSELHbits.ANS11 = 0;       // RB4/AN11 needs to have its analogue input turned off
    // RB5
    TRISBbits.TRISB5 = 1;       // RB5 (connected to switch) is input (1)

    // Configure the inturrupts
    // RB0 - INT0 is ALWAYS a high priority interrupt
    INTCON2bits.INTEDG0 = 0;    // Falling edge of RB0 / INT0 (switch pressed)
    INTCONbits.INT0IF = 0;      // Clear INT0 flag
    INTCONbits.INT0IE = 1;      // Enable INT0 interrupt

    // RB1 / INT1
    INTCON2bits.INTEDG1 = 0;    // Falling edge of RB1 / INT1
    INTCON3bits.INT1IP = 0;     // Low priority for RB1 / INT1
    INTCON3bits.INT1IF = 0;      // Clear INT1 flag
    INTCON3bits.INT1IE = 1;      // Enable INT1 interrupts

    // RB2 / INT2
    INTCON2bits.INTEDG2 = 0;    // Falling edge of RB2 / INT2
    INTCON3bits.INT2IP = 1;     // High priority for RB2 / INT2
    INTCON3bits.INT2IF = 0;      // Clear INT2 flag
    INTCON3bits.INT2IE = 1;      // Enable INT2 interrupts

    // RB4 - RB5 port change - leaving RB6+RB7 as they are used for ICSP    
    INTCON2bits.RBIP = 0;       // Low priority  Port Change Interrupt Priority
    PortBValue = PORTB;         // Read port B first
    INTCONbits.RBIF = 0;        // Clear port change flag
    IOCBbits.IOCB4 = 1;         // Enable B4 with port change
    IOCBbits.IOCB5 = 1;         // Enable B5 with port change
    INTCONbits.RBIE = 1;        // Enable port change interrupts


    // Set up global interrupts
    RCONbits.IPEN = 1;          // Enable priority levels on interrupts
    INTCONbits.GIEL = 1;        // Low priority interrupts allowed
    INTCONbits.GIEH = 1;        // Interrupting enabled.

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
        LATDbits.LATD0 = ~LATDbits.LATD0; // toggle LATD;
    }
   
    if (INTCON3bits.INT2IF) {
        INTCON3bits.INT2IF = 0; // Clear RB2 / INT2 flag
        LATDbits.LATD2 = ~LATDbits.LATD2; // toggle LATD;
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

