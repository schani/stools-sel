/***************************************************************************
 ***************************************************************************
 ***                                                                     ***
 ***                       Schani-Tools fÅr C                            ***
 ***                                                                     ***
 ***                          Window-Teil                                ***
 ***                                                                     ***
 ***                (c) 1990-93 by Schani Electronics                    ***
 ***                                                                     ***
 ***************************************************************************
 ***************************************************************************/

#include <gemein.h>
#include <video.h>
#include <mouse.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <conio.h>
#include <sthelp.h>
#include <dlg.h>
#include <window.h>
#include <utility.h>
#include <global.h>
#include <contain.h>
#include <menu.h>
#include <status.h>
#ifdef _MSDOS
#include <mem.h>
#endif
#include <stdio.h>
#ifndef _WATCOM
#ifdef _MSDOS
#include <alloc.h>
#endif
#else
#include <malloc.h>
#endif

extern ULONG       ulBeepFrequency;
extern ULONG       ulBeepDuration;
extern BOOL        bBeep;
extern CHAR        aacBorder[21][8];
extern GLB_PROGRAM prgProgram;
extern BOOL        bExit;
extern CHAR        apcColors[];
extern INT         iSizeX;
extern INT         iSizeY;
#if !defined(_WINNT) && !defined(_OS2)
extern BOOL        bVOCBeep;
extern BOOL        bSoundBlaster;
#endif

WINDOW        winInfoWindow   = NULL,
              winDesktop      = NULL,
              winWorking      = NULL;
WINDOW_CLASS *pwcFirst        = NULL;
UINT          uiWinsActive    = 0;
CHARACTER    *pcharSave       = NULL, 
             *pcharMoveBuffer = NULL;
INT           iTab            = 8;
BOOL          bTossing        = FALSE,
              bUpdating       = FALSE;

/***************************************************************************
 *                         Interne Funktionen                              *
 ***************************************************************************/

BOOL static int_win_overlapping (WINDOW win1, WINDOW win2)
{
  if ((win1->iX < win2->iX && win1->iX + REAL_WIDTH(win1) > win2->iX) ||
      (win1->iX >= win2->iX && win1->iX < win2->iX + REAL_WIDTH(win2)))
    if ((win1->iY < win2->iY && win1->iY + REAL_HEIGHT(win1) > win2->iY) ||
        (win1->iY >= win2->iY && win1->iY < win2->iY + REAL_HEIGHT(win2)))
      return TRUE;
  return FALSE;
}

void static int_win_make_overlap (WINDOW winWindow, INT *piX, INT *piY, INT *piWidth, INT *piHeight)
{
  INT   iNewX,
        iNewY,
        iNewWidth,
        iNewHeight;

  if ((winWindow->iX < *piX &&
       winWindow->iX + REAL_WIDTH(winWindow) > *piX) ||
      (winWindow->iX >= *piX &&
       winWindow->iX < *piX + *piWidth))
    if ((winWindow->iY < *piY &&
         winWindow->iY + REAL_HEIGHT(winWindow) > *piY) ||
        (winWindow->iY >= *piY &&
         winWindow->iY < *piY + *piHeight))
    {
      iNewX = max(*piX, winWindow->iX);
      iNewY = max(*piY, winWindow->iY);
      iNewWidth = min(*piWidth - iNewX + *piX,
                      REAL_WIDTH(winWindow) - iNewX + winWindow->iX);
      iNewHeight = min(*piHeight - iNewY + *piY,
                       REAL_HEIGHT(winWindow) - iNewY + winWindow->iY);
      if (iNewX <= 0)
      {
        if (-iNewX + 1 >= iNewWidth)
          iNewWidth = 0;
        else
          iNewWidth += iNewX - 1;
        iNewX = 1;
      }
      if (iNewY <= 0)
      {
        if (-iNewY + 1 >= iNewHeight)
          iNewHeight = 0;
        else
          iNewHeight += iNewY - 1;
        iNewY = 1;
      }
      if (iNewX + iNewWidth > iSizeX + 1)
      {
        if (iNewX > iSizeX)
          iNewWidth = 0;
        else
          iNewWidth = iSizeX + 1 - iNewX;
      }
      if (iNewY + iNewHeight > iSizeY + 1)
      {
        if (iNewY > iSizeY)
          iNewHeight = 0;
        else
          iNewHeight = iSizeY + 1 - iNewY;
      }
      *piX = iNewX;
      *piY = iNewY;
      *piWidth = iNewWidth;
      *piHeight = iNewHeight;
      return;
    }
  *piWidth = 0;
  *piHeight = 0;
}

void static int_win_make_region (INT iX1, INT iY1, INT iWidth1, 
                                 INT iHeight1, INT iX2, INT iY2, 
                                 INT iWidth2, INT iHeight2, INT *piX,
                                 INT *piY, INT *piWidth, INT *piHeight)
{
  *piX = max(min(iX1, iX2), 1);
  *piY = max(min(iY1, iY2), 1);
  *piWidth = min(max(iX1 + iWidth1, iX2 + iWidth2), iSizeX + 1);
  *piHeight = min(max(iY1 + iHeight1, iY2 + iHeight2), iSizeY + 1);
  *piWidth -= *piX;
  *piHeight -= *piY;
}

void static int_win_get_region (CHARACTER *pcharBuffer, INT iX, INT iY,
                                INT iWidth, INT iHeight)
{
  INT iCounter;

  pcharBuffer += (iX - 1) + (iY - 1) * iSizeX;
  for (iCounter = 0; iCounter < iHeight; iCounter++)
  {
    vio_lw(iX, iY + iCounter, iWidth, 1, pcharBuffer);
    pcharBuffer += iSizeX;
  }
}

void static int_win_put_region (CHARACTER *pcharBuffer, INT iX, INT iY,
                                INT iWidth, INT iHeight)
{
  INT iCounter;

  pcharBuffer += (iX - 1) + (iY - 1) * iSizeX;
  for (iCounter = 0; iCounter < iHeight; iCounter++)
  {
    vio_sw(iX, iY + iCounter, iWidth, 1, pcharBuffer);
    pcharBuffer += iSizeX;
  }
}

void static int_win_put_window (WINDOW winWindow)
{
  INT        iNewX            = 1,
             iNewY            = 1,
             iCounterX,
             iCounterY,
             iRightX,
             iBottomY,
             iNewWidth        = iSizeX,
             iNewHeight       = iSizeY,
             iCounterPlus;
  CHARACTER *pcharCounter,
            *pcharBuffCounter;

  iNewX = max(winWindow->iX, 1);
  iNewY = max(winWindow->iY, 1);
  iRightX = min(winWindow->iX + REAL_WIDTH(winWindow) - 1, iSizeX);
  iBottomY = min(winWindow->iY + REAL_HEIGHT(winWindow) - 1, iSizeY);
  if (iNewX > iRightX || iRightX <= 0)
    return;
  if (iNewY > iBottomY || iBottomY <= 0)
    return;
  iNewWidth = iRightX - iNewX + 1;
  iNewHeight = iBottomY - iNewY + 1;
  if (iNewWidth == 0 || iNewHeight == 0)
    return;
  pcharBuffCounter = pcharSave;
  memset(pcharSave, 0xff, iNewWidth * iNewHeight * sizeof(CHARACTER));
  pcharCounter = winWindow->pcharSave + iNewX - winWindow->iX + (iNewY - winWindow->iY) * REAL_WIDTH(winWindow);
  iCounterPlus = REAL_WIDTH(winWindow) - iNewWidth;
  for (iCounterY = iNewY; iCounterY < iNewY + iNewHeight; iCounterY++)
  {
    for (iCounterX = iNewX; iCounterX < iNewX + iNewWidth; iCounterX++)
    {
      if (pcharCounter->character != (CHAR)0xff && pcharCounter->character != 0x00)
      {
        pcharBuffCounter->character = pcharCounter->character;
        pcharCounter->character = (CHAR)0xff;
      }
      if (pcharCounter->attribute != (CHAR)0xff && pcharCounter->attribute != 0x00)
      {
        pcharBuffCounter->attribute = pcharCounter->attribute;
        pcharCounter->attribute = (CHAR)0xff;
      }
      pcharCounter++;
      pcharBuffCounter++;
    }
    pcharCounter += iCounterPlus;
  }
  vio_sw_ff(iNewX, iNewY, iNewWidth, iNewHeight, pcharSave);
}

void static int_win_get_window (WINDOW winWindow)
{
  INT        iNewX            = 1,
             iNewY            = 1,
             iCounterX,
             iCounterY,
             iRightX,
             iBottomY,
             iNewWidth        = iSizeX,
             iNewHeight       = iSizeY,
             iCounterPlus;
  CHARACTER *pcharCounter,
            *pcharBuffCounter;

  iNewX = max(winWindow->iX, 1);
  iNewY = max(winWindow->iY, 1);
  iRightX = min(winWindow->iX + REAL_WIDTH(winWindow) - 1, iSizeX);
  iBottomY = min(winWindow->iY + REAL_HEIGHT(winWindow) - 1, iSizeY);
  if (iNewX > iRightX || iRightX <= 0)
    return;
  if (iNewY > iBottomY || iBottomY <= 0)
    return;
  iNewWidth = iRightX - iNewX + 1;
  iNewHeight = iBottomY - iNewY + 1;
  if (iNewWidth == 0 || iNewHeight == 0)
    return;
  pcharBuffCounter = pcharSave;
  vio_lw(iNewX, iNewY, iNewWidth, iNewHeight, pcharSave);
  pcharCounter = winWindow->pcharSave + iNewX - winWindow->iX + (iNewY - winWindow->iY) * REAL_WIDTH(winWindow);
  iCounterPlus = REAL_WIDTH(winWindow) - iNewWidth;
  for (iCounterY = iNewY; iCounterY < iNewY + iNewHeight; iCounterY++)
  {
    for (iCounterX = iNewX; iCounterX < iNewX + iNewWidth; iCounterX++)
    {
      if (pcharCounter->character == (CHAR)0xff)
        pcharCounter->character = pcharBuffCounter->character;
      if (pcharCounter->attribute == (CHAR)0xff)
        pcharCounter->attribute = pcharBuffCounter->attribute;
      pcharCounter++;
      pcharBuffCounter++;
    }
    pcharCounter += iCounterPlus;
  }
}

void static int_win_save_region (WINDOW winWindow, INT iX, INT iY,
                                 INT iWidth, INT iHeight, UCHAR ucFlags)
{
  INT        iNewWidth        = iWidth,
             iNewHeight       = iHeight,
             iCounterPlus,
             iCounterX,
             iCounterY,
             iNewX            = iX,
             iNewY            = iY;
  CHARACTER *pcharCounter,
            *pcharBuffCounter;

  int_win_make_overlap(winWindow, &iNewX, &iNewY, &iNewWidth, &iNewHeight);
  if (iNewWidth == 0 || iNewHeight == 0)
    return;
  pcharBuffCounter = pcharSave;
  vio_lw(iNewX, iNewY, iNewWidth, iNewHeight, pcharSave);
  pcharCounter = winWindow->pcharSave + iNewX - winWindow->iX + (iNewY - winWindow->iY) * REAL_WIDTH(winWindow);
  iCounterPlus = REAL_WIDTH(winWindow) - iNewWidth;
  for (iCounterY = iNewY; iCounterY < iNewY + iNewHeight; iCounterY++)
  {
    for (iCounterX = iNewX; iCounterX < iNewX + iNewWidth; iCounterX++)
    {
      if (ucFlags & WIN_CHAR)
        if (pcharCounter->character == (CHAR)0xff)
          pcharCounter->character = pcharBuffCounter->character;
      if (ucFlags & WIN_ATTRI)
        if (pcharCounter->attribute == (CHAR)0xff)
          pcharCounter->attribute = pcharBuffCounter->attribute;
      pcharCounter++;
      pcharBuffCounter++;
    }
    pcharCounter += iCounterPlus;
  }
}

