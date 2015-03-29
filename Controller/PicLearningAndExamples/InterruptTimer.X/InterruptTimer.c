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
#include "compilertype.h"
#include "InterruptTimer.h"
#include <delays.h>



#ifdef COMPILER_XC8
#include <GenericTypeDefs.h>
#endif

#define Switch_Pin      PORTBbits.RB0

#pragma interruptlow InterruptServiceLow
void InterruptServiceLow(void)
{
    if(INTCON3bits.INT1IF){
      LATD = 0b101010101;
    }
    LATD = 0b101010101;
    INTCON3bits.INT1IF=0;   //Clear INT1 flag
}

void main (void)
{
    TRISD = 0b00000000;     	// LEDS - PORTD bits 7:0 are all outputs (0)

    INTCON2bits.RBPU = 0;       // enable PORTB internal pullups
    WPUBbits.WPUB0 = 1;         // enable pull up on RB0
    ANSELH = 0x00;              // Set RB<4:0> as digital I/O


    // Set up switch interrupt on INT0
    INTCON2bits.INTEDG0 = 0;    // interrupt on falling edge of INT0 (switch pressed)
    INTCONbits.INT0IF = 0;      // ensure flag is cleared
    INTCONbits.INT0IE = 1;      // enable INT0 interrupt

    // Set up global interrupts
    RCONbits.IPEN = 1;          // Enable priority levels on interrupts
    INTCONbits.GIEL = 1;        // Low priority interrupts allowed
    INTCONbits.GIEH = 1;        // Interrupting enabled.




    LATD = 0b010101010;
    
    while (1)
    {
//        if (Switch_Pin != 1) {
//            LATD = 0b101010101;
//        } else {
//
//        }
        Delay1KTCYx(30);	    // Delay 50 x 1000 = 50,000 cycles; 200ms @ 1MHz
    }

}
