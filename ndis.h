#ifndef __NDIS_H
#define __NDIS_H

#pragma pack(1)

#define ETH_FCS_LEN     4
#define ETH_ALEN        6
#define ETH_HLEN        14
#define ETH_ZLEN        60
#define ETH_DATA_LEN    1500
#define ETH_FRAME_LEN   1514

struct ethhdr
{
  UCHAR  h_dest[ETH_ALEN];
  UCHAR  h_source[ETH_ALEN];
  USHORT h_proto;
};

#define NAME_LEN                16
#define ADDR_LEN                16
#define ADDR_SIZE            6
#define ETH_HEADER_LEN      14
#define ETH_PROTOCOL        12

#define NUM_MCADDRS  4

/* System request function - opcodes */
#define InitiateBind                    1
#define Bind                                    2
#define UnBind                  5

/* Protocol Manager opcodes */
#define opGetPMInfo                     1
#define opRegisterModule                2
#define opBindAndStart                  3
#define opGetPMLinkage                  4

/* General Request opcodes */
#define InitiateDiagnostics                     1
#define ReadErrorLog                            2
#define SetStationAddress                       3
#define OpenAdapter                             4
#define CloseAdapter                            5
#define ResetMAC                                6
#define SetPacketFilter                         7
#define AddMulticastAddress                     8
#define DeleteMulticastAddress                  9
#define UpdateStatistics                        10
#define ClearStatistics                         11
#define Interrupt                               12
#define SetFunctionalAddress                    13
#define SetLookAhead                            14
                                        /* V3.00.10 NDIS 2.02 enhancement     */
#define UnusedGenReq                  15
#define ModifyOpenParms               16

/* Status Indication opcodes */
#define RingStatus                              1
#define AdapterCheck                            2
#define StartReset                              3
#define InterruptStatus                         4
#define EndReset                                5

/* Return error codes */
#define SUCCESS                         0x0000
#define WAIT_FOR_RELEASE                0x0001
#define REQUEST_QUEUED                  0x0002
#define FRAME_NOT_RECOGNIZED            0x0003
#define FRAME_REJECTED                  0x0004
#define FORWARD_FRAME                   0x0005
#define OUT_OF_RESOURCE                 0x0006
#define INVALID_PARAMETER               0x0007
#define INVALID_FUNCTION                0x0008
#define NOT_SUPPORTED                   0x0009
#define HARDWARE_ERROR                  0x000A
#define TRANSMIT_ERROR                  0x000B
#define NO_SUCH_DESTINATION             0x000C
#define ALREADY_STARTED                 0x0020
#define INCOMPLETE_BINDING              0x0021
#define DRIVER_NOT_INITIALIZED          0x0022
#define HARDWARE_NOT_FOUND              0x0023
#define HARDWARE_FAILURE                0x0024
#define CONFIGURATION_FAILURE           0x0025
#define INTERRUPT_CONFLICT              0x0026
#define INCOMPATIBLE_MAC                0x0027
#define INITIALIZATION_FAILED           0x0028
#define GENERAL_FAILURE                 0x00FF

/* Adapter Check error codes */
#define AdapCheckInoperative            0x8000
#define AdapCheckIllegalOp              0x1000
#define AdapCheckLocalParityErr         0x0800
#define AdapCheckParityErr              0x0400
#define AdapCheckInternalParityErr      0x0100
#define AdapCheckRingXmitParityErr      0x0080
#define AdapCheckRingRcvParityErr       0x0040
#define AdapCheckXmitOverrun            0x0020
#define AdapCheckRcvOverrun             0x0010
#define AdapCheckUnrecogInterrupt       0x0008
#define AdapCheckUnrecogErrInt          0x0004
#define AdapCheckNoPCSysService         0x0003
#define AdapCheckUnrecogSuperReq        0x0002
#define AdapCheckProgramRequest         0x0001

/* protocol levels, interface types */
#define MACLvl                                          1
#define MACTyp                                          1

#define MAJOR_NDIS_VERSION            2
#define MINOR_NDIS_VERSION            0

/*
 * Definintions for the MAC Service Supported Flags.
 * Bits 0-14.
 */