void static int_win_save_windows (WINDOW winWindow)
{
  WINDOW winCounter;

  if (uiWinsActive <= 1)
    return;
  for (winCounter = prgProgram.winLast->winPrev; winCounter;
       winCounter = winCounter->winPrev)
    if (int_win_overlapping(winWindow, winCounter))
    {
      if (winWindow->flFlags.binShadow)
      {
        int_win_save_region(winCounter, winWindow->iX,
                            winWindow->iY, winWindow->iWidth,
                            winWindow->iHeight, WIN_CHAR | WIN_ATTRI);
        int_win_save_region(winCounter, 
                            winWindow->iX + winWindow->iWidth,
                            winWindow->iY + 1, 2,
                            winWindow->iHeight, WIN_ATTRI);
        int_win_save_region(winCounter, winWindow->iX + 2,
                            winWindow->iY + winWindow->iHeight, 
                            winWindow->iWidth - 2, 1, WIN_ATTRI);
      }
      else
        int_win_save_region(winCounter, winWindow->iX,
                            winWindow->iY, winWindow->iWidth,
                            winWindow->iHeight, WIN_CHAR | WIN_ATTRI);
    }
}

void static int_win_write_region (WINDOW winWindow, INT iX, INT iY, INT iWidth, INT iHeight, INT iBufferWidth,
                                  INT iBufferX, INT iBufferY, CHARACTER *pcharBuffer, UCHAR ucFlags)
{
  INT        iNewWidth        = iWidth,
             iNewHeight       = iHeight,
             iCounterPlus,
             iBufferPlus,
             iCounterX,
             iCounterY,
             iNewX            = iX,
             iNewY            = iY;
  CHARACTER *pcharCounter,
            *pcharBuffCounter;

  int_win_make_overlap(winWindow, &iNewX, &iNewY, &iNewWidth, &iNewHeight);
  if (iNewWidth == 0 || iNewHeight == 0)
    return;
  pcharBuffCounter = pcharSave;
  memset(pcharSave, 0xff, iNewWidth * iNewHeight * sizeof(CHARACTER));
  pcharCounter = winWindow->pcharSave + iNewX - winWindow->iX + (iNewY - winWindow->iY) * REAL_WIDTH(winWindow);
  pcharBuffer += iNewX + iBufferX - iX + (iNewY + iBufferY - iY) * iBufferWidth;
  iCounterPlus = REAL_WIDTH(winWindow) - iNewWidth;
  iBufferPlus = iBufferWidth - iNewWidth;
  for (iCounterY = iNewY; iCounterY < iNewY + iNewHeight; iCounterY++)
  {
    for (iCounterX = iNewX; iCounterX < iNewX + iNewWidth; iCounterX++)
    {
      if (ucFlags & WIN_CHAR)
        if (pcharCounter->character != (CHAR)0xff && pcharBuffer->character != 0x00 &&
      pcharCounter->character != 0x00)
        {
          pcharBuffCounter->character = pcharCounter->character;
          pcharCounter->character = (CHAR)0xff;
          pcharBuffer->character = 0x00;
        }
      if (ucFlags & WIN_ATTRI)
        if (pcharCounter->attribute != (CHAR)0xff && pcharBuffer->attribute != 0x00 &&
      pcharCounter->attribute != 0x00)
        {
          pcharBuffCounter->attribute = pcharCounter->attribute;
          pcharCounter->attribute = (CHAR)0xff;
          pcharBuffer->attribute = 0x00;
        }
      pcharCounter++;
      pcharBuffCounter++;
      pcharBuffer++;
    }
    pcharCounter += iCounterPlus;
    pcharBuffer += iBufferPlus;
  }
  vio_sw_ff(iNewX, iNewY, iNewWidth, iNewHeight, pcharSave);
}

BOOL static int_win_write_windows (WINDOW winWindow)
{
  WINDOW     winCounter;
  CHARACTER *pcharBuffer;

  if (uiWinsActive <= 1)
    return TRUE;
  if (!(pcharBuffer = utl_alloc(REAL_WIDTH(winWindow) * REAL_HEIGHT(winWindow) * sizeof(CHARACTER))))
    return FALSE;
  memcpy(pcharBuffer, winWindow->pcharSave, REAL_WIDTH(winWindow) * REAL_HEIGHT(winWindow) * sizeof(CHARACTER));
  for (winCounter = prgProgram.winLast->winPrev; winCounter;
       winCounter = winCounter->winPrev)
    if (int_win_overlapping(winWindow, winCounter))
      int_win_write_region(winCounter, winWindow->iX,
                           winWindow->iY, REAL_WIDTH(winWindow),
                           REAL_HEIGHT(winWindow), REAL_WIDTH(winWindow), 0,
                           0, pcharBuffer, WIN_CHAR | WIN_ATTRI);
  utl_free(pcharBuffer);
  return TRUE;
}

void static int_win_set_ff (WINDOW winWindow, INT iX, INT iY, INT iWidth, INT iHeight, UCHAR ucFlags)
{
  CHARACTER *pcharBuffer;
  INT        iCounterX,
             iCounterY,
             iCounterPlus;

  pcharBuffer = winWindow->pcharSave + iY * REAL_WIDTH(winWindow) + iX;
  iCounterPlus = REAL_WIDTH(winWindow) - iWidth;
  for (iCounterY = 0; iCounterY < iHeight; iCounterY++)
  {
    for (iCounterX = 0; iCounterX < iWidth; iCounterX++)
    {
      if (ucFlags & WIN_CHAR)
        pcharBuffer->character = 0x00;
      if (ucFlags & WIN_ATTRI)
        pcharBuffer->attribute = 0x00;
      pcharBuffer++;
    }
    pcharBuffer += iCounterPlus;
  }
}

void static int_win_border_title (WINDOW winWindow)
{
  MOUSE;
  CHAR   *pcTitle;

  BEGIN_MOUSE_IN(winWindow);
  pcTitle = utl_alloc(iSizeX + 1);
  winWindow->flFlags.binDrawCheck = FALSE;
  win_draw_border(winWindow, 0, 0, winWindow->iWidth, winWindow->iHeight, 
                  winWindow->flFlags.binBorderType,
                  win_get_color(winWindow, PAL_COL_BORDER));
  if (winWindow->pcTitle)
  {
    strcpy(pcTitle, winWindow->pcTitle);
    pcTitle[winWindow->iWidth - 2] = 0;
    switch (winWindow->flFlags.binTitlePos)
    {
      case TITLE_T_C :
        win_ss_a(winWindow, (winWindow->iWidth - strlen(pcTitle)) >> 1, 0, pcTitle,
     win_get_color(winWindow, PAL_COL_TITLE));
        break;
      case TITLE_T_L :
        win_ss_a(winWindow, 1, 0, pcTitle, win_get_color(winWindow, PAL_COL_TITLE));
        break;
      case TITLE_T_R :
        win_ss_a(winWindow, winWindow->iWidth - strlen(pcTitle) - 1, 0, pcTitle,
     win_get_color(winWindow, PAL_COL_TITLE));
        break;
      case TITLE_B_C :
        win_ss_a(winWindow, (winWindow->iWidth - strlen(pcTitle)) >> 1, winWindow->iHeight - 1, pcTitle,
                 win_get_color(winWindow, PAL_COL_TITLE));
        break;
      case TITLE_B_L :
        win_ss_a(winWindow, 1, winWindow->iHeight - 1, pcTitle, win_get_color(winWindow, PAL_COL_TITLE));
        break;
      case TITLE_B_R :
        win_ss_a(winWindow, winWindow->iWidth - strlen(pcTitle) - 1, winWindow->iHeight - 1, pcTitle,
                 win_get_color(winWindow, PAL_COL_TITLE));
        break;
    }
  }
  if (winWindow->flFlags.binCloseable)
    win_ss_a(winWindow, 2, 0, "\\\\", win_get_color(winWindow, PAL_COL_ICON));
  if (winWindow->flFlags.binSizeable)
  {
    if (winWindow->flFlags.binMaximized)
      win_ss_a(winWindow, winWindow->iWidth - 5, 0, "\\\\", win_get_color(winWindow, PAL_COL_ICON));
    else
      win_ss_a(winWindow, winWindow->iWidth - 5, 0, "\\\\", win_get_color(winWindow, PAL_COL_ICON));
    win_s_za(winWindow, winWindow->iWidth - 1, winWindow->iHeight - 1, 'X',
       win_get_color(winWindow, PAL_COL_ICON));        
  }
  END_MOUSE;
  winWindow->flFlags.binDrawCheck = TRUE;
  utl_free(pcTitle);
}

BOOL static int_win_front (WINDOW winWindow)
{
  WINDOW     winCounter;
  CHARACTER *pcharVirScreen;
  CNT_STACK *psStack;
  MOUSE;

  BEGIN_MOUSE;
  bTossing = TRUE;
  if (!(pcharVirScreen = utl_alloc(iSizeX * iSizeY * sizeof(CHARACTER))))
    return FALSE;
  vio_lw(1, 1, iSizeX, iSizeY, pcharVirScreen);
  vio_set_vir_page(pcharVirScreen);
  psStack = cnt_stack_make();
  for (winCounter = prgProgram.winLast; winCounter != winWindow;
       winCounter = prgProgram.winLast)
  {
    cnt_stack_put(psStack, (ULONG)winCounter);
    if (!win_hide(winCounter))
      return FALSE;
  }
  win_hide(winWindow);
  for (; !cnt_stack_is_empty(psStack);)
    if (!win_show((WINDOW)cnt_stack_get(psStack)))
      return FALSE;
  win_show(winWindow);
  cnt_stack_destroy(psStack);
  vio_set_phys_page();
  vio_sw(1, 1, iSizeX, iSizeY, pcharVirScreen);
  utl_free(pcharVirScreen);
  bTossing = FALSE;
  END_MOUSE;
  return TRUE;
}

void static int_win_printf_output (WINDOW winWindow, CHAR *pcString)
{
  INT iX,
      iY,
      iCounter;
  MOUSE;

  BEGIN_MOUSE_IN(winWindow);
  for (; *pcString; pcString++)
    switch (*pcString)
    {
      case '\a' :
        utl_beep();
        break;
      case '\v' :
        break;
      case '\b' :
        win_get_cursor_pos(winWindow, &iX, &iY);
        if (iX > 1)
          win_set_cursor_pos(winWindow, iX - 1, iY);
        break;
      case '\r' :
        win_get_cursor_pos(winWindow, &iX, &iY);
        win_set_cursor_pos(winWindow, 1, iY);
        break;
      case '\t' :
        for (iCounter = 0; iCounter < iTab; iCounter++)
          win_printch(winWindow, ' ');
        break;
      case '\n' :
        win_get_cursor_pos(winWindow, &iX, &iY);
        iX = 1;
        iY++;
        if (iY > winWindow->iHeight - 2)
        {
          win_scroll_up(winWindow, 1);
          iY = winWindow->iHeight - 2;
        }
        win_set_cursor_pos(winWindow, iX, iY);
        break;
      case '\f' :
        win_scroll_up(winWindow, 0);
        win_set_cursor_pos(winWindow, 1, winWindow->iHeight - 2);
        break;
      default :
        win_printch(winWindow, *pcString);
    }
  END_MOUSE;
}

void static int_win_desktop_handler (WINDOW winWindow, UTL_EVENT *peventEvent)
{                       
  switch (peventEvent->uiKind)
  {
    case E_MESSAGE :
      switch (peventEvent->uiMessage)
      {
        case M_REDRAW :
          mnu_redraw();
          sts_redraw();
          dsk_sw_za(1, 2, iSizeX, iSizeY - 1, '±', dsk_get_color(PAL_COL_BACKGROUND));
          peventEvent->uiKind = E_DONE;
          break;                
      }
      break;
    default :
      mnu_event(peventEvent);
      if (peventEvent->uiKind != E_DONE)
        sts_event(peventEvent);
      break;
  }
}

static WINDOW_CLASS* int_win_get_class (CHAR *pcName)
{
  WINDOW_CLASS *pwc;

  if (pcName == NULL)
    pcName = "standard_dialogue";
  for (pwc = pwcFirst; pwc; pwc = pwc->pwcNext)
    if (!strncmp(pcName, pwc->acName, WIN_MAX_CLASS_NAME))
      return pwc;
  return NULL;
}

