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
#include "ports.h"
#include "lcddisplay.h"

/** D E C L A R A T I O N S **************************************************/


/** P R O T O T Y P E S ******************************************************/
void interrupt XC8_HighISR_Handler(void);
unsigned short ConvertTimerOverflowToWheelRPM(void);
unsigned char ConvertTimerOverflowToMPH(void);

#ifdef COMPILER_XC8
#include <GenericTypeDefs.h>
#endif

/** V A R I A B L E S *************************************************/
unsigned char PortBValue;  // 8-bit variable

unsigned char Throttle;
unsigned short TimerOverflowCounter;
unsigned char TimerValueAtOverflow;
unsigned short TimerCounterAtOverflow;
unsigned char Speed;
unsigned short RPM;
unsigned char CanCalculateSpeed;


/** D E C L A R A T I O N S *******************************************/
#define ANSEL_VAL       0b00000001
#define ANSELH_VAL      0b00000000
#define ADCON0_VAL      0b00000001

void main (void)
{
    InitPorts();
    InitInterrupts();
    InitMotorPWM();
    InitLCD();
    InitAnalogueInputs();
    LCDInitialDisplay();
    
    CCPR1L = 16;

    // Testing config
    LATD = 0;
    LATC = 0;


    // Initialise and start Timer 0
  
    TMR0H = 0xFF;
    TMR0L = 0x83;
    T0CONbits.TMR0ON = 1;           // start timer
    
    CanCalculateSpeed = 0;
    while (1)
    { // we update the port pins in our "background" loop while the interrupts
      // handle the switch and timer.
       CCPR1L = CCPR1L + 1;      
       Throttle = ADC_Convert();      // MSB from ADC
       Throttle = Throttle >> 4;     // We have 0 - 255 from ADC, so divide by 16 to get a lookup range
       if (CanCalculateSpeed) {
           RPM = ConvertTimerOverflowToWheelRPM();
           Speed = ConvertTimerOverflowToMPH();
           CanCalculateSpeed = 0;
       }       
       
       // Update the display with key metrics
       LCDUpdate(Speed, RPM, Throttle, CCPR1L);

       /// Show loop update
      // LATDbits.LATD7 = ~LATDbits.LATD7; // toggle LATD;
       Delay1KTCYx(1);
       
    }
}


/** I N T E R R U P T S ***********************************************/
// -------------------- Iterrupt Service Routines --------------------------


void interrupt XC8_HighISR_Handler(void)
{
    // Check to see what caused the interrupt
    // (Necessary when more than 1 interrupt at a priority level)

    // Check for INT0 interrupt
    // RB0 / INT0 check
    if (INTCONbits.INT0IF )
    {
        // Wheel has turned
        // clear (reset) flag
        INTCONbits.INT0IF = 0;
       
        TimerCounterAtOverflow = TimerOverflowCounter;
        TimerOverflowCounter = 0;
        TMR0H = 0xFF;
        TMR0L = 0x83;
        CanCalculateSpeed = 1;
    }
    if  (INTCONbits.TMR0IF) // Check for Timer0 Interrupt
    {
        INTCONbits.TMR0IF = 0;          // clear (reset) flag
        TimerOverflowCounter = TimerOverflowCounter + 1;

        // Handle the stopped case
        if (TimerOverflowCounter > 3003 ) {
            TimerCounterAtOverflow = TimerOverflowCounter;
            CanCalculateSpeed = 1;
        }
        TMR0H = 0xFF;
        TMR0L = 0x83;
        //LATDbits.LATD7 = ~LATDbits.LATD7; // toggle LATD

    }

}  // return from high-priority interrupt

#ifdef COMPILER_C18
//----------------------------------------------------------------------------
// High priority interrupt vector

#pragma code C18_HighISR_Vector = 0x08
void C18_HighISR_Vector (void)
{
  _asm
    goto C18_HighISR_Handler //jump to interrupt routine
  _endasm
}
#pragma code
#endif


