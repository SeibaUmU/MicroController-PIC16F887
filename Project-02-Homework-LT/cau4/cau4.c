
// PIC16F887 Configuration Bit Settings

// 'C' source line config statements

// CONFIG1
#pragma config FOSC = HS        // Oscillator Selection bits (HS oscillator: High-speed crystal/resonator on RA6/OSC2/CLKOUT and RA7/OSC1/CLKIN)
#pragma config WDTE = OFF        // Watchdog Timer Enable bit (WDT enabled)
#pragma config PWRTE = OFF      // Power-up Timer Enable bit (PWRT disabled)
#pragma config MCLRE = ON       // RE3/MCLR pin function select bit (RE3/MCLR pin function is MCLR)
#pragma config CP = OFF         // Code Protection bit (Program memory code protection is disabled)
#pragma config CPD = OFF        // Data Code Protection bit (Data memory code protection is disabled)
#pragma config BOREN = ON       // Brown Out Reset Selection bits (BOR enabled)
#pragma config IESO = ON        // Internal External Switchover bit (Internal/External Switchover mode is enabled)
#pragma config FCMEN = ON       // Fail-Safe Clock Monitor Enabled bit (Fail-Safe Clock Monitor is enabled)
#pragma config LVP = ON         // Low Voltage Programming Enable bit (RB3/PGM pin has PGM function, low voltage programming enabled)

// CONFIG2
#pragma config BOR4V = BOR40V   // Brown-out Reset Selection bit (Brown-out Reset set to 4.0V)
#pragma config WRT = OFF        // Flash Program Memory Self Write Enable bits (Write protection off)

// #pragma config statements should precede project file includes.
// Use project enums instead of #define for ON and OFF.

#include <xc.h>
#include <math.h>  
#define _XTAL_FREQ 4000000
const unsigned char a[]= 
{0xC0,0XF9,0XA4,0XB0,0X99,0X92,0X82,0XF8,0X80,0X90,0X88,0X83,0XC6,0XA1,0X86,0X8E }; // Khai b�o gi� tr? c?a led 7 ?o?n
unsigned long adc_value = 0; // ban ?�� cho gi� tr? adc_value =0
void adc (void) 
{
    // ch?n ch�n analog RE1
    ANS6 = 1; 
    TRISE1 = 1;
    // canh ph?i  ADCON1
    ADFM = 1;
    //RA2  Ch�nh l� +0V
    VCFG1 = 0;  
    //RA3 ch�nh l� 5V c?ng ch�nh l� t? +0V ??n +5V ly tu nguon pic
    VCFG0 = 0;
    // fosc/8
    ADCS1 = 0;
    ADCS0 = 1;
    // Ch?n k�nh theo ch�n analog ? ?�y l� ch�n RE6 = AN6 ( 6 = 0110)
    CHS3 = 0;
    CHS2 = 1;
    CHS1 = 1;
    CHS0 = 0;
    // B?t t�nh n?ng ADC
    ADON = 1; // cho phep ADC
    // khai b�o ng?t
    ADIF = 0; //  // xoa co ngat
    GIE = PEIE = ADIE =0; //cam ngat  
}
void main ()
{
    ANSEL = ANSELH = 0X00;
    TRISD = 0X00;
    TRISC = 0X00;
    TRISB = 0X00;
    nRBPU=0; WPUB=0xFF; //R treo 
    PORTD = 0X00;
    PORTC = 0X00;
    PORTB = 0X00;
    adc();
    while(1)
    {
        __delay_us(200);
        // B?t ??u chuy?n ??i
        GO = 1;
        while(GO);
        
        // ??c gi� tr?
        adc_value = ADRESL;
        adc_value |=(unsigned int)ADRESH << 8;
        
        // Hi?n th? ra LED
        // Theo h�nh m� ph?ng: 
        // PORTC: H�ng tr?m ("2")
        // PORTD: H�ng ch?c ("5")
        // PORTB: H�ng ??n v? ("6")
        PORTC = a[adc_value/100];
        PORTD = a[adc_value/10%10];
        PORTB = a[adc_value%10];
    }  
}
/*da dung: 1.25v ra 256 led (Vref = Vref(+) - Vref(-) = 5-0 = 5
 adc_value = Vin x (2^10 - 1) / Vref = 1.25 x 1023 / 5 = 255.75
 
 */

