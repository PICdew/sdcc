/* z80pst.c

   Copyright (C) 1989-1995 Alan R. Baldwin
   721 Berkeley St., Kent, Ohio 44240

This program is free software; you can redistribute it and/or modify it
under the terms of the GNU General Public License as published by the
Free Software Foundation; either version 3, or (at your option) any
later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>. */

/*
 * Extensions: P. Felber
 */

#include "asxxxx.h"
#include "z80.h"

struct  mne     mne[] = {

        /* machine */

        /* system */

    { NULL,     "CON",          S_ATYP,         0,      A_CON },
    { NULL,     "OVR",          S_ATYP,         0,      A_OVR },
    { NULL,     "REL",          S_ATYP,         0,      A_REL },
    { NULL,     "ABS",          S_ATYP,         0,      A_ABS },
    { NULL,     "NOPAG",        S_ATYP,         0,      A_NOPAG },
    { NULL,     "PAG",          S_ATYP,         0,      A_PAG },


    { NULL,     ".page",        S_PAGE,         0,      0 },
    { NULL,     ".title",       S_HEADER,       0,      O_TITLE },
    { NULL,     ".sbttl",       S_HEADER,       0,      O_SBTTL },
    { NULL,     ".module",      S_MODUL,        0,      0 },
    { NULL,     ".include",     S_INCL,         0,      0 },
    { NULL,     ".area",        S_DAREA,        0,      0 },

    { NULL,     ".org",         S_ORG,          0,      0 },
    { NULL,     ".radix",       S_RADIX,        0,      0 },
    { NULL,     ".globl",       S_GLOBL,        0,      0 },

    { NULL,     ".if",          S_CONDITIONAL,  0,      O_IF },
    { NULL,     ".else",        S_CONDITIONAL,  0,      O_ELSE },
    { NULL,     ".endif",       S_CONDITIONAL,  0,      O_ENDIF },
    { NULL,     ".byte",        S_DATA,         0,      O_1BYTE },
    { NULL,     ".db",          S_DATA,         0,      O_1BYTE },
    { NULL,     ".word",        S_DATA,         0,      O_2BYTE },
    { NULL,     ".dw",          S_DATA,         0,      O_2BYTE },
    { NULL,     ".df",          S_FLOAT,        0,      0 },
    { NULL,     ".blkb",        S_BLK,          0,      1 },
    { NULL,     ".ds",          S_BLK,          0,      1 },
    { NULL,     ".blkw",        S_BLK,          0,      2 },
    { NULL,     ".ascii",       S_ASCIX,        0,      O_ASCII },
    { NULL,     ".ascis",       S_ASCIX,        0,      O_ASCIS },
    { NULL,     ".asciz",       S_ASCIX,        0,      O_ASCIZ },
    { NULL,     ".even",        S_EVEN,         0,      0 },
    { NULL,     ".odd",         S_ODD,          0,      0 },
/* sdas specific */
    { NULL,     ".optsdcc",     S_OPTSDCC,      0,      0 },
/* end sdas specific */

        /* z80 / hd64180 */

    { NULL,     "ld",           S_LD,           0,      0x40 },

    { NULL,     "call",         S_CALL,         0,      0xC4 },
    { NULL,     "jp",           S_JP,           0,      0xC2 },
    { NULL,     "jr",           S_JR,           0,      0x18 },
    { NULL,     "djnz",         S_DJNZ,         0,      0x10 },
    { NULL,     "ret",          S_RET,          0,      0xC0 },

    { NULL,     "bit",          S_BIT,          0,      0x40 },
    { NULL,     "res",          S_BIT,          0,      0x80 },
    { NULL,     "set",          S_BIT,          0,      0xC0 },

    { NULL,     "inc",          S_INC,          0,      0x04 },
    { NULL,     "dec",          S_DEC,          0,      0x05 },

    { NULL,     "add",          S_ADD,          0,      0x80 },
    { NULL,     "adc",          S_ADC,          0,      0x88 },
    { NULL,     "sub",          S_SUB,          0,      0x90 },
    { NULL,     "sbc",          S_SBC,          0,      0x98 },

    { NULL,     "and",          S_AND,          0,      0xA0 },
    { NULL,     "cp",           S_AND,          0,      0xB8 },
    { NULL,     "or",           S_AND,          0,      0xB0 },
    { NULL,     "xor",          S_AND,          0,      0xA8 },