static BOOL int_win_is_class (WINDOW winWindow, ULONG ulClass)
{
  if (strcmp(winWindow->pwcClass->acName, (CHAR*)ulClass))
    return FALSE;
  return TRUE;
}

/***************************************************************************
 *                       Allgemeine Funktionen                             *
 ***************************************************************************/

void win_init (void)
{
  INT           iX,
                iY,
                iPercent;
  WINDOW_CLASS *pwc;

  if (!(pcharSave = utl_alloc(iSizeX * iSizeY * sizeof(CHARACTER))))
    return;
  if (!(pcharMoveBuffer = utl_alloc(iSizeX * iSizeY * sizeof(CHARACTER))))
    return;

  win_register_class("standard_dialogue", PAL_WIN_DLG, B_EEEE, TITLE_T_C,
                     WIN_FL_SHADOW | WIN_FL_CURSOR | WIN_FL_MOVEABLE | WIN_FL_MODAL | WIN_FL_FOCUSABLE,
                     win_std_handler);
  win_register_class("desktop", PAL_WIN_DESKTOP, 0, 0, 0, int_win_desktop_handler);

  pwc = int_win_get_class("desktop");
  if (!(winDesktop = utl_alloc(sizeof(WB))))
    return;
  winDesktop->iX = 0;
  winDesktop->iY = 0;
  winDesktop->iWidth = iSizeX + 2;
  winDesktop->iHeight = iSizeY + 2;
  vio_get_cursor(&iX, &iY, &iPercent);
  winDesktop->iCursX = iX;
  winDesktop->iCursY = iY;
  winDesktop->iCursPercent = iPercent;
  winDesktop->pcTitle = NULL;
  winDesktop->handle_event = pwc->handle_event;
  winDesktop->pelementFirst = winDesktop->pelementLast = winDesktop->pelementActive = NULL;
  winDesktop->redraw_func = NULL;
  winDesktop->pcColors = pwc->pcColors;
  if (!(winDesktop->pcharSave = utl_alloc(winDesktop->iWidth * winDesktop->iHeight * sizeof(CHARACTER))))
    return;
  memset(winDesktop->pcharSave, 0xff, winDesktop->iWidth * winDesktop->iHeight * sizeof(CHARACTER));
  winDesktop->flFlags = pwc->flFlags;
  winDesktop->ulAddInfo = 0;
  winDesktop->pwcClass = pwc;
  winDesktop->winNext = winDesktop->winPrev = NULL;
  prgProgram.winFirst = prgProgram.winLast = prgProgram.winFocussed = winDesktop;
  glb_send_message(&prgProgram, winDesktop, M_INIT, 0);
  glb_send_message(&prgProgram, winDesktop, M_SHOW, 0);
  glb_send_message(&prgProgram, winDesktop, M_GET_FOCUS, 0);
  uiWinsActive++;
  win_update_cursor();
}

void win_draw_elements (WINDOW winWindow)
{
  DLG_ELEMENT *pelementCounter;

  for (pelementCounter = winWindow->pelementFirst; pelementCounter;
       pelementCounter = pelementCounter->pelementNext)
    glb_send_message(winWindow, pelementCounter, M_DRAW, 0);
}    

void win_add_element (WINDOW winWindow, DLG_ELEMENT *pelementElement)
{
  win_insert_element(winWindow, pelementElement, winWindow->pelementLast);
}

void win_insert_element (WINDOW winWindow, DLG_ELEMENT *pelementElement, DLG_ELEMENT *pelementAfter)
{
  pelementElement->winOwner = winWindow;
  if (pelementAfter)
  {
    pelementElement->pelementPrev = pelementAfter;
    pelementElement->pelementNext = pelementAfter->pelementNext;
    if (pelementAfter->pelementNext)
      pelementAfter->pelementNext->pelementPrev = pelementElement;
    pelementAfter->pelementNext = pelementElement;
  }
  else
  {
    pelementElement->pelementPrev = NULL;
    pelementElement->pelementNext = winWindow->pelementFirst;
    if (winWindow->pelementFirst)
      winWindow->pelementFirst->pelementPrev = pelementElement;
    winWindow->pelementFirst = pelementElement;
  }
  if (!pelementElement->pelementNext)
    winWindow->pelementLast = pelementElement;
  glb_send_message(winWindow, pelementElement, M_INIT, 0);
  if (!winWindow->pelementActive &&
      pelementElement->flFlags.binCanBeActivated)
  {
    winWindow->pelementActive = pelementElement;    
    pelementElement->flFlags.binFocussed = TRUE;
    glb_send_message(winWindow, pelementElement, M_GET_FOCUS, 0);
  }
}

void win_delete_element (WINDOW winWindow, DLG_ELEMENT *pelementElement)
{
  DLG_ELEMENT *pelementCounter;

  if (winWindow->pelementActive == pelementElement)
    glb_send_message(winWindow, pelementElement, M_LOST_FOCUS, 0);
  glb_send_message(winWindow, pelementElement, M_QUIT, 0);
  if (pelementElement->pelementPrev)
    pelementElement->pelementPrev->pelementNext =
      pelementElement->pelementNext;
  else
    winWindow->pelementFirst = pelementElement->pelementNext;
  if (pelementElement->pelementNext)
    pelementElement->pelementNext->pelementPrev =
      pelementElement->pelementPrev;
  else
    winWindow->pelementLast = pelementElement->pelementPrev;
  utl_free(pelementElement);
  if (winWindow->pelementActive == pelementElement)
  {
    for (pelementCounter = winWindow->pelementFirst; pelementCounter;
         pelementCounter = pelementCounter->pelementNext)
      if (pelementCounter->flFlags.binCanBeActivated)
      {
        winWindow->pelementActive = pelementCounter;
        pelementCounter->flFlags.binFocussed = TRUE;
        glb_send_message(winWindow, pelementCounter, M_GET_FOCUS, 0);
        return;
      }
    winWindow->pelementActive = NULL;
  }
}

DLG_ELEMENT* win_get_element (WINDOW winWindow, UINT uiID)
{
  DLG_ELEMENT *pelementCounter;
  
  for (pelementCounter = winWindow->pelementFirst; pelementCounter;
       pelementCounter = pelementCounter->pelementNext)
    if (pelementCounter->uiID == uiID)
      return(pelementCounter);
  return NULL;
}

void win_make_local (WINDOW winWindow, UTL_EVENT *peventFrom, UTL_EVENT *peventTo)
{
  memcpy(peventTo, peventFrom, sizeof(UTL_EVENT));
  peventTo->iHor -= winWindow->iX;
  peventTo->iVer -= winWindow->iY;
}

WINDOW win_which (UTL_EVENT *peventEvent)
{
  WINDOW winCounter;
 
  switch (peventEvent->uiKind)
  {
    case E_NULL :
    case E_KEY :
    case E_MESSAGE :
      return prgProgram.winFocussed;
    case E_MSM_MOVE :
    case E_MSM_L_DOWN :
    case E_MSM_L_UP :
    case E_MSM_L_DOUBLE:
    case E_MSM_R_DOWN :
    case E_MSM_R_UP :
    case E_MSM_R_DOUBLE :
    case E_MSM_M_DOWN :
    case E_MSM_M_UP :
    case E_MSM_M_DOUBLE :
      for (winCounter = prgProgram.winLast; winCounter; 
           winCounter = winCounter->winPrev)
        if (IN_WINDOW(peventEvent->iHor, peventEvent->iVer, winCounter))
          return winCounter;
  }
  return prgProgram.winFocussed;
}

void win_set_focus (WINDOW winWindow)
{    
  WINDOW winFocussed;  
                     
  if (winWindow == prgProgram.winFocussed && winWindow != winDesktop)
    return;
  winFocussed = prgProgram.winFocussed;
  prgProgram.winFocussed = winWindow;
  glb_send_message(&prgProgram, winFocussed, M_LOST_FOCUS, 0);
  glb_send_message(&prgProgram, prgProgram.winFocussed, M_GET_FOCUS, 0);
  win_update_cursor();
}

void win_pre_process (WINDOW winWindow, UTL_EVENT *peventEvent)
{
  static INT          siHor,
                      siVer;
  static WINDOW       swinBuffer      = NULL;
         INT          iNewX,
                      iNewY,
                      iNewWidth,
                      iNewHeight,
                      iOffsetX,
                      iOffsetY;

  switch (peventEvent->uiKind)
  {
    case E_MSM_L_DOWN :
      if (peventEvent->iVer == winWindow->iY &&
          peventEvent->iHor > winWindow->iX + 1 &&
          peventEvent->iHor < winWindow->iX + 5 &&
          winWindow->flFlags.binCloseable)
      {
        glb_send_message(&prgProgram, winWindow, M_CLOSE, 0);
        peventEvent->uiKind = E_DONE;
      }
      else
        if (peventEvent->iHor >= winWindow->iX + winWindow->iWidth - 5 &&
            peventEvent->iHor <= winWindow->iX + winWindow->iWidth - 3 &&
            peventEvent->iVer == winWindow->iY &&
            winWindow->flFlags.binSizeable)
        {
          if (winWindow->flFlags.binMaximized)
            win_resize(winWindow, winWindow->iLastWidth,
                       winWindow->iLastHeight);
          else
            win_maximize(winWindow);
          peventEvent->uiKind = E_DONE;
        }
        else
          if (peventEvent->iVer == winWindow->iY && 
              peventEvent->iHor >= winWindow->iX &&
              peventEvent->iHor < winWindow->iX + winWindow->iWidth &&
              winWindow->flFlags.binMoveable)
          {
            winWindow->flFlags.binDragging = TRUE;
            win_set_working(winWindow);
            bUpdating = TRUE;
            siHor = peventEvent->iHor;
            siVer = peventEvent->iVer;
            peventEvent->uiKind = E_DONE;
          }
          else
            if (peventEvent->iHor == winWindow->iX + winWindow->iWidth - 1 &&
                peventEvent->iVer == winWindow->iY + winWindow->iHeight - 1 &&
                winWindow->flFlags.binSizeable)
            {
              winWindow->flFlags.binSizing = TRUE;
              win_set_working(winWindow);         
              bUpdating = TRUE;
              siHor = peventEvent->iHor;
              siVer = peventEvent->iVer;
              peventEvent->uiKind = E_DONE;
            }                            
      break;
    case E_MSM_L_UP :
      if (winWindow->flFlags.binDragging || winWindow->flFlags.binSizing)
      {                                
        win_set_working(NULL);
        bUpdating = FALSE;
        if (swinBuffer && !win_active(swinBuffer))
        {                     
          win_update_on_top();
          if (!win_active(swinBuffer))
            glb_send_message(&prgProgram, swinBuffer, M_DEACTIVATE, 0);
          swinBuffer = NULL;
        }
        winWindow->flFlags.binDragging = winWindow->flFlags.binSizing = FALSE;
        peventEvent->uiKind = E_DONE;
      }
      break;
    case E_MSM_MOVE :
      if (winWindow->flFlags.binDragging)
      {
        iNewX = winWindow->iX + peventEvent->iHor - siHor;
        iNewY = winWindow->iY + peventEvent->iVer - siVer;
        if (iNewX != winWindow->iX || iNewY != winWindow->iY)
        {
          if (winWindow != prgProgram.winLast)
            swinBuffer = prgProgram.winLast;
          win_move(winWindow, iNewX, iNewY);
          siHor = peventEvent->iHor;
          siVer = peventEvent->iVer;
        }
        peventEvent->uiKind = E_DONE;
      }
      else
        if (winWindow->flFlags.binSizing)
        {
          iOffsetX = peventEvent->iHor - siHor;
          iOffsetY = peventEvent->iVer - siVer;
          if (iOffsetX || iOffsetY)
          {
            if (winWindow != prgProgram.winLast)
              swinBuffer = prgProgram.winLast;
            iNewWidth = min(max(winWindow->iWidth + iOffsetX, winWindow->iMinWidth), winWindow->iMaxWidth);
            iNewHeight = min(max(winWindow->iHeight + iOffsetY, winWindow->iMinHeight), winWindow->iMaxHeight);
            siHor += iNewWidth - winWindow->iWidth;
            siVer += iNewHeight - winWindow->iHeight;
            win_resize(winWindow, iNewWidth, iNewHeight);
          }
          peventEvent->uiKind = E_DONE;
        }
      break;
  }
}

