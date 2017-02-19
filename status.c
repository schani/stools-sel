/***************************************************************************
 ***************************************************************************
 ***                                                                     ***
 ***                       Schani-Tools fÅr C                            ***
 ***                                                                     ***
 ***                       Statuszeilen-Teil                             ***
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
#include <string.h>

void  static int_sts_display_help_line   (void);
INT   static int_sts_get_item_x          (INT);
void  static int_sts_display_item        (CHAR*, INT, BOOL);
void  static int_sts_display_active_item (CHAR*, INT);
void  static int_sts_display_items       (void);
INT   static int_sts_get_index           (INT);

extern WINDOW      winDesktop;
extern GLB_PROGRAM prgProgram;
extern INT         iSizeX;
extern INT         iSizeY;

CHAR           acCurHelpLine[80]        = "";
BOOL           bHelpLine                = FALSE;
INT            iStackPos                = -1;
STS_ITEM      *pitemActItems            = NULL;
STS_LINE_SAVE  alnStack[STS_STACK_SIZE];

/***************************************************************************
 *                       Interne Funktionen                                *
 ***************************************************************************/

void static int_sts_display_help_line (void)
{
  BOOL bMouse;

  dsk_sw_za(1, iSizeY, iSizeX, 1, ' ', dsk_get_color(PAL_COL_DSK_STATUS_LINE));
  bMouse = msm_cursor_off();
  dsk_ss(1, iSizeY, acCurHelpLine);
  if (bMouse)
    msm_cursor_on();
}

INT static int_sts_get_item_x (INT iIndex)
{
  INT iCounter,
      iCounterX = 2;

  for (iCounter = 0; iCounter < iIndex; iCounter++)
    iCounterX += utl_hot_strlen(pitemActItems[iCounter].pcText) + 2;
  return iCounterX;
}

void static int_sts_display_item (CHAR *pcText, INT iPos, BOOL bActive)
{
  BOOL bMouse;

  bMouse = msm_cursor_off();
  dsk_sw_a(iPos - 1, iSizeY, utl_hot_strlen(pcText) + 2, 1, dsk_get_color(PAL_COL_DSK_STATUS_LINE));
  if (bActive)   
    dsk_write_hot(iPos, iSizeY, pcText, dsk_get_color(PAL_COL_DSK_STATUS_ACT_ITEM),
                  dsk_get_color(PAL_COL_DSK_STATUS_HOT_KEY));
  else
    dsk_write_hot(iPos, iSizeY, pcText, dsk_get_color(PAL_COL_DSK_STATUS_INACT_ITEM),
                  dsk_get_color(PAL_COL_DSK_STATUS_INACT_ITEM));
  if (bMouse)
    msm_cursor_on();
}

void static int_sts_display_active_item (CHAR *pcText, INT iPos)
{
  BOOL bMouse;

  bMouse = msm_cursor_off();
  dsk_sw_a(iPos - 1, iSizeY, utl_hot_strlen(pcText) + 2, 1, dsk_get_color(PAL_COL_DSK_STATUS_SEL_ACT_ITEM));
  dsk_write_hot(iPos, iSizeY, pcText, dsk_get_color(PAL_COL_DSK_STATUS_SEL_ACT_ITEM),
                dsk_get_color(PAL_COL_DSK_STATUS_SEL_HOT_KEY));
  if (bMouse)
    msm_cursor_on();
}

void static int_sts_display_items (void)
{
  STS_ITEM *pitemCounter;
  INT       iActPos      = 2;

  dsk_sw_za(1, iSizeY, iSizeX, 1, ' ', dsk_get_color(PAL_COL_DSK_STATUS_LINE));
  if (pitemActItems)
    for (pitemCounter = pitemActItems; pitemCounter->pcText; pitemCounter++)
    {
      int_sts_display_item(pitemCounter->pcText, iActPos, pitemCounter->bActive);
      iActPos += utl_hot_strlen(pitemCounter->pcText) + 2;
    }
}

INT static int_sts_get_index (INT iX)
{
  INT iCounter,
      iCounterX = 1;

  for (iCounter = 0; pitemActItems[iCounter].pcText; iCounter++)
    if (iX >= iCounterX && iX < iCounterX + utl_hot_strlen(pitemActItems[iCounter].pcText) + 2)
      return iCounter;
    else
      iCounterX += utl_hot_strlen(pitemActItems[iCounter].pcText) + 2;
  return -1;
}

/***************************************************************************
 *                       Allgemeine Funktionen                             *
 ***************************************************************************/

void sts_write_help (CHAR *pcHelpLine)
{
  if (bHelpLine && !strcmp(pcHelpLine, acCurHelpLine))
    return;
  bHelpLine = TRUE;
  strcpy(acCurHelpLine, pcHelpLine);
  int_sts_display_help_line();
}

void sts_clear_help (void)
{
  bHelpLine = FALSE;
  if (pitemActItems)
    int_sts_display_items();
  else
    dsk_sw_za(1, iSizeY, iSizeX, 1, ' ', dsk_get_color(PAL_COL_DSK_STATUS_LINE));
}

