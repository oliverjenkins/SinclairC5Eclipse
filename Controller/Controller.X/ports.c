#include <xc.h>
/*
 * 19   RA0/AN0/C12IN0              RA0     A       0   ADC - Throttle Input
 * 20   RA1/AN1/C12IN1-             RA1     A       1   ADC - Motor Speed (back emf)
 * 21   RA2/AN2/VREF-/CVREF/C2IN+   RA2     A       2   ADC - Battery Voltage
 * 22   RA3/AN3/VREF+/C1IN+         RA3     A       3   Battery Voltage Select [1]
 * 23   RA4/T0CKI/C1OUT             RA4     A       4   Battery Voltage Select [1]
 * 24   RA5/AN4/SS/HLVDIN/C2OUT     RA5     A       5   Battery Voltage Select [1]
 * 31   OSC2/CLKOUT/RA6             RA6     A       6   Battery Voltage Select [1]
 * 32   OSC1/CLKIN/RA7              RA7     A       7
 * 8    RB0/INT0/FLT0/AN12          RB0     B       0   Digital Input - Wheel RPM
 * 9    RB1/INT1/AN10/C12IN3-       RB1     B       1   Digital Input - Brakes On Button
 * 10   RB2/INT2/AN8                RB2     B       2   Digital Input - Lights On Button
 * 11   RB3/AN9/C12IN2-/CCP2        RB3     B       3
 * 14   RB4/KBI0/AN11               RB4     B       4   Digital Input - Left turn Button
 * 15   RB5/KBI1/PGM                RB5     B       5   Digital Input - Right turn Button
 * 16   RB6/KBI2/PGC                RB6     B       6   ICSP
 * 17   RB7/KBI3/PGD                RB7     B       7   ICSP
 * 32   RC0/T1OSO/T13CKI            RC0     C       0   Digital Input - LCD function button
 * 35   RC1/T1OSI/CCP2              RC1     C       1   Digital Input - Horn button
 * 36   RC2/CCP1/P1A                RC2     C       2   PWM - Motor driver Half Bridge
 * 37   RC3/SCK/SCL                 RC3     C       3   Digital Output - Brake LED
 * 42   RC4/SDI/SDA                 RC4     C       4   Digital Output - Headlights LED
 * 43   RC5/SDO                     RC5     C       5   Digital Output - Left indicator
 * 44   RC6/TX/CK                   RC6     C       6   Digital Output - Right Indicator
 * 1    RC7/RX/DT                   RC7     C       7   Digital Output - Horn
 * 38   RD0/PSP0                    RD0     D       0   LCD - Data 1
 * 39   RD1/PSP1                    RD1     D       1   LCD - Data 2
 * 40   RD2/PSP2                    RD2     D       2   LCD - Data 3
 * 41   RD3/PSP3                    RD3     D       3   LCD - Data 4
 * 2    RD4/PSP4                    RD4     D       4   Speed configuration / Operation Mode
 * 3    RD5/PSP5/P1B                RD5     D       5   PWM - Motor driver Half Bridge
 * 4    RD6/PSP6/P1C                RD6     D       6
 * 5    RD7/PSP7/P1D                RD7     D       7
 * 25   RE0/RD/AN5                  RE0     E       0   LCD - Read Select
 * 26   RE1/WR/AN6                  RE1     E       1   LCD - Read/Write
 * 27   RE2/CS/AN7                  RE2     E       2   LCD - Enable

 */


