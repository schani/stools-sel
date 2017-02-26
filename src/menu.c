/***************************************************************************
 ***************************************************************************
 ***                                                                     ***
 ***                        Schani-Tools fÅr C                           ***
 ***                                                                     ***
 ***                            MenÅ-Teil                                ***
 ***                                                                     ***
 ***                (c) 1990-93 by Schani Electronics                    ***
 ***                                                                     ***
 ***************************************************************************
 ***************************************************************************/

#include <gemein.h>
#include <video.h>
#include <utility.h>
#include <contain.h>
#include <menu.h>
#include <status.h>
#include <sthelp.h>
#include <dlg.h>
#include <window.h>
#include <global.h>
#include <mouse.h>
#include <stdlib.h>
#include <string.h>

static void        int_mnu_write_menu_bar  (void);
static void        int_mnu_del_item        (MNU_ITEM*);
static void        int_mnu_del_window      (MNU_WINDOW*);
static BOOL        int_mnu_search_item     (MNU_ITEM*, WORD);
static BOOL        int_mnu_search_window   (MNU_WINDOW*, WORD);
static void        int_mnu_draw_item       (MNU_WINDOW*, WINDOW, BOOL);
static void        int_mnu_display_window  (MNU_WINDOW*, INT, INT);
static MNU_WINDOW* int_mnu_open_window     (MNU_WINDOW*, CNT_STACK*);
static MNU_ITEM*   int_mnu_check_item      (MNU_WINDOW*, WINDOW, UTL_EVENT*);
static BOOL        int_mnu_is_tear_down    (WINDOW, ULONG);
static void        int_mnu_make_tear_down  (MNU_WINDOW*, INT, INT);
static BOOL        int_mnu_tear_down_event (UTL_EVENT*, MNU_WINDOW*);
static BOOL        int_mnu_check_tear_down (WINDOW, ULONG);
static MNU_ITEM*   int_mnu_find_item       (MNU_WINDOW*, UINT, MNU_WINDOW**);

extern WINDOW      winDesktop;
extern GLB_PROGRAM prgProgram;    
extern BOOL        bUpdating;
extern ULONG       ulDoubleClick;

MNU_MENU *pmenuLast = NULL;
 
static void int_mnu_write_menu_bar (void)
{
  MNU_ITEM *pitemCounterRight,
           *pitemCounterLeft;
  INT       iCounter          = 0,
            iX;
    
  dsk_sw_za(1, 1, 80, 1, ' ', dsk_get_color(PAL_COL_DSK_MENU_BAR));
  if (!pmenuLast)
    return;              
  if (pmenuLast->iRight > 0)
  {
    for (pitemCounterRight = pmenuLast->pwindowWindow->pitemLast, iX = 80;
         iCounter < pmenuLast->iRight && pitemCounterRight;
         pitemCounterRight = pitemCounterRight->pitemPrev, iCounter++)
    {
      iX -= utl_hot_strlen(pitemCounterRight->pcText) + 2;
      if (pitemCounterRight->bActive)
        dsk_write_hot(iX + 1, 1, pitemCounterRight->pcText,
                      dsk_get_color(PAL_COL_DSK_MENU_ACT_ITEM),
                      dsk_get_color(PAL_COL_DSK_MENU_HOT_KEY));
      else
        dsk_write_hot(iX + 1, 1, pitemCounterRight->pcText,
                      dsk_get_color(PAL_COL_DSK_MENU_INACT_ITEM),
                      dsk_get_color(PAL_COL_DSK_MENU_INACT_ITEM));
      pitemCounterRight->iX = iX;
    }
    if (!pitemCounterRight)
      return;              
    pitemCounterRight = pitemCounterRight->pitemNext;
  }
  else
    pitemCounterRight = NULL;
  for (pitemCounterLeft = pmenuLast->pwindowWindow->pitemFirst, iX = 2;
       pitemCounterLeft != pitemCounterRight;
       pitemCounterLeft = pitemCounterLeft->pitemNext)
  {
    if (pitemCounterLeft->bActive)
      dsk_write_hot(iX + 1, 1, pitemCounterLeft->pcText,
                    dsk_get_color(PAL_COL_DSK_MENU_ACT_ITEM),
                    dsk_get_color(PAL_COL_DSK_MENU_HOT_KEY));
    else
      dsk_write_hot(iX + 1, 1, pitemCounterLeft->pcText,
                    dsk_get_color(PAL_COL_DSK_MENU_INACT_ITEM),
                    dsk_get_color(PAL_COL_DSK_MENU_INACT_ITEM));
    pitemCounterLeft->iX = iX;
    iX += utl_hot_strlen(pitemCounterLeft->pcText) + 2;        
  }                     
}
 
static void int_mnu_del_item (MNU_ITEM *pitemItem)
{                 
  if (pitemItem->pitemNext)
    int_mnu_del_item(pitemItem->pitemNext);
  if (pitemItem->pwindowWindow)
    int_mnu_del_window(pitemItem->pwindowWindow);
  utl_free(pitemItem);
}               

static void int_mnu_del_window (MNU_WINDOW *pwindowWindow)
{
  int_mnu_del_item(pwindowWindow->pitemFirst);
  utl_free(pwindowWindow);
}

static BOOL int_mnu_search_item (MNU_ITEM *pitemItem, WORD wKey)
{
  UTL_EVENT *peventEvent;
  
  if (pitemItem->wHotKey == wKey)
  {
    peventEvent = utl_alloc(sizeof(UTL_EVENT));
    utl_fill_event(peventEvent);
    peventEvent->uiKind = E_MESSAGE;
    peventEvent->uiMessage = pitemItem->uiMessage;
    win_handle_event(prgProgram.winFocussed, peventEvent);
    return TRUE;
  }
  if (pitemItem->pwindowWindow)
    if (int_mnu_search_window(pitemItem->pwindowWindow, wKey))
      return TRUE;
  if (pitemItem->pitemNext)
    if (int_mnu_search_item(pitemItem->pitemNext, wKey))
      return TRUE;
  return FALSE;
}

static BOOL int_mnu_search_window (MNU_WINDOW *pwindowWindow, WORD wKey)
{
  if (int_mnu_search_item(pwindowWindow->pitemFirst, wKey))
    return TRUE;
  return FALSE;
}    