BOOL sts_new_status_line (STS_ITEM *pitemItems)
{
  CHAR *pcBuffer;

  if (iStackPos < STS_STACK_SIZE - 1)
  {
    if (iStackPos != -1 || pitemActItems || bHelpLine)
    {
      iStackPos++;
      alnStack[iStackPos].bHelpLine = bHelpLine;
      if (!(pcBuffer = utl_alloc(strlen(acCurHelpLine) + 1)))
        return FALSE;
      strcpy(pcBuffer, acCurHelpLine);
      alnStack[iStackPos].pcHelpLine = pcBuffer;
      alnStack[iStackPos].pitemItems = pitemActItems;
    }
    bHelpLine = FALSE;
    pitemActItems = pitemItems;
    int_sts_display_items();
  }
  else
    return FALSE;
  return TRUE;
}

void sts_del_status_line (void)
{
  if (iStackPos != -1)
  {
    strcpy(acCurHelpLine, alnStack[iStackPos].pcHelpLine);
    utl_free(alnStack[iStackPos].pcHelpLine);
    bHelpLine = alnStack[iStackPos].bHelpLine;
    pitemActItems = alnStack[iStackPos].pitemItems;
    iStackPos--;
  }
  else
  {
    bHelpLine = FALSE;
    pitemActItems = NULL;
  }
  if (bHelpLine)
    int_sts_display_help_line();
  else
    int_sts_display_items();
}       

void sts_pre_process (UTL_EVENT *peventEvent)
{
  UTL_EVENT  eventEvent;
  STS_ITEM  *pitemCounter;

  if (!pitemActItems || bHelpLine)
    return;   
  if (peventEvent->uiKind == E_KEY)
    for (pitemCounter = pitemActItems; pitemCounter->pcText; pitemCounter++)
      if (peventEvent->wKey == pitemCounter->wHotKey)
        if (pitemCounter->bActive)
        {
          utl_fill_event(&eventEvent);
          eventEvent.uiKind = E_MESSAGE;
          eventEvent.uiMessage = pitemCounter->uiID;
          win_handle_event(prgProgram.winFocussed, &eventEvent);
          peventEvent->uiKind = E_DONE;
          return;
        }             
}

void sts_event (UTL_EVENT *peventEvent)
{
  UTL_EVENT  eventEvent;
  INT        iIndex,
             iX;
  BOOL       bCursor;

  switch (peventEvent->uiKind)
  {
    case E_MSM_L_DOWN :
      if (!pitemActItems || bHelpLine)
        break;
      if (peventEvent->iVer == iSizeY && (iIndex = int_sts_get_index(peventEvent->iHor)) != -1)
      {
        iX = int_sts_get_item_x(iIndex);
        int_sts_display_active_item(pitemActItems[iIndex].pcText, iX);
        bCursor = TRUE;
        do
        {
          utl_event(&eventEvent);
          switch (eventEvent.uiKind)
          {
            case E_MSM_MOVE :
              if (eventEvent.iVer != iSizeY)
              {
                if (bCursor)
                {
                  bCursor = FALSE;
                  int_sts_display_item(pitemActItems[iIndex].pcText, iX, pitemActItems[iIndex].bActive);
                }
                break;
              }
              if (iIndex == int_sts_get_index(eventEvent.iHor) && !bCursor)
              {
                bCursor = TRUE;
                int_sts_display_active_item(pitemActItems[iIndex].pcText, iX);
                break;
              }
              if (iIndex != int_sts_get_index(eventEvent.iHor) && bCursor)
              {
                bCursor = FALSE;
                int_sts_display_item(pitemActItems[iIndex].pcText, iX, pitemActItems[iIndex].bActive);
              }
              break;
            case E_MSM_L_UP :
              int_sts_display_item(pitemActItems[iIndex].pcText, iX, pitemActItems[iIndex].bActive);
              if (bCursor)
              {
                eventEvent.uiKind = E_MESSAGE;
                eventEvent.uiMessage = pitemActItems[iIndex].uiID;
                win_handle_event(prgProgram.winFocussed, &eventEvent);
              }
              peventEvent->uiKind = E_DONE;
              return;
          }
        } while (TRUE);
      }
      break;
  }
}

void sts_redraw (void)
{
  dsk_sw_za(1, iSizeY, iSizeX, 1, ' ', dsk_get_color(PAL_COL_DSK_STATUS_LINE));
  if (bHelpLine)
    int_sts_display_help_line();
  else
    if (pitemActItems)
      int_sts_display_items();
}

BOOL sts_active (UINT uiID, UCHAR ucStatus)
{
  STS_ITEM *pitemItem;

  if (!pitemActItems)
    return FALSE;
  pitemItem = pitemActItems;
  while (pitemItem->pcText && uiID != pitemItem->uiID)
    pitemItem++;
  if (pitemItem->pcText)
  {
    if (ucStatus == FLIP)
      pitemItem->bActive = !pitemItem->bActive;
    else
      pitemItem->bActive = ucStatus;
    if (!bHelpLine)
      int_sts_display_items();
    return TRUE;
  }
  return FALSE;
}

/*
Local Variables:
compile-command: "wmake -f stools.mk -h -e"
End:
*/
