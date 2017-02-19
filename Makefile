STOOLS_SRC := boxes.c		dlgact.c	dlgedit.c	dlglist.c	dlgscrol.c	memofile.c	sdl.c		status.c	utility.c	window.c \
	buffers.c	dlgbord.c	dlghelp.c	dlgpush.c	dlgtext.c	menu.c	sthelp.c \
	dlg.c		dlgcolor.c	dlglabel.c	dlgradio.c	global.c	queue.c		stack.c

STOOLS_H := include/boxes.h		include/context.h	include/global.h	include/mouse.h		include/stack.h		include/stnt.h		include/utility.h \
	include/buffers.h	include/dlg.h		include/memofile.h	include/queue.h		include/status.h	include/stools.h	include/video.h \
	include/contain.h	include/gemein.h	include/menu.h		include/sound.h		include/sthelp.h	include/window.h	src/palette.c

OBJDIR := obj

STOOLS_OBJ := $(STOOLS_SRC:%.c=$(OBJDIR)/%.o)

CFLAGS := -D_LINUX -Iinclude -O0 -g -Wall -Wno-invalid-source-encoding `pkg-config --cflags sdl2 SDL2_ttf`

test : $(OBJDIR)/test.o $(STOOLS_OBJ)
	gcc -o $@ -g $^ `pkg-config --libs sdl2 SDL2_ttf`

$(OBJDIR)/test.o : test.c $(STOOLS_H)
	gcc $(CFLAGS) -o $@ -c $<

$(OBJDIR)/%.o : src/%.c $(STOOLS_H)
	gcc $(CFLAGS) -o $@ -c $<
