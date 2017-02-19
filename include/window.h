/***************************************************************************
 ***************************************************************************
 ***                                                                     ***
 ***                      Schani-Tools fr C                             ***
 ***                                                                     ***
 ***                  Headerdatei fr Window-Teil                        ***
 ***                                                                     ***
 ***               (c) 1990-93 by Schani Electronics                     ***
 ***                                                                     ***
 ***************************************************************************
 ***************************************************************************/

#ifndef _STOOLS_WINDOW
#define _STOOLS_WINDOW

/***************************************************************************
 *                          Window-Block                                   *
 ***************************************************************************/

#define WIN_FL_SHADOW          0x0001
#define WIN_FL_CURSOR          0x0002
#define WIN_FL_MOVEABLE        0x0004
#define WIN_FL_CLOSEABLE       0x0008
#define WIN_FL_SIZEABLE        0x0010
#define WIN_FL_MODAL           0x0020
#define WIN_FL_FOCUSABLE       0x0040
#define WIN_FL_ON_TOP          0x0080

typedef struct
{
  UINT binBorderType    : 5;
  UINT binTitlePos      : 3;
  UINT binShadow        : 1;
  UINT binCursorAct     : 1;
  UINT binMoveable      : 1;
  UINT binCloseable     : 1;
  UINT binSizeable      : 1;
  UINT binModal         : 1;
  UINT binDragging      : 1;
  UINT binSizing        : 1;
  UINT binDrawCheck     : 1;
  UINT binCanBeFocussed : 1;
  UINT binAlwaysOnTop   : 1;
  UINT binMaximized     : 1;
} WIN_FLAGS;

#ifndef _DEFINED_EVENT
#define _DEFINED_EVENT
typedef struct _UTL_EVENT UTL_EVENT;
#endif

#define WIN_MAX_CLASS_NAME         32

typedef struct _WINDOW_CLASS
{
  CHAR                   acName[WIN_MAX_CLASS_NAME];
  struct _WINDOW_CLASS  *pwcNext;
  void                 (*handle_event)(struct _WB*, UTL_EVENT*);
  CHAR                  *pcColors;
  WIN_FLAGS              flFlags;
} WINDOW_CLASS;

typedef struct _WB
{
  void         (*handle_event)(struct _WB*, UTL_EVENT*);
  INT            iX;
  INT            iY;
  INT            iWidth;
  INT            iHeight;
  INT            iMinWidth;
  INT            iMinHeight;
  INT            iMaxWidth;
  INT            iMaxHeight; 
  INT            iLastWidth;
  INT            iLastHeight;
  INT            iCursX;
  INT            iCursY;
  INT            iCursPercent;
  CHAR          *pcTitle;
  ULONG          ulAddInfo;
  CHARACTER     *pcharSave;      
  CHAR          *pcColors;
  struct _WB    *winNext;
  struct _WB    *winPrev;
  DLG_ELEMENT   *pelementFirst;
  DLG_ELEMENT   *pelementLast;
  DLG_ELEMENT   *pelementActive;
  WINDOW_CLASS  *pwcClass;
  void         (*redraw_func)(struct _WB*);
  WIN_FLAGS      flFlags;
} WB;

#ifndef _DEFINED_WINDOW
#define _DEFINED_WINDOW
typedef WB* WINDOW;
#endif

/***************************************************************************
 *                   Konstanten fr interne Zwecke                         *
 ***************************************************************************/

#define        AKT_WIN            prgProgram.winFocussed
#define        REAL_WIDTH(w)      ((w)->flFlags.binShadow ? (w)->iWidth + 2 : (w)->iWidth)
#define        REAL_HEIGHT(w)     ((w)->flFlags.binShadow ? (w)->iHeight + 1 : (w)->iHeight)
#define        ON_SCREEN(x,y)     ((x) >= 1 && (x) <= iSizeX && (y) >= 1 && (y) <= iSizeY)

#define        WIN_CHAR          0x01
#define        WIN_ATTRI         0x02

/***************************************************************************
 *                   Konstanten fr Titelpositionen                        *
 ***************************************************************************/

#define        TITLE_T_C          0
#define        TITLE_T_L          1
#define        TITLE_T_R          2
#define        TITLE_B_C          3
#define        TITLE_B_L          4
#define        TITLE_B_R          5

/***************************************************************************
 *                              Makros                                     *
 ***************************************************************************/

#define IN_WINDOW(x,y,w)        ((x >= w->iX) && (x <= (w->iX +            \
                                 w->iWidth - 1)) && (y >= w->iY) &&        \
                                 (y <= (w->iY + w->iHeight - 1)))
#define ON_WINDOW(x,y,w)        ((x >= w->iX) && (x <= (w->iX +            \
                                 REAL_WIDTH(w) - 1)) && (y >= w->iY) &&    \
                                   (y <= (w->iY + REAL_HEIGHT(w) - 1)))
#define ON_BORDER(x,y,w)        (IN_WINDOW(x,y,w) && (x == w->iX ||        \
                                 y == w->iY ||                             \
                                 x == w->iX + w->iWidth - 1 ||             \
                                 y == w->iY + w->iHeight - 1))
