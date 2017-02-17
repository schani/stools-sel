/***************************************************************************
 ***************************************************************************
 ***                                                                     ***
 ***                      Schani-Tools fÅr C                             ***
 ***                                                                     ***
 ***                         Utility-Teil                                ***
 ***                                                                     ***
 ***               (c) 1990-94 by Schani Electronics                     ***
 ***                                                                     ***
 ***************************************************************************
 ***************************************************************************/

#ifdef _OS2
#define INCL_SUB
#define INCL_DOSPROCESS
#define INCL_DOSNLS
#include <os2.h>
#endif
#ifdef _WINNT
#include <windows.h>
#endif
#ifdef _MSDOS
#include <bios.h>
#endif

#include <gemein.h>
#include <video.h>
#include <mouse.h>
#include <sthelp.h>
#include <dlg.h>
#include <window.h>
#include <dos.h>
#include <string.h>
#include <stdlib.h>
#include <conio.h>
#include <stdio.h>
#include <time.h>
#include <ctype.h>
#include <utility.h>
#ifdef _MSDOS
#include <sound.h>
#endif
#include <global.h>
#ifdef _WINNT
#include <stnt.h>
#endif
#ifdef _OS2
#include <stos2.h>
#endif

extern CHAR        aacBorder[22][8];
extern BOOL        bSaverActive;
extern BOOL        bSaver;
extern ULONG       ulScreenTimeout;
extern BOOL        bBeep;
extern ULONG       ulBeepFrequency;
extern ULONG       ulBeepDuration;
extern ULONG       ulDoubleClick;
#ifdef _MSDOS
extern BOOL        bSoundBlaster;
extern BOOL        bVOCBeep;
extern BOOL        bPlayingBeep;
extern CHAR        acBeepVOC[128];
extern WORD        wStatusWord;
#endif
#ifdef _OS2
extern HMOU        hmouMouse;
#endif
extern GLB_PROGRAM prgProgram;
extern INT         iSizeX;
extern INT         iSizeY;

void          (*mem_error) (void)  = NULL;
void          (*scr_saver) (BOOL)  = NULL;
UTL_SHORT_CUT  *pscFirstShort      = NULL;
UTL_NULL_EVENT *pneFirstEvent      = NULL,
               *pneNextEvent       = NULL;
BOOL            bShortsOn          = TRUE,
                bWinEvents         = TRUE;
WORD            awAltLetters[]     = {
                                       K_A_A,
                                       K_A_B,
                                       K_A_C,
                                       K_A_D,
                                       K_A_E,
                                       K_A_F,
                                       K_A_G,
                                       K_A_H,
                                       K_A_I,
                                       K_A_J,
                                       K_A_K,
                                       K_A_L,
                                       K_A_M,
                                       K_A_N,
                                       K_A_O,
                                       K_A_P,
                                       K_A_Q,
                                       K_A_R,
                                       K_A_S,
                                       K_A_T,
                                       K_A_U,
                                       K_A_V,
                                       K_A_W,
                                       K_A_X,
                                       K_A_Y,
                                       K_A_Z
                                     },
                awAltDigits[]      = {
                                       K_A_0,
                                       K_A_1,
                                       K_A_2,
                                       K_A_3,
                                       K_A_4,
                                       K_A_5,
                                       K_A_6,
                                       K_A_7,
                                       K_A_8,
                                       K_A_9
                                     };
#ifdef _WINNT
HANDLE          hStdIn             = INVALID_HANDLE_VALUE;
DWORD           dwKeyboardStatus   = 0;
BOOL            bKeyboardInsert    = FALSE;
#endif
#ifdef _OS2
HKBD            hkbdKeyboard       = 0;
#endif
#ifdef _MSDOS
BOOL            bLoadVOCOnCall     = FALSE;
void           *pBeepVOC           = NULL;
#endif

/***************************************************************************
 *                       Interne Funktionen                                *
 ***************************************************************************/

#ifdef _MSDOS
BOOL static int_utl_fnstrcmp (CHAR far *pcString1, CHAR *pcString2)
{
  for (; (*pcString1 && *pcString2); pcString1++, pcString2++)
    if (*pcString1 != *pcString2)
      return TRUE;
  if (!*pcString1 && !*pcString2)
    return FALSE;
  else
    return TRUE;
}
#endif

BOOL static int_utl_char_valid (CHAR cZeichen)
{

  CHAR *pcUngueltig = "\\/[]{}:|<>+=,;.\"?*";

  if (cZeichen <= 32)
    return FALSE;
  for (; *pcUngueltig; pcUngueltig++)
    if (cZeichen == *pcUngueltig)
      return FALSE;
  return TRUE;
}

BOOL static int_utl_string_valid (CHAR *pcString, BOOL bName)
{

  BOOL   bPunkt = FALSE;
  CHAR  *pcPos;

  if (strlen(pcString) > 12)
    return FALSE;
  if (pcPos = strchr(pcString, '.'))
  {
    if ((pcString + strlen(pcString) - 1 - pcPos) > 3)
      return FALSE;
    if ((pcPos - pcString) > 8)
      return FALSE;
  }
  while (*pcString)
  {
    if (int_utl_char_valid(*pcString))
      pcString++;
    else
      switch (*pcString)
      {
        case '.' :
          if (bPunkt)
            return FALSE;
          else
          {
            bPunkt = TRUE;
            pcString++;
          }
          break;
        case '*' :
        case '?' :
          if (bName)
            pcString++;
          else
            return FALSE;
          break;
        default :
          return FALSE;
      }
  }
  return TRUE;
}

BOOL static int_utl_short_cut (WORD wKey)
{

  UTL_SHORT_CUT *pscShortCut;

  if (pscFirstShort)
  {
    for (pscShortCut = pscFirstShort;
         (pscShortCut->wKey != wKey) && pscShortCut; pscShortCut = pscShortCut->pscNextShort)
      ;
    if (pscShortCut && (pscShortCut->wKey == wKey))
    {
      pscShortCut->short_func();
      return TRUE;
    }
  }
  return FALSE;
}

#ifdef _WINNT
WORD static int_utl_translate_key (KEY_EVENT_RECORD *pker)
{
  WORD wReturnVar;

  dwKeyboardStatus = pker->dwControlKeyState;
  if (!pker->bKeyDown)
    return 0;
  switch (pker->wVirtualKeyCode)
  {
    case VK_SHIFT :
    case VK_CONTROL :
    case VK_MENU :
    case VK_PAUSE :
    case VK_CAPITAL :
    case VK_NUMLOCK :
    case VK_SCROLL :
      return 0;
    case VK_INSERT :
      bKeyboardInsert = !bKeyboardInsert;
      break;
  }
  wReturnVar = pker->uChar.AsciiChar;
  if (wReturnVar == 0)
    wReturnVar = pker->wVirtualKeyCode | K_VK;
  if (pker->dwControlKeyState & LEFT_ALT_PRESSED || pker->dwControlKeyState & RIGHT_ALT_PRESSED)
    wReturnVar |= K_ALT;
  if (pker->dwControlKeyState & LEFT_CTRL_PRESSED || pker->dwControlKeyState & RIGHT_CTRL_PRESSED)
    wReturnVar |= K_CTRL;
  if (pker->dwControlKeyState & SHIFT_PRESSED && pker->uChar.AsciiChar == 0)
    wReturnVar |= K_SHIFT;

  return wReturnVar;
}
#endif
#ifdef _OS2
WORD static int_utl_translate_key (KBDKEYINFO *pkki)
{
  WORD     wReturnVar;
  KBDTRANS kt;

  kt.chChar = pkki->chChar;
  kt.chScan = pkki->chScan;
  kt.fbStatus = pkki->fbStatus;
  kt.bNlsShift = 0;
  kt.time = pkki->time;
  kt.fsState = pkki->fsState;
  kt.fsXlate = kt.fsShift = 0;
  kt.fsDD = 0;
  kt.sZero = 0;
  KbdXlate(&kt, hkbdKeyboard);
  if (pkki->chChar == 0 || pkki->chChar == 224)
    wReturnVar = (WORD)pkki->chScan | K_VK;
  else
    wReturnVar = (WORD)kt.chChar;
  if (pkki->fsState & 0x03 && (pkki->chChar == 0 || pkki->chChar == 224))
    wReturnVar |= K_SHIFT;
  if (pkki->fsState & 0x04)
    wReturnVar |= K_CTRL;
  if (pkki->fsState & 0x08)
    wReturnVar |= K_ALT;

  return wReturnVar;
}
#endif

