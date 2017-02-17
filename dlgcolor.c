/***************************************************************************
 ***************************************************************************
 ***                                                                     ***
 ***                       Schani-Tools fÅr C                            ***
 ***                                                                     ***
 ***                         Dialogbox-Teil                              ***
 ***                                                                     ***
 ***                (c) 1990-94 by Schani Electronics                    ***
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
 *                           Color Fields                                  *
 ***************************************************************************/

void static int_dlg_draw_color_field (DLG_ELEMENT *pelementElement)
{
  DLG_COLOR_FIELD *pcf;
  CHAR             cChar     = 'X',
                   cCounter;
                           
  pcf = (DLG_COLOR_FIELD*)pelementElement->pAddInfo;
  if (!pcf->bIntensive)
    cChar = 219;
  for (cCounter = 0; cCounter < 16; cCounter++)
    if (cCounter < 8 || pcf->bIntensive)
      win_sw_za(OWNER, pcf->coord.iX + (cCounter % 4) * 3, pcf->coord.iY + cCounter / 4, 3, 1, cChar,
		cCounter + 112);
  win_s_za(OWNER, pcf->coord.iX + (pcf->cColor % 4) * 3 + 1, pcf->coord.iY + pcf->cColor / 4, 177, 7);
  if (FOCUSSED)
    win_set_cursor_pos(OWNER, pcf->coord.iX + (pcf->cColor % 4) * 3 + 1, pcf->coord.iY + pcf->cColor / 4);
}

