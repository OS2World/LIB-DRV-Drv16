/**
 * parse1.c - Some command line parsing routines
 *
 * Copyright (c) 2013 David Azarewicz david@88watts.net
 *
 * Parts copied from/inspired by routines in the OS2AHCI driver.
 * Those parts are (c) wherever those were copied from.
 *
 * This file is part of the Drv16 Device Driver Development Kit.
 *
 * Drv16 is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Drv16 is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Drv16.  If not, see <http://www.gnu.org/licenses/>.
 */
#include "Dev16lib.h"

int __watcall ArgCmp(const char far *CmdString, const char far *ArgString)
{
  while (*ArgString && *CmdString)
  {
    if (toupper(*CmdString) != *ArgString) return 0;
    CmdString++;
    ArgString++;
  }

  return(!*ArgString); /* return true if we hit the end of ArgString, false otherwise */
}

/**
 * Convert a string to a long value using the specified base.
 * pString is the string to be converted.
 * pEnd is a pointer to the string where conversion ended. Can be NULL.
 * Base is the base of the conversion. If 0 the function determines the base
        automatically. octal(begins with a 0) or hex(begins with 0x), or decimal.
 */
long __watcall strtol(const char far *pString, const char far * far *pEnd, int Base)
{
  register const char far *s = pString;
  long val = 0;
  int negative = 0;

  /* skip leading whitespace */
  while (*s == ' ' || *s == '\t') s++;

  /* positive or negative */
  if (*s == '-') {
    negative = 1;
    s++;
  } else if (*s == '+') {
    s++;
  }

  /* If the input parameter Base is zero, then we need to
   * determine if it is octal, decimal, or hexadecimal.
   */
  if (Base == 0) {
    if (s[0] == '0') {
      if (toupper(s[1]) == 'X') Base = 16;
      else Base = 8;
    } else {
      Base = 10;
    }
  }

  if (Base < 2 || Base > 36) return 0;
  if ( (Base==16) && (s[0]=='0') && (toupper(s[1])=='X') ) s+=2;
  if ( (Base==8) && (s[0]=='0') ) s++;

  /* convert string to long integer */
  while (*s) {
    int digit = (*s <= '9') ? (*s - '0') : (toupper(*s) - 'A' + 10);
    if (digit < 0 || digit >= Base) break;
    val *= Base;
    val += digit;
    s++;
  }

  if (pEnd != NULL) *pEnd = s;
  if (negative) val = -val;
  return(val);
}

