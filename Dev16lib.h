/**
 * Dev16lib.h - Functions and externals in the Drv16 library
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

#include <devhelp.h>
#include <strategy.h>
#include <rmbase.h>

typedef struct {
  ULONG ulNextDD;
  USHORT usAttribs;
  USHORT pfnStrategy;
  USHORT pfnIDC;
  char abName[8];
  USHORT usDevProtCS;     //Protected mode CS
  USHORT usDevProtDS;     //Protected mode DS
  USHORT usDevRealCS;     //Real mode CS (not used)
  USHORT usDevRealDS;     //Real mode DS (not used)
  ULONG ulCaps;
} DEV_HEADER;

/* Header externals */
extern DEV_HEADER Header;
extern void _TextEnd(void);
extern char _DataEnd;

unsigned char PortInByte(unsigned short);
#pragma aux PortInByte = \
  "in   al,dx" \
  value [al] \
  parm nomemory [dx] \
  modify nomemory exact [al];

void PortOutByte(unsigned short, unsigned char);
#pragma aux PortOutByte = \
  "out  dx,al" \
  parm nomemory [dx] [al] \
  modify nomemory exact [];

unsigned short PortInWord(unsigned short);
#pragma aux PortInWord = \
  "in   ax,dx" \
  value [ax] \
  parm nomemory [dx] \
  modify nomemory exact [ax];

void PortOutWord(unsigned short, unsigned short);
#pragma aux PortOutWord = \
  "out  dx,ax" \
  parm nomemory [dx] [ax] \
  modify nomemory exact [];

unsigned long PortInDword(unsigned short);
#pragma aux PortInDword = \
  "in eax,dx" \
  "mov edx, eax" \
  "shr edx, 16" \
  parm nomemory [dx] value [dx ax] modify nomemory exact [dx ax];

void PortOutDword(unsigned short, unsigned long);
#pragma aux PortOutDword = \
  "movzx eax,ax" \
  "shl edx,16" \
  "or eax,edx" \
  "out dx,eax" \
  parm nomemory [dx] [cx ax] modify nomemory exact [];

unsigned long ReadDword(void far *);
#pragma aux ReadDword = \
  "mov eax, es:[si]" \
  "mov edx, eax" \
  "shr edx, 16" \
  parm caller nomemory [es si] \
  value [dx ax] \
  modify exact [dx ax];

unsigned long WriteDword(void far *, unsigned long);
#pragma aux WriteDword = \
  "movzx   eax,ax" \
  "shl edx,16" \
  "or eax,edx" \
  "mov es:[di],eax" \
  parm caller nomemory [es di] [dx ax] \
  modify exact [dx];

unsigned short ReadWord(void far *);
#pragma aux ReadWord = \
  "mov ax, es:[si]" \
  parm caller nomemory [es si] \
  value [ax] \
  modify exact [ax];

unsigned long WriteWord(void far *, unsigned short);
#pragma aux WriteWord = \
  "mov es:[di],ax" \
  parm caller nomemory [es di] [ax] \
  modify exact [];

unsigned short ReadByte(void far *);
#pragma aux ReadByte = \
  "movzx ax, byte ptr es:[si]" \
  parm caller nomemory [es si] \
  value [ax] \
  modify exact [ax];

unsigned long WriteByte(void far *, unsigned short);
#pragma aux WriteByte = \
  "mov es:[di],al" \
  parm caller nomemory [es di] [al] \
  modify exact [];

USHORT DevGetDS(void);
#pragma aux DevGetDS = "mov ax, ds" value [ax] modify nomemory exact [];

USHORT DevGetCS(void);
#pragma aux DevGetCS = "mov ax, cs" value [ax] modify nomemory exact [];

USHORT DevGetSS(void);
#pragma aux DevGetSS = "mov ax, ss" value [ax] modify nomemory exact [];

void DevInt3(void);
#pragma aux DevInt3 = "int 3" modify nomemory exact [];

void DevCli();
#pragma aux DevCli = "cli" modify nomemory exact [];