int static int_utl_file_compare (const void *p1, const void *p2)
{
  UTL_DIRECTORY_ENTRY *pde1 = (UTL_DIRECTORY_ENTRY*)p1,
                      *pde2 = (UTL_DIRECTORY_ENTRY*)p2;

  if (pde1->bSubDir && !pde2->bSubDir)
    return -1;
  if (!pde1->bSubDir && pde2->bSubDir)
    return 1;
  return strcmp(pde1->pcName, pde2->pcName);
}

/***************************************************************************
 *                       Allgemeine Funktionen                             *
 ***************************************************************************/

void utl_init (void)
{
  mem_error = utl_std_mem_error;
  scr_saver = utl_std_scr_saver;
  utl_randomize();
#ifdef _WINNT
  if (hStdIn == INVALID_HANDLE_VALUE)
    hStdIn = GetStdHandle(STD_INPUT_HANDLE);
  if (hStdIn == INVALID_HANDLE_VALUE)
  {
	  fprintf(stderr, "Error: Could not get input handle.");
	  exit(1);
  }
  if (!SetConsoleMode(hStdIn, ENABLE_MOUSE_INPUT))
  {
	  fprintf(stderr, "Error: Could not set console mode.");
	  exit(1);
  }
#endif
#ifdef _OS2
  {
    ULONG ulCP,
          ulWritten;
  
    if (hkbdKeyboard == 0)
    {
      KbdOpen(&hkbdKeyboard);
      KbdGetFocus(0, hkbdKeyboard);
      DosQueryCp(sizeof(ULONG), &ulCP, &ulWritten);
      KbdSetCp(0, ulCP, hkbdKeyboard);
    }
  }
#endif
}

void utl_randomize (void)
{
  srand((int)time(NULL));
}

UINT utl_random (UINT uiMax)
{
  return rand() % uiMax;
}

void utl_std_mem_error (void)
{
  msm_cursor_off();
  utl_short_cuts(FALSE);
  utl_border(25, 7, 31, 10, B_EEEE, glb_get_color(PAL_WIN_ERROR,
                                                  PAL_COL_BORDER));
  utl_shadow(25, 7, 31, 10);
  vio_sw_za(26, 8, 29, 8, ' ', glb_get_color(PAL_WIN_ERROR,
                                             PAL_COL_BACKGROUND));
  vio_ss(28,  9, "Speicherzuordnungsfehler!");
  vio_ss(28, 11, "Vermutlich zu wenig");
  vio_ss(28, 12, "Arbeitsspeicher frei");
  vio_ss(28, 14, "Bitte eine Taste drÅcken!");
  utl_get_key();
  vio_sp_za(K_SPACE, 7);
  exit(1);
}

void utl_std_scr_saver (BOOL bMouse)
{

         UTL_EVENT eventEvent;
         WORD      wButtons,
                   wOldButtons;
         INT       iHor,
                   iVer,
                   iOldHor,
                   iOldVer;
         UINT      uiEbene,
                   uiStern,
                   uiMax                                     = 0;
         BOOL      bEnd                                      = FALSE;
  static UTL_STERN sastrSterne[UTL_SS_EBENEN][UTL_SS_STERNE];

  utl_cls(15);
  if (bMouse)
    msm_get_buttons(&wOldButtons, &iOldHor, &iOldVer);
  for (uiEbene = 0; uiEbene < UTL_SS_EBENEN; uiEbene++)
    for (uiStern = 0; uiStern < UTL_SS_STERNE; uiStern++)
    {
      sastrSterne[uiEbene][uiStern].ucX = utl_random(80) + 1;
      sastrSterne[uiEbene][uiStern].ucY = utl_random(25) + 1;
    }
  do
  {
    utl_event(&eventEvent);
    if (eventEvent.uiKind != E_NULL)
      bEnd = TRUE;
    if (bMouse)
    {
      msm_get_buttons(&wButtons, &iHor, &iVer);
      if (wButtons != wOldButtons || iHor != iOldHor || iVer != iOldVer)
        bEnd = TRUE;
    }
    if (!bEnd)
    {
      for (uiEbene = 0; uiEbene <= uiMax; uiEbene++)
        for (uiStern = 0; uiStern < UTL_SS_STERNE; uiStern++)
        {
          vio_s_z(sastrSterne[uiEbene][uiStern].ucX,
                  sastrSterne[uiEbene][uiStern].ucY, ' ');
          if (!(--sastrSterne[uiEbene][uiStern].ucX))
            sastrSterne[uiEbene][uiStern].ucX = 80;
          vio_s_z(sastrSterne[uiEbene][uiStern].ucX,
                  sastrSterne[uiEbene][uiStern].ucY, UTL_SS_STERN);
        }
      utl_delay(50);
      if ((++uiMax) >= UTL_SS_EBENEN)
        uiMax = 0;
    }
  }
  while (!bEnd);
}

void utl_set_mem_error (void (*new_mem_error) (void))
{
  mem_error = new_mem_error;
}

void utl_set_scr_saver (void (*new_scr_saver) (BOOL))
{
  scr_saver = new_scr_saver;
}

void utl_save_screen (void)
{
  BOOL       bMouse,
             bShorts;
  INT        iPercent,
             iDummy;
  CHARACTER *pcharScreenBuffer;

  bMouse = msm_cursor_off();
  bShorts = utl_short_cuts(FALSE);
  pcharScreenBuffer = utl_alloc(iSizeX * iSizeY * sizeof(CHARACTER));
  vio_lw(1, 1, 80, 25, pcharScreenBuffer);
  vio_get_cursor(&iDummy, &iDummy, &iPercent);
#ifndef _MSDOS
  vio_set_cursor_type(0);
#endif
  scr_saver(bMouse);
#ifndef _MSDOS
  vio_set_cursor_type(iPercent);
#endif
  vio_sw(1, 1, 80, 25, pcharScreenBuffer);
  utl_free(pcharScreenBuffer);
  if (bMouse)
    msm_cursor_on();
  utl_short_cuts(bShorts);
}

void utl_delay (ULONG ulWait)
{
#ifdef _MSDOS
  ULONG ulT1,
        ulT2;

  ulWait /= 55;
  if(!ulWait)
    return;
  ulT1 = ulWait + clock();
  do
  {
    ulT2 = clock();
  } while( ulT2 < ulT1 );
#endif
#ifdef _WINNT
  Sleep(ulWait);
#endif
#ifdef _OS2
  DosSleep(ulWait);
#endif
}

void* utl_alloc (UINT uiSize)
{

  void *pReturnVar;

  pReturnVar = malloc(uiSize);
  if (uiSize && !pReturnVar && mem_error)
  {
    mem_error();
    pReturnVar = malloc(uiSize);
  }
  return pReturnVar;
}

void* utl_calloc (UINT uiItems, UINT uiSize)
{

  void *pReturnVar;

  pReturnVar = calloc(uiItems, uiSize);
  if (uiSize && uiItems && !pReturnVar && mem_error)
  {
    mem_error();
    pReturnVar = malloc(uiSize);
  }
  return pReturnVar;
}         

void* utl_realloc (void *pBlock, UINT uiSize)
{
  pBlock = realloc(pBlock, uiSize);
  if (uiSize && !pBlock && mem_error)
  {
    mem_error();
    pBlock = realloc(pBlock, uiSize);
  }
  return(pBlock);
}

void utl_sound (ULONG ulFrequency)
{
#ifdef _MSDOS
  UINT uiZaehler;

  if (ulFrequency)
  {
    outp(97, (inp(97) | 3));
    outp(67, 182);
    uiZaehler = (UINT)(1193180 / ulFrequency);
    outp(66, (uiZaehler & 0x00ff));
    outp(66, (uiZaehler >> 8));
  }
  else
    outp(97, (inp(97) & ~3));
#endif
}

void utl_set_voc_beep (CHAR *pcName, BOOL bLoadOnCall)
{
#ifdef _MSDOS                   /* in Windows NT we use the default MessageBeep */
  if (pBeepVOC)
    utl_free(pBeepVOC);
  strcpy(acBeepVOC, pcName);
  bLoadVOCOnCall = bLoadOnCall;
  if (!bLoadOnCall)
    if (!(pBeepVOC = sbl_load_voc(acBeepVOC)))
    {
      bVOCBeep = FALSE;
      return;
    }
#endif
}

void utl_tone (ULONG ulFrequency, ULONG ulDuration)
{
#ifdef _WINNT
  Beep(ulFrequency, ulDuration);
#endif
#ifdef _OS2
  DosBeep(ulFrequency, ulDuration);
#endif
#ifdef _MSDOS
  utl_sound(ulFrequency);
  utl_delay(ulDuration);
  utl_no_sound();
#endif
}

