/***************************************************************************
 ***************************************************************************
 ***                                                                     ***
 ***                       Schani-Tools f�r C                            ***
 ***                                                                     ***
 ***                          Headerdatei                                ***
 ***                                                                     ***
 ***               (c) 1990-93 by Schani Electronics                     ***
 ***                                                                     ***
 ***************************************************************************
 ***************************************************************************/

#include <gemein.h>
#include <video.h>
#include <mouse.h>
#include <buffers.h>
#include <sthelp.h>
#include <dlg.h>
#include <window.h>
#include <utility.h>
#include <global.h>
#include <contain.h>
#include <menu.h>
#include <boxes.h>
#include <status.h>
#include <sound.h>
#include <stdio.h>
#include <memofile.h>
#include <context.h>

extern UCHAR       ucActPalette;
extern UINT        uiMouseSpeed;
extern ULONG       ulDoubleClick;
extern BOOL        bBeep;
extern ULONG       ulBeepFrequency;
extern ULONG       ulBeepDuration;
extern BOOL        bSaver;
extern BOOL        bSaverActive;
extern ULONG       ulScreenTimeout;
extern INT         iSizeX;
extern INT         iSizeY;

#ifdef _MSDOS
extern BOOL        bSoundBlaster;
extern BOOL        bVOCBeep;
extern BOOL        bPlayingBeep;
extern WORD        wStatusWord;
extern CHAR        acBeepVOC[128];
#endif

extern WINDOW      winDesktop;
extern BOOL        bExit;
extern GLB_PROGRAM prgProgram;