#define XY_IN_WINDOW(w,x,y)     (x >= 1 && y >= 1 && x < w->iWidth - 1 &&  \
                                 y < w->iHeight - 1)
#define WH_IN_WINDOW(w,x,y,b,h) (x >= 1 && y >= 1 && x + b < w->iWidth &&  \
                                 y + h < w->iHeight)
#define CHECK_XY(w,x,y)         if (w->flFlags.binDrawCheck)               \
                                  if (!XY_IN_WINDOW(w,x,y))                \
                                    return
#define CHECK_WH(w,x,y,b,h)     if (w->flFlags.binDrawCheck)               \
                                  if (!WH_IN_WINDOW(w,x,y,b,h))            \
                                  {                                        \
                                    if (!XY_IN_WINDOW(w,x,y))              \
                                      return;                              \
                                    b = min(b,w->iWidth - x - 1);          \
                                    h = min(h,w->iHeight - y - 1);         \
                                  }

#define win_set_add_info(w,i)    w->ulAddInfo = (ULONG)(i)
#define win_get_add_info(w)      (w->ulAddInfo)
#define win_get_title(w)         (w->pcTitle)
#define win_get_last_button(w,i) glb_send_message(NULL, w, M_QUERY_VALUES, \
                                                  (ULONG)(i))
#define win_get_color(w,c)       (w->pcColors[c])

/***************************************************************************
 *                             Desktop                                     *
 ***************************************************************************/

#define dsk_cls()                  win_cls(winDesktop)
#define dsk_cursor(s)              win_cursor(winDesktop,s)
#define dsk_set_cursor_pos(x,y)    win_set_cursor_pos(winDesktop,x,y)
#define dsk_get_cursor_pos(x,y)    win_get_cursor_pos(winDesktop,x,y)
#define dsk_set_cursor_type(p)     win_set_cursor_type(winDesktop,p)
#define dsk_get_cursor_type(p)     win_get_cursor_type(winDesktop,p)
#define dsk_draw_border(x,y,b,h,t) win_draw_border(winDesktop,x,y,b,h,t)
#define dsk_printch(c)             win_printch(winDesktop,c)
#define dsk_up(x,y,b,h,a)          win_up(winDesktop,x,y,b,h,a)
#define dsk_scroll_up(a)           win_scroll_up(winDesktop,a)
#define dsk_down(x,y,b,h,a)        win_down(winDesktop,x,y,b,h,a)
#define dsk_scroll_down(a)         win_scroll_down(winDesktop,a)
#define dsk_ins_zeile(z)           win_ins_zeile(winDesktop,z)
#define dsk_del_zeile(z)           win_del_zeile(winDesktop,z)
#define dsk_write_hot(x,y,s,n,h)   win_write_hot(winDesktop,x,y,s,n,h)
#define dsk_get_color(c)           win_get_color(winDesktop,c)
#define dsk_l_a(x,y)               win_l_a(winDesktop,x,y)
#define dsk_l_z(x,y)               win_l_z(winDesktop,x,y)
#define dsk_l_za(x,y)              win_l_za(winDesktop,x,y)
#define dsk_s_a(x,y,a)             win_s_a(winDesktop,x,y,a)
#define dsk_s_z(x,y,c)             win_s_z(winDesktop,x,y,c)
#define dsk_s_za(x,y,c,a)          win_s_za(winDesktop,x,y,c,a)
#define dsk_sp_a(a)                win_sp_a(winDesktop,a)
#define dsk_sp_z(c)                win_sp_z(winDesktop,c)
#define dsk_sp_za(c,a)             win_sp_za(winDesktop,c,a)
#define dsk_ss(x,y,s)              win_ss(winDesktop,x,y,s)
#define dsk_ss_a(x,y,s,a)          win_ss_a(winDesktop,x,y,s,a)
#define dsk_sw_a(x,y,b,h,a)        win_sw_a(winDesktop,x,y,b,h,a)
#define dsk_sw_z(x,y,b,h,c)        win_sw_z(winDesktop,x,y,b,h,c)
#define dsk_sw_za(x,y,b,h,c,a)     win_sw_za(winDesktop,x,y,b,h,c,a)
#define dsk_lw(x,y,b,h,p)          win_lw(winDesktop,x,y,b,h,p)
#define dsk_sw(x,y,b,h,p)          win_sw(winDesktop,x,y,b,h,p)

/***************************************************************************
 *                        Funktionsprototypen                              *
 ***************************************************************************/

