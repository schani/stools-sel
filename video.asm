IFDEF MS
  .MODEL SMALL,C
  %OUT Memory Model Small
ELSE
  IFDEF MM
	 .MODEL MEDIUM,C
	 %OUT Memory Model Medium
  ELSE
	 IFDEF MC
		.MODEL COMPACT,C
		SAVE_DATA EQU 1
		%OUT Memory Model Compact
	 ELSE
		.MODEL LARGE,C
		SAVE_DATA EQU 1
		%OUT Memory Model Large
	 ENDIF
  ENDIF
ENDIF

.CODE

  BILDPTR     dd 0B0000000h
  SAVEPTR     dd 000000000h
  COLOR       db 00h
  SEITE       db 00h
  CURSOR1     dw 0000h
  CURSOR2     dw 0000h
  VIRTUAL     db 00h

  ZEILE       dw  0*80, 1*80, 2*80, 3*80, 4*80, 5*80, 6*80, 7*80, 8*80, 9*80
              dw 10*80,11*80,12*80,13*80,14*80,15*80,16*80,17*80,18*80,19*80
              dw 20*80,21*80,22*80,23*80,24*80

  COPYRI      db "Schani Tools (c) 1990-93 by Schani Electronics"

;==========================================================================

ADRESSE MACRO ucX, ucY
  xor  bh,bh
  mov  bl,ucY
  shl  bx,1
  mov  es,word ptr BILDPTR+2
  mov  di,word ptr BILDPTR
  xor  ah,ah
  mov  al,ucX
  add  ax,ZEILE[bx]
  shl  ax,1
  add  di,ax
ENDM

;==========================================================================

  PUBLIC vio_init
  vio_init PROC

  mov  word ptr BILDPTR,0000h
  mov  word ptr BILDPTR+2,0B000h
  mov  ah,0fh
  int  10h
  cmp  al,7
  jz   ENDE
  mov  word ptr BILDPTR+2,0B800h
  mov  byte ptr COLOR,01h
ENDE:
  mov  SEITE,0h
  mov  ah,05h
  mov  al,0h
  int  10h
  ret

  vio_init ENDP

;==========================================================================

  PUBLIC vio_set_page
  vio_set_page PROC ucSeite : BYTE

  cmp  COLOR,0
  je   MONO_SEG
  mov  word ptr BILDPTR+2,0B800h
  jmp  WEITER
MONO_SEG:
  mov  word ptr BILDPTR+2,0B000h
WEITER:
  mov  al,ucSeite
  mov  SEITE,al
  xor  ah,ah
  mov  cl,12
  shl  ax,cl
  mov  word ptr BILDPTR,ax
  mov  ah,05h
  mov  al,SEITE
  int  10h
  ret

  vio_set_page ENDP

;==========================================================================

  PUBLIC vio_get_page
  vio_get_page PROC

  mov  al,SEITE
  ret

  vio_get_page ENDP

;==========================================================================

  PUBLIC vio_set_vir_page
  vio_set_vir_page PROC pvNewZeiger : FAR PTR BYTE

  les  ax,BILDPTR
  mov  [word ptr SAVEPTR],ax
  mov  word ptr [SAVEPTR+2],es
  les  ax,pvNewZeiger
  mov  [word ptr BILDPTR],ax
  mov  word ptr [BILDPTR+2],es
  mov  ah,03
  mov  bh,SEITE
  int  10h
  mov  CURSOR1,dx
  mov  CURSOR2,cx
  mov  ah,1
  mov  VIRTUAL,ah
  ret

  vio_set_vir_page ENDP

;==========================================================================

  PUBLIC vio_set_phys_page
  vio_set_phys_page PROC

  les  ax,SAVEPTR
  mov  [word ptr BILDPTR],ax
  mov  word ptr [BILDPTR+2],es
  mov  ah,2
  mov  bh,SEITE
  mov  dx,CURSOR1
  int  10h
  mov  ah,1
  mov  bh,SEITE
  mov  cx,CURSOR2
  int  10h
  mov  ah,0
  mov  VIRTUAL,ah
  ret

  vio_set_phys_page ENDP

