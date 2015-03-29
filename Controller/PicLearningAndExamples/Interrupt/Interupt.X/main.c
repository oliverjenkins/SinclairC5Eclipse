/** C O N F I G U R AT I O N B I T S ******************************/

#pragma config FOSC = INTIO67, FCMEN = OFF, IESO = OFF                  // CONFIG1H
#pragma config PWRT = OFF, BOREN = SBORDIS, BORV = 30                   // CONFIG2L
#pragma config WDTEN = OFF, WDTPS = 32768                               // CONFIG2H
#pragma config LPT1OSC = OFF, PBADEN = ON, CCP2MX = PORTC   // CONFIG3H
#pragma config STVREN = ON, LVP = OFF, XINST = OFF                      // CONFIG4L
#pragma config CP0 = OFF, CP1 = OFF, CP2 = OFF, CP3 = OFF               // CONFIG5L
#pragma config CPB = OFF, CPD = OFF                                     // CONFIG5H
#pragma config WRT0 = OFF, WRT1 = OFF, WRT2 = OFF, WRT3 = OFF           // CONFIG6L
#pragma config WRTB = OFF, WRTC = OFF, WRTD = OFF                       // CONFIG6H
#pragma config EBTR0 = OFF, EBTR1 = OFF, EBTR2 = OFF, EBTR3 = OFF       // CONFIG7L
#pragma config EBTRB = OFF                                              // CONFIG7H


#ifdef __DEBUG
#warning MCLR is used as RESET not a GPIO input
#else
#pragma config MCLRE = OFF
#endif
/*
 FOSC = INTIO67
            This sets the PIC18F45K20 to run using the internal oscillator, so
            no crystal or external oscillator is needed. The default frequency is
            1 MHz. The oscillator is covered in more detail in Lesson 9. It also
            sets OSC1 and OSC2 pins to be used as the RA6 and RA7 I/O
            port pins as the OSC pin functions are not needed.
WDTEN = OFF
            This turns off the Watchdog Timer, as it is not used in this lesson.
            When the Watchdog Timer is enabled, it must be cleared periodically
            in the code or it will reset the microcontroller.
LVP = OFF
            This turns off Low-Voltage-Programming, and frees the PGM pin to
            be used as the RB5 I/O port pin. (LVP mode is not used by the
            PICkit 3 programmer.)

 */
/** I N C L U D E S **************************************************/
#include "p18f45K20.h"
#include "delays.h"

#define Switch          PORTBbits.RB0
#define DetectsInARow   5

/** V A R I A B L E S *************************************************/
 // declare statically allocated uninitialized variables
#pragma udata mysection = 0x300 
unsigned char LED_Display; // 8-bit variable
unsigned char LedDir = 0;


/** D E C L A R A T I O N S *******************************************/


#pragma code
void main (void)
{
    
    // Initial variable configue
    LED_Display = 1;

    // Configure the Switch on PortB.RA0
    INTCON2bits.RBPU = 0; // enable PORTB internal pullups
    WPUBbits.WPUB0 = 1; // enable pull up on RB0
    ANSELH = 0x00; // Turn a few pins to digital input rather than ADC
    TRISBbits.TRISB0 = 1; // PORTB bit 0 (connected to switch) is input (1)

    // Configure the LEDS on PORTD
    TRISD = 0b00000000;// PORTD bit 7 to output (0); bits 6:0 are inputs (1)

    // Set up switch interrupts on INT0
    INTCON2bits.INTEDG0 = 0;        // falling edge of
    INTCONbits.INT0IF = 0;      // ensure flag is cleared
    INTCONbits.INT0IE = 1;      // enable INT0 interrupt

    // Set-up Interrupts
    RCONbits.IPEN = 0;          // No priority on interupts - branches to 0008h
    INTCONbits.GIEL = 1;        // Low priority interrupts allowed
    INTCONbits.GIEH = 1;        // Interrupting enabled.

    while (1) {
        LATD = LED_Display;
        if (LedDir == 0) {
            LED_Display <<= 1;

            if (LED_Display == 0) {
                LED_Display = 1;
            }
        } else {
            LED_Display >>= 1;
            if (LED_Display == 0) {
                LED_Display = 128;
            }
        }
        Delay1KTCYx(50);//

    }
}


/** I N T E R  U P T S ******************************************/
//Need both function per prority level i.e. InterruptHandlerHigh and InterruptVectorHigh to work
#pragma interrupt InterruptHandlerHigh
void InterruptHandlerHigh () {
    // Check for INT0 interrupt
    if (INTCONbits.INT0IF)
    {
        // clear (reset) flag
        INTCONbits.INT0IF = 0;

        LedDir = ~ LedDir;
    }
    return;
}

#pragma code InterruptHandler = 0x08
void InterruptVectorHigh (void)
{
  _asm
    goto InterruptHandlerHigh //jump to interrupt routine
  _endasm
}
