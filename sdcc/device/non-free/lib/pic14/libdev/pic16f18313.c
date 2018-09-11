/*
 * This definitions of the PIC16F18313 MCU.
 *
 * This file is part of the GNU PIC library for SDCC, originally
 * created by Molnar Karoly <molnarkaroly@users.sf.net> 2016.
 *
 * This file is generated automatically by the cinc2h.pl, 2016-04-13 17:23:23 UTC.
 *
 * SDCC is licensed under the GNU Public license (GPL) v2. Note that
 * this license covers the code to the compiler and other executables,
 * but explicitly does not cover any code or objects generated by sdcc.
 *
 * For pic device libraries and header files which are derived from
 * Microchip header (.inc) and linker script (.lkr) files Microchip
 * requires that "The header files should state that they are only to be
 * used with authentic Microchip devices" which makes them incompatible
 * with the GPL. Pic device libraries and header files are located at
 * non-free/lib and non-free/include directories respectively.
 * Sdcc should be run with the --use-non-free command line option in
 * order to include non-free header files and libraries.
 *
 * See http://sdcc.sourceforge.net/ for the latest information on sdcc.
 */

#include <pic16f18313.h>

//==============================================================================

__at(0x0000) __sfr INDF0;

__at(0x0001) __sfr INDF1;

__at(0x0002) __sfr PCL;

__at(0x0003) __sfr STATUS;
__at(0x0003) volatile __STATUSbits_t STATUSbits;

__at(0x0004) __sfr FSR0;

__at(0x0004) __sfr FSR0L;

__at(0x0005) __sfr FSR0H;

__at(0x0006) __sfr FSR1;

__at(0x0006) __sfr FSR1L;

__at(0x0007) __sfr FSR1H;

__at(0x0008) __sfr BSR;
__at(0x0008) volatile __BSRbits_t BSRbits;

__at(0x0009) __sfr WREG;

__at(0x000A) __sfr PCLATH;

__at(0x000B) __sfr INTCON;
__at(0x000B) volatile __INTCONbits_t INTCONbits;

__at(0x000C) __sfr PORTA;
__at(0x000C) volatile __PORTAbits_t PORTAbits;

__at(0x0010) __sfr PIR0;
__at(0x0010) volatile __PIR0bits_t PIR0bits;

__at(0x0011) __sfr PIR1;
__at(0x0011) volatile __PIR1bits_t PIR1bits;

__at(0x0012) __sfr PIR2;
__at(0x0012) volatile __PIR2bits_t PIR2bits;

__at(0x0013) __sfr PIR3;
__at(0x0013) volatile __PIR3bits_t PIR3bits;

__at(0x0014) __sfr PIR4;
__at(0x0014) volatile __PIR4bits_t PIR4bits;

__at(0x0015) __sfr TMR0L;
__at(0x0015) volatile __TMR0Lbits_t TMR0Lbits;

__at(0x0016) __sfr TMR0H;
__at(0x0016) volatile __TMR0Hbits_t TMR0Hbits;

__at(0x0017) __sfr T0CON0;
__at(0x0017) volatile __T0CON0bits_t T0CON0bits;

__at(0x0018) __sfr T0CON1;
__at(0x0018) volatile __T0CON1bits_t T0CON1bits;

__at(0x0019) __sfr TMR1;

__at(0x0019) __sfr TMR1L;

__at(0x001A) __sfr TMR1H;

__at(0x001B) __sfr T1CON;
__at(0x001B) volatile __T1CONbits_t T1CONbits;

__at(0x001C) __sfr T1GCON;
__at(0x001C) volatile __T1GCONbits_t T1GCONbits;

__at(0x001D) __sfr TMR2;

__at(0x001E) __sfr PR2;

__at(0x001F) __sfr T2CON;
__at(0x001F) volatile __T2CONbits_t T2CONbits;

__at(0x008C) __sfr TRISA;
__at(0x008C) volatile __TRISAbits_t TRISAbits;

__at(0x0090) __sfr PIE0;
__at(0x0090) volatile __PIE0bits_t PIE0bits;

__at(0x0091) __sfr PIE1;
__at(0x0091) volatile __PIE1bits_t PIE1bits;

__at(0x0092) __sfr PIE2;
__at(0x0092) volatile __PIE2bits_t PIE2bits;

__at(0x0093) __sfr PIE3;
__at(0x0093) volatile __PIE3bits_t PIE3bits;

