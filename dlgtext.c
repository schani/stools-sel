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
 *                           Text Fields                                   *
 ***************************************************************************/

void static int_dlg_draw_text_field (DLG_ELEMENT *pelementElement)
{
  INT             i;
  BOOL            bMouse,
                  bNull  = FALSE;
  DLG_TEXT_FIELD *ptext;
                  
  ptext = (DLG_TEXT_FIELD*)pelementElement->pAddInfo;
  bMouse = msm_cursor_off();
  win_s_za(OWNER, ptext->coord.iX, ptext->coord.iY, '[', win_get_color(OWNER, PAL_COL_BACKGROUND));
  win_s_za(OWNER, ptext->coord.iX + ptext->iDispLength + 1, ptext->coord.iY, ']',
     win_get_color(OWNER, PAL_COL_BACKGROUND));
  for (i = 0; i < ptext->iDispLength; i++)
  {
    if (!(ptext->pcInput[ptext->iFirstChar + i]))
      bNull = TRUE;
    if (!bNull)
    {
      if (ptext->bPassword)
        win_s_za(OWNER, ptext->coord.iX + i + 1, ptext->coord.iY, '*',
     win_get_color(OWNER, PAL_COL_BACKGROUND));
      else
        win_s_za(OWNER, ptext->coord.iX + i + 1, ptext->coord.iY, ptext->pcInput[ptext->iFirstChar + i],
                 win_get_color(OWNER, PAL_COL_BACKGROUND));
    }
    else
      win_s_za(OWNER, ptext->coord.iX + i + 1, ptext->coord.iY, (CHAR)250, win_get_color(OWNER, PAL_COL_BACKGROUND));
  }
  if (ptext->bMarked && ptext->iMarkLength)
    win_sw_a(OWNER, ptext->coord.iX + 1 + min(max(0, ptext->iMarkStart - ptext->iFirstChar),
                ptext->iDispLength - 1), ptext->coord.iY,
             min(max(ptext->iMarkStart + ptext->iMarkLength - ptext->iFirstChar, 0), ptext->iDispLength) -
             min(max(ptext->iMarkStart - ptext->iFirstChar, 0), ptext->iDispLength), 1,
             win_get_color(OWNER, PAL_COL_MARKED_TEXT));
  if (FOCUSSED)
    win_set_cursor_pos(OWNER, ptext->coord.iX + ptext->iPos - ptext->iFirstChar + 1, ptext->coord.iY);
  if (bMouse)
    msm_cursor_on();
}