;==========================================================================

  PUBLIC vio_set_cursor_pos
  vio_set_cursor_pos PROC ucX : BYTE, ucY : BYTE

  mov  al,VIRTUAL
  cmp  al,0
  jne  SET_POS_VIR
  mov  ah,02h
  mov  bh,SEITE
  mov  dl,ucX
  dec  dl
  mov  dh,ucY
  dec  dh
  int  10h
  ret
SET_POS_VIR:
  mov  dh,ucY
  dec  dh
  mov  dl,ucX
  dec  dl
  mov  CURSOR1,dx
  ret

  vio_set_cursor_pos ENDP

;==========================================================================

  PUBLIC vio_set_cursor_typ
  vio_set_cursor_typ PROC ucStart : BYTE, ucStop : BYTE

  mov  al,VIRTUAL
  cmp  al,0
  JNE  SET_TYPE_VIRTUAL
  mov  ah,01h
  mov  ch,ucStart
  mov  cl,ucStop
  int  10h
SET_TYPE_VIRTUAL:
  mov  ch,ucStart
  mov  cl,ucStop
  mov  CURSOR2,cx
  ret

  vio_set_cursor_typ ENDP

;==========================================================================

  PUBLIC vio_get_cursor
  vio_get_cursor PROC pucX : PTR BYTE, pucY : PTR BYTE, \
                      pucStart : PTR BYTE, pucStop : PTR BYTE

  mov  ah,03h
  mov  bh,SEITE
  int  10h
  inc  dl
  inc  dh
  IFDEF SAVE_DATA
    push ds
    mov  ah,VIRTUAL
    cmp  ah,0
    jne  SAVE_GET_POS_VIR
    lds  bx,pucX
    mov  [bx],dl
    lds  bx,pucY
    mov  [bx],dh
    jmp  SAVE_GO_ON
SAVE_GET_POS_VIR:
    mov  ax,CURSOR1
    inc  ah
    inc  al
    lds  bx,pucX
    mov  [bx],al
    lds  bx,pucY
    mov  [bx],ah
    mov  ax,CURSOR2
    lds  bx,pucStart
    mov  [bx],ah
    lds  bx,pucStop
    mov  [bx],al
SAVE_GO_ON:
    lds  bx,pucStart
    mov  [bx],ch
    lds  bx,pucStop
    mov  [bx],cl
    pop  ds
  ELSE
    mov  ah,VIRTUAL
    cmp  ah,0
    jne  GET_POS_VIR
    mov  bx,pucX
    mov  [bx],dl
    mov  bx,pucY
    mov  [bx],dh
    jmp  GO_ON
GET_POS_VIR:
    mov  ax,CURSOR1
    inc  ah
    inc  al
    mov  bx,pucX
    mov  [bx],al
    mov  bx,pucY
    mov  [bx],ah
    mov  ax,CURSOR2
    mov  bx,pucStart
    mov  [bx],ah
    mov  bx,pucStop
    mov  [bx],al
GO_ON:
    mov  bx,pucStart
    mov  [bx],ch
    mov  bx,pucStop
    mov  [bx],cl
  ENDIF
  ret

  vio_get_cursor ENDP

;==========================================================================

  PUBLIC vio_set_mode
  vio_set_mode PROC ucModus : BYTE

  mov  ah,00h
  mov  al,ucModus
  int  10h
  call vio_init
  ret

  vio_set_mode ENDP

;==========================================================================

  PUBLIC vio_get_mode
  vio_get_mode PROC

  mov  ah,0fh
  int  10h
  ret

  vio_get_mode ENDP

;==========================================================================

  PUBLIC vio_scroll_up
  vio_scroll_up PROC ucX : BYTE, ucY : BYTE, ucBreite : BYTE, \
                     ucHoehe : BYTE, ucAttri : BYTE, ucAnzahl : BYTE

  dec  ucX
  dec  ucY
  mov  ah,06h
  mov  al,ucAnzahl
  mov  cl,ucX
  mov  ch,ucY
  mov  dl,ucBreite
  mov  dh,ucHoehe
  add  dl,cl
  add  dh,ch
  dec  dl
  dec  dh
  mov  bh,ucAttri
  int  10h
  ret

  vio_scroll_up ENDP