void interrupt low_priority XC8_LowISR_Handler(void)
{
    // Check to see what caused the interrupt
    // (Necessary when more than 1 interrupt at a priority level)
    if (INTCON3bits.INT1IF) {
        INTCON3bits.INT1IF = 0; // Clear RB1 / INT1 flag
        LATDbits.LATD1 = ~LATDbits.LATD1; // toggle LATD;
    }

    if (INTCONbits.RBIF) {
        PortBValue = PORTB;
        INTCONbits.RBIF = 0;
        if (PortBValue & 0b00010000) {  // Check for RB4
            LATDbits.LATD4 = ~LATDbits.LATD4; // toggle LATD;
        }
        if (PortBValue & 0b00100000) {// Check for RB5
            LATDbits.LATD5 = ~LATDbits.LATD5; // toggle LATD;
        }
    }
    // Check for another interrupt, examples:
    // if (PIR1bits.TMR1IF)     // Timer 1
    // if (PIR1bits.ADIF)       // ADC
}

#ifdef COMPILER_C18
//----------------------------------------------------------------------------
// Low priority interrupt vector

#pragma code C18_LowISR_Vector = 0x18
void C18_LowISR_Vector (void)
{
  _asm
    goto C18_LowISR_Handler //jump to interrupt routine
  _endasm
}
#pragma code
#endif


unsigned char ConvertTimerOverflowToMPH() {
    if (TimerCounterAtOverflow > 3003) {
        return 0;
    }else if (TimerCounterAtOverflow <= 95) {
        return 30;
    } else if (TimerCounterAtOverflow <=98) {
        return 29;
    } else if (TimerCounterAtOverflow <=102) {
        return 28;
    } else if (TimerCounterAtOverflow <=106) {
        return 27;
    } else if (TimerCounterAtOverflow <=110) {
        return 26;
    } else if (TimerCounterAtOverflow <=114) {
        return 25;
    } else if (TimerCounterAtOverflow <=119) {
        return 24;
    } else if (TimerCounterAtOverflow <=124) {
        return 23;
    } else if (TimerCounterAtOverflow <=130) {
        return 21;
    } else if (TimerCounterAtOverflow <=136) {
        return 21;
    } else if (TimerCounterAtOverflow <=143) {
        return 19;
    } else if (TimerCounterAtOverflow <=150) {
        return 19;
    } else if (TimerCounterAtOverflow <=159) {
        return 17;
    } else if (TimerCounterAtOverflow <=168) {
        return 17;
    } else if (TimerCounterAtOverflow <=178) {
        return 16;
    } else if (TimerCounterAtOverflow <=190) {
        return 15;
    } else if (TimerCounterAtOverflow <=204) {
        return 14;
    } else if (TimerCounterAtOverflow <=219) {
        return 13;
    } else if (TimerCounterAtOverflow <=238) {
        return 12;
    } else if (TimerCounterAtOverflow <=260) {
        return 11;
    } else if (TimerCounterAtOverflow <=286) {
        return 10;
    } else if (TimerCounterAtOverflow <=318) {
        return 9;
    } else if (TimerCounterAtOverflow <=358) {
        return 8;
    } else if (TimerCounterAtOverflow <=410) {
        return 7;
    } else if (TimerCounterAtOverflow <=479) {
        return 6;
    } else if (TimerCounterAtOverflow <=576) {
        return 5;
    } else if (TimerCounterAtOverflow <=722) {
        return 4;
    } else if (TimerCounterAtOverflow <=966) {
        return 3;
    } else if (TimerCounterAtOverflow <=1462) {
        return 2;
    } else if (TimerCounterAtOverflow <=3002) {
        return 1;
    } else {
        return 0;
    }
}

