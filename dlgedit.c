/***************************************************************************
 ***************************************************************************
 ***                                                                     ***
 ***                       Schani-Tools fÅr C                            ***
 ***                                                                     ***
 ***                             Editor                                  ***
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
#include <boxes.h>
#include <stdlib.h>
#include <string.h>

extern GLB_PROGRAM prgProgram;

static CHAR        acLineBuffer[MAX_LINE_LENGTH + 1];
static INT         iBufferLength;

static INT int_dlg_editor_count_lines (DLG_ELEMENT *pelementElement)
{
  INT         iReturnVar = 1;
  CHAR       *pcCounter;                                      
  DLG_EDITOR *pedit;
              
  pedit = (DLG_EDITOR*)pelementElement->pAddInfo;
  for (pcCounter = pedit->pcBuffer; *pcCounter; pcCounter++)
    if (*pcCounter == '\n')
      iReturnVar++;
  return iReturnVar;
}

static CHAR* int_dlg_editor_get_line (DLG_ELEMENT *pelementElement, INT iLine)
{
  CHAR       *pcCounter;
  DLG_EDITOR *pedit;
              
  pedit = (DLG_EDITOR*)pelementElement->pAddInfo;
  for (pcCounter = pedit->pcFirstLine; iLine > 0; iLine--)
    pcCounter = int_dlg_editor_next_line(pelementElement, pcCounter);
  return pcCounter;
}

static void int_dlg_editor_make_line_buffer (DLG_ELEMENT *pelementElement, CHAR *pcLine)
{
  CHAR       *pcCounter;
  DLG_EDITOR *pedit;
              
  pedit = (DLG_EDITOR*)pelementElement->pAddInfo;
  memset(acLineBuffer, 0, MAX_LINE_LENGTH + 1);
  iBufferLength = 0;
  pcCounter = acLineBuffer;
  for (;;)
  {
    if (pcLine == pedit->pcCursor)
      pcLine += pedit->ulGapLength;
    if (!(*pcLine && *pcLine != '\n' && pcLine < pedit->pcBuffer + pedit->ulBufferLength))
      break;
    *pcCounter++ = *pcLine++;
    iBufferLength++;
  }
  *pcCounter = 0;
}

static CHAR* int_dlg_editor_next_line (DLG_ELEMENT *pelementElement, CHAR *pcLine)
{
  DLG_EDITOR *pedit;
              
  pedit = (DLG_EDITOR*)pelementElement->pAddInfo;
  for (; *pcLine != '\n' && pcLine < pedit->pcBuffer + pedit->ulBufferLength; )
    if (pcLine == pedit->pcCursor)
      pcLine += pedit->ulGapLength;
    else
      pcLine++;
  if (pcLine == pedit->pcBuffer + pedit->ulBufferLength)
    return pcLine;
  if (pcLine == pedit->pcCursor)
    pcLine += pedit->ulGapLength;
  else
    pcLine++;
  return pcLine;
}

static CHAR* int_dlg_editor_prev_line (DLG_ELEMENT *pelementElement, CHAR *pcLine)
{
  DLG_EDITOR *pedit;
              
  pedit = (DLG_EDITOR*)pelementElement->pAddInfo;
  if (*pcLine == '\n')
  {
    if (pcLine == pedit->pcCursor + pedit->ulGapLength)
      pcLine = pedit->pcCursor;
    else
      pcLine--;
  }
  for (; *pcLine != '\n' && pcLine != pedit->pcBuffer; )
    if (pcLine == pedit->pcCursor + pedit->ulGapLength)
      pcLine = pedit->pcCursor;
    else
      pcLine--;
  if (pcLine == pedit->pcCursor + pedit->ulGapLength)
    pcLine = pedit->pcCursor;
  else
    pcLine--;
  for (; *pcLine != '\n' && pcLine != pedit->pcBuffer; )
  {
    if (pcLine == pedit->pcCursor + pedit->ulGapLength)
      pcLine = pedit->pcCursor;
    else
      pcLine--;
  }
  if (pcLine != pedit->pcBuffer)
  {
    if (pcLine == pedit->pcCursor)
      pcLine += pedit->ulGapLength;
    else
      pcLine++;
  }
  return pcLine;
}

static void int_dlg_editor_draw (DLG_ELEMENT *pelementElement, INT iFirst, INT iLines)
{
  INT         iCounter;
  CHAR       *pcLine,
             *pcBuffer;
  DLG_EDITOR *pedit;
              
  pedit = (DLG_EDITOR*)pelementElement->pAddInfo;
  pcLine = int_dlg_editor_get_line(pelementElement, iFirst);
  for (iCounter = 0; iCounter < iLines; iCounter++)
  {
    int_dlg_editor_make_line_buffer(pelementElement, pcLine);
    pcBuffer = acLineBuffer + pedit->iLeftChar;
    if (iBufferLength >= pedit->iLeftChar)
      iBufferLength -= pedit->iLeftChar;
    else
      iBufferLength = 0;
    iBufferLength = min(iBufferLength, pedit->reg.iWidth - 2);
    pcBuffer[iBufferLength] = 0;
    win_ss(OWNER, pedit->reg.iX + 1, pedit->reg.iY + iCounter + iFirst + 1, pcBuffer);
    win_sw_z(OWNER, pedit->reg.iX + iBufferLength + 1, pedit->reg.iY + iCounter + iFirst + 1,
             pedit->reg.iWidth - iBufferLength - 2, 1, ' ');
    pcLine = int_dlg_editor_next_line(pelementElement, pcLine);
  }
}

static void int_dlg_editor_set_cursor (DLG_ELEMENT *pelementElement)
{
  DLG_EDITOR *pedit;

  pedit = (DLG_EDITOR*)pelementElement->pAddInfo;
  if (FOCUSSED)
    win_set_cursor_pos(OWNER, pedit->reg.iX + pedit->coordCursor.iX, pedit->reg.iY + pedit->coordCursor.iY);
}

static INT int_dlg_editor_len_to_end (DLG_ELEMENT *pelementElement)
{
  CHAR       *pcCounter;
  INT         i         = 0;
  DLG_EDITOR *pedit;
              
  pedit = (DLG_EDITOR*)pelementElement->pAddInfo;
  pcCounter = pedit->pcCursor + pedit->ulGapLength;
  for (; *pcCounter && *pcCounter != '\n' && pcCounter < pedit->pcBuffer + pedit->ulBufferLength;
       pcCounter++)
    i++;
  return i;
}

static void int_dlg_editor_go_char (DLG_ELEMENT *pelementElement, INT iChar)
{
  CHAR       *pcCursor;
  DLG_EDITOR *pedit;
              
  pedit = (DLG_EDITOR*)pelementElement->pAddInfo;
  if (iChar < 0 && -iChar >= pedit->iRow)
    iChar = -pedit->iRow + 1;
  if (iChar > 0 && iChar > int_dlg_editor_len_to_end(pelementElement))
    iChar = int_dlg_editor_len_to_end(pelementElement);
  if (!iChar)
    return;
  if (iChar < 0)
  {
    pcCursor = pedit->pcCursor + iChar;
    memmove(pcCursor + pedit->ulGapLength, pcCursor, -iChar);
    pedit->pcCursor = pcCursor;
    *(pedit->pcCursor) = 'X';
    pedit->iRow += iChar;
    if (pedit->coordCursor.iX <= -iChar)
    {
      pedit->coordCursor.iX = 1;
      pedit->iLeftChar = pedit->iRow - 1;
      int_dlg_editor_draw(pelementElement, 0, pedit->reg.iHeight - 2);
    }
    else
      pedit->coordCursor.iX += iChar;
  }
  else
  {
    pcCursor = pedit->pcCursor + iChar;
    memmove(pedit->pcCursor, pedit->pcCursor + pedit->ulGapLength, iChar);
    pedit->pcCursor = pcCursor;
    *(pedit->pcCursor) = 'X';
    pedit->iRow += iChar;
    if (pedit->reg.iWidth - 2 - pedit->coordCursor.iX < iChar)
    {
      pedit->coordCursor.iX = pedit->reg.iWidth - 2;
      pedit->iLeftChar = pedit->iRow - pedit->reg.iWidth + 2;
      int_dlg_editor_draw(pelementElement, 0, pedit->reg.iHeight - 2);
    }
    else
      pedit->coordCursor.iX += iChar;
  }
  int_dlg_editor_set_cursor(pelementElement);
}

static INT int_dlg_editor_line_length (DLG_ELEMENT *pelementElement)
{
  CHAR       *pcLine;
  DLG_EDITOR *pedit;
              
  pedit = (DLG_EDITOR*)pelementElement->pAddInfo;
  pcLine = int_dlg_editor_get_line(pelementElement, pedit->coordCursor.iY - 1);
  int_dlg_editor_make_line_buffer(pelementElement, pcLine);
  return iBufferLength;
}

static void int_dlg_editor_go_line (DLG_ELEMENT *pelementElement, INT iLine)
{
  CHAR       *pcLine;
  BOOL        bCompleteRedraw  = FALSE,
              bRedraw          = FALSE;
  INT         iOldY,
              iCounter;
  DLG_EDITOR *pedit;
              
  pedit = (DLG_EDITOR*)pelementElement->pAddInfo;
  if (iLine < 0 && pedit->iLine <= -iLine)
    iLine = -pedit->iLine + 1;
  if (iLine > 0 && pedit->iLines - pedit->iLine < iLine)
    iLine = pedit->iLines - pedit->iLine;
  if (!iLine)
    return;
  if (iLine < 0)
  {
    pcLine = int_dlg_editor_get_line(pelementElement, pedit->coordCursor.iY - 1);
    for (iCounter = 0; iCounter < -iLine; iCounter++)
      pcLine = int_dlg_editor_prev_line(pelementElement, pcLine);
    for (iCounter = 0;
         iCounter < pedit->iRow - 1 && *pcLine != '\n' && pcLine < pedit->pcBuffer + pedit->ulBufferLength;
         pcLine++, iCounter++)
      ;
    memmove(pedit->pcCursor + pedit->ulGapLength - (pedit->pcCursor - pcLine), pcLine,
            pedit->pcCursor - pcLine);
    pedit->pcCursor = pcLine;
    if (pedit->coordCursor.iY > -iLine)
      pedit->coordCursor.iY += iLine;
    else
    {
      for (iCounter = 0; iCounter < -iLine - pedit->coordCursor.iY + 1; iCounter++)
        pedit->pcFirstLine = int_dlg_editor_prev_line(pelementElement, pedit->pcFirstLine);
      iOldY = pedit->coordCursor.iY;
      pedit->coordCursor.iY = 1;
      bRedraw = TRUE;
      if (-iLine > pedit->reg.iHeight - 2)
        bCompleteRedraw = TRUE;
    }
    if (pedit->iRow > int_dlg_editor_line_length(pelementElement) + 1)
    {
      pedit->iRow = int_dlg_editor_line_length(pelementElement) + 1;
      if (pedit->iLeftChar < pedit->iRow)
        pedit->coordCursor.iX = pedit->iRow - pedit->iLeftChar;
      else
      {
        pedit->coordCursor.iX = 1;
        pedit->iLeftChar = pedit->iRow - 1;
        bRedraw = TRUE;
        bCompleteRedraw = TRUE;
      }
    }
    if (bRedraw)
    {
      if (bCompleteRedraw)
      {
        *(pedit->pcCursor) = 'X';
        int_dlg_editor_draw(pelementElement, 0, pedit->reg.iHeight - 2);
      }
      else
      {
        if (-iLine - iOldY + 1 < pedit->reg.iHeight - 2)
          win_down(OWNER, pedit->reg.iX + 1, pedit->reg.iY + 1, pedit->reg.iWidth - 2, pedit->reg.iHeight - 2,
                   -iLine - iOldY + 1);
        int_dlg_editor_draw(pelementElement, 0, min(-iLine - iOldY + 1, pedit->reg.iHeight - 2));
      }
    }
  }
  else
  {
    pcLine = int_dlg_editor_get_line(pelementElement, pedit->coordCursor.iY + iLine - 1);
    for (iCounter = 0;
         iCounter < pedit->iRow - 1 && *pcLine != '\n' && pcLine < pedit->pcBuffer + pedit->ulBufferLength;
         pcLine++, iCounter++)
      ;
    memmove(pedit->pcCursor, pedit->pcCursor + pedit->ulGapLength,
            pcLine - (pedit->pcCursor + pedit->ulGapLength));
    pedit->pcCursor = pcLine - pedit->ulGapLength;
    if (pedit->reg.iHeight - 2 >= pedit->coordCursor.iY + iLine)
      pedit->coordCursor.iY += iLine;
    else
    {
      for (iCounter = 0; iCounter < iLine - (pedit->reg.iHeight - 2 - pedit->coordCursor.iY); iCounter++)
        pedit->pcFirstLine = int_dlg_editor_next_line(pelementElement, pedit->pcFirstLine);
      iOldY = pedit->coordCursor.iY;
      pedit->coordCursor.iY = pedit->reg.iHeight - 2;
      bRedraw = TRUE;
      if (iLine > pedit->reg.iHeight - 2)
        bCompleteRedraw = TRUE;
    }
    if (pedit->iRow > int_dlg_editor_line_length(pelementElement) + 1)
    {
      pedit->iRow = int_dlg_editor_line_length(pelementElement) + 1;
      if (pedit->iLeftChar < pedit->iRow)
        pedit->coordCursor.iX = pedit->iRow - pedit->iLeftChar;
      else
      {
        pedit->coordCursor.iX = 1;
        pedit->iLeftChar = pedit->iRow - 1;
        bRedraw = TRUE;
        bCompleteRedraw = TRUE;
      }
    }
    if (bRedraw)
    {
      if (bCompleteRedraw)
        int_dlg_editor_draw(pelementElement, 0, pedit->reg.iHeight - 2);
      else
      {
        if (iLine - (pedit->reg.iHeight - 2 - iOldY) < pedit->reg.iHeight - 2)
          win_up(OWNER, pedit->reg.iX + 1, pedit->reg.iY + 1, pedit->reg.iWidth - 2, pedit->reg.iHeight - 2,
                 iLine - (pedit->reg.iHeight - 2 - iOldY));
        int_dlg_editor_draw(pelementElement, 2 * pedit->reg.iHeight - 4 - iLine - iOldY,
                            -pedit->reg.iHeight + 2 + iLine + iOldY);
      }
    }
  }
  pedit->iLine += iLine;
  *(pedit->pcCursor) = 'X';
  int_dlg_editor_set_cursor(pelementElement);
}

static void int_dlg_editor_insert_char (DLG_ELEMENT *pelementElement, CHAR cChar)
{
  DLG_EDITOR *pedit;
              
  pedit = (DLG_EDITOR*)pelementElement->pAddInfo;
  if (pedit->ulGapLength <= 1)
  {
    utl_beep();
    return;
  }
  *(pedit->pcCursor) = cChar;
  pedit->pcCursor++;
  *(pedit->pcCursor) = 'X';
  pedit->ulGapLength--;
  pedit->ulTextLength++;
  if (cChar != '\n')
  {
    pedit->iRow++;
    if (pedit->coordCursor.iX < pedit->reg.iWidth - 2)
    {
      pedit->coordCursor.iX++;
      int_dlg_editor_draw(pelementElement, pedit->coordCursor.iY - 1, 1);
    }
    else
    {
      pedit->iLeftChar++;
      int_dlg_editor_draw(pelementElement, 0, pedit->reg.iHeight - 2);
    }
  }
  else
  {
    pedit->coordCursor.iX = 1;
    pedit->iRow = 1;
    pedit->iLines++;
    pedit->iLine++;
    pedit->iVC = max(pedit->iLines / (pedit->reg.iHeight - 4), 1);
    if (pedit->iLeftChar > 0)
    {
      if (pedit->coordCursor.iY < pedit->reg.iHeight - 2)
        pedit->coordCursor.iY++;
      else
        pedit->pcFirstLine = int_dlg_editor_next_line(pelementElement, pedit->pcFirstLine);
      pedit->iLeftChar = 0;
      int_dlg_editor_draw(pelementElement, 0, pedit->reg.iHeight - 2);
    }
    else
    {
      if (pedit->coordCursor.iY < pedit->reg.iHeight - 2)
        pedit->coordCursor.iY++;
      else
      {
        pedit->pcFirstLine = int_dlg_editor_next_line(pelementElement, pedit->pcFirstLine);
        win_up(OWNER, pedit->reg.iX + 1, pedit->reg.iY + 1,
               pedit->reg.iWidth - 2, pedit->reg.iHeight - 3, 1);
      }
      int_dlg_editor_draw(pelementElement, pedit->coordCursor.iY - 2,
                          pedit->reg.iHeight - pedit->coordCursor.iY);
    }
  }
  int_dlg_editor_set_cursor(pelementElement);
}

static void int_dlg_editor_del_char (DLG_ELEMENT *pelementElement, BOOL bBack)
{
  CHAR        cChar;
  DLG_EDITOR *pedit;
              
  pedit = (DLG_EDITOR*)pelementElement->pAddInfo;
  if (bBack)
  {
    if (pedit->pcCursor == pedit->pcBuffer)
    {
      utl_beep();
      return;
    }
    pedit->ulTextLength--;
    pedit->ulGapLength++;
    pedit->pcCursor--;
    cChar = *(pedit->pcCursor);
    *(pedit->pcCursor) = 'X';
    if (cChar != '\n')
    {
      pedit->iRow--;
      if (pedit->coordCursor.iX > 1)
      {
        pedit->coordCursor.iX--;
        int_dlg_editor_draw(pelementElement, pedit->coordCursor.iY - 1, 1);
      }
      else
      {
        pedit->iLeftChar--;
        int_dlg_editor_draw(pelementElement, 0, pedit->reg.iHeight - 2);
      }
    }
    else
    {
      pedit->iLines--;
      if (pedit->coordCursor.iY <= 1)
        pedit->pcFirstLine =
          int_dlg_editor_prev_line(pelementElement,
                                   int_dlg_editor_next_line(pelementElement, pedit->pcFirstLine));
      else
        pedit->coordCursor.iY--;
      pedit->iLine--;
      pedit->iRow = int_dlg_editor_line_length(pelementElement) -
        int_dlg_editor_len_to_end(pelementElement) + 1;
      if (pedit->iRow > pedit->reg.iWidth - 2)
      {
        pedit->iLeftChar = pedit->iRow - 1;
        pedit->coordCursor.iX = 1;
      }
      else
        pedit->coordCursor.iX = pedit->iRow;
      int_dlg_editor_draw(pelementElement, 0, pedit->reg.iHeight - 2);
    }
  }
  else
  {
    if (pedit->pcCursor + pedit->ulGapLength >= pedit->pcBuffer + pedit->ulBufferLength)
    {
      utl_beep();
      return;
    }
    pedit->ulTextLength--;
    cChar = *(pedit->pcCursor + pedit->ulGapLength);
    pedit->ulGapLength++;
    if (cChar != '\n')
      int_dlg_editor_draw(pelementElement, pedit->coordCursor.iY - 1, 1);
    else
    {
      pedit->iLines--;
      int_dlg_editor_draw(pelementElement, pedit->coordCursor.iY - 1,
                          pedit->reg.iHeight - 1 - pedit->coordCursor.iY);
    }
  }
  int_dlg_editor_set_cursor(pelementElement);
}

static void int_dlg_editor_del_line (DLG_ELEMENT *pelementElement)
{
  INT         iLineLength,
              iLenToEnd;
  DLG_EDITOR *pedit;
              
  pedit = (DLG_EDITOR*)pelementElement->pAddInfo;
  if (pedit->pcCursor + pedit->ulGapLength >= pedit->pcBuffer + pedit->ulBufferLength)
    return;
  iLineLength = int_dlg_editor_line_length(pelementElement);
  iLenToEnd = int_dlg_editor_len_to_end(pelementElement);
  pedit->ulGapLength += iLineLength;
  pedit->ulTextLength -= iLineLength;
  pedit->pcCursor -= iLineLength - iLenToEnd;
  pedit->iRow = 1;
  pedit->coordCursor.iX = 1;
  if (pedit->iLeftChar > 0)
  {
    pedit->iLeftChar = 0;
    int_dlg_editor_draw(pelementElement, 0, pedit->reg.iHeight - 2);
  }
  else
    int_dlg_editor_draw(pelementElement, pedit->coordCursor.iY - 1, 1);
  if (pedit->pcCursor + pedit->ulGapLength < pedit->pcBuffer + pedit->ulBufferLength)
    int_dlg_editor_del_char(pelementElement, FALSE);
}

void dlg_editor_handle_event (DLG_ELEMENT *pelementElement, UTL_EVENT *peventEvent)
{
  CHAR        cChar;
  INT         iPos,
              iLine;
  DLG_EDITOR *pedit;
  UTL_EVENT   eventEvent;

  pedit = pelementElement->pAddInfo;
  win_make_local(OWNER, peventEvent, &eventEvent);
  if (FOCUSSED && OWNER_FOCUSSED && pelementElement->pcHelpLine)
    sts_write_help(pelementElement->pcHelpLine);
  switch (eventEvent.uiKind)
  {
    case E_MESSAGE :
      switch (eventEvent.uiMessage)
      {
        case M_INIT :
          if (pedit->pelementBorder)
            win_add_element(OWNER, pedit->pelementBorder);
          win_add_element(OWNER, pedit->pelementScrollH);
          win_add_element(OWNER, pedit->pelementScrollV);
        case M_DRAW :
          win_sw_a(OWNER, pedit->reg.iX + 1, pedit->reg.iY + 1, pedit->reg.iWidth - 2, pedit->reg.iHeight - 2,
                   win_get_color(OWNER, PAL_COL_BACKGROUND));
          int_dlg_editor_draw(pelementElement, 0, pedit->reg.iHeight - 2);
          peventEvent->uiKind = E_DONE;
          break;
        case M_GET_FOCUS :
          int_dlg_editor_set_cursor(pelementElement);
          peventEvent->uiKind = E_DONE;
          break;
        case M_LOST_FOCUS :     
          pedit->bDragging = FALSE;
          peventEvent->uiKind = E_DONE;
          break;
        case M_QUIT :
          memmove(pedit->pcCursor, pedit->pcCursor + pedit->ulGapLength,
                  pedit->ulTextLength - (pedit->pcCursor - pedit->pcBuffer));
          pedit->pcBuffer[pedit->ulTextLength] = 0;
          utl_free(pelementElement->pAddInfo);
          peventEvent->uiKind = E_DONE;
          return;
        case M_QUERY_VALUES :
          *((CHAR**)peventEvent->ulAddInfo) = pedit->pcBuffer;
          peventEvent->uiKind = E_DONE;
          break;
        case M_SCROLL_UP :
          if (peventEvent->ulAddInfo == DLG_VERTICAL)
            int_dlg_editor_go_line(pelementElement, -1);
          else
            int_dlg_editor_go_char(pelementElement, -1);
          SET_EDIT_SCROLL;
          peventEvent->uiKind = E_DONE;
          break;
        case M_SCROLL_DOWN :
          if (peventEvent->ulAddInfo == DLG_VERTICAL)
            int_dlg_editor_go_line(pelementElement, 1);
          else
            int_dlg_editor_go_char(pelementElement, 1);
          SET_EDIT_SCROLL;
          peventEvent->uiKind = E_DONE;
          break;
        case M_SCROLL_PAGE_UP :
          if (peventEvent->ulAddInfo == DLG_VERTICAL)
            int_dlg_editor_go_line(pelementElement, -(pedit->reg.iHeight - 2));
          else
            int_dlg_editor_go_char(pelementElement, -(pedit->reg.iWidth - 2));
          SET_EDIT_SCROLL;
          peventEvent->uiKind = E_DONE;
          break;
        case M_SCROLL_PAGE_DOWN :
          if (peventEvent->ulAddInfo == DLG_VERTICAL)
            int_dlg_editor_go_line(pelementElement, pedit->reg.iHeight - 2);
          else
            int_dlg_editor_go_char(pelementElement, pedit->reg.iWidth - 2);
          SET_EDIT_SCROLL;
          peventEvent->uiKind = E_DONE;
          break;     
        case M_SCROLL_CHANGED :
          dlg_scroll_query_values(peventEvent->pFrom, &iPos);
          if (peventEvent->ulAddInfo == DLG_VERTICAL)
          {
            iLine = iPos * pedit->iVC + 1;
            int_dlg_editor_go_line(pelementElement, iLine - pedit->iLine);
          }
          else
          {
            iLine = iPos * pedit->iHC + 1;
            int_dlg_editor_go_char(pelementElement, iLine - pedit->iRow);
          }      
          peventEvent->uiKind = E_DONE;
          break;
        case M_SCROLL_END_DRAGGING :
          SET_EDIT_SCROLL;
          peventEvent->uiKind = E_DONE;
          break;
        case M_SET_DISPLAY :
          pedit->reg = *(VIO_REGION*)peventEvent->ulAddInfo;
          if (pedit->coordCursor.iX > pedit->reg.iWidth - 2)
            int_dlg_editor_go_char(pelementElement, pedit->reg.iWidth - pedit->coordCursor.iX - 2);
          if (pedit->coordCursor.iY > pedit->reg.iHeight - 2)
            int_dlg_editor_go_line(pelementElement, pedit->reg.iHeight - pedit->coordCursor.iY - 2);
          if (pedit->pelementBorder)
            dlg_border_set_display(pedit->pelementBorder, pedit->reg.iX, pedit->reg.iY, pedit->reg.iWidth,
                                   pedit->reg.iHeight);
          dlg_scroll_set_display(pedit->pelementScrollH, pedit->reg.iX + 1,
                                 pedit->reg.iY + pedit->reg.iHeight - 1, pedit->reg.iWidth - 4);
          dlg_scroll_set_display(pedit->pelementScrollV, pedit->reg.iX + pedit->reg.iWidth - 1,
                                 pedit->reg.iY + 1, pedit->reg.iHeight - 4);
          SET_EDIT_SCROLL;
          peventEvent->uiKind = E_DONE;
          break;
      }  
      break;
    case E_KEY :
      if (!FOCUSSED)
        break;
      switch (eventEvent.wKey)
      {
        case K_LEFT :
          int_dlg_editor_go_char(pelementElement, -1);
          SET_EDIT_SCROLL;
          peventEvent->uiKind = E_DONE;
          break;
        case K_RIGHT :
          int_dlg_editor_go_char(pelementElement, 1);
          SET_EDIT_SCROLL;
          peventEvent->uiKind = E_DONE;
          break;
        case K_HOME :
          int_dlg_editor_go_char(pelementElement, -pedit->iRow + 1);
          SET_EDIT_SCROLL;
          peventEvent->uiKind = E_DONE;
          break;
        case K_END :
          int_dlg_editor_go_char(pelementElement, int_dlg_editor_len_to_end(pelementElement));
          SET_EDIT_SCROLL;
          peventEvent->uiKind = E_DONE;
          break;
        case K_UP :
          int_dlg_editor_go_line(pelementElement, -1);
          SET_EDIT_SCROLL;
          peventEvent->uiKind = E_DONE;
          break;
        case K_DOWN :
          int_dlg_editor_go_line(pelementElement, 1);
          SET_EDIT_SCROLL;
          peventEvent->uiKind = E_DONE;
          break;
        case K_PGUP :
          int_dlg_editor_go_line(pelementElement, -(pedit->reg.iHeight - 2));
          SET_EDIT_SCROLL;
          peventEvent->uiKind = E_DONE;
          break;
        case K_PGDN :
          int_dlg_editor_go_line(pelementElement, pedit->reg.iHeight - 2);
          SET_EDIT_SCROLL;
          peventEvent->uiKind = E_DONE;
          break;
        case K_C_PGUP :
          int_dlg_editor_go_line(pelementElement, -pedit->iLine + 1);
          SET_EDIT_SCROLL;
          peventEvent->uiKind = E_DONE;
          break;
        case K_C_PGDN :
          int_dlg_editor_go_line(pelementElement, pedit->iLines - pedit->iLine);
          SET_EDIT_SCROLL;
          peventEvent->uiKind = E_DONE;
          break;
        case K_DEL :
          if (pedit->bCanChange)
          {
            int_dlg_editor_del_char(pelementElement, FALSE);
            glb_send_message(pelementElement, RECIPIENT, M_EDITOR_CHANGED, eventEvent.wKey);
            SET_EDIT_SCROLL;
            peventEvent->uiKind = E_DONE;
          }
          break;
        case K_BACKSPACE :
          if (pedit->bCanChange)
          {
            int_dlg_editor_del_char(pelementElement, TRUE);
            glb_send_message(pelementElement, RECIPIENT, M_EDITOR_CHANGED, eventEvent.wKey);
            SET_EDIT_SCROLL;
            peventEvent->uiKind = E_DONE;
          }
          break;
        case K_C_Y :
          if (pedit->bCanChange)
          {
            int_dlg_editor_del_line(pelementElement);
            glb_send_message(pelementElement, RECIPIENT, M_EDITOR_CHANGED, eventEvent.wKey);
            SET_EDIT_SCROLL;
            peventEvent->uiKind = E_DONE;
          }
          break;
        default :
          if (eventEvent.wKey < 256 && (eventEvent.wKey >= 32 || eventEvent.wKey == K_ENTER))
          {
            if (pedit->bCanChange)
            {
              if (eventEvent.wKey != K_ENTER)
                cChar = (CHAR)eventEvent.wKey;
              else
                cChar = '\n';
              int_dlg_editor_insert_char(pelementElement, cChar);
              glb_send_message(pelementElement, RECIPIENT, M_EDITOR_CHANGED, eventEvent.wKey);
              SET_EDIT_SCROLL;
              peventEvent->uiKind = E_DONE;
            }
          }
          break; 
      }
      break;
    case E_MSM_L_DOWN :
      eventEvent.iHor -= pedit->reg.iX;
      eventEvent.iVer -= pedit->reg.iY;
      if (eventEvent.iHor > 0 && eventEvent.iVer > 0 &&
          eventEvent.iHor < pedit->reg.iWidth - 1 && eventEvent.iVer < pedit->reg.iHeight - 1)
      {                     
        int_dlg_editor_go_line(pelementElement, eventEvent.iVer - (INT)pedit->coordCursor.iY);
        int_dlg_editor_go_char(pelementElement, eventEvent.iHor - (INT)pedit->coordCursor.iX);
        pedit->bDragging = TRUE;
        peventEvent->uiKind = E_DONE;
      }
      break;       
    case E_MSM_L_UP :
      pedit->bDragging = FALSE;
      eventEvent.iHor -= pedit->reg.iX;
      eventEvent.iVer -= pedit->reg.iY;
      if (eventEvent.iHor > 0 && eventEvent.iVer > 0 &&
          eventEvent.iHor < pedit->reg.iWidth - 1 && eventEvent.iVer < pedit->reg.iHeight - 1)
        peventEvent->uiKind = E_DONE;
      break;
    case E_MSM_MOVE :
      eventEvent.iHor -= pedit->reg.iX;
      eventEvent.iVer -= pedit->reg.iY;
      if (eventEvent.iHor > 0 && eventEvent.iVer > 0 && eventEvent.iHor < pedit->reg.iWidth - 1 &&
          eventEvent.iVer < pedit->reg.iHeight - 1 && pedit->bDragging)
      {                     
        int_dlg_editor_go_line(pelementElement, eventEvent.iVer - pedit->coordCursor.iY);
        int_dlg_editor_go_char(pelementElement, eventEvent.iHor - pedit->coordCursor.iX);
        peventEvent->uiKind = E_DONE;
      }
      break;
  }
  if (!(eventEvent.wButtons & MSM_B_LEFT))
    pedit->bDragging = FALSE;
}

DLG_ELEMENT* dlg_init_editor (INT iX, INT iY, INT iWidth, INT iHeight, ULONG ulBufferLength,
                              CHAR *pcBuffer, BOOL bCanChange, BOOL bBorder, CHAR *pcHelpLine, UINT uiID,
                              BOOL bCanBeActivated, void *pRecipient)
{
  DLG_ELEMENT *pelementReturnVar;
  DLG_EDITOR  *pedit;

  if (!(pelementReturnVar = utl_alloc(sizeof(DLG_ELEMENT))))
    return NULL;
  pelementReturnVar->handle_event = dlg_editor_handle_event;
  pelementReturnVar->pcHelpLine = pcHelpLine;
  pelementReturnVar->uiID = uiID;
  pelementReturnVar->flFlags.binCanBeActivated = bCanBeActivated;
  pelementReturnVar->flFlags.binFocussed = FALSE;
  pelementReturnVar->pRecipient = pRecipient;
  if (!(pelementReturnVar->pAddInfo = utl_alloc(sizeof(DLG_EDITOR))))
    return NULL;
  pedit = pelementReturnVar->pAddInfo;
  pedit->reg.iX = iX;
  pedit->reg.iY = iY;
  pedit->reg.iWidth = iWidth;
  pedit->reg.iHeight = iHeight;
  pedit->coordCursor.iX = 1;
  pedit->coordCursor.iY = 1;
  pedit->iLine = 1;
  pedit->iRow = 1;
  pedit->ulBufferLength = ulBufferLength;
  pedit->ulTextLength = strlen(pcBuffer);
  pedit->ulGapLength = pedit->ulBufferLength - pedit->ulTextLength;
  pedit->iLeftChar = 0;
  pedit->pcCursor = pcBuffer;
  pedit->pcFirstLine = pcBuffer;
  pedit->pcBuffer = pcBuffer;   
  pedit->bDragging = FALSE;
  pedit->iLines = int_dlg_editor_count_lines(pelementReturnVar);
  pedit->iVC = max(pedit->iLines / (iHeight - 4), 1);
  pedit->iHC = MAX_LINE_LENGTH / (iWidth - 4);
  pedit->bCanChange = bCanChange;
  memmove(pedit->pcCursor + pedit->ulGapLength, pedit->pcCursor,
          pedit->ulTextLength - (pedit->pcCursor - pcBuffer));
  *(pedit->pcCursor) = 'X';
  pedit->pelementScrollH = dlg_init_scroll(iX + 1, iY + iHeight - 1, iWidth - 4, 0, DLG_HORIZONTAL, NULL,
                                           uiID + 10000, FALSE, pelementReturnVar);
  pedit->pelementScrollV = dlg_init_scroll(iX + iWidth - 1, iY + 1, iHeight - 4, 0, DLG_VERTICAL, NULL,
                                           uiID + 20000, FALSE, pelementReturnVar);
  if (bBorder)
    pedit->pelementBorder = dlg_init_border(iX, iY, iWidth, iHeight, uiID + 30000); 
  else
    pedit->pelementBorder = NULL;
  return pelementReturnVar;
}

/*
Local Variables:
compile-command: "wmake -f stools.mk -h -e"
End:
*/
