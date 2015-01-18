; lock.asm - Some lock based routines
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

        extern DOSIODELAYCNT:ABS

_DATA         segment word public use16 'DATA'
_DATA         ends

_TEXT         segment dword public use16 'CODE'
              assume  CS:_TEXT,DS:_DATA

;; #pragma aux TestAndSetBit parm [ax] [es bx] value [ax] ;
;; USHORT TestAndSetBit(USHORT BitNumbar, volatile USHORT far * addr)
;;
;; Arguments
;;    AX: bit number
;;    ES:BX: memory pointer
;; Returns
;;    AX = 0 if bit not set, 1 if bit set
;; Notes
TestAndSetBit_  proc  near
        public TestAndSetBit_
        and   ax, 000fh            ;; only 16 bits allowed
   lock bts   word ptr es:bx, ax
        setc  al
        xor   ah, ah
        ret
TestAndSetBit_  endp

;; #pragma aux ClearBit parm [ax] [es bx] modify [ax] ;
;; void ClearBit(USHORT BitNumber, volatile USHORT far * addr)
;;
;; Arguments
;;    AX: bit number
;;    ES:BX: memory pointer
;; Returns
;;    nothing
;; Notes
ClearBit_  proc  near
        public ClearBit_
        and   ax, 000fh            ;; only 16 bits allowed
   lock btr   word ptr es:bx, ax
        ret
ClearBit_  endp

;; #pragma aux TestAndSetBitL parm [ax] [es bx] value [ax] ;
;; USHORT TestAndSetBitL(USHORT BitNumbar, volatile ULONG far * addr)
;;
;; Arguments
;;    AX: bit number
;;    ES:BX: memory pointer
;; Returns
;;    AX = 0 if bit not set, 1 if bit set
;; Notes
TestAndSetBitL_  proc  near
        public TestAndSetBitL_
        movzx eax, ax
        and   eax, 001fh            ;; only 32 bits allowed
   lock bts   dword ptr es:bx, eax
        setc  al
        xor   ah, ah
        ret
TestAndSetBitL_  endp

;; #pragma aux ClearBitL parm [ax] [es bx] modify [ax] ;
;; void ClearBitL(USHORT BitNumber, volatile ULONG far * addr)
;;
;; Arguments
;;    AX: bit number
;;    ES:BX: memory pointer
;; Returns
;;    nothing
;; Notes
ClearBitL_  proc  near
        public ClearBitL_
        movzx eax, ax
        and   eax, 001fh            ;; only 32 bits allowed
   lock btr   dword ptr es:bx, eax
        ret
ClearBitL_  endp

_TEXT       ENDS

        End