void DevSti();
#pragma aux DevSti = "sti" modify nomemory exact [];

void DevClc();
#pragma aux DevClc = "clc" modify nomemory exact [];

void DevStc();
#pragma aux DevStc = "stc" modify nomemory exact [];

USHORT DevXchg(unsigned short far *, USHORT);
#pragma aux DevXchg = "xchg es:[di],ax" parm [es di] [ax] value [ax];

USHORT DevSetDS(USHORT newDS);
#pragma aux DevSetDS = \
  "mov ax, ds" \
  "mov ds, dx" \
  parm [dx] value [ax];

/* Init stuff ***********************************************************/

/* Must be called first thing in the init routine */
int __watcall Drv16Init(PREQPACKET prp);

/* Must be called at init complete */
int __watcall Drv16InitComplete(void);

/* PCI stuff ************************************************************/
#pragma pack(1)
#define MAXCAPCNT       32

typedef struct _PCI_CAP {
  UCHAR pos;
  UCHAR type;
} PCI_CAP, FAR * PPCI_CAP;

typedef struct _PCI_BAR {
  ULONG bar;
  ULONG start;
  ULONG size;
  unsigned io:1;
  unsigned type:2;
  unsigned prefetchable:1;
} PCI_BAR, FAR * PPCI_BAR;

struct pci_dev {
  USHORT BusDevFunc;
  USHORT device;
  USHORT vendor;
  USHORT subsystem_vendor;
  USHORT subsystem_device;
  USHORT status;
  USHORT command;
  USHORT pcie_flags_reg; /* cached PCI-E Capabilities Register */
  UCHAR headertype;
  UCHAR irq;
  UCHAR ipin;
  UCHAR capcnt;
  UCHAR pcie_cap; /* PCI-E capability offset */
  UCHAR pcie_mpss; /* PCI-E Max Payload Size Supported */
  PCI_BAR bars[6];
  PCI_CAP caps[MAXCAPCNT];
  unsigned __int64 features;
};
typedef struct pci_dev PCI_DEVICEINFO, far *PPCI_DEVICEINFO;
#pragma pack()


/* Read a PCI config register
 * BusDevFunc: bits 15-8=Bus, bits 7-3=Device, bits 2-0=Function
 * reg: the register to read
 * size: the size of the read. 1=byte, 2=word, 4=dword
 * *data: a pointer to where to write the result
 * Returns: non-zero on failure.
 */
extern int __watcall PciReadConfig(const USHORT BusDevFunc, const UCHAR reg, const UCHAR size, void far *data);

/* Write a PCI config register
 * BusDevFunc: bits 15-8=Bus, bits 7-3=Device, bits 2-0=Function
 * reg: the register to write
 * size: the size of the write. 1=byte, 2=word, 4=dword
 * data: the data to write
 * Returns: non-zero on failure.
 */
extern int __watcall PciWriteConfig(const USHORT BusDevFunc, const UCHAR reg, const UCHAR size, const ULONG data);

/* Find a PCI device by Vendor:Device ID
 * Vendor: the vendor ID to look for
 * Device: The device ID to look for
 * Index: Which device to return. zero based. 0 finds the first one, 1 finds the second one, etc.
 * Returns: 0xFFFF on failure. the BusDevFunc of the device if success.
 */
extern USHORT __watcall PciFindDevice(USHORT Vendor, USHORT Device, USHORT Index);

/* Find a PCI device by class ID
 * Class: the class ID to look for
 * Index: Which device to return. zero based. 0 finds the first one, 1 finds the second one, etc.
 * Returns: 0xFFFF on failure. the BusDevFunc of the device if success.
 */
extern USHORT __watcall PciFindClass(ULONG Class, USHORT Index);

/* Get the PCI device's configuration information
 * BusDevFunc: bits 15-8=Bus, bits 7-3=Device, bits 2-0=Function
 * pDevInfo: Pointer to the PCI_DEVICEINFO structure where the information is returned.
 * Returns: non-zero on failure.
 */