static void int_mnu_draw_item (MNU_WINDOW *pwindowWindow, WINDOW winWindow, BOOL bMarked)
{
  INT  iX,
       iY,
       iWidth,
       iActItem      = PAL_COL_DSK_MENU_ACT_ITEM,
       iHotKey       = PAL_COL_DSK_MENU_HOT_KEY,
       iSelActItem   = PAL_COL_DSK_MENU_SEL_ACT_ITEM,
       iSelHotKey    = PAL_COL_DSK_MENU_SEL_HOT_KEY,
       iInactItem    = PAL_COL_DSK_MENU_INACT_ITEM,
       iSelInactItem = PAL_COL_DSK_MENU_SEL_INACT_ITEM;
  BOOL bMenuBar      = FALSE;

  if (pwindowWindow == pmenuLast->pwindowWindow)
  {                
    winWindow = winDesktop;
    iX = pwindowWindow->pitemActive->iX;
    iY = 1;    
    iWidth = utl_hot_strlen(pwindowWindow->pitemActive->pcText) + 2;
    bMenuBar = TRUE;
  }
  else
  {
    iX = 1;
    iY = pwindowWindow->pitemActive->iLine;
    iWidth = pwindowWindow->iWidth - 2;
    iActItem = PAL_COL_WIN_ACT_ITEM;
    iHotKey = PAL_COL_WIN_HOT_KEY;
    iSelActItem = PAL_COL_WIN_SEL_ACT_ITEM;
    iSelHotKey = PAL_COL_WIN_SEL_HOT_KEY;
    iInactItem = PAL_COL_WIN_INACT_ITEM;
    iSelInactItem = PAL_COL_WIN_SEL_INACT_ITEM;
  }                             
  if (pwindowWindow->pitemActive->uiMessage == M_MNU_LINE)
  {
    win_sw_za(winWindow, iX, iY, iWidth, 1, 'ƒ', win_get_color(winWindow, PAL_COL_BORDER));
    return;
  }
  if (bMarked)
  {
    if (pwindowWindow->pitemActive->bActive)
    {
      win_sw_a(winWindow, iX, iY, iWidth, 1, win_get_color(winWindow, iSelActItem));
      win_write_hot(winWindow, iX + 1, iY,
                    pwindowWindow->pitemActive->pcText,
                    win_get_color(winWindow, iSelActItem),
                    win_get_color(winWindow, iSelHotKey));
    }
    else
    {
      win_sw_a(winWindow, iX, iY, iWidth, 1, win_get_color(winWindow, iSelInactItem));
      win_write_hot(winWindow, iX + 1, iY,
                    pwindowWindow->pitemActive->pcText,
                    win_get_color(winWindow, iSelInactItem),
                    win_get_color(winWindow, iSelInactItem));
    }
  }
  else
  {
    if (pwindowWindow->pitemActive->bActive)
    {
      win_sw_a(winWindow, iX, iY, iWidth, 1,
               win_get_color(winWindow, iActItem));
      win_write_hot(winWindow, iX + 1, iY,
                    pwindowWindow->pitemActive->pcText,
                    win_get_color(winWindow, iActItem),
                    win_get_color(winWindow, iHotKey));
    }
    else
    {
      win_sw_a(winWindow, iX, iY, iWidth, 1,
               win_get_color(winWindow, iInactItem));
      win_write_hot(winWindow, iX + 1, iY,
                    pwindowWindow->pitemActive->pcText,
                    win_get_color(winWindow, iInactItem),
                    win_get_color(winWindow, iInactItem));
    }
  }
  if (pwindowWindow->pitemActive->pwindowWindow && !bMenuBar)
    win_s_z(winWindow, pwindowWindow->iWidth - 3, iY, '');
}

static void int_mnu_display_window (MNU_WINDOW *pwindowWindow, INT iX, INT iY)
{
  MNU_ITEM *pitemBuffer;

  pwindowWindow->winWindow = win_new(iX, iY, pwindowWindow->iWidth, pwindowWindow->iHeight, "pull_down", 0);
  win_cls(pwindowWindow->winWindow);
  glb_send_message(&prgProgram, pwindowWindow->winWindow, M_DRAW, 0);
  pitemBuffer = pwindowWindow->pitemActive;
  for (pwindowWindow->pitemActive = pwindowWindow->pitemFirst;
       pwindowWindow->pitemActive;
       pwindowWindow->pitemActive = pwindowWindow->pitemActive->pitemNext)
    int_mnu_draw_item(pwindowWindow, pwindowWindow->winWindow,
                      pwindowWindow->pitemActive == pitemBuffer);
  pwindowWindow->pitemActive = pitemBuffer;
  win_show(pwindowWindow->winWindow);
}

static MNU_WINDOW* int_mnu_open_window (MNU_WINDOW *pwindowWindow, CNT_STACK *psStack)
{
  INT iX,
      iY;

  if (pwindowWindow == pmenuLast->pwindowWindow)
  {
    iX = pwindowWindow->pitemActive->iX - 1;
    iY = 2;
  }
  else
  {
    iX = pwindowWindow->winWindow->iX + 1;
    iY = pwindowWindow->winWindow->iY + pwindowWindow->pitemActive->iLine + 1;
  }                                       
  pwindowWindow = pwindowWindow->pitemActive->pwindowWindow;
  iX = min(iX, 80 - pwindowWindow->iWidth);
  iY = min(iY, 26 - pwindowWindow->iHeight);
  cnt_stack_put(psStack, (ULONG)pwindowWindow);
  int_mnu_display_window(pwindowWindow, iX, iY);
  return(pwindowWindow);
}

