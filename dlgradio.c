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
 *                          Radio Buttons                                  *
 ***************************************************************************/

void static int_dlg_draw_radio_but (DLG_ELEMENT *pelementElement)
{
  DLG_RADIO_BUTTON *pradiobut;
  DLG_BUTTON       *pbut;

  pradiobut = (DLG_RADIO_BUTTON*)pelementElement->pAddInfo;
  for (pbut = pradiobut->pbutFirst; pbut->pcText; pbut++)
  {
    win_ss_a(OWNER, pbut->coord.iX, pbut->coord.iY, "( )", win_get_color(OWNER, PAL_COL_BACKGROUND));
    win_write_hot(OWNER, pbut->coord.iX + 4, pbut->coord.iY, pbut->pcText, win_get_color(OWNER, PAL_COL_LABEL),
                  win_get_color(OWNER, PAL_COL_LABEL_HOT_KEY));
  }
  pbut = pradiobut->pbutFirst;
  win_s_za(OWNER, pbut[pradiobut->iActive].coord.iX + 1, pbut[pradiobut->iActive].coord.iY, 7,
           win_get_color(OWNER, PAL_COL_BACKGROUND));
}

void static int_dlg_change_radio_but(DLG_ELEMENT *pelementElement, INT iActive)
{
  DLG_RADIO_BUTTON *pradiobut;

  pradiobut = (DLG_RADIO_BUTTON*)pelementElement->pAddInfo;
  win_s_za(OWNER, pradiobut->pbutFirst[pradiobut->iActive].coord.iX + 1,
	   pradiobut->pbutFirst[pradiobut->iActive].coord.iY, ' ', win_get_color(OWNER, PAL_COL_BACKGROUND));
  pradiobut->iActive = iActive;
  win_s_za(OWNER, pradiobut->pbutFirst[pradiobut->iActive].coord.iX + 1,
	   pradiobut->pbutFirst[pradiobut->iActive].coord.iY, 7, win_get_color(OWNER, PAL_COL_BACKGROUND));
  if (FOCUSSED)
    win_set_cursor_pos(OWNER, pradiobut->pbutFirst[pradiobut->iActive].coord.iX + 1,
		       pradiobut->pbutFirst[pradiobut->iActive].coord.iY);
  glb_send_message(pelementElement, RECIPIENT, M_RADIO_BUT_CHANGED, iActive);
}

