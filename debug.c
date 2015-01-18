/**
 * debug.c - Debug output routines
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

#define BUFSIZE 256
typedef char _far *va_list;
#define va_start(va, last) va = (va_list) (&last + 1)
#define va_arg(va, type) ((type _far *) (va += sizeof(type)))[-1]
#define va_end(va) va = 0

extern int _vsprintf(char _far *buf, const char *fmt, va_list va);

int D16g_DbgLevel = 1;
USHORT D16g_ComBase = 0; /* default is to the debug buffer */
int D16g_DbgBufWrap = 0;

#define DBG_MEMBUFF_SIZE 0x10000
static char far* DbgMemoryBuffer = NULL;
static unsigned short DbgMemoryWritePos = 0;
static unsigned short DbgMemoryReadPos = 0;
static char TmpBuf[BUFSIZE];

static struct {
  ULONG Baud;
  USHORT Data;
} BaudCodes[] = {
  115200, 0x0001,
  57600, 0x0002,
  38400, 0x0003,
  19200, 0x0006,
  9600, 0x000C,
  4800, 24,
  2400, 48,
  1200, 96,
  600, 192,
  300, 384,
  0, 0 /* end of list */
};

/**
 * Copy debug data to user buffer
 * @param  Buffer user buffer
 * @param  Count  size of user buffer
 * @return bytes copied
 */
unsigned short __watcall dCopyToUser(unsigned char far *Buffer, unsigned short Count)
{
  unsigned short i;

  if (!DbgMemoryBuffer) return 0;

  for (i = 0; i < Count && ( DbgMemoryReadPos != DbgMemoryWritePos ); i++)
  {
    *Buffer++ = DbgMemoryBuffer[DbgMemoryReadPos];
    DbgMemoryReadPos++;
    DbgMemoryReadPos &= (DBG_MEMBUFF_SIZE-1);
  }

  return i;
}

static void dPutString(char *Str)
{
  if (!D16g_ComBase && !DbgMemoryBuffer)
  {
    ULONG PhysAdr;

    PhysAdr = AllocPhysMemory(DBG_MEMBUFF_SIZE);
    if (!PhysAdr) return;

    DbgMemoryBuffer = (char far *)MapPhysToVirt(PhysAdr, DBG_MEMBUFF_SIZE);
    if (!DbgMemoryBuffer) return;

    DbgMemoryWritePos = 0;
    DbgMemoryReadPos = 0;
  }

  for (; *Str != '\0'; Str++) {
    if (!D16g_ComBase)
    {
      /* write the char to the debug buffer */

      /* fill buffer and stop. Comment out this line to keep filling a circular buffer. */
      if ( !D16g_DbgBufWrap && (((DbgMemoryWritePos+1) & (DBG_MEMBUFF_SIZE-1)) == DbgMemoryReadPos) ) return; /* buffer is full */

      DbgMemoryBuffer[DbgMemoryWritePos] = *Str;
      DbgMemoryWritePos++;
      DbgMemoryWritePos &= (DBG_MEMBUFF_SIZE-1);

      /* keep the latest full buffer of information */
      if (DbgMemoryWritePos == DbgMemoryReadPos) DbgMemoryReadPos = (DbgMemoryReadPos+1) & (DBG_MEMBUFF_SIZE-1);
    }
    else
    {
      /* write the char to the serial port */
      while (!(PortInByte(D16g_ComBase + 5) & 0x20));
      PortOutByte(D16g_ComBase, *Str);
    }
  }
}

/**
 * Print messages to serial port
 */
void __watcall dprintf(int level, const char *fmt, ...)
{
  va_list va;

  if(level > D16g_DbgLevel) return;

  va_start(va, fmt);
  _vsprintf(TmpBuf, fmt, va);
  dPutString(TmpBuf);
}

/**
 * Print hex buffer to COM port.
 */
void __watcall dHexDump(const void _far *p, int len, const char *fmt, ...)
{
  va_list va;
  const unsigned char _far *buf = p;
  int i;

  /* print header */
  va_start(va, fmt);
  _vsprintf(TmpBuf, fmt, va);
  dPutString(TmpBuf);

  /* print hex block */
  while (len > 0) {
    dprintf(0, "%Fp ", buf);

    /* print hex block */
    for (i = 0; i < 16; i++) {
      if (i < len) {
        dprintf(0, "%c%02x", ((i == 8) ? '-' : ' '), buf[i]);
      } else {
        dprintf(0, "   ");
      }
    }

    /* print ASCII block */
    dprintf(0, "   ");
    for (i = 0; i < ((len > 16) ? 16 : len); i++) {
      dprintf(0, "%c", (buf[i] >= 32 && buf[i] < 128) ? buf[i] : '.');
    }
    dprintf(0, "\n");

    buf += 16;
    len -= 16;
  }
}

/**
 * Initialize the COM port
 */
void __watcall InitComPort(ULONG BaudRate)
{
  int i;
  USHORT RegData;

  if (D16g_ComBase == 0) return; /* no com port in use */

  /* Find the baud code for the given baud rate */
  for (i = 0; BaudCodes[i].Baud; i++) if (BaudCodes[i].Baud == BaudRate) break;
  if (BaudCodes[i].Baud == 0) i = 0; /* default to 115200 */
  RegData = BaudCodes[i].Data;

  __asm {
    mov     bx,RegData
    mov     dx,D16g_ComBase ; Base address
    cli
    add     dx,3            ; Line Control (+3)
    mov     al,10000000b    ; Set baud flag
    out     dx,al           ; for speed setting

    ; Set Baud
    dec     dx              ; High divisor address
    dec     dx              ; (+1)
    mov     al,bh           ; High divisor value
    out     dx,al           ; set it
    dec     dx              ; Low divisor address (+0)
    mov     al,bl           ; Low divisor value
    out     dx,al           ; set baud rate

    ; Set frame
    mov      al,00000011b   ; Set 8 bit, none, none
    add      dx,3           ; Line Control (+3)
    out      dx,al

    inc      dx
    mov      al,3
    out      dx,al          ; DTR & RTS to High
    sti
  }
}