;==========================================================================

  PUBLIC vio_scroll_down
  vio_scroll_down PROC ucX : BYTE, ucY : BYTE, ucBreite : BYTE, \
                       ucHoehe : BYTE, ucAttri : BYTE, ucAnzahl : BYTE

  dec  ucX
  dec  ucY
  mov  ah,07h
  mov  al,ucAnzahl
  mov  cl,ucX
  mov  ch,ucY
  mov  dl,ucBreite
  mov  dh,ucHoehe
  add  dl,cl
  add  dh,ch
  dec  dl
  dec  dh
  mov  bh,ucAttri
  int  10h
  ret

  vio_scroll_down ENDP

;==========================================================================

  PUBLIC vio_l_a
  vio_l_a PROC USES di, ucX : BYTE, ucY : BYTE

  dec  ucX
  dec  ucY
  ADRESSE ucX,ucY
  inc  di
  mov  al,es:[di]
  ret

  vio_l_a ENDP

;==========================================================================

  PUBLIC vio_l_z
  vio_l_z PROC USES di, ucX : BYTE, ucY : BYTE

  dec  ucX
  dec  ucY
  ADRESSE ucX,ucY
  mov  al,es:[di]
  ret

  vio_l_z ENDP

;==========================================================================

  PUBLIC vio_l_za
  vio_l_za PROC USES di, ucX : BYTE, ucY : BYTE

  dec  ucX
  dec  ucY
  ADRESSE ucX,ucY
  mov  ax,es:[di]
  ret

  vio_l_za ENDP

;==========================================================================

  PUBLIC vio_s_a
  vio_s_a PROC USES di, ucX : BYTE, ucY : BYTE, ucAttri : BYTE

  dec  ucX
  dec  ucY
  ADRESSE ucX,ucY
  mov  al,ucAttri
  mov  es:[di+1],al
  ret

  vio_s_a ENDP

;==========================================================================

  PUBLIC vio_s_z
  vio_s_z PROC USES di, ucX : BYTE, ucY : BYTE, ucZeichen : BYTE

  dec  ucX
  dec  ucY
  ADRESSE ucX,ucY
  mov  al,ucZeichen
  mov  es:[di],al
  ret

  vio_s_z ENDP

;==========================================================================

  PUBLIC vio_s_za
  vio_s_za PROC USES di, \
           ucX : BYTE, ucY : BYTE, ucZeichen : BYTE, ucAttri : BYTE

  dec  ucX
  dec  ucY
  ADRESSE ucX,ucY
  mov  ah,ucAttri
  mov  al,ucZeichen
  mov  es:[di],ax
  ret

  vio_s_za ENDP

;==========================================================================

  PUBLIC vio_sp_a
  vio_sp_a PROC ucAttri : BYTE

  mov al,ucAttri
  les bx,BILDPTR
  inc bx
  mov cx,2000
A_SCHREIBEN:
  mov es:[bx],al
  inc bx
  inc bx
  loop A_SCHREIBEN
  ret

  vio_sp_a ENDP

;==========================================================================

  PUBLIC vio_sp_z
  vio_sp_z PROC ucZeichen : BYTE

  mov al,ucZeichen
  les bx,BILDPTR
  mov cx,2000
Z_SCHREIBEN:
  mov es:[bx],al
  inc bx
  inc bx
  loop Z_SCHREIBEN
  ret

  vio_sp_z ENDP

;==========================================================================

  PUBLIC vio_sp_za
  vio_sp_za PROC USES di, ucZeichen : BYTE, ucAttri : BYTE

  mov  ah,ucAttri
  mov  al,ucZeichen
  les  di,dword ptr BILDPTR
  mov  cx,2000
  cld
  rep  stosw
  ret

  vio_sp_za ENDP

;==========================================================================

  PUBLIC vio_ss
  vio_ss PROC USES si di, ucX : BYTE, ucY : BYTE, pucString : PTR BYTE

  dec  ucX
  dec  ucY
  ADRESSE ucX,ucY
  IFDEF SAVE_DATA
    push ds
    lds  si,pucString
  ELSE
    mov  si,pucString
  ENDIF
S_SCHREIBEN:
  mov  ah,[BYTE PTR si]
  cmp  ah,0
  jz   ENDE_SS
  mov  BYTE PTR es:[di],ah
  inc  di
  inc  di
  inc  si
  jmp  S_SCHREIBEN
ENDE_SS:
  IFDEF SAVE_DATA
    pop  ds
  ENDIF
  ret

  vio_ss ENDP