void win_handle_event (WINDOW winWindow, UTL_EVENT *peventEvent)
{
  DLG_ELEMENT *pelementElement;

  win_pre_process(winWindow, peventEvent);
  if (peventEvent->uiKind == E_DONE)
    return;
  if (winWindow->pelementFirst /*&& !BORDER_CLICK(*peventEvent, winWindow)*/)
  {
    if (winWindow->pelementActive)
    {
      if (peventEvent->uiKind == E_KEY)
      {
        if (peventEvent->wKey == K_TAB)
        {
          dlg_activate_next(winWindow);
          return;
        }
        if (peventEvent->wKey == K_SHIFT_TAB)
        {
          dlg_activate_prev(winWindow);
          return;
        }
      }
      winWindow->pelementActive->handle_event(winWindow->pelementActive, peventEvent);
    }
    if (peventEvent->uiKind != E_DONE /*&& 
        !BORDER_EVENT(*peventEvent, winWindow)*/)
      for (pelementElement = winWindow->pelementFirst; pelementElement;
           pelementElement = pelementElement->pelementNext)
      {   
        pelementElement->handle_event(pelementElement, peventEvent);
        if (peventEvent->uiKind == E_DONE)
        {
          if (pelementElement->flFlags.binCanBeActivated)
            dlg_set_focus(winWindow, pelementElement);
          return;
        }
      }
    else
      if (peventEvent->uiKind == E_DONE)
        return;
  }
  if (peventEvent->uiKind != E_DONE && winWindow->handle_event)
    winWindow->handle_event(winWindow, peventEvent);
  if (peventEvent->uiKind != E_DONE &&
      !prgProgram.winFocussed->flFlags.binModal)
    prgProgram.handle_event(&prgProgram, peventEvent);
  if (peventEvent->uiKind == E_DONE)
    return;
  if ((peventEvent->uiKind == E_KEY && peventEvent->wKey != K_INS) ||
      peventEvent->uiKind == E_MSM_L_DOWN ||
      peventEvent->uiKind == E_MSM_R_DOWN ||
      peventEvent->uiKind == E_MSM_M_DOWN ||
      peventEvent->uiKind == E_MSM_L_DOUBLE ||
      peventEvent->uiKind == E_MSM_R_DOUBLE ||
      peventEvent->uiKind == E_MSM_M_DOUBLE)
    utl_beep();
} 

void win_set_working (WINDOW winWindow)
{
  winWorking = winWindow;
}          

void win_std_handler (WINDOW winWindow, UTL_EVENT *peventEvent)
{         
  INT iPos;

  switch (peventEvent->uiKind)
  {
    case E_MESSAGE :
      switch (peventEvent->uiMessage)
      {               
        case M_GET_FOCUS :
          win_show(winWindow);
          sts_new_status_line(NULL);
          peventEvent->uiKind = E_DONE;
          break;
        case M_LOST_FOCUS :
          sts_del_status_line();
          peventEvent->uiKind = E_DONE;
          break;
        case M_SCROLL_UP :
        case M_SCROLL_PAGE_UP :
          dlg_scroll_query_values(peventEvent->pFrom, &iPos);
          dlg_scroll_set_values(peventEvent->pFrom, iPos - 1);
          peventEvent->uiKind = E_DONE;
          break;
        case M_SCROLL_DOWN :
        case M_SCROLL_PAGE_DOWN :
          dlg_scroll_query_values(peventEvent->pFrom, &iPos);
          dlg_scroll_set_values(peventEvent->pFrom, iPos + 1);
          peventEvent->uiKind = E_DONE;
          break;
        case M_ACT_BUT_CLICKED :
        case M_LIST_BOX_CLICKED :
          win_set_add_info(winWindow, ((DLG_ELEMENT*)peventEvent->pFrom)->uiID);
          if (winWindow->flFlags.binModal)
            bExit = TRUE;
          peventEvent->uiKind = E_DONE;
          break;           
        case M_CLOSE :
          if (winWindow->flFlags.binModal)
            bExit = TRUE;
          else
            win_delete(winWindow);
          peventEvent->uiKind = E_DONE;
          break;           
        case M_SHOW:
        case M_ACTIVATE :
          win_set_focus(winWindow);
          peventEvent->uiKind = E_DONE;
          break;
        case M_QUERY_VALUES :
          *((UINT*)peventEvent->ulAddInfo) = win_get_add_info(winWindow);
          peventEvent->uiKind = E_DONE;
          break;
        case M_DRAW :
        case M_REDRAW :
          win_cls(winWindow);
          win_draw_elements(winWindow);
          peventEvent->uiKind = E_DONE;
          break;
      }
      break;
  }
}

void win_update_on_top (void)
{
  WINDOW     winCounter;
  CNT_QUEUE *pqQueue;

  bUpdating = TRUE;
  pqQueue = cnt_queue_make();
  for (winCounter = prgProgram.winFirst; winCounter;
       winCounter = winCounter->winNext)
    if (winCounter->flFlags.binAlwaysOnTop)
      cnt_queue_put(pqQueue, (ULONG)winCounter);
  while (!cnt_queue_is_empty(pqQueue))
    win_show((WINDOW)cnt_queue_get(pqQueue));
  cnt_queue_destroy(pqQueue);
  bUpdating = FALSE;
}

void win_update_cursor (void)
{
  WINDOW     winFocussed;
  CHARACTER *pchar;

  winFocussed = prgProgram.winFocussed;
  if (winFocussed->flFlags.binCursorAct)
    if (winFocussed->iCursX < winFocussed->iWidth - 1 &&
        winFocussed->iCursY < winFocussed->iHeight - 1 &&
        winFocussed->iCursX && winFocussed->iCursY)
    {
      pchar = winFocussed->pcharSave + winFocussed->iCursY * REAL_WIDTH(winFocussed) + winFocussed->iCursX;
      if (pchar->character == (CHAR)0xff || pchar->attribute == (CHAR)0xff)
      {
        vio_set_cursor_pos(winFocussed->iX + winFocussed->iCursX, winFocussed->iY + winFocussed->iCursY);
#ifndef _MSDOS
        vio_set_cursor_type(winFocussed->iCursPercent);
#endif
        return;
      }
    }
#ifndef _MSDOS
  vio_set_cursor_type(0);
#endif
}

BOOL win_register_class (CHAR *pcName, UCHAR ucColors, UCHAR ucBorder, UCHAR ucTitlePos, UINT uiFlags,
                         void (*handle_event)(WINDOW, UTL_EVENT*))
{
  WINDOW_CLASS *pwc;

  if (pwc = int_win_get_class(pcName))
    return FALSE;
  if (!(pwc = utl_alloc(sizeof(WINDOW_CLASS))))
    return FALSE;
  strncpy(pwc->acName, pcName, WIN_MAX_CLASS_NAME);
  pwc->pwcNext = pwcFirst;
  pwc->handle_event = handle_event;
  pwc->pcColors = prgProgram.pcPalette + PAL_COLORS * ucColors;
  memset(&pwc->flFlags, 0x00, sizeof(WIN_FLAGS));
  pwc->flFlags.binBorderType = ucBorder;
  pwc->flFlags.binTitlePos = ucTitlePos;
  if (uiFlags & WIN_FL_SHADOW)
    pwc->flFlags.binShadow = TRUE;
  if (uiFlags & WIN_FL_CURSOR)
    pwc->flFlags.binCursorAct = TRUE;
  if (uiFlags & WIN_FL_MOVEABLE)
    pwc->flFlags.binMoveable = TRUE;
  if (uiFlags & WIN_FL_CLOSEABLE)
    pwc->flFlags.binCloseable = TRUE;
  if (uiFlags & WIN_FL_SIZEABLE)
    pwc->flFlags.binSizeable = TRUE;
  if (uiFlags & WIN_FL_MODAL)
    pwc->flFlags.binModal = TRUE;
  if (uiFlags & WIN_FL_FOCUSABLE)
    pwc->flFlags.binCanBeFocussed = TRUE;
  if (uiFlags & WIN_FL_ON_TOP)
    pwc->flFlags.binAlwaysOnTop = TRUE;
  pwc->flFlags.binDrawCheck = TRUE;
  pwcFirst = pwc;
  return TRUE;
}

CHAR* win_get_class (WINDOW win)
{
  if (win == NULL)
    win = AKT_WIN;
  return win->pwcClass->acName;
}

WINDOW win_find_class_window (CHAR *pcClass)
{
  return win_find(int_win_is_class, (ULONG)pcClass);
}

WINDOW win_new (INT iX, INT iY, INT iWidth, INT iHeight, CHAR *pcClassName, ULONG ulAddInfo)
{
  WINDOW        winReturnVar;
  WINDOW_CLASS *pwc;

  if (!(pwc = int_win_get_class(pcClassName)))
    return NULL;
  if (iWidth > iSizeX || iHeight > iSizeY)
    return NULL;
  if (!(winReturnVar = (WINDOW)utl_alloc(sizeof(WB))))
    return NULL;
  winReturnVar->iX = iX;
  winReturnVar->iY = iY;
  winReturnVar->iWidth = iWidth;
  winReturnVar->iHeight = iHeight;
  winReturnVar->iMinWidth = 10;
  winReturnVar->iMinHeight = 3;
  winReturnVar->iMaxWidth = iSizeX;
  winReturnVar->iMaxHeight = iSizeY - 2;
  winReturnVar->iCursX = 1;
  winReturnVar->iCursY = 1;
  winReturnVar->iCursPercent = 20;
  winReturnVar->pcTitle = NULL;
  winReturnVar->pcColors = pwc->pcColors;
  winReturnVar->pelementFirst = winReturnVar->pelementLast = winReturnVar->pelementActive = NULL;
  if (!(winReturnVar->pcharSave = utl_alloc((iWidth + 2) * (iHeight + 1) * sizeof(CHARACTER))))
    return NULL;
  memset(winReturnVar->pcharSave, 8, (iWidth + 2) * (iHeight + 1) * sizeof(CHARACTER));
  winReturnVar->flFlags = pwc->flFlags;
  winReturnVar->ulAddInfo = ulAddInfo;
  winReturnVar->handle_event = pwc->handle_event;
  winReturnVar->redraw_func = win_cls;
  winReturnVar->pwcClass = pwc;
  winReturnVar->winNext = winReturnVar->winPrev = NULL;
  win_cls(winReturnVar);
  int_win_border_title(winReturnVar);
  if (winReturnVar->flFlags.binShadow)
  {
    int_win_set_ff(winReturnVar, winReturnVar->iWidth, 0, 2, 1, WIN_CHAR | WIN_ATTRI);
    int_win_set_ff(winReturnVar, winReturnVar->iWidth, 1, 2, winReturnVar->iHeight, WIN_CHAR);
    int_win_set_ff(winReturnVar, 0, winReturnVar->iHeight, 2, 1, WIN_CHAR | WIN_ATTRI);
    int_win_set_ff(winReturnVar, 2, winReturnVar->iHeight, winReturnVar->iWidth - 2, 1, WIN_CHAR);
  }
  glb_send_message(&prgProgram, winReturnVar, M_INIT, 0);
  return winReturnVar;
}

