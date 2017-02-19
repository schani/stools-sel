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
 *                            Scroll Bar                                   *
 ***************************************************************************/
      
void static int_dlg_draw_scroll (DLG_ELEMENT *pelementElement)
{
  DLG_SCROLL *pscroll;
  INT         i;
  BOOL        bClipping;

  pscroll = (DLG_SCROLL*)pelementElement->pAddInfo;
  bClipping = win_clipping(OWNER, QUERY);
  if (pscroll->bNoClipping)
    win_clipping(OWNER, FALSE);
  win_s_za(OWNER, pscroll->coord.iX, pscroll->coord.iY, UP_ARROW, win_get_color(OWNER, PAL_COL_SCROLL_BAR));
  win_s_za(OWNER, SCROLL_X(pscroll->iLength), SCROLL_Y(pscroll->iLength), DOWN_ARROW,
     win_get_color(OWNER, PAL_COL_SCROLL_BAR));
  for (i = 0; i < pscroll->iLength; i++)
    if (i == pscroll->iPos)
      win_s_za(OWNER, SCROLL_X(i), SCROLL_Y(i), (CHAR)219, win_get_color(OWNER, PAL_COL_SCROLL_BAR));
    else
      win_s_za(OWNER, SCROLL_X(i), SCROLL_Y(i), (CHAR)176, win_get_color(OWNER, PAL_COL_SCROLL_BAR));
  win_clipping(OWNER, bClipping);
}             

void static int_dlg_scroll_set_pos (DLG_ELEMENT *pelementElement, INT iPos)
{
  DLG_SCROLL *pscroll;
  BOOL        bClipping;

  pscroll = (DLG_SCROLL*)pelementElement->pAddInfo;
  if (iPos == pscroll->iPos || iPos < 0 || iPos > pscroll->iLength - 1)
    return;
  bClipping = win_clipping(OWNER, QUERY);
  if (pscroll->bNoClipping)
    win_clipping(OWNER, FALSE);
  win_s_z(OWNER, SCROLL_X(pscroll->iPos), SCROLL_Y(pscroll->iPos), (CHAR)176);
  pscroll->iPos = iPos;
  win_s_z(OWNER, SCROLL_X(pscroll->iPos), SCROLL_Y(pscroll->iPos), (CHAR)219);
  win_clipping(OWNER, bClipping);
}