;==========================================================================

  PUBLIC vio_ss_a
  vio_ss_a PROC USES si di, ucX : BYTE, ucY : BYTE, pucString : PTR BYTE, \
                            ucAttri : BYTE

  dec  ucX
  dec  ucY
  ADRESSE ucX,ucY
  mov  ah,ucAttri
  IFDEF SAVE_DATA
    push ds
    lds  si,pucString
  ELSE
    mov  si,pucString
  ENDIF
S_A_SCHREIBEN:
  mov  al,[BYTE PTR si]
  cmp  al,0
  je   ENDE_SS_A
  mov  WORD PTR es:[di],ax
  inc  di
  inc  di
  inc  si
  jmp  S_A_SCHREIBEN
ENDE_SS_A:
  IFDEF SAVE_DATA
    pop  ds
  ENDIF
  ret

  vio_ss_a ENDP

;==========================================================================

  PUBLIC vio_sw_a
  vio_sw_a PROC USES di, ucX : BYTE, ucY : BYTE, ucBreite : BYTE, \
                         ucHoehe : BYTE, ucAttri : BYTE

  dec  ucX
  dec  ucY
  ADRESSE ucX,ucY
  inc  di
  xor  bh,bh
  mov  bl,80
  sub  bl,ucBreite
  shl  bl,1
  mov  al,ucAttri
  xor  ch,ch
  xor  dh,dh
  mov  cl,ucBreite
  cmp  cl,0
  je   W_A_ENDE
  mov  dl,ucHoehe
  cmp  dl,0
  je   W_A_ENDE
W_A_SCHREIBEN:
  mov  BYTE PTR es:[di],al
  inc  di
  inc  di
  loop W_A_SCHREIBEN
  add  di,bx
  dec  dx
  mov  cl,ucBreite
  jnz  W_A_SCHREIBEN
W_A_ENDE:
  ret

  vio_sw_a ENDP

;==========================================================================

  PUBLIC vio_sw_z
  vio_sw_z PROC USES di, ucX : BYTE, ucY : BYTE, ucBreite : BYTE, \
                         ucHoehe : BYTE, ucZeichen : BYTE

  dec  ucX
  dec  ucY
  ADRESSE ucX,ucY
  xor  bh,bh
  mov  bl,80
  sub  bl,ucBreite
  shl  bl,1
  mov  al,ucZeichen
  xor  ch,ch
  xor  dh,dh
  mov  cl,ucBreite
  cmp  cl,0
  je   W_Z_ENDE
  mov  dl,ucHoehe
  cmp  dl, 0
  je   W_Z_ENDE
W_Z_SCHREIBEN:
  mov  BYTE PTR es:[di],al
  inc  di
  inc  di
  loop W_Z_SCHREIBEN
  add  di,bx
  dec  dx
  mov  cl,ucBreite
  jnz  W_Z_SCHREIBEN
W_Z_ENDE:
  ret

  vio_sw_z ENDP

;==========================================================================

  PUBLIC vio_sw_za
  vio_sw_za PROC USES di, ucX : BYTE, ucY : BYTE, ucBreite : BYTE, \
                          ucHoehe : BYTE, ucZeichen : BYTE, ucAttri : BYTE

  dec  ucX
  dec  ucY
  ADRESSE ucX,ucY
  xor  bh,bh
  mov  bl,80
  sub  bl,ucBreite
  shl  bl,1
  mov  al,ucZeichen
  mov  ah,ucAttri
  xor  ch,ch
  xor  dh,dh
  mov  cl,ucBreite
  cmp  cl,0
  je   W_ZA_ENDE
  mov  dl,ucHoehe
  cmp  dl,0
  je   W_ZA_ENDE
W_ZA_SCHREIBEN:
  mov  WORD PTR es:[di],ax
  inc  di
  inc  di
  loop W_ZA_SCHREIBEN
  add  di,bx
  dec  dx
  mov  cl,ucBreite
  jnz  W_ZA_SCHREIBEN
W_ZA_ENDE:
  ret

  vio_sw_za ENDP