BOOL win_show (WINDOW winWindow)
{
  UCHAR ucSendMessage = 0;
  MOUSE;

  if (win_active(winWindow))
    return TRUE;
  BEGIN_MOUSE;
  if (win_visible(winWindow) && !win_active(winWindow))
  {
    if (!int_win_front(winWindow))
      return FALSE;
    ucSendMessage |= 2;
  }
  if (!win_active(winWindow))
  {
    prgProgram.winLast->winNext = winWindow;
    winWindow->winPrev = prgProgram.winLast;
    prgProgram.winLast = winWindow;
    uiWinsActive++;
    int_win_save_windows(winWindow);
    int_win_put_window(winWindow);
    ucSendMessage |= 3;
  }
  win_update_cursor();
  if (!winWindow->flFlags.binDragging && 
      !winWindow->flFlags.binSizing && !bTossing)
  {
    if (ucSendMessage != 0)
    {
      glb_send_message(&prgProgram, prgProgram.winLast->winPrev, M_DEACTIVATE, 0);
      if (ucSendMessage & 1)
        glb_send_message(&prgProgram, winWindow, M_SHOW, 0);
      if (ucSendMessage & 2)
        glb_send_message(&prgProgram, winWindow, M_ACTIVATE, 0);
    }
    if (!bUpdating)
      win_update_on_top();
  }
  END_MOUSE;
  return TRUE;
}

BOOL win_hide (WINDOW winWindow)
{
  WINDOW winSetFocus = NULL;
  BOOL   b;
  MOUSE;                  

  if (!winWindow)
    winWindow = AKT_WIN;
  BEGIN_MOUSE;
  if (win_visible(winWindow) && !win_active(winWindow))
  {        
    bUpdating = TRUE;
    b = win_show(winWindow);
    bUpdating = FALSE;
    if (!b)
      return FALSE;
  }
  if (win_active(winWindow))
  {
    if (!winWindow->flFlags.binDragging && !winWindow->flFlags.binSizing && !bTossing)
    {                                      
      if (winWindow == prgProgram.winFocussed)
      {
        for (winSetFocus = winWindow->winPrev; winSetFocus;
             winSetFocus = winSetFocus->winPrev)
          if (winSetFocus->flFlags.binCanBeFocussed)
            break;
        if (!winSetFocus)
          winSetFocus = winDesktop;
      }
      glb_send_message(&prgProgram, winWindow, M_HIDE, 0);
    }
    int_win_get_window(winWindow);
    if (!int_win_write_windows(winWindow))
      return FALSE;
    if (winWindow->winPrev)
    {
      winWindow->winPrev->winNext = NULL;
      prgProgram.winLast = winWindow->winPrev;
      winWindow->winPrev = NULL;
    }
    else
      prgProgram.winFirst = winWindow->winNext;
    uiWinsActive--;
  }
  if (winSetFocus)
    win_set_focus(winSetFocus);
  END_MOUSE;
  win_update_cursor();
  return TRUE;
}

BOOL win_delete (WINDOW winWindow)
{
  if (!winWindow)
    winWindow = AKT_WIN;               
  if (win_visible(winWindow))
    if (!win_hide(winWindow))
      return FALSE;
  glb_send_message(&prgProgram, winWindow, M_QUIT, 0);
  while (winWindow->pelementFirst)
    win_delete_element(winWindow, winWindow->pelementFirst);
  if (winWindow->pcTitle)
    utl_free(winWindow->pcTitle);
  utl_free(winWindow->pcharSave);
  utl_free(winWindow);
  return TRUE;
}

BOOL win_title (WINDOW winWindow, CHAR *pcTitle, UCHAR ucTitlePos)
{
  if (!winWindow)
    winWindow = AKT_WIN;
  if (winWindow->pcTitle)
    utl_free(winWindow->pcTitle);
  winWindow->pcTitle = utl_alloc(strlen(pcTitle) + 1);
  strcpy(winWindow->pcTitle, pcTitle);
  pcTitle = winWindow->pcTitle;
  if (strlen(pcTitle) > (winWindow->iWidth - 2))
    pcTitle[winWindow->iWidth - 2] = 0;
  winWindow->flFlags.binTitlePos = ucTitlePos;
  int_win_border_title(winWindow);
  glb_send_message(&prgProgram, winWindow, M_REDRAW, 0);
  return TRUE;
}

BOOL win_border (WINDOW winWindow, UCHAR ucBorderType)
{
  if (!winWindow)
    winWindow = AKT_WIN;
  winWindow->flFlags.binBorderType = ucBorderType;
  int_win_border_title(winWindow);
  return TRUE;
}

BOOL win_color (WINDOW winWindow, UCHAR ucColors)
{
  if (!winWindow)
    winWindow = AKT_WIN;
  winWindow->pcColors = prgProgram.pcPalette + PAL_COLORS * ucColors;
  if (winWindow != winDesktop)
    int_win_border_title(winWindow);
  glb_send_message(&prgProgram, winWindow, M_REDRAW, 0);
  return TRUE;
}                           

void win_update (WINDOW winWindow)
{
  UCHAR ucColors;
  
  ucColors = ((winWindow->pcColors - apcColors) / PAL_COLORS) % PAL_WINDOWS;
  win_color(winWindow, ucColors);
}

BOOL win_move (WINDOW winWindow, INT iX, INT iY)
{
  INT       iRegionX,
            iRegionY,
            iRegionWidth,
            iRegionHeight;   
  BOOL      bWasUpdating;
  VIO_COORD coord;
  MOUSE;

  if (!winWindow)
    winWindow = AKT_WIN;
  bWasUpdating = bUpdating;
  bUpdating = TRUE;
  if (win_visible(winWindow) && !win_active(winWindow))
    if (!win_show(winWindow))
      return FALSE;
  if (iX == winWindow->iX && iY == winWindow->iY)
    return TRUE;
  if (win_active(winWindow))
  {
    BEGIN_MOUSE;
    int_win_make_region(winWindow->iX, winWindow->iY, REAL_WIDTH(winWindow),
                        REAL_HEIGHT(winWindow), iX, iY, REAL_WIDTH(winWindow),
                        REAL_HEIGHT(winWindow), &iRegionX, &iRegionY, 
                        &iRegionWidth, &iRegionHeight);
    int_win_get_region(pcharMoveBuffer, iRegionX, iRegionY, iRegionWidth, iRegionHeight);
    vio_set_vir_page(pcharMoveBuffer);
    if (!win_hide(winWindow))
      return FALSE;
    winWindow->iX = iX;
    winWindow->iY = iY;
    if (!win_show(winWindow))
      return FALSE;
    vio_set_phys_page();
    int_win_put_region(pcharMoveBuffer, iRegionX, iRegionY, iRegionWidth, iRegionHeight);
    END_MOUSE;
  }
  else
  {
    winWindow->iX = iX;
    winWindow->iY = iY;
  }
  coord.iX = iX;
  coord.iY = iY;
  glb_send_message(&prgProgram, winWindow, M_MOVE, &coord);
  if (!(bUpdating = bWasUpdating))
    win_update_on_top();
  return TRUE;
}

BOOL win_resize (WINDOW winWindow, INT iWidth, INT iHeight)
{
  INT       iRegionX,
            iRegionY,
            iRegionWidth,
            iRegionHeight,
            iRealWidth,
            iRealHeight;
  BOOL      bWasActive    = FALSE,
            bWasUpdating,
            bSizing;
  VIO_COORD coord;
  MOUSE;

  if (!winWindow)
    winWindow = AKT_WIN;
  if (iWidth == winWindow->iWidth && iHeight == winWindow->iHeight)
  {
    if (winWindow->flFlags.binMaximized)
      int_win_border_title(winWindow);
    return TRUE;
  }
  if (iWidth < winWindow->iMinWidth || iWidth > winWindow->iMaxWidth ||
      iHeight < winWindow->iMinHeight || iHeight > winWindow->iMaxHeight)
    return FALSE;
  bWasUpdating = bUpdating;
  bUpdating = TRUE;
  if (win_visible(winWindow) && !win_active(winWindow))
    if (!win_show(winWindow))
      return FALSE;
  bSizing = winWindow->flFlags.binSizing;
  winWindow->flFlags.binSizing = TRUE;
  if (iWidth != winWindow->iMaxWidth || iHeight != winWindow->iMaxHeight)
    winWindow->flFlags.binMaximized = FALSE;              
  if (win_active(winWindow))
  {
    BEGIN_MOUSE;
    bWasActive = TRUE;
    iRealWidth = winWindow->flFlags.binShadow ? iWidth + 2 : iWidth;
    iRealHeight = winWindow->flFlags.binShadow ? iHeight + 1 : iHeight;
    if (winWindow->flFlags.binMaximized)
    {
      iRegionX = 1;
      iRegionY = 2;
    }
    else
    {
      iRegionX = winWindow->iX;
      iRegionY = winWindow->iY;
    }
    int_win_make_region(winWindow->iX, winWindow->iY, REAL_WIDTH(winWindow),
                        REAL_HEIGHT(winWindow), iRegionX, iRegionY,
                        iRealWidth, iRealHeight, &iRegionX, &iRegionY, 
                        &iRegionWidth, &iRegionHeight);
    int_win_get_region(pcharMoveBuffer, iRegionX, iRegionY, iRegionWidth, iRegionHeight);
    vio_set_vir_page(pcharMoveBuffer);
    if (!win_hide(winWindow))
      return FALSE;
  }
  utl_free(winWindow->pcharSave);       
  if (winWindow->flFlags.binMaximized)
  {
    winWindow->iX = 1;
    winWindow->iY = 2;
  }
  winWindow->iWidth = iWidth;
  winWindow->iHeight = iHeight;
  if (!(winWindow->pcharSave = utl_alloc(iRealWidth * iRealHeight * sizeof(CHARACTER))))
    return FALSE;
  memset(winWindow->pcharSave, 8, iRealWidth * iRealHeight * sizeof(CHARACTER));
  int_win_border_title(winWindow);
  if (winWindow->flFlags.binShadow)
  {
    int_win_set_ff(winWindow, winWindow->iWidth, 0, 2, 1, WIN_CHAR | WIN_ATTRI);
    int_win_set_ff(winWindow, winWindow->iWidth, 1, 2, winWindow->iHeight, WIN_CHAR);
    int_win_set_ff(winWindow, 0, winWindow->iHeight, 2, 1, WIN_CHAR | WIN_ATTRI);
    int_win_set_ff(winWindow, 2, winWindow->iHeight, winWindow->iWidth - 2, 1, WIN_CHAR);
  }
  if (winWindow->handle_event)
    glb_send_message(&prgProgram, winWindow, M_DRAW, 0);
  else
    winWindow->redraw_func(winWindow);
  if (bWasActive)
  {
    if (!win_show(winWindow))
      return FALSE;
    vio_set_phys_page();
    int_win_put_region(pcharMoveBuffer, iRegionX, iRegionY, iRegionWidth, iRegionHeight);
    END_MOUSE;
  }
  coord.iX = iWidth;
  coord.iY = iHeight;
  glb_send_message(&prgProgram, winWindow, M_SIZE, &coord);
  winWindow->flFlags.binSizing = bSizing;
  if (!(bUpdating = bWasUpdating))
    win_update_on_top();
  return TRUE;
}           

BOOL win_maximize (WINDOW winWindow)
{
  if (!winWindow)
    winWindow = AKT_WIN;
  winWindow->flFlags.binMaximized = TRUE;
  winWindow->iLastWidth = winWindow->iWidth;
  winWindow->iLastHeight = winWindow->iHeight;
  if (winWindow->iWidth == winWindow->iMaxWidth && winWindow->iHeight == winWindow->iMaxHeight)
    return win_move(winWindow, 1, 2);
  return win_resize(winWindow, winWindow->iMaxWidth, winWindow->iMaxHeight);
}

WINDOW win_find (BOOL (*good_window)(WINDOW, ULONG), ULONG ulAddInfo)
{
  WINDOW winCounter;
  
  for (winCounter = prgProgram.winLast; winCounter;
       winCounter = winCounter->winPrev)
    if (good_window(winCounter, ulAddInfo))
      return winCounter;
  return NULL;
}    

void win_set_size_limits (WINDOW winWindow, INT iMinWidth, INT iMinHeight, INT iMaxWidth, INT iMaxHeight)
{
  if (!winWindow)
    winWindow = AKT_WIN;
  winWindow->iMinWidth = iMinWidth;
  winWindow->iMinHeight = iMinHeight;
  winWindow->iMaxWidth = iMaxWidth;
  winWindow->iMaxHeight = iMaxHeight;
}