static MNU_ITEM* int_mnu_check_item (MNU_WINDOW *pwindowWindow, WINDOW winWindow, UTL_EVENT *peventEvent)
{
  MNU_ITEM *pitemItem;
            
  switch (peventEvent->uiKind)
  {
    case E_MSM_L_DOWN :
    case E_MSM_L_UP :
    case E_MSM_L_DOUBLE :
    case E_MSM_MOVE :
      if (pwindowWindow == pmenuLast->pwindowWindow)
      {
        if (peventEvent->iVer != 1)
          return NULL;
        for (pitemItem = pwindowWindow->pitemFirst; pitemItem;
             pitemItem = pitemItem->pitemNext)
          if (peventEvent->iHor >= pitemItem->iX &&
              peventEvent->iHor <=
                pitemItem->iX + utl_hot_strlen(pitemItem->pcText) + 1)
            return pitemItem;
      }
      else
      {
        if (IN_WINDOW(peventEvent->iHor, peventEvent->iVer, winWindow))
        {
          if (ON_BORDER(peventEvent->iHor, peventEvent->iVer, winWindow))
            return pwindowWindow->pitemActive;
          for (pitemItem = pwindowWindow->pitemFirst; pitemItem;
               pitemItem = pitemItem->pitemNext)
            if (peventEvent->iVer - winWindow->iY == pitemItem->iLine)
              return pitemItem;
        }
      } 
      break;
    case E_KEY :
      for (pitemItem = pwindowWindow->pitemFirst; pitemItem;
           pitemItem = pitemItem->pitemNext)
        if (peventEvent->wKey == pitemItem->wHotKey ||
            dlg_hot_key(peventEvent, pitemItem->cDispHotKey))
          return pitemItem;
      break;
  }
  return NULL;
}          

static BOOL int_mnu_is_tear_down (WINDOW winWindow, ULONG ulAddInfo)
{
  if (strcmp(win_get_class(winWindow), "tear_down"))
    return FALSE;
  if (((MNU_TEAR_DOWN*)win_get_add_info(winWindow))->pwindowWindow == (MNU_WINDOW*)ulAddInfo)
    return TRUE;
  return FALSE;
}

static void int_mnu_make_tear_down (MNU_WINDOW *pwindowWindow, INT iX, INT iY)
{
  WINDOW         winWindow;
  MNU_TEAR_DOWN *ptdWindow;
                                              
  if (win_find(int_mnu_is_tear_down, (ULONG)pwindowWindow))
    return;
  ptdWindow = utl_alloc(sizeof(MNU_TEAR_DOWN));
  ptdWindow->pwindowWindow = pwindowWindow;
  ptdWindow->bDragging = FALSE;
  winWindow = win_new(iX, iY, pwindowWindow->iWidth, pwindowWindow->iHeight, "tear_down", (ULONG)ptdWindow);
  win_cls(winWindow);
  glb_send_message(&prgProgram, winWindow, M_DRAW, 0);
  win_show(winWindow);
}                                               

static BOOL int_mnu_tear_down_event (UTL_EVENT *peventEvent, MNU_WINDOW *pwindowWindow)
{
  INT iX,
      iY;  
         
  iX = peventEvent->iHor - pwindowWindow->winWindow->iX;
  iY = peventEvent->iVer - pwindowWindow->winWindow->iY;
  if (iX >= 2 && iX <= 4 && iY == 0)
    return TRUE;
  return FALSE;
}

static BOOL int_mnu_check_tear_down (WINDOW winWindow, ULONG ulWindow)
{
  MNU_ITEM *pitemCounter;

  if ((MNU_WINDOW*)ulWindow == ((MNU_TEAR_DOWN*)win_get_add_info(winWindow))->pwindowWindow)
    return TRUE;
  for (pitemCounter = ((MNU_WINDOW*)ulWindow)->pitemFirst; pitemCounter;
       pitemCounter = pitemCounter->pitemNext)           
    if (pitemCounter->pwindowWindow)
      if (int_mnu_check_tear_down(winWindow, (ULONG)pitemCounter->pwindowWindow))
        return TRUE;
  return FALSE;
} 

static MNU_ITEM* int_mnu_find_item (MNU_WINDOW *pwindowWindow, UINT uiMessage, MNU_WINDOW **ppwindowFound)
{
  MNU_ITEM *pitemCounter,
           *pitemItem;

  for (pitemCounter = pwindowWindow->pitemFirst; pitemCounter;
       pitemCounter = pitemCounter->pitemNext)
  {
    if (pitemCounter->uiMessage == uiMessage)
    {
      *ppwindowFound = pwindowWindow;
      return pitemCounter;
    }
    else                               
      if (pitemCounter->pwindowWindow)
        if ((pitemItem = int_mnu_find_item(pitemCounter->pwindowWindow, uiMessage, ppwindowFound)))
          return pitemItem;
  }
  return NULL;
}

void mnu_init (void)
{
  win_register_class("pull_down", PAL_WIN_MENU, B_EEEE, TITLE_T_C,
                     WIN_FL_SHADOW | WIN_FL_MODAL | WIN_FL_FOCUSABLE, mnu_window_handler);
  win_register_class("tear_down", PAL_WIN_MENU, B_EEEE, TITLE_T_C,
                     WIN_FL_SHADOW | WIN_FL_MOVEABLE | WIN_FL_CLOSEABLE | WIN_FL_ON_TOP, mnu_tear_down_handler);
}

void mnu_redraw (void)
{
  int_mnu_write_menu_bar();
}

void mnu_window_handler (WINDOW winWindow, UTL_EVENT *peventEvent)
{
  BOOL bClipping;
                            
  switch (peventEvent->uiMessage)                          
  {
    case M_DRAW :      
    case M_REDRAW :
      bClipping = win_clipping(winWindow, FALSE);
      win_ss_a(winWindow, 2, 0, "()", win_get_color(winWindow, PAL_COL_ICON));
      win_clipping(winWindow, bClipping);
      peventEvent->uiKind = E_DONE;
      break;
  }
}             