extern int __watcall PciGetDeviceInfo(USHORT BusDevFunc, PPCI_DEVICEINFO pDevInfo);
extern int __watcall PciSetPowerStateD0(PPCI_DEVICEINFO pDevInfo);
extern int __watcall PciSetPowerStateD3hot(PPCI_DEVICEINFO pDevInfo);
extern int __watcall PciSetBusMaster(PPCI_DEVICEINFO pDevInfo);

/* Memory stuff *********************************************************/

/* Map a physical address to a 16:16 virtual address.
 * This creates a permanent mapping.
 * Adr: The physical memory address to map.
 * Size: The size of the region of memory to map.
 * Returns: the 16:16 virtual address
 */
extern void far * __watcall MapPhysToVirt(ULONG Adr, ULONG Size);

extern ULONG __watcall AllocPhysMemory(ULONG size);

extern ULONG __watcall MapPhysAddressToGdt(USHORT gdt, ULONG address, ULONG length);

/* Resource Manager stuff ***********************************************/

extern short __watcall RmAddMem(HDRIVER hDriver, PHRESOURCE phResource, ULONG addr, ULONG length);
extern short __watcall RmAddIrq(HDRIVER hDriver, PHRESOURCE phResource, USHORT irq, USHORT pin);
extern short __watcall RmAddIo(HDRIVER hDriver, PHRESOURCE phResource, USHORT addr, USHORT length);

/* The Rm1 functions are simplified for drivers that only support one adapter.
 * This avoids the necessity for the driver to manage all the handles.
 */

/* Creates a driver. Must be called first */
extern int __watcall Rm1CreateDriver(PDRIVERSTRUCT pDrs);

/* Destroys everything that was created */
extern void __watcall Rm1Destroy(int iDestroyDriver);

/* Add the specified type of resource */
extern short __watcall Rm1AddMem(ULONG addr, ULONG length);
extern short __watcall Rm1AddIrq(USHORT irq, USHORT pin);
extern short __watcall Rm1AddIo(USHORT addr, USHORT length);

/* Creates the adapter. Must be called after adding the resources. */
extern short __watcall Rm1CreateAdapter(PADAPTERSTRUCT pAds);

/* NDIS functions *******************************************************/

/*************************************************************
 * Required Functions that will be called by the NDIS module *
 *************************************************************/

/* This function is called to setup the interrupt for the driver. */
short __watcall NdisDriverStartIrq();

/* This function is called to start the driver operation. */
short __watcall NdisDriverOpen();

/* This function is called to parse the parameters in the PROTOCOL.INI file. */
short __watcall NdisDriverProcessParms(struct ModCfg far *pConfig);

void __watcall NdisDriverSetMcast(USHORT Flags);

void __watcall NdisDriverGetHwStats();

void __watcall NdisDriverSetMac(UCHAR *Mac, USHORT ulLen);

short __watcall NdisDriverStartXmit(struct TxBufDesc far *pDsc);

short __watcall NdisDriverXferRx(PUSHORT pcopied, USHORT frameOffset, struct TDBufDesc far *pd);

short __watcall NdisDriverReleaseRx(USHORT pos);

/*************************************************************************
 * Services provided by the NDIS module that can be called by a driver.  *
 *************************************************************************/

extern int __watcall NdisInit(PSZ pLogMsgFile);

extern int __watcall NdisRegisterDriver(void);

extern PCHAR __watcall GetConfigString(struct KeywordEntry far *pEntry);

extern struct KeywordEntry far* __watcall FindKey(struct ModCfg far *pConfig, char *pKey);

extern void __watcall NdisIndicationsOn(void);

extern void __watcall NdisIndicationsOff(void);

extern ULONG __watcall NdisRcvChain(UCHAR *pInd, void *pRxBuf, ULONG ulCurRx, ULONG ulLen);

extern ULONG __watcall NdisRcvLkAhead(UCHAR *pInd, void *pRxBuf, ULONG ulLen1, ULONG ulLen2);

