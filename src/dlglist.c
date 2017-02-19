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
 *                            List Box                                     *
 ***************************************************************************/
                        
void static int_dlg_list_box_draw_line (DLG_ELEMENT *pelementElement, INT iLine)
{
  DLG_LIST_BOX  *plist;
  CHAR         **ppcString;
  INT            i;
  BOOL           bWrite    = TRUE;

  plist = (DLG_LIST_BOX*)pelementElement->pAddInfo;
  ppcString = plist->ppcFirstLine + plist->iFirstLine;
  for (i = 0; i < iLine; i++)
  {
    if (!*ppcString)
    {
      bWrite = FALSE;
      break;
    }
    ppcString++;
  }
  if (!*ppcString)
    bWrite = FALSE;           
  win_sw_za(OWNER, plist->reg.iX + 1, plist->reg.iY + iLine + 1, plist->reg.iWidth - 2, 1, ' ',
            win_get_color(OWNER, PAL_COL_LIST_BOX_ITEM));
  if (bWrite)
    win_ssn(OWNER, plist->reg.iX + 2, plist->reg.iY + iLine + 1, *ppcString, plist->reg.iWidth - 3);
}

void static int_dlg_draw_list_box (DLG_ELEMENT *pelementElement)
{
  DLG_LIST_BOX  *plist;
  INT            i;

  plist = (DLG_LIST_BOX*)pelementElement->pAddInfo;
  for (i = 0; i < plist->reg.iHeight - 2; i++)
    int_dlg_list_box_draw_line(pelementElement, i);
  DRAW_MARKER;
}

BOOL static int_dlg_list_box_up (DLG_ELEMENT *pelementElement)
{
  DLG_LIST_BOX *plist;
  
  plist = (DLG_LIST_BOX*)pelementElement->pAddInfo;
  if (plist->iPos <= 0)
    return FALSE;
  CLEAR_MARKER;
  if (plist->iPos == plist->iFirstLine)
  {            
    win_down(OWNER, plist->reg.iX + 1, plist->reg.iY + 1, plist->reg.iWidth - 2, plist->reg.iHeight - 2, 1);
    plist->iPos = --plist->iFirstLine;
    int_dlg_list_box_draw_line(pelementElement, 0);
  }
  else
    plist->iPos--;
  DRAW_MARKER; 
  if (FOCUSSED)
    SET_CURSOR;
  SET_SCROLL;
  return TRUE;
}         

BOOL static int_dlg_list_box_down (DLG_ELEMENT *pelementElement)
{
  DLG_LIST_BOX *plist;
  
  plist = (DLG_LIST_BOX*)pelementElement->pAddInfo;
  if (plist->iPos >= plist->iElements - 1)
    return FALSE;
  CLEAR_MARKER;
  if (plist->iPos == plist->iFirstLine + plist->reg.iHeight - 3)
  {
    win_up(OWNER, plist->reg.iX + 1, plist->reg.iY + 1,
           plist->reg.iWidth - 2, plist->reg.iHeight - 2, 1);
    plist->iPos++;
    plist->iFirstLine++;
    int_dlg_list_box_draw_line(pelementElement, plist->reg.iHeight - 3);
  }
  else
    plist->iPos++;
  DRAW_MARKER;      
  if (FOCUSSED)
    SET_CURSOR;     
  SET_SCROLL;                     
  return TRUE;
}                                

BOOL static int_dlg_list_box_page_up (DLG_ELEMENT *pelementElement)
{
  DLG_LIST_BOX *plist;
  
  plist = (DLG_LIST_BOX*)pelementElement->pAddInfo;
  if (plist->iPos <= 0)
    return FALSE;
  CLEAR_MARKER;
  if (plist->iFirstLine <= 0)
    plist->iPos = 0;
  else
  {  
    plist->iPos -= min(plist->iFirstLine, plist->reg.iHeight - 2);
    plist->iFirstLine -= min(plist->iFirstLine, plist->reg.iHeight - 2);
    int_dlg_draw_list_box(pelementElement);
  }
  DRAW_MARKER; 
  if (FOCUSSED)
    SET_CURSOR;
  SET_SCROLL;
  return(TRUE);
}         

BOOL static int_dlg_list_box_page_down (DLG_ELEMENT *pelementElement)
{
  DLG_LIST_BOX *plist;
  
  plist = (DLG_LIST_BOX*)pelementElement->pAddInfo;
  if (plist->iPos == plist->iElements - 1)
    return FALSE;
  CLEAR_MARKER;
  if (plist->iFirstLine + plist->reg.iHeight - 2 >= plist->iElements)
    plist->iPos = plist->iElements - 1;
  else
  {  
    plist->iFirstLine += plist->reg.iHeight - 2;
    plist->iPos = min(plist->iPos + plist->reg.iHeight - 2, plist->iElements - 1);
    int_dlg_draw_list_box(pelementElement);
  }
  DRAW_MARKER; 
  if (FOCUSSED)
    SET_CURSOR;
  SET_SCROLL;
  return(TRUE);
} 

