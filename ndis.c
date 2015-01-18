/**
 * ndis.c - NDIS interface helper routines
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
#define INCL_DOS
#include "Dev16lib.h"
#include "ndis.h"

static USHORT usIndications;
static USHORT hDev;
static USHORT hLog;
static PSZ pLogMsgFile;

struct MCastBuf AdapterMCB =
{
    NUM_MCADDRS,
    0
};

struct MACSpecChar AdapterSC =
{
    sizeof(struct MACSpecChar), /* u16 MscSize                  Table size                    */
    "DIX+802.3",                /* ach MscType[NAME_LEN]        MAC type name                 */
    6,                          /* u16 MscStnAdrSz;             Station address length        */
    "",                         /* ach MscPermStnAdr[ADDR_LEN]; Permanent station address
                                                                Filled in by DriverInitAdapter() */
    "",                         /* ach MscCurrStnAdr[ADDR_LEN]; Current station address
                                                                Filled in by DriverInitAdapter() */
    0,                          /* u32 MscCurrFncAdr;           Current functional address    */
    NULL,                       /* fd* MscMCp;                  Address of multicast buffer
                                                                Filled in by DriverInitAdapter() */
    1000000000,                 /* u32 MscLinkSpd;              Link speed (bits/sec)
                                                                Updated by the driver module  */
    GDT_ADDRESS_SUPP        |
    MULTIPLE_XFER_DATA_SUPP |
    OPEN_ADAPTER_SUPP       |
    SET_STN_ADDR_SUPP       |
    STAT_ALWAYS_CURR        |
    PROMISCUOUS_SUPP        |
    BROADCAST_SUPP          |
    MULTICAST_SUPP          |
    0,                          /* u32 MscService;              Services supported flags      */
    1514,                       /* u16 MscMaxFrame;             Maximum frame size            */
    (ULONG)1514 * 64,           /* u32 MscTBufCap;              Transmit buffer capacity
                                                                Reasonable default. Actual value filled
                                                                in by DriverInitAdapter() */
    1514,                       /* u16 MscTBlkSz;               Transmit buf alloc block size */
    (ULONG)1514 * 128,          /* u32 MscRBufCap;              Receive buffer capacity
                                                                Reasonable default. Actual value filled
                                                                in by DriverInitAdapter() */
    1514,                       /* u16 MscRBlkSz;               Receive buf alloc block size  */
    { 0xFF, 0xFF, 0xFF},        /* ach MscVenCode[3];           Vendor code                   */
    0,                          /* u8  MscVenAdapter;           Vendor adapter code           */
    NULL,                       /* fd* MscVenAdaptDesc;         Ptr to vendor adapter desc
                                                                Filled in by DriverInitAdapter() */
    0,                          /* u16 MscInterrupt;            Interrupt level used          */
    1,                          /* u16 MscTxQDepth;             Transmit Queue Depth          */
    8                           /* u16 MscMaxDataBlocks;        Maximum number of data blocks */
};

struct MACSpecStat AdapterSS =
{
    sizeof(struct MACSpecStat)
};

static struct MACUprDisp AdapterDT;

struct CommChar AdapterCC =
{
    sizeof(struct CommChar), /* u16 CcSize      */
    0,                       /* u16 CcLvl       */
    0,                       /* u16 CcSpLvl     */
    1,                       /* u8  CcMjrVer    */
    0,                       /* u8  CcMnrVer    */
    0,                       /* u32 CcBindFnc   */
    "",                      /* ach CcName Filled in by NdisDriverProcessParms() */
    1,                       /* u8  CcUPLevel   */
    1,                       /* u8  CcUIType    */
    0,                       /* u8  CcLPLevel   */
    1,                       /* u8  CcLiType    */
    0,                       /* u16 CcModuleID  */
    0,                       /* U16 CcDataSeg   Filled in by NdisRegisterDriver() */
    NULL,                    /* fn* CcSysReq    Filled in by NdisRegisterDriver() */
    NULL,                    /* fd* CcSCp       Filled in by NdisRegisterDriver() */
    NULL,                    /* fd* CcSSp       Filled in by NdisRegisterDriver() */
    NULL,                    /* fd* CcUDp       Filled in by NdisRegisterDriver() */
    NULL,                    /* fd* CcLDp       No lower dispatch table */
    0,                       /* u32 CcRsv1      */
    0                        /* u32 CcRsv2      */
};

