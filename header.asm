; Header module for 16 bit drivers
;
; Copyright (c) 2013 David Azarewicz david@88watts.net
;
; This file is part of the Drv16 Device Driver Development Kit.
;
; Drv16 is free software: you can redistribute it and/or modify
; it under the terms of the GNU General Public License as published by
; the Free Software Foundation, either version 3 of the License, or
; (at your option) any later version.
;
; Drv16 is distributed in the hope that it will be useful,
; but WITHOUT ANY WARRANTY; without even the implied warranty of
; MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
; GNU General Public License for more details.
;
; You should have received a copy of the GNU General Public License
; along with Drv16.  If not, see <http://www.gnu.org/licenses/>.

        .686p

include     header.inc

DEVFLAGS        equ DEV_CHAR_DEV or DEV_IOCTL or DEVLEV_3 or DEV_30
DEVCAPS         equ DEV_INITCOMPLETE or DEV_IOCTL2 or DEV_16MB or DEV_SAVERESTORE

_HDRSEG segment dword public use16 'DATA'

; The device header
_Header         label byte
                public  _Header
NextHdr         DD    -1
DrvFlags        DW    DEVFLAGS
MainEntry       DW    _TEXT:StrategyHandler_
IDCEntry        DW    _TEXT:IdcHandler_
DriverName      DB    "DRV16$  "             ; Use UtSetDriverName() to change this
ProtCS          DW    0
ProtDS          DW    0
RealCS          DW    0
RealDS          DW    0
Capabilities    DD    DEVCAPS
Rsvd            DW    0
; End of device header

_HDRSEG ends

;;;;;;;;;;;;;;; You should not change anything below this line ;;;;;;;;;;;;;;;;;;;;

CONST     segment word public use16 'DATA'
CONST     ends

CONST2    segment word public use16 'DATA'
CONST2    ends

_DATA     segment word public use16 'DATA'
          public _Device_Help
_Device_Help dd 0
_DATA     ends

LIBDATA   segment word public use16 'DATA'
LIBDATA   ends

_BSS      segment word public use16 'BSS'
_BSS      ends

_ENDDATA  segment word public use16 'ENDDS'
          public __DataEnd
__DataEnd  label byte
_ENDDATA  ends

_INITDATA segment word public use16 'ENDDS'
_INITDATA ends

_TEXT     segment byte public use16 'CODE'
          extrn StrategyHandler_:near
          extrn IdcHandler_:near
_TEXT     ends

RMCODE    segment dword public use16 'CODE'
RMCODE    ends

LIBCODE   segment dword public use16 'CODE'
LIBCODE   ends

_ENDTEXT  segment byte public use16 'CODE'
          public _TextEnd_
_TextEnd_  label near
_ENDTEXT  ends

_INITTEXT segment byte public use16 'CODE'
_INITTEXT ends

DGROUP    group _HDRSEG, CONST, CONST2, _DATA, LIBDATA, _BSS, _ENDDATA, _INITDATA
CGROUP    GROUP _TEXT, RMCODE, LIBCODE, _ENDTEXT, _INITTEXT

        end