void dlg_text_field_handle_event (DLG_ELEMENT *pelementElement, UTL_EVENT *peventEvent)
{
  DLG_TEXT_FIELD *ptext;
  UTL_EVENT       eventEvent;
  INT             iPos,
                  iOldLength;
  VIO_REGION      reg;

  ptext = (DLG_TEXT_FIELD*)pelementElement->pAddInfo;
  win_make_local(OWNER, peventEvent, &eventEvent);
  if (FOCUSSED && OWNER_FOCUSSED && pelementElement->pcHelpLine)
    sts_write_help(pelementElement->pcHelpLine);
  switch (peventEvent->uiKind)
  {        
    case E_MESSAGE :
      switch (eventEvent.uiMessage)
      {
        case M_INIT :
          int_dlg_draw_text_field(pelementElement);
          peventEvent->uiKind = E_DONE;
          break;
        case M_GET_FOCUS :
          ptext->bMarked = TRUE;
          ptext->iMarkStart = 0;
          ptext->iPos = ptext->iMarkLength = ptext->iLength;
          if (ptext->iLength >= ptext->iDispLength)
            ptext->iFirstChar = ptext->iLength - ptext->iDispLength + 1;
          else
            ptext->iFirstChar = 0;
          int_dlg_draw_text_field(pelementElement);
          peventEvent->uiKind = E_DONE;
          break;
        case M_LOST_FOCUS :
          ptext->bMarked = FALSE;
          int_dlg_draw_text_field(pelementElement);  
          peventEvent->uiKind = E_DONE;
          break;
        case M_QUIT :
          utl_free(pelementElement->pAddInfo);
          peventEvent->uiKind = E_DONE;
          return;
        case M_SET_VALUES :
          strcpy(ptext->pcInput, (CHAR*)peventEvent->ulAddInfo);
          ptext->iLength = strlen(ptext->pcInput);
          ptext->bMarked = FALSE;
          if (FOCUSSED)
          {
            ptext->iPos = ptext->iLength;
            if (ptext->iDispLength >= ptext->iLength)
              ptext->iFirstChar = max(ptext->iLength - ptext->iDispLength + 1, 0);
            else
              ptext->iFirstChar = 0;
          }
          else
            ptext->iPos = ptext->iFirstChar = 0;
          int_dlg_draw_text_field(pelementElement);
          peventEvent->uiKind = E_DONE;
          break;
        case M_QUERY_VALUES :           
          strcpy((CHAR*)peventEvent->ulAddInfo, ptext->pcInput);
          peventEvent->uiKind = E_DONE;
          break;                       
        case M_SET_DISPLAY :
    reg = *(VIO_REGION*)peventEvent->ulAddInfo;
    ptext->coord.iX = reg.iX;
    ptext->coord.iY = reg.iY;
    ptext->iDispLength = reg.iWidth;
          if (ptext->iPos >= ptext->iFirstChar + ptext->iDispLength)
            ptext->iFirstChar = ptext->iPos - ptext->iDispLength + 1;
          peventEvent->uiKind = E_DONE;
          break;
        case M_DRAW :
          int_dlg_draw_text_field(pelementElement);
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
          peventEvent->uiKind = E_DONE;
    if (ptext->iPos > 0)
      ptext->iPos--;
          ptext->bMarked = ptext->bDragging = FALSE;
          if (ptext->iPos < ptext->iFirstChar)
            ptext->iFirstChar--;
          int_dlg_draw_text_field(pelementElement);
          break;
  case K_C_LEFT :
          if (!FOCUSSED)
            break;          
          peventEvent->uiKind = E_DONE;
    ptext->bDragging = FALSE;
          if (!ptext->iPos)
            break;
          ptext->iPos--;
          if (ptext->bMarked)
          {
            if (ptext->iMarkStart == ptext->iPos + 1)
            {
              ptext->iMarkStart = ptext->iPos;
              ptext->iMarkLength++;
            }
            else
              ptext->iMarkLength--;
          }
          else
          {
            ptext->bMarked = TRUE;
            ptext->iMarkStart = ptext->iPos;
            ptext->iMarkLength = 1;
          }       
          if (!ptext->iMarkLength)
            ptext->bMarked = FALSE;
          if (ptext->iPos < ptext->iFirstChar)
            ptext->iFirstChar--;
          int_dlg_draw_text_field(pelementElement);
          break;
        case K_RIGHT :    
          if (!FOCUSSED)
            break;
          peventEvent->uiKind = E_DONE;
          ptext->bMarked = ptext->bDragging = FALSE;
          if (ptext->iPos < ptext->iLength)
            ptext->iPos++;
          if (ptext->iPos == ptext->iFirstChar + ptext->iDispLength)
            ptext->iFirstChar++;
          int_dlg_draw_text_field(pelementElement);
          break;
  case K_C_RIGHT :
          if (!FOCUSSED)
            break;
          peventEvent->uiKind = E_DONE;
    ptext->bDragging = FALSE;
          if (ptext->iPos >= ptext->iLength)
            break;
          ptext->iPos++;
          if (ptext->bMarked)
          {
            if (ptext->iMarkStart == ptext->iPos - 1)
            {                    
              ptext->iMarkStart = ptext->iPos;
              ptext->iMarkLength--;
            }
            else
              ptext->iMarkLength++;
          }
          else
          {
            ptext->bMarked = TRUE;
            ptext->iMarkStart = ptext->iPos - 1;
            ptext->iMarkLength = 1;
          }
          if (!ptext->iMarkLength)
            ptext->bMarked = FALSE;
          if (ptext->iPos == ptext->iFirstChar + ptext->iDispLength)
            ptext->iFirstChar++;
          int_dlg_draw_text_field(pelementElement);
          break;
        case K_BACKSPACE :
          if (!FOCUSSED)
            break;             
          peventEvent->uiKind = E_DONE;
          if (ptext->iPos)
          {
            if (ptext->bMarked)
              ptext->bMarked = FALSE;
            if (--ptext->iPos < ptext->iFirstChar)
              ptext->iFirstChar--;
            utl_strdel(ptext->pcInput, ptext->iPos, 1);
            ptext->iLength--;
            int_dlg_draw_text_field(pelementElement);
            glb_send_message(pelementElement, RECIPIENT, M_TEXT_FIELD_CHANGED, ptext->pcInput);
          }
          break;
        case K_DEL :                                    
          if (!FOCUSSED)
            break;            
          peventEvent->uiKind = E_DONE;
          if (ptext->bMarked)
          {
            ptext->bMarked = FALSE;
            utl_strdel(ptext->pcInput, ptext->iMarkStart, ptext->iMarkLength);
            ptext->iPos = ptext->iMarkStart;
            if (ptext->iMarkStart < ptext->iFirstChar)
              ptext->iFirstChar = ptext->iPos;
            ptext->iLength -= ptext->iMarkLength;
            int_dlg_draw_text_field(pelementElement);
            glb_send_message(pelementElement, RECIPIENT, M_TEXT_FIELD_CHANGED, ptext->pcInput);
          }
          else
          {
            if (ptext->iPos < ptext->iLength)
            {
              utl_strdel(ptext->pcInput, ptext->iPos, 1);
              ptext->iLength--;
              int_dlg_draw_text_field(pelementElement);
              glb_send_message(pelementElement, RECIPIENT, M_TEXT_FIELD_CHANGED, ptext->pcInput);
            }
          }
          break;
        case K_HOME :
          if (!FOCUSSED)
            break;
          peventEvent->uiKind = E_DONE;
          if (ptext->iPos)
          {             
            ptext->bMarked = FALSE;
            ptext->iFirstChar = 0;
            ptext->iPos = 0;
            int_dlg_draw_text_field(pelementElement);
          }                             
          else
            if (ptext->bMarked)
            {
              ptext->bMarked = FALSE;
              int_dlg_draw_text_field(pelementElement);
            }
          break;
        case K_END :
          if (!FOCUSSED)
            break;
          peventEvent->uiKind = E_DONE;
          if (ptext->iPos != ptext->iLength)
          {                  
            ptext->bMarked = FALSE;
            ptext->iPos = ptext->iLength;
            if (ptext->iLength >= ptext->iDispLength)
              ptext->iFirstChar = ptext->iLength - ptext->iDispLength + 1;
            else
              ptext->iFirstChar = 0;
            int_dlg_draw_text_field(pelementElement);
          }
          else
            if (ptext->bMarked)
            {
              ptext->bMarked = FALSE;
              int_dlg_draw_text_field(pelementElement);
            }
          break;
        default :
          if (!FOCUSSED)
            break;
          if (eventEvent.wKey <= 0xff && eventEvent.wKey >= 0x20)
          {   
            peventEvent->uiKind = E_DONE;
            if (ptext->iLength == ptext->iRealLength)
              break;
            if (ptext->bMarked)
            {
              ptext->bMarked = FALSE;
              utl_strdel(ptext->pcInput, ptext->iMarkStart, ptext->iMarkLength);
              utl_strins(ptext->pcInput, ptext->iMarkStart, (CHAR)eventEvent.wKey);
              ptext->iLength -= ptext->iMarkLength - 1;
              if (ptext->iMarkStart < ptext->iFirstChar)
              {
                ptext->iFirstChar = ptext->iMarkStart;
                if (ptext->iDispLength > 1)
                  ptext->iPos = ptext->iFirstChar + 1;
                else
                  ptext->iPos = ptext->iFirstChar;
              }
              else                                       
              {
                ptext->iPos = ptext->iMarkStart + 1;
                if (ptext->iPos - ptext->iFirstChar == ptext->iDispLength)
                  ptext->iFirstChar++;
              }
            }
            else
            {
              if (INSERT_ON)
              {
                utl_strins(ptext->pcInput, ptext->iPos, (CHAR)eventEvent.wKey);
                ptext->iPos++;
                if (ptext->iPos - ptext->iFirstChar == ptext->iDispLength)
                  ptext->iFirstChar++;
                ptext->iLength++;
              }
              else
              {
                if (ptext->iPos == ptext->iLength)
                {
                  utl_strins(ptext->pcInput, ptext->iPos, (CHAR)eventEvent.wKey);
                  ptext->iLength++;
                }
                else
                  ptext->pcInput[ptext->iPos] = (CHAR)eventEvent.wKey;
                ptext->iPos++;
                if (ptext->iPos - ptext->iFirstChar == ptext->iDispLength)
                  ptext->iFirstChar++;
              }
            }
            int_dlg_draw_text_field(pelementElement);
            glb_send_message(pelementElement, RECIPIENT, M_TEXT_FIELD_CHANGED, ptext->pcInput);
          }
          break;
      }
      break;
    case E_MSM_L_DOWN :
      eventEvent.iHor -= ptext->coord.iX;
      eventEvent.iVer -= ptext->coord.iY;
      if (eventEvent.iHor >= 0 && eventEvent.iHor <= ptext->iDispLength + 1 && eventEvent.iVer == 0)
      {
        peventEvent->uiKind = E_DONE;
        ptext->bMarked = FALSE;
  ptext->bDragging = TRUE;
        if (eventEvent.iHor >= 1 && eventEvent.iHor < ptext->iDispLength + 1)
          ptext->iPos = min(ptext->iFirstChar + eventEvent.iHor - 1, ptext->iLength);
        int_dlg_draw_text_field(pelementElement);
      }         
      else
        if (ptext->bMarked)
        {
          ptext->bMarked = FALSE;
    ptext->bDragging = FALSE;
          int_dlg_draw_text_field(pelementElement);
        }             
      break;
    case E_MSM_L_UP :
      eventEvent.iHor -= ptext->coord.iX;
      eventEvent.iVer -= ptext->coord.iY;
      ptext->bDragging = FALSE;
      if (eventEvent.iHor >= 0 && eventEvent.iHor <= ptext->iDispLength + 1 && eventEvent.iVer == 0)
        peventEvent->uiKind = E_DONE;
      break;
    case E_MSM_MOVE :
      if (!FOCUSSED)
        break;
      eventEvent.iHor -= ptext->coord.iX;
      eventEvent.iVer -= ptext->coord.iY;
      if (!(eventEvent.wButtons & MSM_B_LEFT))
  ptext->bDragging = FALSE;
      if (eventEvent.iHor >= 0 && eventEvent.iHor <= ptext->iDispLength + 1 && eventEvent.iVer == 0)
        if (eventEvent.iHor >= 1 && eventEvent.iHor < ptext->iDispLength + 1)
        {    
          iPos = min(ptext->iLength, ptext->iFirstChar + eventEvent.iHor - 1);
          if (ptext->bDragging)
          {
            peventEvent->uiKind = E_DONE;
            if (!ptext->bMarked)
            {
              ptext->bMarked = TRUE;
              if (ptext->iPos < iPos)
              {
                ptext->iMarkStart = ptext->iPos;
                ptext->iMarkLength = iPos - ptext->iPos;
              }
              else
              {
                ptext->iMarkStart = iPos;
                ptext->iMarkLength = ptext->iPos - iPos;
              }
            }
            else
            {
              if (iPos < ptext->iMarkStart)
              {
                if (ptext->iPos == ptext->iMarkStart)
                  ptext->iMarkLength += ptext->iMarkStart - iPos;
                else
                  ptext->iMarkLength = ptext->iMarkStart - iPos;
                ptext->iMarkStart = iPos;
              }
              else
              {
                if (ptext->iPos == ptext->iMarkStart)
                {
                  if (iPos <= ptext->iMarkStart + ptext->iMarkLength)
                  {
                    ptext->iMarkLength -= iPos - ptext->iMarkStart;
                    ptext->iMarkStart = iPos;
                  }
                  else
                  {           
                    iOldLength = ptext->iMarkLength;
                    ptext->iMarkLength = iPos - ptext->iMarkStart - ptext->iMarkLength;
                    ptext->iMarkStart = ptext->iMarkStart + iOldLength;
                  }
                }
                else
                  ptext->iMarkLength = iPos - ptext->iMarkStart;
              }
            }
            ptext->iPos = iPos;
            if (!ptext->iMarkLength)
              ptext->bMarked = FALSE;
            int_dlg_draw_text_field(pelementElement);
          }
        }
      break;
    case E_NULL :
      if (!FOCUSSED)
        break;
      eventEvent.iHor -= ptext->coord.iX;
      eventEvent.iVer -= ptext->coord.iY;
      if (!(eventEvent.wButtons & MSM_B_LEFT))
  ptext->bDragging = FALSE;
      if (!ptext->bDragging)
        break;
      if (eventEvent.iHor <= 0)
      {       
        peventEvent->uiKind = E_DONE;
        if (ptext->iPos <= 0)
          break;
        ptext->iPos--;
        if (ptext->bMarked)
        {
          if (ptext->iMarkStart == ptext->iPos + 1)
          {                    
            ptext->iMarkStart = ptext->iPos;
            ptext->iMarkLength++;
          }
          else
            ptext->iMarkLength--;
        }
        else
        {
          ptext->bMarked = TRUE;
          ptext->iMarkStart = ptext->iPos;
          ptext->iMarkLength = 1;
        }       
        if (ptext->iPos < ptext->iFirstChar)
          ptext->iFirstChar--;
        if (!ptext->iMarkLength)
          ptext->bMarked = FALSE;           
        int_dlg_draw_text_field(pelementElement);
        utl_delay(50);
        break;
      }
      if (eventEvent.iHor > ptext->iDispLength)
      {
        peventEvent->uiKind = E_DONE;
        if (ptext->iPos >= ptext->iLength)
          break;
        ptext->iPos++;
        if (ptext->bMarked)
        {
          if (ptext->iMarkStart == ptext->iPos - 1)
          {                    
            ptext->iMarkStart = ptext->iPos;
            ptext->iMarkLength--;
          }
          else
            ptext->iMarkLength++;
        }
        else
        {
          ptext->bMarked = TRUE;
          ptext->iMarkStart = ptext->iPos - 1;
          ptext->iMarkLength = 1;
        }
        if (ptext->iPos == ptext->iFirstChar + ptext->iDispLength)
          ptext->iFirstChar++;
        if (!ptext->iMarkLength)
          ptext->bMarked = FALSE;
        int_dlg_draw_text_field(pelementElement);
        utl_delay(50);
      }
      break;
  }
}

