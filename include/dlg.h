/***************************************************************************
 ***************************************************************************
 ***                                                                     ***
 ***                       Schani-Tools fr C                            ***
 ***                                                                     ***
 ***                  Headerdatei fr Dialogbox-Teil                     ***
 ***                                                                     ***
 ***                (c) 1990-93 by Schani Electronics                    ***
 ***                                                                     ***
 ***************************************************************************
 ***************************************************************************/

#ifndef _DEFINED_WINDOW
#define _DEFINED_WINDOW
struct _WB;
typedef struct _WB*       WINDOW;
#endif

#ifndef _DEFINED_EVENT
#define _DEFINED_EVENT
typedef struct _UTL_EVENT UTL_EVENT;
#endif

/***************************************************************************
 *                            Konstanten                                   *
 ***************************************************************************/

#define M_SET_VALUES             101
#define M_QUERY_VALUES           102 
#define M_SET_DISPLAY            103

#define M_PUSH_BUT_CHANGED       104

#define M_RADIO_BUT_CHANGED      105

#define M_ACT_BUT_CLICKED        106

#define M_SCROLL_UP              107
#define M_SCROLL_DOWN            108
#define M_SCROLL_PAGE_UP         109
#define M_SCROLL_PAGE_DOWN       110
#define M_SCROLL_BEGIN_DRAGGING  111
#define M_SCROLL_CHANGED         112
#define M_SCROLL_END_DRAGGING    113

#define M_LIST_BOX_CHANGED       114
#define M_LIST_BOX_CLICKED       115
#define M_LIST_BOX_NEW_LIST      116

#define M_COLOR_FIELD_CHANGED    117

#define M_TEXT_FIELD_CHANGED     118

#define M_HELP_CONTEXT_CHANGED   119

#define M_EDITOR_CHANGED         120

/***************************************************************************
 *                             Makros                                      *
 ***************************************************************************/

#define DLG_RADIO_END     { { 0, 0 }, 0, NULL }

#define DLG_HORIZONTAL    1
#define DLG_VERTICAL      2

#define MAX_LINE_LENGTH   1023

#define OWNER             pelementElement->winOwner
#define RECIPIENT         pelementElement->pRecipient ?                  \
                            pelementElement->pRecipient :                \
                            (void*)pelementElement->winOwner
                            
#define SCROLL_X(p)       ((pscroll->ucType == DLG_HORIZONTAL) ?                                \
                           (pscroll->coord.iX + p + 1) : (pscroll->coord.iX))                        
#define SCROLL_Y(p)       ((pscroll->ucType == DLG_HORIZONTAL) ?                                \
                           (pscroll->coord.iY) : (pscroll->coord.iY + p + 1))       
#define UP_ARROW          (CHAR)((pscroll->ucType == DLG_HORIZONTAL) ? 27 : 24)
#define DOWN_ARROW        (CHAR)((pscroll->ucType == DLG_HORIZONTAL) ? 26 : 25)

#define DRAW_MARKER       win_sw_a(OWNER, plist->reg.iX + 1,                                                   \
                                   plist->reg.iY + plist->iPos - plist->iFirstLine + 1,                        \
                                   plist->reg.iWidth - 2, 1, win_get_color(OWNER, PAL_COL_LIST_BOX_SEL_ITEM))
#define CLEAR_MARKER      win_sw_a(OWNER, plist->reg.iX + 1,                                                   \
                                   plist->reg.iY + plist->iPos - plist->iFirstLine + 1,                        \
                                   plist->reg.iWidth - 2, 1, win_get_color(OWNER, PAL_COL_LIST_BOX_ITEM))
#define SET_CURSOR        win_set_cursor_pos(OWNER, plist->reg.iX + 2,                                         \
                                             plist->reg.iY + plist->iPos - plist->iFirstLine + 1)
#define SET_SCROLL        glb_send_message(pelementElement, plist->pelementScroll, M_SET_VALUES,               \
                                           min(plist->iPos / max(plist->iC,1), plist->reg.iHeight - 5))
                                           
#define IN_COLOR_FIELD    (eventEvent.iHor >= 0 && eventEvent.iHor < 12 && eventEvent.iVer >= 0 &&             \
                           ((pcf->bIntensive && eventEvent.iVer < 4) ||                                        \
			    (!pcf->bIntensive && eventEvent.iVer < 2)))