void utl_beep (void)
{
#ifdef _MSDOS
  if (bBeep)
  {                     
    if (bSoundBlaster && bVOCBeep)
      utl_voc_beep();
    else
      utl_tone(ulBeepFrequency, ulBeepDuration);
  }
#endif
#ifdef _WINNT
  if (bBeep)
    utl_voc_beep();
#endif
#ifdef _OS2
  if (bBeep)
    utl_tone(ulBeepFrequency, ulBeepDuration);
#endif
}

void utl_voc_beep (void)
{
#ifdef _MSDOS
  if (!bPlayingBeep && bSoundBlaster)
  {
    if (bLoadVOCOnCall)
      if (!(pBeepVOC = sbl_load_voc(acBeepVOC)))
        return;
    sbl_start_voc(pBeepVOC);
    bPlayingBeep = TRUE;
  }
#endif
#ifdef _WINNT
  /* MessageBeep(MB_OK); */
#endif
}

void utl_shadow (INT iX, INT iY, UINT uiWidth, UINT uiHeight)
{

  INT iZaehler1,
      iZaehler2;
  MOUSE;

  BEGIN_MOUSE;
  for (iZaehler1 = (iY + 1); iZaehler1 <= (INT)(iY + uiHeight); iZaehler1++)
    for (iZaehler2 = (iX + uiWidth); iZaehler2 < (INT)(iX + uiWidth + 2);
         iZaehler2++)
      if (iZaehler1 >= 1 && iZaehler1 <= 25 && 
          iZaehler2 >= 1 && iZaehler2 <= 80)
      vio_s_a(iZaehler2, iZaehler1, 8);
  if (iY + uiHeight >= 1 && iY + uiHeight <= 25)
    for (iZaehler1 = (iX + 2); iZaehler1 < (INT)(iX + uiWidth); iZaehler1++)
      if (iZaehler1 >= 1 && iZaehler1 <= 80)
        vio_s_a(iZaehler1, iY + uiHeight, 8);
  END_MOUSE;
}

void utl_border (INT iX, INT iY, INT iWidth, INT iHeight, UCHAR ucRahmenTyp, CHAR cAttri)
{

  INT iPointX,
      iPointY;
  MOUSE;

  BEGIN_MOUSE;
  if (ON_SCREEN(iX, iY))
    vio_s_za(iX, iY, aacBorder[ucRahmenTyp][0], cAttri);
  iPointX = iX + iWidth - 1;
  if (ON_SCREEN(iPointX, iY))
    vio_s_za(iPointX, iY, aacBorder[ucRahmenTyp][1], cAttri);
  iPointY = iY + iHeight - 1;
  if (ON_SCREEN(iX, iPointY))
    vio_s_za(iX, iPointY, aacBorder[ucRahmenTyp][2], cAttri);
  iPointX = iX + iWidth - 1;
  iPointY = iY + iHeight - 1;
  if (ON_SCREEN(iPointX, iPointY))
    vio_s_za(iPointX, iPointY, aacBorder[ucRahmenTyp][3], cAttri);
  iPointY = iY + iHeight - 1;
  for (iPointX = iX + 1; iPointX < iX + iWidth - 1; iPointX++)
  {
    if (ON_SCREEN(iPointX, iY))
      vio_s_za(iPointX, iY, aacBorder[ucRahmenTyp][4], cAttri);
    if (ON_SCREEN(iPointX, iPointY))
      vio_s_za(iPointX, iPointY, aacBorder[ucRahmenTyp][5], cAttri);
  }
  iPointX = iX + iWidth - 1;
  for (iPointY = iY + 1; iPointY < iY + iHeight - 1; iPointY++)
  {
    if (ON_SCREEN(iX, iPointY))
      vio_s_za(iX, iPointY, aacBorder[ucRahmenTyp][6], cAttri);
    if (ON_SCREEN(iPointX, iPointY))
      vio_s_za(iPointX, iPointY, aacBorder[ucRahmenTyp][7], cAttri);
  }
  END_MOUSE;
}

BOOL utl_kb_hit (void)
{
#ifdef _MSDOS
#ifdef __TURBOC__
  if (!bioskey(1))
    return FALSE;
  else
    return TRUE;
#endif
#if defined _WATCOM || defined _MSC_VER || defined _QC
  if (!_bios_keybrd(_KEYBRD_READY))
    return FALSE;
  else
    return TRUE;
#endif
#endif
#ifdef _WINNT
  DWORD         dwEvents,
                dwCounter;
  INPUT_RECORD *pirEvents;

  GetNumberOfConsoleInputEvents(hStdIn, &dwEvents);
  if (!(pirEvents = utl_alloc(dwEvents * sizeof(INPUT_RECORD))))
    return FALSE;
  PeekConsoleInput(hStdIn, pirEvents, dwEvents, &dwEvents);
  for (dwCounter = 0; dwCounter < dwEvents; dwCounter++)
    if (pirEvents[dwCounter].EventType == KEY_EVENT)
      if (pirEvents[dwCounter].Event.KeyEvent.bKeyDown)
      {
        utl_free(pirEvents);
        return TRUE;
      }
  utl_free(pirEvents);
  return FALSE;
#endif
#ifdef _OS2
  KBDKEYINFO kki;

  kki.chScan = 0;
  KbdPeek(&kki, hkbdKeyboard);
  if (kki.chScan == 0)
    return FALSE;
  return TRUE;
#endif
}

BOOL utl_insert_null_event (void (*event_handler) (void))
{
  UTL_NULL_EVENT *pneNullEvent;

  if (pneFirstEvent)
  {
    for (pneNullEvent = pneFirstEvent; pneNullEvent->pneNextEvent;
         pneNullEvent = pneNullEvent->pneNextEvent)
      ;
    if (!(pneNullEvent->pneNextEvent = utl_alloc(sizeof(UTL_NULL_EVENT))))
      return FALSE;
    pneNullEvent = pneNullEvent->pneNextEvent;
  }
  else
  {
    if (!(pneNullEvent = utl_alloc(sizeof(UTL_NULL_EVENT))))
      return FALSE;
    pneFirstEvent = pneNullEvent;
  }
  pneNullEvent->event_handler = event_handler;
  pneNullEvent->pneNextEvent = NULL;
  return TRUE;
}

void utl_del_null_event (void (*event_handler) (void))
{

  UTL_NULL_EVENT *pneNullEvent,
                 *pnePuffer;

  if (pneFirstEvent)
  {
    if (pneFirstEvent->event_handler == event_handler)
    {
      utl_free(pneFirstEvent);
      pneFirstEvent = NULL;
    }
    else
    {
      if (!(pneFirstEvent->pneNextEvent))
        return;
      for (pneNullEvent = pneFirstEvent;
           pneNullEvent->pneNextEvent->event_handler != event_handler;
           pneNullEvent = pneNullEvent->pneNextEvent)
        if (!(pneNullEvent->pneNextEvent->pneNextEvent))
          return;
      pnePuffer = pneNullEvent->pneNextEvent->pneNextEvent;
      utl_free(pneNullEvent->pneNextEvent);
      pneNullEvent->pneNextEvent = pnePuffer;
    }
  }
  pneNextEvent = NULL;
}

BOOL utl_insert_short (WORD wKey, void (*short_func) (void))
{

  UTL_SHORT_CUT *pscShortCut;

  if (pscFirstShort)
  {
    for (pscShortCut = pscFirstShort; pscShortCut->pscNextShort;
         pscShortCut = pscShortCut->pscNextShort)
      ;
    if (!(pscShortCut->pscNextShort = utl_alloc(sizeof(UTL_SHORT_CUT))))
      return FALSE;
    pscShortCut = pscShortCut->pscNextShort;
  }
  else
  {
    if (!(pscFirstShort = utl_alloc(sizeof(UTL_SHORT_CUT))))
      return FALSE;
    pscShortCut = pscFirstShort;
  }
  pscShortCut->wKey = wKey;
  pscShortCut->short_func = short_func;
  pscShortCut->pscNextShort = NULL;
  return TRUE;
}

void utl_del_short (WORD wKey)
{

  UTL_SHORT_CUT *pscShortCut,
                *pscPuffer;

  if (pscFirstShort)
  {
    if (pscFirstShort->wKey == wKey)
    {
      utl_free(pscFirstShort);
      pscFirstShort = NULL;
    }
    else
    {
      if (!(pscFirstShort->pscNextShort))
        return;
      for (pscShortCut = pscFirstShort;
           pscShortCut->pscNextShort->wKey != wKey;
           pscShortCut = pscShortCut->pscNextShort)
        if (!(pscShortCut->pscNextShort->pscNextShort))
          return;
      pscPuffer = pscShortCut->pscNextShort->pscNextShort;
      utl_free(pscShortCut->pscNextShort);
      pscShortCut->pscNextShort = pscPuffer;
    }
  }
}

