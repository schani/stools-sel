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
 *                             Borders                                     *
 ***************************************************************************/
                     
void int_dlg_draw_border (DLG_ELEMENT *pelementElement)
{
  DLG_BORDER *pborder;

  pborder = (DLG_BORDER*)pelementElement->pAddInfo;
  if (pborder->reg.iWidth == 1)
    win_sw_za(OWNER, pborder->reg.iX, pborder->reg.iY, 1, pborder->reg.iHeight, '≥',
              win_get_color(OWNER, PAL_COL_BORDER));
  else
    if (pborder->reg.iHeight == 1)
      win_sw_za(OWNER, pborder->reg.iX, pborder->reg.iY, pborder->reg.iWidth, 1, 'ƒ',
                win_get_color(OWNER, PAL_COL_BORDER));
    else
      win_draw_border(OWNER, pborder->reg.iX, pborder->reg.iY, pborder->reg.iWidth, pborder->reg.iHeight,
		      B_EEEE, win_get_color(OWNER, PAL_COL_BORDER));
}

void dlg_border_handle_event (DLG_ELEMENT *pelementElement, UTL_EVENT *peventEvent)
{                         
  DLG_BORDER *pborder;

  pborder = (DLG_BORDER*)pelementElement->pAddInfo;
  switch (peventEvent->uiKind)
  {
    case E_MESSAGE :
      switch (peventEvent->uiMessage)
      {
        case M_INIT :
          int_dlg_draw_border(pelementElement);
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
	  pborder->reg = *(VIO_REGION*)peventEvent->ulAddInfo;
          peventEvent->uiKind = E_DONE;
          break;
        case M_DRAW :                          
          int_dlg_draw_border(pelementElement);
          peventEvent->uiKind = E_DONE;
          break;
      }
      break;
  }
}

DLG_ELEMENT* dlg_init_border (INT iX, INT iY, INT iWidth, INT iHeight, UINT uiID)
{
  DLG_ELEMENT *pelementReturnVar;
  DLG_BORDER  *pborder;

  if (!(pelementReturnVar = utl_alloc(sizeof(DLG_ELEMENT))))
    return(NULL);
  pelementReturnVar->handle_event = dlg_border_handle_event;
  pelementReturnVar->pcHelpLine = NULL;
  pelementReturnVar->uiID = uiID;
  pelementReturnVar->flFlags.binCanBeActivated = FALSE;
  pelementReturnVar->flFlags.binFocussed = FALSE;
  pelementReturnVar->pRecipient = NULL;
  if (!(pelementReturnVar->pAddInfo = utl_alloc(sizeof(DLG_BORDER))))
    return(NULL);
  pborder = (DLG_BORDER*)pelementReturnVar->pAddInfo;
  pborder->reg.iX = iX;
  pborder->reg.iY = iY;
  pborder->reg.iWidth = iWidth;
  pborder->reg.iHeight = iHeight;
  return pelementReturnVar;
}