#define FOCUSSED          pelementElement->flFlags.binFocussed          

#define OWNER_FOCUSSED    (prgProgram.winFocussed == OWNER &&            \
                           !OWNER->flFlags.binDragging &&                \
                           !OWNER->flFlags.binSizing)

#define SET_HELP_CURSOR   if (FOCUSSED)                                  \
                            win_set_cursor_pos(OWNER, phelpbox->reg.iX + phelpbox->coordCursor.iX + 1,  \
                                               phelpbox->reg.iY + phelpbox->coordCursor.iY + 1)

#define SET_HELP_SCROLL   {                                                                             \
                            glb_send_message(pelementElement, phelpbox->pelementScrollH, M_SET_VALUES,  \
                                             min(phelpbox->iFirstChar / max(1,phelpbox->iHC),           \
                                             phelpbox->reg.iWidth - 5));                                \
                            glb_send_message(pelementElement, phelpbox->pelementScrollV, M_SET_VALUES,  \
                                             min(phelpbox->iFirstLine / max(1,phelpbox->iVC),           \
                                             phelpbox->reg.iHeight - 5));                               \
                          }
                          
#define SET_EDIT_SCROLL   {                                                                             \
                            glb_send_message(pelementElement, pedit->pelementScrollH, M_SET_VALUES,     \
                                             min((pedit->iRow - 1) / max(pedit->iHC,1),                 \
                                                 pedit->reg.iWidth - 5));                               \
                            glb_send_message(pelementElement, pedit->pelementScrollV, M_SET_VALUES,     \
                                             min((pedit->iLine - 1) / max(pedit->iVC,1),                \
                                                 pedit->reg.iHeight - 5));                              \
                          }

#define dlg_push_button_set_values(e,s)       glb_send_message(NULL, e, M_SET_VALUES, s)
#define dlg_push_button_query_values(e,s)     glb_send_message(NULL, e,  M_QUERY_VALUES, s)
#define dlg_push_button_set_display(e,x,y)    {                                                         \
                                                VIO_COORD coord;                                        \
                                                                                                        \
                                                coord.iX = x;                                           \
                                                coord.iY = y;                                           \
                                                glb_send_message(NULL, e, M_SET_DISPLAY, &coord);       \
                                              }

#define dlg_radio_button_set_values(e,s)      glb_send_message(NULL, e, M_SET_VALUES, s)
#define dlg_radio_button_query_values(e,s)    glb_send_message(NULL, e, M_QUERY_VALUES, s)
#define dlg_radio_button_set_display(e,x,y)   {                                                         \
                                                VIO_COORD coord;                                        \
                                                                                                        \
                                                coord.iX = x;                                           \
                                                coord.iY = y;                                           \
                                                glb_send_message(NULL, e, M_SET_DISPLAY, &coord);       \
                                              }

#define dlg_act_button_set_display(e,x,y)     {                                                         \
                                                VIO_COORD coord;                                        \
                                                                                                        \
                                                coord.iX = x;                                           \
                                                coord.iY = y;                                           \
                                                glb_send_message(NULL, e, M_SET_DISPLAY, &coord);       \
                                              }

#define dlg_scroll_set_values(e,s)            glb_send_message(NULL, e, M_SET_VALUES, s)
#define dlg_scroll_query_values(e,s)          glb_send_message(NULL, e, M_QUERY_VALUES, s)
#define dlg_scroll_set_display(e,x,y,l)       {                                                         \
                                                VIO_REGION reg;                                         \
                                                                                                        \
                                                reg.iX = x;                                             \
                                                reg.iY = y;                                             \
                                                reg.iWidth = l;                                         \
                                                glb_send_message(NULL, e, M_SET_DISPLAY, &reg);         \
                                              }

#define dlg_label_set_display(e,x,y)          {                                                         \
                                                VIO_COORD coord;                                        \
                                                                                                        \
                                                coord.iX = x;                                           \
                                                coord.iY = y;                                           \
                                                glb_send_message(NULL, e, M_SET_DISPLAY, &coord);       \
                                              }

