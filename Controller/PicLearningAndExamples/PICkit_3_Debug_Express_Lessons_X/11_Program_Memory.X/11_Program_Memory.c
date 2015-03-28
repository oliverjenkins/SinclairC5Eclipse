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
// PIC18F45K20 PICkit 3 Debug Express Lesson 11 - Program Memory
//
// This lesson demonstrates accessing data stored in Program Memory.
// Reading, erasing, and writing flash program memory from within
// the application program are covered.
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
#include "..\compilertype.h"
#include "11_Program_Memory.h"  // header file


/** V A R I A B L E S *************************************************/

/** D E C L A R A T I O N S *******************************************/
#ifdef COMPILER_C18
#define ROM rom

#pragma romdata mystrings = 0x100
near ROM struct {
    char hello_str[];
    char mchp_str[];
    char fill_60[];
    unsigned short pad_01[];
} mydata =
#endif

#ifdef COMPILER_XC8
#define ROM const
near ROM struct {
    char hello_str[];
    char mchp_str[];
    char fill_60[];
    unsigned short pad_01[];
} mydata @ 0x100 =
#endif
    {
      "Hello!",
      "Microchip",
      "012345678901234567890123456789012345678901234567890123456789",
      {
        0x0000,
        0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,
        0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,
        0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,
        0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,
        0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,
        0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,
        0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,
        0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,
        0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,
        0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,
        0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,
        0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,
        0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,
        0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,
        0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,
        0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,
        0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,
        0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,
        0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000
      }
    };

#ifdef COMPILER_C18
#pragma code
#endif

void main (void)
{
    near ROM char *rom_pointer;     // 16-bit pointer to program memory
    char singlechar = '?';
    char i = 0;
    unsigned char Alphabet[32];

    // read using a pointer to data
    rom_pointer = mydata.hello_str;        // = &hello_str[0]

    do
    {
        singlechar = *(rom_pointer + i++);
    } while (singlechar != 0);      // string is terminated with 0x00 value.

    // read at specific address
    singlechar = ProgMemRdAddress(0x107);  // returns 'M' from "Microchip".

    // Erase the 64 bytes starting at 0x100
    ProgMemErase64(0x100);

    // create a RAM buffer with the uppercase alphabet in ASCII plus the characters [\]^_`
    for (i = 0; i < 32; i++)
    {
        Alphabet[i] = 'A' + i;
    }
    // write the buffer into program memory
    ProgMemWr32(0x100, Alphabet);

    while(1)
    {
    };      // all done
}

unsigned char ProgMemRdAddress(unsigned int address)
{ // reads and returns the flash program memory byte value at the 16-bit address given
  // given in "address".

    near ROM unsigned char *ptr;

    ptr = (near ROM unsigned char *)address;

    return *ptr;
}

void ProgMemErase64(unsigned int address)
{ // the program memory of the PIC18F4520 is erased 64-bytes at a time, which must
  // aligned on a 64-byte address boundary
    near ROM unsigned char *ptr;

    // assigning the pointer sets the TBLPTRU:TBLPTRH:TBLPTRL SFR registers
    ptr = (near ROM unsigned char *)(address & 0xFFC0);// ensure erase starts on 64-byte boundary
    TABLAT = *ptr;   // force load of the table pointer
    
    // set up the erase.  Program execution will be suspended during the flash erase
    EECON1bits.EEPGD = 1;               // point to flash program memory
    EECON1bits.CFGS = 0;                // not configuration registers
    EECON1bits.FREE = 1;                // we're erasing
    EECON1bits.WREN = 1;                // enable write/erase operations

    // execute code sequence, which cannot be interrupted, then execute erase

    //INTCONbits.GIE = 0;   // Disable interrupts
    EECON2 = 0x55;          // Begin Write sequence
    EECON2 = 0xAA;
    EECON1bits.WR = 1;      // Set WR bit to begin 64-byte erase
    //INTCONbits.GIE = 1;   // re-enable interrupts

    EECON1bits.WREN = 0;                // disable write/erase operations
}

void ProgMemWr32(unsigned int address, unsigned char *buffer_ptr)
{ // program memory must be written 32 bytes at a time in the PIC18F4520, starting at a 
  // 32-byte address boundary.  It must also be erased first.
    char i;
    near ROM unsigned char *ptr;

    // assigning the pointer sets the TBLPTRU:TBLPTRH:TBLPTRL SFR registers
    ptr = (near ROM unsigned char *)(address & 0xFFE0);// ensure erase starts on 32-byte boundary

    for (i = 0; i < 32; i++)
    {
#ifdef COMPILER_C18
        *(ptr + i) = buffer_ptr[i];
#endif

#ifdef COMPILER_XC8
        TABLAT = *(ptr + i);   // force load of the table pointer
        TABLAT = buffer_ptr[i];
        asm("TBLWT\n");
#endif

    }

    // NOTE! When writing program memory flash, the TBLPTRU:TBLPTRH:TBLPTRL registers
    // must be pointing within the 32-byte block intended to be written.  For example,
    // if you wish to write 32 bytes at 0x100, if the last write increments the pointer
    // past 0x11F to 0x120, the 32 bytes will actually be written at 0x120, not 0x100.

    EECON1bits.EEPGD = 1;               // write to flash program memory
    EECON1bits.CFGS = 0;                // not configuration registers
    EECON1bits.FREE = 0;                // we're not erasing now.
    EECON1bits.WREN = 1;                // enable write/erase operations

    // execute code sequence, which cannot be interrupted, then execute write32

    //INTCONbits.GIE = 0;   // Disable interrupts
    EECON2 = 0x55;          // Begin Write sequence
    EECON2 = 0xAA;
    EECON1bits.WR = 1;      // Set WR bit to begin 32-byte write
    //INTCONbits.GIE = 1;   // re-enable interrupts

    EECON1bits.WREN = 0;                // disable write/erase operations
}
