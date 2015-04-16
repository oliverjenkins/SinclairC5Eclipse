/*
 * File:   analogueinput_main.c
 * Author: Oliver Jenkins
 *
 * Created on 14 April 2015, 07:43
 *
 * Configuration test to configure each of the analogue inputs in turn
 * Tested with a pot going to each pin to light LEDs on PortD based on ADC value

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

/** V A R I A B L E S *************************************************/
unsigned char ADC_Value;  // 8-bit variable


/** D E C L A R A T I O N S *******************************************/
// declare constant data in program memory starting at address 0x180

const unsigned char ADC_LED_Lookup[16] @ 0x180 = {
    0b00000000,
    0b00000001,
    0b00000010,
    0b00000011,
    0b00000101,
    0b00000111,
    0b00001011,
    0b00001111,
    0b00010111,
    0b00011111,
    0b00101111,
    0b00111111,
    0b01011111,
    0b01111111,
    0b10111111,
    0b11111111,
};

/** I N C L U D E S **************************************************/
#include <xc.h>
#include <delays.h>


unsigned char ADC_Convert(void);

/*  Config

    TRISAbits.TRISA0 = 1;       // TRISA0 input

    // ANSEL and ANSELH registers set if the pin is to be an analogue or digital inputs
    // each bit corresponding to a particular input.  Set the bit to 1 for analogue
    // REG      bit7    bit6    bit5    bit4    bit3    bit2    bit1    bit0
    // ANSEL    ANS7    ANS6    ANS5    ANS4    ANS3    ANS2    ANS1    ANS0
    // ANSELH   -       -       -       ANS12   ANS11   ANS10   ANS9    ANS8
    // All bits of the ANSELH register initialize to ?0? if the PBADEN bit of CONFIG3H is ?0?
    ANSEL = 0b00000010;
    ANSELH = 0b00000000;

    // Sets bits VCFG1 and VCFG0 in ADCON1 so the ADC voltage reference is VSS to VDD
    ADCON1 = 0b00000000;        // What voltage to compare against page 272
    ADCON2 = 0b00111000;        // How long should AD conversion take - 20 TAD and FOSC/2 page 273

    // Which channel (e.g. AN2) should be used (bits 5 - 2) and information (bit 1) and start the process (bit 0) - page 271
    /* bit 5-2 CHS<3:0>: Analog Channel Select bits
    0000 = AN0
    0001 = AN1
    0010 = AN2
    0011 = AN3
    0100 = AN4
    0101 = AN5
    0110 = AN6
    0111 = AN7
    1000 = AN8
    1001 = AN9
    1010 = AN10
    1011 = AN11
    1100 = AN12

    ADCON0 = 0b00000101;

 * I can use ADCON0 to change the channel, and then run a compare
 *
 *
 *
 * Pin# Name                        Name    Port    ANSEL       ANSELH      ADCON0
 * 19   RA0/AN0/C12IN0              AN0     A       0b-------1  0b--------  0b00000001
 * 20   RA1/AN1/C12IN1-             AN1     A       0b------1-  0b--------  0b00000101
 * 21   RA2/AN2/VREF-/CVREF/C2IN+   AN2     A       0b-----1--  0b--------  0b00001001
 * 22   RA3/AN3/VREF+/C1IN+         AN3     A       0b----1---  0b--------  0b00001101
 * 24   RA5/AN4/SS/HLVDIN/C2OUT     AN4     A       0b---1----  0b--------  0b00010001
 * 8    RB0/INT0/FLT0/AN12          AN12    B       0b--------  0b---1----  0b00101001
 * 9    RB1/INT1/AN10/C12IN3-       AN10    B       0b--------  0b-----1--  0b00101001
 * 10   RB2/INT2/AN8                AN8     B       0b--------  0b-------1  0b00100001
 * 11   RB3/AN9/C12IN2-/CCP2(1)     AN9     B       0b--------  0b------1-  0b00100101
 * 14   RB4/KBI0/AN11               AN11    B       0b--------  0b----1---  0b00101101
 * 25   RE0/RD/AN5                  AN5     E       0b--1-----  0b--------  0b00010101
 * 26   RE1/WR/AN6                  AN6     E       0b-1------  0b--------  0b00011001
 * 27   RE2/CS/AN7                  AN7     E       0b1-------  0b--------  0b00011101
 */

#define ANSEL_VAL       0b10000000
#define ANSELH_VAL      0b00000000
#define ADCON0_VAL      0b00011101

/** M A I N   F U N C T I O N ***************************************/
void main (void)
{
    TRISD = 0b00000000;         // PORTD bits 7:0 are all outputs (0)
        
    // Analogue configuration
    TRISAbits.TRISA0 = 1;       // TRISA0 input
    //
    
    // ANSEL and ANSELH registers set if the pin is to be an analogue or digital inputs
    // each bit corresponding to a particular input.  Set the bit to 1 for analogue
    // REG      bit7    bit6    bit5    bit4    bit3    bit2    bit1    bit0
    // ANSEL    ANS7    ANS6    ANS5    ANS4    ANS3    ANS2    ANS1    ANS0
    // ANSELH   -       -       -       ANS12   ANS11   ANS10   ANS9    ANS8
    // All bits of the ANSELH register initialize to ?0? if the PBADEN bit of CONFIG3H is ?0?
    ANSEL = ANSEL_VAL;
    ANSELH = ANSELH_VAL;
    
    // Sets bits VCFG1 and VCFG0 in ADCON1 so the ADC voltage reference is VSS to VDD
    ADCON1 = 0b00000000;        // What voltage to compare against page 272
    ADCON2 = 0b00111000;        // How long should AD conversion take - 20 TAD and FOSC/2 page 273

    // Which channel (e.g. AN2) should be used (bits 5 - 2) and information (bit 1) and start the process (bit 0) - page 271
    /* bit 5-2 CHS<3:0>: Analog Channel Select bits
    0000 = AN0
    0001 = AN1
    0010 = AN2
    0011 = AN3
    0100 = AN4
    0101 = AN5
    0110 = AN6
    0111 = AN7
    1000 = AN8
    1001 = AN9
    1010 = AN10
    1011 = AN11
    1100 = AN12
    */
    ADCON0 = ADCON0_VAL;


    while (1)
    {
        ADC_Value = ADC_Convert();      // MSB from ADC
        ADC_Value = ADC_Value >> 4;     // We have 0 - 255 from ADC, so divide by 16 to get a lookup range

        // use lookup table to output one LED on based on LED_Number value
        LATD = ADC_LED_Lookup[ADC_Value];

    }
}


unsigned char ADC_Convert(void)
{ // start an ADC conversion and return the 8 most-significant bits of the result
    ADCON0bits.GO_DONE = 1;             // start conversion
    while (ADCON0bits.GO_DONE == 1);    // wait for it to complete
    return ADRESH;                      // return high byte of result
}