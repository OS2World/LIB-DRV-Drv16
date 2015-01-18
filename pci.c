/**
 * pci.c - PCI config space access routines
 *
 * Copyright (c) 2013 David Azarewicz david@88watts.net
 *
 * Parts copied from/inspired by routines in the nveth driver.
 * Those parts are Copyright (C) 2007 nickk
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
#define INCL_DOSDEVICES
#include "Dev16lib.h"
#include "pci_regs.h"

#define OEMHLP_QUERY_PCI_BIOS      0x00
#define OEMHLP_FIND_PCI_DEVICE     0x01
#define OEMHLP_FIND_PCI_CLASS      0x02
#define OEMHLP_READ_PCI_CONFIG     0x03
#define OEMHLP_WRITE_PCI_CONFIG    0x04
#define OEMHLP_FUNC_PCI 0x000B
//#define OEMHLP_CATEGORY         0x00

#define BUS_PCI 2
#define PCI_BASE_ADDRESS_0      0x10    /* 32 bits */
#define PCI_BAR(reg)   (UCHAR) (0x10 + (reg) * sizeof(ULONG))

#define PCI_D0           0
#define PCI_D1           1
#define PCI_D2           2
#define PCI_D3hot        3
#define PCI_D3cold       4
#define PCI_UNKNOWN      5
#define PCI_POWER_ERROR (-1)


/******************************************************************************
 * OEMHLP IOCtl parameter union. The parameter area is generally used as input
 * to the OEMHLP IOCtl calls.
 */
#pragma pack(1)
typedef union {

  /* query PCI BIOS information" */
  struct {
    UCHAR    subfunction;
  } bios_info;

  /* find PCI device */
  struct {
    UCHAR    subfunction;
    USHORT   device;
    USHORT   vendor;
    UCHAR    index;
  } find_device;

  /* find PCI class code */
  struct {
    UCHAR    subfunction;
    ULONG    class;
    UCHAR    index;
  } find_class;

  /* read PCI configuration space */
  struct {
    UCHAR    subfunction;
    UCHAR    bus;
    UCHAR    dev_func;
    UCHAR    reg;
    UCHAR    size;
  } read_config;

  /* write PCI configuration space */
  struct {
    UCHAR    subfunction;
    UCHAR    bus;
    UCHAR    dev_func;
    UCHAR    reg;
    UCHAR    size;
    ULONG    data;
  } write_config;

} OH_PARM;

/******************************************************************************
 * OEMHLP IOCtl data union. The data area is generally used as output from the
 * OEMHLP IOCtl calls.
 */
typedef union {

  /* query PCI BIOS information" */
  struct {
    UCHAR    rc;
    UCHAR    hw_mech;
    UCHAR    major_version;
    UCHAR    minor_version;
    UCHAR    last_bus;
  } bios_info;

  /* find PCI device */
  struct {
    UCHAR    rc;
    UCHAR    bus;
    UCHAR    dev_func;
  } find_device;

  /* find PCI class code */
  struct {
    UCHAR    rc;
    UCHAR    bus;
    UCHAR    dev_func;
  } find_class;

  /* read PCI confguration space */
  struct {
    UCHAR    rc;
    ULONG    data;
  } read_config;

  /* write PCI confguration space */
  struct {
    UCHAR    rc;
  } write_config;

} OH_DATA;
#pragma pack()

static USHORT hOemHelp;
static IDCTABLE OemHelpIdc; /* OEMHLP$ IDC entry point */
static OH_PARM OhParm = {0};
static OH_DATA OhData = {0};
static USHORT PciBiosVersion = 0;

/******************************************************************************
 * Call OEMHLP$ IDC entry point with the specified IOCtl parameter and data
 * packets.
 */