void InitPorts(void) {
    // Turn off all analogue functions, we will enable the ones needed later
    ANSEL = 0;
    ANSELH = 0;
    // Port A
    /*
        * 19   RA0/AN0/C12IN0              RA0     A       0   ADC - Throttle Input
        * 20   RA1/AN1/C12IN1-             RA1     A       1   ADC - Motor Speed (back emf)
        * 21   RA2/AN2/VREF-/CVREF/C2IN+   RA2     A       2   ADC - Battery Voltage
        * 22   RA3/AN3/VREF+/C1IN+         RA3     A       3   Battery Voltage Select [1]
        * 23   RA4/T0CKI/C1OUT             RA4     A       4   Battery Voltage Select [1]
        * 24   RA5/AN4/SS/HLVDIN/C2OUT     RA5     A       5   Battery Voltage Select [1]
        * 31   OSC2/CLKOUT/RA6             RA6     A       6   Battery Voltage Select [1]
        * 32   OSC1/CLKIN/RA7              RA7     A       7
    */
    TRISA = 0b11100000;
    ANSEL = 0b11100000;

    // Port B
    /*
     * 9    RB1/INT1/AN10/C12IN3-       RB1     B       1   Digital Input - Brakes On Button
     * 10   RB2/INT2/AN8                RB2     B       2   Digital Input - Lights On Button
     * 11   RB3/AN9/C12IN2-/CCP2        RB3     B       3
     * 14   RB4/KBI0/AN11               RB4     B       4   Digital Input - Left turn Button
     * 15   RB5/KBI1/PGM                RB5     B       5   Digital Input - Right turn Button
     * 16   RB6/KBI2/PGC                RB6     B       6   ICSP
     * 17   RB7/KBI3/PGD                RB7     B       7   ICSP
     */
    INTCON2bits.RBPU = 0;       // enable PORTB internal pullups
    WPUBbits.WPUB0 = 1;         // Pull up on RB0

    TRISB = 0b00110111; 


    // Port C
    TRISCbits.RC2 = 0;
    
    // Port D
    /*
     * 38   RD0/PSP0                    RD0     D       0   LCD - Data 1
     * 39   RD1/PSP1                    RD1     D       1   LCD - Data 2
     * 40   RD2/PSP2                    RD2     D       2   LCD - Data 3
     * 41   RD3/PSP3                    RD3     D       3   LCD - Data 4
     * 2    RD4/PSP4                    RD4     D       4   Speed configuration / Operation Mode
     * 3    RD5/PSP5/P1B                RD5     D       5   PWM - Motor driver Half Bridge
     * 4    RD6/PSP6/P1C                RD6     D       6
     * 5    RD7/PSP7/P1D                RD7     D       7
     */
    TRISD = 0b00010000;         // PORTD are all outputs (0)
}

void InitInterrupts(void) {
    // Configure the inturrupts
    // RB0 - INT0 is ALWAYS a high priority interrupt
    INTCON2bits.INTEDG0 = 0;    // Falling edge of RB0 / INT0 (switch pressed)
    INTCONbits.INT0IF = 0;      // Clear INT0 flag
    INTCONbits.INT0IE = 1;      // Enable INT0 interrupt
    
    
    // Timer 0 - used to determine RPM
    // Use this http://www.enmcu.com/software/timer0calculatorandcodegeneration
    INTCONbits.TMR0IF = 0;          // clear roll-over interrupt flag
    INTCON2bits.TMR0IP = 1;         // Timer0 is hgih priority interrupt
    INTCONbits.TMR0IE = 1;          // enable the Timer0 interrupt.

    T0CON = 0b000000000;   // 8-bit timer, 1:2 prescaler = 0.20020480000 per overflow
    T0CONbits.T08BIT = 0;
    T0CONbits.T0CS = 0;
    T0CONbits.PSA = 1;

    // Set up global interrupts
    RCONbits.IPEN = 1;          // Enable priority levels on interrupts
    INTCONbits.GIEL = 1;        // Low priority interrupts allowed
    INTCONbits.GIEH = 1;        // Interrupting enabled.
}

void InitMotorPWM(void) {
 // Configure the operation of CCP - PWM rather than Capture or Compare
    CCP1CON = 0b00001100;   // Simple Mode for PW1, with PWM, we're not sending output to RD5/PSP5/P1B etc

    // Set the frequency of the PWM signal
    PR2 = 255;   // PWM period = (PR2+1) * prescaler * Tcy = 1ms

    // Enable Timer2 module for PWM to run
    T2CON = 0b00000100;     // Enable TMR2 with prescaler = 1

    // Duty Cycle - how much is it 'on'
    CCPR1L = 0; // pulse width = CCPR1L * prescaler * Tcy = 100us    
}

void InitAnalogueInputs(void) {

// Analogue configuration
    TRISAbits.TRISA0 = 1;       // TRISA0 input
    //

    // ANSEL and ANSELH registers set if the pin is to be an analogue or digital inputs
    // each bit corresponding to a particular input.  Set the bit to 1 for analogue
    // REG      bit7    bit6    bit5    bit4    bit3    bit2    bit1    bit0
    // ANSEL    ANS7    ANS6    ANS5    ANS4    ANS3    ANS2    ANS1    ANS0
    // ANSELH   -       -       -       ANS12   ANS11   ANS10   ANS9    ANS8
    // All bits of the ANSELH register initialize to ?0? if the PBADEN bit of CONFIG3H is ?0?
    ANSEL = 0b00000001;

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
    ADCON0 = 0b00000001;

}

unsigned char ADC_Convert(void)
{ // start an ADC conversion and return the 8 most-significant bits of the result
    ADCON0bits.GO_DONE = 1;             // start conversion
    while (ADCON0bits.GO_DONE == 1);    // wait for it to complete
    return ADRESH;                      // return high byte of result
}