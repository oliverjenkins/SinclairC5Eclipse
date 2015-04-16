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


#define OUTPUT_PORT_PIN     LATDbits.LD2
#define OUTPUT_PORT_TRIS    TRISDbits.RD2

/*  Config

 CCP is Capture/Compare/PWM.  There are two CCP1 and CCP2.  Each one can independantly
 * measure a waveform frequency, or provide a PWM output.  CCP1 is an enhanced module
 * that allows a PWM waveform to be outputed to several pins at once.  Since the C5
 * motor can only drive forwards this is not  - DONT USE ENHANCED CCP (ECCP)
 * CCP1 - Enhanced module - can multiplex PWM to other pins
 * CCP2 - Simple module

    // Init PWM
    TRISCbits.RC2 = 0;      // Set RC2/CCP1 as output for PWM1
    TRISCbits.RC1 = 0;      // Set RC1/CCP2 as output for PWM2 of

    // Configure the operation of CCP - PWM rather than Capture or Compare
    CCP1CON = 0b00001100;   // Simple Mode for PW1, with PWM, we're not sending output to RD5/PSP5/P1B etc
    CCP2CON = 0b00001100;   // PWM Mode for

    // Set the frequency of the PWM signal
    PR2 = 249;   // PWM period = (PR2+1) * prescaler * Tcy = 1ms

    // Enable Timer2 module for PWM to run
    T2CON = 0b00000100;     // Enable TMR2 with prescaler = 1

    // Duty Cycle - how much is it 'on'
    CCPR1L = 25; // pulse width = CCPR1L * prescaler * Tcy = 100us
    CCPR2L = 50;
 
 * Pin# Name                        Name    Port    N
 
 * 35   RC1/T1OSI/CCP2              CCP2     C       1
 * 11   RB3/AN9/C12IN2-/CCP2(1)     CCP2     B       3

 * 36   RC2/CCP1/P1A                CCP1     C       2
 
 
 * (1) RB3 can be configured as the alternate peripheral pin
     for the CCP2 module by clearing the CCP2MX Configuration
     bit of CONFIG3H

 * Supply voltage of 4.28v
 * CCPR2L           V on CCP2
 * 0                0
 * 32               0.23v
 * 64               0.46v
 * 128              0.92v
 * 160              1.14v
 * 192              1.37v
 * 224              1.60v
 * 255              1.82v

 
 * FOSC         Frequency of oscillator         = 1MHz
 * TOSC         Instruction cycle FOSC / 4
 * PWM Period = (PR2 + 1) * 4 * TOSC * T2CON.TMR2 Prescale Value
 
 *  */

/** M A I N   F U N C T I O N ***************************************/
void main (void)
{
 
    // Init I/O
    TRISD = 0b00000000;         // PORTD bits 7:0 are all outputs (0)
    LATD = 0;

    // Init PWM
    TRISCbits.RC2 = 0;      // Set RC2/CCP1 as output for PWM1
    TRISCbits.RC1 = 0;      // Set RC1/CCP2 as output for PWM2 of

    // Configure the operation of CCP - PWM rather than Capture or Compare 
    CCP1CON = 0b00001100;   // Simple Mode for PW1, with PWM, we're not sending output to RD5/PSP5/P1B etc  
    CCP2CON = 0b00001100;   // PWM Mode for
    
    // Set the frequency of the PWM signal
    PR2 = 255;   // PWM period = (PR2+1) * prescaler * Tcy = 1ms

    // Enable Timer2 module for PWM to run
    T2CON = 0b00001100;     // Enable TMR2 with prescaler = 1

    // Duty Cycle - how much is it 'on'
    CCPR1L = 25; // pulse width = CCPR1L * prescaler * Tcy = 100us   
    CCPR2L = 255;       // 100% duty cycle

    while (1)
    {

        OUTPUT_PORT_PIN = 1;

        CCPR1L = 10;
        //CCPR2L = 10;
        Delay1KTCYx(150);

        CCPR1L = 60;
        //CCPR2L = 60;
        Delay1KTCYx(150);

        CCPR1L = 120;
        //CCPR2L = 120;
        Delay1KTCYx(150);

        OUTPUT_PORT_PIN = 0;
        
        CCPR1L = 180;
        //CCPR2L = 180;
        Delay1KTCYx(150);

        CCPR1L = 255;
        //CCPR2L = 255;
        Delay1KTCYx(150);
        
        
    }
}