__at(0x0094) __sfr PIE4;
__at(0x0094) volatile __PIE4bits_t PIE4bits;

__at(0x0097) __sfr WDTCON;
__at(0x0097) volatile __WDTCONbits_t WDTCONbits;

__at(0x009B) __sfr ADRES;

__at(0x009B) __sfr ADRESL;

__at(0x009C) __sfr ADRESH;

__at(0x009D) __sfr ADCON0;
__at(0x009D) volatile __ADCON0bits_t ADCON0bits;

__at(0x009E) __sfr ADCON1;
__at(0x009E) volatile __ADCON1bits_t ADCON1bits;

__at(0x009F) __sfr ADACT;
__at(0x009F) volatile __ADACTbits_t ADACTbits;

__at(0x010C) __sfr LATA;
__at(0x010C) volatile __LATAbits_t LATAbits;

__at(0x0111) __sfr CM1CON0;
__at(0x0111) volatile __CM1CON0bits_t CM1CON0bits;

__at(0x0112) __sfr CM1CON1;
__at(0x0112) volatile __CM1CON1bits_t CM1CON1bits;

__at(0x0115) __sfr CMOUT;
__at(0x0115) volatile __CMOUTbits_t CMOUTbits;

__at(0x0116) __sfr BORCON;
__at(0x0116) volatile __BORCONbits_t BORCONbits;

__at(0x0117) __sfr FVRCON;
__at(0x0117) volatile __FVRCONbits_t FVRCONbits;

__at(0x0118) __sfr DACCON0;
__at(0x0118) volatile __DACCON0bits_t DACCON0bits;

__at(0x0119) __sfr DACCON1;
__at(0x0119) volatile __DACCON1bits_t DACCON1bits;

__at(0x018C) __sfr ANSELA;
__at(0x018C) volatile __ANSELAbits_t ANSELAbits;

__at(0x0197) __sfr VREGCON;
__at(0x0197) volatile __VREGCONbits_t VREGCONbits;

__at(0x0199) __sfr RC1REG;

__at(0x0199) __sfr RCREG;

__at(0x0199) __sfr RCREG1;

__at(0x019A) __sfr TX1REG;

__at(0x019A) __sfr TXREG;

__at(0x019A) __sfr TXREG1;

__at(0x019B) __sfr SP1BRG;

__at(0x019B) __sfr SP1BRGL;

__at(0x019B) __sfr SPBRG;

__at(0x019B) __sfr SPBRG1;

__at(0x019B) __sfr SPBRGL;

__at(0x019C) __sfr SP1BRGH;

__at(0x019C) __sfr SPBRGH;

__at(0x019C) __sfr SPBRGH1;

__at(0x019D) __sfr RC1STA;
__at(0x019D) volatile __RC1STAbits_t RC1STAbits;

__at(0x019D) __sfr RCSTA;
__at(0x019D) volatile __RCSTAbits_t RCSTAbits;

__at(0x019D) __sfr RCSTA1;
__at(0x019D) volatile __RCSTA1bits_t RCSTA1bits;

__at(0x019E) __sfr TX1STA;
__at(0x019E) volatile __TX1STAbits_t TX1STAbits;

__at(0x019E) __sfr TXSTA;
__at(0x019E) volatile __TXSTAbits_t TXSTAbits;

__at(0x019E) __sfr TXSTA1;
__at(0x019E) volatile __TXSTA1bits_t TXSTA1bits;

__at(0x019F) __sfr BAUD1CON;
__at(0x019F) volatile __BAUD1CONbits_t BAUD1CONbits;

__at(0x019F) __sfr BAUDCON;
__at(0x019F) volatile __BAUDCONbits_t BAUDCONbits;

__at(0x019F) __sfr BAUDCON1;
__at(0x019F) volatile __BAUDCON1bits_t BAUDCON1bits;

__at(0x019F) __sfr BAUDCTL;
__at(0x019F) volatile __BAUDCTLbits_t BAUDCTLbits;

__at(0x019F) __sfr BAUDCTL1;
__at(0x019F) volatile __BAUDCTL1bits_t BAUDCTL1bits;

__at(0x020C) __sfr WPUA;
__at(0x020C) volatile __WPUAbits_t WPUAbits;

__at(0x0211) __sfr SSP1BUF;
__at(0x0211) volatile __SSP1BUFbits_t SSP1BUFbits;

__at(0x0211) __sfr SSPBUF;
__at(0x0211) volatile __SSPBUFbits_t SSPBUFbits;