BOOL static int_dlg_list_box_set_pos (DLG_ELEMENT *pelementElement, INT iPos)
{
  DLG_LIST_BOX *plist;
  
  plist = (DLG_LIST_BOX*)pelementElement->pAddInfo;
  if (iPos < 0 || iPos >= plist->iElements)
    return FALSE;
  if (iPos == plist->iPos)
    return TRUE;
  CLEAR_MARKER;
  if (iPos >= plist->iFirstLine && iPos <= plist->iFirstLine + plist->reg.iHeight - 3)
    plist->iPos = iPos;
  else
  {  
    plist->iPos = plist->iFirstLine = iPos;
    int_dlg_draw_list_box(pelementElement);
  }
  DRAW_MARKER; 
  if (FOCUSSED)
    SET_CURSOR;
  SET_SCROLL;
  return(TRUE);
} 

void dlg_list_box_handle_event (DLG_ELEMENT *pelementElement, UTL_EVENT *peventEvent)
{
  DLG_LIST_BOX  *plist;
  UTL_EVENT      eventEvent;
  INT            iScrollPos;
  CHAR         **ppcCounter;

  plist = (DLG_LIST_BOX*)pelementElement->pAddInfo;
  win_make_local(OWNER, peventEvent, &eventEvent);
  if (FOCUSSED && OWNER_FOCUSSED && pelementElement->pcHelpLine)
    sts_write_help(pelementElement->pcHelpLine);
  switch (eventEvent.uiKind)
  {
    case E_MESSAGE :
      switch (eventEvent.uiMessage)
      {
        case M_INIT :     
          if (plist->pelementBorder)
            win_add_element(OWNER, plist->pelementBorder);
          win_add_element(OWNER, plist->pelementScroll);
          int_dlg_draw_list_box(pelementElement);
          peventEvent->uiKind = E_DONE;
          break;
        case M_GET_FOCUS :
          SET_CURSOR;
        case M_LOST_FOCUS :
          if (eventEvent.uiKind == M_LOST_FOCUS)
            plist->bDragging = FALSE;
          peventEvent->uiKind = E_DONE;
          break;        
        case M_QUIT :
          utl_free(pelementElement->pAddInfo);
          peventEvent->uiKind = E_DONE;
          return;
        case M_SET_VALUES :
          int_dlg_list_box_set_pos(pelementElement, (INT)peventEvent->ulAddInfo);
          peventEvent->uiKind = E_DONE;
          break;                       
        case M_QUERY_VALUES :
          *((INT*)(peventEvent->ulAddInfo)) = plist->iPos;
          peventEvent->uiKind = E_DONE;
          break;
        case M_DRAW :
          int_dlg_draw_list_box(pelementElement);
          peventEvent->uiKind = E_DONE;
          break;
        case M_SET_DISPLAY :
    plist->reg = *(VIO_REGION*)peventEvent->ulAddInfo;
          if (plist->iPos >= plist->iFirstLine + plist->reg.iHeight - 2)
            plist->iFirstLine = plist->iPos - plist->reg.iHeight - 3; 
          if (plist->pelementBorder)
            dlg_border_set_display(plist->pelementBorder, plist->reg.iX, plist->reg.iY, plist->reg.iWidth,
                                   plist->reg.iHeight);
          dlg_scroll_set_display(plist->pelementScroll, plist->reg.iX + plist->reg.iWidth - 1,
         plist->reg.iY + 1, plist->reg.iHeight - 4);
          SET_SCROLL;
          peventEvent->uiKind = E_DONE;
          break;                  
        case M_LIST_BOX_NEW_LIST :
          plist->ppcFirstLine = (CHAR**)peventEvent->ulAddInfo;
          plist->iPos = plist->iFirstLine = 0;
          int_dlg_draw_list_box(pelementElement);
          for (ppcCounter = plist->ppcFirstLine, plist->iElements = 0; *ppcCounter;
         plist->iElements++, ppcCounter++)
            ;
          plist->iC = max(plist->iElements / (plist->reg.iHeight - 4), 1);
          SET_SCROLL;
          if (FOCUSSED)
            SET_CURSOR;
          peventEvent->uiKind = E_DONE;
          break;
        case M_SCROLL_UP :
          if (int_dlg_list_box_up(pelementElement))
            glb_send_message(pelementElement, RECIPIENT, M_LIST_BOX_CHANGED, plist->iPos);
          if (!FOCUSSED)
            dlg_set_focus(OWNER, pelementElement);
          peventEvent->uiKind = E_DONE;
          break;
        case M_SCROLL_DOWN :
          if (int_dlg_list_box_down(pelementElement))
            glb_send_message(pelementElement, RECIPIENT, M_LIST_BOX_CHANGED, plist->iPos);
          if (!FOCUSSED)
            dlg_set_focus(OWNER, pelementElement);
          peventEvent->uiKind = E_DONE;
          break;
        case M_SCROLL_PAGE_UP :
          if (int_dlg_list_box_page_up(pelementElement))
            glb_send_message(pelementElement, RECIPIENT, M_LIST_BOX_CHANGED, plist->iPos);
          if (!FOCUSSED)
            dlg_set_focus(OWNER, pelementElement);
          peventEvent->uiKind = E_DONE;
          break;       
        case M_SCROLL_PAGE_DOWN :
          if (int_dlg_list_box_page_down(pelementElement))
            glb_send_message(pelementElement, RECIPIENT, M_LIST_BOX_CHANGED, plist->iPos);
          if (!FOCUSSED)
            dlg_set_focus(OWNER, pelementElement);
          peventEvent->uiKind = E_DONE;
          break;                  
        case M_SCROLL_CHANGED :
          dlg_scroll_query_values(plist->pelementScroll, &iScrollPos);
          if (int_dlg_list_box_set_pos(pelementElement, iScrollPos * plist->iC))
            glb_send_message(pelementElement, RECIPIENT, M_LIST_BOX_CHANGED, plist->iPos);
        case M_SCROLL_BEGIN_DRAGGING :
          if (!FOCUSSED)
            dlg_set_focus(OWNER, pelementElement);
          peventEvent->uiKind = E_DONE;
          break;                  
      }
      break;
    case E_KEY :
      switch (eventEvent.wKey)
      {
        case K_UP :
          if (!FOCUSSED)
            break;
          if (int_dlg_list_box_up(pelementElement))
            glb_send_message(pelementElement, RECIPIENT, M_LIST_BOX_CHANGED, plist->iPos);
          peventEvent->uiKind = E_DONE;
          break;       
        case K_DOWN :
          if (!FOCUSSED)
            break;
          if (int_dlg_list_box_down(pelementElement))
            glb_send_message(pelementElement, RECIPIENT, M_LIST_BOX_CHANGED, plist->iPos);
          peventEvent->uiKind = E_DONE;
          break;
        case K_PGUP :
          if (!FOCUSSED)
            break;
          if (int_dlg_list_box_page_up(pelementElement))
            glb_send_message(pelementElement, RECIPIENT, M_LIST_BOX_CHANGED, plist->iPos);
          peventEvent->uiKind = E_DONE;
          break;
        case K_PGDN :
          if (!FOCUSSED)
            break;
          if (int_dlg_list_box_page_down(pelementElement))
            glb_send_message(pelementElement, RECIPIENT, M_LIST_BOX_CHANGED, plist->iPos);
          peventEvent->uiKind = E_DONE;
          break;
        case K_HOME :
          if (!FOCUSSED)
            break;
          if (int_dlg_list_box_set_pos(pelementElement, 0))
            glb_send_message(pelementElement, RECIPIENT, M_LIST_BOX_CHANGED, plist->iPos);
          peventEvent->uiKind = E_DONE;
          break;
        case K_END :        
          if (!FOCUSSED)
            break;
          if (int_dlg_list_box_set_pos(pelementElement, plist->iElements - 1))
            glb_send_message(pelementElement, RECIPIENT, M_LIST_BOX_CHANGED, plist->iPos);
          peventEvent->uiKind = E_DONE;
          break;
        case K_SPACE :
          if (!FOCUSSED)
            break;
          glb_send_message(pelementElement, RECIPIENT, M_LIST_BOX_CLICKED, plist->iPos);
          peventEvent->uiKind = E_DONE;
          break;
      }         
      break;
    case E_MSM_L_DOWN :
      eventEvent.iHor -= plist->reg.iX + 1;
      eventEvent.iVer -= plist->reg.iY + 1;
      if (eventEvent.iHor < 0 || eventEvent.iHor >= plist->reg.iWidth - 2 ||
          eventEvent.iVer < 0 || eventEvent.iVer >= plist->reg.iHeight - 2)
        break;
      if (int_dlg_list_box_set_pos(pelementElement, plist->iFirstLine + eventEvent.iVer))
      {
        plist->bDragging = TRUE;
        glb_send_message(pelementElement, RECIPIENT, M_LIST_BOX_CHANGED, plist->iPos);
      }
      peventEvent->uiKind = E_DONE;
      break;
    case E_MSM_MOVE :
      eventEvent.iHor -= plist->reg.iX + 1;
      eventEvent.iVer -= plist->reg.iY + 1;
      if (eventEvent.iHor < 0 || eventEvent.iHor >= plist->reg.iWidth - 2 ||
          eventEvent.iVer < 0 || eventEvent.iVer >= plist->reg.iHeight - 2)
        break;
      if (FOCUSSED)
        peventEvent->uiKind = E_DONE;
      if (!plist->bDragging)
        break;
      if (int_dlg_list_box_set_pos(pelementElement, plist->iFirstLine + eventEvent.iVer))
        glb_send_message(pelementElement, RECIPIENT, M_LIST_BOX_CHANGED, plist->iPos);
      break;
    case E_MSM_L_DOUBLE :
      plist->bDragging = FALSE;
      eventEvent.iHor -= plist->reg.iX + 1;
      eventEvent.iVer -= plist->reg.iY + 1;
      if (eventEvent.iHor < 0 || eventEvent.iHor >= plist->reg.iWidth - 2 ||
          eventEvent.iVer < 0 || eventEvent.iVer >= plist->reg.iHeight - 2)
        break;
      glb_send_message(pelementElement, RECIPIENT, M_LIST_BOX_CLICKED, plist->iPos);
      peventEvent->uiKind = E_DONE;
      break;
    case E_MSM_L_UP :
      plist->bDragging = FALSE;
      peventEvent->uiKind = E_DONE;
      break;
    case E_NULL :
      if (!FOCUSSED || !plist->bDragging)
        break;
      eventEvent.iHor -= plist->reg.iX + 1;
      eventEvent.iVer -= plist->reg.iY + 1;
      if (eventEvent.iVer < 0)
      {
        if (int_dlg_list_box_up(pelementElement))
          glb_send_message(pelementElement, RECIPIENT, M_LIST_BOX_CHANGED, plist->iPos);
        peventEvent->uiKind = E_DONE;
        utl_delay(50);
      }
      if (eventEvent.iVer >= plist->reg.iHeight - 2)
      {
        if (int_dlg_list_box_down(pelementElement))
          glb_send_message(pelementElement, RECIPIENT, M_LIST_BOX_CHANGED, plist->iPos);
        peventEvent->uiKind = E_DONE;
        utl_delay(50);
      }
      break;
  }
  if (!(eventEvent.wButtons & MSM_B_LEFT))
    plist->bDragging = FALSE;
}

