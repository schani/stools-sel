CC=f:\programm\bc31\bin\BCC
AS=f:\programm\tasm\TASM
!if ($(SPEICHER)!='S')
!if ($(SPEICHER)!='M')
!if ($(SPEICHER)!='C')
!if ($(SPEICHER)!='L')
CSPEICHER=l
ASPEICHER=L
!else
CSPEICHER=l
ASPEICHER=L
!endif
!else
CSPEICHER=c
ASPEICHER=C
!endif
!else
CSPEICHER=m
ASPEICHER=M
!endif
!else
CSPEICHER=s
ASPEICHER=S
!endif                 
CMODELL = -m$(CSPEICHER)
AMODELL = /dM$(ASPEICHER)
CFLAGS = -c -D_MSDOS -v -w-sus -w-pia -w-par -G -I..\INCLUDE -IINCLUDE -Z $(CMODELL)
AFLAGS = /ml /zi $(AMODELL)
LIBDIR = LIB
BIBLIOTHEK = bc_st_$(CSPEICHER)

.c.obj:
   $(CC) $(CFLAGS) $*.c

.asm.obj:
   $(AS) $(AFLAGS) $*.asm;

stools.lib: global.obj   video.obj    mouse.obj    utility.obj  \
            buffers.obj  window.obj   menu.obj     dlg.obj      \
            status.obj   sthelp.obj   queue.obj    sound.obj    \
            memofile.obj stack.obj    dlgpush.obj  dlgact.obj   \
            dlgradio.obj dlgscrol.obj dlglabel.obj dlgbord.obj  \
            dlglist.obj  dlgcolor.obj dlgtext.obj  dlghelp.obj  \
            boxes.obj    dlgedit.obj
   del stools.lib
   f:\programm\bc31\bin\tlib @objekt
   copy stools.lib $(LIBDIR)\$(BIBLIOTHEK).lib
   del stools.lib

global.obj  : global.c

video.obj : video.asm

mouse.obj   : mouse.asm

utility.obj : utility.c

buffers.obj : buffers.c

window.obj  : window.c

menu.obj : menu.c

dlg.obj : dlg.c

boxes.obj : boxes.c

status.obj : status.c

sthelp.obj : sthelp.c

queue.obj : queue.c

dlgedit.obj : dlgedit.c

sound.obj : sound.c

memofile.obj : memofile.c

stack.obj : stack.c

dlgpush.obj : dlgpush.c

dlgact.obj : dlgact.c

dlgradio.obj : dlgradio.c
                                                    
dlgscrol.obj : dlgscrol.c                                      

dlglabel.obj : dlglabel.c

dlgbord.obj : dlgbord.c             

dlglist.obj : dlglist.c                           

dlgcolor.obj : dlgcolor.c                                     

dlgtext.obj : dlgtext.c

dlghelp.obj : dlghelp.c