__at(0x0212) __sfr SSP1ADD;
__at(0x0212) volatile __SSP1ADDbits_t SSP1ADDbits;

__at(0x0212) __sfr SSPADD;
__at(0x0212) volatile __SSPADDbits_t SSPADDbits;

__at(0x0213) __sfr SSP1MSK;
__at(0x0213) volatile __SSP1MSKbits_t SSP1MSKbits;

__at(0x0213) __sfr SSPMSK;
__at(0x0213) volatile __SSPMSKbits_t SSPMSKbits;

__at(0x0214) __sfr SSP1STAT;
__at(0x0214) volatile __SSP1STATbits_t SSP1STATbits;

__at(0x0214) __sfr SSPSTAT;
__at(0x0214) volatile __SSPSTATbits_t SSPSTATbits;

__at(0x0215) __sfr SSP1CON;
__at(0x0215) volatile __SSP1CONbits_t SSP1CONbits;

__at(0x0215) __sfr SSP1CON1;
__at(0x0215) volatile __SSP1CON1bits_t SSP1CON1bits;

__at(0x0215) __sfr SSPCON;
__at(0x0215) volatile __SSPCONbits_t SSPCONbits;

__at(0x0215) __sfr SSPCON1;
__at(0x0215) volatile __SSPCON1bits_t SSPCON1bits;

__at(0x0216) __sfr SSP1CON2;
__at(0x0216) volatile __SSP1CON2bits_t SSP1CON2bits;

__at(0x0216) __sfr SSPCON2;
__at(0x0216) volatile __SSPCON2bits_t SSPCON2bits;

__at(0x0217) __sfr SSP1CON3;
__at(0x0217) volatile __SSP1CON3bits_t SSP1CON3bits;

__at(0x0217) __sfr SSPCON3;
__at(0x0217) volatile __SSPCON3bits_t SSPCON3bits;

__at(0x028C) __sfr ODCONA;
__at(0x028C) volatile __ODCONAbits_t ODCONAbits;

__at(0x0291) __sfr CCPR1;

__at(0x0291) __sfr CCPR1L;

__at(0x0292) __sfr CCPR1H;

__at(0x0293) __sfr CCP1CON;
__at(0x0293) volatile __CCP1CONbits_t CCP1CONbits;

__at(0x0294) __sfr CCP1CAP;
__at(0x0294) volatile __CCP1CAPbits_t CCP1CAPbits;

__at(0x0295) __sfr CCPR2;

__at(0x0295) __sfr CCPR2L;

__at(0x0296) __sfr CCPR2H;

__at(0x0297) __sfr CCP2CON;
__at(0x0297) volatile __CCP2CONbits_t CCP2CONbits;

__at(0x0298) __sfr CCP2CAP;
__at(0x0298) volatile __CCP2CAPbits_t CCP2CAPbits;

__at(0x029F) __sfr CCPTMRS;
__at(0x029F) volatile __CCPTMRSbits_t CCPTMRSbits;

__at(0x030C) __sfr SLRCONA;
__at(0x030C) volatile __SLRCONAbits_t SLRCONAbits;

__at(0x038C) __sfr INLVLA;
__at(0x038C) volatile __INLVLAbits_t INLVLAbits;

__at(0x0391) __sfr IOCAP;
__at(0x0391) volatile __IOCAPbits_t IOCAPbits;

__at(0x0392) __sfr IOCAN;
__at(0x0392) volatile __IOCANbits_t IOCANbits;

__at(0x0393) __sfr IOCAF;
__at(0x0393) volatile __IOCAFbits_t IOCAFbits;

__at(0x039A) __sfr CLKRCON;
__at(0x039A) volatile __CLKRCONbits_t CLKRCONbits;

__at(0x039C) __sfr MDCON;
__at(0x039C) volatile __MDCONbits_t MDCONbits;

__at(0x039D) __sfr MDSRC;
__at(0x039D) volatile __MDSRCbits_t MDSRCbits;

__at(0x039E) __sfr MDCARH;
__at(0x039E) volatile __MDCARHbits_t MDCARHbits;

__at(0x039F) __sfr MDCARL;
__at(0x039F) volatile __MDCARLbits_t MDCARLbits;

__at(0x040C) __sfr CCDNA;
__at(0x040C) volatile __CCDNAbits_t CCDNAbits;

__at(0x041F) __sfr CCDCON;
__at(0x041F) volatile __CCDCONbits_t CCDCONbits;

