/***************************************************************************
 ***************************************************************************
 ***                                                                     ***
 ***                       Schani-Tools fÅr C                            ***
 ***                                                                     ***
 ***                         Dialogbox-Teil                              ***
 ***                                                                     ***
 ***                (c) 1990-93 by Schani Electronics                    ***
 ***                                                                     ***
 ***************************************************************************
 ***************************************************************************/

#include <gemein.h>
#include <video.h>
#include <sthelp.h>
#include <dlg.h>
#include <window.h>
#include <utility.h>
#include <global.h>

void dlg_set_focus (WINDOW winWindow, DLG_ELEMENT *pelementElement)
{
  if (pelementElement->flFlags.binCanBeActivated)
  {
    winWindow->pelementActive->flFlags.binFocussed = FALSE;
    glb_send_message(winWindow, winWindow->pelementActive, M_LOST_FOCUS, 0);
    winWindow->pelementActive = pelementElement;
    winWindow->pelementActive->flFlags.binFocussed = TRUE;
    glb_send_message(winWindow, winWindow->pelementActive, M_GET_FOCUS, 0);
  }
}

void dlg_activate_next (WINDOW winWindow)
{
  DLG_ELEMENT *pelementCounter;

  if (winWindow->pelementActive)
  {
    if (winWindow->pelementActive->pelementNext)
      pelementCounter = winWindow->pelementActive->pelementNext;
    else
      pelementCounter = winWindow->pelementFirst;
    for (; !pelementCounter->flFlags.binCanBeActivated; )
      if (pelementCounter->pelementNext)
        pelementCounter = pelementCounter->pelementNext;
      else
        pelementCounter = winWindow->pelementFirst;
    if (pelementCounter != winWindow->pelementActive)
      dlg_set_focus(winWindow, pelementCounter);
  }
}

void dlg_activate_prev (WINDOW winWindow)
{
  DLG_ELEMENT *pelementCounter;

  if (winWindow->pelementActive)
  {
    if (winWindow->pelementActive->pelementPrev)
      pelementCounter = winWindow->pelementActive->pelementPrev;
    else
      pelementCounter = winWindow->pelementLast;
    for (; !pelementCounter->flFlags.binCanBeActivated; )
      if (pelementCounter->pelementPrev)
        pelementCounter = pelementCounter->pelementPrev;
      else
        pelementCounter = winWindow->pelementLast;
    if (pelementCounter != winWindow->pelementActive)
      dlg_set_focus(winWindow, pelementCounter);
  }
}

BOOL dlg_hot_key (UTL_EVENT *peventEvent, WORD wHotKey)
{
  if (wHotKey > 0xff)
  {
    if (peventEvent->wKey == wHotKey)
      return TRUE;
    else
      return FALSE;
  }
  if (!(peventEvent->byKeybStatus & ALT))
  {
    if (peventEvent->wKey < 256)
    {
      if (utl_upper((CHAR)peventEvent->wKey) == utl_upper(wHotKey))
        return TRUE;
    }
#ifdef _MSDOS
    else
      if (peventEvent->wKey == (wHotKey | EXT_CODE))
        return TRUE;
#endif
  }
  else
    if (peventEvent->wKey == utl_alt_code((CHAR)wHotKey))
      return TRUE;
  return FALSE;
}
