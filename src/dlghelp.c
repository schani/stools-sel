/***************************************************************************
 ***************************************************************************
 ***                                                                     ***
 ***                       Schani-Tools fÅr C                            ***
 ***                                                                     ***
 ***                           Hilfe-Box                                 ***
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

static CHAR         int_dlg_get_attri                (DLG_ELEMENT*, BYTE);
static HLP_LINK*    int_dlg_check_link               (DLG_ELEMENT*);
static void         int_dlg_hide_link                (DLG_ELEMENT*, HLP_LINK*);
static void         int_dlg_show_link                (DLG_ELEMENT*, HLP_LINK*);
static void         int_dlg_make_line_buffer         (DLG_ELEMENT*, INT);
static void         int_dlg_make_color_buffer        (DLG_ELEMENT*, INT);
static void         int_dlg_draw_line                (DLG_ELEMENT*, INT);
static void         int_dlg_draw_inner               (DLG_ELEMENT*);

extern GLB_PROGRAM prgProgram;

static CHAR acLineBuffer[256];
static CHAR acColorBuffer[256];

static CHAR int_dlg_get_attri (DLG_ELEMENT *pelementElement, BYTE byColor)
{
  switch (byColor)
  {
    case COLOR_PLAIN :
      return win_get_color(OWNER, PAL_COL_HELP);
    case COLOR_BOLD :
      return win_get_color(OWNER, PAL_COL_HELP_BOLD);
    case COLOR_ITALIC :
      return win_get_color(OWNER, PAL_COL_HELP_ITALIC);
    case COLOR_UNDERLINE :
      return win_get_color(OWNER, PAL_COL_HELP_UNDERLINE);
    case COLOR_BOLD_ITALIC :
      return win_get_color(OWNER, PAL_COL_HELP_BOLD_ITALIC);
    case COLOR_BOLD_UNDERLINE :
      return win_get_color(OWNER, PAL_COL_HELP_BOLD_UNDERLINE);
    case COLOR_ITALIC_UNDERLINE :
      return win_get_color(OWNER, PAL_COL_HELP_ITALIC_UNDERLINE);
    case COLOR_BOLD_ITALIC_UNDERLINE :
      return win_get_color(OWNER, PAL_COL_HELP_BOLD_ITALIC_UNDERLINE);
  }
  return 0;
}

static HLP_LINK* int_dlg_check_link (DLG_ELEMENT *pelementElement)
{
  INT           iCursorX,
                iCursorY,
                iIndex;
  HLP_LINK     *plnkReturnVar;                                 
  DLG_HELP_BOX *phelpbox;
             
  phelpbox = (DLG_HELP_BOX*)pelementElement->pAddInfo;
  plnkReturnVar = phelpbox->pctxContext->plnkLinks;
  iCursorX = phelpbox->coordCursor.iX + phelpbox->iFirstChar;
  if (phelpbox->coordCursor.iY < phelpbox->pctxContext->swFreeze)
    iCursorY = phelpbox->coordCursor.iY;
  else
    iCursorY = phelpbox->coordCursor.iY + phelpbox->iFirstLine;
  for (iIndex = 0; iIndex < phelpbox->pctxContext->dwLinks; iIndex++)
  {
    if (iCursorX >= plnkReturnVar->swX && iCursorY >= plnkReturnVar->swY &&
        iCursorX < plnkReturnVar->swX + plnkReturnVar->swWidth &&
        iCursorY < plnkReturnVar->swY + plnkReturnVar->swHeight)
      return plnkReturnVar;
    plnkReturnVar++;
  }
  return NULL;
}

static void int_dlg_hide_link (DLG_ELEMENT *pelementElement, HLP_LINK *plnkLink)
{
  INT           iFirstLine,
                iLastLine,
                iCounter;
  DLG_HELP_BOX *phelpbox;
  MOUSE;

  if (plnkLink == NULL)
    return;
  phelpbox = (DLG_HELP_BOX*)pelementElement->pAddInfo;
  if (plnkLink->swY < phelpbox->pctxContext->swFreeze)
  {
    iFirstLine = plnkLink->swY;
    iLastLine = iFirstLine + plnkLink->swHeight;
  }
  else
  {
    iFirstLine = plnkLink->swY - phelpbox->iFirstLine;
    if (iFirstLine >= phelpbox->reg.iHeight - 2)
      return;
    iLastLine = min(iFirstLine + plnkLink->swHeight, phelpbox->reg.iHeight - 3);
  }
  BEGIN_MOUSE_IN(OWNER);
  for (iCounter = iFirstLine; iCounter <= iLastLine; iCounter++)
    int_dlg_draw_line(pelementElement, iCounter);
  END_MOUSE;
}

static void int_dlg_show_link (DLG_ELEMENT *pelementElement, HLP_LINK *plnkLink)
{
  INT           iX,
                iY,
                iWidth,
                iHeight;
  DLG_HELP_BOX *phelpbox;
                
  if (plnkLink == NULL)
    return;
  phelpbox = (DLG_HELP_BOX*)pelementElement->pAddInfo;
  iX = plnkLink->swX - phelpbox->iFirstChar;
  if (iX >= phelpbox->reg.iWidth - 2)
    return;
  iWidth = min(plnkLink->swWidth, phelpbox->reg.iWidth - 2 - iX);
  if (plnkLink->swY < phelpbox->pctxContext->swFreeze)
  {
    iY = plnkLink->swY;
    iHeight = plnkLink->swHeight;
  }
  else
  {
    iY = plnkLink->swY - phelpbox->iFirstLine;
    if (iY >= phelpbox->reg.iHeight - 2)
      return;
    iHeight = min(plnkLink->swHeight, phelpbox->reg.iHeight - 2 - iY);
  }
  win_sw_a(OWNER, phelpbox->reg.iX + iX + 1, phelpbox->reg.iY + iY + 1, iWidth, iHeight,
           win_get_color(OWNER, PAL_COL_HELP_SEL_HOT_SPOT));
}

static void int_dlg_make_line_buffer (DLG_ELEMENT *pelementElement, INT iLine)
{
  DLG_HELP_BOX *phelpbox;
                
  phelpbox = (DLG_HELP_BOX*)pelementElement->pAddInfo;
  if (iLine >= phelpbox->pctxContext->swFreeze)
    iLine += phelpbox->iFirstLine;
  if (strlen(phelpbox->pctxContext->ppcText[iLine]) < phelpbox->iFirstChar)
    strcpy(acLineBuffer, "");
  else
    strcpy(acLineBuffer, phelpbox->pctxContext->ppcText[iLine] + phelpbox->iFirstChar);
  acLineBuffer[phelpbox->reg.iWidth - 2] = 0;
}

static void int_dlg_make_color_buffer (DLG_ELEMENT *pelementElement, INT iLine)
{
  INT           iCounter1,
                iCounter2,
                iIndex        = 0;
  SWORD         swCount;
  CHAR          cAttri,
                acBuffer[256];
  DLG_HELP_BOX *phelpbox;
                
  phelpbox = (DLG_HELP_BOX*)pelementElement->pAddInfo;
  if (iLine >= phelpbox->pctxContext->swFreeze)
    iLine += phelpbox->iFirstLine;
  if (strlen(phelpbox->pctxContext->ppcText[iLine]) >= phelpbox->iFirstChar)
  {
    for (iCounter1 = 0; phelpbox->pctxContext->ppclrColors[iLine][iCounter1].swCount != -1; iCounter1++)
    {
      swCount = phelpbox->pctxContext->ppclrColors[iLine][iCounter1].swCount;
      cAttri = int_dlg_get_attri(pelementElement, phelpbox->pctxContext->ppclrColors[iLine][iCounter1].byColor);
      for (iCounter2 = 0; iCounter2 < swCount; iCounter2++)
      {
        acBuffer[iIndex] = cAttri;
        iIndex++;
      }
    }
    swCount = phelpbox->pctxContext->ppclrColors[iLine][iCounter1].swCount;
    cAttri = int_dlg_get_attri(pelementElement, phelpbox->pctxContext-> ppclrColors[iLine][iCounter1].byColor);
    for (; iIndex < 255; iIndex++)
      acBuffer[iIndex] = cAttri;
    acBuffer[iIndex] = 0;
    strcpy(acColorBuffer, acBuffer + phelpbox->iFirstChar);
  }
}

static void int_dlg_draw_line (DLG_ELEMENT *pelementElement, INT iLine)
{
  INT           iCounter;
  DLG_HELP_BOX *phelpbox;
  MOUSE;
                
  phelpbox = (DLG_HELP_BOX*)pelementElement->pAddInfo;
  BEGIN_MOUSE_IN(OWNER);  
  if (iLine < phelpbox->pctxContext->swFreeze || phelpbox->iFirstLine + iLine < phelpbox->pctxContext->swLines)
  {
    int_dlg_make_line_buffer(pelementElement, iLine);
    int_dlg_make_color_buffer(pelementElement, iLine);
    for (iCounter = 0; acLineBuffer[iCounter]; iCounter++)
    {
      phelpbox->pcharBuffer[iCounter].character = acLineBuffer[iCounter];
      phelpbox->pcharBuffer[iCounter].attribute = acColorBuffer[iCounter];
    }
    win_sw(OWNER, phelpbox->reg.iX + 1, phelpbox->reg.iY + iLine + 1, iCounter, 1, phelpbox->pcharBuffer);
    win_sw_za(OWNER, phelpbox->reg.iX + iCounter + 1, phelpbox->reg.iY + iLine + 1,
              phelpbox->reg.iWidth - 2 - iCounter, 1, K_SPACE, win_get_color(OWNER, PAL_COL_HELP));
  }
  else
    win_sw_za(OWNER, phelpbox->reg.iX + 1, phelpbox->reg.iY + iLine + 1, phelpbox->reg.iWidth - 2, 1, K_SPACE,
              win_get_color(OWNER, PAL_COL_HELP));
  END_MOUSE;
}

static void int_dlg_draw_inner (DLG_ELEMENT *pelementElement)
{
  INT           iCounter;
  DLG_HELP_BOX *phelpbox;
  MOUSE;
                
  phelpbox = (DLG_HELP_BOX*)pelementElement->pAddInfo;
  BEGIN_MOUSE_IN(OWNER);
  for (iCounter = 0; iCounter < phelpbox->reg.iHeight - 2; iCounter++)
    int_dlg_draw_line(pelementElement, iCounter);
  END_MOUSE;
}

void dlg_help_box_handle_event (DLG_ELEMENT *pelementElement, UTL_EVENT *peventEvent)
{
  DLG_HELP_BOX *phelpbox;
  HLP_CONTEXT  *pctxNewContext;
  HLP_LINK     *plnkNewLink;
  BOOL          bRedrawLink = FALSE;
  INT           iPos;
  UTL_EVENT     eventEvent;

  phelpbox = (DLG_HELP_BOX*)pelementElement->pAddInfo;
  win_make_local(OWNER, peventEvent, &eventEvent);
  plnkNewLink = int_dlg_check_link(pelementElement);
  if (plnkNewLink != phelpbox->plnkActLink)
  {
    if (phelpbox->plnkActLink)
      int_dlg_hide_link(pelementElement, phelpbox->plnkActLink);
    phelpbox->plnkActLink = plnkNewLink;
    if (phelpbox->plnkActLink)
      int_dlg_show_link(pelementElement, phelpbox->plnkActLink);
  }                
  if (FOCUSSED && OWNER_FOCUSSED && pelementElement->pcHelpLine)
    sts_write_help(pelementElement->pcHelpLine);
  switch (eventEvent.uiKind)
  {                        
    case E_MESSAGE :
      switch (eventEvent.uiMessage)
      {
        case M_INIT :
          if (phelpbox->pelementBorder)
            win_add_element(OWNER, phelpbox->pelementBorder);
          win_add_element(OWNER, phelpbox->pelementScrollH);
          win_add_element(OWNER, phelpbox->pelementScrollV);
          int_dlg_draw_inner(pelementElement);
          peventEvent->uiKind = E_DONE;
          break;
        case M_GET_FOCUS :
          SET_HELP_CURSOR;
        case M_LOST_FOCUS :
          if (eventEvent.uiMessage == M_LOST_FOCUS)
            phelpbox->bDragging = FALSE;
          peventEvent->uiKind = E_DONE;
          break;
        case M_QUIT :
          hlp_release_context(phelpbox->pctxContext);
          utl_free(pelementElement->pAddInfo);
          peventEvent->uiKind = E_DONE;
          return;
        case M_SET_VALUES :
          if (!(pctxNewContext = hlp_get_context(peventEvent->ulAddInfo)))
          {
            box_info(BOX_INFO, BOX_OK, "Zu diesem Thema existiert keine Hilfe!", 0);
            break;
          }
          hlp_release_context(phelpbox->pctxContext);
          phelpbox->coordCursor.iX = phelpbox->coordCursor.iY = 0;
          phelpbox->iFirstLine = phelpbox->iFirstChar = 0;
          phelpbox->pctxContext = pctxNewContext;       
          phelpbox->plnkActLink = NULL;
          phelpbox->bDragging = FALSE;
          phelpbox->dwContext = peventEvent->ulAddInfo;
          phelpbox->iHC = 100 / (phelpbox->reg.iWidth - 4);
          phelpbox->iVC = phelpbox->pctxContext->swLines / (phelpbox->reg.iHeight - 4);
          int_dlg_draw_inner(pelementElement);
          SET_HELP_CURSOR;
          SET_HELP_SCROLL;    
          peventEvent->uiKind = E_DONE;
          break;
        case M_QUERY_VALUES :
          *((DWORD*)peventEvent->ulAddInfo) = phelpbox->dwContext;
          peventEvent->uiKind = E_DONE;             
          break;
        case M_SET_DISPLAY :
          phelpbox->reg = *(VIO_REGION*)peventEvent->ulAddInfo;
          phelpbox->coordCursor.iX = min(phelpbox->coordCursor.iX, phelpbox->reg.iWidth - 3);
          phelpbox->coordCursor.iY = min(phelpbox->coordCursor.iY, phelpbox->reg.iHeight - 3);
          utl_free(phelpbox->pcharBuffer);
          phelpbox->pcharBuffer = utl_alloc(sizeof(CHARACTER) * phelpbox->reg.iWidth);
          dlg_scroll_set_display(phelpbox->pelementScrollH, phelpbox->reg.iX + 1,
                                 phelpbox->reg.iY + phelpbox->reg.iHeight - 1, phelpbox->reg.iWidth - 4);
          dlg_scroll_set_display(phelpbox->pelementScrollV, phelpbox->reg.iX + phelpbox->reg.iWidth - 1,
                                 phelpbox->reg.iY + 1, phelpbox->reg.iHeight - 4);
          if (phelpbox->pelementBorder)
            dlg_border_set_display(phelpbox->pelementBorder, phelpbox->reg.iX, phelpbox->reg.iY,
                                   phelpbox->reg.iWidth, phelpbox->reg.iHeight);
          phelpbox->iHC = 100 / (phelpbox->reg.iWidth - 4);
          phelpbox->iVC = phelpbox->pctxContext->swLines / (phelpbox->reg.iHeight - 4);
          SET_HELP_SCROLL;
          SET_HELP_CURSOR;
          peventEvent->uiKind = E_DONE;
          break;
        case M_DRAW :
          int_dlg_draw_inner(pelementElement);
          peventEvent->uiKind = E_DONE;
          break;                        
        case M_SCROLL_CHANGED :
          if (peventEvent->ulAddInfo == DLG_HORIZONTAL)
          {
            dlg_scroll_query_values(phelpbox->pelementScrollH, &iPos);
            phelpbox->iFirstChar = iPos * phelpbox->iHC;
            bRedrawLink = TRUE;
            int_dlg_draw_inner(pelementElement);
            SET_HELP_CURSOR;
          }
          else
          {
            dlg_scroll_query_values(phelpbox->pelementScrollV, &iPos);
            phelpbox->iFirstLine = iPos * phelpbox->iVC;
            bRedrawLink = TRUE;
            int_dlg_draw_inner(pelementElement);
            SET_HELP_CURSOR;
          }
          break;
        case M_SCROLL_UP :
          if (peventEvent->ulAddInfo == DLG_HORIZONTAL)
          {  
            if (phelpbox->iFirstChar)
            {
              phelpbox->iFirstChar--;
              if (phelpbox->coordCursor.iX < phelpbox->reg.iWidth - 3)
                phelpbox->coordCursor.iX++;
              bRedrawLink = TRUE;
              int_dlg_draw_inner(pelementElement);
              SET_HELP_CURSOR;
              SET_HELP_SCROLL;
            }
          }
          else
            if (phelpbox->iFirstLine)
            {
              phelpbox->iFirstLine--;
              if (phelpbox->coordCursor.iY < phelpbox->reg.iHeight - 3)
                phelpbox->coordCursor.iY++;
              bRedrawLink = TRUE;
              int_dlg_draw_inner(pelementElement);
              SET_HELP_CURSOR;
              SET_HELP_SCROLL;
            }
          break;
        case M_SCROLL_PAGE_UP :
          if (peventEvent->ulAddInfo == DLG_HORIZONTAL)
          {  
            if (phelpbox->iFirstChar)
            {                            
              phelpbox->iFirstChar = max(phelpbox->iFirstChar - phelpbox->reg.iWidth, 0);
              bRedrawLink = TRUE;
              int_dlg_draw_inner(pelementElement);
              SET_HELP_CURSOR;
              SET_HELP_SCROLL;
            }
          }
          else
            if (phelpbox->iFirstLine)
            {
              phelpbox->iFirstLine = max(phelpbox->iFirstLine - phelpbox->reg.iHeight, 0);
              bRedrawLink = TRUE;
              int_dlg_draw_inner(pelementElement);
              SET_HELP_CURSOR;
              SET_HELP_SCROLL;
            }
          break;
        case M_SCROLL_DOWN :
          if (peventEvent->ulAddInfo == DLG_HORIZONTAL)
          {  
            if (phelpbox->iFirstChar < 100)
            {
              phelpbox->iFirstChar++;
              if (phelpbox->coordCursor.iX)
                phelpbox->coordCursor.iX--;
              bRedrawLink = TRUE;
              int_dlg_draw_inner(pelementElement);
              SET_HELP_CURSOR;
              SET_HELP_SCROLL;
            }
          }
          else
            if (phelpbox->iFirstLine < phelpbox->pctxContext->swLines - 1)
            {
              phelpbox->iFirstLine++;
              if (phelpbox->coordCursor.iY)
                phelpbox->coordCursor.iY--;
              bRedrawLink = TRUE;
              int_dlg_draw_inner(pelementElement);
              SET_HELP_CURSOR;
              SET_HELP_SCROLL;
            }
          break;
        case M_SCROLL_PAGE_DOWN :
          if (peventEvent->ulAddInfo == DLG_HORIZONTAL)
          {  
            if (phelpbox->iFirstChar < 100)
            {                            
              phelpbox->iFirstChar = min(phelpbox->iFirstChar + phelpbox->reg.iWidth, 100);
              bRedrawLink = TRUE;
              int_dlg_draw_inner(pelementElement);
              SET_HELP_CURSOR;
              SET_HELP_SCROLL;
            }
          }
          else
            if (phelpbox->iFirstLine < phelpbox->pctxContext->swLines - 1)
            {
              phelpbox->iFirstLine = min(phelpbox->iFirstLine + phelpbox->reg.iHeight,
                                         phelpbox->pctxContext->swLines - 1);
              bRedrawLink = TRUE;
              int_dlg_draw_inner(pelementElement);
              SET_HELP_CURSOR;
              SET_HELP_SCROLL;
            }
          break;
      }
      break;
    case E_KEY :
      if (!FOCUSSED)
        break;
      switch (eventEvent.wKey)
      {
        case K_LEFT :
          if (phelpbox->coordCursor.iX)
          {
            phelpbox->coordCursor.iX--;
            SET_HELP_CURSOR;
          }
          else
            if (phelpbox->iFirstChar)
            {
              phelpbox->iFirstChar--;
              int_dlg_draw_inner(pelementElement);
              SET_HELP_SCROLL;
              bRedrawLink = TRUE;
            }                          
          peventEvent->uiKind = E_DONE;
          break;
        case K_RIGHT :
          if (phelpbox->coordCursor.iX < phelpbox->reg.iWidth - 3)
          {
            phelpbox->coordCursor.iX++;
            SET_HELP_CURSOR;
          }
          else
            if (phelpbox->iFirstChar < 100)
            {
              phelpbox->iFirstChar++;
              int_dlg_draw_inner(pelementElement);
              SET_HELP_SCROLL;
              bRedrawLink = TRUE;
            }                          
          peventEvent->uiKind = E_DONE;
          break;
        case K_UP :
          if (phelpbox->coordCursor.iY)
          {
            phelpbox->coordCursor.iY--;
            SET_HELP_CURSOR;
          }
          else
            if (phelpbox->iFirstLine)
            {
              phelpbox->iFirstLine--;
              int_dlg_draw_inner(pelementElement);
              SET_HELP_SCROLL;
              bRedrawLink = TRUE;
            }                          
          peventEvent->uiKind = E_DONE;
          break;
        case K_PGUP :
          phelpbox->iFirstLine =
            max(phelpbox->iFirstLine - phelpbox->reg.iHeight + phelpbox->pctxContext->swFreeze, 0);
          bRedrawLink = TRUE;
          int_dlg_draw_inner(pelementElement);
          SET_HELP_CURSOR;
          SET_HELP_SCROLL;
          peventEvent->uiKind = E_DONE;
          break;
        case K_DOWN :
          if (phelpbox->iFirstLine + phelpbox->coordCursor.iY == phelpbox->pctxContext->swLines - 1)
            break;
          if (phelpbox->coordCursor.iY < phelpbox->reg.iHeight - 3)
          {
            phelpbox->coordCursor.iY++;
            SET_HELP_CURSOR;
          }
          else
          {
            phelpbox->iFirstLine++;
            int_dlg_draw_inner(pelementElement);
            SET_HELP_SCROLL;
            bRedrawLink = TRUE;
          }                           
          peventEvent->uiKind = E_DONE;
          break;
        case K_PGDN :
          phelpbox->iFirstLine =
               min(phelpbox->iFirstLine + phelpbox->reg.iHeight - phelpbox->pctxContext->swFreeze,
                   phelpbox->pctxContext->swLines - 1);
          bRedrawLink = TRUE;
          int_dlg_draw_inner(pelementElement);
          SET_HELP_CURSOR;
          SET_HELP_SCROLL;
          peventEvent->uiKind = E_DONE;
          break;
        case K_HOME :
          phelpbox->coordCursor.iX = 0;
          SET_HELP_CURSOR;
          if (phelpbox->iFirstChar)
          {
            phelpbox->iFirstChar = 0;
            int_dlg_draw_inner(pelementElement);
            SET_HELP_SCROLL;
            bRedrawLink = TRUE;
          }                            
          peventEvent->uiKind = E_DONE;
          break;
        case K_END :
          iPos = strlen(phelpbox->pctxContext->ppcText[phelpbox->iFirstLine + phelpbox->coordCursor.iY]);
          if (phelpbox->coordCursor.iX + phelpbox->iFirstChar == iPos)
            break;
          if (iPos > phelpbox->reg.iWidth - 3)
          {
            phelpbox->coordCursor.iX = phelpbox->reg.iWidth - 3;
            phelpbox->iFirstChar = iPos - phelpbox->coordCursor.iX;
          }
          else
          {
            phelpbox->coordCursor.iX = iPos;
            phelpbox->iFirstChar = 0;
          }
          int_dlg_draw_inner(pelementElement);
          SET_HELP_CURSOR;  
          SET_HELP_SCROLL;
          bRedrawLink = TRUE;         
          peventEvent->uiKind = E_DONE;
          break;
        case K_SPACE :
          if (phelpbox->plnkActLink)
          {
            plnkNewLink = phelpbox->plnkActLink;
            glb_send_message(pelementElement, pelementElement, M_SET_VALUES, plnkNewLink->dwContext);
            glb_send_message(pelementElement, RECIPIENT, M_HELP_CONTEXT_CHANGED, plnkNewLink->dwContext);
          }                
          peventEvent->uiKind = E_DONE;
          break;
      }
      break;
    case E_MSM_L_UP :
      phelpbox->bDragging = FALSE;
      eventEvent.iHor -= phelpbox->reg.iX;
      eventEvent.iVer -= phelpbox->reg.iY;
      if (eventEvent.iHor < 1 || eventEvent.iHor >= phelpbox->reg.iWidth - 1 ||
          eventEvent.iVer < 1 || eventEvent.iVer >= phelpbox->reg.iHeight - 1)
        break;
      if (phelpbox->plnkActLink)
      {
        DWORD dwNewContext = phelpbox->plnkActLink->dwContext;
        glb_send_message(pelementElement, pelementElement, M_SET_VALUES, dwNewContext);
        glb_send_message(pelementElement, RECIPIENT, M_HELP_CONTEXT_CHANGED, dwNewContext);
      }
      peventEvent->uiKind = E_DONE;
      break;
    case E_MSM_L_DOWN :
      phelpbox->bDragging = FALSE;
      eventEvent.iHor -= phelpbox->reg.iX;
      eventEvent.iVer -= phelpbox->reg.iY;
      if (eventEvent.iHor < 1 || eventEvent.iHor >= phelpbox->reg.iWidth - 1 ||
          eventEvent.iVer < 1 || eventEvent.iVer >= phelpbox->reg.iHeight - 1)
        break;
      phelpbox->coordCursor.iX = eventEvent.iHor - 1;
      phelpbox->coordCursor.iY = min(eventEvent.iVer - 1,
                                     phelpbox->pctxContext->swLines - phelpbox->iFirstLine - 1);
      SET_HELP_CURSOR;
      phelpbox->bDragging = TRUE;
      peventEvent->uiKind = E_DONE;
      break;
    case E_MSM_MOVE :
      if (!phelpbox->bDragging)
        break;
      eventEvent.iHor -= phelpbox->reg.iX;
      eventEvent.iVer -= phelpbox->reg.iY;
      if (eventEvent.iHor < 1 || eventEvent.iHor >= phelpbox->reg.iWidth - 1 ||
          eventEvent.iVer < 1 || eventEvent.iVer >= phelpbox->reg.iHeight - 1)
        break;
      phelpbox->coordCursor.iX = eventEvent.iHor - 1;
      phelpbox->coordCursor.iY = min(eventEvent.iVer - 1,
                                     phelpbox->pctxContext->swLines - phelpbox->iFirstLine - 1);
      SET_HELP_CURSOR;
      break;
  }
  plnkNewLink = int_dlg_check_link(pelementElement);
  if (plnkNewLink != phelpbox->plnkActLink)
  {
    if (phelpbox->plnkActLink)
      int_dlg_hide_link(pelementElement, phelpbox->plnkActLink);
    phelpbox->plnkActLink = plnkNewLink;
    if (phelpbox->plnkActLink)
      int_dlg_show_link(pelementElement, phelpbox->plnkActLink);
  }
  else
    if (bRedrawLink)
      int_dlg_show_link(pelementElement, phelpbox->plnkActLink);
  if (!(eventEvent.wButtons & MSM_B_LEFT))
    phelpbox->bDragging = FALSE;
}

DLG_ELEMENT* dlg_init_help_box(INT iX, INT iY, INT iWidth, INT iHeight, DWORD dwContext, BOOL bBorder,
                               CHAR *pcHelpLine, UINT uiID, BOOL bCanBeActivated, void *pRecipient)
{
  DLG_ELEMENT  *pelementReturnVar;
  DLG_HELP_BOX *phelpbox;
  HLP_CONTEXT  *pctxContext;

  if (!(pctxContext = hlp_get_context(dwContext)))
    return NULL;
  if (!(pelementReturnVar = utl_alloc(sizeof(DLG_ELEMENT))))
    return NULL;
  pelementReturnVar->handle_event = dlg_help_box_handle_event;
  pelementReturnVar->pcHelpLine = pcHelpLine;                    
  pelementReturnVar->uiID = uiID;
  pelementReturnVar->flFlags.binCanBeActivated = bCanBeActivated;
  pelementReturnVar->flFlags.binFocussed = FALSE;
  pelementReturnVar->pRecipient = pRecipient;
  if (!(pelementReturnVar->pAddInfo = utl_alloc(sizeof(DLG_HELP_BOX))))
    return NULL;
  phelpbox = (DLG_HELP_BOX*)(pelementReturnVar->pAddInfo);
  phelpbox->reg.iX = iX;
  phelpbox->reg.iY = iY;
  phelpbox->reg.iWidth = iWidth;
  phelpbox->reg.iHeight = iHeight;
  phelpbox->coordCursor.iX = phelpbox->coordCursor.iY = 0;
  phelpbox->iFirstLine = phelpbox->iFirstChar = 0;
  phelpbox->bDragging = FALSE;
  phelpbox->dwContext = dwContext;
  if (!(phelpbox->pcharBuffer = utl_alloc(sizeof(CHARACTER) * iWidth)))
    return NULL;
  phelpbox->plnkActLink = NULL;
  phelpbox->iHC = 100 / (iWidth - 4);
  phelpbox->iVC = pctxContext->swLines / (iHeight - 4);
  phelpbox->pctxContext = pctxContext;
  if (bBorder)
    phelpbox->pelementBorder = dlg_init_border(iX, iY, iWidth, iHeight, uiID + 30000);
  else
    phelpbox->pelementBorder = NULL;
  phelpbox->pelementScrollH = dlg_init_scroll(iX + 1, iY + iHeight - 1, iWidth - 4, 0, DLG_HORIZONTAL, NULL,
                                              uiID + 10000, FALSE, pelementReturnVar);
  phelpbox->pelementScrollV = dlg_init_scroll(iX + iWidth - 1, iY + 1, iHeight - 4, 0, DLG_VERTICAL, NULL,
                                              uiID + 20000, FALSE, pelementReturnVar);
  return pelementReturnVar;
}

/*
Local Variables:
compile-command: "wmake -f stools.mk -h -e"
End:
*/
