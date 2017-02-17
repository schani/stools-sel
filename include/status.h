/***************************************************************************
 ***************************************************************************
 ***                                                                     ***
 ***                      Schani-Tools f�r C                             ***
 ***                                                                     ***
 ***               Headerdatei f�r Statuszeilen-Teil                     ***
 ***                                                                     ***
 ***               (c) 1990-93 by Schani Electronics                     ***
 ***                                                                     ***
 ***************************************************************************
 ***************************************************************************/

/***************************************************************************
 *                            Konstanten                                   *
 ***************************************************************************/

#define STS_STACK_SIZE     10
#define STS_END            { NULL, 0, 0, 0 }

/***************************************************************************
 *                     Allgemeine Datenstrukturen                          *
 ***************************************************************************/

typedef struct
{
  CHAR  *pcText;
  UINT   uiID;
  WORD   wHotKey;
  BOOL   bActive;
} STS_ITEM;

/***************************************************************************
 *                      Interne Datenstrukturen                            *
 ***************************************************************************/

typedef struct
{
  BOOL      bHelpLine;
  CHAR     *pcHelpLine;
  STS_ITEM *pitemItems;
} STS_LINE_SAVE;

/***************************************************************************
 *                       Funktionsprototypen                               *
 ***************************************************************************/

#ifdef __cplusplus
extern "C"
{
#endif
void  static int_sts_display_help_line   (void);
INT   static int_sts_get_item_x          (INT);
void  static int_sts_display_item        (CHAR*, INT, BOOL);
void  static int_sts_display_active_item (CHAR*, INT);
void  static int_sts_display_items       (void);
INT   static int_sts_get_index           (INT);
void         sts_pre_process             (UTL_EVENT*);
void         sts_write_help              (CHAR*);
void         sts_clear_help              (void);
BOOL         sts_new_status_line         (STS_ITEM*);
void         sts_del_status_line         (void);
void         sts_event                   (UTL_EVENT*);
void         sts_redraw                  (void);
BOOL         sts_active                  (UINT, UCHAR);
#ifdef __cplusplus
}
#endif