static int CallOemHelp(OH_PARM far *pParm, OH_DATA far *pData)
{
  if (OemHelpIdc.ProtIDCEntry == NULL || OemHelpIdc.ProtIDC_DS == 0)
  {
    /* attach to OEMHLP$ device driver */
    if (DevHelp_AttachDD("OEMHLP$ ", (NPBYTE)&OemHelpIdc) || OemHelpIdc.ProtIDCEntry == NULL || OemHelpIdc.ProtIDC_DS == 0)
    {
      return -1;
    }
  }

  if (!hOemHelp)
  {
    USHORT usAction;

    /* open a link to the OEMHlp device driver */
    if (DosOpen("OEMHLP$", &hOemHelp, &usAction, 0L, 0, 1, 0x40, 0L)) return -1;
  }

  if ((DevGetCS() & 0x0003) == 0) /* executing at ring 0 */
  {
    unsigned short prot_idc_ds;
    void (far *func)(void);
    REQPACKET Rp;

    /* assemble IOCtl request */
    memset(&Rp, 0x00, sizeof(Rp));
    Rp.bLength = sizeof(Rp);
    Rp.bUnit = 0;
    Rp.bCommand = STRATEGY_GENIOCTL;
    Rp.usStatus = 0;

    Rp.ioctl.bCategory = IOCTL_OEMHLP;
    Rp.ioctl.bFunction = OEMHLP_FUNC_PCI;
    Rp.ioctl.pvParm = (void far *)pParm;
    Rp.ioctl.pvData = (void far *)pData;
    Rp.ioctl.usParmLen = sizeof(*pParm);
    Rp.ioctl.usDataLen = sizeof(*pData);

    func = OemHelpIdc.ProtIDCEntry;
    prot_idc_ds = OemHelpIdc.ProtIDC_DS;

    _asm {
      push ds;
      push es;
      push bx;
      push si;
      push di;

      push ss
      pop  es
      lea  bx, Rp;
      mov  ds, prot_idc_ds;
      call dword ptr [func];

      pop  di;
      pop  si;
      pop  bx;
      pop  es;
      pop  ds;
    }

    if (Rp.usStatus & RPERR) return -1;
  } /* end of ring 0 */
  else /* executing at ring 3 */
  {
    if (DosDevIOCtl(pData, pParm, OEMHLP_FUNC_PCI, IOCTL_OEMHLP, hOemHelp)) return -1;
  }

  return(pData->bios_info.rc);
}

static int PciInit(void)
{
  if (!PciBiosVersion)
  {
    OhParm.bios_info.subfunction = OEMHLP_QUERY_PCI_BIOS;
    if (CallOemHelp(&OhParm, &OhData)) return -1;
    if (OhData.bios_info.rc) return -1;
    if (OhData.bios_info.major_version < BUS_PCI) return -2;
    PciBiosVersion = OhData.bios_info.major_version;
  }
  return 0;
}

int __watcall PciReadConfig(const USHORT BusDevFunc, const UCHAR reg, const UCHAR size, void far *data)
{
  if (PciInit()) return -1;

  OhParm.read_config.subfunction = OEMHLP_READ_PCI_CONFIG;
  OhParm.read_config.bus = BusDevFunc>>8;
  OhParm.read_config.dev_func = BusDevFunc&0xff;
  OhParm.read_config.reg = reg;
  OhParm.read_config.size = size;

  if (CallOemHelp(&OhParm, &OhData)) return -1;

  switch (size)
  {
    case 1:
      *(UCHAR far *)data = (UCHAR)OhData.read_config.data;
      break;
    case 2:
      *(USHORT far *)data = (USHORT)OhData.read_config.data;
      break;
    case 4:
      *(ULONG far *)data = (ULONG)OhData.read_config.data;
      break;
    default:
      return -1;
  }
  return 0;
}

int __watcall PciWriteConfig(const USHORT BusDevFunc, const UCHAR reg, const UCHAR size, const ULONG data)
{
  if (PciInit()) return -1;

  OhParm.write_config.subfunction = OEMHLP_WRITE_PCI_CONFIG;
  OhParm.write_config.bus = BusDevFunc>>8;
  OhParm.write_config.dev_func = BusDevFunc&0xff;
  OhParm.write_config.reg = reg;
  OhParm.write_config.size = size;
  OhParm.write_config.data = data;

  if (CallOemHelp(&OhParm, &OhData)) return -1;
  return 0;
}

USHORT __watcall PciFindDevice(USHORT Vendor, USHORT Device, USHORT Index)
{
  if (PciInit()) return -1;

  OhParm.find_device.subfunction = OEMHLP_FIND_PCI_DEVICE;
  OhParm.find_device.device = Device;
  OhParm.find_device.vendor = Vendor;
  OhParm.find_device.index = Index;
  if (CallOemHelp(&OhParm, &OhData)) return 0xffff;
  if (OhData.find_device.rc) return 0xffff;
  return ((USHORT)OhData.find_device.bus<<8) | OhData.find_device.dev_func;
}

USHORT __watcall PciFindClass(ULONG Class, USHORT Index)
{
  OhParm.find_device.subfunction = OEMHLP_FIND_PCI_CLASS;
  OhParm.find_class.class = Class;
  OhParm.find_class.index = Index;
  if (CallOemHelp(&OhParm, &OhData)) return 0xffff;
  if (OhData.find_device.rc) return 0xffff;
  return ((USHORT)OhData.find_device.bus<<8) | OhData.find_device.dev_func;
}

#define PCI_FIND_CAP_TTL 48

