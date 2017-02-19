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

/***************************************************************************
 *                          Push Buttons                                   *
 ***************************************************************************/

void dlg_label_handle_event (DLG_ELEMENT *pelementElement, UTL_EVENT *peventEvent)
{
  DLG_LABEL *plabel;
  UTL_EVENT  eventEvent;

  plabel = (DLG_LABEL*)pelementElement->pAddInfo;
  win_make_local(OWNER, peventEvent, &eventEvent);
  switch (eventEvent.uiKind)
  {
    case E_MESSAGE :
      switch (eventEvent.uiMessage)
      {
        case M_INIT :
          win_write_hot(OWNER, plabel->coord.iX, plabel->coord.iY, plabel->pcText,
                        win_get_color(OWNER, PAL_COL_LABEL), win_get_color(OWNER, PAL_COL_LABEL_HOT_KEY));
          peventEvent->uiKind = E_DONE;
          break;
        case M_QUIT :
          utl_free(pelementElement->pAddInfo);
          peventEvent->uiKind = E_DONE;
          break;
        case M_SET_VALUES :
        case M_QUERY_VALUES :
          peventEvent->uiKind = E_DONE;
          break;
        case M_SET_DISPLAY :
	  plabel->coord = *(VIO_COORD*)peventEvent->ulAddInfo;
          peventEvent->uiKind = E_DONE;
          break;
        case M_DRAW :
          win_write_hot(OWNER, plabel->coord.iX, plabel->coord.iY, plabel->pcText,
                        win_get_color(OWNER, PAL_COL_LABEL), win_get_color(OWNER, PAL_COL_LABEL_HOT_KEY));
          peventEvent->uiKind = E_DONE;
          break;
      }
      break;
    case E_KEY :
      if (!plabel->pelementElement)
        break;
      if (dlg_hot_key(&eventEvent, plabel->wHotKey))
      {
        dlg_set_focus(OWNER, plabel->pelementElement);
        peventEvent->uiKind = E_DONE;
      }
      break;
    case E_MSM_L_DOWN :
      if (!plabel->pelementElement)
        break;
      if (eventEvent.iVer == plabel->coord.iY && eventEvent.iHor >= plabel->coord.iX &&
          eventEvent.iHor < plabel->coord.iX + utl_hot_strlen(plabel->pcText))
      {
        dlg_set_focus(OWNER, plabel->pelementElement);
        peventEvent->uiKind = E_DONE;
      }
      break;
  }
}

DLG_ELEMENT* dlg_init_label (INT iX, INT iY, CHAR *pcText, DLG_ELEMENT *pelementElement, UINT uiID)
{
  DLG_ELEMENT *pelementReturnVar;
  DLG_LABEL   *plabel;

  if (!(pelementReturnVar = utl_alloc(sizeof(DLG_ELEMENT))))
    return(NULL);
  pelementReturnVar->handle_event = dlg_label_handle_event;
  pelementReturnVar->pcHelpLine = NULL;
  pelementReturnVar->uiID = uiID;
  pelementReturnVar->flFlags.binCanBeActivated = FALSE;
  pelementReturnVar->flFlags.binFocussed = FALSE;
  pelementReturnVar->pRecipient = NULL;
  if (!(pelementReturnVar->pAddInfo = utl_alloc(sizeof(DLG_LABEL))))
    return(NULL);
  plabel = (DLG_LABEL*)pelementReturnVar->pAddInfo;
  plabel->coord.iX = iX;
  plabel->coord.iY = iY;
  plabel->pcText = pcText;
  plabel->pelementElement = pelementElement;
  plabel->wHotKey = utl_get_hot_key(pcText);
  return pelementReturnVar;
}

/*
Local Variables:
compile-command: "wmake -f stools.mk -h -e"
End:
*/