void dlg_scroll_handle_event(DLG_ELEMENT *pelementElement, UTL_EVENT *peventEvent)
{
  DLG_SCROLL *pscroll;
  UTL_EVENT   eventEvent;
  VIO_REGION *preg;

  pscroll = (DLG_SCROLL*)pelementElement->pAddInfo;
  win_make_local(OWNER, peventEvent, &eventEvent);
  if (FOCUSSED && OWNER_FOCUSSED && pelementElement->pcHelpLine)
    sts_write_help(pelementElement->pcHelpLine);
  switch (eventEvent.uiKind)
  {                     
    case E_MESSAGE :
      switch (eventEvent.uiMessage)
      {
        case M_INIT :
          if (pscroll->ucType == DLG_HORIZONTAL)
          {
            if (pscroll->coord.iY == OWNER->iHeight - 1)
              pscroll->bNoClipping = TRUE;
            else
              pscroll->bNoClipping = FALSE;
          }
          else
          {
            if (pscroll->coord.iX == OWNER->iWidth - 1)
              pscroll->bNoClipping = TRUE;
            else
              pscroll->bNoClipping = FALSE;
          }
          int_dlg_draw_scroll(pelementElement);
          peventEvent->uiKind = E_DONE;
          break;
        case M_GET_FOCUS :
          win_set_cursor_pos(OWNER, pscroll->coord.iX, pscroll->coord.iY);
        case M_LOST_FOCUS :
          if (eventEvent.uiMessage == M_LOST_FOCUS)
          {                                 
            if (pscroll->bDragging)
            {
              pscroll->bDragging = FALSE;
              glb_send_message(pelementElement, RECIPIENT, M_SCROLL_END_DRAGGING, pscroll->ucType);
            }
          }
          peventEvent->uiKind = E_DONE;
          break;
        case M_QUIT :
          utl_free(pelementElement->pAddInfo);
          peventEvent->uiKind = E_DONE;
          return;
        case M_SET_VALUES : 
          int_dlg_scroll_set_pos(pelementElement, (INT)peventEvent->ulAddInfo);
          peventEvent->uiKind = E_DONE;
          break;
        case M_QUERY_VALUES :
          *(INT*)peventEvent->ulAddInfo = pscroll->iPos;
          peventEvent->uiKind = E_DONE;
          break;
        case M_DRAW :
          int_dlg_draw_scroll(pelementElement);
          peventEvent->uiKind = E_DONE;
          break;
        case M_SET_DISPLAY :
          preg = (VIO_REGION*)peventEvent->ulAddInfo;
          pscroll->coord.iX = preg->iX;
          pscroll->coord.iY = preg->iY;
          pscroll->iPos = (pscroll->iPos * preg->iWidth) / pscroll->iLength;
          pscroll->iLength = preg->iWidth;
          peventEvent->uiKind = E_DONE;
          break;
      }
      break;
    case E_KEY :
      switch (eventEvent.wKey)
      {
        case K_LEFT :
        case K_UP :   
          if (!FOCUSSED)
            break;
          glb_send_message(pelementElement, RECIPIENT, M_SCROLL_UP, pscroll->ucType);
          peventEvent->uiKind = E_DONE;
          break;                       
        case K_RIGHT :
        case K_DOWN :      
          if (!FOCUSSED)
            break;
          glb_send_message(pelementElement, RECIPIENT, M_SCROLL_DOWN, pscroll->ucType);
          peventEvent->uiKind = E_DONE;
          break;        
        case K_PGUP :
          if (!FOCUSSED)
            break;
          glb_send_message(pelementElement, RECIPIENT, M_SCROLL_PAGE_UP, pscroll->ucType);
          peventEvent->uiKind = E_DONE;
          break;
        case K_PGDN :
          if (!FOCUSSED)
            break;
          glb_send_message(pelementElement, RECIPIENT, M_SCROLL_PAGE_DOWN, pscroll->ucType);
          peventEvent->uiKind = E_DONE;
          break;
      }  
      break;
    case E_MSM_L_UP :                 
      if ((eventEvent.iHor >= pscroll->coord.iX) && (eventEvent.iHor <= SCROLL_X(pscroll->iLength)) &&
          (eventEvent.iVer >= pscroll->coord.iY) && (eventEvent.iVer <= SCROLL_Y(pscroll->iLength)))
        peventEvent->uiKind = E_DONE;
      if (pscroll->bDragging)
      {
        pscroll->bDragging = FALSE;
        glb_send_message(pelementElement, RECIPIENT, M_SCROLL_END_DRAGGING, pscroll->ucType);
      }     
      pscroll->bButtonDown = FALSE;
      break;                                
    case E_MSM_L_DOWN :
      if ((eventEvent.iHor >= pscroll->coord.iX) &&
          (eventEvent.iHor <= SCROLL_X(pscroll->iLength)) &&
          (eventEvent.iVer >= pscroll->coord.iY) &&
          (eventEvent.iVer <= SCROLL_Y(pscroll->iLength)))
      {                          
        pscroll->bButtonDown = TRUE;
        if (eventEvent.iHor == pscroll->coord.iX && eventEvent.iVer == pscroll->coord.iY)
        {
          glb_send_message(pelementElement, RECIPIENT, M_SCROLL_UP, pscroll->ucType);
          pscroll->dwNextReaction = peventEvent->dwTimer + 400;
          peventEvent->uiKind = E_DONE;
          break;
        }
        if (eventEvent.iHor == SCROLL_X(pscroll->iLength) && eventEvent.iVer == SCROLL_Y(pscroll->iLength))
        {
          glb_send_message(pelementElement, RECIPIENT, M_SCROLL_DOWN, pscroll->ucType);
          pscroll->dwNextReaction = peventEvent->dwTimer + 400;
          peventEvent->uiKind = E_DONE;
          break;
        }
        if (eventEvent.iHor == SCROLL_X(pscroll->iPos) && eventEvent.iVer == SCROLL_Y(pscroll->iPos))
        {          
          glb_send_message(pelementElement, RECIPIENT, M_SCROLL_BEGIN_DRAGGING, pscroll->ucType);
          pscroll->bDragging = TRUE;
          peventEvent->uiKind = E_DONE;
          break;
        }
        if (peventEvent->uiKind != E_DONE)
        {
          if (eventEvent.iHor < SCROLL_X(pscroll->iPos) || eventEvent.iVer < SCROLL_Y(pscroll->iPos))
            glb_send_message(pelementElement, RECIPIENT, M_SCROLL_PAGE_UP, pscroll->ucType);
          else
            glb_send_message(pelementElement, RECIPIENT, M_SCROLL_PAGE_DOWN, pscroll->ucType);
          pscroll->dwNextReaction = peventEvent->dwTimer + 400;
          peventEvent->uiKind = E_DONE;
        }
      }
      break;
    case E_NULL :
      if ((eventEvent.iHor >= pscroll->coord.iX) && (eventEvent.iHor <= SCROLL_X(pscroll->iLength)) &&
          (eventEvent.iVer >= pscroll->coord.iY) && (eventEvent.iVer <= SCROLL_Y(pscroll->iLength)) &&
          pscroll->bButtonDown && !pscroll->bDragging)
      {
        if (pscroll->dwNextReaction > peventEvent->dwTimer)
          break;
        if (eventEvent.iHor == pscroll->coord.iX && eventEvent.iVer == pscroll->coord.iY)
        {
          glb_send_message(pelementElement, RECIPIENT, M_SCROLL_UP, pscroll->ucType);
          pscroll->dwNextReaction = peventEvent->dwTimer + 50;
          peventEvent->uiKind = E_DONE;
          break;
        }
        if (eventEvent.iHor == SCROLL_X(pscroll->iLength) && eventEvent.iVer == SCROLL_Y(pscroll->iLength))
        {
          glb_send_message(pelementElement, RECIPIENT, M_SCROLL_DOWN, pscroll->ucType);
          pscroll->dwNextReaction = peventEvent->dwTimer + 50;
          peventEvent->uiKind = E_DONE;
          break;
        }
        if (eventEvent.iHor == SCROLL_X(pscroll->iPos) && eventEvent.iVer == SCROLL_Y(pscroll->iPos))
          peventEvent->uiKind = E_DONE;
        if (peventEvent->uiKind != E_DONE)
        {
          if (eventEvent.iHor < SCROLL_X(pscroll->iPos) || eventEvent.iVer < SCROLL_Y(pscroll->iPos))
            glb_send_message(pelementElement, RECIPIENT, M_SCROLL_PAGE_UP, pscroll->ucType);
          else
            glb_send_message(pelementElement, RECIPIENT, M_SCROLL_PAGE_DOWN, pscroll->ucType);
          pscroll->dwNextReaction = peventEvent->dwTimer + 50;
          peventEvent->uiKind = E_DONE;
        }
      }
      break;
    case E_MSM_MOVE :
      if ((eventEvent.iHor >= SCROLL_X(0)) && (eventEvent.iHor <= SCROLL_X(pscroll->iLength - 1)) &&
          (eventEvent.iVer >= SCROLL_Y(0)) && (eventEvent.iVer <= SCROLL_Y(pscroll->iLength - 1)) &&
          pscroll->bDragging)
      {                          
        int_dlg_scroll_set_pos(pelementElement,
                               max(eventEvent.iHor - pscroll->coord.iX,
                                   eventEvent.iVer - pscroll->coord.iY) - 1);
        glb_send_message(pelementElement, RECIPIENT, M_SCROLL_CHANGED, pscroll->ucType);
        peventEvent->uiKind = E_DONE;
      }
      break;
  }
  if (!(eventEvent.wButtons & MSM_B_LEFT))
  {
    pscroll->bDragging = FALSE;
    pscroll->bButtonDown = FALSE;
  }
}