;==========================================================================

  PUBLIC vio_lw
  vio_lw PROC USES si di, ucX : BYTE, ucY : BYTE, ucBreite : BYTE, \
                          ucHoehe : BYTE, puiBuffer : PTR WORD

  dec  ucX
  dec  ucY
  ADRESSE ucX,ucY
  mov  si,di
  xor  bh,bh
  mov  bl,80
  sub  bl,ucBreite
  shl  bl,1
  xor  ch,ch
  mov  cl,ucBreite
  cmp  cl,0
  je   W_LESEN_ENDE
  xor  dh,dh
  mov  dl,ucHoehe
  cmp  cl,0
  je   W_LESEN_ENDE
  IFDEF SAVE_DATA
    push ds
    lds  di,puiBuffer
  ELSE
    mov  di,puiBuffer
  ENDIF
  push cx
W_LESEN:
  mov  ax,WORD PTR es:[si]
  mov  [WORD PTR di],ax
  inc  si
  inc  si
  inc  di
  inc  di
  loop W_LESEN
  add  si,bx
  pop  cx
  push cx
  dec  dx
  jz   ENDE_LW
  jmp  W_LESEN
ENDE_LW:
  pop  cx
  IFDEF SAVE_DATA
    pop  ds
  ENDIF
W_LESEN_ENDE:
  ret

  vio_lw ENDP

;==========================================================================

  PUBLIC vio_sw
  vio_sw PROC USES si di, ucX : BYTE, ucY : BYTE, ucBreite : BYTE, \
                          ucHoehe : BYTE, puiBuffer : PTR WORD

  dec  ucX
  dec  ucY
  ADRESSE ucX,ucY
  xor  bh,bh
  mov  bl,80
  sub  bl,ucBreite
  shl  bl,1
  xor  ch,ch
  mov  cl,ucBreite
  cmp  cl,0
  je   W_SCHREIBEN_ENDE
  xor  dh,dh
  mov  dl,ucHoehe
  cmp  dl,0
  je   W_SCHREIBEN_ENDE
  IFDEF SAVE_DATA
    push ds
    lds  si,puiBuffer
  ELSE
    mov  si,puiBuffer
  ENDIF
  push cx
W_SCHREIBEN:
  mov  ax,[WORD PTR si]
  mov  WORD PTR es:[di],ax
  inc  si
  inc  si
  inc  di
  inc  di
  loop W_SCHREIBEN
  add  di,bx
  pop  cx
  push cx
  dec  dx
  jz   ENDE_SW
  jmp  W_SCHREIBEN
ENDE_SW:
  pop  cx
  IFDEF SAVE_DATA
    pop  ds
  ENDIF
W_SCHREIBEN_ENDE:
  ret

  vio_sw ENDP

;==========================================================================
  
  PUBLIC vio_sw_ff
  vio_sw_ff PROC USES si di, ucX : BYTE, ucY : BYTE, ucBreite : BYTE, \
                             ucHoehe : BYTE, puiBuffer : PTR WORD

  dec  ucX
  dec  ucY
  ADRESSE ucX,ucY
  xor  bh,bh
  mov  bl,80
  sub  bl,ucBreite
  shl  bl,1
  xor  ch,ch
  mov  cl,ucBreite
  cmp  cl,0
  je   W_FF_SCHREIBEN_ENDE
  xor  dh,dh
  mov  dl,ucHoehe
  cmp  dl,0
  je   W_FF_SCHREIBEN_ENDE
  IFDEF SAVE_DATA
    push ds
    lds  si,puiBuffer
  ELSE
    mov  si,puiBuffer
  ENDIF
  push cx
W_FF_SCHREIBEN:
  mov  ax,[WORD PTR si]
  cmp  al,0ffh
  je   DONT_WRITE_CHAR
  mov  BYTE PTR es:[di],al
DONT_WRITE_CHAR:
  inc  di
  cmp  ah,0ffh
  je   DONT_WRITE_ATTRI
  mov  BYTE PTR es:[di],ah
DONT_WRITE_ATTRI:
  inc  di
  inc  si
  inc  si
  loop W_FF_SCHREIBEN
  add  di,bx
  pop  cx
  push cx
  dec  dx
  jz   ENDE_FF_SW
  jmp  W_FF_SCHREIBEN
ENDE_FF_SW:
  pop  cx
  IFDEF SAVE_DATA
    pop  ds
  ENDIF
W_FF_SCHREIBEN_ENDE:
  ret

  vio_sw_ff ENDP

;==========================================================================

  PUBLIC vio_color
  vio_color PROC

  mov al,COLOR
  ret

  vio_color ENDP

END