#define BROADCAST_SUPP          (ULONG)0x00000001
#define MULTICAST_SUPP          (ULONG)0x00000002
#define FUNC_GROUP_ADDR_SUPP    (ULONG)0x00000004
#define PROMISCUOUS_SUPP        (ULONG)0x00000008
#define SET_STN_ADDR_SUPP       (ULONG)0x00000010
#define STAT_ALWAYS_CURR        (ULONG)0x00000020
#define DIAGNOSTICS_SUPP        (ULONG)0x00000040
#define LOOPBACK_SUPP           (ULONG)0x00000080
#define RECEIVECHAIN_MOSTLY     (ULONG)0x00000100
#define IBM_SOURCE_R_SUPP       (ULONG)0x00000200
#define RESET_MAC_SUPP          (ULONG)0x00000400
#define OPEN_ADAPTER_SUPP       (ULONG)0x00000800
#define INTERRUPT_RQ_SUPP       (ULONG)0x00001000
#define SRC_RTNG_BRDG_SUPP      (ULONG)0x00002000
#define GDT_ADDRESS_SUPP        (ULONG)0x00004000
#define MULTIPLE_XFER_DATA_SUPP (ULONG)0x00008000
                                        /* V3.00.10 NDIS 2.02 enhancements    */
#define RCVLK_FRAME_SIZE_ZERO   (ULONG)0x00010000
#define MODIFYOPEN_SUPP         (ULONG)0x00020000

/*
 * Packet filter bit definitions.
 */
#define FLTR_DIRECTED   0x01
#define FLTR_BRDCST     0x02
#define FLTR_PRMSCS     0x04
#define FLTR_SRC_RTG    0x08
#define FLTR_SET_FILTER 0x8000

/*
 * MAC status bits in MSS
 * bits 0-2 are coded to reflect the hardware status.
 */
#define HW_MISSING  0x0000
#define HW_BUD_ERR  0x0001
#define HW_CFG_ERR  0x0002
#define HW_FAULT    0x0003
#define HW_SOFT_ERR 0x0004
#define HW_OK       0x0007
#define MS_HW_MASK  HW_OK

#define MS_BOUND    0x0008
#define MS_OPEN     0x0010
#define MS_DIAGS_ON 0x0020

#define M83Sb_RFCRC     1
#define M83Sb_RFFE      2
#define M83Sb_RFMaxSz   4

#define M83Sb_TFColMx   1
#define M83Sb_TFCrr     2
#define M83Sb_TFShrt    4
#define M83Sb_TFOpen    8
#define M83Sb_TFLong    16
#define M83Sb_TFRemFl   32

#define M85Sb_RFCon     1
#define M85Sb_RFCopy    2

#define M85Sb_TFunrun   1
#define M85Sb_TFLine    2
#define M85Sb_TFAbDlm   4
#define M85Sb_TFLost    8
#define M85Sb_TFToken   16

#define MAX_IMMED_LEN   64
#define MAX_DATABLK     8
#define MAX_TX_DATABLK  MAX_DATABLK
#define MAX_TD_DATABLK  MAX_DATABLK
#define MAX_RX_DATABLK  MAX_DATABLK

#define MIN_LOOKAHEAD_DEFAULT   64
#define MAX_LOOKAHEAD           256

#define ndiscall    far pascal

/*
 * multicast address structure is ADDR_LEN bytes ulong
 */
struct MCastAddr
{
  UCHAR mAddr[ADDR_LEN];
};

/*
 * the multicast address buffer manages NUM_MCADDRS multicast address
 * structures.
 */
struct MCastBuf
{
  USHORT McbMax;         /* max # of multicast addresses */
  USHORT McbCnt;         /* curr# "              "       " */

  struct MCastAddr McbAddrs[NUM_MCADDRS];
};

/*
 * MAC Service Specific characteristics table
 */