void dlg_radio_but_handle_event(DLG_ELEMENT *pelementElement, UTL_EVENT *peventEvent)
{
  DLG_RADIO_BUTTON *pradiobut;
  DLG_BUTTON       *pbut;
  UTL_EVENT         eventEvent;
  VIO_COORD         coord;

  pradiobut = (DLG_RADIO_BUTTON*)pelementElement->pAddInfo;
  win_make_local(OWNER, peventEvent, &eventEvent);
  if (FOCUSSED && OWNER_FOCUSSED && pelementElement->pcHelpLine)
    sts_write_help(pelementElement->pcHelpLine);
  switch (eventEvent.uiKind)
  {
    case E_MESSAGE :
      switch (eventEvent.uiMessage)
      {
        case M_INIT :
          int_dlg_draw_radio_but(pelementElement);
          peventEvent->uiKind = E_DONE;
          break;
        case M_GET_FOCUS :
          coord = pradiobut->pbutFirst[pradiobut->iActive].coord;
          win_set_cursor_pos(OWNER, coord.iX + 1, coord.iY);
        case M_LOST_FOCUS :
          if (eventEvent.uiMessage == M_LOST_FOCUS)
            pradiobut->bButtonDown = FALSE;
          peventEvent->uiKind = E_DONE;
          break;
        case M_QUIT :
          utl_free(pelementElement->pAddInfo);
          peventEvent->uiKind = E_DONE;
          return;
        case M_SET_VALUES :
          pradiobut->iActive = (INT)peventEvent->ulAddInfo;
          int_dlg_draw_radio_but(pelementElement);
          peventEvent->uiKind = E_DONE;
          break;
        case M_QUERY_VALUES :
          *(INT*)peventEvent->ulAddInfo = pradiobut->iActive;
          peventEvent->uiKind = E_DONE;
          break;
        case M_SET_DISPLAY :
	  coord = *(VIO_COORD*)peventEvent->ulAddInfo;
          for (pbut = pradiobut->pbutFirst; pbut->pcText; pbut++)             
          {
            pbut->coord.iX += coord.iX;
            pbut->coord.iY += coord.iY;
          }     
          peventEvent->uiKind = E_DONE;
          break;
        case M_DRAW :
          int_dlg_draw_radio_but(pelementElement);
          peventEvent->uiKind = E_DONE;
          break;
      }
      break;
    case E_KEY :
      switch (peventEvent->wKey)
      {
        case K_UP :
          if (!FOCUSSED)
            break;
          if (pradiobut->iActive > 0)
            int_dlg_change_radio_but(pelementElement, pradiobut->iActive - 1);
          peventEvent->uiKind = E_DONE;
          break;
        case K_DOWN :
          if (!FOCUSSED)
            break;
          if (pradiobut->pbutFirst[pradiobut->iActive + 1].pcText)
            int_dlg_change_radio_but(pelementElement, pradiobut->iActive + 1);
          peventEvent->uiKind = E_DONE;
          break;
        default :
          for (pbut = pradiobut->pbutFirst; pbut->pcText; pbut++)
            if (dlg_hot_key(&eventEvent, pbut->wHotKey))
            {
              int_dlg_change_radio_but(pelementElement, pbut - pradiobut->pbutFirst);
              peventEvent->uiKind = E_DONE;
	      break;
            }
          break;
      }
      break;
    case E_MSM_L_DOWN :
      pradiobut->bButtonDown = TRUE;
    case E_MSM_MOVE :
      if (pradiobut->bButtonDown && (eventEvent.uiKind == E_MSM_L_DOWN || FOCUSSED))
        for (pbut = pradiobut->pbutFirst; pbut->pcText; pbut++)
          if (eventEvent.iVer == pbut->coord.iY && eventEvent.iHor >= pbut->coord.iX &&
              eventEvent.iHor <= pbut->coord.iX + utl_hot_strlen(pbut->pcText) + 3)
          {
            if (pelementElement->flFlags.binCanBeActivated)
              win_set_cursor_pos(OWNER, pbut->coord.iX + 1, pbut->coord.iY);
            peventEvent->uiKind = E_DONE;
            break;
          }
      if (eventEvent.uiKind == E_MSM_L_DOWN && peventEvent->uiKind != E_DONE)
        pradiobut->bButtonDown = FALSE;
      break;
    case E_MSM_L_UP :
      pradiobut->bButtonDown = FALSE;
      for (pbut = pradiobut->pbutFirst; pbut->pcText; pbut++)
        if (eventEvent.iVer == pbut->coord.iY && eventEvent.iHor >= pbut->coord.iX &&
            eventEvent.iHor <= pbut->coord.iX + utl_hot_strlen(pbut->pcText) + 3)
        {
          int_dlg_change_radio_but(pelementElement, pbut - pradiobut->pbutFirst);
          peventEvent->uiKind = E_DONE;
          break;
        }
      break;
  }
  if (!(eventEvent.wButtons & MSM_B_LEFT))
    pradiobut->bButtonDown = FALSE;
}

DLG_ELEMENT* dlg_init_radio_button(DLG_BUTTON *pbutFirst, INT iActive, CHAR *pcHelpLine, UINT uiID,
                                   BOOL bCanBeActivated, void *pRecipient)
{
  DLG_ELEMENT      *pelementReturnVar;
  DLG_RADIO_BUTTON *pradiobut;
  DLG_BUTTON       *pbut;

  if (!(pelementReturnVar = utl_alloc(sizeof(DLG_ELEMENT))))
    return NULL;
  pelementReturnVar->handle_event = dlg_radio_but_handle_event;
  pelementReturnVar->pcHelpLine = pcHelpLine;
  pelementReturnVar->uiID = uiID;
  pelementReturnVar->flFlags.binCanBeActivated = bCanBeActivated;
  pelementReturnVar->flFlags.binFocussed = FALSE;
  pelementReturnVar->pRecipient = pRecipient;
  if (!(pelementReturnVar->pAddInfo = utl_alloc(sizeof(DLG_RADIO_BUTTON))))
    return NULL;
  pradiobut = (DLG_RADIO_BUTTON*)pelementReturnVar->pAddInfo;
  pradiobut->pbutFirst = pbutFirst;
  pradiobut->iActive = iActive;
  pradiobut->bButtonDown = FALSE;
  for (pbut = pbutFirst; pbut->pcText; pbut++)
    pbut->wHotKey = (WORD)utl_get_hot_key(pbut->pcText);
  return pelementReturnVar;
}
