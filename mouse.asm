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
		%OUT Memory Model Compact
		SAVE_DATA EQU 1
	 ELSE
		.MODEL LARGE,C
		%OUT Memory Model Large
		SAVE_DATA EQU 1
	 ENDIF
  ENDIF
ENDIF

.CODE

  COPYRI    db "Schani Tools (c) 1990-92 by Schani Electronics"
  AKTIV     db 00h
  VORHANDEN db 00h

;==========================================================================

  PUBLIC msm_init
  msm_init PROC

  mov  byte ptr AKTIV,00h
  mov  byte ptr VORHANDEN,00h
  mov  ax,0000h
  int  33h
  cmp  ax,0
  jz   ENDE_I
  mov  byte ptr VORHANDEN,01h
  mov  al,bl
ENDE_I:
  ret

  msm_init ENDP

;==========================================================================

  PUBLIC msm_cursor_on
  msm_cursor_on PROC

  cmp  byte ptr VORHANDEN,00h
  jz   RETURN_ON
  mov  al,01h
  cmp  byte ptr AKTIV,01h
  je   ENDE_ON
  mov  ax,0001h
  int  33h
  mov  al,byte ptr AKTIV
ENDE_ON:
  mov  byte ptr AKTIV,01h
RETURN_ON:
  ret

  msm_cursor_on ENDP

;==========================================================================

  PUBLIC msm_cursor_off
  msm_cursor_off PROC

  mov  al,00h
  cmp  byte ptr VORHANDEN,00h
  jz   ENDE_OFF
  cmp  byte ptr AKTIV,00h
  je   ENDE_OFF
  mov  ax,0002h
  int  33h
  mov  al,byte ptr AKTIV
ENDE_OFF:
  mov  byte ptr AKTIV,00h
  ret

  msm_cursor_off ENDP

;==========================================================================

  PUBLIC msm_cursor
  msm_cursor PROC

  mov  al,00h
  cmp  byte ptr VORHANDEN,00h
  jz   ENDE_IS
  mov  al,byte ptr AKTIV
ENDE_IS:
  ret

  msm_cursor ENDP

;==========================================================================

  PUBLIC msm_get_buttons
  msm_get_buttons PROC USES di, puiButtons : PTR WORD, puiHor : PTR WORD, \
                                puiVer : PTR WORD

  mov  ax,0003h
  int  33h
  IFDEF SAVE_DATA
    push ds
    lds  di,puiButtons
    mov  [di],bx
  ELSE
    mov  di,puiButtons
    mov  [di],bx
  ENDIF
  mov  bx,cx
  mov  cl,3
  shr  bx,cl
  shr  dx,cl
  inc  bx
  inc  dx
  IFDEF SAVE_DATA
    lds  di,puiHor
    mov  [di],bx
    lds  di,puiVer
    mov  [di],dx
    pop  ds
  ELSE
    mov  di,puiHor
    mov  [di],bx
    mov  di,puiVer
    mov  [di],dx
  ENDIF
  ret

  msm_get_buttons ENDP

;==========================================================================

  PUBLIC msm_set_position
  msm_set_position PROC uiHor : WORD, uiVer : WORD

  mov  ax,0004h
  mov  cx,uiHor
  mov  dx,uiVer
  dec  cx
  dec  dx
  mov  bx,cx
  mov  cl,3
  shl  bx,cl
  shl  dx,cl
  mov  cx,bx
  int  33h
  ret

  msm_set_position ENDP

;==========================================================================

  PUBLIC msm_button_press
  msm_button_press PROC USES di, uiKnopf : WORD, puiButtons : PTR WORD, \
                                 puiKlicks : PTR WORD, puiHor : PTR WORD, \
                                 puiVer : PTR WORD

  mov  ax,0005h
  mov  bx,uiKnopf
  int  33h
  IFDEF SAVE_DATA
    push ds
    lds  di,puiButtons
    mov  [di],ax
    lds  di,puiKlicks
    mov  [di],bx
  ELSE
    mov  di,puiButtons
    mov  [di],ax
    mov  di,puiKlicks
    mov  [di],bx
  ENDIF
  mov  bx,cx
  mov  cl,3
  shr  bx,cl
  shr  dx,cl
  inc  bx
  inc  dx
  IFDEF SAVE_DATA
    lds  di,puiHor
    mov  [di],bx
    lds  di,puiVer
    mov  [di],dx
    pop  ds
  ELSE
    mov  di,puiHor
    mov  [di],bx
    mov  di,puiVer
    mov  [di],dx
  ENDIF
  ret

  msm_button_press ENDP

;==========================================================================

  PUBLIC msm_button_release
  msm_button_release PROC USES di, uiKnopf : WORD, puiButtons : PTR WORD, \
                                   puiKlicks : PTR WORD, puiHor : PTR WORD, \
                                   puiVer : PTR WORD

  mov  ax,0006h
  mov  bx,uiKnopf
  int  33h
  IFDEF SAVE_DATA
    push ds
    lds  di,puiButtons
    mov  [di],ax
    lds  di,puiKlicks
    mov  [di],bx
  ELSE
    mov  di,puiButtons
    mov  [di],ax
    mov  di,puiKlicks
    mov  [di],bx
  ENDIF
  mov  bx,cx
  mov  cl,3
  shr  bx,cl
  shr  dx,cl
  inc  bx
  inc  dx
  IFDEF SAVE_DATA
    lds  di,puiHor
    mov  [di],bx
    lds  di,puiVer
    mov  [di],dx
    pop  ds
  ELSE
    mov  di,puiHor
    mov  [di],bx
    mov  di,puiVer
    mov  [di],dx
  ENDIF
  ret

  msm_button_release ENDP

