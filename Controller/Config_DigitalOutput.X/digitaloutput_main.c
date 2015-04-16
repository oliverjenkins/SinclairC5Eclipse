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


#define OUTPUT_PORT_PIN     LATEbits.LE2
#define OUTPUT_PORT_TRIS    TRISEbits.RE2

/*  Config
 
* LAT[Port]bits.L[Port][N]          e.g. LATAbits.LA0
* TRIS[Port]bits.R[Port][N]         e.g. TRISAbits.RA0
 *
 * Pin# Name                        Name    Port    N
 * 19   RA0/AN0/C12IN0              RA0     A       0
 * 20   RA1/AN1/C12IN1-             RA1     A       1
 * 21   RA2/AN2/VREF-/CVREF/C2IN+   RA2     A       2
 * 22   RA3/AN3/VREF+/C1IN+         RA3     A       3
 * 23   RA4/T0CKI/C1OUT             RA4     A       4
 * 24   RA5/AN4/SS/HLVDIN/C2OUT     RA5     A       5
 * 8    RB0/INT0/FLT0/AN12          RB0     B       0
 * 9    RB1/INT1/AN10/C12IN3-       RB1     B       1
 * 10   RB2/INT2/AN8                RB2     B       2
 * 11   RB3/AN9/C12IN2-/CCP2(1)     RB3     B       3
 * 14   RB4/KBI0/AN11               RB4     B       4
 * 15   RB5/KBI1/PGM                RB5     B       5
 * 16   RB6/KBI2/PGC                RB6     B       6
 * 17   RB7/KBI3/PGD                RB7     B       7
 * 32   RC0/T1OSO/T13CKI            RC0     C       0
 * 35   RC1/T1OSI/CCP2              RC1     C       1
 * 36   RC2/CCP1/P1A                RC2     C       2
 * 37   RC3/SCK/SCL                 RC3     C       3
 * 42   RC4/SDI/SDA                 RC4     C       4
 * 43   RC5/SDO                     RC5     C       5
 * 44   RC6/TX/CK                   RC6     C       6
 * 1    RC7/RX/DT                   RC7     C       7
 * 38   RD0/PSP0                    RD0     D       0
 * 39   RD1/PSP1                    RD1     D       1
 * 40   RD2/PSP2                    RD2     D       2
 * 41   RD3/PSP3                    RD3     D       3
 * 2    RD4/PSP4                    RD4     D       4
 * 3    RD5/PSP5/P1B                RD5     D       5
 * 4    RD6/PSP6/P1C                RD6     D       6
 * 5    RD7/PSP7/P1D                RD7     D       7
 * 25   RE0/RD/AN5                  RE0     E       0
 * 26   RE1/WR/AN6                  RE1     E       1
 * 27   RE2/CS/AN7                  RE2     E       2
 */

/** M A I N   F U N C T I O N ***************************************/
void main (void)
{

    OUTPUT_PORT_TRIS = 0;

    while (1)
    {
        OUTPUT_PORT_PIN = 1;
        Delay1KTCYx(50);


        OUTPUT_PORT_PIN = 0;
        Delay1KTCYx(50);
    }
}