void dlg_color_field_handle_event (DLG_ELEMENT *pelementElement, UTL_EVENT *peventEvent)
{
  DLG_COLOR_FIELD *pcf;
  UTL_EVENT        eventEvent;

  pcf = (DLG_COLOR_FIELD*)pelementElement->pAddInfo;
  win_make_local(OWNER, peventEvent, &eventEvent);
  if (FOCUSSED && OWNER_FOCUSSED && pelementElement->pcHelpLine)
    sts_write_help(pelementElement->pcHelpLine);
  switch (peventEvent->uiKind)
  {
    case E_MESSAGE :
      switch (eventEvent.uiMessage)
      {
        case M_INIT :
          int_dlg_draw_color_field(pelementElement);
          peventEvent->uiKind = E_DONE;
          break;
        case M_GET_FOCUS :
          win_set_cursor_pos(OWNER, pcf->coord.iX + (pcf->cColor % 4) * 3 + 1, pcf->coord.iY + pcf->cColor / 4);
          peventEvent->uiKind = E_DONE;
        case M_LOST_FOCUS :
          if (eventEvent.uiMessage == M_LOST_FOCUS)
            pcf->bDragging = FALSE;
          peventEvent->uiKind = E_DONE;
          break;
        case M_QUIT :
          utl_free(pelementElement->pAddInfo);
          peventEvent->uiKind = E_DONE;
          return;
        case M_SET_VALUES :
          pcf->cColor = peventEvent->ulAddInfo;
          int_dlg_draw_color_field(pelementElement);
          peventEvent->uiKind = E_DONE;
          break;
        case M_QUERY_VALUES :
          *(CHAR*)peventEvent->ulAddInfo = pcf->cColor;
          peventEvent->uiKind = E_DONE;
          break;            
        case M_SET_DISPLAY :
	  pcf->coord = *(VIO_COORD*)peventEvent->ulAddInfo;
          peventEvent->uiKind = E_DONE;
          break;
        case M_DRAW :
          int_dlg_draw_color_field(pelementElement);
          peventEvent->uiKind = E_DONE;
          break;                  
      }
      break;
    case E_KEY :
      switch (eventEvent.wKey)
      {
        case K_LEFT :
          if (!FOCUSSED)
            break;
          if (pcf->cColor)
          {
            pcf->cColor--;
            int_dlg_draw_color_field(pelementElement);
            glb_send_message(pelementElement, RECIPIENT, M_COLOR_FIELD_CHANGED, pcf->cColor);
          }
          peventEvent->uiKind = E_DONE;
          break;
        case K_SPACE :
        case K_RIGHT :
          if (!FOCUSSED)
            break;
          if ((pcf->bIntensive && pcf->cColor < 15) || (!pcf->bIntensive && pcf->cColor < 7))
          {
            pcf->cColor++;
            int_dlg_draw_color_field(pelementElement);
            glb_send_message(pelementElement, RECIPIENT, M_COLOR_FIELD_CHANGED, pcf->cColor);
          }
          peventEvent->uiKind = E_DONE;
          break;
        case K_UP :
          if (!FOCUSSED)
            break;
          if (pcf->cColor > 3)
          {
            pcf->cColor -= 4;
            int_dlg_draw_color_field(pelementElement);
            glb_send_message(pelementElement, RECIPIENT, M_COLOR_FIELD_CHANGED, pcf->cColor);
          }
          peventEvent->uiKind = E_DONE;
          break;
        case K_DOWN :
          if (!FOCUSSED)
            break;
          if ((pcf->bIntensive && pcf->cColor < 12) || (!pcf->bIntensive && pcf->cColor < 4))
          {
            pcf->cColor += 4;
            int_dlg_draw_color_field(pelementElement);
            glb_send_message(pelementElement, RECIPIENT, M_COLOR_FIELD_CHANGED, pcf->cColor);
          }
          peventEvent->uiKind = E_DONE;
          break;
      }
      break;
    case E_MSM_L_UP :
      eventEvent.iHor -= pcf->coord.iX;
      eventEvent.iVer -= pcf->coord.iY;
      pcf->bDragging = FALSE;
      if (IN_COLOR_FIELD)
        peventEvent->uiKind = E_DONE;
      break;
    case E_MSM_L_DOWN :
      eventEvent.iHor -= pcf->coord.iX;
      eventEvent.iVer -= pcf->coord.iY;
      if (IN_COLOR_FIELD)   
      {
        pcf->bDragging = TRUE;
        pcf->cColor = eventEvent.iHor / 3 + eventEvent.iVer * 4;
        glb_send_message(pelementElement, RECIPIENT, M_COLOR_FIELD_CHANGED, pcf->cColor);
        int_dlg_draw_color_field(pelementElement);
        peventEvent->uiKind = E_DONE;             
      }
      break;
    case E_MSM_MOVE :
      eventEvent.iHor -= pcf->coord.iX;
      eventEvent.iVer -= pcf->coord.iY;
      if (IN_COLOR_FIELD && pcf->bDragging)   
      {
        pcf->cColor = eventEvent.iHor / 3 + eventEvent.iVer * 4;
        glb_send_message(pelementElement, RECIPIENT, M_COLOR_FIELD_CHANGED, pcf->cColor);
        int_dlg_draw_color_field(pelementElement);
        peventEvent->uiKind = E_DONE;             
      }
      break;
  }
  if (!(eventEvent.wButtons & MSM_B_LEFT))
    pcf->bDragging = FALSE;
}

DLG_ELEMENT* dlg_init_color_field(INT iX, INT iY, BOOL bIntensive, CHAR cColor, CHAR *pcHelpLine, UINT uiID,
                                  BOOL bCanBeActivated, void *pRecipient)
{
  DLG_ELEMENT     *pelementReturnVar;
  DLG_COLOR_FIELD *pcf;

  if (!(pelementReturnVar = utl_alloc(sizeof(DLG_ELEMENT))))
    return NULL;
  pelementReturnVar->handle_event = dlg_color_field_handle_event;
  pelementReturnVar->pcHelpLine = pcHelpLine;
  pelementReturnVar->uiID = uiID;
  pelementReturnVar->flFlags.binCanBeActivated = bCanBeActivated;
  pelementReturnVar->flFlags.binFocussed = FALSE;
  pelementReturnVar->pRecipient = pRecipient;
  if (!(pelementReturnVar->pAddInfo = utl_alloc(sizeof(DLG_COLOR_FIELD))))
    return NULL;
  pcf = (DLG_COLOR_FIELD*)pelementReturnVar->pAddInfo;
  pcf->coord.iX = iX;
  pcf->coord.iY = iY;
  pcf->bIntensive = bIntensive;
  pcf->cColor = cColor;
  pcf->bDragging = FALSE;
  return pelementReturnVar;
}