BOOL utl_short_cuts (BOOL bStatus)
{

  BOOL bReturnVar;

  bReturnVar = bShortsOn;
  bShortsOn = bStatus;
  return bReturnVar;
}

WORD utl_get_key (void)
{
#ifdef _MSDOS
  WORD wReturnVar;

  if (!(wReturnVar = getch()))
    wReturnVar = (getch() | EXT_CODE);

  return wReturnVar;
#endif
#ifdef _WINNT
  WORD         wReturnVar;
  DWORD        dwEvents;
  INPUT_RECORD ir;

  do
  {
    GetNumberOfConsoleInputEvents(hStdIn, &dwEvents);
    if (dwEvents == 0)
      WaitForSingleObject(hStdIn, INFINITE);
    ReadConsoleInput(hStdIn, &ir, 1, &dwEvents);
    if (dwEvents == 1)
      switch (ir.EventType)
      {
        case KEY_EVENT :
          wReturnVar = int_utl_translate_key(&ir.Event.KeyEvent);
          if (wReturnVar != 0)
            return wReturnVar;
          break;
        case MOUSE_EVENT :
          dwKeyboardStatus = ir.Event.MouseEvent.dwControlKeyState;
          msm_process_input_event(&ir.Event.MouseEvent);
          break;
      }
  } while (TRUE);
  
  return 0;
#endif
#ifdef _OS2
  KBDKEYINFO kki;

  KbdCharIn(&kki, 0, hkbdKeyboard);
  return int_utl_translate_key(&kki);
#endif
}

DWORD utl_get_timer (void)
{
#ifdef _MSDOS
  union REGS regs;

  regs.h.ah = 0;
  int86(0x1a, &regs, &regs);
  return ((DWORD)((ULONG)regs.x.dx + ((DWORD)regs.x.cx << 16)) * 549) / 10;
#endif
#ifdef _WINNT
  SYSTEMTIME st;

  GetLocalTime(&st);
  return (DWORD)st.wHour * 3600000 + (DWORD)st.wMinute * 60000 + (DWORD)st.wSecond * 1000 + (DWORD)st.wMilliseconds;
#endif
#ifdef _OS2
  DATETIME dt;

  DosGetDateTime(&dt);
  return (DWORD)dt.hours * 3600000 + (DWORD)dt.minutes * 60000 + (DWORD)dt.seconds * 1000 + (DWORD)dt.hundredths * 10;
#endif
}

void utl_cls (CHAR cAttri)
{
  vio_scroll_down(1, 1, 80, 25, cAttri, 0);
}

BYTE utl_keyb_status (void)
{
#ifdef _MSDOS
  union REGS regs;

  regs.h.ah = 0x02;
  int86(0x16, &regs, &regs);
  return(regs.h.al);
#endif
#ifdef _WINNT
  BYTE byReturnVar = 0;

  if (dwKeyboardStatus & LEFT_ALT_PRESSED || dwKeyboardStatus & RIGHT_ALT_PRESSED)
    byReturnVar |= ALT;
  if (dwKeyboardStatus & LEFT_CTRL_PRESSED || dwKeyboardStatus & RIGHT_CTRL_PRESSED)
    byReturnVar |= CTRL;
  if (dwKeyboardStatus & SHIFT_PRESSED)
    byReturnVar |= SHIFT_RIGHT;
  if (dwKeyboardStatus & NUMLOCK_ON)
    byReturnVar |= NUM_LOCK;
  if (dwKeyboardStatus & SCROLLLOCK_ON)
    byReturnVar |= SCROLL_LOCK;
  if (dwKeyboardStatus & CAPSLOCK_ON)
    byReturnVar |= CAPS_LOCK;
  if (bKeyboardInsert)
    byReturnVar |= INSERT;

  return byReturnVar;
#endif
#ifdef _OS2
  KBDINFO ki;

  KbdGetStatus(&ki, hkbdKeyboard);
  return ki.fsState & 0xff;
#endif
}   

void utl_strdel (CHAR *pcText, INT iStart, INT iLength)
{
  CHAR *pcSource,
       *pcDest;

  pcDest = (pcText + iStart);
  pcSource = (pcText + iStart + iLength);
  for ( ; *pcSource; pcSource++, pcDest++)
    *pcDest = *pcSource;
  *pcDest = *pcSource;
}                          

void utl_strins (CHAR *pcText, INT iPos, CHAR cChar)
{
  CHAR *pcChar;

  pcChar = (pcText + strlen(pcText));
  for ( ; pcChar - pcText > iPos; pcChar--)
    *(pcChar + 1) = *pcChar;
  *(pcChar + 1) = *pcChar;
  *pcChar = cChar;
}

void utl_ltrim (CHAR *pcString)
{
  while (*pcString == 32)
    utl_strdel(pcString, 0, 1);
}

void utl_rtrim (CHAR *pcString)
{
  INT iCounter;

  iCounter = strlen(pcString) - 1;
  while (pcString[iCounter] == 32)
  {
    pcString[iCounter] = 0;
    iCounter--;
  }
}

CHAR utl_upper (CHAR cBuchstabe)
{
  if ((cBuchstabe > 96) && (cBuchstabe < 123))
    return(cBuchstabe & 0xdf);
  switch (cBuchstabe)
  {
    case 'Ñ' :
      return('é');
    case 'î' :
      return('ô');
    case 'Å' :
      return('ö');
    default  :
      return(cBuchstabe);
  }
}

CHAR utl_lower (CHAR cBuchstabe)
{
  if ((cBuchstabe > 64) && (cBuchstabe < 91))
    return(cBuchstabe | 0x20);
  switch (cBuchstabe)
  {
    case 'é' :
      return('Ñ');
    case 'ô' :
      return('î');
    case 'ö' :
      return('Å');
    default  :
      return(cBuchstabe);
  }
}

CHAR* utl_str_upper (CHAR *pcString)
{
  CHAR *pcZaehler;

  for (pcZaehler = pcString; *pcZaehler; pcZaehler++)
    *pcZaehler = utl_upper(*pcZaehler);
  return pcString;
}

CHAR* utl_str_lower (CHAR *pcString)
{
  CHAR *pcZaehler;

  for (pcZaehler = pcString; *pcZaehler; pcZaehler++)
    *pcZaehler = utl_lower(*pcZaehler);
  return pcString;
}

WORD utl_alt_code (CHAR cBuchstabe)
{
  if (isalpha(cBuchstabe))
    return(awAltLetters[utl_upper(cBuchstabe) - 65]);
  if (isdigit(cBuchstabe))
    return(awAltDigits[cBuchstabe - 48]);

  return 0;
}    

CHAR utl_get_hot_key (CHAR *pcText)
{
  while (*pcText)
  {
    if (*pcText == '#')
      return pcText[1];
    pcText++;
  }

  return 0;
}    

INT utl_hot_strlen (CHAR *pcText)
{
  INT iReturnVar = 0;
  
  for (; *pcText; pcText++)
    if (*pcText != '~' && *pcText != '#')
      iReturnVar++;
  return iReturnVar;
}

void utl_get_path (CHAR *pcBuffer)
{
#ifdef _MSDOS
  CHAR         far *pcPath;
  union  REGS       regs;
  struct SREGS      sregs;

  pcPath = (CHAR far*)pcBuffer;
  pcPath[1] = ':';
  pcPath[2] = '\\';
  *pcPath = (CHAR)(utl_get_drive() + 65);
  regs.h.ah = 0x47;
  regs.h.dl = 0;
  pcPath += 3;
  sregs.ds = FP_SEG(pcPath);
  regs.x.si = FP_OFF(pcPath);
  intdosx(&regs, &regs, &sregs);
#endif
#ifdef _WINNT
  GetCurrentDirectory(128, pcBuffer);
#endif
#ifdef _OS2
  ULONG ulDriveNumber,
        ulDummy;

  DosQueryCurrentDisk(&ulDriveNumber, &ulDummy);
  pcBuffer[0] = (CHAR)ulDriveNumber - 1 + 'A';
  pcBuffer[1] = ':';
  pcBuffer[2] = '\\';
  DosQueryCurrentDir(ulDriveNumber, pcBuffer + 3, &ulDummy);
#endif
}

