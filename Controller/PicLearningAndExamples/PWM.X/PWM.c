//******************************************************************************
//Software License Agreement
//
//The software supplied herewith by Microchip Technology
//Incorporated (the "Company") is intended and supplied to you, the
//Company�s customer, for use solely and exclusively on Microchip
//products. The software is owned by the Company and/or its supplier,
//and is protected under applicable copyright laws. All rights are
//reserved. Any use in violation of the foregoing restrictions may
//subject the user to criminal sanctions under applicable laws, as
//well as to civil liability for the breach of the terms and
//conditions of this license.
//
//THIS SOFTWARE IS PROVIDED IN AN "AS IS" CONDITION. NO WARRANTIES,
//WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING, BUT NOT LIMITED
//TO, IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
//PARTICULAR PURPOSE APPLY TO THIS SOFTWARE. THE COMPANY SHALL NOT,
//IN ANY CIRCUMSTANCES, BE LIABLE FOR SPECIAL, INCIDENTAL OR
//CONSEQUENTIAL DAMAGES, FOR ANY REASON WHATSOEVER.
// *******************************************************************
// PIC18F45K20 PICkit 3 Debug Express Lesson 12 - CCP PWM
//
// This lesson demonstrates using the ECCP1 module to create a PWM
// output signal tp LED 7.  The PWM signal is modulated, meaning the
// duty cycle is changed, to change the brightness of the LED.
//
// *******************************************************************
// *    See included documentation for Lesson instructions           *
// *******************************************************************

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

#include <delays.h>
#include "PWM.h"

/** V A R I A B L E S *************************************************/


/** D E C L A R A T I O N S *******************************************/

void main (void)
{
    // Set RD7/P1D pin output so P1D PWM output drives LED7
    TRISDbits.TRISD7 = 0;
    TRISDbits.TRISD0 = 0;
    TRISDbits.TRISD1 = 0;

    PWM_Init();

    while(1) {
        LATDbits.LATD0 = ~LATDbits.LATD0; // toggle LATD
        LATDbits.LATD1 = ~LATDbits.LATD1; // toggle LATD

        CCPR1L = 0;
        Delay1KTCYx(255);


        CCPR1L = 16;
        Delay1KTCYx(255);
        
        CCPR1L = 50;
        Delay1KTCYx(255);

        CCPR1L = 64;
        Delay1KTCYx(255);

        CCPR1L = 128;
        Delay1KTCYx(255);

        CCPR1L = 255;
        Delay1KTCYx(255);
    }

}


void PWM_Init(void) {

    // Set up 8-bit Timer2 to generate the PWM period (frequency)
    T2CON = 0b00000111;// Prescale = 1:16, timer on, postscale not used with CCP module
    PR2 = 249;         // Timer 2 Period Register = 250 counts
    // Thus, the PWM frequency is:
    // 1MHz clock / 4 = 250kHz instruction rate.
    // (250kHz / 16 prescale) / 250) = 62.5Hz, a period of 16ms.

    CCP1CON = 0b01001100;
    // P1Mx = 01 Full-Bridge output forward, so we get the PWM
    // signal on P1D to LED7.  Only Single Output (00) is needed,
    // but the P1A pin does not connect to a demo board LED
    // CCP1Mx = 1100, PWM mode with P1D active-high.
}