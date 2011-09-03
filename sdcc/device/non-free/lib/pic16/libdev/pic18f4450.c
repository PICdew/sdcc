/*
 * pic18f4450.c - device specific definitions
 *
 * This file is part of the GNU PIC library for SDCC,
 * originally devised by Vangelis Rokas <vrokas AT otenet.gr>
 *
 * It has been automatically generated by inc2h-pic16.pl,
 * (c) 2007 by Raphael Neider <rneider AT web.de>
 */

#include <pic18f4450.h>


__sfr __at (0xF66) UFRM;

__sfr __at (0xF66) UFRML;

__sfr __at (0xF67) UFRMH;

__sfr __at (0xF68) UIR;
volatile __UIRbits_t __at (0xF68) UIRbits;

__sfr __at (0xF69) UIE;
volatile __UIEbits_t __at (0xF69) UIEbits;

__sfr __at (0xF6A) UEIR;
volatile __UEIRbits_t __at (0xF6A) UEIRbits;

__sfr __at (0xF6B) UEIE;
volatile __UEIEbits_t __at (0xF6B) UEIEbits;

__sfr __at (0xF6C) USTAT;
volatile __USTATbits_t __at (0xF6C) USTATbits;

__sfr __at (0xF6D) UCON;
volatile __UCONbits_t __at (0xF6D) UCONbits;

__sfr __at (0xF6E) UADDR;
volatile __UADDRbits_t __at (0xF6E) UADDRbits;

__sfr __at (0xF6F) UCFG;
volatile __UCFGbits_t __at (0xF6F) UCFGbits;

__sfr __at (0xF70) UEP0;
volatile __UEP0bits_t __at (0xF70) UEP0bits;

__sfr __at (0xF71) UEP1;
volatile __UEP1bits_t __at (0xF71) UEP1bits;

__sfr __at (0xF72) UEP2;
volatile __UEP2bits_t __at (0xF72) UEP2bits;

__sfr __at (0xF73) UEP3;
volatile __UEP3bits_t __at (0xF73) UEP3bits;

__sfr __at (0xF74) UEP4;
volatile __UEP4bits_t __at (0xF74) UEP4bits;

__sfr __at (0xF75) UEP5;
volatile __UEP5bits_t __at (0xF75) UEP5bits;

__sfr __at (0xF76) UEP6;
volatile __UEP6bits_t __at (0xF76) UEP6bits;

__sfr __at (0xF77) UEP7;
volatile __UEP7bits_t __at (0xF77) UEP7bits;

__sfr __at (0xF78) UEP8;
volatile __UEP8bits_t __at (0xF78) UEP8bits;

__sfr __at (0xF79) UEP9;
volatile __UEP9bits_t __at (0xF79) UEP9bits;

__sfr __at (0xF7A) UEP10;
volatile __UEP10bits_t __at (0xF7A) UEP10bits;

__sfr __at (0xF7B) UEP11;
volatile __UEP11bits_t __at (0xF7B) UEP11bits;

__sfr __at (0xF7C) UEP12;
volatile __UEP12bits_t __at (0xF7C) UEP12bits;

__sfr __at (0xF7D) UEP13;
volatile __UEP13bits_t __at (0xF7D) UEP13bits;

__sfr __at (0xF7E) UEP14;
volatile __UEP14bits_t __at (0xF7E) UEP14bits;

__sfr __at (0xF7F) UEP15;
volatile __UEP15bits_t __at (0xF7F) UEP15bits;

__sfr __at (0xF80) PORTA;
volatile __PORTAbits_t __at (0xF80) PORTAbits;

__sfr __at (0xF81) PORTB;
volatile __PORTBbits_t __at (0xF81) PORTBbits;

__sfr __at (0xF82) PORTC;
volatile __PORTCbits_t __at (0xF82) PORTCbits;

__sfr __at (0xF83) PORTD;
volatile __PORTDbits_t __at (0xF83) PORTDbits;

__sfr __at (0xF84) PORTE;
volatile __PORTEbits_t __at (0xF84) PORTEbits;