extern ULONG __watcall NdisIndicationsComplete(void);

#define NDISINFO_Indications 4
extern ULONG __watcall NdisGetInfo(ULONG ulItem);

extern void __watcall NdisLogMsg(USHORT MsgNumber, USHORT Display, USHORT IvCount, PCHAR pIvString);

/* Tracing functions ****************************************************/

/* Initialize the Trace functions.
 * usMajor defines the major trace code for the driver.
 * Minor trace codes greater than usLevel will not be output.
 */
extern void __watcall TraceInit(USHORT usMajor, USHORT usLevel);
extern void __watcall TraceBuf(USHORT minCode, USHORT DataLen, void far *data);
extern void __watcall TraceArgs(USHORT minCode, USHORT DataLen, ...);

/* Mutex functions ******************************************************/

typedef USHORT Mutex;

extern USHORT __watcall MutexLock(PUSHORT handle);

extern USHORT __watcall MutexRequest(PUSHORT handle);

extern void __watcall MutexUnlock(PUSHORT handle);

/* Debug functions ******************************************************/

/* The debug level threshold for debug messages printed with dprint(). Numbers
 * greater than D16g_DbgLevel will not be printed. Default is 1.
 */
extern int D16g_DbgLevel;

/* Set to non-zero to allow the debug buffer to wrap. If zero, dprintf() output
 * will fill the debug buffer and then do nothing when it is full. This allows
 * you to capture only the first buffer full of data without overwriting it. If
 * you want the last buffer full of data, set this to a non-zero value. Then the
 * buffer will keep filling until you read it. Reading the buffer clears the
 * buffer.
 */
extern int D16g_DbgBufWrap;

/* The base address of the COM port to write data to with dprintf(). If zero
 * data is written to the internal debug buffer.
 */
extern unsigned short D16g_ComBase;
extern void __watcall dprintf(int level, const char *fmt, ...);
extern void __watcall dHexDump(const void _far *p, int len, const char *fmt, ...);

/* Sets the baud rate for the COM port */
extern void __watcall InitComPort(ULONG BaudRate);

/* Copies the internal debug buffer to the specified Buffer address */
extern unsigned short __watcall dCopyToUser(unsigned char far *Buffer, unsigned short Count);

/* Misc functions *******************************************************/

typedef struct {
  volatile ULONG far *pMsCount;
  ULONG Start;
  ULONG End;
} TIMER;

extern struct InfoSegGDT far * D16g_pSysInfoSeg;
extern int D16g_InitComplete;

/* Copies the specified string to the device driver header, enforcing the
 * rules (8 char max, blank padded)
 */
extern void __watcall UtSetDriverName(PSZ pDevName);

/* Modifies the given string assuming it is a device driver name.
 * Finds the first '$'.
 * If iOverwrite is non-zero, the character immediately before the '$'
 * is replaced with '0'+iChar. If iOverwrite is zero,  '0'+iChar is
 * inserted immediately before the '$'. Non-zero status is returned on any
 * failure: iChar is not between 0 and 9, inserting a char would make the
 * name longer than 8 characters, the name is only 1 character. The buffer
 * must be at least 9 bytes long to hold the terminating null character.
 */
extern int __watcall UtModifyName(PSZ pDevName, int iChar, int iOverwrite);

#pragma aux ClearBit parm [ax] [es bx] modify [ax] ;
void ClearBit(USHORT BitNumber, volatile USHORT far *addr);

#pragma aux TestAndSetBit parm [ax] [es bx] value [ax] ;
USHORT TestAndSetBit(USHORT BitNumber, volatile USHORT far *addr);

#pragma aux ClearBitL parm [ax] [es bx] modify [ax] ;
void ClearBitL(USHORT BitNumber, volatile ULONG far * addr);

#pragma aux TestAndSetBitL parm [ax] [es bx] value [ax] ;
USHORT TestAndSetBitL(USHORT BitNumbar, volatile ULONG far * addr);