DLG_ELEMENT* dlg_init_list_box(INT iX, INT iY, INT iWidth, INT iHeight, CHAR **ppcFirstLine, BOOL bBorder,
             CHAR *pcHelpLine, UINT uiID, BOOL bCanBeActivated, void *pRecipient)
{
  DLG_ELEMENT  *pelementReturnVar;
  DLG_LIST_BOX *plist;

  if (ppcFirstLine && *ppcFirstLine)
  {
    if (!(pelementReturnVar = utl_alloc(sizeof(DLG_ELEMENT))))
      return NULL;
    pelementReturnVar->handle_event = dlg_list_box_handle_event;
    pelementReturnVar->pcHelpLine = pcHelpLine;
    pelementReturnVar->uiID = uiID;
    pelementReturnVar->flFlags.binCanBeActivated = bCanBeActivated;
    pelementReturnVar->flFlags.binFocussed = FALSE;
    pelementReturnVar->pRecipient = pRecipient;
    if (!(pelementReturnVar->pAddInfo = utl_alloc(sizeof(DLG_LIST_BOX))))
      return NULL;
    plist = (DLG_LIST_BOX*)pelementReturnVar->pAddInfo;
    plist->reg.iX = iX;
    plist->reg.iY = iY;
    plist->reg.iWidth = iWidth;
    plist->reg.iHeight = iHeight;
    plist->iFirstLine = 0;
    plist->iPos = 0;
    plist->bDragging = FALSE;  
    plist->ppcFirstLine = ppcFirstLine;
    for (plist->iElements = 0; *ppcFirstLine; plist->iElements++, ppcFirstLine++)
      ;                                
    plist->iC = max(plist->iElements / (iHeight - 4), 1);
    plist->pelementScroll = dlg_init_scroll(iX + iWidth - 1, iY + 1, iHeight - 4, 0, DLG_VERTICAL, NULL,
              uiID + 10000, FALSE, pelementReturnVar);
    if (bBorder)
      plist->pelementBorder = dlg_init_border(iX, iY, iWidth, iHeight, uiID + 20000);
    else
      plist->pelementBorder = NULL;
    return pelementReturnVar;
  }
  return NULL;
}
