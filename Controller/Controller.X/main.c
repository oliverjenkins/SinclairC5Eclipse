
/** C O N F I G U R A T I O N   B I T S ******************************/

#pragma config FOSC = INTIO67, FCMEN = OFF, IESO = OFF                      // CONFIG1H
#pragma config PWRT = OFF, BOREN = SBORDIS, BORV = 30                       // CONFIG2L
#pragma config WDTEN = OFF, WDTPS = 32768                                   // CONFIG2H
#pragma config LPT1OSC = ON, PBADEN = ON, CCP2MX = PORTC                   // CONFIG3H
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
#include "xlcd.h"

/** P R O T O T Y P E S ******************************************************/
void ADC_Init(void);
void PWM_Init(void);
unsigned char ADC_Convert(void);

/** D E C L A R A T I O N S *******************************************/
// declare constant data in program memory starting at address 0x180

const unsigned char ADC_ThrottlePWM_Lookup[16] @ 0x180 = {
    0,
    18,
    36,
    54,
    72,
    90,
    90,
    108,
    126,
    144,
    162,
    180,
    198,
    216,
    234,
    255,
};

/** V A R I A B L E S *************************************************/
unsigned char ADC_Value;  // 8-bit variable
unsigned char Counter;

/** M A I N   F U N C T I O N ***************************************/
void main (void)
{

     // Init I/O
    TRISD = 0b00000000;         // PORTD bits 7:0 are all outputs (0)
    TRISAbits.TRISA0 = 1;       // TRISA0 input
    LATD = 0;                   // LEDs off

    INTCON2bits.RBPU = 0;       // enable PORTB internal pullups
    WPUBbits.WPUB0 = 1;         // enable pull up on RB0

    // Init ADC
    ADC_Init();

    // Init PWM
    PWM_Init();

    // LCD
    InitLCD();
  
    WriteCmdLCD(0b00000010);        // Cursor to home
    putLCD("Testing ");

    SetLCDDDRamAddr(0x040);         // Cursor to second line
    putLCD("LCD:");


    
    LATD = 1;
    ADC_Value = 12;
    Counter = 12;
    while (1)
    {

        ADC_Value = ADC_Convert();      // MSB from ADC
        ADC_Value = ADC_Value >> 4;     // We have 0 - 255 from ADC, so divide by 16 to get a lookup range

        // use lookup table to output one LED on based on LED_Number value
        CCPR1L = ADC_ThrottlePWM_Lookup[ADC_Value];

        // Update LCD with various details
  
        while(BusyLCD());
        SetLCDDDRamAddr(0x045);         // Cursor to second line
        putLCD("12");
     

        Delay1KTCYx(30);	    // Delay 50 x 1000 = 50,000 cycles; 200ms @ 1MHz

    }
}

void ADC_Init(void)
{
    // First, we need to make sure the AN0 pin is enabled as an analog input
    // as the demo board potentiometer is connected to RA0/AN0
    // Don't forget that RB0/AN12 must be digital!
    ANSEL = 0;          //turn off all other analog inputs
    ANSELH = 0;
    ANSELbits.ANS0 = 1; // turn on RA0 analog

    // Sets bits VCFG1 and VCFG0 in ADCON1 so the ADC voltage reference is VSS to VDD

    ADCON1 = 0;

    // The ADC clock must as short as possible but still greater than the
    // minimum TAD time, datasheet parameter 130.  At the time this lesson was
    // written TAD minimum for the PIC18F45K20 is 1.4us.
    // At 1MHz clock, selecting ADCS = FOSC/2 = 500kHz.  One clock period
    // 1 / 500kHz = 2us, which greater than minimum required 1.4us.
    // So ADCON2 bits ADCS2-0 = 000
    //
    // The ACQT aquisition time should take into accound the internal aquisition
    // time TACQ of the ADC, datasheet paramter 130, and the settling time of
    // of the application circuit connected to the ADC pin.  Since the actual
    // settling time of the RC circuit with the demo board potentiometer is very
    // long but accuracy is not very important to this demo, we'll set ACQT2-0 to
    // 20TAD = 111
    //
    // ADFM = 0 so we can easily read the 8 Most Significant bits from the ADRESH
    // Special Function Register
    ADCON2 = 0b00111000;

    // Select channel 0 (AN0) to read the potentiometer voltage and turn on ADC
    ADCON0 = 0b00000001;

}

void PWM_Init(void) {
    // Use this to to calculate http://www.micro-examples.com/public/microex-navig/doc/097-pwm-calculator.html
    // Use OSCCON to set the PICs clock frequency - page 29 of Datasheet

    // Set up 8-bit Timer2 to generate the PWM period (frequency)
    PR2 = 249;         // Timer 2 Period Register = 250 counts
    T2CON = 0b00000111;// Prescale = 1:16, timer on, postscale not used with CCP module
    // Thus, the PWM frequency is:
    // 1MHz clock / 4 = 250kHz instruction rate.
    // (250kHz / 16 prescale) / 250) = 62.5Hz, a period of 16ms.
   
    CCP1CON = 0b01001100;   // TODO: This breaks  LCD for some reaseon

    //CCP1CON = 0b00000000;
    // P1Mx = 01 Full-Bridge output forward, so we get the PWM
    // signal on P1D to LED7.  Only Single Output (00) is needed,
    // but the P1A pin does not connect to a demo board LED
    // CCP1Mx = 1100, PWM mode with P1D active-high.
}

unsigned char ADC_Convert(void)
{ // start an ADC conversion and return the 8 most-significant bits of the result
    ADCON0bits.GO_DONE = 1;             // start conversion
    while (ADCON0bits.GO_DONE == 1);    // wait for it to complete
    return ADRESH;                      // return high byte of result
}