/* Thise are spin loop functions. Avoid using these if possible.
 * Use ProcBlock instead.
 */
#pragma aux udelay parm [cx] modify [ax cx] ;
extern void udelay(USHORT microseconds);
extern void __watcall mdelay(USHORT milliseconds);

/* This routine is obsolete and will eventually be removed */
extern USHORT __watcall GetHex(PCHAR pString, PUCHAR pResult, USHORT MaxLen);

extern short __watcall GetString(const char far *pStringIn, PCHAR pResult, USHORT MaxLen);

/* Compare the given command line pointer to the given argument string and
 * return true if there is a match. ArgString should be all upper case.
 */
extern int __watcall ArgCmp(const char far *CmdString, const char far *ArgString);

/* Convert a string to a long value using the specified base.
 * pString is the string to be converted.
 * pEnd is a pointer to the string where conversion ended. Can be NULL.
 * Base is the base of the conversion. If 0 the function determines the base
        automatically. octal(begins with a 0) or hex(begins with 0x), or decimal.
 */
extern long __watcall strtol(const char far *pString, const char far * far *pEnd, int Base);

extern ULONG __watcall TimeGetSec(void);
extern ULONG __watcall TimeGetMs(void);
extern void __watcall TimeReset(void);
extern void __watcall TimeInit(void);
extern ULONG __watcall TimeDiff(void);
extern void __watcall msleep(ULONG msecs);
extern void __watcall TimerInit(TIMER far *pTimer, ULONG Milliseconds);
extern int __watcall TimerCheckAndBlock(TIMER far *pTimer);

/* some libc stuff ******************************************************/

/* ctype macros */
#define isupper(ch) ((ch) >= 'A' && (ch) <= 'Z')
#define islower(ch) ((ch) >= 'a' && (ch) <= 'z')
extern int __watcall tolower(int ch);
extern int __watcall toupper(int ch);

#ifndef __size_t
#define __size_t
typedef unsigned long size_t;
//#define size_t unsigned long
#endif

extern short __watcall stricmp(char far *str0, char far *str1);

/* Intrinsics */
extern void far *_fmemchr( const void far *s, short c, size_t n );
extern short _fmemcmp( const void far *s1, const void far *s2, size_t n );
extern void far *_fmemcpy( void far *s1, const void far *s2, size_t n );
extern void far *_fmemset( void far *s, short c, size_t n );

extern char far *_fstrcat( char far *s1, const char far *s2 );
extern short _fstrcmp( const char far *s1, const char far *s2 );
extern char far *_fstrcpy( char far *s1, const char far *s2 );
extern short _fstrlen( const char far *s );

#pragma intrinsic(_fmemchr,_fmemcmp,_fmemcpy,_fmemset)
#pragma intrinsic(_fstrcat,_fstrcmp,_fstrcpy,_fstrlen)

#define memchr _fmemchr
#define memcmp _fmemcmp
#define memcpy _fmemcpy
#define memset _fmemset
#define strcat _fstrcat
#define strcmp _fstrcmp
#define strcpy _fstrcpy
#define strlen _fstrlen

/*
 * Very basic formatted string functions
 * leading zero modifer is supported.
 * single digit length modifier is supported.
 * modifiers are: l=long F=far
 * functions are: s=string, c=char, d=decimal, x=hex, p=pointer
 * Examples:
 * %s = near string
 * %Fs = far string
 * %d = 16 bit decimal
 * %ld = 32 bit decimal
 * %u = 16 bit unsigned decimal
 * %lu = 32 bit unsigned decimal
 * %x = 16 bit hex
 * %lx = 32 bit hex
 * %p = 16 bit pointer
 * %Fp or %lp = 16:16 pointer
 */
extern int __watcall sprintf(char _far *buf, const char *fmt, ...);
extern short __watcall stricmp(char far *str0, char far *str1);
extern long __watcall atol(char far *str);

/* cprintf outputs directly to the console */
extern void __watcall cprintf(const char *fmt, ...);

