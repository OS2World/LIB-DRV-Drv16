/**
 * util.c - Miscellaneous utility routines
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

void __watcall mdelay(USHORT milliseconds)
{
  for( ; milliseconds > 0; milliseconds--) udelay(1000);
}

void __watcall UtSetDriverName(PSZ pDevName)
{
  int i;

  for (i=0; i<8; i++)
  {
    if (*pDevName) Header.abName[i] = toupper(*pDevName++);
    else Header.abName[i] = ' ';
  }
}

int __watcall UtModifyName(PSZ pDevName, int iChar, int iOverwrite)
{
  PCHAR p;
  int i;

  if (iChar < 0 || iChar > 9) return 1;
  for (i=0; i<8; i++) if (pDevName[i] == '$') break;
  if (i < 1) return 1;
  p = &pDevName[i-1];
  if (iOverwrite)
  {
    *p = '0'+iChar;
  }
  else
  {
    if (i > 6) return 1;
    p++;
    *p++ = '0'+iChar;
    *p++ = '$';
    *p = 0;
  }
  return 0;
}