__at(0x048C) __sfr CCDPA;
__at(0x048C) volatile __CCDPAbits_t CCDPAbits;

__at(0x0498) __sfr NCO1ACC;

__at(0x0498) __sfr NCO1ACCL;

__at(0x0499) __sfr NCO1ACCH;

__at(0x049A) __sfr NCO1ACCU;

__at(0x049B) __sfr NCO1INC;

__at(0x049B) __sfr NCO1INCL;

__at(0x049C) __sfr NCO1INCH;

__at(0x049D) __sfr NCO1INCU;

__at(0x049E) __sfr NCO1CON;
__at(0x049E) volatile __NCO1CONbits_t NCO1CONbits;

__at(0x049F) __sfr NCO1CLK;

__at(0x0617) __sfr PWM5DCL;
__at(0x0617) volatile __PWM5DCLbits_t PWM5DCLbits;

__at(0x0618) __sfr PWM5DCH;
__at(0x0618) volatile __PWM5DCHbits_t PWM5DCHbits;

__at(0x0619) __sfr PWM5CON;
__at(0x0619) volatile __PWM5CONbits_t PWM5CONbits;

__at(0x0619) __sfr PWM5CON0;
__at(0x0619) volatile __PWM5CON0bits_t PWM5CON0bits;

__at(0x061A) __sfr PWM6DCL;
__at(0x061A) volatile __PWM6DCLbits_t PWM6DCLbits;

__at(0x061B) __sfr PWM6DCH;
__at(0x061B) volatile __PWM6DCHbits_t PWM6DCHbits;

__at(0x061C) __sfr PWM6CON;
__at(0x061C) volatile __PWM6CONbits_t PWM6CONbits;

__at(0x061C) __sfr PWM6CON0;
__at(0x061C) volatile __PWM6CON0bits_t PWM6CON0bits;

__at(0x0691) __sfr CWG1CLKCON;
__at(0x0691) volatile __CWG1CLKCONbits_t CWG1CLKCONbits;

__at(0x0692) __sfr CWG1DAT;
__at(0x0692) volatile __CWG1DATbits_t CWG1DATbits;

__at(0x0693) __sfr CWG1DBR;
__at(0x0693) volatile __CWG1DBRbits_t CWG1DBRbits;

__at(0x0694) __sfr CWG1DBF;
__at(0x0694) volatile __CWG1DBFbits_t CWG1DBFbits;

__at(0x0695) __sfr CWG1CON0;
__at(0x0695) volatile __CWG1CON0bits_t CWG1CON0bits;

__at(0x0696) __sfr CWG1CON1;
__at(0x0696) volatile __CWG1CON1bits_t CWG1CON1bits;

__at(0x0697) __sfr CWG1AS0;
__at(0x0697) volatile __CWG1AS0bits_t CWG1AS0bits;

__at(0x0698) __sfr CWG1AS1;
__at(0x0698) volatile __CWG1AS1bits_t CWG1AS1bits;

__at(0x0699) __sfr CWG1STR;
__at(0x0699) volatile __CWG1STRbits_t CWG1STRbits;

__at(0x0891) __sfr NVMADR;

__at(0x0891) __sfr NVMADRL;
__at(0x0891) volatile __NVMADRLbits_t NVMADRLbits;

__at(0x0892) __sfr NVMADRH;
__at(0x0892) volatile __NVMADRHbits_t NVMADRHbits;

__at(0x0893) __sfr NVMDAT;

__at(0x0893) __sfr NVMDATL;
__at(0x0893) volatile __NVMDATLbits_t NVMDATLbits;

__at(0x0894) __sfr NVMDATH;
__at(0x0894) volatile __NVMDATHbits_t NVMDATHbits;

__at(0x0895) __sfr NVMCON1;
__at(0x0895) volatile __NVMCON1bits_t NVMCON1bits;

__at(0x0896) __sfr NVMCON2;

__at(0x089B) __sfr PCON0;
__at(0x089B) volatile __PCON0bits_t PCON0bits;

__at(0x0911) __sfr PMD0;
__at(0x0911) volatile __PMD0bits_t PMD0bits;

__at(0x0912) __sfr PMD1;
__at(0x0912) volatile __PMD1bits_t PMD1bits;

__at(0x0913) __sfr PMD2;
__at(0x0913) volatile __PMD2bits_t PMD2bits;

__at(0x0914) __sfr PMD3;
__at(0x0914) volatile __PMD3bits_t PMD3bits;