__sfr __at (0xF89) LATA;
volatile __LATAbits_t __at (0xF89) LATAbits;

__sfr __at (0xF8A) LATB;
volatile __LATBbits_t __at (0xF8A) LATBbits;

__sfr __at (0xF8B) LATC;
volatile __LATCbits_t __at (0xF8B) LATCbits;

__sfr __at (0xF8C) LATD;
volatile __LATDbits_t __at (0xF8C) LATDbits;

__sfr __at (0xF8D) LATE;
volatile __LATEbits_t __at (0xF8D) LATEbits;

__sfr __at (0xF92) DDRA;
volatile __DDRAbits_t __at (0xF92) DDRAbits;

__sfr __at (0xF92) TRISA;
volatile __TRISAbits_t __at (0xF92) TRISAbits;

__sfr __at (0xF93) DDRB;
volatile __DDRBbits_t __at (0xF93) DDRBbits;

__sfr __at (0xF93) TRISB;
volatile __TRISBbits_t __at (0xF93) TRISBbits;

__sfr __at (0xF94) DDRC;
volatile __DDRCbits_t __at (0xF94) DDRCbits;

__sfr __at (0xF94) TRISC;
volatile __TRISCbits_t __at (0xF94) TRISCbits;

__sfr __at (0xF95) DDRD;
volatile __DDRDbits_t __at (0xF95) DDRDbits;

__sfr __at (0xF95) TRISD;
volatile __TRISDbits_t __at (0xF95) TRISDbits;

__sfr __at (0xF96) DDRE;
volatile __DDREbits_t __at (0xF96) DDREbits;

__sfr __at (0xF96) TRISE;
volatile __TRISEbits_t __at (0xF96) TRISEbits;

__sfr __at (0xF9D) PIE1;
volatile __PIE1bits_t __at (0xF9D) PIE1bits;

__sfr __at (0xF9E) PIR1;
volatile __PIR1bits_t __at (0xF9E) PIR1bits;

__sfr __at (0xF9F) IPR1;
volatile __IPR1bits_t __at (0xF9F) IPR1bits;

__sfr __at (0xFA0) PIE2;
volatile __PIE2bits_t __at (0xFA0) PIE2bits;

__sfr __at (0xFA1) PIR2;
volatile __PIR2bits_t __at (0xFA1) PIR2bits;

__sfr __at (0xFA2) IPR2;
volatile __IPR2bits_t __at (0xFA2) IPR2bits;

__sfr __at (0xFA6) EECON1;
volatile __EECON1bits_t __at (0xFA6) EECON1bits;

__sfr __at (0xFA7) EECON2;

__sfr __at (0xFAB) RCSTA;
volatile __RCSTAbits_t __at (0xFAB) RCSTAbits;

__sfr __at (0xFAC) TXSTA;
volatile __TXSTAbits_t __at (0xFAC) TXSTAbits;

__sfr __at (0xFAD) TXREG;

__sfr __at (0xFAE) RCREG;

__sfr __at (0xFAF) SPBRG;

__sfr __at (0xFB0) SPBRGH;

__sfr __at (0xFB8) BAUDCON;
volatile __BAUDCONbits_t __at (0xFB8) BAUDCONbits;

__sfr __at (0xFBD) CCP1CON;
volatile __CCP1CONbits_t __at (0xFBD) CCP1CONbits;

__sfr __at (0xFBE) CCPR1;

__sfr __at (0xFBE) CCPR1L;

__sfr __at (0xFBF) CCPR1H;

__sfr __at (0xFC0) ADCON2;
volatile __ADCON2bits_t __at (0xFC0) ADCON2bits;

__sfr __at (0xFC1) ADCON1;
volatile __ADCON1bits_t __at (0xFC1) ADCON1bits;

__sfr __at (0xFC2) ADCON0;
volatile __ADCON0bits_t __at (0xFC2) ADCON0bits;

__sfr __at (0xFC3) ADRES;

__sfr __at (0xFC3) ADRESL;