DLG_ELEMENT* dlg_init_text_field(INT iX, INT iY, INT iDispLength, INT iRealLength, BOOL bPassword,
         CHAR *pcInput, CHAR *pcHelpLine, UINT uiID, BOOL bCanBeActivated,
         void *pRecipient)
{
  DLG_ELEMENT    *pelementReturnVar;
  DLG_TEXT_FIELD *ptext;

  if (!(pelementReturnVar = utl_alloc(sizeof(DLG_ELEMENT))))
    return NULL;
  pelementReturnVar->handle_event = dlg_text_field_handle_event;
  pelementReturnVar->pcHelpLine = pcHelpLine;        
  pelementReturnVar->uiID = uiID;
  pelementReturnVar->flFlags.binCanBeActivated = bCanBeActivated;
  pelementReturnVar->flFlags.binFocussed = FALSE;
  pelementReturnVar->pRecipient = pRecipient;
  if (!(pelementReturnVar->pAddInfo = utl_alloc(sizeof(DLG_TEXT_FIELD))))
    return NULL;
  ptext = (DLG_TEXT_FIELD*)pelementReturnVar->pAddInfo;
  ptext->coord.iX = iX;
  ptext->coord.iY = iY;
  ptext->iDispLength = iDispLength;
  ptext->iRealLength = iRealLength;
  ptext->iLength = strlen(pcInput);
  ptext->pcInput = pcInput;
  ptext->iFirstChar = 0;
  ptext->iPos = 0;
  ptext->bMarked = FALSE;
  ptext->bDragging = FALSE;
  ptext->bPassword = bPassword;
  return pelementReturnVar;
}