void win_cls (WINDOW winWindow)
{
  if (!winWindow)
    winWindow = AKT_WIN;
  win_sw_za(winWindow, 1, 1, winWindow->iWidth - 2, winWindow->iHeight - 2, 
            ' ', win_get_color(winWindow, PAL_COL_BACKGROUND));
}

BOOL win_shadow (WINDOW winWindow, BOOL bShadow)
{
  CHARACTER *pcharDest,
            *pcharSource,
            *pcharBuffer;
  INT        iCounterX,
             iCounterY;

  if (!winWindow)
    winWindow = AKT_WIN;
  if (win_visible(winWindow))
    return(FALSE);
  if (bShadow)
    bShadow = TRUE;
  if (winWindow->flFlags.binShadow == bShadow)
    return TRUE;
  if (bShadow)
  {
    if (!(pcharBuffer = pcharDest = utl_alloc((winWindow->iWidth + 2) * (winWindow->iHeight + 1) *
                sizeof(CHARACTER))))
      return FALSE;
    memset(pcharDest, 8, (winWindow->iWidth + 2) * (winWindow->iHeight + 1) * sizeof(CHARACTER));
    pcharSource = winWindow->pcharSave;
    for (iCounterY = 0; iCounterY < winWindow->iHeight; iCounterY++)
    {
      for (iCounterX = 0; iCounterX < winWindow->iWidth; iCounterX++)
        *pcharDest++ = *pcharSource++;
      pcharDest += 2;
    }
    int_win_set_ff(winWindow, winWindow->iWidth, 0, 2, 1, WIN_CHAR | WIN_ATTRI);
    int_win_set_ff(winWindow, winWindow->iWidth, 1, 2, winWindow->iHeight, WIN_CHAR);
    int_win_set_ff(winWindow, 0, winWindow->iHeight, 2, 1, WIN_CHAR | WIN_ATTRI);
    int_win_set_ff(winWindow, 2, winWindow->iHeight, winWindow->iWidth - 2, 1, WIN_CHAR);
    utl_free(winWindow->pcharSave);
    winWindow->pcharSave = pcharBuffer;
    winWindow->flFlags.binShadow = TRUE;
  }
  else
  {
    if (!(pcharBuffer = pcharDest = utl_alloc(winWindow->iWidth * winWindow->iHeight * sizeof(CHARACTER))))
      return FALSE;
    pcharSource = winWindow->pcharSave;
    for (iCounterY = 0; iCounterY < winWindow->iHeight; iCounterY++)
    {
      for (iCounterX = 0; iCounterX < winWindow->iWidth; iCounterX++)
        *pcharDest++ = *pcharSource++;
      pcharSource += 2;
    }
    utl_free(winWindow->pcharSave);
    winWindow->pcharSave = pcharBuffer;
    winWindow->flFlags.binShadow = FALSE;
  }
  return TRUE;
}
               
BOOL win_moveable (WINDOW winWindow, UCHAR ucStatus)
{           
  BOOL bReturnVar;  

  if (!winWindow)
    winWindow = AKT_WIN;
  bReturnVar = winWindow->flFlags.binMoveable;
  switch (ucStatus)
  {
    case QUERY :
      break;
    case FLIP :
      winWindow->flFlags.binMoveable = !(winWindow->flFlags.binMoveable);
      break;
    default :
      winWindow->flFlags.binMoveable = ucStatus;
      break;
  }
  return bReturnVar;
}

BOOL win_closeable (WINDOW winWindow, UCHAR ucStatus)
{           
  BOOL bReturnVar;  

  if (!winWindow)
    winWindow = AKT_WIN;
  bReturnVar = winWindow->flFlags.binCloseable;
  switch (ucStatus)
  {
    case QUERY :
      return bReturnVar;
    case FLIP :
      winWindow->flFlags.binCloseable = !(winWindow->flFlags.binCloseable);
      break;
    default :
      winWindow->flFlags.binCloseable = ucStatus;
      break;
  }                       
  int_win_border_title(winWindow);
  return bReturnVar;
}
                         
BOOL win_sizeable (WINDOW winWindow, UCHAR ucStatus)
{           
  BOOL bReturnVar;  

  if (!winWindow)
    winWindow = AKT_WIN;
  bReturnVar = winWindow->flFlags.binSizeable;
  switch (ucStatus)
  {
    case QUERY :                
      return bReturnVar;
    case FLIP :
      winWindow->flFlags.binSizeable = !(winWindow->flFlags.binSizeable);
      break;
    default :
      winWindow->flFlags.binSizeable = ucStatus;
      break;
  }                               
  int_win_border_title(winWindow);
  return bReturnVar;
}

BOOL win_modal (WINDOW winWindow, UCHAR ucStatus)
{           
  BOOL bReturnVar;  

  if (!winWindow)
    winWindow = AKT_WIN;
  bReturnVar = winWindow->flFlags.binModal;
  switch (ucStatus)
  {
    case QUERY :
      break;
    case FLIP :
      winWindow->flFlags.binModal = !(winWindow->flFlags.binModal);
      break;
    default :
      winWindow->flFlags.binModal = ucStatus;
      break;
  }
  return bReturnVar;
}
         
BOOL win_clipping (WINDOW winWindow, UCHAR ucStatus)
{           
  BOOL bReturnVar;  

  if (!winWindow)
    winWindow = AKT_WIN;
  bReturnVar = winWindow->flFlags.binDrawCheck;
  switch (ucStatus)
  {
    case QUERY :
      break;
    case FLIP :
      winWindow->flFlags.binDrawCheck = !(winWindow->flFlags.binDrawCheck);
      break;
    default :
      winWindow->flFlags.binDrawCheck = ucStatus;
      break;
  }
  return bReturnVar;
}                                 

BOOL win_focusable (WINDOW winWindow, UCHAR ucStatus)
{           
  BOOL bReturnVar;  

  if (!winWindow)
    winWindow = AKT_WIN;
  bReturnVar = winWindow->flFlags.binCanBeFocussed;
  switch (ucStatus)
  {
    case QUERY :
      break;
    case FLIP :
      winWindow->flFlags.binCanBeFocussed = !(winWindow->flFlags.binCanBeFocussed);
      break;
    default :
      winWindow->flFlags.binCanBeFocussed = ucStatus;
      break;
  }
  return bReturnVar;
}
                                        
BOOL win_always_on_top (WINDOW winWindow, UCHAR ucStatus)
{           
  BOOL bReturnVar;  

  if (!winWindow)
    winWindow = AKT_WIN;
  bReturnVar = winWindow->flFlags.binAlwaysOnTop;
  switch (ucStatus)
  {
    case QUERY :
      break;
    case FLIP :
      winWindow->flFlags.binAlwaysOnTop = !(winWindow->flFlags.binAlwaysOnTop);
      break;
    default :
      winWindow->flFlags.binAlwaysOnTop = ucStatus;
      break;
  }
  return bReturnVar;
}                                      

BOOL win_cursor (WINDOW winWindow, UCHAR ucStatus)
{           
  BOOL bReturnVar;  

  if (!winWindow)
    winWindow = AKT_WIN;
  bReturnVar = winWindow->flFlags.binCursorAct;
  switch (ucStatus)
  {
    case QUERY :
      break;
    case FLIP :
      winWindow->flFlags.binCursorAct = !(winWindow->flFlags.binCursorAct);
      break;
    default :
      winWindow->flFlags.binCursorAct = ucStatus;
      break;
  }
  win_update_cursor();
  return bReturnVar;
}

void win_set_redraw (WINDOW winWindow, void (*new_redraw_func) (WINDOW))
{
  winWindow->redraw_func = new_redraw_func;
}                                 

void win_set_cursor_pos (WINDOW winWindow, INT iX, INT iY)
{
  if (!winWindow)
    winWindow = AKT_WIN;
  if (iX < 1 || iY < 1)
    return;
  winWindow->iCursX = iX;
  winWindow->iCursY = iY;
  win_update_cursor();
}

void win_get_cursor_pos (WINDOW winWindow, INT *piX, INT *piY)
{
  if (!winWindow)
    winWindow = AKT_WIN;
  *piX = winWindow->iCursX;
  *piY = winWindow->iCursY;
}

void win_set_cursor_type (WINDOW winWindow, INT iPercent)
{
  if (!winWindow)
    winWindow = AKT_WIN;
  winWindow->iCursPercent = iPercent;
  win_update_cursor();
}

void win_get_cursor_type (WINDOW winWindow, INT *piPercent)
{
  if (!winWindow)
    winWindow = AKT_WIN;
  *piPercent = winWindow->iCursPercent;
}

void win_draw_border (WINDOW winWindow, INT iX, INT iY, INT iWidth, INT iHeight, UCHAR ucRahmenTyp, CHAR cAttri)
{
  MOUSE;

  if (!winWindow)
    winWindow = AKT_WIN;
  if (!cAttri)
    cAttri = win_get_color(winWindow, PAL_COL_BACKGROUND);
  BEGIN_MOUSE_IN(winWindow);
  win_s_za(winWindow, iX, iY, aacBorder[ucRahmenTyp][0], cAttri);
  win_s_za(winWindow, iX + iWidth - 1, iY, aacBorder[ucRahmenTyp][1], cAttri);
  win_s_za(winWindow, iX, iY + iHeight - 1, aacBorder[ucRahmenTyp][2], cAttri);
  win_s_za(winWindow, iX + iWidth - 1, iY + iHeight - 1, aacBorder[ucRahmenTyp][3], cAttri);
  win_sw_za(winWindow, iX + 1, iY, iWidth - 2, 1, aacBorder[ucRahmenTyp][4], cAttri);
  win_sw_za(winWindow, iX + 1, iY + iHeight - 1, iWidth - 2, 1, aacBorder[ucRahmenTyp][5], cAttri);
  win_sw_za(winWindow, iX, iY + 1, 1, iHeight - 2, aacBorder[ucRahmenTyp][6], cAttri);
  win_sw_za(winWindow, iX + iWidth - 1, iY + 1, 1, iHeight - 2, aacBorder[ucRahmenTyp][7], cAttri);
  END_MOUSE;
}

void win_write_hot (WINDOW winWindow, INT iX, INT iY, CHAR *pcText, CHAR cNormAttri, CHAR cHotAttri)
{
  INT   iCounter = 0;
  UCHAR ucHot    = 0;
  MOUSE;

  BEGIN_MOUSE_IN(winWindow);
  for ( ; *pcText; pcText++)
  {                       
    switch (*pcText)
    {
      case '#' :
        ucHot = 1;
        break;
      case '~' :
        if (ucHot <= 1)
          ucHot = 2;
        else
          ucHot = 0;
        break;      
      default :
        if (ucHot != 0)
        {
          win_s_za(winWindow, iX + iCounter, iY, *pcText, cHotAttri);
          if (ucHot == 1)
            ucHot = 0;
        }
        else
          win_s_za(winWindow, iX + iCounter, iY, *pcText, cNormAttri);
        iCounter++;
        break;
    }
  }
  END_MOUSE;
}

INT win_printf (WINDOW winWindow, const CHAR *pcFormat, ...)
{
  INT     iReturnVar;
  CHAR    acBuffer[1024];
  va_list vaArgumente;

  if (!winWindow)
    winWindow = AKT_WIN;
  va_start(vaArgumente, pcFormat);
  iReturnVar = vsprintf(acBuffer, pcFormat, vaArgumente);
  int_win_printf_output(winWindow, acBuffer);
  va_end(vaArgumente);
  return iReturnVar;
}

INT dsk_printf (const CHAR *pcFormat, ...)
{
  INT     iReturnVar;
  CHAR    acBuffer[1024];
  va_list vaArgumente;

  va_start(vaArgumente, pcFormat);
  iReturnVar = vsprintf(acBuffer, pcFormat, vaArgumente);
  int_win_printf_output(winDesktop, acBuffer);
  va_end(vaArgumente);
  return iReturnVar;
}

