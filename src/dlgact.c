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
 *                          Action Buttons                                 *
 ***************************************************************************/

void static int_dlg_draw_act_but (DLG_ELEMENT *pelementElement)
{
  DLG_ACTION_BUTTON *pactbutInfo;
  UCHAR              ucColor;

  pactbutInfo = (DLG_ACTION_BUTTON*)pelementElement->pAddInfo;
  if (pactbutInfo->wHotKey == K_ENTER)
    ucColor = PAL_COL_ACT_BUT_HOT_KEY;
  else
    ucColor = PAL_COL_ACT_BUT;
  win_ss_a(OWNER, pactbutInfo->coord.iX, pactbutInfo->coord.iY, "< ", win_get_color(OWNER, ucColor));
  win_ss_a(OWNER, pactbutInfo->coord.iX + pactbutInfo->iLength + 2, pactbutInfo->coord.iY, " >",
	   win_get_color(OWNER, ucColor));
  win_write_hot(OWNER, pactbutInfo->coord.iX + 2, pactbutInfo->coord.iY, pactbutInfo->pcText,
		win_get_color(OWNER, PAL_COL_ACT_BUT), win_get_color(OWNER, PAL_COL_ACT_BUT_HOT_KEY));
}

void dlg_act_but_handle_event (DLG_ELEMENT *pelementElement, UTL_EVENT *peventEvent)
{
  DLG_ACTION_BUTTON *pactbutInfo;
  UTL_EVENT          eventEvent;

  pactbutInfo = (DLG_ACTION_BUTTON*)pelementElement->pAddInfo;
  win_make_local(OWNER, peventEvent, &eventEvent);
  if (FOCUSSED && OWNER_FOCUSSED && pelementElement->pcHelpLine)
    sts_write_help(pelementElement->pcHelpLine);
  switch (peventEvent->uiKind)
  {
    case E_MESSAGE :
      switch (eventEvent.uiMessage)
      {
        case M_INIT :
        case M_LOST_FOCUS :
          pactbutInfo->bButtonDown = FALSE;
          int_dlg_draw_act_but(pelementElement);
          peventEvent->uiKind = E_DONE;
          break;
        case M_GET_FOCUS :
          win_set_cursor_pos(OWNER, pactbutInfo->coord.iX + 2, pactbutInfo->coord.iY);
          peventEvent->uiKind = E_DONE;
          break;
        case M_QUIT :
          utl_free(pelementElement->pAddInfo);
          peventEvent->uiKind = E_DONE;
          return;
        case M_SET_VALUES :
        case M_QUERY_VALUES :
          peventEvent->uiKind = E_DONE;
          break;
        case M_SET_DISPLAY :
	  pactbutInfo->coord = *(VIO_COORD*)peventEvent->ulAddInfo;
          peventEvent->uiKind = E_DONE;
          break;
        case M_DRAW :
          int_dlg_draw_act_but(pelementElement);
          peventEvent->uiKind = E_DONE;
          break;
      }
      break;
    case E_KEY :
      switch (eventEvent.wKey)
      {
        case K_SPACE :
        case K_ENTER :
          glb_send_message(pelementElement, RECIPIENT, M_ACT_BUT_CLICKED, 0);
          peventEvent->uiKind = E_DONE;
          break;
        default :
          if (dlg_hot_key(&eventEvent, pactbutInfo->wHotKey))
          {
            glb_send_message(pelementElement, RECIPIENT, M_ACT_BUT_CLICKED, 0);
            peventEvent->uiKind = E_DONE;
          }
          break;
      }
      break;
    case E_MSM_MOVE :
      if (pactbutInfo->bButtonDown)
      {
        int_dlg_draw_act_but(pelementElement);
        if (eventEvent.iVer == pactbutInfo->coord.iY && eventEvent.iHor >= pactbutInfo->coord.iX &&
            eventEvent.iHor <= pactbutInfo->coord.iX + pactbutInfo->iLength + 3)
        {
          win_sw_a(OWNER, pactbutInfo->coord.iX, pactbutInfo->coord.iY, pactbutInfo->iLength + 4, 1,
                   win_get_color(OWNER, PAL_COL_ACT_BUT_INVERS));
          peventEvent->uiKind = E_DONE;
        }
      }
      break;
    case E_MSM_L_UP :
      int_dlg_draw_act_but(pelementElement);
      if (eventEvent.iVer == pactbutInfo->coord.iY && eventEvent.iHor >= pactbutInfo->coord.iX &&
          eventEvent.iHor <= pactbutInfo->coord.iX + pactbutInfo->iLength + 3 && pactbutInfo->bButtonDown)
      {
        glb_send_message(pelementElement, RECIPIENT, M_ACT_BUT_CLICKED, 0);
        peventEvent->uiKind = E_DONE;
      }
      pactbutInfo->bButtonDown = FALSE;
      break;
    case E_MSM_L_DOWN :
      if (eventEvent.iVer == pactbutInfo->coord.iY && eventEvent.iHor >= pactbutInfo->coord.iX &&
          eventEvent.iHor <= pactbutInfo->coord.iX + pactbutInfo->iLength + 3)
      {
        win_sw_a(OWNER, pactbutInfo->coord.iX, pactbutInfo->coord.iY, pactbutInfo->iLength + 4, 1,
                 win_get_color(OWNER, PAL_COL_ACT_BUT_INVERS));
        pactbutInfo->bButtonDown = TRUE;
        peventEvent->uiKind = E_DONE;
      }
      break;
  }
  if (!(eventEvent.wButtons & MSM_B_LEFT))
    pactbutInfo->bButtonDown = FALSE;
}

DLG_ELEMENT* dlg_init_act_button(INT iX, INT iY, CHAR *pcText, WORD wHotKey, CHAR *pcHelpLine, UINT uiID,
                                 BOOL bCanBeActivated, void *pRecipient)
{
  DLG_ELEMENT       *pelementReturnVar;
  DLG_ACTION_BUTTON *pactbutInfo;

  if (!(pelementReturnVar = utl_alloc(sizeof(DLG_ELEMENT))))
    return(NULL);
  pelementReturnVar->handle_event = dlg_act_but_handle_event;
  pelementReturnVar->pcHelpLine = pcHelpLine;
  pelementReturnVar->uiID = uiID;
  pelementReturnVar->flFlags.binCanBeActivated = bCanBeActivated;
  pelementReturnVar->flFlags.binFocussed = FALSE;
  pelementReturnVar->pRecipient = pRecipient;
  if (!(pelementReturnVar->pAddInfo = utl_alloc(sizeof(DLG_ACTION_BUTTON))))
    return(NULL);
  pactbutInfo = (DLG_ACTION_BUTTON*)pelementReturnVar->pAddInfo;
  pactbutInfo->coord.iX = iX;
  pactbutInfo->coord.iY = iY;
  pactbutInfo->pcText = pcText;
  pactbutInfo->bButtonDown = FALSE;
  pactbutInfo->iLength = utl_hot_strlen(pcText);
  if (wHotKey != 0)
    pactbutInfo->wHotKey = wHotKey;
  else
    pactbutInfo->wHotKey = (WORD)utl_get_hot_key(pcText);
  return pelementReturnVar;
}
