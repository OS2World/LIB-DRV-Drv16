/**
 * init.c - Driver init routines
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

struct InfoSegGDT far * D16g_pSysInfoSeg = NULL;
int D16g_InitComplete = 0;

int __watcall Drv16InitComplete(void)
{
  D16g_InitComplete = 1;
  return 0;
}

#pragma code_seg ("_inittext");
#pragma data_seg ("_initdata","endds");

int __watcall Drv16Init(PREQPACKET prp)
{
  USHORT far *ppSysInfoSeg;

  Device_Help = prp->init_in.ulDevHlp;

  DevHelp_GetDOSVar(DHGETDOSV_SYSINFOSEG, 0, (PPVOID)&ppSysInfoSeg);
  SELECTOROF(D16g_pSysInfoSeg) = *ppSysInfoSeg;
  OFFSETOF(D16g_pSysInfoSeg) = 0;

  return 0;
}