struct MACSpecChar
{
  USHORT   MscSize;                 /* Table size*/
  char   MscType[NAME_LEN];       /* MAC type name*/
  USHORT   MscStnAdrSz;             /* Station address length */
  UCHAR   MscPermStnAdr[ADDR_LEN]; /* Permanent station address */
  UCHAR   MscCurrStnAdr[ADDR_LEN]; /* Current station address */
  ULONG  MscCurrFncAdr;           /* Current functional address */
  struct MCastBuf far* MscMCp;           /* Address of multicast buffer*/
  ULONG  MscLinkSpd;              /* Link speed (bits/sec)    */
  ULONG  MscService;              /* Services supported flags */
  USHORT   MscMaxFrame;             /* Maximum frame size */
  ULONG  MscTBufCap;              /* Transmit buffer capacity*/
  USHORT   MscTBlkSz;               /* Transmit buf alloc block size*/
  ULONG  MscRBufCap;              /* Receive buffer capacity */
  USHORT   MscRBlkSz;               /* Receive buf alloc block size */
  UCHAR   MscVenCode[3];           /* Vendor code          */
  UCHAR   MscVenAdapter;           /* Vendor adapter code  */
  PCHAR  MscVenAdaptDesc;         /* Ptr to vendor adapter desc   */
  USHORT   MscInterrupt;            /* Interrupt level used */
  USHORT   MscTxQDepth;             /* Transmit Queue Depth */
  USHORT   MscMaxDataBlocks;        /* Maximum number of data blocks   */
};

struct ProtSpecChar
{
  USHORT PscSize;
  char PscType[NAME_LEN];
  USHORT PscTypeCode;
};

/*
 * MAC service-specific status table
 */
struct MACSpecStat
{
  USHORT  MssSize;           /* Table size                        */
  ULONG MssDiagDT;         /* Last diagnostic Date/Time         */
  ULONG MssStatus;         /* MAC status - bit mask             */
  USHORT  MssFilter;         /* Current packet filter             */
  PVOID  MssM8Sp;           /* pointer to Media specific status  */
  ULONG MssClearDT;        /* Last clear stats Date/Time        */
  ULONG MssFR;             /* Frames received: total            */
  ULONG MssRFCRC;          /* Receive fail: CRC error           */
  ULONG MssFRByt;          /* Frames received: total bytes      */
  ULONG MssRFLack;         /* Receive fail: lack of buffers     */
  ULONG MssFRMC;           /* Frames received: multicast        */
  ULONG MssFRBC;           /* Frames received: broadcast        */
  ULONG MssRFErr;          /* rcv fail: errors in general       */
  ULONG MssRFMax;          /* rcv fail: exceeds max size        */
  ULONG MssRFMin;          /* rcv fail: less than min size      */
  ULONG MssFRMCByt;        /* Frames rcvd: multicast bytes      */
  ULONG MssFRBCByt;        /* Frames rcvd: broadcast bytes      */
  ULONG MssRFHW;           /* rcv fail: hardware error          */
  ULONG MssFS;             /* Frames sent: total                */
  ULONG MssFSByt;          /* Frames sent: total bytes          */
  ULONG MssFSMC;           /* Frames sent: multicast            */
  ULONG MssFSBC;           /* Frames sent: broadcast            */
  ULONG MssFSBCByt;        /* Frames sent: broadcast bytes      */
  ULONG MssFSMCByt;        /* Frames sent: multicast bytes      */
  ULONG MssSFTime;         /* Send fail: time-out               */
  ULONG MssSFHW;           /* Send fail: hardware error         */
};

/*
 * 802.3 status table
 */
struct MAC8023Stat
{
  USHORT  M83sSize;      /* Table size                     */
  USHORT  M83sVer;       /* Version                        */
  ULONG M83sRFAln;     /* Receive fail: Alignment error  */
  ULONG M83sRMask;     /* Receive fail bit mask (below)  */
  ULONG M83sRFOvrn;    /* Receive fail: Overrun          */
  ULONG M83sFSCols;    /* Frames sent: after collisions  */
  ULONG M83sFSDfr;     /* Frames sent: after deferring   */
  ULONG M83sSFColMx;   /* Frames not sent: Max collisions*/
  ULONG M83sTotCol;    /* Total collision during tran attempts */
  ULONG M83sTotLCol;   /* Total late collisions          */
  ULONG M83sFSCol1;    /* Frames sent: after 1 collision */
  ULONG M83sFSColM;    /* Frames sent: multpl collisions */
  ULONG M83sFSHrtB;    /* Frames sent: CD heart beat     */
  ULONG M83sJabber;    /* Jabber errors                  */
  ULONG M83sLostCS;    /* Lost carrier sense during tran */
  ULONG M83sTMask;     /* Transmit fail bit mask (below) */
};