__at(0x0915) __sfr PMD4;
__at(0x0915) volatile __PMD4bits_t PMD4bits;

__at(0x0916) __sfr PMD5;
__at(0x0916) volatile __PMD5bits_t PMD5bits;

__at(0x0918) __sfr CPUDOZE;
__at(0x0918) volatile __CPUDOZEbits_t CPUDOZEbits;

__at(0x0919) __sfr OSCCON1;
__at(0x0919) volatile __OSCCON1bits_t OSCCON1bits;

__at(0x091A) __sfr OSCCON2;
__at(0x091A) volatile __OSCCON2bits_t OSCCON2bits;

__at(0x091B) __sfr OSCCON3;
__at(0x091B) volatile __OSCCON3bits_t OSCCON3bits;

__at(0x091C) __sfr OSCSTAT1;
__at(0x091C) volatile __OSCSTAT1bits_t OSCSTAT1bits;

__at(0x091D) __sfr OSCEN;
__at(0x091D) volatile __OSCENbits_t OSCENbits;

__at(0x091E) __sfr OSCTUNE;
__at(0x091E) volatile __OSCTUNEbits_t OSCTUNEbits;

__at(0x091F) __sfr OSCFRQ;
__at(0x091F) volatile __OSCFRQbits_t OSCFRQbits;

__at(0x0E0F) __sfr PPSLOCK;
__at(0x0E0F) volatile __PPSLOCKbits_t PPSLOCKbits;

__at(0x0E10) __sfr INTPPS;
__at(0x0E10) volatile __INTPPSbits_t INTPPSbits;

__at(0x0E11) __sfr T0CKIPPS;
__at(0x0E11) volatile __T0CKIPPSbits_t T0CKIPPSbits;

__at(0x0E12) __sfr T1CKIPPS;
__at(0x0E12) volatile __T1CKIPPSbits_t T1CKIPPSbits;

__at(0x0E13) __sfr T1GPPS;
__at(0x0E13) volatile __T1GPPSbits_t T1GPPSbits;

__at(0x0E14) __sfr CCP1PPS;
__at(0x0E14) volatile __CCP1PPSbits_t CCP1PPSbits;

__at(0x0E15) __sfr CCP2PPS;
__at(0x0E15) volatile __CCP2PPSbits_t CCP2PPSbits;

__at(0x0E18) __sfr CWG1PPS;
__at(0x0E18) volatile __CWG1PPSbits_t CWG1PPSbits;

__at(0x0E1A) __sfr MDCIN1PPS;
__at(0x0E1A) volatile __MDCIN1PPSbits_t MDCIN1PPSbits;

__at(0x0E1B) __sfr MDCIN2PPS;
__at(0x0E1B) volatile __MDCIN2PPSbits_t MDCIN2PPSbits;

__at(0x0E1C) __sfr MDMINPPS;
__at(0x0E1C) volatile __MDMINPPSbits_t MDMINPPSbits;

__at(0x0E20) __sfr SSP1CLKPPS;
__at(0x0E20) volatile __SSP1CLKPPSbits_t SSP1CLKPPSbits;

__at(0x0E21) __sfr SSP1DATPPS;
__at(0x0E21) volatile __SSP1DATPPSbits_t SSP1DATPPSbits;

__at(0x0E22) __sfr SSP1SSPPS;
__at(0x0E22) volatile __SSP1SSPPSbits_t SSP1SSPPSbits;

__at(0x0E24) __sfr RXPPS;
__at(0x0E24) volatile __RXPPSbits_t RXPPSbits;

__at(0x0E25) __sfr TXPPS;
__at(0x0E25) volatile __TXPPSbits_t TXPPSbits;

__at(0x0E28) __sfr CLCIN0PPS;
__at(0x0E28) volatile __CLCIN0PPSbits_t CLCIN0PPSbits;

__at(0x0E29) __sfr CLCIN1PPS;
__at(0x0E29) volatile __CLCIN1PPSbits_t CLCIN1PPSbits;

__at(0x0E2A) __sfr CLCIN2PPS;
__at(0x0E2A) volatile __CLCIN2PPSbits_t CLCIN2PPSbits;

__at(0x0E2B) __sfr CLCIN3PPS;
__at(0x0E2B) volatile __CLCIN3PPSbits_t CLCIN3PPSbits;

__at(0x0E90) __sfr RA0PPS;
__at(0x0E90) volatile __RA0PPSbits_t RA0PPSbits;

