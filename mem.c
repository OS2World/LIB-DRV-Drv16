/**
 * mem.c - Some memory routines
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

#define PAGESIZE 0x1000

void far * __watcall MapPhysToVirt(ULONG Adr, ULONG Size)
{
  USHORT rc;
  USHORT Gdt;
  USHORT offset;
  ULONG phys;

  if (Size > 0x10000L) return 0;

  rc = DevHelp_AllocGDTSelector((USHORT far*)&Gdt, 1);
  if (rc) return 0;

  offset = (USHORT)(Adr & 0xFFF);
  phys = Adr & 0xFFFFF000;
  Size = (Size + offset + (PAGESIZE - 1)) & 0xFFFFF000;
  rc = DevHelp_PhysToGDTSel(phys, Size, Gdt, (DevGetCS()&3)==0?6:1);
  if (rc)
  {
      DevHelp_FreeGDTSelector(Gdt);
      return 0;
  }
  return (void far *)((((ULONG)Gdt << 16) & 0xFFFF0000) + offset);
}

ULONG __watcall AllocPhysMemory(ULONG size)
{
  USHORT rc;
  ULONG addr = 0;
  ULONG len = (size + (PAGESIZE - 1)) & ~(PAGESIZE - 1);
  rc = DevHelp_AllocPhys(len, 0, (ULONG far *)&addr);
  if (rc)
  {
    rc = DevHelp_AllocPhys(len, 1, (ULONG far *)&addr);
    if (rc) return 0L;
  }
  return addr;
}

ULONG __watcall MapPhysAddressToGdt(USHORT gdt, ULONG address, ULONG length)
{
  USHORT offset;
  ULONG phys;
  ULONG len;

  if (length > 0x10000L) return 0;

  offset = (address & 0xFFF);
  phys = address & 0xFFFFF000;
  len = (length + offset + (PAGESIZE - 1)) & 0xFFFFF000;
  if (DevHelp_PhysToGDTSel(phys, len, gdt, (DevGetCS()&3)==0?6:1)) return 0;
  return (((ULONG)gdt << 16) & 0xFFFF0000) + offset;
}