void utl_set_path (CHAR *pcPath)
{
#ifdef _MSDOS
  CHAR         far *pcBuffer;
  union REGS        regs;
  struct SREGS      sregs;

  if (pcPath[1] == ':')
  {
    utl_set_drive(toupper(*pcPath) - 'A');
    pcPath += 2;
  }
  pcBuffer = (CHAR far*)pcPath;
  regs.h.ah = 0x3b;
  sregs.ds = FP_SEG(pcBuffer);
  regs.x.dx = FP_OFF(pcBuffer);
  intdosx(&regs, &regs, &sregs);
#endif
#ifdef _WINNT
  SetCurrentDirectory(pcPath);
#endif
#ifdef _OS2
  if (pcPath[1] == ':')
  {
    DosSetDefaultDisk(toupper(pcPath[0]) - 'A' + 1);
    pcPath += 2;
  }
  DosSetCurrentDir(pcPath);
#endif
}

/*
BOOL utl_get_files (CHAR *pcPuffer, CHAR *pcMaske, UINT uiBufferLength)
{
#ifndef _WINNT
  union  REGS       regs;
  struct SREGS      sregs;
  UTL_FILE     far *fileFile;
  CHAR         far *pcFarMaske,
               far *pcAllMaske     = "*.*";
  UCHAR             ucZaehler;
  UINT              uiEintraege    = 0,
                    uiMaxEintraege;

  uiMaxEintraege = (uiBufferLength - 1) / 13;
  pcFarMaske = (CHAR far*)pcMaske;
  regs.h.ah = 0x2F;
  intdosx(&regs, &regs, &sregs);
  fileFile = (UTL_FILE far*)(((ULONG)sregs.es << 16) + (ULONG)regs.x.bx);
  regs.h.ah = 0x4e;
  regs.x.cx = ~0;
  sregs.ds = FP_SEG(pcAllMaske);
  regs.x.dx = FP_OFF(pcAllMaske);
  intdosx(&regs, &regs, &sregs);
  fileFile->acName[12] = 0;
  if (!regs.x.cflag)
  {
    if (int_utl_fnstrcmp(fileFile->acName, "."))
    {
      if (fileFile->ucAttribut == UTL_SUBDIR)
      {
        uiEintraege++;
        for (ucZaehler = 0; ucZaehler < 13; ucZaehler++)
          pcPuffer[ucZaehler] = utl_upper(fileFile->acName[ucZaehler]);
        pcPuffer += 13;
      }
    }
    do
    {
      regs.h.ah = 0x4f;
      intdos(&regs, &regs);
      fileFile->acName[12] = 0;
      if (!regs.x.cflag && (fileFile->ucAttribut == UTL_SUBDIR))
      {
        for (ucZaehler = 0; ucZaehler < 13; ucZaehler++)
          pcPuffer[ucZaehler] = utl_upper(fileFile->acName[ucZaehler]);
        pcPuffer += 13;
        uiEintraege++;
      }
    } while (!regs.x.cflag && (uiEintraege < uiMaxEintraege));
  }
  regs.h.ah = 0x4e;
  regs.x.cx = ~UTL_SUBDIR;
  sregs.ds = FP_SEG(pcFarMaske);
  regs.x.dx = FP_OFF(pcFarMaske);
  intdosx(&regs, &regs, &sregs);
  fileFile->acName[12] = 0;
  if (!regs.x.cflag)
  {
    if (int_utl_fnstrcmp(fileFile->acName, "."))
    {
      if (fileFile->ucAttribut != UTL_SUBDIR)
      {
        uiEintraege++;
        for (ucZaehler = 0; ucZaehler < 13; ucZaehler++)
          pcPuffer[ucZaehler] = utl_lower(fileFile->acName[ucZaehler]);
        pcPuffer += 13;
      }
    }
    do
    {
      regs.h.ah = 0x4f;
      intdos(&regs, &regs);
      fileFile->acName[12] = 0;
      if (!regs.x.cflag && (fileFile->ucAttribut != UTL_SUBDIR))
      {
        for (ucZaehler = 0; ucZaehler < 13; ucZaehler++)
          pcPuffer[ucZaehler] = utl_lower(fileFile->acName[ucZaehler]);
        pcPuffer += 13;
        uiEintraege++;
      }
    } while (!regs.x.cflag && (uiEintraege < uiMaxEintraege));
    *pcPuffer = 0;
  }
  else
    *pcPuffer = 0;
  pcPuffer -= (13 * uiEintraege);
  if (uiEintraege)
    qsort(pcPuffer, uiEintraege, 13,
          (int(*)(const void*, const void*))strcmp);

  return TRUE;
#else
  WIN32_FIND_DATA ffd;
  HANDLE          hFind;
  UINT            uiFree;

  uiFree = uiBufferSize - 1;
  hFind = FindFirstFile(pcMaske, &ffd);
  while (hFind != INVALID_HANDLE_VALUE)
  {
    if (strlen(ffd.cFileName) + 1 > uiFree)
    {
      *pcPuffer = 0;

      return TRUE;
    }
    strcpy(pcPuffer, ffd.cFileName);
    pcPuffer += strlen(pcPuffer) + 1;
    FindNextFile(hFind, &ffd);
  }
  *pcPuffer = 0;

  return TRUE;
#endif
}*/

