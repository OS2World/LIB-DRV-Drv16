; misc1.asm
;
; Copyright (c) 2013 David Azarewicz david@88watts.net
;
; Parts copied from/inspired by routines in the OS2AHCI driver.
; Those parts are (c) wherever those were copied from.
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

; Unsigned long divide routine;
; taken from OS/2 Uniaud project, original author: Timur Tabi
__U4D proc near
    public __U4D
    shl     edx,10h            ;; Load dx:ax into eax
    mov     dx,ax
    mov     eax,edx
    xor     edx,edx            ;; Zero extend eax into edx
    shl     ecx,10h            ;; Load cx:bx into ecx
    mov     cx,bx
    div     ecx                ;; Divide eax/ecx into eax
    mov     ecx,edx            ;; Load edx into cx:bx
    shr     ecx,10h
    mov     bx,dx
    mov     edx,eax            ;; Load eax into dx:ax
    shr     edx,10h
    ret
__U4D endp

; Long multiply routine;
; taken from OS/2 Uniaud project, original author: Timur Tabi
__U4M proc    near
    Public __U4M
__I4M label   near
    Public __I4M
    shl     edx,10h            ;; Load dx:ax into eax
    mov     dx,ax
    mov     eax,edx
    mov     dx,cx              ;; Load cx:bx into edx
    shl     edx,10h
    mov     dx,bx
    mul     edx                ;; Multiply eax*edx into edx:eax
    mov     edx,eax            ;; Load eax into dx:ax
    shr     edx,10h
    ret
__U4M endp


; Signed long divide routine;
; taken from OS/2 Uniaud project, original author: Timur Tabi
__I4D proc near
    Public __I4D
    shl     edx,10h            ;; Load dx:ax into eax
    mov     dx,ax
    mov     eax,edx
    cdq                        ;; Sign extend eax into edx
    shl     ecx,10h            ;; Load cx:bx into ecx
    mov     cx,bx
    idiv    ecx                ;; Divide eax/ecx into eax
    mov     ecx,edx            ;; Load edx into cx:bx
    shr     ecx,10h
    mov     bx,dx
    mov     edx,eax            ;; Load eax into dx:ax
    shr     edx,10h
    ret
__I4D endp

; Delay the specified number of microseconds.
; #pragma aux udelay parm [cx] modify exact [ax cx] ;
; void udelay(USHORT microseconds);
align 4                      ; make sure routine aligns on 4 bytes
udelay_ Proc near
    public udelay_
@IODly1:
    mov ax,DOSIODELAYCNT     ; Number of loops for 500ns
    add ax, ax               ; * 2 for number of loops for 1us
@IODly2:
    dec ax
    db 03eh                  ; give branch hint "branch taken" to enforce consistent timing
    jnz short @IODly2
    loop @IODly1
    ret
udelay_ EndP

_TEXT   ends

       End

