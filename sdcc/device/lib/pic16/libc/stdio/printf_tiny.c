/*-----------------------------------------------------------------
    printf_tiny.c - source file for reduced version of printf

    Modified for pic16 port, by Vangelis Rokas, 2004 (vrokas@otenet.gr)
    
    Written By - Sandeep Dutta . sandeep.dutta@usa.net (1999)

    This library is free software; you can redistribute it and/or modify it
    under the terms of the GNU Library General Public License as published by the
    Free Software Foundation; either version 2, or (at your option) any
    later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU Library General Public License for more details.

    You should have received a copy of the GNU Library General Public License
    along with this program; if not, write to the Free Software
    Foundation, 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.

    In other words, you are welcome to use, share and improve this program.
    You are forbidden to forbid anyone else to use, share and improve
    what you give them.   Help stamp out software-hoarding!
-------------------------------------------------------------------------*/

/*
** $Id$
*/

/* This function uses function putchar() to dump a character
 * to standard output. putchar() is defined in libc18f.lib
 * as dummy function, which will be linked if no putchar()
 * function is provided by the user.
 * The user can write his own putchar() function and link it
 * with the source *BEFORE* the libc18f.lib library. This way
 * the linker will link the first function (i.e. the user's function) */

/* following formats are supported :-
   format     output type       argument-type
     %u*       unsigned            *

     %b	       binary
     %d        decimal             int
     %ld       decimal             long
     %hd       decimal             char
     %x        hexadecimal         int
     %lxX      hexadecimal         long
     %hxX      hexadecimal         char
     %o        octal               int
     %lo       octal               long
     %ho       octal               char
     %c        character           char
     %s        character           generic pointer
*/

#include <ctype.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>

#if 0
#define	DPUT(c)	putchar( c )
#else
#define DPUT(c)
#endif

#define ISLONG		(flong)
#define ISSTR		(fstr)
#define ISCHAR		(fchar)
#define HAVESIGN	(nosign)


#if 1
extern void io_long(long);
extern void io_int(long);
extern void io_char(char);
#endif

void printf_tiny(char *fmt, ...) 
{
  char radix;
  char flong, fstr;
  char fchar, nosign;
  char upcase;

  char *str, *ch;
  data char *str1;
  long val;
//  static char buffer[16];
  char buffer[16];
  va_list ap ;

    va_start(ap,fmt);
    ch = fmt;
    
    while( *ch ) {			//for (; *fmt ; fmt++ )
        if (*ch == '%') {
            ISLONG = 0;
            ISSTR = 0;
            ISCHAR = 0;
            HAVESIGN = 0;
            radix = 0;
            upcase = 0;
            ch++;

            if(*ch == 'u') {
              HAVESIGN = 1;
              ch++;
            }
            
            if(*ch == 'l') {
              ISLONG = 1;
              ch++;
            } else
            if(*ch == 'h') {
              ISCHAR = 1;
              ch++;
            }
            
            if(*ch == 's')ISSTR = 1;
            else if(*ch == 'd')radix = 10;
            else if(*ch == 'x'){ radix = 16; upcase = 0; }
            else if(*ch == 'X'){ radix = 16; upcase = 1; }
            else if(*ch == 'c')radix = 0;
            else if(*ch == 'o')radix = 8;
            else if(*ch == 'b')radix = 2;
            
            if(ISSTR) {
                str = va_arg(ap, char *);
                while(*str) { putchar(*str);str++;}
            } else {
              if(ISLONG)val = va_arg(ap, long);
              else
              if(ISCHAR)val = va_arg(ap, char);
              else {
                  val = va_arg(ap, int);
              }

              if(radix) {
                if(HAVESIGN)
                  ultoa(val, buffer, radix);
                else
                  ltoa (val, buffer, radix);

                str1 = buffer;
                while( (*str1) ) {
                  radix = *str1;
                  if(upcase)
                    radix = toupper( radix );
                  putchar ( radix );
                  str1++;
                }
              }	else
                putchar((char)val);
            }
          } else
            putchar(*ch);

        ch++;
    }
}