void mnu_tear_down_handler (WINDOW winWindow, UTL_EVENT *peventEvent)
{
  MNU_TEAR_DOWN *ptdWindow;                                            
  MNU_WINDOW    *pwindowWindow;
  MNU_ITEM      *pitemBuffer;
  UTL_EVENT      eventEvent;
  
  ptdWindow = (MNU_TEAR_DOWN*)win_get_add_info(winWindow);
  pwindowWindow = ptdWindow->pwindowWindow;
  if (!(peventEvent->wButtons & MSM_B_LEFT))
    ptdWindow->bDragging = FALSE;
  switch (peventEvent->uiKind)
  {
    case E_MESSAGE :
      switch (peventEvent->uiMessage)
      {
        case M_DRAW :
        case M_REDRAW :
          pitemBuffer = pwindowWindow->pitemActive;
          for (pwindowWindow->pitemActive = pwindowWindow->pitemFirst;
               pwindowWindow->pitemActive;
               pwindowWindow->pitemActive =
                 pwindowWindow->pitemActive->pitemNext)
            int_mnu_draw_item(pwindowWindow, winWindow,
                              pwindowWindow->pitemActive == pitemBuffer);
          pwindowWindow->pitemActive = pitemBuffer;
          peventEvent->uiKind = E_DONE;
          break;
        case M_QUIT :
          utl_free(ptdWindow);
          peventEvent->uiKind = E_DONE;
          break;
        case M_CLOSE :
          win_delete(winWindow);
          peventEvent->uiKind = E_DONE;
          break;
      }
      break;
    case E_MSM_L_DOWN :
      pitemBuffer = int_mnu_check_item(pwindowWindow, winWindow, peventEvent);
      if (!pitemBuffer)
        return;
      ptdWindow->bDragging = TRUE;
      if (pitemBuffer != pwindowWindow->pitemActive)
      {
        int_mnu_draw_item(pwindowWindow, winWindow, FALSE);
        pwindowWindow->pitemActive = pitemBuffer;
        int_mnu_draw_item(pwindowWindow, winWindow, TRUE);
      } 
      peventEvent->uiKind = E_DONE;
      break;
    case E_MSM_L_DOUBLE :
      ptdWindow->bDragging = FALSE;
      pitemBuffer = int_mnu_check_item(pwindowWindow, winWindow, peventEvent);
      if (!pitemBuffer)
        return;

      peventEvent->uiKind = E_DONE;
      if (pitemBuffer->uiMessage == M_MNU_LINE)
      {
        pwindowWindow->pitemActive = pwindowWindow->pitemFirst;
        int_mnu_draw_item(pwindowWindow, winWindow, TRUE);
        return;
      }
      if (pitemBuffer->bActive)
      {                                           
        if (pitemBuffer->pwindowWindow)
          int_mnu_make_tear_down(pitemBuffer->pwindowWindow, winWindow->iX + 1,
                                 winWindow->iY + pitemBuffer->iLine + 1);
        else
        {
          utl_fill_event(&eventEvent);
          eventEvent.uiKind = E_MESSAGE;
          eventEvent.uiMessage = pitemBuffer->uiMessage;
          win_handle_event(prgProgram.winFocussed, &eventEvent);
        }
      }   
      break;
    case E_MSM_L_UP :
      ptdWindow->bDragging = FALSE;
      peventEvent->uiKind = E_DONE;
      break;  
    case E_MSM_MOVE :
      pitemBuffer = int_mnu_check_item(pwindowWindow, winWindow, peventEvent);
      if (!pitemBuffer)
        return;
      if (pitemBuffer != pwindowWindow->pitemActive && ptdWindow->bDragging)
      {
        int_mnu_draw_item(pwindowWindow, winWindow, FALSE);
        pwindowWindow->pitemActive = pitemBuffer;
        int_mnu_draw_item(pwindowWindow, winWindow, TRUE);
      } 
      peventEvent->uiKind = E_DONE;
      break;
  }
}

void mnu_pre_process (UTL_EVENT *peventEvent)
{                         
  if (!pmenuLast)
    return;                                                         
  if (peventEvent->uiKind != E_KEY)
    return;
  if (int_mnu_search_item(pmenuLast->pwindowWindow->pitemFirst, peventEvent->wKey))
    peventEvent->uiKind = E_DONE;
  else
    mnu_event(peventEvent);
}                                 