    { NULL,     "ex",           S_EX,           0,      0xE3 },

    { NULL,     "push",         S_PUSH,         0,      0xC5 },
    { NULL,     "pop",          S_PUSH,         0,      0xC1 },

    { NULL,     "in",           S_IN,           0,      0xDB },
    { NULL,     "out",          S_OUT,          0,      0xD3 },

    { NULL,     "rl",           S_RL,           0,      0x10 },
    { NULL,     "rlc",          S_RL,           0,      0x00 },
    { NULL,     "rr",           S_RL,           0,      0x18 },
    { NULL,     "rrc",          S_RL,           0,      0x08 },
    { NULL,     "sla",          S_RL,           0,      0x20 },
    { NULL,     "sra",          S_RL,           0,      0x28 },
    { NULL,     "sll",          S_RL_UNDOCD,    0,      0x30 },
    { NULL,     "srl",          S_RL,           0,      0x38 },

    { NULL,     "rst",          S_RST,          0,      0xC7 },

    { NULL,     "im",           S_IM,           0,      0xED },

    { NULL,     "ccf",          S_INH1,         0,      0x3F },
    { NULL,     "cpl",          S_INH1,         0,      0x2F },
    { NULL,     "daa",          S_INH1,         0,      0x27 },
    { NULL,     "di",           S_INH1,         0,      0xF3 },
    { NULL,     "ei",           S_INH1,         0,      0xFB },
    { NULL,     "exx",          S_INH1,         0,      0xD9 },
    { NULL,     "nop",          S_INH1,         0,      0x00 },
    { NULL,     "halt",         S_INH1,         0,      0x76 },
    { NULL,     "rla",          S_INH1,         0,      0x17 },
    { NULL,     "rlca",         S_INH1,         0,      0x07 },
    { NULL,     "rra",          S_INH1,         0,      0x1F },
    { NULL,     "rrca",         S_INH1,         0,      0x0F },
    { NULL,     "scf",          S_INH1,         0,      0x37 },

    { NULL,     "cpd",          S_INH2,         0,      0xA9 },
    { NULL,     "cpdr",         S_INH2,         0,      0xB9 },
    { NULL,     "cpi",          S_INH2,         0,      0xA1 },
    { NULL,     "cpir",         S_INH2,         0,      0xB1 },
    { NULL,     "ind",          S_INH2,         0,      0xAA },
    { NULL,     "indr",         S_INH2,         0,      0xBA },
    { NULL,     "ini",          S_INH2,         0,      0xA2 },
    { NULL,     "inir",         S_INH2,         0,      0xB2 },
    { NULL,     "ldd",          S_INH2,         0,      0xA8 },
    { NULL,     "lddr",         S_INH2,         0,      0xB8 },
    { NULL,     "ldi",          S_INH2,         0,      0xA0 },
    { NULL,     "ldir",         S_INH2,         0,      0xB0 },
    { NULL,     "neg",          S_INH2,         0,      0x44 },
    { NULL,     "otdr",         S_INH2,         0,      0xBB },
    { NULL,     "otir",         S_INH2,         0,      0xB3 },
    { NULL,     "outd",         S_INH2,         0,      0xAB },
    { NULL,     "outi",         S_INH2,         0,      0xA3 },
    { NULL,     "reti",         S_INH2,         0,      0x4D },
    { NULL,     "retn",         S_INH2,         0,      0x45 },
    { NULL,     "rld",          S_INH2,         0,      0x6F },
    { NULL,     "rrd",          S_INH2,         0,      0x67 },

    { NULL,	".allow_undocumented", X_UNDOCD,	0,	0 },

        /* 64180 */

    { NULL,     ".hd64",        X_HD64,         0,      0 },

    { NULL,     "otdm",         X_INH2,         0,      0x8B },
    { NULL,     "otdmr",        X_INH2,         0,      0x9B },
    { NULL,     "otim",         X_INH2,         0,      0x83 },
    { NULL,     "otimr",        X_INH2,         0,      0x93 },
    { NULL,     "slp",          X_INH2,         0,      0x76 },

    { NULL,     "in0",          X_IN,           0,      0x00 },
    { NULL,     "out0",         X_OUT,          0,      0x01 },

    { NULL,     "mlt",          X_MLT,          0,      0x4C },

    { NULL,     "tst",          X_TST,          0,      0x04 },
    { NULL,     "tstio",        X_TSTIO,        S_EOL,  0x74 }
};