struct ProtLwrDisp ProtDT;
struct CommChar ProtCC;

/* CallProtMan
 * Calls the Protocol Manager via an IOCTL and returns the status of that call.
 *
 * Only used internally by this module.
 */
static USHORT CallProtMan(USHORT hPM, struct RqBlk far * RqB)
{
  USHORT stat;

  /* Data, Parm, Function, Category, Device */
  stat = DosDevIOCtl(0, RqB, 0x58, 0x81, hPM);

  if (stat) return stat;

  return RqB->Status;
}

/* GetPMI
 * Calls the Protocol Manager to get the pointer.
 *
 * Only used internally by this module.
 */
static struct ModCfg far * GetPMI(USHORT hPM)
{
  struct RqBlk ReqBlock = {0};

  ReqBlock.Opcode = opGetPMInfo;

  if (CallProtMan(hPM, &ReqBlock)) return 0;

  return (struct ModCfg far *)ReqBlock.Pointer1;
}

/* FindKey
 * Finds the next keyword entry.
 *
 * Called from external driver.
 */
struct KeywordEntry far* __watcall FindKey(struct ModCfg far *pConfig, char *pKey)
{
  struct KeywordEntry far* pEntry;

  pEntry = pConfig->KE;

  do
  {
    if (!stricmp((char far *)pEntry->KeyWord, pKey)) return pEntry;
    pEntry = pEntry->NextKeywordEntry;
  }
  while(pEntry);

  return 0;
}

PCHAR __watcall GetConfigString(struct KeywordEntry far *pEntry)
{
  if (pEntry && pEntry->NumParams >= 1 && pEntry->Params[0].ParamType == 1)
  {
    return (char far *)(&pEntry->Params[1]);
  }
  return "";
}

/* SysReq
 */
USHORT ndiscall SysReq(ULONG dwParam1, ULONG dwParam2, USHORT wParam, USHORT wOper, USHORT wProtDS)
{
  USHORT wRC = GENERAL_FAILURE;
  USHORT wDS = DevSetDS(wProtDS);

  switch(wOper)
  {
    case Bind:
      if (NdisDriverOpen()) break;

      if (!(AdapterCC.CcSSp->MssStatus & MS_BOUND))
      {
        struct CommChar far* pCC       = (struct CommChar far *)dwParam1;
        struct CommChar far* far* ppCC = (struct CommChar far* far*)dwParam2;

        if (!pCC || !pCC->CcLDp || !ppCC) break;

        memcpy(&ProtCC, pCC       , sizeof(struct CommChar));
        memcpy(&ProtDT, pCC->CcLDp, sizeof(struct ProtLwrDisp));

        *ppCC = &AdapterCC;

        AdapterCC.CcSSp->MssStatus |= MS_BOUND;
        wRC = 0;
      }
      break;
  }

  DevSetDS(wDS);
  return wRC;
}

static USHORT AddMCast(PCHAR pAddr)
{
  int i;

  if (AdapterMCB.McbCnt == AdapterMCB.McbMax) return 1;

  for(i = 0; i < AdapterMCB.McbCnt; i++)
  {
    if (!memcmp(AdapterMCB.McbAddrs[i].mAddr, pAddr, 6)) return 1;
  }

  memcpy(AdapterMCB.McbAddrs[AdapterMCB.McbCnt].mAddr, pAddr, 6);

  AdapterMCB.McbCnt++;
  return 0;
}

static USHORT DelMCast(PCHAR pAddr)
{
  USHORT i;

  for (i = 0; i < AdapterMCB.McbCnt; i++)
  {
    if (!memcmp(AdapterMCB.McbAddrs[i].mAddr, pAddr, 6))
    {
      short j;

      for (j = i + 1; j < AdapterMCB.McbCnt; j++)
      {
        AdapterMCB.McbAddrs[j - 1] = AdapterMCB.McbAddrs[j];
      }
      AdapterMCB.McbCnt--;
      return 0;
    }
  }
  return 1;
}

/* GenReq
 */
