#define _XTAL_FREQ 8000000

#define RS RA2_bit
#define EN RA3_bit
#define D4 RA4_bit
#define D5 RA5_bit
#define D6 RA6_bit
#define D7 RA7_bit

#include <xc.h>
#include "lcd.h";


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

int main()
{
  unsigned int a;
  TRISA = 0x00;
  Lcd_Init();
  while(1)
  {
    Lcd_Clear();
    Lcd_Set_Cursor(1,1);
    Lcd_Write_String("LCD Library for");
    Lcd_Set_Cursor(2,1);
    Lcd_Write_String("MPLAB XC8");
    __delay_ms(2000);
    Lcd_Clear();
    Lcd_Set_Cursor(1,1);
    Lcd_Write_String("Developed By");
    Lcd_Set_Cursor(2,1);
    Lcd_Write_String("electroSome");
    __delay_ms(2000);
    Lcd_Clear();
    Lcd_Set_Cursor(1,1);
    Lcd_Write_String("www.electroSome.com");

    for(a=0;a<15;a++)
    {
        __delay_ms(300);
        Lcd_Shift_Left();
    }

    for(a=0;a<15;a++)
    {
        __delay_ms(300);
        Lcd_Shift_Right();
    }

    Lcd_Clear();
    Lcd_Set_Cursor(2,1);
    Lcd_Write_Char('e');
    Lcd_Write_Char('S');
    __delay_ms(2000);
  }
  return 0;
}