/*
 * 802.5 status table
 */
struct MAC8025Stat
{
  USHORT  M85sSize;       /* Table size                       */
  USHORT  M85sVer;        /* Version                          */
  ULONG M85sFCS;        /* FCS error or repeat frame code error */
  ULONG M85sRMask;      /* Receive fail bit mask (below)    */
  ULONG M85s5hbtt;      /* "5 half-bit transition" errors.  */
  ULONG M85sACerr;      /* Access Control errors            */
  ULONG M85sFsAbrtD;    /* Frames sent: w/ abort delimiter  */
  ULONG M85sFsNrcv;     /* Frames sent: Not received.       */
  ULONG M85sFrNoBuf;    /* Frames recognized, no buffers    */
  ULONG M85sFcopyErr;   /* Frame copy errors                */
  ULONG M85sFreqErr;    /* Frequency errors                 */
  ULONG M85sMonRegen;   /* Active Monitor regeneration count.   */
  ULONG M85sRsrvd1;     /* reserved                         */
  ULONG M85sRsrvd2;     /* reserved                         */
  ULONG M85sRsrvd3;     /* reserved                         */
  ULONG M85sTMask;      /* Transmit fail bit mask           */
};

struct CommChar;

/*
 * MAC upper dispatch table
 */
struct MACUprDisp
{
  struct CommChar far *MudCCp;   /* Back pointer to CC table */

  /* MAC entry points, all are assembly functions. */ /* Not sure if this is still true */

  USHORT (ndiscall *MudGReq)      (USHORT, USHORT, USHORT, ULONG, USHORT, USHORT);
  USHORT (ndiscall *MudXmitChain) (USHORT, USHORT, PUCHAR, USHORT);
  USHORT (ndiscall *MudXferData)  (PUCHAR, USHORT, PUCHAR, USHORT);
  USHORT (ndiscall *MudRcvRelease)(USHORT, USHORT);
  USHORT (ndiscall *MudIndOn)     (USHORT);
  USHORT (ndiscall *MudIndOff)    (USHORT);
};

/*
 * protocol lower dispatch table
 */
struct ProtLwrDisp
{
  struct CommChar far *PldCCp;   /* Back pointer to CC table */
  ULONG PldIFF;           /* Interface flags */

  /* Protocol stack entry points. */
  /* NOTE: these functions are defined as returning long, even though
           they return ushort so that h2inc.exe will generate the
           correct sizes for ndis.inc
   */
  USHORT (ndiscall *PldReqConfirm) (USHORT,USHORT,USHORT,USHORT,USHORT,USHORT);
  USHORT (ndiscall *PldXmitConfirm)(USHORT,USHORT,USHORT,USHORT,USHORT);
  USHORT (ndiscall *PldRcvLkAhead) (USHORT,USHORT,USHORT,PUCHAR,PUCHAR,USHORT);
  USHORT (ndiscall *PldIndComplete)(USHORT,USHORT);
  USHORT (ndiscall *PldRcvChain)   (USHORT,USHORT,USHORT,PUCHAR,PUCHAR,USHORT);
  USHORT (ndiscall *PldStatInd)    (USHORT,USHORT,PUCHAR,USHORT,USHORT);
};

/*
 * common characteristics table
 */
struct CommChar
{
  USHORT  CcSize;            /* Table size */
  USHORT  CcLvl;             /* V3.00.10 Major Level of table */
  USHORT  CcSpLvl;           /* Level of spec tables */
  UCHAR  CcMjrVer;          /* V3.00.10 Major Level of table */
  UCHAR  CcMnrVer;          /* V3.00.10 Minor Level of table */
  ULONG CcBindFnc;         /* "Hints" on binding support */
  char  CcName[NAME_LEN];  /* Module name */
  UCHAR  CcUPLevel;         /* Upper protocol level */
  UCHAR  CcUIType;          /* Upper interface type */
  UCHAR  CcLPLevel;         /* Lower protocol level */
  UCHAR  CcLIType;          /* Lower interface type */
  USHORT  CcModuleID;        /* Module ID (supplied by PM) */
  USHORT  CcDataSeg;         /* Module Data Segment */

