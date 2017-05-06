/***************************************************************************
 ***************************************************************************
 ***                                                                     ***
 ***                       SE-Menu Headerdatei                           ***
 ***                                                                     ***
 ***                (c) 1990-92 by Schani Electronics                    ***
 ***                                                                     ***
 ***************************************************************************
 ***************************************************************************/

#define IN_CLICK_FIELD   ((eventEvent.uiHor > (win_get_aktiv()->iX + 1)) && \
			 (eventEvent.uiHor < (win_get_aktiv()->iX + \
			 win_get_aktiv()->uiWidth - 2)) && \
			 (eventEvent.uiVer > (win_get_aktiv()->iY + 1)) && \
			 (eventEvent.uiVer < (win_get_aktiv()->iY + \
			 win_get_aktiv()->uiHeight - 2)))

#define HAUPTMENU             1

#define   DATEI            1000
#define     ID_OEFFNEN     1001
#define     ID_SCHLIESSEN  1002
       /***********************/
#define     ID_UEBER       1003
       /***********************/
#define     ID_SHUTDOWN    1004
#define     ID_ENDE        1005

#define   PROGRAMM         1100
#define     ID_DAZUFUEGEN  1101
#define     ID_EINFUEGEN   1102
#define     ID_BEARBEITEN  1103
#define     ID_PRG_BEWEGEN 1104
       /***********************/
#define     ID_LOESCHEN    1105
       /***********************/
#define     ID_SORT_ALPHA  1106
       /***********************/
#define     ID_BEWEGEN     1107
#define     ID_TITEL       1108

#define   TERMINE          1200
#define     ID_KALENDER    1201
#define     ID_REMEMBER    1202
       /***********************/
#define     ID_OPTIONS     1203

#define   USER             1300
#define     ID_LOGOUT      1301
       /***********************/
#define     ID_PASSWORD    1302
#define     ID_SEND_MAIL   1303
       /***********************/
#define     ID_USER_EDIT   1304

#define   OPTIONEN         1400
#define     ID_FARBPALETTE 1401
#define     ID_CUST_COLORS 1402
#define     ID_MAUSTEMPO   1403
#define     ID_WARNTON     1404
#define     ID_SCR_SAVER   1405
#define     ID_SOUNDS      1406
#define     ID_SONSTIGES   1407
       /***********************/
#define     ID_SUPER_OPT   1408

#define BOX_PROGRAMM         10
#define BOX_GRUPPE           11
#define BOX_SEARCH           12
#define BOX_EDIT             13
#define BOX_DELETE           14
#define BOX_ADD              15
#define BOX_CHANGE_PWD       16
#define BOX_SEARCH_START     17
#define BOX_SEARCH_LOGOUT    18
#define BOX_LINK             19
#define BOX_COPY             20
#define BOX_CHANGE           21
#define BOX_MEMO             22

#define RIGHT_SUPERVISOR      1
#define RIGHT_CHANGE          2
#define RIGHT_PWD_CHANGE      4
#define RIGHT_QUIT            8

#define REM_NONE              0
#define REM_DAILY             1
#define REM_START             2

#define USER_SIZE           498

#define CANCEL            64000

typedef struct
{
  UCHAR aucName[61];
  UCHAR aucHelpLine[79];
  UCHAR aucWorkDir[129];
  UCHAR aucCmdLine[129];
  UCHAR ucProgramm;
  UINT  uiGruppe;
} MNU_EINTRAG;

typedef struct __MNU_MENU
{
  UCHAR              ucEintraege;
  MNU_EINTRAG        ameEintrag[18];
  WINDOW             winWindow;
  UINT               uiGruppe;
  UCHAR              ucAktItem;
  UCHAR              ucChanged;
  struct __MNU_MENU *pmenuUnterMenu;
  struct __MNU_MENU *pmenuOberMenu;
} _MNU_MENU;

typedef struct
{
  UCHAR *pucName;
  UCHAR *pucHelpLine;
} MNU_ZEILE;

typedef struct
{
  UCHAR ucProgramm;
  UCHAR aucName[61];
  UCHAR aucHelpLine[79];
  UCHAR aucWorkDir[129];
  UCHAR aucCmdLine[129];
  UINT  uiGruppe;
  UINT  uiNext;
} MNU_DISK_EINTRAG;

typedef struct
{
  UINT  uiFirst;
  UCHAR aucTitel[51];
  CHAR  cWinX;
  CHAR  cWinY;
  UCHAR aucDummy[348];
} MNU_DISK_MENU;

typedef struct
{
  UCHAR aucName[21];
  UCHAR aucPassword[21];
  UCHAR aucItems[30];
  UINT  uiRights;
} MNU_USER;

typedef struct
{
  UCHAR aucFrom[21];
  UCHAR aucTo[21];
  UCHAR aucSubject[61];
  UCHAR ucDay;
  UCHAR ucMonth;
  UINT  uiYear;
  UINT  uiLength;
} MNU_MESSAGE;

typedef struct
{
  UCHAR ucDay;
  UCHAR ucMonth;
  UINT  uiYear;
  UCHAR ucHour;
  UCHAR ucMinute;
} MNU_DATE;

