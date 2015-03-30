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

/** V A R I A B L E S *************************************************/


/** D E C L A R A T I O N S *******************************************/


#pragma code
void main (void)
{

  
    // Configure the LEDS on PORTD
    TRISD = 0b00000000;// PORTD bit 7 to output (0); bits 6:0 are inputs (1)


     // Init Timer
    INTCONbits.TMR0IF = 0;      // clear roll-over interrupt flag
    T0CON = 0b00001011;         // 16Bit Timer, Prescale 1:4 or in line with operation cycle
    TMR0H = 0;                  // clear timer - always write upper byte first
    TMR0L = 0;


    while (1) {
        LATD = 0b11110000;

        // The clock runs at 1Mhz = 1 000 000 hertz
        // 1 / 1 000 000 = 0.000001 or 1uS to complete a clock cycle
        // Four clock cycles per operation = 0.000004
        // Delay1KTCYx will delay for n thousand of operations
        // 1 second = 250 * 1000 * 0.000004
        TMR0H = 0;                  // clear timer - always write upper byte first
        TMR0L = 0;
        T0CONbits.TMR0ON = 1;       // start timer
        Delay1KTCYx(250); // should be a second
        T0CONbits.TMR0ON = 0;       // start timer
        LATD = 0b00001111;


         T0CONbits.TMR0ON = 0;
         TMR0H = 0;                  // clear timer - always write upper byte first
         TMR0L = 0;
        Delay1KTCYx(250);

    }
}