/* returns zero on success, non-zero on fail */
static int PciFindCaps(PCI_DEVICEINFO far *pDevInfo)
{
    UCHAR pos, id;
    USHORT val16;
    int ttl;

    if ( PciReadConfig(pDevInfo->BusDevFunc, PCI_STATUS, sizeof(val16), &val16)) return -1;
    if (!(val16 & PCI_STATUS_CAP_LIST)) return 0;

    pos = 0;

    switch (pDevInfo->headertype)
    {
    case PCI_HEADER_TYPE_NORMAL:
    case PCI_HEADER_TYPE_BRIDGE:
        pos = PCI_CAPABILITY_LIST;
        break;
    case PCI_HEADER_TYPE_CARDBUS:
        pos = PCI_CB_CAPABILITY_LIST;
        break;
    default :
        return -1;
    }

    ttl = PCI_FIND_CAP_TTL;

    while (ttl--)
    {
        if (PciReadConfig(pDevInfo->BusDevFunc, pos, sizeof(pos), &pos)) break;
        if (pos < 0x40) break;
        pos &= ~3;
        if (PciReadConfig(pDevInfo->BusDevFunc, pos + PCI_CAP_LIST_ID, sizeof(id), &id)) break;
        if (id == 0xff) break;
        if (id == PCI_CAP_ID_EXP) pDevInfo->pcie_cap = pos;
        if (pDevInfo->capcnt < MAXCAPCNT)
        {
            pDevInfo->caps[pDevInfo->capcnt].type = id;
            pDevInfo->caps[pDevInfo->capcnt].pos = pos;
            pDevInfo->capcnt++;
        }
        pos += PCI_CAP_LIST_NEXT;
    }

    if (pDevInfo->pcie_cap)
    {
        PciReadConfig(pDevInfo->BusDevFunc, pDevInfo->pcie_cap + PCI_EXP_FLAGS, sizeof(USHORT), &pDevInfo->pcie_flags_reg);
        PciReadConfig(pDevInfo->BusDevFunc, pDevInfo->pcie_cap + PCI_EXP_DEVCAP, sizeof(USHORT), &pDevInfo->pcie_mpss);
        pDevInfo->pcie_mpss &= PCI_EXP_DEVCAP_PAYLOAD;
    }

    return pDevInfo->capcnt == 0;
}

static UCHAR PciGetCapPos(const UCHAR type, PCI_DEVICEINFO far *pDevInfo)
{
  short i;

  for (i = 0; i < pDevInfo->capcnt; i++)
  {
    if (pDevInfo->caps[i].type == type) return (pDevInfo->caps[i].pos);
  }
  return 0;
}

int __watcall PciSetPowerStateD0(PCI_DEVICEINFO far *pDevInfo)
{
    UCHAR CapPos = PciGetCapPos(PCI_CAP_ID_PM, pDevInfo);
    USHORT pmc, pmcsr, powerstate;

    if (!CapPos) return -1;

    if (PciReadConfig(pDevInfo->BusDevFunc, CapPos + PCI_PM_PMC, sizeof(pmc), &pmc)) return -1;
    if ((pmc & PCI_PM_CAP_VER_MASK) > 3) return -1;
    if (PciReadConfig(pDevInfo->BusDevFunc, CapPos + PCI_PM_CTRL, sizeof(pmcsr), &pmcsr)) return -1;

    powerstate = pmcsr & PCI_PM_CTRL_STATE_MASK;
    if (powerstate == PCI_D0) return 0;

    pmcsr &= ~PCI_PM_CTRL_STATE_MASK;
    if (PciWriteConfig(pDevInfo->BusDevFunc, CapPos + PCI_PM_CTRL, sizeof(pmcsr), pmcsr)) return -1;

    return 0;
}

int __watcall PciSetPowerStateD3hot(PCI_DEVICEINFO far *pDevInfo)
{
    UCHAR CapPos = PciGetCapPos(PCI_CAP_ID_PM, pDevInfo);
    USHORT pmc, pmcsr, powerstate;

    if (!CapPos) return -1;

    if (PciReadConfig(pDevInfo->BusDevFunc, CapPos + PCI_PM_PMC, sizeof(pmc), &pmc)) return -1;
    if ((pmc & PCI_PM_CAP_VER_MASK) > 3) return -1;
    if (PciReadConfig(pDevInfo->BusDevFunc, CapPos + PCI_PM_CTRL, sizeof(pmcsr), &pmcsr)) return -1;

    powerstate = pmcsr & PCI_PM_CTRL_STATE_MASK;
    if (powerstate == PCI_D3hot) return 0;

    pmcsr &= ~PCI_PM_CTRL_STATE_MASK;
    pmcsr |= PCI_D3hot;
    if (PciWriteConfig(pDevInfo->BusDevFunc, CapPos + PCI_PM_CTRL, sizeof(pmcsr), pmcsr)) return -1;

    return 0;
}

