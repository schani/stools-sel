/***************************************************************************
 ***************************************************************************
 ***                                                                     ***
 ***                      Schani-Tools fÅr C                             ***
 ***                                                                     ***
 ***                  Headerdatei fÅr Utility-Teil                       ***
 ***                                                                     ***
 ***               (c) 1990-93 by Schani Electronics                     ***
 ***                                                                     ***
 ***************************************************************************
 ***************************************************************************/

#include <stdlib.h>
#ifdef _MSDOS
#ifdef __TURBOC__
#include <dos.h>
#include <dir.h>
#include <alloc.h>
#endif
#if defined _MSC_VER || defined _QC
#include <malloc.h>
#endif
#endif

/***************************************************************************
 *                          Datenstrukturen                                *
 ***************************************************************************/

struct _UTL_EVENT
{
  UINT   uiKind;
  INT    iHor;
  INT    iVer;
  WORD   wButtons;
  DWORD  dwTimer;
  BYTE   byKeybStatus;
  WORD   wKey;
  UINT   uiMessage;
  void  *pFrom;
  ULONG  ulAddInfo;
};

#ifndef _DEFINED_EVENT
#define _DEFINED_EVENT
typedef struct _UTL_EVENT UTL_EVENT;
#endif

typedef struct
{
  CHAR  acDummy[21];
  UCHAR ucAttribut;
  UINT  uiZeit;
  UINT  uiDatum;
  ULONG ulGroesse;
  CHAR  acName[13];
} UTL_FILE;

typedef struct _UTL_SHORT_CUT
{
  WORD                    wKey;
  void                  (*short_func) (void);
  struct _UTL_SHORT_CUT  *pscNextShort;
} UTL_SHORT_CUT;

typedef struct _UTL_NULL_EVENT
{
  void                   (*event_handler) (void);
  struct _UTL_NULL_EVENT  *pneNextEvent;
} UTL_NULL_EVENT;

typedef struct
{
  UCHAR ucX;
  UCHAR ucY;
} UTL_STERN;

typedef struct
{
  CHAR *pcName;
  BOOL  bSubDir;
} UTL_DIRECTORY_ENTRY;

/***************************************************************************
 *                             Konstanten                                  *
 ***************************************************************************/

#define INSERT_ON          (utl_keyb_status() & INSERT)

#define E_DONE           0
#define E_NULL           1
#define E_MSM_MOVE       2
#define E_MSM_L_DOWN     3
#define E_MSM_L_UP       4
#define E_MSM_L_DOUBLE   5
#define E_MSM_R_DOWN     6
#define E_MSM_R_UP       7
#define E_MSM_R_DOUBLE   8
#define E_MSM_M_DOWN     9
#define E_MSM_M_UP      10
#define E_MSM_M_DOUBLE  11
#define E_KEY           12
#define E_MESSAGE       13

#define M_INIT           1
#define M_QUIT           2
#define M_ACTIVATE       3
#define M_DEACTIVATE     4
#define M_SHOW           5
#define M_HIDE           6
#define M_GET_FOCUS      7
#define M_LOST_FOCUS     8
#define M_MOVE           9
#define M_SIZE           10
#define M_DRAW           11
#define M_CLOSE          12
#define M_REDRAW         13

#define SHIFT_RIGHT    1
#define SHIFT_LEFT     2
#define SHIFT          3
#define CTRL           4
#define ALT            8
#define SCROLL_LOCK   16
#define NUM_LOCK      32
#define CAPS_LOCK     64
#define INSERT       128

#define UTL_SUBDIR              0x10

#define UTL_SS_EBENEN  3
#define UTL_SS_STERNE 10
#define UTL_SS_STERN   '*'

/***************************************************************************
 *                               Makros                                    *
 ***************************************************************************/

#define utl_free_far(p)                              free(p)

#ifdef _MSDOS
#define utl_no_sound()                   utl_sound(0)
#endif
#if defined(_WINNT) || defined(_OS2) || defined(_UNIX)
#define utl_no_sound()
#endif

#define utl_alltrim(pucString)           utl_ltrim(pucString),              \
                                         utl_rtrim(pucString)