void win_printch (WINDOW winWindow, CHAR cChar)
{
  INT iX,
      iY;
  MOUSE;

  if (!winWindow)
    winWindow = AKT_WIN;
  BEGIN_MOUSE_IN(winWindow);
  win_get_cursor_pos(winWindow, &iX, &iY);
  win_s_za(winWindow, iX, iY, cChar, win_get_color(winWindow, PAL_COL_BACKGROUND));
  iX++;
  if (iX > winWindow->iWidth - 2)
  {
    iX = 1;
    iY++;
    if (iY > winWindow->iHeight - 2)
    {
      iY = winWindow->iHeight - 2;
      win_scroll_up(winWindow, 1);
    }
  }
  win_set_cursor_pos(winWindow, iX, iY);
  END_MOUSE;
}

void win_up (WINDOW winWindow, INT iX, INT iY, INT iWidth, INT iHeight, INT iRows)
{
  MOUSE;

  if (!winWindow)
    winWindow = AKT_WIN;
  CHECK_WH(winWindow, iX, iY, iWidth, iHeight);
  BEGIN_MOUSE_IN(winWindow);
  if (iRows > 0)
  {
    win_lw(winWindow, iX, iY + iRows, iWidth, iHeight - iRows, pcharSave);
    win_sw(winWindow, iX, iY, iWidth, iHeight - iRows, pcharSave);
    win_sw_za(winWindow, iX, iY + iHeight - iRows, iWidth, iRows, ' ',
        win_get_color(winWindow, PAL_COL_BACKGROUND));
  }
  else
    win_sw_za(winWindow, iX, iY, iWidth, iHeight, ' ', win_get_color(winWindow, PAL_COL_BACKGROUND));
  END_MOUSE;
}

void win_scroll_up (WINDOW winWindow, INT iRows)
{
  win_up(winWindow, 1, 1, winWindow->iWidth - 2, winWindow->iHeight - 2, iRows);
}

void win_down (WINDOW winWindow, INT iX, INT iY, INT iWidth, INT iHeight, INT iRows)
{
  MOUSE;

  if (!winWindow)
    winWindow = AKT_WIN;
  CHECK_WH(winWindow, iX, iY, iWidth, iHeight);
  BEGIN_MOUSE_IN(winWindow);
  if (iRows > 0)
  {
    win_lw(winWindow, iX, iY, iWidth, iHeight - iRows, pcharSave);
    win_sw(winWindow, iX, iY + iRows, iWidth, iHeight - iRows, pcharSave);
    win_sw_za(winWindow, iX, iY, iWidth, iRows, ' ', win_get_color(winWindow, PAL_COL_BACKGROUND));
  }
  else
    win_sw_za(winWindow, iX, iY, iWidth, iHeight, ' ', win_get_color(winWindow, PAL_COL_BACKGROUND));
  END_MOUSE;
}

void win_scroll_down (WINDOW winWindow, INT iRows)
{
  win_down(winWindow, 1, 1, winWindow->iWidth - 2, winWindow->iHeight - 2, iRows);
}

void win_set_tab (INT iNewTab)
{
  if (iNewTab)
    iTab = iNewTab;
}

void win_ins_zeile (WINDOW winWindow, INT iRow)
{
  MOUSE;

  if (!winWindow)
    winWindow = AKT_WIN;
  CHECK_XY(winWindow, 1, iRow);
  BEGIN_MOUSE_IN(winWindow);
  iRow = min(iRow, winWindow->iHeight - 2);
  win_down(winWindow, 1, iRow, winWindow->iWidth - 2, winWindow->iHeight - iRow - 1, 1);
  END_MOUSE;
}

void win_del_zeile (WINDOW winWindow, INT iRow)
{
  MOUSE;

  if (!winWindow)
    winWindow = AKT_WIN;
  CHECK_XY(winWindow, 1, iRow);
  BEGIN_MOUSE_IN(winWindow);
  iRow = min(iRow, winWindow->iHeight - 2);
  win_up(winWindow, 1, iRow, winWindow->iWidth - 2, winWindow->iHeight - iRow - 1, 1);
  END_MOUSE;
}

BOOL win_info (CHAR *pcText)
{
  INT    iLines       = 0,
         iChars       = 0,
         iLine,
         iCounter;
  CHAR  *pcCounter;
  MOUSE;

  if (pcText != NULL)
  {
    pcCounter = pcText;
    for (; ((*(pcCounter - 1) == '\n') || !iLines); pcCounter++)
    {
      for (iCounter = 0; ((*pcCounter != '\n') && *pcCounter);
           pcCounter++)
      {
        iCounter++;
        if (iChars < iCounter)
          iChars = iCounter;
      }
      iLines++;
    }
    if (!(winInfoWindow = win_new(37 - iChars / 2, 10 - iLines / 2, iChars + 6, iLines + 4,
                                  "standard_dialogue", 0)))
      return FALSE;
    if (!win_show(winInfoWindow))
      return FALSE;
    win_set_focus(winInfoWindow);
    pcCounter = pcText;
    BEGIN_MOUSE_IN(winInfoWindow);
    for (iLine = 0; iLine < iLines; iLine++)
    {
      if (iLine)
        pcCounter++;
      for (iCounter = 0; *pcCounter != '\n' && *pcCounter; pcCounter++, iCounter++)
        win_s_z(winInfoWindow, 3 + iCounter, 2 + iLine, *pcCounter);
    }
    END_MOUSE;
  }
  else
    if (winInfoWindow)
      if (!win_delete(winInfoWindow))
        return FALSE;
  return TRUE;
}

CHAR win_l_a (WINDOW winWindow, INT iX, INT iY)
{
  CHARACTER *pcharBuffer;
  CHAR       cReturnVar;
  MOUSE;

  if (!winWindow)
    winWindow = AKT_WIN;
  CHECK_XY(winWindow, iX, iY)(0);
  pcharBuffer = winWindow->pcharSave + iY * REAL_WIDTH(winWindow) + iX;
  if (pcharBuffer->attribute == (CHAR)0xff)
  {
    BEGIN_MOUSE_IN(winWindow);
    cReturnVar = vio_l_a(iX + winWindow->iX, iY + winWindow->iY);
    END_MOUSE;
    return cReturnVar;
  }
  return pcharBuffer->attribute;
}

CHAR win_l_z (WINDOW winWindow, INT iX, INT iY)
{
  CHARACTER *pcharBuffer;
  CHAR       cReturnVar;
  MOUSE;

  if (!winWindow)
    winWindow = AKT_WIN;
  CHECK_XY(winWindow, iX, iY)(0);
  pcharBuffer = winWindow->pcharSave + iY * REAL_WIDTH(winWindow) + iX;
  if (pcharBuffer->character == (CHAR)0xff)
  {
    BEGIN_MOUSE_IN(winWindow);
    cReturnVar = vio_l_z(iX + winWindow->iX, iY + winWindow->iY);
    END_MOUSE;
    return cReturnVar;
  }
  return pcharBuffer->character;
}

CHARACTER win_l_za (WINDOW winWindow, INT iX, INT iY)
{
  CHARACTER charReturnVar = {0, 0};
  MOUSE;

  if (!winWindow)
    winWindow = AKT_WIN;
  CHECK_XY(winWindow, iX, iY)(charReturnVar);
  charReturnVar = *(winWindow->pcharSave + iY * REAL_WIDTH(winWindow) + iX);
  BEGIN_MOUSE_IN(winWindow);
  if (charReturnVar.character == (CHAR)0xff)
    charReturnVar.character = vio_l_z(iX + winWindow->iX, iY + winWindow->iY);
  if (charReturnVar.attribute == (CHAR)0xff)
    charReturnVar.attribute = vio_l_a(iX + winWindow->iX, iY + winWindow->iY);
  END_MOUSE;
  return charReturnVar;
}

void win_s_a (WINDOW winWindow, INT iX, INT iY, CHAR cAttri)
{
  CHARACTER *pcharBuffer;
  MOUSE;

  if (!winWindow)
    winWindow = AKT_WIN;
  CHECK_XY(winWindow, iX, iY);
  pcharBuffer = winWindow->pcharSave + iY * REAL_WIDTH(winWindow) + iX;
  if (pcharBuffer->attribute == (CHAR)0xff)
  {
    BEGIN_MOUSE_IN(winWindow);
    vio_s_a(iX + winWindow->iX, iY + winWindow->iY, cAttri);
    END_MOUSE;
  }
  else
    pcharBuffer->attribute = cAttri;
}

void win_s_z (WINDOW winWindow, INT iX, INT iY, CHAR cChar)
{
  CHARACTER *pcharBuffer;
  MOUSE;

  if (!winWindow)
    winWindow = AKT_WIN;
  CHECK_XY(winWindow, iX, iY);
  pcharBuffer = winWindow->pcharSave + iY * REAL_WIDTH(winWindow) + iX;
  if (pcharBuffer->character == (CHAR)0xff)
  {
    BEGIN_MOUSE_IN(winWindow);
    vio_s_z(iX + winWindow->iX, iY + winWindow->iY, cChar);
    END_MOUSE;
  }
  else
    pcharBuffer->character = cChar;
}

void win_s_za (WINDOW winWindow, INT iX, INT iY, CHAR cChar, CHAR cAttri)
{
  CHARACTER *pcharBuffer;
  MOUSE;

  if (!winWindow)
    winWindow = AKT_WIN;
  CHECK_XY(winWindow, iX, iY);
  BEGIN_MOUSE_IN(winWindow);
  pcharBuffer = winWindow->pcharSave + iY * REAL_WIDTH(winWindow) + iX;
  if (pcharBuffer->character == (CHAR)0xff)
    vio_s_z(iX + winWindow->iX, iY + winWindow->iY, cChar);
  else
    pcharBuffer->character = cChar;
  if (pcharBuffer->attribute == (CHAR)0xff)
    vio_s_a(iX + winWindow->iX, iY + winWindow->iY, cAttri);
  else
    pcharBuffer->attribute = cAttri;
  END_MOUSE;
}

void win_sp_a (WINDOW winWindow, CHAR cAttri)
{
  if (!winWindow)
    winWindow = AKT_WIN;

  win_sw_a(winWindow, 1, 1, winWindow->iWidth - 2, winWindow->iHeight - 2, cAttri);
}

void win_sp_z (WINDOW winWindow, CHAR cChar)
{
  if (!winWindow)
    winWindow = AKT_WIN;

  win_sw_z(winWindow, 1, 1, winWindow->iWidth - 2, winWindow->iHeight - 2, cChar);
}

void win_sp_za (WINDOW winWindow, CHAR cChar, CHAR cAttri)
{
  if (!winWindow)
    winWindow = AKT_WIN;

  win_sw_za(winWindow, 1, 1, winWindow->iWidth - 2, winWindow->iHeight - 2, cChar, cAttri);
}

void win_ss (WINDOW winWindow, INT iX, INT iY, CHAR *pcString)
{
  CHARACTER *pcharBuffer;
  MOUSE;

  if (!winWindow)
    winWindow = AKT_WIN;
  CHECK_XY(winWindow, iX, iY);
  BEGIN_MOUSE_IN(winWindow);
  pcharBuffer = winWindow->pcharSave + REAL_WIDTH(winWindow) * iY + iX;
  while (*pcString && iX < winWindow->iWidth - 1)
  {
    if (pcharBuffer->character == (CHAR)0xff)
      vio_s_z(winWindow->iX + iX, winWindow->iY + iY, *pcString);
    else
      pcharBuffer->character = *pcString;
    iX++;
    pcharBuffer++;
    pcString++;
  }
  END_MOUSE;
}

void win_ss_a (WINDOW winWindow, INT iX, INT iY, CHAR *pcString, CHAR cAttri)
{
  CHARACTER *pcharBuffer;
  MOUSE;

  if (!winWindow)
    winWindow = AKT_WIN;
  CHECK_XY(winWindow, iX, iY);
  BEGIN_MOUSE_IN(winWindow);
  pcharBuffer = winWindow->pcharSave + REAL_WIDTH(winWindow) * iY + iX;
  while (*pcString && iX < winWindow->iWidth - 1)
  {
    if (pcharBuffer->character == (CHAR)0xff)
      vio_s_z(winWindow->iX + iX, winWindow->iY + iY, *pcString);
    else
      pcharBuffer->character = *pcString;
    if (pcharBuffer->attribute == (CHAR)0xff)
      vio_s_a(winWindow->iX + iX, winWindow->iY + iY, cAttri);
    else
      pcharBuffer->attribute = cAttri;
    pcharBuffer++;
    iX++;
    pcString++;
  }
  END_MOUSE;
}