;==========================================================================

  PUBLIC msm_set_hor
  msm_set_hor PROC uiMin : WORD, uiMax : WORD

  mov  ax,0007h
  mov  cx,uiMin
  mov  dx,uiMax
  dec  cx
  dec  dx
  mov  bx,cx
  mov  cl,3
  shl  bx,cl
  shl  dx,cl
  mov  cx,bx
  int  33h
  ret

  msm_set_hor ENDP

;==========================================================================

  PUBLIC msm_set_ver
  msm_set_ver PROC uiMin : WORD, uiMax : WORD

  mov  ax,0008h
  mov  cx,uiMin
  mov  dx,uiMax
  dec  cx
  dec  dx
  mov  bx,cx
  mov  cl,3
  shl  bx,cl
  shl  dx,cl
  mov  cx,bx
  int  33h
  ret

  msm_set_ver ENDP

;==========================================================================

  PUBLIC msm_set_soft_cursor
  msm_set_soft_cursor PROC uiAnd : WORD, uiXor : WORD

  mov  ax,000Ah
  mov  bx,0000h
  mov  cx,uiAnd
  mov  dx,uiXor
  int  33h
  ret

  msm_set_soft_cursor ENDP

;==========================================================================

  PUBLIC msm_set_hard_cursor
  msm_set_hard_cursor PROC uiStart : WORD, uiStop : WORD

  mov  ax,000Ah
  mov  bx,0000h
  mov  cx,uiStart
  mov  dx,uiStop
  int  33h
  ret

  msm_set_hard_cursor ENDP

;==========================================================================

  PUBLIC msm_move
  msm_move PROC USES di, puiHor : PTR WORD, puiVer : PTR WORD

  mov  ax,000Bh
  int  33h
  mov  bx,cx
  mov  cl,3
  shr  bx,cl
  shr  dx,cl
  inc  bx
  inc  dx
  IFDEF SAVE_DATA
    push ds
    lds  di,puiHor
    mov  [di],bx
    lds  di,puiVer
    mov  [di],dx
    pop  ds
  ELSE
    mov  di,puiHor
    mov  [di],bx
    mov  di,puiVer
    mov  [di],dx
  ENDIF
  ret

  msm_move ENDP

;==========================================================================

  PUBLIC msm_lightpen_on
  msm_lightpen_on PROC

  mov  ax,000Dh
  int  33h
  ret

  msm_lightpen_on ENDP

;==========================================================================

  PUBLIC msm_lightpen_off
  msm_lightpen_off PROC

  mov  ax,000Eh
  int  33h
  ret

  msm_lightpen_off ENDP

;==========================================================================

  PUBLIC msm_set_mickeys
  msm_set_mickeys PROC uiHorMickey : WORD, uiVerMickey : WORD

  mov  ax,000Fh
  mov  cx,uiHorMickey
  mov  dx,uiVerMickey
  int  33h
  ret

  msm_set_mickeys ENDP

;==========================================================================

  PUBLIC msm_get_mickeys
  msm_get_mickeys PROC USES di, puiHorMickey : PTR WORD, \
                                puiVerMickey : PTR WORD

  mov  ax,001Bh
  int  33h
  IFDEF SAVE_DATA
    push ds
    lds  di,puiHorMickey
    mov  [di],bx
    lds  di,puiVerMickey
    mov  [di],dx
    pop  ds
  ELSE
    mov  di,puiHorMickey
    mov  [di],bx
    mov  di,puiVerMickey
    mov  [di],dx
  ENDIF
  ret

  msm_get_mickeys ENDP

;==========================================================================

  PUBLIC msm_set_threshold
  msm_set_threshold PROC uiMickeys : WORD

  mov  ax,0013h
  mov  dx,uiMickeys
  int  33h
  ret

  msm_set_threshold ENDP

;==========================================================================

  PUBLIC msm_get_threshold
  msm_get_threshold PROC

  mov  ax,001Bh
  int  33h
  mov  ax,dx
  ret

  msm_get_threshold ENDP

;==========================================================================

  PUBLIC msm_set_page
  msm_set_page PROC uiPage : WORD

  mov  ax,001Dh
  mov  bx,uiPage
  int  33h
  ret

  msm_set_page ENDP

;==========================================================================

  PUBLIC msm_get_page
  msm_get_page PROC

  mov  ax,001Eh
  int  33h
  mov  ax,bx
  ret

  msm_get_page ENDP

;==========================================================================

  PUBLIC msm_get_version
  msm_get_version PROC USES di, pucVersionMajor : PTR BYTE, \
                                pucVersionMinor : PTR BYTE

  mov  ax,0024h
  int  33h
  IFDEF SAVE_DATA
    push ds
    lds  di,pucVersionMajor
    mov  [di],bh
    lds  di,pucVersionMinor
    mov  [di],bl
    pop  ds
  ELSE
    mov  di,pucVersionMajor
    mov  [di],bh
    mov  di,pucVersionMinor
    mov  [di],bl
  ENDIF
  mov  al,ch
  ret

  msm_get_version ENDP

END