__at(0x0E91) __sfr RA1PPS;
__at(0x0E91) volatile __RA1PPSbits_t RA1PPSbits;

__at(0x0E92) __sfr RA2PPS;
__at(0x0E92) volatile __RA2PPSbits_t RA2PPSbits;

__at(0x0E94) __sfr RA4PPS;
__at(0x0E94) volatile __RA4PPSbits_t RA4PPSbits;

__at(0x0E95) __sfr RA5PPS;
__at(0x0E95) volatile __RA5PPSbits_t RA5PPSbits;

__at(0x0F0F) __sfr CLCDATA;
__at(0x0F0F) volatile __CLCDATAbits_t CLCDATAbits;

__at(0x0F10) __sfr CLC1CON;
__at(0x0F10) volatile __CLC1CONbits_t CLC1CONbits;

__at(0x0F11) __sfr CLC1POL;
__at(0x0F11) volatile __CLC1POLbits_t CLC1POLbits;

__at(0x0F12) __sfr CLC1SEL0;
__at(0x0F12) volatile __CLC1SEL0bits_t CLC1SEL0bits;

__at(0x0F13) __sfr CLC1SEL1;
__at(0x0F13) volatile __CLC1SEL1bits_t CLC1SEL1bits;

__at(0x0F14) __sfr CLC1SEL2;
__at(0x0F14) volatile __CLC1SEL2bits_t CLC1SEL2bits;

__at(0x0F15) __sfr CLC1SEL3;
__at(0x0F15) volatile __CLC1SEL3bits_t CLC1SEL3bits;

__at(0x0F16) __sfr CLC1GLS0;
__at(0x0F16) volatile __CLC1GLS0bits_t CLC1GLS0bits;

__at(0x0F17) __sfr CLC1GLS1;
__at(0x0F17) volatile __CLC1GLS1bits_t CLC1GLS1bits;

__at(0x0F18) __sfr CLC1GLS2;
__at(0x0F18) volatile __CLC1GLS2bits_t CLC1GLS2bits;

__at(0x0F19) __sfr CLC1GLS3;
__at(0x0F19) volatile __CLC1GLS3bits_t CLC1GLS3bits;

__at(0x0F1A) __sfr CLC2CON;
__at(0x0F1A) volatile __CLC2CONbits_t CLC2CONbits;

__at(0x0F1B) __sfr CLC2POL;
__at(0x0F1B) volatile __CLC2POLbits_t CLC2POLbits;

__at(0x0F1C) __sfr CLC2SEL0;
__at(0x0F1C) volatile __CLC2SEL0bits_t CLC2SEL0bits;

__at(0x0F1D) __sfr CLC2SEL1;
__at(0x0F1D) volatile __CLC2SEL1bits_t CLC2SEL1bits;

__at(0x0F1E) __sfr CLC2SEL2;
__at(0x0F1E) volatile __CLC2SEL2bits_t CLC2SEL2bits;

__at(0x0F1F) __sfr CLC2SEL3;
__at(0x0F1F) volatile __CLC2SEL3bits_t CLC2SEL3bits;

__at(0x0F20) __sfr CLC2GLS0;
__at(0x0F20) volatile __CLC2GLS0bits_t CLC2GLS0bits;

__at(0x0F21) __sfr CLC2GLS1;
__at(0x0F21) volatile __CLC2GLS1bits_t CLC2GLS1bits;

__at(0x0F22) __sfr CLC2GLS2;
__at(0x0F22) volatile __CLC2GLS2bits_t CLC2GLS2bits;

__at(0x0F23) __sfr CLC2GLS3;
__at(0x0F23) volatile __CLC2GLS3bits_t CLC2GLS3bits;

__at(0x0FE4) __sfr STATUS_SHAD;
__at(0x0FE4) volatile __STATUS_SHADbits_t STATUS_SHADbits;

__at(0x0FE5) __sfr WREG_SHAD;

__at(0x0FE6) __sfr BSR_SHAD;

__at(0x0FE7) __sfr PCLATH_SHAD;

__at(0x0FE8) __sfr FSR0L_SHAD;

__at(0x0FE9) __sfr FSR0H_SHAD;

__at(0x0FEA) __sfr FSR1L_SHAD;

__at(0x0FEB) __sfr FSR1H_SHAD;

__at(0x0FED) __sfr STKPTR;

__at(0x0FEE) __sfr TOSL;

__at(0x0FEF) __sfr TOSH;