__sfr __at (0xFC4) ADRESH;

__sfr __at (0xFC9) SSPBUF; /* unimplemented */

__sfr __at (0xFCA) T2CON;
volatile __T2CONbits_t __at (0xFCA) T2CONbits;

__sfr __at (0xFCB) PR2;

__sfr __at (0xFCC) TMR2;

__sfr __at (0xFCD) T1CON;
volatile __T1CONbits_t __at (0xFCD) T1CONbits;

__sfr __at (0xFCE) TMR1L;

__sfr __at (0xFCF) TMR1H;

__sfr __at (0xFD0) RCON;
volatile __RCONbits_t __at (0xFD0) RCONbits;

__sfr __at (0xFD1) WDTCON;
volatile __WDTCONbits_t __at (0xFD1) WDTCONbits;

__sfr __at (0xFD2) HLVDCON;
volatile __HLVDCONbits_t __at (0xFD2) HLVDCONbits;

__sfr __at (0xFD2) LVDCON;
volatile __LVDCONbits_t __at (0xFD2) LVDCONbits;

__sfr __at (0xFD3) OSCCON;
volatile __OSCCONbits_t __at (0xFD3) OSCCONbits;

__sfr __at (0xFD5) T0CON;
volatile __T0CONbits_t __at (0xFD5) T0CONbits;

__sfr __at (0xFD6) TMR0L;

__sfr __at (0xFD7) TMR0H;

__sfr __at (0xFD8) STATUS;
volatile __STATUSbits_t __at (0xFD8) STATUSbits;

__sfr __at (0xFD9) FSR2L;

__sfr __at (0xFDA) FSR2H;

__sfr __at (0xFDB) PLUSW2;

__sfr __at (0xFDC) PREINC2;

__sfr __at (0xFDD) POSTDEC2;

__sfr __at (0xFDE) POSTINC2;

__sfr __at (0xFDF) INDF2;

__sfr __at (0xFE0) BSR;

__sfr __at (0xFE1) FSR1L;

__sfr __at (0xFE2) FSR1H;

__sfr __at (0xFE3) PLUSW1;

__sfr __at (0xFE4) PREINC1;

__sfr __at (0xFE5) POSTDEC1;

__sfr __at (0xFE6) POSTINC1;

__sfr __at (0xFE7) INDF1;

__sfr __at (0xFE8) WREG;

__sfr __at (0xFE9) FSR0L;

__sfr __at (0xFEA) FSR0H;

__sfr __at (0xFEB) PLUSW0;

__sfr __at (0xFEC) PREINC0;

__sfr __at (0xFED) POSTDEC0;

__sfr __at (0xFEE) POSTINC0;

__sfr __at (0xFEF) INDF0;

__sfr __at (0xFF0) INTCON3;
volatile __INTCON3bits_t __at (0xFF0) INTCON3bits;

__sfr __at (0xFF1) INTCON2;
volatile __INTCON2bits_t __at (0xFF1) INTCON2bits;

__sfr __at (0xFF2) INTCON;
volatile __INTCONbits_t __at (0xFF2) INTCONbits;

__sfr __at (0xFF3) PROD;

__sfr __at (0xFF3) PRODL;

__sfr __at (0xFF4) PRODH;

__sfr __at (0xFF5) TABLAT;

__sfr __at (0xFF6) TBLPTR;

__sfr __at (0xFF6) TBLPTRL;

__sfr __at (0xFF7) TBLPTRH;

__sfr __at (0xFF8) TBLPTRU;

__sfr __at (0xFF9) PC;

__sfr __at (0xFF9) PCL;

__sfr __at (0xFFA) PCLATH;

__sfr __at (0xFFB) PCLATU;

__sfr __at (0xFFC) STKPTR;
volatile __STKPTRbits_t __at (0xFFC) STKPTRbits;

__sfr __at (0xFFD) TOS;

__sfr __at (0xFFD) TOSL;

__sfr __at (0xFFE) TOSH;

__sfr __at (0xFFF) TOSU;