#define dlg_border_set_display(e,x,y,w,h)     {                                                         \
                                                VIO_REGION reg;                                         \
                                                                                                        \
                                                reg.iX = x;                                             \
                                                reg.iY = y;                                             \
                                                reg.iWidth = w;                                         \
                                                reg.iHeight = h;                                        \
                                                glb_send_message(NULL, e, M_SET_DISPLAY, &reg);         \
                                              }

#define dlg_list_box_set_values(e,s)          glb_send_message(NULL, e, M_SET_VALUES, s)
#define dlg_list_box_query_values(e,s)        glb_send_message(NULL, e, M_QUERY_VALUES, s)
#define dlg_list_box_new_list(e,l)            glb_send_message(NULL, e, M_LIST_BOX_NEW_LIST, l)
#define dlg_list_box_set_display(e,x,y,w,h)   {                                                         \
                                                VIO_REGION reg;                                         \
                                                                                                        \
                                                reg.iX = x;                                             \
                                                reg.iY = y;                                             \
                                                reg.iWidth = w;                                         \
                                                reg.iHeight = h;                                        \
                                                glb_send_message(NULL, e, M_SET_DISPLAY, &reg);         \
                                              }

#define dlg_color_field_set_values(e,s)       glb_send_message(NULL, e, M_SET_VALUES, s)
#define dlg_color_field_query_values(e,s)     glb_send_message(NULL, e, M_QUERY_VALUES, s)
#define dlg_color_field_set_display(e,x,y)    {                                                         \
                                                VIO_COORD coord;                                        \
                                                                                                        \
                                                coord.iX = x;                                           \
                                                coord.iY = y;                                           \
                                                glb_send_message(NULL, e, M_SET_DISPLAY, &coord);       \
                                              }

#define dlg_text_field_set_values(e,s)        glb_send_message(NULL, e, M_SET_VALUES, s)
#define dlg_text_field_query_values(e,s)      glb_send_message(NULL, e, M_QUERY_VALUES, s)
#define dlg_text_field_set_display(e,x,y,w)   {                                                         \
                                                VIO_REGION reg;                                         \
                                                                                                        \
                                                reg.iX = x;                                             \
                                                reg.iY = y;                                             \
                                                reg.iWidth = w;                                         \
                                                glb_send_message(NULL, e, M_SET_DISPLAY, &reg);         \
                                              }

#define dlg_help_box_set_values(e,c)          glb_send_message(NULL, e, M_SET_VALUES, c)
#define dlg_help_box_query_values(e,c)        glb_send_message(NULL, e, M_QUERY_VALUES, c)
#define dlg_help_box_set_display(e,x,y,w,h)   {	                                                        \
                                                VIO_REGION reg;                                         \
 						                                                        \
                                                reg.iX = x;                                             \
                                                reg.iY = y;                                             \
                                                reg.iWidth = w;                                         \
                                                reg.iHeight = h;                                        \
                                                glb_send_message(NULL, e, M_SET_DISPLAY, &reg);         \
                                              }

#define dlg_editor_query_values(e,c)        glb_send_message(NULL, e, M_QUERY_VALUES, c);
#define dlg_editor_set_display(e,x,y,w,h)   {                                                           \
                                              VIO_REGION reg;                                           \
                                                                                                        \
                                              reg.iX = x;                                               \
                                              reg.iY = y;                                               \
                                              reg.iWidth = w;                                           \
                                              reg.iHeight = h;                                          \
                                              glb_send_message(NULL, e, M_SET_DISPLAY, &reg);           \
                                            }

/***************************************************************************
 *                     Allgemeine Datenstrukturen                          *
 ***************************************************************************/

typedef struct
{
  UINT binCanBeActivated : 1;
  UINT binFocussed       : 1;
} DLG_FLAGS;

typedef struct _DLG_ELEMENT
{
  void                (*handle_event)(struct _DLG_ELEMENT*, UTL_EVENT*);
  DLG_FLAGS             flFlags;
  UINT                  uiID;
  CHAR                 *pcHelpLine;
  void                 *pAddInfo;
  struct _DLG_ELEMENT  *pelementNext;
  struct _DLG_ELEMENT  *pelementPrev;
  WINDOW                winOwner;
  void                 *pRecipient;
} DLG_ELEMENT;

/***************************************************************************
 *                      Interne Datenstrukturen                            *
 ***************************************************************************/

typedef struct
{
  VIO_COORD  coord;
  BOOL       bActive;
  WORD       wHotKey;
  BOOL       bButtonDown;
  CHAR      *pcText;
} DLG_PUSH_BUTTON;

