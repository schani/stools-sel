/***************************************************************************
 ***************************************************************************
 ***                                                                     ***
 ***                       Schani-Tools fÅr C                            ***
 ***                                                                     ***
 ***                Headerdatei fÅr fertige Dialogboxen                  ***
 ***                                                                     ***
 ***                (c) 1990-93 by Schani Electronics                    ***
 ***                                                                     ***
 ***************************************************************************
 ***************************************************************************/

/***************************************************************************
 *                           Datenstrukturen                               *
 ***************************************************************************/

typedef struct
{
  CHAR                 *pcFileName;
  CHAR                **ppcFirstLine;
  CHAR                 *pcBuffer;
  CHAR                 *pcPath;
  CHAR                 *pcMask;
  UTL_DIRECTORY_ENTRY  *pdeTable;
} BOX_FILE_WIN;

/***************************************************************************
 *                             Konstanten                                  *
 ***************************************************************************/

#define BOX_OK            1
#define BOX_CANCEL        2
#define BOX_HELP          4
#define BOX_MOUSE_SPEED   5
#define BOX_MOUSE_CLICK   6
#define BOX_PALETTE       7
#define BOX_DRIVE         8
#define BOX_FILE_NAME     9
#define BOX_DRIVES       10
#define BOX_FILES        11
#define BOX_SOUND        12
#define BOX_SOUND_DIGI   13
#define BOX_SOUND_LENGTH 14
#define BOX_SOUND_FREQ   15
#define BOX_DIGI_NAME    16
#define BOX_SEARCH_DIGI  17
#define BOX_SOUND_TEST   18
#define BOX_COLOR        19
#define BOX_COLOR_WINDOW 20
#define BOX_COLOR_CHAR   21
#define BOX_COLOR_BACK   22
#define BOX_SAVER_ACTIVE 23
#define BOX_SAVER_TIME   24
#define BOX_HELP_HELP    25
#define BOX_HELP_BACK    26
#define BOX_EDIT_EDIT    27

#define BOX_INFO          PAL_WIN_INFO
#define BOX_WARNING       PAL_WIN_WARNING
#define BOX_ERROR         PAL_WIN_ERROR

#define M_HELP_HELP      150
#define M_HELP_BACK      151
#define M_CHANGE_CONTEXT 152

/***************************************************************************
 *                        Funktionsprototypen                              *
 ***************************************************************************/
#ifdef __cplusplus
extern "C" 
{
#endif
static void  int_box_file_window  (WINDOW, UTL_EVENT*);
static void  int_box_color_window (WINDOW, UTL_EVENT*);
static void  int_box_edit_window  (WINDOW, UTL_EVENT*);
void         box_init             (void);
UINT         box_info             (UCHAR, UINT, CHAR*, DWORD);
BOOL         box_mouse            (void);
BOOL         box_palette          (void);
BOOL         box_drives           (void);
BOOL         box_load_save        (CHAR*, CHAR*);
BOOL         box_beep             (BOOL);
BOOL         box_custom_palette   (void);
BOOL         box_screen_saver     (void);
BOOL         box_modal_help       (DWORD);
BOOL         box_help             (DWORD);
BOOL         box_edit             (CHAR*, CHAR*, ULONG, BOOL, ULONG);
#ifdef __cplusplus
}
#endif
