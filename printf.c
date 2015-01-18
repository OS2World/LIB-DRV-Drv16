/**
 * printf.c - A simple printf function
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
#define INCL_DOS
#include "Dev16lib.h"

#define BUFSIZE 256

typedef char _far *va_list;
#define va_start(va, last) va = (va_list) (&last + 1)
#define va_arg(va, type) ((type _far *) (va += sizeof(type)))[-1]
#define va_end(va) va = 0

static char  HexDigits[] = "0123456789abcdef";

/* Convert 'long' to ASCII with the specified base
 */
static void long_to_asc(long val, char _far *buf, int basein, int zero, int flen)
{
  register unsigned long abs_val;
  char tmp[36];
  char _far *ptmp = tmp;
  char _far *s;
  unsigned short base;

  if (basein > 16) {
    strcpy(buf, "[EVAL]");
    return;
  }

  abs_val = (unsigned long) ((val < 0 && basein < 0) ? -val : val);
  tmp[sizeof(tmp) - 1] = '\0';

  base = basein;
  if (basein < 0) base = -basein;
  if (base < 2) base = 10;
  for (s = ptmp + sizeof(tmp) - 2; s > ptmp; s--) {
    *s = HexDigits[abs_val % base];
    flen--;
    if ((abs_val /= base) == 0) break;
  }

  /* left-pad the resulting number with zeros or spaces up to 'flen' */
  while (flen > 0) {
    *(--s) = (zero) ? '0' : ' ';
    flen--;
  }

  /* prepend minus sign if val was negative and basein was negative */
  if (val < 0 && basein < 0) {
    *(--s) = '-';
    flen--;
  }

  strcpy(buf, s);
}

/**
 * Print a formatted message into a string buffer. This is very basic,
 * supporting only strings and integers (16 and 32 bits (l), decimal (d),
 * unsigned (u), and hex (x)). Formatting length modifiers are only supported
 * with a single digit (32-bit numbers don't need more than 9 characters) and
 * an optional '0' in front.
 */
int _vsprintf(char _far *buf, const char *fmt, va_list va)
{
  char _far *orig = buf;
  char _far *s;
  int lmod;
  int fptr;
  int zero;
  int flen;

  for (; *fmt != '\0'; fmt++) {
    switch (*fmt) {

    case '%':
      fmt++;
      zero = flen = 0;
      if (*fmt >= '0' && *fmt <= '9') {
        /* formatting length modifiers */
        zero = (*fmt == '0') ? 1 : 0;
        fmt += zero;
        if ((flen = *fmt - '0') >= 1 && flen <= 9) fmt++;
      }

      /* data type modifiers */
      lmod = (*fmt == 'l') ? 1 : 0;
      fptr = (*fmt == 'F') ? 1 : 0;
      fmt += lmod + fptr;

      switch (*fmt) {

      case 's':
        if (fptr) {
          char _far *p = va_arg(va, char _far *);
          s = (p == 0) ? "[null]" : p;
        } else {
          char *p = va_arg(va, char *);
          s = (p == 0) ? "[null]" : p;
        }
        while ((*buf = *(s++)) != '\0') buf++;
        break;

      case 'c':
        *(buf++) = (char) va_arg(va, int);
        break;

      case 'd':
        long_to_asc((lmod) ? va_arg(va, long) : va_arg(va, int), buf, -10, zero, flen);
        buf += strlen(buf);
        break;

      case 'u':
        long_to_asc((lmod) ? va_arg(va, ULONG) : va_arg(va, USHORT), buf, 10, zero, flen);
        buf += strlen(buf);
        break;

      case 'x':
        long_to_asc((lmod) ? va_arg(va, ULONG) : va_arg(va, USHORT), buf, 16, zero, flen);
        buf += strlen(buf);
        break;

      case 'p':
        if (fptr || lmod) {
          USHORT off = va_arg(va, USHORT);
          USHORT seg = va_arg(va, USHORT);
          long_to_asc(seg, buf, 16, 1, 4);
          buf += strlen(buf);
          *(buf++) = ':';
          long_to_asc(off, buf, 16, 1, 4);
          buf += strlen(buf);
        } else {
          long_to_asc(va_arg(va, USHORT), buf, 16, 1, 4);
          buf += strlen(buf);
        }
        break;

      default:
        *(buf++) = *fmt;
        break;
      }
    break;

    case '\n':
      *(buf++) = '\r';
      *(buf++) = '\n';
      break;

    default:
      *(buf++) = *fmt;
      break;
    }
  }

  *buf = '\0';
  return((int) (buf - orig));
}

/*
 * Print a formatted message into a string buffer. Relies on _vsprintf()
 */
int __watcall sprintf(char _far *buf, const char *fmt, ...)
{
  va_list va;

  va_start(va, fmt);
  return(_vsprintf(buf, fmt, va));
}

#pragma code_seg ("_inittext");
#pragma data_seg ("_initdata","endds");

/*
 * Print a message to the system console. This works only during device driver
 * initialization.
 *
 * NOTE: This function uses a 1K buffer for the resulting message. Thus,
 *       messages should never exceed 1024 bytes...
 */
void __watcall cprintf(const char *fmt, ...)
{
  static char buf[BUFSIZE];
  va_list va;

  va_start(va, fmt);
  _vsprintf(buf, fmt, va);

  DosPutMessage(1, strlen(buf), buf);
}