typedef struct
{
  VIO_COORD  coord;
  WORD       wHotKey;
  CHAR      *pcText;
} DLG_BUTTON;

typedef struct
{
  INT         iActive;
  BOOL        bButtonDown;
  DLG_BUTTON *pbutFirst;
} DLG_RADIO_BUTTON;

typedef struct
{
  VIO_COORD  coord;
  BOOL       bPassword;
  INT        iDispLength;
  INT        iRealLength;
  INT        iLength;
  INT        iFirstChar;
  INT        iPos;
  BOOL       bMarked;
  BOOL       bDragging;
  INT        iMarkStart;
  INT        iMarkLength;
  CHAR      *pcInput;
} DLG_TEXT_FIELD;

typedef struct
{
  VIO_COORD coord;
  UCHAR     ucType;
  INT       iLength;
  INT       iPos;
  DWORD     dwNextReaction;
  BOOL      bButtonDown;
  BOOL      bDragging;                           
  BOOL      bNoClipping;
} DLG_SCROLL;

typedef struct
{
  VIO_REGION    reg;
  INT           iFirstLine;
  INT           iPos;
  INT           iElements;       
  BOOL          bDragging;  
  INT           iC;
  CHAR        **ppcFirstLine;
  DLG_ELEMENT  *pelementScroll;  
  DLG_ELEMENT  *pelementBorder;
} DLG_LIST_BOX;

typedef struct
{
  VIO_COORD coord;
  BOOL      bIntensive;
  CHAR      cColor;
  BOOL      bDragging;
} DLG_COLOR_FIELD;

typedef struct
{
  VIO_COORD  coord;
  INT        iLength;
  CHAR      *pcText;
  BOOL       bButtonDown;
  WORD       wHotKey;
} DLG_ACTION_BUTTON;             

typedef struct
{
  VIO_COORD    coord;
  CHAR        *pcText;
  WORD         wHotKey;
  DLG_ELEMENT *pelementElement;
} DLG_LABEL;

typedef struct
{
  VIO_REGION reg;
} DLG_BORDER;                       

typedef struct
{
  VIO_REGION   reg;
  VIO_COORD    coordCursor;
  INT          iFirstLine;
  INT          iFirstChar;    
  DWORD        dwContext;
  BOOL         bDragging;    
  INT          iHC;
  INT          iVC;
  CHARACTER   *pcharBuffer;
  HLP_LINK    *plnkActLink;
  HLP_CONTEXT *pctxContext;              
  DLG_ELEMENT *pelementBorder;
  DLG_ELEMENT *pelementScrollH;
  DLG_ELEMENT *pelementScrollV;
} DLG_HELP_BOX;           

typedef struct
{
  VIO_REGION   reg;
  VIO_COORD    coordCursor;
  INT          iLine;
  INT          iRow;
  INT          iLines;
  ULONG        ulBufferLength;
  ULONG        ulTextLength;
  ULONG        ulGapLength;
  INT          iLeftChar;         
  INT          iHC;
  INT          iVC;
  CHAR        *pcCursor;
  CHAR        *pcFirstLine;
  CHAR        *pcBuffer;
  BOOL         bCanChange;
  BOOL         bDragging;
  DLG_ELEMENT *pelementScrollH;
  DLG_ELEMENT *pelementScrollV;
  DLG_ELEMENT *pelementBorder;
} DLG_EDITOR;

/***************************************************************************
 *                       Funktionsprototypen                               *
 ***************************************************************************/

