/**
 * rm.c - Some Resource Manager helpers
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
#include "rmcalls.h"

ULONG RMFlags = 0;
PFNRM RM_Help0 = 0;
PFNRM RM_Help3 = 0;

short __watcall RmAddMem(HDRIVER hDriver, PHRESOURCE phResource, ULONG addr, ULONG length)
{
  RESOURCESTRUCT ResStruct = {0};

  ResStruct.ResourceType = RS_TYPE_MEM;
  ResStruct.MEMResource.MemBase = addr;
  ResStruct.MEMResource.MemSize = length;
  ResStruct.MEMResource.MemFlags = RS_MEM_EXCLUSIVE;

  return RMAllocResource(hDriver, phResource, &ResStruct);
}

short __watcall RmAddIrq(HDRIVER hDriver, PHRESOURCE phResource, USHORT irq, USHORT pin)
{
  RESOURCESTRUCT  ResStruct = {0};

  ResStruct.ResourceType = RS_TYPE_IRQ;
  ResStruct.IRQResource.IRQLevel = irq;
  ResStruct.IRQResource.PCIIrqPin = pin;
  ResStruct.IRQResource.IRQFlags = RS_IRQ_SHARED;

  return RMAllocResource(hDriver, phResource, &ResStruct);
}

short __watcall RmAddIo(HDRIVER hDriver, PHRESOURCE phResource, USHORT addr, USHORT length)
{
  RESOURCESTRUCT  ResStruct = {0};

  ResStruct.ResourceType = RS_TYPE_IO;
  ResStruct.IOResource.BaseIOPort = addr;
  ResStruct.IOResource.NumIOPorts = length;
  ResStruct.IOResource.IOFlags = RS_IO_EXCLUSIVE;
  ResStruct.IOResource.IOAddressLines = 16;

  return RMAllocResource(hDriver, phResource, &ResStruct);
}