void mnu_event (UTL_EVENT *peventEvent)
{        
  static CNT_STACK  *spsStack       = NULL;
  static UINT        suiLookBack;
  static BOOL        sbWorking     = FALSE;
  static MNU_WINDOW *spwindowWindow;
  static WINDOW      swinFocussed;
         BOOL        bWentBack     = FALSE,
                     bSetDone      = FALSE,
                     bDrawWindow;    
         MNU_WINDOW *pwindowWindow;              
         MNU_ITEM   *pitemItem;
         INT         iX,
                     iY;
         UTL_EVENT   eventEvent;
                               
  if (!pmenuLast)
    return;
  if (!spsStack)
    spsStack = cnt_stack_make();
  do
  {                             
    if (bWentBack)
    {
      suiLookBack++;
      spwindowWindow = (MNU_WINDOW*)cnt_stack_look_back(spsStack, suiLookBack);
    }
    switch (peventEvent->uiKind)
    {
      case E_KEY :
        switch (peventEvent->wKey)
        {
          case K_F10 :
            if (!sbWorking)
            {
              sbWorking = TRUE;
              bUpdating = TRUE;
              swinFocussed = prgProgram.winFocussed;
              win_set_focus(winDesktop);
              sts_new_status_line(NULL);
              win_set_working(winDesktop);
              spwindowWindow = pmenuLast->pwindowWindow;
              cnt_stack_put(spsStack, (ULONG)spwindowWindow);
              suiLookBack = 0;
              int_mnu_draw_item(spwindowWindow, spwindowWindow->winWindow, TRUE);
            }
            peventEvent->uiKind = E_DONE;
            break;
          case K_ESC :
            if (!sbWorking)
              return;
            if (spwindowWindow == pmenuLast->pwindowWindow)
            {
              sbWorking = FALSE;
              bUpdating = FALSE;
              sts_del_status_line();
              win_set_focus(swinFocussed);
              win_set_working(NULL);
              int_mnu_draw_item(spwindowWindow, spwindowWindow->winWindow, FALSE);
              cnt_stack_get(spsStack);
            }
            else
            {       
              if (spwindowWindow == pmenuLast->pwindowWindow->pitemActive->pwindowWindow)
              {
                sbWorking = FALSE;
                bUpdating = FALSE;
                win_delete(((MNU_WINDOW*)cnt_stack_get(spsStack))->winWindow);      
                spwindowWindow = (MNU_WINDOW*)cnt_stack_get(spsStack);
                sts_del_status_line();
                win_set_focus(swinFocussed);
                win_set_working(NULL);
                int_mnu_draw_item(spwindowWindow, spwindowWindow->winWindow, FALSE);
              }
              else
              {
                win_delete(spwindowWindow->winWindow);
                cnt_stack_get(spsStack);
                spwindowWindow = (MNU_WINDOW*)cnt_stack_look_back(spsStack, 0);
              }
            }
            peventEvent->uiKind = E_DONE;
            break;
          case K_SPACE :
          case K_ENTER :
            if (!sbWorking)
              return;                    
            peventEvent->uiKind = E_DONE;
            if (!spwindowWindow->pitemActive->bActive)
              break;
            if (spwindowWindow->pitemActive->pwindowWindow)
              spwindowWindow = int_mnu_open_window(spwindowWindow, spsStack);        
            else
            {                         
              cnt_stack_get(spsStack);
              pwindowWindow = spwindowWindow;
              for (; pwindowWindow != pmenuLast->pwindowWindow;
                   pwindowWindow = (MNU_WINDOW*)cnt_stack_get(spsStack))
                win_delete(pwindowWindow->winWindow);
              int_mnu_draw_item(pwindowWindow, pwindowWindow->winWindow, FALSE);
              cnt_stack_get(spsStack);
              sbWorking = FALSE;
              bUpdating = FALSE;
              sts_del_status_line();
              win_set_focus(swinFocussed);
              win_set_working(NULL);
              utl_fill_event(&eventEvent);
              eventEvent.uiKind = E_MESSAGE;
              eventEvent.uiMessage = spwindowWindow->pitemActive->uiMessage;
              win_handle_event(prgProgram.winFocussed, &eventEvent);
            }
            peventEvent->uiKind = E_DONE;
            break;
          case K_UP :
            if (!sbWorking)
              return;
            if (spwindowWindow != pmenuLast->pwindowWindow)
            {
              if (spwindowWindow->pitemFirst != spwindowWindow->pitemLast)
              {                   
                int_mnu_draw_item(spwindowWindow, spwindowWindow->winWindow, FALSE);
                spwindowWindow->pitemActive = spwindowWindow->pitemActive->pitemPrev;
                for (; spwindowWindow->pitemActive->uiMessage == M_MNU_LINE; )
                  spwindowWindow->pitemActive = spwindowWindow->pitemActive->pitemPrev;
                if (!spwindowWindow->pitemActive)
                  spwindowWindow->pitemActive = spwindowWindow->pitemLast;
                int_mnu_draw_item(spwindowWindow, spwindowWindow->winWindow, TRUE);
              }
            }
            peventEvent->uiKind = E_DONE;
            break;
          case K_DOWN :
            if (!sbWorking)
              return;
            if (spwindowWindow != pmenuLast->pwindowWindow)
            {
              if (spwindowWindow->pitemFirst != spwindowWindow->pitemLast)
              {                   
                int_mnu_draw_item(spwindowWindow, spwindowWindow->winWindow, FALSE);
                spwindowWindow->pitemActive = spwindowWindow->pitemActive->pitemNext;
                for (; spwindowWindow->pitemActive->uiMessage == M_MNU_LINE; )
                  spwindowWindow->pitemActive = spwindowWindow->pitemActive->pitemNext;
                if (!spwindowWindow->pitemActive)
                  spwindowWindow->pitemActive = spwindowWindow->pitemFirst;
                int_mnu_draw_item(spwindowWindow, spwindowWindow->winWindow, TRUE);
              }
            }                        
            else
              if (spwindowWindow->pitemActive->pwindowWindow)
                spwindowWindow = int_mnu_open_window(spwindowWindow, spsStack);        
            peventEvent->uiKind = E_DONE;
            break;
          case K_LEFT :
            if (!sbWorking)
              return;
            if (spwindowWindow == pmenuLast->pwindowWindow)
            {
              if (spwindowWindow->pitemFirst != spwindowWindow->pitemLast)
              {
                if (suiLookBack)
                {
                  for (; suiLookBack > 0; suiLookBack--)
                    win_delete(((MNU_WINDOW*)cnt_stack_get(spsStack))->winWindow);
                  bDrawWindow = TRUE;
                }
                else
                  bDrawWindow = FALSE;
                int_mnu_draw_item(spwindowWindow, spwindowWindow->winWindow, FALSE);
                spwindowWindow->pitemActive = spwindowWindow->pitemActive->pitemPrev;
                if (!spwindowWindow->pitemActive)
                  spwindowWindow->pitemActive = spwindowWindow->pitemLast;
                int_mnu_draw_item(spwindowWindow, spwindowWindow->winWindow, TRUE);
                if (bDrawWindow && spwindowWindow->pitemActive->pwindowWindow)
                  spwindowWindow = int_mnu_open_window(spwindowWindow, spsStack);
              }
              peventEvent->uiKind = E_DONE;
            }
            break;
          case K_RIGHT :
            if (!sbWorking)
              return;
            if (spwindowWindow == pmenuLast->pwindowWindow)
            {
              if (spwindowWindow->pitemFirst != spwindowWindow->pitemLast)
              {
                if (suiLookBack)
                {
                  for (; suiLookBack > 0; suiLookBack--)
                    win_delete(((MNU_WINDOW*)cnt_stack_get(spsStack))->winWindow);
                  bDrawWindow = TRUE;
                }
                else
                  bDrawWindow = FALSE;
                int_mnu_draw_item(spwindowWindow, spwindowWindow->winWindow, FALSE);
                spwindowWindow->pitemActive = spwindowWindow->pitemActive->pitemNext;
                if (!spwindowWindow->pitemActive)
                  spwindowWindow->pitemActive = spwindowWindow->pitemFirst;
                int_mnu_draw_item(spwindowWindow, spwindowWindow->winWindow, TRUE);
                if (bDrawWindow && spwindowWindow->pitemActive->pwindowWindow)
                  spwindowWindow = int_mnu_open_window(spwindowWindow, spsStack);
              }
              peventEvent->uiKind = E_DONE;
            }
            break;
          default :
            if (!sbWorking)
            {
              pitemItem = int_mnu_check_item(pmenuLast->pwindowWindow, NULL, peventEvent);
              if (pitemItem && (peventEvent->wKey == pitemItem->wHotKey || peventEvent->byKeybStatus & ALT))
              {
                sbWorking = TRUE;
                bUpdating = TRUE;
                swinFocussed = prgProgram.winFocussed;
                win_set_focus(winDesktop);
                sts_new_status_line(NULL);
                win_set_working(winDesktop);
                spwindowWindow = pmenuLast->pwindowWindow;
                cnt_stack_put(spsStack, (ULONG)spwindowWindow);
                suiLookBack = 0;
                spwindowWindow->pitemActive = pitemItem;
                int_mnu_draw_item(spwindowWindow, spwindowWindow->winWindow, TRUE);
                if (pitemItem->bActive)
                {
                  if (spwindowWindow->pitemActive->pwindowWindow)
                    spwindowWindow = int_mnu_open_window(spwindowWindow, spsStack);
                  else
                  {
                    cnt_stack_get(spsStack);
                    pwindowWindow = spwindowWindow;
                    for (; pwindowWindow != pmenuLast->pwindowWindow;
                         pwindowWindow = (MNU_WINDOW*)cnt_stack_get(spsStack))
                      win_delete(pwindowWindow->winWindow);
                    int_mnu_draw_item(pwindowWindow, pwindowWindow->winWindow, FALSE);
                    cnt_stack_get(spsStack);
                    sbWorking = FALSE;
                    bUpdating = FALSE;
                    sts_del_status_line();
                    win_set_focus(swinFocussed);
                    win_set_working(NULL);
                    utl_fill_event(&eventEvent);
                    eventEvent.uiKind = E_MESSAGE;
                    eventEvent.uiMessage = spwindowWindow->pitemActive->uiMessage;
                    win_handle_event(prgProgram.winFocussed, &eventEvent);
                  }
                }
                peventEvent->uiKind = E_DONE;
              }
            }
            else
            {
              pitemItem = int_mnu_check_item(spwindowWindow, NULL, peventEvent);
              if (pitemItem)
              {
                if (pitemItem != spwindowWindow->pitemActive)
                {
                  int_mnu_draw_item(spwindowWindow, spwindowWindow->winWindow, FALSE);
                  spwindowWindow->pitemActive = pitemItem;
                  int_mnu_draw_item(spwindowWindow, spwindowWindow->winWindow, TRUE);
                }
                if (pitemItem->bActive)
                {
                  if (spwindowWindow->pitemActive->pwindowWindow)
                    spwindowWindow = int_mnu_open_window(spwindowWindow, spsStack);
                  else
                  {
                    cnt_stack_get(spsStack);
                    pwindowWindow = spwindowWindow;
                    for (; pwindowWindow != pmenuLast->pwindowWindow;
                         pwindowWindow = (MNU_WINDOW*)cnt_stack_get(spsStack))
                      win_delete(pwindowWindow->winWindow);
                    int_mnu_draw_item(pwindowWindow, pwindowWindow->winWindow, FALSE);
                    cnt_stack_get(spsStack);
                    sbWorking = FALSE;
                    bUpdating = FALSE;
                    sts_del_status_line();
                    win_set_focus(swinFocussed);
                    win_set_working(NULL);
                    utl_fill_event(&eventEvent);
                    eventEvent.uiKind = E_MESSAGE;
                    eventEvent.uiMessage = spwindowWindow->pitemActive->uiMessage;
                    win_handle_event(prgProgram.winFocussed, &eventEvent);
                  }
                }
                peventEvent->uiKind = E_DONE;
              }
            }
            return;
        }
        break;
      case E_MSM_L_DOWN :
        if (!sbWorking)
        {
          pitemItem = int_mnu_check_item(pmenuLast->pwindowWindow, pmenuLast->pwindowWindow->winWindow,
                                         peventEvent);
          if (pitemItem)
          {
            sbWorking = TRUE;
            bUpdating = TRUE;
            swinFocussed = prgProgram.winFocussed;
            win_set_focus(winDesktop);
            sts_new_status_line(NULL);
            win_set_working(winDesktop);
            spwindowWindow = pmenuLast->pwindowWindow;
            cnt_stack_put(spsStack, (ULONG)spwindowWindow);
            suiLookBack = 0;                        
            spwindowWindow->pitemActive = pitemItem;
            int_mnu_draw_item(spwindowWindow, spwindowWindow->winWindow, TRUE);
            if (pitemItem->pwindowWindow)
              spwindowWindow = int_mnu_open_window(spwindowWindow, spsStack);
            peventEvent->uiKind = E_DONE;
          }
        }
        else
        {
          pitemItem = int_mnu_check_item(spwindowWindow, spwindowWindow->winWindow, peventEvent);
          if (spwindowWindow == pmenuLast->pwindowWindow)
          {
            if (pitemItem)
            {
              if (suiLookBack)
                for (; suiLookBack > 0; suiLookBack--)
                  win_delete(((MNU_WINDOW*)cnt_stack_get(spsStack))->winWindow);
              spwindowWindow = pmenuLast->pwindowWindow;
              int_mnu_draw_item(spwindowWindow, spwindowWindow->winWindow, FALSE);
              spwindowWindow->pitemActive = pitemItem;
              int_mnu_draw_item(spwindowWindow, spwindowWindow->winWindow, TRUE);
              if (pitemItem->pwindowWindow)
                spwindowWindow = int_mnu_open_window(spwindowWindow, spsStack);
              peventEvent->uiKind = E_DONE;
            }
            else
            {
              if (suiLookBack)
                for (; suiLookBack > 0; suiLookBack--)
                  win_delete(((MNU_WINDOW*)cnt_stack_get(spsStack))->winWindow);
              int_mnu_draw_item(pmenuLast->pwindowWindow, pmenuLast->pwindowWindow->winWindow, FALSE);
              cnt_stack_get(spsStack);
              sbWorking = FALSE;
              bUpdating = FALSE;
              sts_del_status_line();
              win_set_focus(swinFocussed);
              win_set_working(NULL);
              peventEvent->uiKind = E_DONE;
            }
          }
          else
          {
            if (int_mnu_tear_down_event(peventEvent, spwindowWindow))
            {
              pwindowWindow = spwindowWindow;
              iX = pwindowWindow->winWindow->iX;
              iY = pwindowWindow->winWindow->iY;
              for (spwindowWindow = (MNU_WINDOW*)cnt_stack_get(spsStack);
                   spwindowWindow != pmenuLast->pwindowWindow;
                   spwindowWindow = (MNU_WINDOW*)cnt_stack_get(spsStack))
                win_delete(spwindowWindow->winWindow);
              int_mnu_draw_item(pmenuLast->pwindowWindow, pmenuLast->pwindowWindow->winWindow, FALSE);
              sbWorking = FALSE;
              bUpdating = FALSE;
              sts_del_status_line();
              win_set_focus(swinFocussed);
              win_set_working(NULL);
              int_mnu_make_tear_down(pwindowWindow, iX, iY);
              peventEvent->uiKind = E_DONE;
              break;
            }
            if (pitemItem)
            {
              if (suiLookBack)
                for (; suiLookBack > 0; suiLookBack--)
                  win_delete(((MNU_WINDOW*)cnt_stack_get(spsStack))->winWindow);
              spwindowWindow = (MNU_WINDOW*)cnt_stack_look_back(spsStack, 0);
              if (pitemItem != spwindowWindow->pitemActive)
              {
                int_mnu_draw_item(spwindowWindow, spwindowWindow->winWindow, FALSE);
                spwindowWindow->pitemActive = pitemItem;
                int_mnu_draw_item(spwindowWindow, spwindowWindow->winWindow, TRUE);
              }
              peventEvent->uiKind = E_DONE;
            }
          } 
        }    
        break;     
      case E_MSM_L_UP :
        if (!sbWorking)
          return;
        pitemItem = int_mnu_check_item(spwindowWindow, spwindowWindow->winWindow, peventEvent);
        if (pitemItem)
        {
          if (suiLookBack)
          {
            suiLookBack = 0;
            spwindowWindow = (MNU_WINDOW*)cnt_stack_look_back(spsStack, 0);
            peventEvent->uiKind = E_DONE;
            return;
          }
          if (pitemItem->uiMessage == M_MNU_LINE)
          {
            spwindowWindow->pitemActive = spwindowWindow->pitemFirst;
            int_mnu_draw_item(spwindowWindow, spwindowWindow->winWindow, TRUE);
            peventEvent->uiKind = E_DONE;
          }
          else
          {                              
            peventEvent->uiKind = E_DONE;
            if (!spwindowWindow->pitemActive->bActive)
              break;
            if (spwindowWindow->pitemActive->pwindowWindow)
              spwindowWindow = int_mnu_open_window(spwindowWindow, spsStack);        
            else
            {                         
              cnt_stack_get(spsStack);
              pwindowWindow = spwindowWindow;
              for (; pwindowWindow != pmenuLast->pwindowWindow;
                   pwindowWindow = (MNU_WINDOW*)cnt_stack_get(spsStack))
                win_delete(pwindowWindow->winWindow);
              int_mnu_draw_item(pwindowWindow, pwindowWindow->winWindow, FALSE);
              cnt_stack_get(spsStack);
              sbWorking = FALSE;
              bUpdating = FALSE;
              sts_del_status_line();
              win_set_focus(swinFocussed);
              win_set_working(NULL);
              utl_fill_event(&eventEvent);
              eventEvent.uiKind = E_MESSAGE;
              eventEvent.uiMessage = pitemItem->uiMessage;
              win_handle_event(prgProgram.winFocussed, &eventEvent);
            }
          }
        }
        else
        {
          if (spwindowWindow == pmenuLast->pwindowWindow)
          {
            if (suiLookBack)
              for (; suiLookBack > 0; suiLookBack--)
                win_delete(((MNU_WINDOW*)cnt_stack_get(spsStack))->winWindow);
            int_mnu_draw_item(pmenuLast->pwindowWindow, pmenuLast->pwindowWindow->winWindow, FALSE);
            cnt_stack_get(spsStack);
            sbWorking = FALSE;
            bUpdating = FALSE;
            sts_del_status_line();
            win_set_focus(swinFocussed);
            win_set_working(NULL);
            peventEvent->uiKind = E_DONE;
          }
        }
        break;
      case E_MSM_MOVE :
        if (!sbWorking)
          return;
        if (peventEvent->wButtons & MSM_B_LEFT)
        {
          pitemItem = int_mnu_check_item(spwindowWindow, spwindowWindow->winWindow, peventEvent);
          if (pitemItem)
          {
            if (spwindowWindow == pmenuLast->pwindowWindow)
            {
              if (suiLookBack)
                for (; suiLookBack > 1; suiLookBack--)
                  win_delete(((MNU_WINDOW*)cnt_stack_get(spsStack))->winWindow);
              if (pitemItem != spwindowWindow->pitemActive)
              {
                win_delete(((MNU_WINDOW*)cnt_stack_get(spsStack))->winWindow);
                int_mnu_draw_item(spwindowWindow, spwindowWindow->winWindow, FALSE);
                spwindowWindow->pitemActive = pitemItem;
                int_mnu_draw_item(spwindowWindow, spwindowWindow->winWindow, TRUE);
                if (pitemItem->pwindowWindow)
                  spwindowWindow = int_mnu_open_window(spwindowWindow, spsStack);
              }
              suiLookBack = 0;
              spwindowWindow = (MNU_WINDOW*)cnt_stack_look_back(spsStack, 0);
            }
            else
            {
              if (suiLookBack)
                for (; suiLookBack > 0; suiLookBack--)
                  win_delete(((MNU_WINDOW*)cnt_stack_get(spsStack))->winWindow);
              spwindowWindow = (MNU_WINDOW*)cnt_stack_look_back(spsStack, 0);
              if (pitemItem != spwindowWindow->pitemActive)
              {
                int_mnu_draw_item(spwindowWindow, spwindowWindow->winWindow, FALSE);
                spwindowWindow->pitemActive = pitemItem;
                int_mnu_draw_item(spwindowWindow, spwindowWindow->winWindow, TRUE);
              }
            }
            peventEvent->uiKind = E_DONE;
          }
          else
            if (spwindowWindow == pmenuLast->pwindowWindow)
            {
              suiLookBack = 0;
              spwindowWindow = (MNU_WINDOW*)cnt_stack_look_back(spsStack, 0);
              peventEvent->uiKind = E_DONE;
              return;
            }
        } 
        break;
      default :
        if (!sbWorking)
          return;
        peventEvent->uiKind = E_DONE;
        break;
    }                   
    if (!sbWorking)
      return;
    bWentBack = TRUE;
  } while (peventEvent->uiKind != E_DONE && spwindowWindow != pmenuLast->pwindowWindow);
  if (peventEvent->uiKind != E_DONE)
  {
    suiLookBack = 0;
    spwindowWindow = (MNU_WINDOW*)cnt_stack_look_back(spsStack, 0);
  }
  if (bSetDone)
    peventEvent->uiKind = E_DONE;
}  
                                                   