#ifdef __cplusplus
extern "C"
{
#endif
static CHAR         int_dlg_get_attri                (DLG_ELEMENT*, BYTE);
static HLP_LINK*    int_dlg_check_link               (DLG_ELEMENT*);
static void         int_dlg_hide_link                (DLG_ELEMENT*, HLP_LINK*);
static void         int_dlg_show_link                (DLG_ELEMENT*, HLP_LINK*);
static void         int_dlg_make_line_buffer         (DLG_ELEMENT*, INT);
static void         int_dlg_make_color_buffer        (DLG_ELEMENT*, INT);
static void         int_dlg_draw_line                (DLG_ELEMENT*, INT);
static void         int_dlg_draw_inner               (DLG_ELEMENT*);

static INT          int_dlg_editor_count_lines       (DLG_ELEMENT*);
static CHAR*        int_dlg_editor_get_line          (DLG_ELEMENT*, INT);
static void         int_dlg_editor_make_line_buffer  (DLG_ELEMENT*, CHAR*);
static CHAR*        int_dlg_editor_next_line         (DLG_ELEMENT*, CHAR*);
static CHAR*        int_dlg_editor_prev_line         (DLG_ELEMENT*, CHAR*);
static void         int_dlg_editor_draw              (DLG_ELEMENT*, INT, INT);
static void         int_dlg_editor_set_cursor        (DLG_ELEMENT*);
static INT          int_dlg_editor_len_to_end        (DLG_ELEMENT*);
static void         int_dlg_editor_go_char           (DLG_ELEMENT*, INT);
static INT          int_dlg_editor_line_length       (DLG_ELEMENT*);
static void         int_dlg_editor_go_line           (DLG_ELEMENT*, INT);
static void         int_dlg_editor_insert_char       (DLG_ELEMENT*, CHAR);
static void         int_dlg_editor_del_char          (DLG_ELEMENT*, BOOL);
static void         int_dlg_editor_del_line          (DLG_ELEMENT*);

void                dlg_set_focus                    (WINDOW, DLG_ELEMENT*);
void                dlg_activate_next                (WINDOW);
void                dlg_activate_prev                (WINDOW);
BOOL                dlg_hot_key                      (UTL_EVENT*, WORD);

void                dlg_push_but_handle_event        (DLG_ELEMENT*, UTL_EVENT*);
void                dlg_radio_but_handle_event       (DLG_ELEMENT*, UTL_EVENT*);
void                dlg_act_but_handle_event         (DLG_ELEMENT*, UTL_EVENT*);  
void                dlg_scroll_handle_event          (DLG_ELEMENT*, UTL_EVENT*);
void                dlg_label_handle_event           (DLG_ELEMENT*, UTL_EVENT*);
void                dlg_border_handle_event          (DLG_ELEMENT*, UTL_EVENT*);
void                dlg_list_box_handle_event        (DLG_ELEMENT*, UTL_EVENT*);
void                dlg_color_field_handle_event     (DLG_ELEMENT*, UTL_EVENT*);
void                dlg_text_field_handle_event      (DLG_ELEMENT*, UTL_EVENT*);
void                dlg_help_box_handle_event        (DLG_ELEMENT*, UTL_EVENT*);
void                dlg_editor_handle_event          (DLG_ELEMENT*, UTL_EVENT*);

DLG_ELEMENT*        dlg_init_push_button             (INT, INT, CHAR*, BOOL, CHAR*, UINT, BOOL, void*);

DLG_ELEMENT*        dlg_init_radio_button            (DLG_BUTTON*, INT, CHAR*, UINT, BOOL, void*);

DLG_ELEMENT*        dlg_init_act_button              (INT, INT, CHAR*, WORD, CHAR*, UINT, BOOL, void*);

DLG_ELEMENT*        dlg_init_scroll                  (INT, INT, INT, INT, UCHAR, CHAR*, UINT, BOOL, void*);

DLG_ELEMENT*        dlg_init_label                   (INT, INT, CHAR*, DLG_ELEMENT*, UINT);

DLG_ELEMENT*        dlg_init_border                  (INT, INT, INT, INT, UINT);

DLG_ELEMENT*        dlg_init_list_box                (INT, INT, INT, INT, CHAR**, BOOL, CHAR*, UINT, BOOL,
						      void*);

DLG_ELEMENT*        dlg_init_color_field             (INT, INT, BOOL, CHAR, CHAR*, UINT, BOOL, void*);

DLG_ELEMENT*        dlg_init_text_field              (INT, INT, INT, INT, BOOL, CHAR*, CHAR*, UINT, BOOL,
						      void*);

DLG_ELEMENT*        dlg_init_help_box                (INT, INT, INT, INT, DWORD, BOOL, CHAR*, UINT, BOOL,
                                                      void*);

DLG_ELEMENT*        dlg_init_editor                  (INT, INT, INT, INT, ULONG, CHAR*, BOOL, BOOL, CHAR*,
                                                      UINT, BOOL, void*);
#ifdef __cplusplus
}
#endif
