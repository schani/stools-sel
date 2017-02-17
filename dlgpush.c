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
#include <mouse.h>
#include <sthelp.h>
#include <dlg.h>
#include <window.h>
#include <utility.h>
#include <global.h>
#include <contain.h>
#include <menu.h>
#include <status.h>
#include <stdlib.h>
#include <string.h>

extern GLB_PROGRAM prgProgram;

/***************************************************************************
 *                          Push Buttons                                   *
 ***************************************************************************/

void static int_dlg_draw_push_button (DLG_ELEMENT *pelementElement)
{
  DLG_PUSH_BUTTON *ppushbut;

  ppushbut = (DLG_PUSH_BUTTON*)pelementElement->pAddInfo;
  win_ss_a(OWNER, ppushbut->coord.iX, ppushbut->coord.iY, "[ ]", win_get_color(OWNER, PAL_COL_BACKGROUND));
  if (ppushbut->bActive)
    win_s_za(OWNER, ppushbut->coord.iX + 1, ppushbut->coord.iY, 'X', win_get_color(OWNER, PAL_COL_BACKGROUND));
  win_write_hot(OWNER, ppushbut->coord.iX + 4, ppushbut->coord.iY, ppushbut->pcText,
		win_get_color(OWNER, PAL_COL_LABEL), win_get_color(OWNER, PAL_COL_LABEL_HOT_KEY));
}

void dlg_push_but_handle_event (DLG_ELEMENT *pelementElement, UTL_EVENT *peventEvent)
{
  DLG_PUSH_BUTTON *ppushbut;
  UTL_EVENT        eventEvent;

  ppushbut = (DLG_PUSH_BUTTON*)pelementElement->pAddInfo;
  win_make_local(OWNER, peventEvent, &eventEvent);
  if (FOCUSSED && OWNER_FOCUSSED && pelementElement->pcHelpLine)
    sts_write_help(pelementElement->pcHelpLine);
  switch (eventEvent.uiKind)
  {
    case E_MESSAGE :
      switch (eventEvent.uiMessage)
      {
        case M_INIT :
          int_dlg_draw_push_button(pelementElement);
          peventEvent->uiKind = E_DONE;
          break;
        case M_GET_FOCUS :
          win_set_cursor_pos(OWNER, ppushbut->coord.iX + 1, ppushbut->coord.iY);
        case M_LOST_FOCUS :
          if (eventEvent.uiMessage == M_LOST_FOCUS)
            ppushbut->bButtonDown = FALSE;
          peventEvent->uiKind = E_DONE;
          break;
        case M_QUIT :
          utl_free(pelementElement->pAddInfo);
          peventEvent->uiKind = E_DONE;
          return;
        case M_SET_VALUES :
          if (ppushbut->bActive = (BOOL)peventEvent->ulAddInfo)
            win_s_z(OWNER, ppushbut->coord.iX + 1, ppushbut->coord.iY, ' ');
          else
            win_s_z(OWNER, ppushbut->coord.iX + 1, ppushbut->coord.iY, 'X');
          peventEvent->uiKind = E_DONE;
          break;
        case M_QUERY_VALUES :
          *(BOOL*)peventEvent->ulAddInfo = ppushbut->bActive;
          peventEvent->uiKind = E_DONE;
          break;                       
        case M_SET_DISPLAY :
	  ppushbut->coord = *(VIO_COORD*)peventEvent->ulAddInfo;
          peventEvent->uiKind = E_DONE;
          break;
        case M_DRAW :
          int_dlg_draw_push_button(pelementElement);
          peventEvent->uiKind = E_DONE;
          break;
      }
      break;
    case E_KEY :
      switch (eventEvent.wKey)
      {
        case K_SPACE :
          if (!FOCUSSED)
            break;
          ppushbut->bActive = !ppushbut->bActive;
          if (ppushbut->bActive)
            win_s_za(OWNER, ppushbut->coord.iX + 1, ppushbut->coord.iY, 'X',
		     win_get_color(OWNER, PAL_COL_BACKGROUND));
          else
            win_s_za(OWNER, ppushbut->coord.iX + 1, ppushbut->coord.iY, ' ', 
                     win_get_color(OWNER, PAL_COL_BACKGROUND));
          glb_send_message(pelementElement, RECIPIENT, M_PUSH_BUT_CHANGED, ppushbut->bActive);
          peventEvent->uiKind = E_DONE;
          break;
        default :
          if (dlg_hot_key(&eventEvent, ppushbut->wHotKey))
          {
            ppushbut->bActive = !ppushbut->bActive;
            if (ppushbut->bActive)
              win_s_za(OWNER, ppushbut->coord.iX + 1, ppushbut->coord.iY, 'X',
		       win_get_color(OWNER, PAL_COL_BACKGROUND));
            else
              win_s_za(OWNER, ppushbut->coord.iX + 1, ppushbut->coord.iY, ' ',
                       win_get_color(OWNER, PAL_COL_BACKGROUND));
            glb_send_message(pelementElement, RECIPIENT, M_PUSH_BUT_CHANGED, ppushbut->bActive);
            peventEvent->uiKind = E_DONE;
          }
          break;
      }
      break;
    case E_MSM_L_UP :
      if (eventEvent.iVer == ppushbut->coord.iY && eventEvent.iHor >= ppushbut->coord.iX &&
          eventEvent.iHor <= ppushbut->coord.iX + utl_hot_strlen(ppushbut->pcText) + 3 && ppushbut->bButtonDown)
      {
        ppushbut->bActive = !ppushbut->bActive;
        if (ppushbut->bActive)
          win_s_za(OWNER, ppushbut->coord.iX + 1, ppushbut->coord.iY, 'X',
		   win_get_color(OWNER, PAL_COL_BACKGROUND));
        else
          win_s_za(OWNER, ppushbut->coord.iX + 1, ppushbut->coord.iY, ' ',
		   win_get_color(OWNER, PAL_COL_BACKGROUND));
        glb_send_message(pelementElement, RECIPIENT, M_PUSH_BUT_CHANGED, ppushbut->bActive);
        peventEvent->uiKind = E_DONE;
      }
      ppushbut->bButtonDown = FALSE;
      break;
    case E_MSM_L_DOWN :
      if (eventEvent.iVer == ppushbut->coord.iY && eventEvent.iHor >= ppushbut->coord.iX &&
	  eventEvent.iHor <= (ppushbut->coord.iX + utl_hot_strlen(ppushbut->pcText) + 3))
      {
        ppushbut->bButtonDown = TRUE;
        peventEvent->uiKind = E_DONE;
      }
      break;
  }
  if (!(eventEvent.wButtons & MSM_B_LEFT))
    ppushbut->bButtonDown = FALSE;
}