BOOL mnu_new_menu (UINT uiID, INT iRight, MNU_ITEM *pitemFirst)
{
  MNU_MENU *pmenuMenu;
  WINDOW    winWindow;

  if (!pitemFirst)
    return FALSE;
  if (!(pmenuMenu = utl_alloc(sizeof(MNU_MENU))))
    return FALSE;
  if (pmenuLast)
  {
    pmenuLast->psTearDowns = cnt_stack_make();
    do
    {
      winWindow = win_find(int_mnu_check_tear_down, (ULONG)pmenuLast->pwindowWindow);
      if (winWindow)
      {
        win_hide(winWindow);
        cnt_stack_put(pmenuLast->psTearDowns, (ULONG)winWindow);
      }
    } while (winWindow);
  }
  pmenuMenu->uiID = uiID;
  pmenuMenu->iRight = iRight;
  pmenuMenu->pwindowWindow = mnu_new_window(pitemFirst);
  pmenuMenu->pmenuPrev = pmenuLast;
  pmenuLast = pmenuMenu;
  int_mnu_write_menu_bar();
  return TRUE;
}              

MNU_ITEM* mnu_new_item (UINT uiMessage, CHAR *pcText, CHAR *pcHelpLine, WORD wHotKey, UINT uiContext,
                        MNU_WINDOW *pwindowWindow, MNU_ITEM *pitemNext)
{
  MNU_ITEM *pitemItem;

  if (!(pitemItem = utl_alloc(sizeof(MNU_ITEM))))
    return NULL;
  pitemItem->uiMessage = uiMessage;
  pitemItem->pcText = pcText;              
  pitemItem->pcHelpLine = pcHelpLine;
  if (pcText)
    pitemItem->cDispHotKey = utl_get_hot_key(pcText);
  pitemItem->wHotKey = wHotKey;
  pitemItem->uiContext = uiContext;         
  pitemItem->bActive = TRUE;
  pitemItem->pwindowWindow = pwindowWindow;
  pitemItem->pitemNext = pitemNext;
  pitemItem->pitemPrev = NULL;
  if (pitemNext)
    pitemNext->pitemPrev = pitemItem;
  return pitemItem;
}

