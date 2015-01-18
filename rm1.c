/**
 * rm1.c - Some Resource Manager helpers for single adapter drivers
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
#include "rmbase.h"
#include "rmcalls.h"

#define MAX_RESHANDLES  8

static HDRIVER hDriver = 0;
static HADAPTER hAdapter = 0;

static struct
{
  int ctHandles;
  HRESOURCE arHandles[MAX_RESHANDLES];
} ahRes = {0};

void __watcall Rm1Destroy(int iDestroyDriver)
{
  while (ahRes.ctHandles)
  {
    RMDeallocResource(hDriver, ahRes.arHandles[--ahRes.ctHandles]);
  }

  if (hAdapter)
  {
    RMDestroyAdapter(hDriver, hAdapter);
    hAdapter = 0;
  }

  if (hDriver && iDestroyDriver)
  {
    RMDestroyDriver(hDriver);
    hDriver = 0;
  }
}

short __watcall Rm1AddMem(ULONG addr, ULONG length)
{
  int rc;

  if (!hDriver) return -1;
  if (ahRes.ctHandles >= MAX_RESHANDLES) return -1;
  rc = RmAddMem(hDriver, &ahRes.arHandles[ahRes.ctHandles], addr, length);
  if (!rc) ahRes.ctHandles++;
  return rc;
}

short __watcall Rm1AddIrq(USHORT irq, USHORT pin)
{
  int rc;

  if (!hDriver) return -1;
  if (ahRes.ctHandles >= MAX_RESHANDLES) return -1;
  rc = RmAddIrq(hDriver, &ahRes.arHandles[ahRes.ctHandles], irq, pin);
  if (!rc) ahRes.ctHandles++;
  return rc;
}

short __watcall Rm1AddIo(USHORT addr, USHORT length)
{
  int rc;

  if (!hDriver) return -1;
  if (ahRes.ctHandles >= MAX_RESHANDLES) return -1;
  rc = RmAddIo(hDriver, &ahRes.arHandles[ahRes.ctHandles], addr, length);
  if (!rc) ahRes.ctHandles++;
  return rc;
}

short __watcall Rm1CreateAdapter(PADAPTERSTRUCT pAds)
{
  int rc;

  if (!hDriver) return -1;
  if (hAdapter) return -1;

  rc = RMCreateAdapter(hDriver, &hAdapter, pAds, 0, (PAHRESOURCE)&ahRes);
  if (rc == 0)
  {
    ahRes.ctHandles = 0;
  }
  return rc;
}

int __watcall Rm1CreateDriver(PDRIVERSTRUCT pDrs)
{
  if (hDriver) return -1;

  hAdapter = 0;
  return RMCreateDriver(pDrs, &hDriver);
}

