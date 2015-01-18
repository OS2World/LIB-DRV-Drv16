/**
 * libc.c - Some libc routines
 *
 * Copyright (c) 2013 David Azarewicz david@88watts.net
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

int __watcall tolower(int ch)
{
  if (ch >= 'A' && ch <= 'Z') ch += ('a' - 'A');
  return ch;
}

int __watcall toupper(int ch)
{
  if (ch >= 'a' && ch <= 'z') ch -= ('a'-'A');
  return ch;
}

short __watcall stricmp(char far *str0, char far *str1)
{
  while(*str0 && *str1 && (toupper(*str0) == toupper(*str1)))
  {
    str0++;
    str1++;
  }

  if (!*str0 && !*str1) return 0;

  return toupper(*str0) - toupper(*str1);
}

long __watcall atol(char far *str)
{
  long mult = 1;
  long res = 0;
  short size, digit;

  if (str == 0) return 0;
  size = strlen(str);
  while (size--)
  {
    digit = str[size] - '0';
    if (digit < 0 || digit > 9) return 0;
    res += digit * mult;
    mult *= 10;
  }
  return res;
}