typedef struct
{
  MNU_DATE dtDate;
  CHAR     acText[51];
  UINT     uiMemoID;
  UINT     uiNext;
} MNU_DISK_TERMIN;

typedef struct _MNU_TERMIN
{
  UINT                uiID;
  MNU_DATE            dtDate;
  CHAR                acText[51];
  UINT                uiMemoID;
  struct _MNU_TERMIN *ptrmNext;
} MNU_TERMIN;

UCHAR       rotate_left       (UCHAR, UCHAR);
void        decrypt_password  (UCHAR*);
void        encrypt_password  (UCHAR*);
UCHAR       get_user          (UCHAR*, UINT*);
UCHAR       password_right    (UINT, UCHAR*);
void        read_user_info    (UINT, MNU_USER*);
UCHAR       read_user         (UINT, MNU_USER*);
void        write_user_info   (UINT, MNU_USER*);
void        write_user        (UINT, MNU_USER*);
UINT        open_user_file    (void);
UCHAR*      get_message       (MNU_MESSAGE*);
void        write_message     (MNU_MESSAGE*, UCHAR*);
void        open_message_file (void);
UINT        get_next_free     (void);
void        set_usage         (UINT, UCHAR);
UCHAR       entry_used        (UINT);
void        read_entry        (UINT, void*);
void        write_entry       (UINT, void*);
UINT        get_menu          (UINT);
void        set_menu          (UINT, UINT);
UINT        copy_menu         (UINT);
void        delete_menu       (UINT);
void        delete_user       (UINT);
void        open_termin_file  (void);
UINT        next_free_termin  (void);
UINT        get_termin        (UINT);
void        set_termin        (UINT, UINT);
void        read_termin       (UINT, MNU_DISK_TERMIN*);
void        write_termin      (UINT, MNU_DISK_TERMIN*);
MNU_TERMIN* termin_list       (void);
MNU_TERMIN* remember_list     (void);
UCHAR       next_termin       (MNU_TERMIN*);
void        add_termin        (UINT, MNU_DISK_TERMIN*);
void        delete_termin     (UINT, UINT);
void        termin_warning    (void);
void        login_user        (void);
UCHAR       has_password      (UINT);
void        save_menus        (void);
void        clear_desktop     (void);
void        uhr_zeigen        (void);
void        park              (void);
void        execute           (UCHAR*, UCHAR*);
UCHAR       today             (struct dosdate_t*);
UCHAR       this_week         (struct dosdate_t*);
UCHAR       this_month        (struct dosdate_t*);
INT         datecmp           (MNU_DATE*, MNU_DATE*);
void        date_to_string    (MNU_DATE*, CHAR*);
UCHAR       string_to_date    (MNU_DATE*, CHAR*);
void        time_to_string    (MNU_DATE*, CHAR*);
UCHAR       string_to_time    (MNU_DATE*, CHAR*);
UINT        list_length       (MNU_TERMIN*);
CHAR**      list_to_array     (MNU_TERMIN*);
void        read_config       (void);
void        write_config      (void);
void        read_menu         (UINT, _MNU_MENU*, _MNU_MENU*);
void        write_menu        (_MNU_MENU*);
UINT        open_menu_file    (UINT);
UINT        create_menu       (UCHAR*);
void        chain_menu        (_MNU_MENU*, UCHAR);
void        make_window       (_MNU_MENU*, CHAR, CHAR, UCHAR*);
UCHAR       get_longest_name  (_MNU_MENU*, UCHAR*);
void        get_items         (void);
void        init_haupt_menu   (void);
void        init_screen       (void);
void        show_window       (_MNU_MENU*);
void        show_windows      (void);
UCHAR       get_grp_prg       (void);
void        show_mail         (void);
void        run               (void);
void        bewegen           (_MNU_MENU*, MNU_EINTRAG**, UCHAR);
UCHAR       auswahl           (UINT*, UCHAR*, UCHAR);
UCHAR*      message_box       (MNU_MESSAGE*);
void        send_mail         (void);
void        show_message      (UCHAR*, MNU_MESSAGE*);
UCHAR       gruppen_box       (UCHAR*, UCHAR*, UCHAR*);
UCHAR       programm_box      (UCHAR*, UCHAR*, UCHAR*, UCHAR*, UCHAR*);
UCHAR       titel_box         (UCHAR*);
UCHAR       zeichen_gueltig   (UCHAR);
UCHAR       datei_gueltig     (UCHAR*);
void        write_help        (UCHAR*);
void        del_gruppe        (UINT);
void        sonstiges_box     (void);
void        supervisor_box    (void);
void        user_box          (UCHAR*);
UCHAR       password_box      (UCHAR*, UCHAR*);
void        user_list         (UCHAR***);
void        user_editor       (void);
UCHAR       edit_user         (UCHAR*, MNU_USER*);
UINT        select_user       (CHAR*, CHAR*);
void        termin_box        (void);
UCHAR       edit_termin_box   (UCHAR, MNU_TERMIN*);
void        remember_box      (void);
UCHAR       memo_box          (CHAR*, UCHAR);
void        termin_option_box (void);
void        ende              (UINT);
