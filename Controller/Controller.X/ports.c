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

    // Set up global interrupts
    RCONbits.IPEN = 1;          // Enable priority levels on interrupts
    INTCONbits.GIEL = 1;        // Low priority interrupts allowed
    INTCONbits.GIEH = 1;        // Interrupting enabled.
}