unsigned short ConvertTimerOverflowToWheelRPM() {
    if (TimerCounterAtOverflow > 3003) {
        return 0;
    } else if (TimerCounterAtOverflow <=93) {
        return 1300;
    } else if (TimerCounterAtOverflow <=96) {
        return 1250;
    } else if (TimerCounterAtOverflow <=100) {
        return 1200;
    } else if (TimerCounterAtOverflow <=104) {
        return 1150;
    } else if (TimerCounterAtOverflow <=109) {
        return 1100;
    } else if (TimerCounterAtOverflow <=114) {
        return 1050;
    } else if (TimerCounterAtOverflow <=120) {
        return 1000;
    } else if (TimerCounterAtOverflow <=126) {
        return 950;
    } else if (TimerCounterAtOverflow <=133) {
        return 900;
    } else if (TimerCounterAtOverflow <=141) {
        return 850;
    } else if (TimerCounterAtOverflow <=150) {
        return 800;
    } else if (TimerCounterAtOverflow <=160) {
        return 750;
    } else if (TimerCounterAtOverflow <=171) {
        return 660;
    } else if (TimerCounterAtOverflow <=185) {
        return 650;
    } else if (TimerCounterAtOverflow <=187) {
        return 640;
    } else if (TimerCounterAtOverflow <=190) {
        return 630;
    } else if (TimerCounterAtOverflow <=193) {
        return 620;
    } else if (TimerCounterAtOverflow <=197) {
        return 610;
    } else if (TimerCounterAtOverflow <=200) {
        return 600;
    } else if (TimerCounterAtOverflow <=204) {
        return 590;
    } else if (TimerCounterAtOverflow <=207) {
        return 580;
    } else if (TimerCounterAtOverflow <=211) {
        return 570;
    } else if (TimerCounterAtOverflow <=214) {
        return 560;
    } else if (TimerCounterAtOverflow <=218) {
        return 550;
    } else if (TimerCounterAtOverflow <=222) {
        return 540;
    } else if (TimerCounterAtOverflow <=227) {
        return 530;
    } else if (TimerCounterAtOverflow <=231) {
        return 520;
    } else if (TimerCounterAtOverflow <=235) {
        return 510;
    } else if (TimerCounterAtOverflow <=240) {
        return 500;
    } else if (TimerCounterAtOverflow <=245) {
        return 490;
    } else if (TimerCounterAtOverflow <=250) {
        return 480;
    } else if (TimerCounterAtOverflow <=256) {
        return 470;
    } else if (TimerCounterAtOverflow <=261) {
        return 460;
    } else if (TimerCounterAtOverflow <=267) {
        return 450;
    } else if (TimerCounterAtOverflow <=273) {
        return 440;
    } else if (TimerCounterAtOverflow <=280) {
        return 430;
    } else if (TimerCounterAtOverflow <=286) {
        return 420;
    } else if (TimerCounterAtOverflow <=293) {
        return 410;
    } else if (TimerCounterAtOverflow <=300) {
        return 400;
    } else if (TimerCounterAtOverflow <=308) {
        return 390;
    } else if (TimerCounterAtOverflow <=316) {
        return 380;
    } else if (TimerCounterAtOverflow <=325) {
        return 370;
    } else if (TimerCounterAtOverflow <=334) {
        return 360;
    } else if (TimerCounterAtOverflow <=343) {
        return 350;
    } else if (TimerCounterAtOverflow <=353) {
        return 340;
    } else if (TimerCounterAtOverflow <=364) {
        return 330;
    } else if (TimerCounterAtOverflow <=375) {
        return 320;
    } else if (TimerCounterAtOverflow <=388) {
        return 310;
    } else if (TimerCounterAtOverflow <=400) {
        return 300;
    } else if (TimerCounterAtOverflow <=414) {
        return 290;
    } else if (TimerCounterAtOverflow <=429) {
        return 280;
    } else if (TimerCounterAtOverflow <=445) {
        return 270;
    } else if (TimerCounterAtOverflow <=462) {
        return 260;
    } else if (TimerCounterAtOverflow <=480) {
        return 250;
    } else if (TimerCounterAtOverflow <=499) {
        return 240;
    } else if (TimerCounterAtOverflow <=522) {
        return 230;
    } else if (TimerCounterAtOverflow <=546) {
        return 220;
    } else if (TimerCounterAtOverflow <=572) {
        return 210;
    } else if (TimerCounterAtOverflow <=600) {
        return 200;
    } else if (TimerCounterAtOverflow <=632) {
        return 190;
    } else if (TimerCounterAtOverflow <=667) {
        return 180;
    } else if (TimerCounterAtOverflow <=706) {
        return 170;
    } else if (TimerCounterAtOverflow <=750) {
        return 160;
    } else if (TimerCounterAtOverflow <=800) {
        return 150;
    } else if (TimerCounterAtOverflow <=857) {
        return 140;
    } else if (TimerCounterAtOverflow <=923) {
        return 130;
    } else if (TimerCounterAtOverflow <=1000) {
        return 120;
    } else if (TimerCounterAtOverflow <=1091) {
        return 110;
    } else if (TimerCounterAtOverflow <=1200) {
        return 100;
    } else if (TimerCounterAtOverflow <=1334) {
        return 90;
    } else if (TimerCounterAtOverflow <=1500) {
        return 80;
    } else if (TimerCounterAtOverflow <=1715) {
        return 70;
    } else if (TimerCounterAtOverflow <=2001) {
        return 60;
    } else if (TimerCounterAtOverflow <=2402) {
        return 50;
    } else if (TimerCounterAtOverflow <=3003) {
        return 40;
    } else {
    	return 0;
    }
}