static USHORT ndiscall GenReq(USHORT Unused1, USHORT Unused2, USHORT wParm, ULONG dwBuf, USHORT wOp, USHORT wAdapterDS)
{
  USHORT wRC = 0;
  USHORT wDS = DevSetDS(wAdapterDS);

  switch(wOp)
  {
  case OpenAdapter:
    if (AdapterCC.CcSSp->MssStatus & MS_OPEN)
    {
      wRC = INVALID_FUNCTION;
    }
    else if (NdisDriverStartIrq())
    {
      wRC = INTERRUPT_CONFLICT;
    }
    else
    {
      AdapterCC.CcSSp->MssStatus |= MS_OPEN | HW_OK;
    }
    break;

  case CloseAdapter:
    if (AdapterCC.CcSSp->MssStatus & MS_OPEN)
    {
      AdapterCC.CcSSp->MssStatus &= ~MS_OPEN;
    }
    else
    {
      wRC = INVALID_FUNCTION;
    }
    break;

  case SetLookAhead:
    if (wParm > AdapterCC.CcSCp->MscMaxFrame)
    {
      wRC = INVALID_PARAMETER;
      break;
    }
    break;

  case SetPacketFilter:
    if (wParm & 0xFFF8)
    {
      wRC = INVALID_PARAMETER;
      break;
    }
    NdisDriverSetMcast(wParm|FLTR_SET_FILTER);
    break;

  case UpdateStatistics:
    NdisDriverGetHwStats();
    break;

  case ClearStatistics:
    break;

  case SetStationAddress:
    if (!(AdapterCC.CcSSp->MssStatus & MS_OPEN))
    {
      memcpy(AdapterCC.CcSCp->MscCurrStnAdr, (PUCHAR)dwBuf, 6);
      NdisDriverSetMac(AdapterCC.CcSCp->MscCurrStnAdr, 6);
    }
    else
    {
      if (!memcmp(AdapterCC.CcSCp->MscCurrStnAdr, (PUCHAR)dwBuf, 6))
      {
        wRC = GENERAL_FAILURE;
      }
    }
    break;

  case AddMulticastAddress:
    if (AddMCast((PCHAR)dwBuf))
    {
      wRC = INVALID_PARAMETER;
    }
    else
    {
      NdisDriverSetMcast(0);
    }
    break;

  case DeleteMulticastAddress:
    if (DelMCast((PCHAR)dwBuf))
    {
      wRC = INVALID_PARAMETER;
    }
    else
    {
      NdisDriverSetMcast(0);
    }
    break;

  default:
    wRC = NOT_SUPPORTED;
    break;
  }

  DevSetDS(wDS);
  return wRC;
}

/* XmitChain
 */
static USHORT ndiscall XmitChain(USHORT Unused1, USHORT Unused2, PUCHAR pBuff, USHORT wAdapterDS)
{
  USHORT wRC = OUT_OF_RESOURCE;
  USHORT wDS = DevSetDS(wAdapterDS);

  if (!pBuff)
  {
    wRC = INVALID_PARAMETER;
  }
  else
  {
    wRC = NdisDriverStartXmit((struct TxBufDesc far *)pBuff);
  }

  DevSetDS(wDS);
  return wRC;
}

/* XferData
 */
static USHORT ndiscall XferData(PUCHAR copied, USHORT frameOffset, PUCHAR pBuff, USHORT wAdapterDS)
{
  USHORT wRC = INVALID_PARAMETER;
  USHORT wDS = DevSetDS(wAdapterDS);

  if (pBuff)
  {
    wRC = NdisDriverXferRx((PUSHORT)copied, frameOffset, (struct TDBufDesc far *)pBuff);
  }

  DevSetDS(wDS);
  return wRC;
}

/* RcvRelease
 */
static USHORT ndiscall RcvRelease(USHORT pos, USHORT wAdapterDS)
{
  USHORT wDS = DevSetDS(wAdapterDS);
  USHORT wRC = NdisDriverReleaseRx(pos);
  DevSetDS(wDS);
  return wRC;
}


static USHORT ndiscall IndicationsOn(USHORT wAdapterDS)
{
  USHORT wRC = 0;
  USHORT wDS = DevSetDS(wAdapterDS);

  NdisIndicationsOn();

  DevSetDS(wDS);
  return wRC;
}

static USHORT ndiscall IndicationsOff(USHORT wAdapterDS)
{
  USHORT wRC = 0;
  USHORT wDS = DevSetDS(wAdapterDS);

  NdisIndicationsOff();

  DevSetDS(wDS);
  return wRC;
}

/* NdisIndicationsOn
 * Can be called by driver and by NDIS stack.
 */
void __watcall NdisIndicationsOn(void)
{
  usIndications--;

  if (!usIndications)
  {
    DevHelp_ProcRun((ULONG)&usIndications);
  }
}

