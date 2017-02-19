/***************************************************************************
 ***************************************************************************
 ***                                                                     ***
 ***                        Schani-Tools fÅr C                           ***
 ***                                                                     ***
 ***                     Headerdatei fÅr MenÅ-Teil                       ***
 ***                                                                     ***
 ***                (c) 1990-93 by Schani Electronics                    ***
 ***                                                                     ***
 ***************************************************************************
 ***************************************************************************/

/***************************************************************************
 *                            Konstanten                                   *
 ***************************************************************************/

#define M_MNU_LINE                             64000

/***************************************************************************
 *                           MenÅ-Blocks                                   *
 ***************************************************************************/

        struct _MNU_ITEM;
        struct _MNU_WINDOW;
typedef struct _MNU_ITEM    MNU_ITEM;
typedef struct _MNU_WINDOW  MNU_WINDOW;

#ifndef _DEFINED_WINDOW
#define _DEFINED_WINDOW
        struct _WB;
typedef struct _WB* WINDOW;
#endif

typedef struct _MNU_MENU
{
  UINT              uiID;
  INT               iRight;
  MNU_WINDOW       *pwindowWindow;
  CNT_STACK        *psTearDowns;
  struct _MNU_MENU *pmenuPrev;
} MNU_MENU;

struct _MNU_ITEM
{
  UINT              uiMessage;
  CHAR             *pcText;
  CHAR             *pcHelpLine;
  CHAR              cDispHotKey;
  WORD              wHotKey;
  UINT              uiContext;
  INT               iLine;        
  INT               iX;
  BOOL              bActive;
  MNU_WINDOW       *pwindowWindow;
  struct _MNU_ITEM *pitemNext;
  struct _MNU_ITEM *pitemPrev;
};

struct _MNU_WINDOW
{
  INT       iWidth;
  INT       iHeight;
  WINDOW    winWindow;
  MNU_ITEM *pitemFirst;
  MNU_ITEM *pitemLast;
  MNU_ITEM *pitemActive;
};

typedef struct
{                           
  BOOL        bDragging;
  MNU_WINDOW *pwindowWindow;
} MNU_TEAR_DOWN;

/***************************************************************************
 *                                Makros                                   *
 ***************************************************************************/

#define mnu_new_line(i)           mnu_new_item(M_MNU_LINE, NULL, NULL, 0, 0, NULL, i)

/***************************************************************************
 *                          Funktionsprototypen                            *
 ***************************************************************************/

#ifdef __cplusplus
extern "C"
{
#endif                       
       void        mnu_init                (void);
       void        mnu_redraw              (void);
       void        mnu_window_handler      (WINDOW, UTL_EVENT*);
       void        mnu_tear_down_handler   (WINDOW, UTL_EVENT*);
       void        mnu_pre_process         (UTL_EVENT*);
       void        mnu_event               (UTL_EVENT*);
       BOOL        mnu_new_menu            (UINT, INT, MNU_ITEM*);
       MNU_ITEM*   mnu_new_item            (UINT, CHAR*, CHAR*, WORD, UINT, MNU_WINDOW*, MNU_ITEM*);
       MNU_WINDOW* mnu_new_window          (MNU_ITEM*);
       void        mnu_del_menu            (void);
       BOOL        mnu_item_active         (UINT, UCHAR);
       BOOL        mnu_is_tear_down        (WINDOW);
#ifdef __cplusplus
}
#endif