DLG_ELEMENT* dlg_init_push_button(INT iX, INT iY, CHAR *pcText, BOOL bActive, CHAR *pcHelpLine, UINT uiID,
                                  BOOL bCanBeActivated, void *pRecipient)
{
  DLG_ELEMENT     *pelementReturnVar;
  DLG_PUSH_BUTTON *ppushbut;

  if (!(pelementReturnVar = utl_alloc(sizeof(DLG_ELEMENT))))
    return NULL;
  pelementReturnVar->handle_event = dlg_push_but_handle_event;
  pelementReturnVar->pcHelpLine = pcHelpLine;
  pelementReturnVar->uiID = uiID;
  pelementReturnVar->flFlags.binCanBeActivated = bCanBeActivated;
  pelementReturnVar->flFlags.binFocussed = FALSE;
  pelementReturnVar->pRecipient = pRecipient;
  if (!(pelementReturnVar->pAddInfo = utl_alloc(sizeof(DLG_PUSH_BUTTON))))
    return NULL;
  ppushbut = (DLG_PUSH_BUTTON*)pelementReturnVar->pAddInfo;
  ppushbut->coord.iX = iX;
  ppushbut->coord.iY = iY;
  ppushbut->pcText = pcText;
  ppushbut->bActive = bActive;
  ppushbut->bButtonDown = FALSE;
  ppushbut->wHotKey = utl_get_hot_key(pcText);
  return pelementReturnVar;
}
