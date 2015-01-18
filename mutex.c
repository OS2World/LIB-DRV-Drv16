/**
 * mutex.c - Simple mutex routines
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

USHORT __watcall MutexLock(PUSHORT handle)
{
  while (DevXchg(handle, 1))
  {
    if (D16g_InitComplete) DevHelp_ProcBlock((ULONG)handle, -1, 0);
    else DevInt3();
  }
  return 1;
}

USHORT __watcall MutexRequest(PUSHORT handle)
{
  if (DevXchg(handle, 1)) return 0;
  return 1;
}

void __watcall MutexUnlock(PUSHORT handle)
{
  DevXchg(handle, 0);
  if (D16g_InitComplete) DevHelp_ProcRun((ULONG)handle);
}

