/*
 * This declarations of the PIC16C62 MCU.
 *
 * This file is part of the GNU PIC library for SDCC, originally
 * created by Molnar Karoly <molnarkaroly@users.sf.net> 2014.
 *
 * This file is generated automatically by the cinc2h.pl, 2014-09-15 14:38:42 UTC.
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

#ifndef __PIC16C62_H__
#define __PIC16C62_H__

//==============================================================================
//
//	Register Addresses
//
//==============================================================================

#ifndef NO_ADDR_DEFINES

#define INDF_ADDR               0x0000
#define TMR0_ADDR               0x0001
#define PCL_ADDR                0x0002
#define STATUS_ADDR             0x0003
#define FSR_ADDR                0x0004
#define PORTA_ADDR              0x0005
#define PORTB_ADDR              0x0006
#define PORTC_ADDR              0x0007
#define PCLATH_ADDR             0x000A
#define INTCON_ADDR             0x000B
#define PIR1_ADDR               0x000C
#define TMR1L_ADDR              0x000E
#define TMR1H_ADDR              0x000F
#define T1CON_ADDR              0x0010
#define TMR2_ADDR               0x0011
#define T2CON_ADDR              0x0012
#define SSPBUF_ADDR             0x0013
#define SSPCON_ADDR             0x0014
#define CCPR1L_ADDR             0x0015
#define CCPR1H_ADDR             0x0016
#define CCP1CON_ADDR            0x0017
#define OPTION_REG_ADDR         0x0081
#define TRISA_ADDR              0x0085
#define TRISB_ADDR              0x0086
#define TRISC_ADDR              0x0087
#define PIE1_ADDR               0x008C
#define PCON_ADDR               0x008E
#define PR2_ADDR                0x0092
#define SSPADD_ADDR             0x0093
#define SSPSTAT_ADDR            0x0094

#endif // #ifndef NO_ADDR_DEFINES

//==============================================================================
//
//	Register Definitions
//
//==============================================================================

extern __at(0x0000) __sfr INDF;
extern __at(0x0001) __sfr TMR0;
extern __at(0x0002) __sfr PCL;

//==============================================================================
//        STATUS Bits

extern __at(0x0003) __sfr STATUS;

typedef union
  {
  struct
    {
    unsigned C                  : 1;
    unsigned DC                 : 1;
    unsigned Z                  : 1;
    unsigned NOT_PD             : 1;
    unsigned NOT_TO             : 1;
    unsigned RP0                : 1;
    unsigned RP1                : 1;
    unsigned IRP                : 1;
    };

  struct
    {
    unsigned                    : 5;
    unsigned RP                 : 2;
    unsigned                    : 1;
    };
  } __STATUSbits_t;

extern __at(0x0003) volatile __STATUSbits_t STATUSbits;

#define _C                      0x01
#define _DC                     0x02
#define _Z                      0x04
#define _NOT_PD                 0x08
#define _NOT_TO                 0x10
#define _RP0                    0x20
#define _RP1                    0x40
#define _IRP                    0x80

//==============================================================================

extern __at(0x0004) __sfr FSR;
extern __at(0x0005) __sfr PORTA;
extern __at(0x0006) __sfr PORTB;
extern __at(0x0007) __sfr PORTC;
extern __at(0x000A) __sfr PCLATH;

//==============================================================================
//        INTCON Bits

extern __at(0x000B) __sfr INTCON;

typedef struct
  {
  unsigned RBIF                 : 1;
  unsigned INTF                 : 1;
  unsigned T0IF                 : 1;
  unsigned RBIE                 : 1;
  unsigned INTE                 : 1;
  unsigned T0IE                 : 1;
  unsigned PEIE                 : 1;
  unsigned GIE                  : 1;
  } __INTCONbits_t;

extern __at(0x000B) volatile __INTCONbits_t INTCONbits;

#define _RBIF                   0x01
#define _INTF                   0x02
#define _T0IF                   0x04
#define _RBIE                   0x08
#define _INTE                   0x10
#define _T0IE                   0x20
#define _PEIE                   0x40
#define _GIE                    0x80

//==============================================================================


//==============================================================================
//        PIR1 Bits

extern __at(0x000C) __sfr PIR1;

typedef struct
  {
  unsigned TMR1IF               : 1;
  unsigned TMR2IF               : 1;
  unsigned CCP1IF               : 1;
  unsigned SSPIF                : 1;
  unsigned                      : 1;
  unsigned                      : 1;
  unsigned                      : 1;
  unsigned                      : 1;
  } __PIR1bits_t;

extern __at(0x000C) volatile __PIR1bits_t PIR1bits;

#define _TMR1IF                 0x01
#define _TMR2IF                 0x02
#define _CCP1IF                 0x04
#define _SSPIF                  0x08

//==============================================================================

extern __at(0x000E) __sfr TMR1L;
extern __at(0x000F) __sfr TMR1H;

//==============================================================================
//        T1CON Bits

extern __at(0x0010) __sfr T1CON;

typedef union
  {
  struct
    {
    unsigned TMR1ON             : 1;
    unsigned TMR1CS             : 1;
    unsigned NOT_T1SYNC         : 1;
    unsigned T1OSCEN            : 1;
    unsigned T1CKPS0            : 1;
    unsigned T1CKPS1            : 1;
    unsigned                    : 1;
    unsigned                    : 1;
    };

  struct
    {
    unsigned                    : 4;
    unsigned T1CKPS             : 2;
    unsigned                    : 2;
    };
  } __T1CONbits_t;

extern __at(0x0010) volatile __T1CONbits_t T1CONbits;

#define _TMR1ON                 0x01
#define _TMR1CS                 0x02
#define _NOT_T1SYNC             0x04
#define _T1OSCEN                0x08
#define _T1CKPS0                0x10
#define _T1CKPS1                0x20

//==============================================================================

extern __at(0x0011) __sfr TMR2;

//==============================================================================
//        T2CON Bits

extern __at(0x0012) __sfr T2CON;

typedef union
  {
  struct
    {
    unsigned T2CKPS0            : 1;
    unsigned T2CKPS1            : 1;
    unsigned TMR2ON             : 1;
    unsigned TOUTPS0            : 1;
    unsigned TOUTPS1            : 1;
    unsigned TOUTPS2            : 1;
    unsigned TOUTPS3            : 1;
    unsigned                    : 1;
    };

  struct
    {
    unsigned T2CKPS             : 2;
    unsigned                    : 6;
    };

  struct
    {
    unsigned                    : 3;
    unsigned TOUTPS             : 4;
    unsigned                    : 1;
    };
  } __T2CONbits_t;

extern __at(0x0012) volatile __T2CONbits_t T2CONbits;

#define _T2CKPS0                0x01
#define _T2CKPS1                0x02
#define _TMR2ON                 0x04
#define _TOUTPS0                0x08
#define _TOUTPS1                0x10
#define _TOUTPS2                0x20
#define _TOUTPS3                0x40

//==============================================================================

extern __at(0x0013) __sfr SSPBUF;

//==============================================================================
//        SSPCON Bits

extern __at(0x0014) __sfr SSPCON;

typedef union
  {
  struct
    {
    unsigned SSPM0              : 1;
    unsigned SSPM1              : 1;
    unsigned SSPM2              : 1;
    unsigned SSPM3              : 1;
    unsigned CKP                : 1;
    unsigned SSPEN              : 1;
    unsigned SSPOV              : 1;
    unsigned WCOL               : 1;
    };

  struct
    {
    unsigned SSPM               : 4;
    unsigned                    : 4;
    };
  } __SSPCONbits_t;

extern __at(0x0014) volatile __SSPCONbits_t SSPCONbits;

#define _SSPM0                  0x01
#define _SSPM1                  0x02
#define _SSPM2                  0x04
#define _SSPM3                  0x08
#define _CKP                    0x10
#define _SSPEN                  0x20
#define _SSPOV                  0x40
#define _WCOL                   0x80

//==============================================================================

extern __at(0x0015) __sfr CCPR1L;
extern __at(0x0016) __sfr CCPR1H;

//==============================================================================
//        CCP1CON Bits

extern __at(0x0017) __sfr CCP1CON;

typedef union
  {
  struct
    {
    unsigned CCP1M0             : 1;
    unsigned CCP1M1             : 1;
    unsigned CCP1M2             : 1;
    unsigned CCP1M3             : 1;
    unsigned CCP1Y              : 1;
    unsigned CCP1X              : 1;
    unsigned                    : 1;
    unsigned                    : 1;
    };

  struct
    {
    unsigned CCP1M              : 4;
    unsigned                    : 4;
    };
  } __CCP1CONbits_t;

extern __at(0x0017) volatile __CCP1CONbits_t CCP1CONbits;

#define _CCP1M0                 0x01
#define _CCP1M1                 0x02
#define _CCP1M2                 0x04
#define _CCP1M3                 0x08
#define _CCP1Y                  0x10
#define _CCP1X                  0x20

//==============================================================================


//==============================================================================
//        OPTION_REG Bits

extern __at(0x0081) __sfr OPTION_REG;

typedef union
  {
  struct
    {
    unsigned PS0                : 1;
    unsigned PS1                : 1;
    unsigned PS2                : 1;
    unsigned PSA                : 1;
    unsigned T0SE               : 1;
    unsigned T0CS               : 1;
    unsigned INTEDG             : 1;
    unsigned NOT_RBPU           : 1;
    };

  struct
    {
    unsigned PS                 : 3;
    unsigned                    : 5;
    };
  } __OPTION_REGbits_t;

extern __at(0x0081) volatile __OPTION_REGbits_t OPTION_REGbits;

#define _PS0                    0x01
#define _PS1                    0x02
#define _PS2                    0x04
#define _PSA                    0x08
#define _T0SE                   0x10
#define _T0CS                   0x20
#define _INTEDG                 0x40
#define _NOT_RBPU               0x80

//==============================================================================

extern __at(0x0085) __sfr TRISA;
extern __at(0x0086) __sfr TRISB;
extern __at(0x0087) __sfr TRISC;

//==============================================================================
//        PIE1 Bits

extern __at(0x008C) __sfr PIE1;

typedef struct
  {
  unsigned TMR1IE               : 1;
  unsigned TMR2IE               : 1;
  unsigned CCP1IE               : 1;
  unsigned SSPIE                : 1;
  unsigned                      : 1;
  unsigned                      : 1;
  unsigned                      : 1;
  unsigned                      : 1;
  } __PIE1bits_t;

extern __at(0x008C) volatile __PIE1bits_t PIE1bits;

#define _TMR1IE                 0x01
#define _TMR2IE                 0x02
#define _CCP1IE                 0x04
#define _SSPIE                  0x08

//==============================================================================


//==============================================================================
//        PCON Bits

extern __at(0x008E) __sfr PCON;

typedef struct
  {
  unsigned                      : 1;
  unsigned NOT_POR              : 1;
  unsigned                      : 1;
  unsigned                      : 1;
  unsigned                      : 1;
  unsigned                      : 1;
  unsigned                      : 1;
  unsigned                      : 1;
  } __PCONbits_t;

extern __at(0x008E) volatile __PCONbits_t PCONbits;

#define _NOT_POR                0x02

//==============================================================================

extern __at(0x0092) __sfr PR2;
extern __at(0x0093) __sfr SSPADD;

//==============================================================================
//        SSPSTAT Bits

extern __at(0x0094) __sfr SSPSTAT;

typedef union
  {
  struct
    {
    unsigned BF                 : 1;
    unsigned UA                 : 1;
    unsigned R                  : 1;
    unsigned S                  : 1;
    unsigned P                  : 1;
    unsigned D                  : 1;
    unsigned                    : 1;
    unsigned                    : 1;
    };

  struct
    {
    unsigned                    : 1;
    unsigned                    : 1;
    unsigned I2C_READ           : 1;
    unsigned I2C_START          : 1;
    unsigned I2C_STOP           : 1;
    unsigned I2C_DATA           : 1;
    unsigned                    : 1;
    unsigned                    : 1;
    };

  struct
    {
    unsigned                    : 1;
    unsigned                    : 1;
    unsigned NOT_W              : 1;
    unsigned                    : 1;
    unsigned                    : 1;
    unsigned NOT_A              : 1;
    unsigned                    : 1;
    unsigned                    : 1;
    };

  struct
    {
    unsigned                    : 1;
    unsigned                    : 1;
    unsigned NOT_WRITE          : 1;
    unsigned                    : 1;
    unsigned                    : 1;
    unsigned NOT_ADDRESS        : 1;
    unsigned                    : 1;
    unsigned                    : 1;
    };

  struct
    {
    unsigned                    : 1;
    unsigned                    : 1;
    unsigned R_W                : 1;
    unsigned                    : 1;
    unsigned                    : 1;
    unsigned D_A                : 1;
    unsigned                    : 1;
    unsigned                    : 1;
    };

  struct
    {
    unsigned                    : 1;
    unsigned                    : 1;
    unsigned READ_WRITE         : 1;
    unsigned                    : 1;
    unsigned                    : 1;
    unsigned DATA_ADDRESS       : 1;
    unsigned                    : 1;
    unsigned                    : 1;
    };
  } __SSPSTATbits_t;

extern __at(0x0094) volatile __SSPSTATbits_t SSPSTATbits;

#define _BF                     0x01
#define _UA                     0x02
#define _R                      0x04
#define _I2C_READ               0x04
#define _NOT_W                  0x04
#define _NOT_WRITE              0x04
#define _R_W                    0x04
#define _READ_WRITE             0x04
#define _S                      0x08
#define _I2C_START              0x08
#define _P                      0x10
#define _I2C_STOP               0x10
#define _D                      0x20
#define _I2C_DATA               0x20
#define _NOT_A                  0x20
#define _NOT_ADDRESS            0x20
#define _D_A                    0x20
#define _DATA_ADDRESS           0x20

//==============================================================================


//==============================================================================
//
//        Configuration Bits
//
//==============================================================================

#define _CONFIG1                0x2007

//----------------------------- CONFIG1 Options -------------------------------

#define _CP_ALL                 0x3F8F  
#define _CP_75                  0x3F9F  
#define _CP_50                  0x3FAF  
#define _CP_OFF                 0x3FBF  
#define _PWRTE_ON               0x3FBF  
#define _PWRTE_OFF              0x3FB7  
#define _WDT_ON                 0x3FBF  
#define _WDT_OFF                0x3FBB  
#define _LP_OSC                 0x3FBC  
#define _XT_OSC                 0x3FBD  
#define _HS_OSC                 0x3FBE  
#define _RC_OSC                 0x3FBF  

//==============================================================================

//==============================================================================

#ifndef NO_BIT_DEFINES

#define CCP1M0                  CCP1CONbits.CCP1M0              // bit 0
#define CCP1M1                  CCP1CONbits.CCP1M1              // bit 1
#define CCP1M2                  CCP1CONbits.CCP1M2              // bit 2
#define CCP1M3                  CCP1CONbits.CCP1M3              // bit 3
#define CCP1Y                   CCP1CONbits.CCP1Y               // bit 4
#define CCP1X                   CCP1CONbits.CCP1X               // bit 5

#define RBIF                    INTCONbits.RBIF                 // bit 0
#define INTF                    INTCONbits.INTF                 // bit 1
#define T0IF                    INTCONbits.T0IF                 // bit 2
#define RBIE                    INTCONbits.RBIE                 // bit 3
#define INTE                    INTCONbits.INTE                 // bit 4
#define T0IE                    INTCONbits.T0IE                 // bit 5
#define PEIE                    INTCONbits.PEIE                 // bit 6
#define GIE                     INTCONbits.GIE                  // bit 7

#define PS0                     OPTION_REGbits.PS0              // bit 0
#define PS1                     OPTION_REGbits.PS1              // bit 1
#define PS2                     OPTION_REGbits.PS2              // bit 2
#define PSA                     OPTION_REGbits.PSA              // bit 3
#define T0SE                    OPTION_REGbits.T0SE             // bit 4
#define T0CS                    OPTION_REGbits.T0CS             // bit 5
#define INTEDG                  OPTION_REGbits.INTEDG           // bit 6
#define NOT_RBPU                OPTION_REGbits.NOT_RBPU         // bit 7

#define NOT_POR                 PCONbits.NOT_POR                // bit 1

#define TMR1IE                  PIE1bits.TMR1IE                 // bit 0
#define TMR2IE                  PIE1bits.TMR2IE                 // bit 1
#define CCP1IE                  PIE1bits.CCP1IE                 // bit 2
#define SSPIE                   PIE1bits.SSPIE                  // bit 3

#define TMR1IF                  PIR1bits.TMR1IF                 // bit 0
#define TMR2IF                  PIR1bits.TMR2IF                 // bit 1
#define CCP1IF                  PIR1bits.CCP1IF                 // bit 2
#define SSPIF                   PIR1bits.SSPIF                  // bit 3

#define SSPM0                   SSPCONbits.SSPM0                // bit 0
#define SSPM1                   SSPCONbits.SSPM1                // bit 1
#define SSPM2                   SSPCONbits.SSPM2                // bit 2
#define SSPM3                   SSPCONbits.SSPM3                // bit 3
#define CKP                     SSPCONbits.CKP                  // bit 4
#define SSPEN                   SSPCONbits.SSPEN                // bit 5
#define SSPOV                   SSPCONbits.SSPOV                // bit 6
#define WCOL                    SSPCONbits.WCOL                 // bit 7

#define BF                      SSPSTATbits.BF                  // bit 0
#define UA                      SSPSTATbits.UA                  // bit 1
#define R                       SSPSTATbits.R                   // bit 2, shadows bit in SSPSTATbits
#define I2C_READ                SSPSTATbits.I2C_READ            // bit 2, shadows bit in SSPSTATbits
#define NOT_W                   SSPSTATbits.NOT_W               // bit 2, shadows bit in SSPSTATbits
#define NOT_WRITE               SSPSTATbits.NOT_WRITE           // bit 2, shadows bit in SSPSTATbits
#define R_W                     SSPSTATbits.R_W                 // bit 2, shadows bit in SSPSTATbits
#define READ_WRITE              SSPSTATbits.READ_WRITE          // bit 2, shadows bit in SSPSTATbits
#define S                       SSPSTATbits.S                   // bit 3, shadows bit in SSPSTATbits
#define I2C_START               SSPSTATbits.I2C_START           // bit 3, shadows bit in SSPSTATbits
#define P                       SSPSTATbits.P                   // bit 4, shadows bit in SSPSTATbits
#define I2C_STOP                SSPSTATbits.I2C_STOP            // bit 4, shadows bit in SSPSTATbits
#define D                       SSPSTATbits.D                   // bit 5, shadows bit in SSPSTATbits
#define I2C_DATA                SSPSTATbits.I2C_DATA            // bit 5, shadows bit in SSPSTATbits
#define NOT_A                   SSPSTATbits.NOT_A               // bit 5, shadows bit in SSPSTATbits
#define NOT_ADDRESS             SSPSTATbits.NOT_ADDRESS         // bit 5, shadows bit in SSPSTATbits
#define D_A                     SSPSTATbits.D_A                 // bit 5, shadows bit in SSPSTATbits
#define DATA_ADDRESS            SSPSTATbits.DATA_ADDRESS        // bit 5, shadows bit in SSPSTATbits

#define C                       STATUSbits.C                    // bit 0
#define DC                      STATUSbits.DC                   // bit 1
#define Z                       STATUSbits.Z                    // bit 2
#define NOT_PD                  STATUSbits.NOT_PD               // bit 3
#define NOT_TO                  STATUSbits.NOT_TO               // bit 4
#define RP0                     STATUSbits.RP0                  // bit 5
#define RP1                     STATUSbits.RP1                  // bit 6
#define IRP                     STATUSbits.IRP                  // bit 7

#define TMR1ON                  T1CONbits.TMR1ON                // bit 0
#define TMR1CS                  T1CONbits.TMR1CS                // bit 1
#define NOT_T1SYNC              T1CONbits.NOT_T1SYNC            // bit 2
#define T1OSCEN                 T1CONbits.T1OSCEN               // bit 3
#define T1CKPS0                 T1CONbits.T1CKPS0               // bit 4
#define T1CKPS1                 T1CONbits.T1CKPS1               // bit 5

#define T2CKPS0                 T2CONbits.T2CKPS0               // bit 0
#define T2CKPS1                 T2CONbits.T2CKPS1               // bit 1
#define TMR2ON                  T2CONbits.TMR2ON                // bit 2
#define TOUTPS0                 T2CONbits.TOUTPS0               // bit 3
#define TOUTPS1                 T2CONbits.TOUTPS1               // bit 4
#define TOUTPS2                 T2CONbits.TOUTPS2               // bit 5
#define TOUTPS3                 T2CONbits.TOUTPS3               // bit 6

#endif // #ifndef NO_BIT_DEFINES

#endif // #ifndef __PIC16C62_H__
