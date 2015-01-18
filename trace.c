/**
 * trace.c - Routines to do tracing
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
#include "infoseg.h"

#define TEST_TRACING(PERF_RMT,Maj) ((*(PERF_RMT+(Maj>>3)) & (0x80>>(Maj&7))))

USHORT D16g_TraceLevel = 0x8000;
USHORT D16g_TraceMajor = 0x00FF;

void __watcall TraceInit(USHORT usMajor, USHORT usLevel)
{
  D16g_TraceMajor = usMajor;
  D16g_TraceLevel = usLevel;
}

void __watcall TraceBuf(USHORT minCode, USHORT DataLen, void far *data)
{
  if (minCode > D16g_TraceLevel) return;
  if (D16g_pSysInfoSeg && TEST_TRACING(D16g_pSysInfoSeg->SIS_mec_table, D16g_TraceMajor))
  {
      DevHelp_RAS(D16g_TraceMajor, minCode, DataLen, data);
  }
}

void __watcall TraceArgs(USHORT minCode, USHORT DataLen, ...)
{
  if (minCode > D16g_TraceLevel) return;
  if (D16g_pSysInfoSeg && TEST_TRACING(D16g_pSysInfoSeg->SIS_mec_table, D16g_TraceMajor))
  {
      DevHelp_RAS(D16g_TraceMajor, minCode, DataLen, (void far *)(&DataLen+1));
  }
}

