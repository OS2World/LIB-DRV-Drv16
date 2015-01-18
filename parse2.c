/**
 * parse2.c - Some protocol.ini parsing routines
 *
 * Copyright (c) 2013 David Azarewicz david@88watts.net
 *
 * Parts copied from/inspired by routines in the nveth driver.
 * Those parts are Copyright (C) 2007 nickk
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

/* Used in ParcConfig */
USHORT __watcall GetHex(PCHAR pString, PUCHAR pResult, USHORT MaxLen)
{
  UCHAR  hValue, converted;
  USHORT numConv;
  short  failed=0;
  USHORT fLength;

  fLength=0; numConv=0; converted=0;
  for (; pString && *pString && !failed; pString++) {
    if (*pString>='0' && *pString<='9') hValue=(UCHAR)(*pString-'0');
    else
    {
      if (*pString>='a' && *pString<='f') hValue=(UCHAR)(*pString-'a'+10);
      else if (*pString>='A' && *pString<='F') hValue=(UCHAR)(*pString-'A'+10);
      else failed=1;
    }
    if (numConv<2)
    {
      converted= (UCHAR)(converted<<4);
      converted|= hValue;
      numConv++;
    }
    if (numConv==2)
    {
      if (fLength<MaxLen)
      {
        *pResult=converted;
        pResult++;
        fLength++;
      } else failed=1;
      converted=0; numConv=0;
    }
  }

  if (numConv) fLength = 0; /* failed */
  if (failed) fLength = 0;
  return(fLength);
}

short __watcall GetString(const char far *pStringIn, PCHAR pResult, USHORT MaxLen)
{
  USHORT Length;

  Length=0;
  if (MaxLen) MaxLen--;
  while ((*pStringIn > ' ') && (Length < MaxLen))
  {
    *pResult++ = *pStringIn++;
    Length++;
  }
  *pResult = 0;
  return(Length);
}