/* NdisIndicationsOff
 * Can be called by driver and by NDIS stack.
 */
void __watcall NdisIndicationsOff(void)
{
  usIndications++;
}

#define NDISINFO_MscMaxFrame 1
#define NDISINFO_MssStatus 2
#define NDISINFO_McbCnt 3
#define NDISINFO_Indications 4
#define NDISINFO_MscLinkSpd 5
ULONG __watcall NdisGetInfo(ULONG ulItem)
{
  switch (ulItem)
  {
  case NDISINFO_MscMaxFrame:
    return AdapterSC.MscMaxFrame;
  case NDISINFO_MssStatus:
    return AdapterSS.MssStatus;
  case NDISINFO_McbCnt:
    return AdapterMCB.McbCnt; /* u16 */
  case NDISINFO_Indications:
    return usIndications;
  case NDISINFO_MscLinkSpd:
    return AdapterSC.MscLinkSpd;
  default:
    return 0;
  }
}

int __watcall NdisInit(PSZ pArgMsgFile)
{
  int rc;
  USHORT Action;
  struct ModCfg far *pConfig;

  pLogMsgFile = pArgMsgFile;

  /* Open the LAN message device driver */
  if (DosOpen("LANMSG$$", &hLog, &Action, 0L, 0, 1, 0x2042, 0L)) hLog = 0;

  if (DosOpen("PROTMAN$", &hDev, &Action, 0L, 0, 0x0001, 0x0091, 0L)) return -3;

  pConfig = GetPMI(hDev);

  rc = 0;
  while(pConfig) {
    rc = NdisDriverProcessParms(pConfig);
    if (rc >= 0) break;
    pConfig = pConfig->NextModCfg;
  }

  return rc;
}

int __watcall NdisRegisterDriver(void)
{
  int rc;
  struct RqBlk ReqBlock = {0};

  /* Setup the structures */
  AdapterCC.CcDataSeg = DevGetDS();
  AdapterCC.CcSCp = &AdapterSC;
  AdapterCC.CcSSp = &AdapterSS;
  AdapterCC.CcUDp = &AdapterDT;
  //AdapterCC.CcLDp = FlatToFar16(NULL);
  AdapterCC.CcSysReq = &SysReq;

  AdapterSC.MscMCp = &AdapterMCB;

  AdapterDT.MudCCp = &AdapterCC;
  AdapterDT.MudGReq = GenReq;
  AdapterDT.MudXmitChain = XmitChain;
  AdapterDT.MudXferData = XferData;
  AdapterDT.MudRcvRelease = RcvRelease;
  AdapterDT.MudIndOn = IndicationsOn;
  AdapterDT.MudIndOff = IndicationsOff;

  ReqBlock.Opcode   = opRegisterModule;
  ReqBlock.Pointer1 = &AdapterCC;
  ReqBlock.Pointer2 = 0;

  rc = CallProtMan(hDev, &ReqBlock);
  DosClose(hDev);

  return rc;
}

/* Log Msg can only be called at init time */
void __watcall NdisLogMsg(USHORT MsgNumber, USHORT Display, USHORT IvCount, PCHAR pIvString)
{
#pragma pack(1)
  struct {
    PUCHAR pCompID;
    PUCHAR far * pIvArray;
    USHORT IvCount;
    USHORT MsgNumber;
    PUCHAR pMsgFile;
    USHORT MsgType;
    USHORT Display;
    PUCHAR pLogPath;
  } LANMsg;
#pragma pack()

  if (MsgNumber==0)
  {
    if (!D16g_InitComplete)
    {
      DosClose(hLog);
      hLog = 0;
    }
    return;
  }

  if (D16g_InitComplete)
  {
    /* Send by IDC */
    /* Still to be written */
  }
  else if (hLog)
  {
    /* send by IOCtl */
    USHORT usStatus;

    LANMsg.pCompID   = NULL; //DEV_NAME;
    LANMsg.pIvArray  = &pIvString;
    LANMsg.IvCount   = IvCount;
    LANMsg.MsgNumber = MsgNumber;
    LANMsg.pMsgFile  = pLogMsgFile;
    LANMsg.MsgType   = 0x0000;
    LANMsg.Display   = Display;
    LANMsg.pLogPath  = NULL;

    DosDevIOCtl( &usStatus, &LANMsg, 1, 0x90, hLog );
  }
}