int __watcall PciSetBusMaster(PCI_DEVICEINFO far *pDevInfo)
{
  UCHAR lat;

  if (PciReadConfig(pDevInfo->BusDevFunc, PCI_COMMAND, sizeof(pDevInfo->command), &pDevInfo->command)) return -1;
  pDevInfo->command |= PCI_COMMAND_MASTER | PCI_COMMAND_MEMORY;

  if (PciWriteConfig(pDevInfo->BusDevFunc, PCI_COMMAND, sizeof(pDevInfo->command), pDevInfo->command)) return -1;

  if (PciReadConfig(pDevInfo->BusDevFunc, PCI_LATENCY_TIMER, sizeof(lat), &lat)) return -1;
  if (lat < 16)
  {
    lat = 64;
    if (PciWriteConfig(pDevInfo->BusDevFunc, PCI_LATENCY_TIMER, sizeof(lat), lat)) return -1;
  }
  return 0;
}

int __watcall PciGetDeviceInfo(USHORT BusDevFunc, PCI_DEVICEINFO far *pDevInfo)
{
  int i;
  USHORT usTmp;

  memset(pDevInfo, 0, sizeof(*pDevInfo));
  pDevInfo->BusDevFunc = BusDevFunc;

  if (PciReadConfig(BusDevFunc, PCI_VENDOR_ID, sizeof(pDevInfo->vendor), &pDevInfo->vendor)) return -1;
  if (PciReadConfig(BusDevFunc, PCI_DEVICE_ID, sizeof(pDevInfo->device), &pDevInfo->device)) return -1;
  if (PciReadConfig(BusDevFunc, PCI_SUBSYSTEM_VENDOR_ID, sizeof(pDevInfo->subsystem_vendor), &pDevInfo->subsystem_vendor)) return -1;
  if (PciReadConfig(BusDevFunc, PCI_SUBSYSTEM_ID, sizeof(pDevInfo->subsystem_device), &pDevInfo->subsystem_device)) return -1;
  if (PciReadConfig(BusDevFunc, PCI_INTERRUPT_LINE, sizeof(usTmp), &usTmp)) return -1;
  pDevInfo->irq = usTmp & 0xFF;
  pDevInfo->ipin = usTmp >> 8;

  if (PciReadConfig(BusDevFunc, PCI_COMMAND, sizeof(pDevInfo->command), &pDevInfo->command)) return -1;
  if (PciReadConfig(BusDevFunc, PCI_STATUS, sizeof(pDevInfo->status), &pDevInfo->status)) return -1;
  if (PciReadConfig(BusDevFunc, PCI_HEADER_TYPE, sizeof(pDevInfo->headertype), &pDevInfo->headertype)) return -1;
  pDevInfo->headertype &= 0x7F;

  for (i = 0; i < 6; i++)
  {
    USHORT reg;
    ULONG addr, mask, size;

    pDevInfo->bars[i].bar = 0L;
    reg = PCI_BASE_ADDRESS_0 + (i << 2);
    if (PciReadConfig(BusDevFunc, reg, sizeof(long), &addr)) return -1;
    pDevInfo->bars[i].io = (addr & 1);
    pDevInfo->bars[i].type = ((addr >> 1) & 3);
    pDevInfo->bars[i].prefetchable = ((addr >> 3) & 1);
    mask = pDevInfo->bars[i].io ? PCI_BASE_ADDRESS_IO_MASK : PCI_BASE_ADDRESS_MEM_MASK;
    pDevInfo->bars[i].start = addr & mask;
    if (pDevInfo->bars[i].start == 0) continue;
    pDevInfo->bars[i].bar = addr;
    if (PciWriteConfig(BusDevFunc, reg, sizeof(addr), -1L)) continue;
    PciReadConfig(BusDevFunc, reg, sizeof(size), &size);
    pDevInfo->bars[i].size = -(size &= mask);
    PciWriteConfig(BusDevFunc, reg, sizeof(addr), addr);
  }
  PciFindCaps(pDevInfo);
  return 0;
}

// SaveState - save the PCI configuration space of a device before suspending
short __watcall PciSavePciState(USHORT BusDevFunc, ULONG far *pPciSave)
{
  short i;
  for (i = 0; i < 16; i++) PciReadConfig(BusDevFunc, (i * 4), sizeof(ULONG), &pPciSave[i]);
  return 0;
}

// RestoreState - Restore the saved state of a PCI device
short __watcall PciRestorePciState(USHORT BusDevFunc, ULONG far *pPciSave)
{
  short i;
  for (i = 0; i < 16; i++) PciWriteConfig(BusDevFunc, (i * 4), sizeof(ULONG), pPciSave[i]);
  return 0;
}