  USHORT (ndiscall *CcSysReq)    (ULONG, ULONG, USHORT, USHORT, USHORT);          /*  System request function */

  struct MACSpecChar far* CcSCp; /* specific characteristics         */
  struct MACSpecStat far* CcSSp; /* specific status*/
  struct MACUprDisp  far* CcUDp; /* upper dispatch table */
  struct ProtLwrDisp far* CcLDp; /* lower dispatch table */

  ULONG CcRsv1;                                                         /* reserved */
  ULONG CcRsv2;                                                         /* reserved */
};

/*
 * Data structures for TransmitChain (Tx), TransferData (TD), and
 * ReceiveLookahed (Rx).
 */
struct TxDataBlock
{
  UCHAR   TxPtrType;    /* 0 => Physical pointer, 1 => GDT pointer */
  UCHAR   TxRsvdByte;   /* Reserverd, must be zero.    */
  USHORT   TxDataLen;    /* Data block length in bytes. */
  PUCHAR TxDataPtr;      /* Far pointer to data block.  */
};

struct TxBufDesc
{
  USHORT   TxImmedLen;   /* Byte count of immediate data (max = 64) */
  PUCHAR TxImmedPtr;   /* Virtual address of Immediate data       */
  USHORT   TxDataCount;  /* Number of Data Blocks (max = 8)         */

  struct TxDataBlock TxDataBlk[MAX_TX_DATABLK];
};

struct TDDataBlock
{
  UCHAR   TDPtrType;    /* 0 => Physical pointer, 1 => GDT pointer  */
  UCHAR   TDRsvdByte;   /* Reserverd, must be zero.    */
  USHORT   TDDataLen;    /* Data block length in bytes. */
  PUCHAR TDDataPtr;      /* Far pointer to data block.  */
};

struct TDBufDesc
{
  USHORT TDDataCount;  /* Number of Data Blocks (max = 8) */

  struct TDDataBlock TDDataBlk[MAX_TD_DATABLK];
};

struct RxDataBlock
{
  USHORT   RxDataLen;    /* Length of the Data Block */
  PUCHAR RxDataPtr;    /* Far Pointer to Data Block*/
};

struct RxBufDesc
{
  USHORT RxDataCount;   /* Number of Data Blocks.*/

  struct RxDataBlock RxDataBlk[MAX_RX_DATABLK];
};

typedef struct RxBufOne
{
  USHORT RxDataCount;
  struct RxDataBlock RxDataBlk;
} RxBufOne;

/*
 * Data structures for the "Module Configuration" structure parsed from
 * the PROTOCOL.INI file. see NDIS spec for details.
 */
struct Param
{
  USHORT ParamType;  /* 0 => 31 bit signed integer, 1 => string      */
  USHORT ParamLen;   /* String length (including null) or 4.         */
};

struct KeywordEntry
{
  struct KeywordEntry far *NextKeywordEntry;    /* Forward pointer */
  struct KeywordEntry far *PrevKeywordEntry;    /* Back Pointer    */
  char  KeyWord[NAME_LEN];               /* Keyword on left side of "="   */
  USHORT  NumParams;                       /* Number of parameters on r. side of "="       */
  struct Param Params[1];                       /* Actual size depends on NumParams             */
};

struct ModCfg
{
  struct ModCfg far *NextModCfg;  /* Module config images are in a   */
  struct ModCfg far *PrevModCfg;  /* double-linked list.   */

  char ModName[NAME_LEN];  /* Bracketed Module Name.  */

  struct KeywordEntry KE[1];      /* Head of Keyword list, always one */
                             /* or more entries per module. */
};

struct ProIni
{
  struct ModCfg MC[1];            /* Head of Module config list.*/
};

/*
 * request block used to register a module
 */
struct RqBlk
{
  USHORT Opcode;
  USHORT Status;
  PVOID Pointer1;
  PVOID Pointer2;
  USHORT Word1;
};

#define NUMBINDINGS  1

struct BindingsList
{
  USHORT NumBindings;
  struct Module
  {
      char ModuleName[NAME_LEN];

  } BoundDriver[NUMBINDINGS];
};

#pragma pack()

#endif