MNU_WINDOW* mnu_new_window (MNU_ITEM *pitemFirst)
{
  MNU_WINDOW *pwindowWindow;               
  MNU_ITEM   *pitemCounter;
  INT         iCounter      = 1,
              iLongest      = 0;
  BOOL        bLowerLevel   = FALSE;       

  if (!pitemFirst)
    return NULL;
  if (!(pwindowWindow = utl_alloc(sizeof(MNU_WINDOW))))
    return NULL;
  pwindowWindow->pitemFirst = pwindowWindow->pitemActive = pitemFirst;
  for (pitemCounter = pitemFirst; pitemCounter->pitemNext;
       pitemCounter = pitemCounter->pitemNext)
  {
    pitemCounter->iLine = iCounter++;
    if (pitemCounter->pcText)
      iLongest = max(iLongest, utl_hot_strlen(pitemCounter->pcText));
    if (pitemCounter->pwindowWindow)
      bLowerLevel = TRUE;
  }
  pitemCounter->iLine = iCounter;
  if (pitemCounter->pcText)
    iLongest = max(iLongest, utl_hot_strlen(pitemCounter->pcText));
  if (pitemCounter->pwindowWindow)
    bLowerLevel = TRUE;
  pwindowWindow->pitemLast = pitemCounter;
  pwindowWindow->iWidth = iLongest + 4;
  if (bLowerLevel)
    pwindowWindow->iWidth += 3;
  pwindowWindow->iHeight = iCounter + 2;
  return pwindowWindow;
}