void win_ssn (WINDOW winWindow, INT iX, INT iY, CHAR *pcString, INT iMaxLen)
{
  CHARACTER *pcharBuffer;
  INT        iWritten    = 0;
  MOUSE;

  if (!winWindow)
    winWindow = AKT_WIN;
  CHECK_XY(winWindow, iX, iY);
  BEGIN_MOUSE_IN(winWindow);
  pcharBuffer = winWindow->pcharSave + REAL_WIDTH(winWindow) * iY + iX;
  while (*pcString && iX < winWindow->iWidth - 1 && iWritten < iMaxLen)
  {
    if (pcharBuffer->character == (CHAR)0xff)
      vio_s_z(winWindow->iX + iX, winWindow->iY + iY, *pcString);
    else
      pcharBuffer->character = *pcString;
    iX++;
    pcharBuffer++;
    pcString++;     
    iWritten++;
  }
  END_MOUSE;
}

void win_ssn_a (WINDOW winWindow, INT iX, INT iY, CHAR *pcString, INT iMaxLen, CHAR cAttri)
{
  CHARACTER *pcharBuffer;
  INT        iWritten    = 0;
  MOUSE;

  if (!winWindow)
    winWindow = AKT_WIN;
  CHECK_XY(winWindow, iX, iY);
  BEGIN_MOUSE_IN(winWindow);
  pcharBuffer = winWindow->pcharSave + REAL_WIDTH(winWindow) * iY + iX;
  while (*pcString && iX < winWindow->iWidth - 1 && iWritten < iMaxLen)
  {
    if (pcharBuffer->character == (CHAR)0xff)
      vio_s_z(winWindow->iX + iX, winWindow->iY + iY, *pcString);
    else
      pcharBuffer->character = *pcString;
    if (pcharBuffer->attribute == (CHAR)0xff)
      vio_s_a(winWindow->iX + iX, winWindow->iY + iY, cAttri);
    else
      pcharBuffer->attribute = cAttri;
    pcharBuffer++;
    iX++;
    pcString++;       
    iWritten++;
  }
  END_MOUSE;
}

void win_sw_a (WINDOW winWindow, INT iX, INT iY, INT iWidth, INT iHeight, CHAR cAttri)
{
  CHARACTER *pcharBuffer;
  INT        iCounterX,
             iCounterY,
             iStartX,
             iLength,
             iAdd;
  MOUSE;

  if (!winWindow)
    winWindow = AKT_WIN;
  CHECK_WH(winWindow, iX, iY, iWidth, iHeight);
  BEGIN_MOUSE_IN(winWindow);
  pcharBuffer = winWindow->pcharSave + REAL_WIDTH(winWindow) * iY + iX;
  iAdd = REAL_WIDTH(winWindow) - iWidth;
  for (iCounterY = 0; iCounterY < iHeight; iCounterY++)
  {
    iCounterX = 0;
    while (iCounterX < iWidth)
    {
      iStartX = iCounterX;
      iLength = 0;
      while (pcharBuffer->attribute == (CHAR)0xff && iCounterX < iWidth)
      {
        iCounterX++;
        iLength++;
        pcharBuffer++;
      }
      if (iLength != 0)
        vio_sw_a(winWindow->iX + iX + iStartX, winWindow->iY + iY + iCounterY, iLength, 1, cAttri);
      while (pcharBuffer->attribute != (CHAR)0xff && iCounterX < iWidth)
      {
        pcharBuffer->attribute = cAttri;
        iCounterX++;
        pcharBuffer++;
      }
    }
    pcharBuffer += iAdd;
  }
  END_MOUSE;
}

void win_sw_z (WINDOW winWindow, INT iX, INT iY, INT iWidth, INT iHeight, CHAR cChar)
{
  CHARACTER *pcharBuffer;
  INT        iCounterX,
             iCounterY,
             iStartX,
             iLength,
             iAdd;
  MOUSE;

  if (!winWindow)
    winWindow = AKT_WIN;
  CHECK_WH(winWindow, iX, iY, iWidth, iHeight);
  BEGIN_MOUSE_IN(winWindow);
  pcharBuffer = winWindow->pcharSave + REAL_WIDTH(winWindow) * iY + iX;
  iAdd = REAL_WIDTH(winWindow) - iWidth;
  for (iCounterY = 0; iCounterY < iHeight; iCounterY++)
  {
    iCounterX = 0;
    while (iCounterX < iWidth)
    {
      iStartX = iCounterX;
      iLength = 0;
      while (pcharBuffer->character == (CHAR)0xff && iCounterX < iWidth)
      {
        iCounterX++;
        iLength++;
        pcharBuffer++;
      }
      if (iLength != 0)
        vio_sw_z(winWindow->iX + iX + iStartX, winWindow->iY + iY + iCounterY, iLength, 1, cChar);
      while (pcharBuffer->character != (CHAR)0xff && iCounterX < iWidth)
      {
        pcharBuffer->character = cChar;
        iCounterX++;
        pcharBuffer++;
      }
    }
    pcharBuffer += iAdd;
  }
  END_MOUSE;
}

void win_sw_za (WINDOW winWindow, INT iX, INT iY, INT iWidth, INT iHeight, CHAR cChar, CHAR cAttri)
{
  CHARACTER *pcharBuffer;
  INT        iCounterX,
             iCounterY,
             iStartX,
             iLength,
             iAdd;
  MOUSE;

  if (!winWindow)
    winWindow = AKT_WIN;
  CHECK_WH(winWindow, iX, iY, iWidth, iHeight);
  BEGIN_MOUSE_IN(winWindow);
  pcharBuffer = winWindow->pcharSave + REAL_WIDTH(winWindow) * iY + iX;
  iAdd = REAL_WIDTH(winWindow) - iWidth;
  for (iCounterY = 0; iCounterY < iHeight; iCounterY++)
  {
    iCounterX = 0;
    while (iCounterX < iWidth)
    {
      iStartX = iCounterX;
      iLength = 0;
      while (pcharBuffer->character == (CHAR)0xff && pcharBuffer->attribute == (CHAR)0xff && iCounterX < iWidth)
      {
        iCounterX++;
        iLength++;
        pcharBuffer++;
      }
      if (iLength != 0)
        vio_sw_za(winWindow->iX + iX + iStartX, winWindow->iY + iY + iCounterY, iLength, 1, cChar, cAttri);
      while ((pcharBuffer->character != (CHAR)0xff || pcharBuffer->attribute != (CHAR)0xff) && iCounterX < iWidth)
      {
  if (pcharBuffer->character == (CHAR)0xff)
    vio_s_z(winWindow->iX + iX + iCounterX, winWindow->iY + iY + iCounterY, cChar);
  else
          pcharBuffer->character = cChar;
  if (pcharBuffer->attribute == (CHAR)0xff)
    vio_s_a(winWindow->iX + iX + iCounterX, winWindow->iY + iY + iCounterY, cAttri);
        else
    pcharBuffer->attribute = cAttri;
        iCounterX++;
        pcharBuffer++;
      }
    }
    pcharBuffer += iAdd;
  }
  END_MOUSE;
}

void win_lw (WINDOW winWindow, INT iX, INT iY, INT iWidth, INT iHeight, CHARACTER *pcharDest)
{
  CHARACTER *pcharBuffer;
  INT        iCounterX,
             iCounterY,
             iStartX,
             iLength,
             iAdd;
  MOUSE;

  if (!winWindow)
    winWindow = AKT_WIN;
  CHECK_WH(winWindow, iX, iY, iWidth, iHeight);
  BEGIN_MOUSE_IN(winWindow);
  pcharBuffer = winWindow->pcharSave + REAL_WIDTH(winWindow) * iY + iX;
  iAdd = REAL_WIDTH(winWindow) - iWidth;
  for (iCounterY = 0; iCounterY < iHeight; iCounterY++)
  {
    iCounterX = 0;
    while (iCounterX < iWidth)
    {
      iStartX = iCounterX;
      iLength = 0;
      while (pcharBuffer->character == (CHAR)0xff && pcharBuffer->attribute == (CHAR)0xff && iCounterX < iWidth)
      {
  iLength++;
  iCounterX++;
  pcharBuffer++;
      }
      if (iLength != 0)
      {
  vio_lw(winWindow->iX + iX + iStartX, winWindow->iY + iY + iCounterY, iLength, 1, pcharDest);
  pcharDest += iLength;
      }
      while ((pcharBuffer->character != (CHAR)0xff || pcharBuffer->attribute != (CHAR)0xff) && iCounterX < iWidth)
      {
        if (pcharBuffer->character == (CHAR)0xff)
          pcharDest->character = vio_l_z(winWindow->iX + iX + iCounterX, winWindow->iY + iY + iCounterY);
        else
          pcharDest->character = pcharBuffer->character;
        if (pcharBuffer->attribute == (CHAR)0xff)
          pcharDest->attribute = vio_l_a(winWindow->iX + iX + iCounterX, winWindow->iY + iY + iCounterY);
        else
          pcharDest->attribute = pcharBuffer->attribute;
        pcharBuffer++;
        pcharDest++;
  iCounterX++;
      }
    }
    pcharBuffer += iAdd;
  }
  END_MOUSE;
}

void win_sw (WINDOW winWindow, INT iX, INT iY, INT iWidth, INT iHeight, CHARACTER *pcharSource)
{
  CHARACTER *pcharBuffer;
  INT        iCounterX,
             iCounterY,
             iStartX,
             iLength,
             iAdd;
  MOUSE;

  if (!winWindow)
    winWindow = AKT_WIN;
  CHECK_WH(winWindow, iX, iY, iWidth, iHeight);
  BEGIN_MOUSE_IN(winWindow);
  pcharBuffer = winWindow->pcharSave + REAL_WIDTH(winWindow) * iY + iX;
  iAdd = REAL_WIDTH(winWindow) - iWidth;
  for (iCounterY = 0; iCounterY < iHeight; iCounterY++)
  {
    iCounterX = 0;
    while (iCounterX < iWidth)
    {
      iStartX = iCounterX;
      iLength = 0;
      while (pcharBuffer->character == (CHAR)0xff && pcharBuffer->attribute == (CHAR)0xff && iCounterX < iWidth)
      {
  iLength++;
  iCounterX++;
  pcharBuffer++;
      }
      if (iLength != 0)
      {
  vio_sw(winWindow->iX + iX + iStartX, winWindow->iY + iY + iCounterY, iLength, 1, pcharSource);
  pcharSource += iLength;
      }
      while ((pcharBuffer->character != (CHAR)0xff || pcharBuffer->attribute != (CHAR)0xff) && iCounterX < iWidth)
      {
        if (pcharBuffer->character == (CHAR)0xff)
          vio_s_z(winWindow->iX + iX + iCounterX, winWindow->iY + iY + iCounterY, pcharSource->character);
        else
          pcharBuffer->character = pcharSource->character;
        if (pcharBuffer->attribute == (CHAR)0xff)
          vio_s_a(winWindow->iX + iX + iCounterX, winWindow->iY + iY + iCounterY, pcharSource->attribute);
        else
          pcharBuffer->attribute = pcharSource->attribute;
        pcharBuffer++;
        pcharSource++;
  iCounterX++;
      }
    }
    pcharBuffer += iAdd;
  }
  END_MOUSE;
}

WINDOW win_get_active (void)
{
  return prgProgram.winLast;
}

BOOL win_active (WINDOW winWindow)
{
  if (prgProgram.winLast == winWindow)
    return TRUE;
  return FALSE;
}

BOOL win_visible (WINDOW winWindow)
{
  WINDOW winCounter;

  for (winCounter = prgProgram.winFirst; winCounter;
       winCounter = winCounter->winNext)
    if (winCounter == winWindow)
      return TRUE;
  return FALSE;
}

/*
Local Variables:
compile-command: "wmake -f stools.mk -h -e"
End:
*/