BOOL utl_get_files (CHAR *pcMask, CHAR *pcBuffer, UTL_DIRECTORY_ENTRY *pdeTable, UINT uiBufferSize,
                    UINT uiTableSize, BOOL bIncludeDirs)
{
#ifdef _WINNT
  UINT            uiBufferFree = uiBufferSize,
                  uiTableUsed  = 0;
  HANDLE          hFind;
  WIN32_FIND_DATA ffd;

  uiTableSize--;                     /* we need one table entry for the terminator */
  hFind = FindFirstFile(pcMask, &ffd);
  while (hFind != INVALID_HANDLE_VALUE && uiTableUsed < uiTableSize)
  {
    if (!(ffd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY && !bIncludeDirs))
    {
      if (strlen(ffd.cFileName) + 1 > uiBufferFree)
        break;
      strcpy(pcBuffer, ffd.cFileName);
      pdeTable[uiTableUsed].pcName = pcBuffer;
      if (ffd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
      {
        pdeTable[uiTableUsed++].bSubDir = TRUE;
        utl_str_upper(pcBuffer);
      }
      else
      {  
        pdeTable[uiTableUsed++].bSubDir = FALSE;
        utl_str_lower(pcBuffer);
      }
      pcBuffer += strlen(ffd.cFileName) + 1;
    }
    if (!FindNextFile(hFind, &ffd))
      break;
  }
  pdeTable[uiTableUsed].pcName = NULL;
  FindClose(hFind);

  qsort(pdeTable, uiTableUsed, sizeof(UTL_DIRECTORY_ENTRY), int_utl_file_compare);

  return TRUE;
#endif
#ifdef _OS2
  UINT         uiBufferFree  = uiBufferSize,
               uiTableUsed   = 0;
  FILEFINDBUF3 ffb3;
  HDIR         hdir          = HDIR_CREATE;
  ULONG        ulSearchCount = 1,
               ulAttributes;
  CHAR         acAll[]       = "*";
  BOOL         bDoneDirs     = FALSE;
  APIRET       rc;

  uiTableSize--;
  ulAttributes = FILE_ARCHIVED | FILE_SYSTEM | FILE_HIDDEN | FILE_READONLY;
  rc = DosFindFirst(pcMask, &hdir, ulAttributes, &ffb3, sizeof(FILEFINDBUF3), &ulSearchCount, FIL_STANDARD);
  if (rc != 0 && !bDoneDirs && bIncludeDirs)
  {
    DosFindClose(hdir);
    ulAttributes = MUST_HAVE_DIRECTORY | FILE_DIRECTORY;
    ulSearchCount = 1;
    hdir = HDIR_CREATE;
    rc = DosFindFirst(acAll, &hdir, ulAttributes, &ffb3, sizeof(FILEFINDBUF3), &ulSearchCount, FIL_STANDARD);
    bDoneDirs = TRUE;
  }
  while (rc == 0 && uiTableUsed < uiTableSize)
  {
    if (strlen(ffb3.achName) + 1 > uiBufferFree)
      break;
    strcpy(pcBuffer, ffb3.achName);
    pdeTable[uiTableUsed].pcName = pcBuffer;
    if (ffb3.attrFile & FILE_DIRECTORY)
    {
      if (strcmp(pcBuffer, "."))
      {
        pdeTable[uiTableUsed++].bSubDir = TRUE;
        utl_str_upper(pcBuffer);
      }
    }
    else
    {
      pdeTable[uiTableUsed++].bSubDir = FALSE;
      utl_str_lower(pcBuffer);
    }
    pcBuffer += strlen(ffb3.achName) + 1;

    ulSearchCount = 1;      
    rc = DosFindNext(hdir, &ffb3, sizeof(FILEFINDBUF3), &ulSearchCount);
    if (rc != 0 && !bDoneDirs && bIncludeDirs)
    {
      DosFindClose(hdir);
      ulAttributes = MUST_HAVE_DIRECTORY | FILE_DIRECTORY;
      ulSearchCount = 1;
      hdir = HDIR_CREATE;
      rc = DosFindFirst(acAll, &hdir, ulAttributes, &ffb3, sizeof(FILEFINDBUF3), &ulSearchCount, FIL_STANDARD);
      bDoneDirs = TRUE;
    }
  }
  pdeTable[uiTableUsed].pcName = NULL;
  DosFindClose(hdir);

  qsort(pdeTable, uiTableUsed, sizeof(UTL_DIRECTORY_ENTRY), int_utl_file_compare);

  return TRUE;
#endif
#ifdef _MSDOS
  return FALSE;
#endif
}

BOOL utl_filename_valid (CHAR *pcName)
{
  BOOL  bReturnVar  = TRUE,
        bName       = TRUE;
  CHAR *pcText,
       *pcFirstChar,
       *pcZeiger,
       *pcLastChar;

  if (!(pcText = utl_alloc(strlen(pcName) + 1)))
    return FALSE;
  pcFirstChar = strcpy(pcText, pcName);
  pcLastChar = (pcFirstChar + strlen(pcFirstChar));
  if (isalpha(*pcFirstChar) && (*(pcFirstChar + 1) == ':'))
    pcFirstChar += 2;
  if (*(pcLastChar - 1) == '\\')
    bReturnVar = FALSE;
  for (pcZeiger = pcFirstChar; pcZeiger < pcLastChar; pcZeiger++)
    if (*pcZeiger == '\\')
      *pcZeiger = 0;
  for (pcLastChar--; pcLastChar >= pcFirstChar; pcLastChar--)
    if (!(*pcLastChar) || (pcLastChar == pcFirstChar))
    {
      if (*pcLastChar)
        bReturnVar = (bReturnVar && int_utl_string_valid(pcLastChar, bName));
      else
        bReturnVar = (bReturnVar && int_utl_string_valid(pcLastChar + 1, bName));
      bName = FALSE;
    }
  utl_free(pcText);

  return bReturnVar;
}

BOOL utl_get_win_events (void)
{
  return bWinEvents;
}

void utl_set_win_events (BOOL bStatus)
{
  bWinEvents = bStatus;
}

UCHAR utl_get_drive (void)
{
#ifdef _MSDOS
  union REGS regs;

  regs.h.ah = 0x19;
  intdos(&regs, &regs);
  return(regs.h.al);
#endif
#ifdef _WINNT
  CHAR acBuffer[128];

  utl_get_path(acBuffer);
  return acBuffer[0] - 'A';
#endif
#ifdef _OS2
  ULONG ulDrive,
        ulDriveMap;

  DosQueryCurrentDisk(&ulDrive, &ulDriveMap);
  return (UCHAR)ulDrive - 1;
#endif
}

void utl_set_drive (UCHAR ucNewDrive)
{
#ifdef _MSDOS
  union REGS regs;

  regs.h.ah = 0x0e;
  regs.h.dl = ucNewDrive;
  intdos(&regs, &regs);
#endif
#ifdef _WINNT
  CHAR acPath[3];

  acPath[0] = ucNewDrive + 'A';
  acPath[1] = ':';
  acPath[2] = 0;
  SetCurrentDirectory(acPath);
#endif
#ifdef _OS2
  DosSetDefaultDisk((ULONG)ucNewDrive + 1);
#endif
}

CHAR* utl_get_drives (void)
{
#ifdef _MSDOS
  union REGS  regs;
  UCHAR       ucZaehler,
              ucOldDrive;
  CHAR       *pcReturnVar,
             *pcDummy;

  if (!(pcDummy = pcReturnVar = utl_alloc(79)))
    return(NULL);
  ucOldDrive = utl_get_drive();
  for (ucZaehler = 0; ucZaehler < 26; ucZaehler++)
  {
    utl_set_drive(ucZaehler);
    if (utl_get_drive() == ucZaehler)
    {
      pcDummy[0] = (ucZaehler + 65);
      pcDummy[1] = ':';
      pcDummy[2] = 0;
      pcDummy += 3;
    }
  }
  *pcDummy = 0;
  utl_set_drive(ucOldDrive);

  return pcReturnVar;
#endif
#ifdef _WINNT
  DWORD  dwDrives;
  CHAR  *pcReturnVar,
        *pcCounter,
         cDriveLetter = 'A';

  dwDrives = GetLogicalDrives();
  pcCounter = pcReturnVar = utl_alloc(79);
  while (dwDrives != 0)
  {
    if (dwDrives & 1)
    {
      pcCounter[0] = cDriveLetter++;
      pcCounter[1] = ':';
      pcCounter[2] = 0;
      pcCounter += 3;
    }
    dwDrives >>= 1;
  }
  *pcCounter = 0;

  return pcReturnVar;
#endif
#ifdef _OS2
  ULONG  ulDrive,
         ulDriveMap;
  CHAR  *pcReturnVar,
        *pcCounter,
         cDriveLetter = 'A';

  DosQueryCurrentDisk(&ulDrive, &ulDriveMap);
  pcCounter = pcReturnVar = utl_alloc(79);
  while (ulDriveMap != 0)
  {
    if (ulDriveMap & 1)
    {
      pcCounter[0] = cDriveLetter++;
      pcCounter[1] = ':';
      pcCounter[2] = 0;
      pcCounter += 3;
    }
    ulDriveMap >>= 1;
  }
  *pcCounter = 0;

  return pcReturnVar;
#endif
}

void utl_move_win (void)
{
  BOOL      bMaus       = FALSE,
            bEnde       = FALSE,
            bWinStatus;
  INT       iOffsetX,
            iOffsetY;
  WORD      wButtons;
  INT       iHor,
            iVer;
  UTL_EVENT eventEvent;
  WINDOW    winWindow;

  if (!(winWindow = win_get_active()))
    return;
  if (!winWindow->flFlags.binMoveable)
    return;
  msm_get_buttons(&wButtons, &iHor, &iVer);
  if (wButtons & MSM_B_LEFT)
    bMaus = TRUE;
  bWinStatus = utl_get_win_events();
  utl_set_win_events(FALSE);
  do
  {
    utl_event(&eventEvent);
    switch (eventEvent.uiKind)
    {
      case E_KEY :
        if (!bMaus)
        {
          switch (eventEvent.wKey)
          {
            case K_LEFT :
              win_move(winWindow, winWindow->iX - 1, winWindow->iY);
              break;
            case K_RIGHT :
              win_move(winWindow, winWindow->iX + 1, winWindow->iY);
              break;
            case K_UP :
              win_move(winWindow, winWindow->iX, winWindow->iY - 1);
              break;
            case K_DOWN :
              win_move(winWindow, winWindow->iX, winWindow->iY + 1);
              break;
            case K_ENTER :
              bEnde = TRUE;
              break;
          }
        }
        break;
      case E_MSM_L_UP :
        if (bMaus)
          bEnde = TRUE;
        break;
      case E_MSM_MOVE :
        if (bMaus)
        {
          iOffsetX = eventEvent.iHor - iHor;
          iOffsetY = eventEvent.iVer - iVer;
          if (iOffsetX || iOffsetY)
          {
             win_move(winWindow, winWindow->iX + iOffsetX,
                      winWindow->iY + iOffsetY);
            iHor += iOffsetX;
            iVer += iOffsetY;
          }
        }
        break;
    }
  } while (!bEnde);
  utl_set_win_events(bWinStatus);
}

void utl_size_win (void)
{
  BOOL      bMaus      = FALSE,
            bEnde      = FALSE,
            bWinStatus;
  INT       iOffsetX,
            iOffsetY;
  WORD      wButtons;
  INT       iHor,
            iVer,
            iNewWidth,
            iNewHeight;
  UTL_EVENT eventEvent;
  WINDOW    winWindow;

  if (!(winWindow = win_get_active()))
    return;
  if (!winWindow->flFlags.binSizeable)
    return;
  msm_get_buttons(&wButtons, &iHor, &iVer);
  if (wButtons & MSM_B_LEFT)
    bMaus = TRUE;
  bWinStatus = utl_get_win_events();
  utl_set_win_events(FALSE);
  do
  {
    utl_event(&eventEvent);
    switch (eventEvent.uiKind)
    {
      case E_KEY :
        if (!bMaus)
        {
          switch (eventEvent.wKey)
          {
            case K_LEFT :
              if (winWindow->iWidth > winWindow->iMinWidth)
                win_resize(winWindow, winWindow->iWidth - 1, winWindow->iHeight);
              break;
            case K_RIGHT :
              if (winWindow->iWidth < winWindow->iMaxWidth)
                win_resize(winWindow, winWindow->iWidth + 1, winWindow->iHeight);
              break;
            case K_UP :
              if (winWindow->iHeight > winWindow->iMinHeight)
                win_resize(winWindow, winWindow->iWidth, winWindow->iHeight - 1);
              break;
            case K_DOWN :
              if (winWindow->iHeight < winWindow->iMaxHeight)
                win_resize(winWindow, winWindow->iWidth, winWindow->iHeight + 1);
              break;
            case K_ENTER :
              bEnde = TRUE;
              break;
          }
        }
        break;
      case E_MSM_L_UP :
        if (bMaus)
          bEnde = TRUE;
        break;
      case E_MSM_MOVE :
        if (bMaus)
        {
          iOffsetX = eventEvent.iHor - iHor;
          iOffsetY = eventEvent.iVer - iVer;
          if (iOffsetX || iOffsetY)
          {
            iNewWidth = min(max(winWindow->iWidth + iOffsetX, winWindow->iMinWidth), winWindow->iMaxWidth);
            iNewHeight = min(max(winWindow->iHeight + iOffsetY, winWindow->iMinHeight), winWindow->iMaxHeight);
            iHor += iNewWidth - winWindow->iWidth;
            iVer += iNewHeight - winWindow->iHeight;
            win_resize(winWindow, iNewWidth, iNewHeight);
          }
        }
        break;
    }
  } while (!bEnde);
  utl_set_win_events(bWinStatus);
}

void utl_fill_event (UTL_EVENT *peventEvent)
{
  peventEvent->uiKind = E_DONE;
  msm_get_buttons(&peventEvent->wButtons, &peventEvent->iHor, &peventEvent->iVer);
  peventEvent->dwTimer = utl_get_timer();
  peventEvent->byKeybStatus = utl_keyb_status();
  peventEvent->wKey = peventEvent->uiMessage = peventEvent->ulAddInfo = 0;
  peventEvent->pFrom = NULL;
}

void utl_event (UTL_EVENT *peventEvent)
{
#ifdef _MSDOS
  static WORD   swButtons     = 0,
                wClickButton  = 0;
  static INT    siHor         = 0,
                siVer         = 0,
                iClickHor     = 0,
                iClickVer     = 0;
  static DWORD  sdwLastTimer  = 0,
                dwLastClick   = 0;

         WORD   wButtons;
         INT    iHor,
                iVer;
         BYTE   byKeybStatus;
         BOOL   bWinSave;

  if (bPlayingBeep && !wStatusWord)
  {
    bPlayingBeep = FALSE;
    if (bLoadVOCOnCall)
    {
      utl_free(pBeepVOC);
      pBeepVOC = FALSE;
    }
  }
  if (!sdwLastTimer)
    sdwLastTimer = utl_get_timer();
  peventEvent->uiKind = E_NULL;
  peventEvent->uiMessage = 0;
  peventEvent->pFrom = NULL;
  peventEvent->ulAddInfo = 0;
  peventEvent->dwTimer = utl_get_timer();
  if (msm_cursor())
  {
    msm_get_buttons(&wButtons, &iHor, &iVer);
    peventEvent->wButtons = wButtons;
    peventEvent->iHor = iHor;
    peventEvent->iVer = iVer;
    if (wButtons != swButtons)
    {
      if (!(swButtons & MSM_B_LEFT) && (wButtons & MSM_B_LEFT))
      {
        if (wClickButton == MSM_B_LEFT && peventEvent->dwTimer - dwLastClick &&
            iClickHor == iHor && iClickVer == iVer)
          peventEvent->uiKind = E_MSM_L_DOUBLE;
        else
          peventEvent->uiKind = E_MSM_L_DOWN;
        
        wClickButton = MSM_B_LEFT;
        dwLastClick = peventEvent->dwTimer;
        iClickHor = iHor;
        iClickVer = iVer;
      }
      if ((swButtons & MSM_B_LEFT) && !(wButtons & MSM_B_LEFT))
        peventEvent->uiKind = E_MSM_L_UP;
      if (!(swButtons & MSM_B_RIGHT) && (wButtons & MSM_B_RIGHT))
      {
        if (wClickButton == MSM_B_RIGHT && peventEvent->dwTimer - dwLastClick &&
            iClickHor == iHor && iClickVer == iVer)
          peventEvent->uiKind = E_MSM_R_DOUBLE;
        else
          peventEvent->uiKind = E_MSM_R_DOWN;
        
        wClickButton = MSM_B_RIGHT;
        dwLastClick = peventEvent->dwTimer;
        iClickHor = iHor;
        iClickVer = iVer;
      }
      if ((swButtons & MSM_B_RIGHT) && !(wButtons & MSM_B_RIGHT))
        peventEvent->uiKind = E_MSM_R_UP;
      if (!(swButtons & MSM_B_MIDDLE) && (wButtons & MSM_B_MIDDLE))
      {
        if (wClickButton == MSM_B_MIDDLE && peventEvent->dwTimer - dwLastClick &&
            iClickHor == iHor && iClickVer == iVer)
          peventEvent->uiKind = E_MSM_M_DOUBLE;
        else
          peventEvent->uiKind = E_MSM_M_DOWN;
        
        wClickButton = MSM_B_MIDDLE;
        dwLastClick = peventEvent->dwTimer;
        iClickHor = iHor;
        iClickVer = iVer;
      }
      if ((swButtons & MSM_B_MIDDLE) && !(wButtons & MSM_B_MIDDLE))
        peventEvent->uiKind = E_MSM_M_UP;
      swButtons = wButtons;
    }
    else
      if ((iHor != siHor) || (iVer != siVer))
      {
        peventEvent->uiKind = E_MSM_MOVE;
        siHor = iHor;
        siVer = iVer;
      }
      else
        if (utl_kb_hit())
        {
          peventEvent->uiKind = E_KEY;
          peventEvent->wKey = utl_get_key();
        }
  }
  else
  {
    peventEvent->wButtons = 0;
    peventEvent->iHor = 0;
    peventEvent->iVer = 0;
    if (utl_kb_hit())
    {
      peventEvent->uiKind = E_KEY;
      peventEvent->wKey = utl_get_key();
    }
  }
  if (peventEvent->uiKind == E_KEY && bShortsOn)
    if (int_utl_short_cut(peventEvent->wKey))
      peventEvent->uiKind = E_NULL;
  peventEvent->byKeybStatus = utl_keyb_status();
  if (peventEvent->uiKind != E_NULL)
    sdwLastTimer = peventEvent->dwTimer;
  else
    if (peventEvent->dwTimer - sdwLastTimer >= ulScreenTimeout && !bSaverActive && bSaver)
    {
      bSaverActive = TRUE;
      bWinSave = bWinEvents;
      bWinEvents = FALSE;
      utl_save_screen();
      bWinEvents = bWinSave;
      utl_event(peventEvent);
      bSaverActive = FALSE;
      return;
    }
  if (peventEvent->uiKind == E_NULL && pneFirstEvent)
  {
    if (pneNextEvent)
    {
      pneNextEvent->event_handler();
      pneNextEvent = pneNextEvent->pneNextEvent;
    }
    else
      pneNextEvent = pneFirstEvent;
  }
#endif
#ifdef _WINNT
  static DWORD        dwLastTimer          = 0,
                      dwLastClick          = 0;
  static WORD         wButtons             = 0,
                      wClickButton         = 0;
  static INT          iClickHor            = 0,
                      iClickVer            = 0;

         DWORD        dwEvents;
         INPUT_RECORD ir;
         BOOL         bWinSave;

  utl_fill_event(peventEvent);
  peventEvent->uiKind = E_NULL;

  if (dwLastTimer == 0)
    dwLastTimer = peventEvent->dwTimer;

  GetNumberOfConsoleInputEvents(hStdIn, &dwEvents);
  while (dwEvents)
  {
    ReadConsoleInput(hStdIn, &ir, 1, &dwEvents);
    if (dwEvents == 0)             /* this should not happen! */
      break;

    dwLastTimer = peventEvent->dwTimer;

    switch (ir.EventType)
    {
      case KEY_EVENT :
        peventEvent->wKey = int_utl_translate_key(&ir.Event.KeyEvent);
        if (peventEvent->wKey != 0)
          if (!int_utl_short_cut(peventEvent->wKey))
          {
            peventEvent->uiKind = E_KEY;
            return;
          }
        break;
      case MOUSE_EVENT :
        msm_process_input_event(&ir.Event.MouseEvent);
        msm_get_buttons(&peventEvent->wButtons, &peventEvent->iHor, &peventEvent->iVer);
        if (ir.Event.MouseEvent.dwEventFlags == MOUSE_MOVED)
        {
          peventEvent->uiKind = E_MSM_MOVE;
          return;
        }
        if ((wButtons & MSM_B_LEFT) != (peventEvent->wButtons & MSM_B_LEFT))
        {
          if (wButtons & MSM_B_LEFT)
            peventEvent->uiKind = E_MSM_L_UP;
          else
          {
            if (wClickButton == MSM_B_LEFT && peventEvent->dwTimer - dwLastClick <= ulDoubleClick &&
                iClickHor == peventEvent->iHor && iClickVer == peventEvent->iVer)
              peventEvent->uiKind = E_MSM_L_DOUBLE;
            else
              peventEvent->uiKind = E_MSM_L_DOWN;
          
            wClickButton = MSM_B_LEFT;
            dwLastClick = peventEvent->dwTimer;
            iClickHor = peventEvent->iHor;
            iClickVer = peventEvent->iVer;
          }

          wButtons = peventEvent->wButtons;
          return;
        }
        if ((wButtons & MSM_B_RIGHT) != (peventEvent->wButtons & MSM_B_RIGHT))
        {
          if (wButtons & MSM_B_RIGHT)
            peventEvent->uiKind = E_MSM_R_UP;
          else
          {
            if (wClickButton == MSM_B_RIGHT && peventEvent->dwTimer - dwLastClick <= ulDoubleClick &&
                iClickHor == peventEvent->iHor && iClickVer == peventEvent->iVer)
              peventEvent->uiKind = E_MSM_R_DOUBLE;
            else
              peventEvent->uiKind = E_MSM_R_DOWN;
          
            wClickButton = MSM_B_RIGHT;
            dwLastClick = peventEvent->dwTimer;
            iClickHor = peventEvent->iHor;
            iClickVer = peventEvent->iVer;
          }

          wButtons = peventEvent->wButtons;
          return;
        }
        if ((wButtons & MSM_B_MIDDLE) != (peventEvent->wButtons & MSM_B_MIDDLE))
        {
          if (wButtons & MSM_B_MIDDLE)
            peventEvent->uiKind = E_MSM_M_UP;
          else
          {
            if (wClickButton == MSM_B_MIDDLE && peventEvent->dwTimer - dwLastClick <= ulDoubleClick &&
                iClickHor == peventEvent->iHor && iClickVer == peventEvent->iVer)
              peventEvent->uiKind = E_MSM_M_DOUBLE;
            else
              peventEvent->uiKind = E_MSM_M_DOWN;
          
            wClickButton = MSM_B_MIDDLE;
            dwLastClick = peventEvent->dwTimer;
            iClickHor = peventEvent->iHor;
            iClickVer = peventEvent->iVer;
          }

          wButtons = peventEvent->wButtons;
          return;
        }
        break;
    }
  }

  if (peventEvent->dwTimer - dwLastTimer >= ulScreenTimeout && !bSaverActive && bSaver)
  {
    bSaverActive = TRUE;
    bWinSave = bWinEvents;
    bWinEvents = FALSE;
    utl_save_screen();
    bWinEvents = bWinSave;
    utl_event(peventEvent);
    bSaverActive = FALSE;
    return;
  }

  if (pneFirstEvent)
  {
    if (pneNextEvent)
    {
      pneNextEvent->event_handler();
      pneNextEvent = pneNextEvent->pneNextEvent;
    }
    else
      pneNextEvent = pneFirstEvent;
  }
#endif
#ifdef _OS2
  static DWORD        dwLastTimer  = 0,
                      dwLastClick  = 0;
  static WORD         wButtons     = 0,
                      wClickButton = 0;
  static INT          iLastHor     = 0,
                      iLastVer     = 0,
                      iClickHor    = 0,
                      iClickVer    = 0;

         BOOL         bWinSave;
         MOUQUEINFO   mqi;
         MOUEVENTINFO mei;
         USHORT       usWait;

  utl_fill_event(peventEvent);
  peventEvent->uiKind = E_NULL;

  if (dwLastTimer == 0)
    dwLastTimer = peventEvent->dwTimer;

  if (utl_kb_hit())
  {
    peventEvent->wKey = utl_get_key();
    dwLastTimer = peventEvent->dwTimer;
    if (!int_utl_short_cut(peventEvent->wKey))
    {
      peventEvent->uiKind = E_KEY;
      return;
    }
  }

  if (msm_cursor())
  {
    MouGetNumQueEl(&mqi, hmouMouse);
    if (mqi.cEvents != 0)
    {
      usWait = 1;
      MouReadEventQue(&mei, &usWait, hmouMouse);
      msm_process_input_event(&mei);
      msm_get_buttons(&peventEvent->wButtons, &peventEvent->iHor, &peventEvent->iVer);
      if ((wButtons & MSM_B_LEFT) != (peventEvent->wButtons & MSM_B_LEFT))
      {
        if (wButtons & MSM_B_LEFT)
          peventEvent->uiKind = E_MSM_L_UP;
        else
        {
          if (wClickButton == MSM_B_LEFT && peventEvent->dwTimer - dwLastClick <= ulDoubleClick &&
              iClickHor == peventEvent->iHor && iClickVer == peventEvent->iVer)
            peventEvent->uiKind = E_MSM_L_DOUBLE;
          else
            peventEvent->uiKind = E_MSM_L_DOWN;
          
          wClickButton = MSM_B_LEFT;
          dwLastClick = peventEvent->dwTimer;
          iClickHor = peventEvent->iHor;
          iClickVer = peventEvent->iVer;
        }

        dwLastTimer = peventEvent->dwTimer;
        wButtons = peventEvent->wButtons;
        return;
      }
      if ((wButtons & MSM_B_RIGHT) != (peventEvent->wButtons & MSM_B_RIGHT))
      {
        if (wButtons & MSM_B_RIGHT)
          peventEvent->uiKind = E_MSM_R_UP;
        else
        {
          if (wClickButton == MSM_B_RIGHT && peventEvent->dwTimer - dwLastClick <= ulDoubleClick &&
              iClickHor == peventEvent->iHor && iClickVer == peventEvent->iVer)
            peventEvent->uiKind = E_MSM_R_DOUBLE;
          else
            peventEvent->uiKind = E_MSM_R_DOWN;
          
          wClickButton = MSM_B_RIGHT;
          dwLastClick = peventEvent->dwTimer;
          iClickHor = peventEvent->iHor;
          iClickVer = peventEvent->iVer;
        }

        dwLastTimer = peventEvent->dwTimer;
        wButtons = peventEvent->wButtons;
        return;
      }
      if ((wButtons & MSM_B_MIDDLE) != (peventEvent->wButtons & MSM_B_MIDDLE))
      {
        if (wButtons & MSM_B_MIDDLE)
          peventEvent->uiKind = E_MSM_M_UP;
        else
        {
          if (wClickButton == MSM_B_MIDDLE && peventEvent->dwTimer - dwLastClick <= ulDoubleClick &&
              iClickHor == peventEvent->iHor && iClickVer == peventEvent->iVer)
            peventEvent->uiKind = E_MSM_M_DOUBLE;
          else
            peventEvent->uiKind = E_MSM_M_DOWN;
          
          wClickButton = MSM_B_MIDDLE;
          dwLastClick = peventEvent->dwTimer;
          iClickHor = peventEvent->iHor;
          iClickVer = peventEvent->iVer;
        }

        dwLastTimer = peventEvent->dwTimer;
        wButtons = peventEvent->wButtons;
        return;
      }
    }
    if (peventEvent->iHor != iLastHor || peventEvent->iVer != iLastVer)
    {
      peventEvent->uiKind = E_MSM_MOVE;
      iLastHor = peventEvent->iHor;
      iLastVer = peventEvent->iVer;
      dwLastTimer = peventEvent->dwTimer;
      return;
    }
  }

  if (peventEvent->dwTimer - dwLastTimer >= ulScreenTimeout && !bSaverActive && bSaver)
  {
    bSaverActive = TRUE;
    bWinSave = bWinEvents;
    bWinEvents = FALSE;
    utl_save_screen();
    bWinEvents = bWinSave;
    utl_event(peventEvent);
    bSaverActive = FALSE;
    return;
  }

  if (pneFirstEvent)
  {
    if (pneNextEvent)
    {
      pneNextEvent->event_handler();
      pneNextEvent = pneNextEvent->pneNextEvent;
    }
    else
      pneNextEvent = pneFirstEvent;
  }
#endif
}

/*
Local Variables:
compile-command: "wmake -f stools.mk -h -e"
End:
*/