void mnu_del_menu (void)
{          
  MNU_MENU *pmenuMenu;
  WINDOW    winWindow;
  
  if (!pmenuLast)
    return;
  do
  {
    winWindow = win_find(int_mnu_check_tear_down, (ULONG)pmenuLast->pwindowWindow);
    if (winWindow)
      win_delete(winWindow);
  } while (winWindow);
  int_mnu_del_window(pmenuLast->pwindowWindow);
  pmenuMenu = pmenuLast;
  pmenuLast = pmenuMenu->pmenuPrev;
  utl_free(pmenuMenu);
  int_mnu_write_menu_bar();
  if (pmenuLast)
  {
    while (!cnt_stack_is_empty(pmenuLast->psTearDowns))
      win_show((WINDOW)cnt_stack_get(pmenuLast->psTearDowns));
    cnt_stack_destroy(pmenuLast->psTearDowns);
  }
}

BOOL mnu_item_active (UINT uiMessage, UCHAR ucStatus)
{
  MNU_WINDOW *pwindowWindow;
  MNU_ITEM   *pitemItem;
  WINDOW      winWindow;
  BOOL        bReturnVar;
  
  if (!pmenuLast)
    return FALSE;
  pitemItem = int_mnu_find_item(pmenuLast->pwindowWindow, uiMessage, &pwindowWindow);
  if (!pitemItem)
    return FALSE;
  bReturnVar = pitemItem->bActive;
  switch (ucStatus)
  {
    case QUERY :
      return bReturnVar;
    case FLIP :
      pitemItem->bActive = !(pitemItem->bActive);
      break;
    default :
      pitemItem->bActive = (BOOL)ucStatus;
      break;
  }
  for (winWindow = prgProgram.winLast; winWindow;
       winWindow = winWindow->winPrev)
    if (mnu_is_tear_down(winWindow))
      if (((MNU_TEAR_DOWN*)win_get_add_info(winWindow))->pwindowWindow == pwindowWindow)                 
      {
        glb_send_message(NULL, winWindow, M_REDRAW, 0);
        break;
      }
  return bReturnVar;
}

BOOL mnu_is_tear_down (WINDOW winWindow)
{
  return int_mnu_check_tear_down(winWindow, (ULONG)pmenuLast->pwindowWindow);
}

/*
Local Variables:
compile-command: "wmake -f stools.mk -h -e"
End:
*/
