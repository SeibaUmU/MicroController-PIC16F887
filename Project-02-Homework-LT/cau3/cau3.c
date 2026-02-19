// PIC16F887 Configuration Bit Settings

// 'C' source line config statements
//setting can thiet de chay duoc chuong trinh
// CONFIG1
#pragma config FOSC = HS        // Oscillator Selection bits (HS oscillator: High-speed crystal/resonator on RA6/OSC2/CLKOUT and RA7/OSC1/CLKIN)
#pragma config WDTE = OFF        // Watchdog Timer Enable bit (WDT enabled)
#pragma config PWRTE = ON      // Power-up Timer Enable bit (PWRT disabled)
#pragma config MCLRE = ON       // RE3/MCLR pin function select bit (RE3/MCLR pin function is MCLR)
#pragma config CP = OFF         // Code Protection bit (Program memory code protection is disabled)
#pragma config CPD = OFF        // Data Code Protection bit (Data memory code protection is disabled)
#pragma config BOREN = OFF       // Brown Out Reset Selection bits (BOR enabled)
#pragma config IESO = OFF        // Internal External Switchover bit (Internal/External Switchover mode is enabled)
#pragma config FCMEN = OFF       // Fail-Safe Clock Monitor Enabled bit (Fail-Safe Clock Monitor is enabled)
#pragma config LVP = OFF         // Low Voltage Programming Enable bit (RB3/PGM pin has PGM function, low voltage programming enabled)
#pragma config DEBUG = ON
// CONFIG2
#pragma config BOR4V = BOR40V   // Brown-out Reset Selection bit (Brown-out Reset set to 4.0V)
#pragma config WRT = OFF        // Flash Program Memory Self Write Enable bits (Write protection off)

// #pragma config statements should precede project file includes.
// Use project enums instead of #define for ON and OFF.

#include <xc.h>
#include <math.h>
#define _XTAL_FREQ 4000000
void main() {
    ANSEL = ANSELH = 0X00;
    //RC2 -> ccp1 , RC1 -> ccp2
    TRISC2 = 1; //CAM XUAT XUNG 
    TMR2 =0X00;
    /*
    T = t_on + t_off = 150u + 50u = 200us
    chọn Pre = 1 (1,4,16)
    T = (PR2 + 1) * 4 * 1/fosc * Pre = > PR2 = 199
    D = PW/T x 100% = 150/200 x 100% = 75%
    PW = (CCPR1L:CCP1CON<5:4>) * (1/fosc) * Pre *1/fosc (x la CCPR1L:CCP1CON<5:4>) => x = 600
    CCPR1L = x / 4 = 150
    CPP1CON<5:4> = x % 4 = 0 ( 0 0) (2: 1 0 )
    => CCPR1L = 150, CCP1CON5 = DC1B1 = 0, CCP1CON4 = DC1B0 = 0 (2: 1 0)
    */
    PR2 = 199;
    CCPR1L = 150;
    DC1B1 = 0; //CCP1ON5
    DC1B0 = 0; //CCP1ON4

    CCP1CON = 0X0C; //CCP1 MODE PWM (1 1 x x)
    TMR2IF = 0;
    //PRESCALE 1:1
    T2CKPS1 = 0; T2CKPS0 = 0;  
    TMR2ON = 1; //BAT TMR2

    TRISB0 = 1;
    TRISB1 = 1;

    nRBPU = 0;   // pull-up PORTB
    WPUB = 0b00000011;         // RB0 và RB1 có pull-up
    // Chỉ Timer2 trên 1 lần để PWM ổn định
    while(TMR2IF == 0);
    while(1) {
        if (RB0 == 0) {
            __delay_ms(20);
            if (RB0 == 0) {
                TRISC2 = 0; //XUAT XUNG
                while(RB0 == 0);
            } 
        }
        if (RB1 == 0) {
            __delay_ms(20);
            if (RB1 == 0) {
                TRISC2 = 1; //KHONG XUAT XUNG
                while(RB1 == 0);
            } 
        }
    }
}