#define utl_free(pBlock)                 free(pBlock)

#define IS_CLICK_EVENT(e)                 ((e).uiKind == E_MSM_L_DOWN || (e).uiKind == E_MSM_L_DOUBLE   \
                                          (e).uiKind == E_MSM_R_DOWN || (e).uiKind == E_MSM_R_DOUBLE    \
                                          (e).uiKind == E_MSM_M_DOWN || (e).uiKind == E_MSM_M_DOUBLE)
#define IS_MOUSE_EVENT(e)                 (CLICK_EVENT(e) ||             \
                                          (e).uiKind == E_MSM_L_UP ||    \
                                          (e).uiKind == E_MSM_R_UP ||    \
                                          (e).uiKind == E_MSM_M_UP ||    \
                                          (e).uiKind == E_MSM_MOVE)
#define IS_BORDER_CLICK(e,w)              (CLICK_EVENT(e) &&                 \
                                          ON_BORDER((e).iHor, (e).iVer, w))
#define IS_BORDER_EVENT(e,w)              (MOUSE_EVENT(e) &&                 \
                                          ON_BORDER((e).iHor, (e).iVer, w))

#if (defined(_WINNT) && !defined(_WATCOM)) || defined(_UNIX)
#define far
#endif

/***************************************************************************
 *                        Funktionsprototypen                              *
 ***************************************************************************/

#ifdef __cplusplus
extern "C"
{
#endif
void         utl_init                (void);
void         utl_randomize           (void);
UINT         utl_random              (UINT);
void         utl_std_mem_error       (void);
void         utl_std_scr_saver       (BOOL);
void         utl_set_mem_error       (void (*) (void));
void         utl_set_scr_saver       (void (*) (BOOL));
void         utl_save_screen         (void);
void         utl_delay               (ULONG);
void         utl_tone                (ULONG, ULONG);
void*        utl_alloc               (UINT);
void*        utl_calloc              (UINT, UINT);
void*        utl_realloc             (void*, UINT);
void         utl_sound               (ULONG);
void         utl_beep                (void);
void         utl_set_voc_beep        (CHAR*, BOOL);
void         utl_voc_beep            (void);
void         utl_shadow              (INT, INT, UINT, UINT);
void         utl_border              (INT, INT, INT, INT, UCHAR, CHAR);
BOOL         utl_kb_hit              (void);
BOOL         utl_insert_null_event   (void (*) (void));
void         utl_del_null_event      (void (*) (void));
BOOL         utl_insert_short        (WORD, void (*) (void));
void         utl_del_short           (WORD);
BOOL         utl_short_cuts          (BOOL);
WORD         utl_get_key             (void);
DWORD        utl_get_timer           (void);
void         utl_cls                 (CHAR);
BYTE         utl_keyb_status         (void);
void         utl_strdel              (CHAR*, INT, INT);
void         utl_strins              (CHAR*, INT, CHAR);
void         utl_ltrim               (CHAR*);
void         utl_rtrim               (CHAR*);
CHAR         utl_upper               (CHAR);
CHAR         utl_lower               (CHAR);
CHAR*        utl_str_upper           (CHAR*);
CHAR*        utl_str_lower           (CHAR*);
WORD         utl_alt_code            (CHAR);
CHAR         utl_get_hot_key         (CHAR*);
INT          utl_hot_strlen          (CHAR*);
void         utl_get_path            (CHAR*);
void         utl_set_path            (CHAR*);
void         utl_split_path          (CHAR *path, CHAR *dir, CHAR *name, CHAR *ext);
BOOL         utl_get_files           (CHAR*, CHAR*, UTL_DIRECTORY_ENTRY*, UINT, UINT, BOOL);
BOOL         utl_filename_valid      (CHAR*);
BOOL         utl_get_win_events      (void);
void         utl_set_win_events      (BOOL);
UCHAR        utl_get_drive           (void);
void         utl_set_drive           (UCHAR);
CHAR*        utl_get_drives          (void);
void         utl_move_win            (void);
void         utl_size_win            (void);
void         utl_fill_event          (UTL_EVENT*);
void         utl_event               (UTL_EVENT*);
#ifdef __cplusplus
}
#endif