DLG_ELEMENT* dlg_init_scroll(INT iX, INT iY, INT iLength, INT iPos, UCHAR ucType, CHAR *pcHelpLine, UINT uiID,
                             BOOL bCanBeActivated, void *pRecipient)
{
  DLG_ELEMENT *pelementReturnVar;
  DLG_SCROLL  *pscroll;

  if (!(pelementReturnVar = utl_alloc(sizeof(DLG_ELEMENT))))
    return NULL;
  pelementReturnVar->handle_event = dlg_scroll_handle_event;
  pelementReturnVar->pcHelpLine = pcHelpLine;
  pelementReturnVar->uiID = uiID;
  pelementReturnVar->flFlags.binCanBeActivated = bCanBeActivated;
  pelementReturnVar->flFlags.binFocussed = FALSE;
  pelementReturnVar->pRecipient = pRecipient;
  if (!(pelementReturnVar->pAddInfo = utl_alloc(sizeof(DLG_SCROLL))))
    return NULL;
  pscroll = (DLG_SCROLL*)pelementReturnVar->pAddInfo;
  pscroll->coord.iX = iX;
  pscroll->coord.iY = iY;
  pscroll->iLength = iLength;
  pscroll->iPos = iPos;          
  pscroll->ucType = ucType;
  pscroll->bButtonDown = pscroll->bDragging = FALSE;
  return pelementReturnVar;
}                                                               