#ifdef __cplusplus
extern "C"
{
#endif
void                 win_init              (void);
void                 win_draw_elements     (WINDOW);
void                 win_add_element       (WINDOW, DLG_ELEMENT*);
void                 win_insert_element    (WINDOW, DLG_ELEMENT*, DLG_ELEMENT*);
void                 win_delete_element    (WINDOW, DLG_ELEMENT*);
DLG_ELEMENT*         win_get_element       (WINDOW, UINT);
void                 win_make_local        (WINDOW, UTL_EVENT*, UTL_EVENT*);
WINDOW               win_which             (UTL_EVENT*);
void                 win_set_focus         (WINDOW);
void                 win_pre_process       (WINDOW, UTL_EVENT*);
void                 win_handle_event      (WINDOW, UTL_EVENT*);
void                 win_set_working       (WINDOW);
void                 win_std_handler       (WINDOW, UTL_EVENT*);
void                 win_update_on_top     (void);
void                 win_update_cursor     (void);
BOOL                 win_register_class    (CHAR*, UCHAR, UCHAR, UCHAR, UINT, void (*)(WINDOW,UTL_EVENT*));
CHAR*                win_get_class         (WINDOW);
WINDOW               win_find_class_window (CHAR*);
WINDOW               win_new               (INT, INT, INT, INT, CHAR*, ULONG);
BOOL                 win_show              (WINDOW);
BOOL                 win_hide              (WINDOW);
BOOL                 win_delete            (WINDOW);
BOOL                 win_title             (WINDOW, CHAR*, UCHAR);
BOOL                 win_border            (WINDOW, UCHAR);
BOOL                 win_color             (WINDOW, UCHAR);
void                 win_update            (WINDOW);
BOOL                 win_move              (WINDOW, INT, INT);
BOOL                 win_resize            (WINDOW, INT, INT);
BOOL                 win_maximize          (WINDOW);
WINDOW               win_find              (BOOL(*)(WINDOW, ULONG), ULONG);
void                 win_set_size_limits   (WINDOW, INT, INT, INT, INT);
void                 win_cls               (WINDOW);
BOOL                 win_shadow            (WINDOW, BOOL);
BOOL                 win_moveable          (WINDOW, UCHAR);
BOOL                 win_closeable         (WINDOW, UCHAR);
BOOL                 win_sizeable          (WINDOW, UCHAR);
BOOL                 win_modal             (WINDOW, UCHAR);
BOOL                 win_clipping          (WINDOW, UCHAR);
BOOL                 win_focusable         (WINDOW, UCHAR);
BOOL                 win_always_on_top     (WINDOW, UCHAR);
BOOL                 win_cursor            (WINDOW, UCHAR);
void                 win_set_redraw        (WINDOW, void (*)(WINDOW));
void                 win_set_cursor_pos    (WINDOW, INT, INT);
void                 win_get_cursor_pos    (WINDOW, INT*, INT*);
void                 win_set_cursor_type   (WINDOW, INT);
void                 win_get_cursor_type   (WINDOW, INT*);
void                 win_draw_border       (WINDOW, INT, INT, INT, INT, UCHAR, CHAR);
void                 win_write_hot         (WINDOW, INT, INT, CHAR*, CHAR, CHAR);
INT                  win_printf            (WINDOW, const CHAR*, ...);
INT                  dsk_printf            (const CHAR*, ...);
void                 win_printch           (WINDOW, CHAR);
void                 win_up                (WINDOW, INT, INT, INT, INT, INT);
void                 win_scroll_up         (WINDOW, INT);
void                 win_down              (WINDOW, INT, INT, INT, INT, INT);
void                 win_scroll_down       (WINDOW, INT);
void                 win_set_tab           (INT);
void                 win_ins_zeile         (WINDOW, INT);
void                 win_del_zeile         (WINDOW, INT);
BOOL                 win_info              (CHAR*);
CHAR                 win_l_a               (WINDOW, INT, INT);
CHAR                 win_l_z               (WINDOW, INT, INT);
CHARACTER            win_l_za              (WINDOW, INT, INT);
void                 win_s_a               (WINDOW, INT, INT, CHAR);
void                 win_s_z               (WINDOW, INT, INT, CHAR);
void                 win_s_za              (WINDOW, INT, INT, CHAR, CHAR);
void                 win_sp_a              (WINDOW, CHAR);
void                 win_sp_z              (WINDOW, CHAR);
void                 win_sp_za             (WINDOW, CHAR, CHAR);
void                 win_ss                (WINDOW, INT, INT, CHAR*);
void                 win_ss_a              (WINDOW, INT, INT, CHAR*, CHAR);
void                 win_ssn               (WINDOW, INT, INT, CHAR*, INT);
void                 win_ssn_a             (WINDOW, INT, INT, CHAR*, INT, CHAR);
void                 win_sw_a              (WINDOW, INT, INT, INT, INT, CHAR);
void                 win_sw_z              (WINDOW, INT, INT, INT, INT, CHAR);
void                 win_sw_za             (WINDOW, INT, INT, INT, INT, CHAR, CHAR);
void                 win_lw                (WINDOW, INT, INT, INT, INT, CHARACTER*);
void                 win_sw                (WINDOW, INT, INT, INT, INT, CHARACTER*);
WINDOW               win_get_active        (void);
BOOL                 win_active            (WINDOW);
BOOL                 win_visible           (WINDOW);
#ifdef __cplusplus
}
#endif
#endif

