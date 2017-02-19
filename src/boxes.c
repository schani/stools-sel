/***************************************************************************
 ***************************************************************************
 ***                                                                     ***
 ***                       Schani-Tools fuer C                           ***
 ***                                                                     ***
 ***                       Fertige Dialogboxen                           ***
 ***                                                                     ***
 ***                (c) 1990-94 by Schani Electronics                    ***
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
#include <sound.h>
#include <boxes.h>
#include <context.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#ifdef _MSDOS
#include <mem.h>
#endif
#include <ctype.h>
#ifdef __TURBOC__
#include <dir.h>
#include <alloc.h>
#endif
#if defined _MSC_VER || defined _QC
#include <malloc.h>
#endif
#include <unistd.h>

static void  int_box_file_window  (WINDOW, UTL_EVENT*);
static void  int_box_color_window (WINDOW, UTL_EVENT*);
static void  int_box_edit_window  (WINDOW, UTL_EVENT*);

extern UINT         uiMouseSpeed;
extern ULONG        ulDoubleClick;
extern BOOL         bBeep;
extern ULONG        ulBeepFrequency;
extern ULONG        ulBeepDuration;
extern BOOL         bSaver;
extern ULONG        ulScreenTimeout;
extern INT          iSizeX;
extern INT          iSizeY;

#ifdef _MSDOS
extern BOOL         bVOCBeep;
extern BOOL         bSoundBlaster;
extern WORD         wStatusWord;
extern CHAR         acBeepVOC[];
#endif

extern BOOL         bExit;             
extern GLB_PROGRAM  prgProgram;
extern WINDOW       winDesktop;
extern CHAR         apcColors[];
extern CHAR        *aapcPalColors[][PAL_COLORS + 1]; 
extern CHAR        *apcPalWindows[];
extern UCHAR        ucActPalette;

CNT_STACK  *psHelpBack;
STS_ITEM    aitemHelpItems[]     =
                                   {
                                     {
                                       "~F1~ Hilfe ""\x81""ber Hilfe",
                                       M_HELP_HELP,
                                       K_F1,
                                       TRUE
                                     },
                                     {
                                       "~Alt-F1~ Zur""\x81""ck",
                                       M_HELP_BACK,
                                       K_A_F1,
                                       TRUE
                                     },
                                     {
                                       "~Esc~ Hilfe beenden",
                                       M_CLOSE,
                                       K_ESC,
                                       TRUE
                                     },
                                     STS_END
                                   };
CHAR       *pcHelp               = " Zeigt einen Hilfetext zu dieser Dialogbox an",
           *pcInfoOK             = " Information best\x84tigen",
           *pcInfoCancel         = " Aktion abbrechen",
           *pcMouseSpeed         = " Geschwindigkeit des Mauscursors",
           *pcMouseClick         = " Geschwindigkeit eines Doppelklicks",
           *pcMouseOK            = " Eingestellte Werte setzen",
           *pcMouseCancel        = " Alte Werte beibehalten",
           *pcPalette            = " Gew\x81nschte Farbpalette",
           *pcPaletteOK          = " Angew\x84hlte Palette setzen",
           *pcPaletteCancel      = " Alte Palette beibehalten",
           *pcDrive              = " Gew\x81nschtes Laufwerk",
           *pcDriveOK            = " Laufwerk ausw\x84hlen",
           *pcDriveCancel        = " Altes Laufwerk beibehalten",
           *pcFileName           = " Gew\x81nschte Datei oder Dateimaske",
           *pcFileFiles          = " Gew\x81nschte Datei",
           *pcFileOK             = " Datei ausw\x84hlen",
           *pcFileCancel         = " Aktion abbrechen",
           *pcFileDrives         = " Laufwerk \x84ndern",
           *pcFileInvalid        = "Der Dateiname ist ung\x81ltig!",
           *pcSound              = " Angabe, ob Warnton erklingen soll",
           *pcSoundDigi          = " Angabe, ob ein Digi-Sound als Warnton dienen soll",
           *pcSoundLength        = " Dauer des Warntones",
           *pcSoundFreq          = " Frequenz des Warntones",
           *pcSoundDigiName      = " Name der VOC-Datei f\x81r Digi-Warnton",
           *pcSoundOK            = " Neue Werte einstellen",
           *pcSoundCancel        = " Alte Werte beibehalten",
           *pcSoundSearchDigi    = " L\x84""\xe1""t Sie in einem Verzeichnisfenster den Digi-Sound suchen",
           *pcSoundTest          = " L\x84""\xe1""t den Warnton erklingen",
           *pcColor              = " Farbe, die Sie \x84ndern wollen",
           *pcColorWindow        = " Fenstertyp, f\x81r den eine Farbe ver\x84ndert werden soll",
           *pcColorChar          = " Vordergrundfarbe",
           *pcColorBack          = " Hintergrundfarbe",
           *pcColorOK            = " šbernimmt die Žnderungen",
           *pcColorCancel        = " Beh\x84lt die alten Farben bei",
           *pcSaverActive        = " Angabe, ob der Bildschirmschoner aktiv ist",
           *pcSaverTime          = " Zeitspanne, nach der sich der Bildschirmschoner aktiviert",
           *pcSaverOK            = " šbernimmt die Žnderungen",
           *pcSaverCancel        = " Beh\x84lt die alten Einstellungen bei",
           *pcHelpHelp           = " Hilfetext",
           *pcHelpCancel         = " Kehrt zum Programm zur""\x81""ck",
           *pcHelpBack           = " Kehrt zum letzten Kontext zur""\x81""ck",
           *pcNAWinNT            = "Diese Funktion ist unter\n"
                                   "Windows NT nicht vorhanden!";
           
static void int_box_modal_help_window (WINDOW winWindow, UTL_EVENT *peventEvent)
{
  DWORD dwContext;

  if (peventEvent->uiKind == E_MESSAGE)
  {
    switch (peventEvent->uiMessage)
    {
      case M_REDRAW :
        win_cls(winWindow);
        win_draw_elements(winWindow);
      case M_INIT :
        peventEvent->uiKind = E_DONE;
        break;
      case M_HELP_CONTEXT_CHANGED :
        cnt_stack_put(psHelpBack, peventEvent->ulAddInfo);
        peventEvent->uiKind = E_DONE;
      case M_ACT_BUT_CLICKED :
        switch (((DLG_ELEMENT*)(peventEvent->pFrom))->uiID)
        {
          case BOX_CANCEL :
            bExit = TRUE;
            break;
          case BOX_HELP :
            dlg_help_box_set_values(win_get_element(winWindow, BOX_HELP_HELP), CTX_BOX_HELP);
            cnt_stack_put(psHelpBack, CTX_BOX_HELP);
            break;
          case BOX_HELP_BACK :
            dwContext = cnt_stack_get(psHelpBack);
            if (!cnt_stack_is_empty(psHelpBack))
            {
              dwContext = cnt_stack_get(psHelpBack);
              dlg_help_box_set_values(win_get_element(winWindow, BOX_HELP_HELP), dwContext);
              cnt_stack_put(psHelpBack, dwContext);
            }
            else
            {
              cnt_stack_put(psHelpBack, dwContext);
              utl_beep();
            }
            break;
        }
        peventEvent->uiKind = E_DONE;
        break;
    }
  }
  if (peventEvent->uiKind != E_DONE)
    win_std_handler(winWindow, peventEvent);
}

static void int_box_help_window (WINDOW winWindow, UTL_EVENT *peventEvent)
{
  DWORD dwContext;

  if (peventEvent->uiKind == E_MESSAGE)
  {
    switch (peventEvent->uiMessage)
    {
      case M_DRAW :
      case M_REDRAW :
        dlg_help_box_set_display(win_get_element(winWindow, BOX_HELP_HELP), 0, 0, winWindow->iWidth,
                                 winWindow->iHeight);
        win_draw_elements(winWindow);
      case M_INIT :
        peventEvent->uiKind = E_DONE;
        break;
      case M_SHOW :
      case M_ACTIVATE :
        win_set_focus(winWindow);
        peventEvent->uiKind = E_DONE;
        break;
      case M_GET_FOCUS :
        win_show(winWindow);
        sts_new_status_line(aitemHelpItems);
        peventEvent->uiKind = E_DONE;
        break;
      case M_LOST_FOCUS :
        sts_del_status_line();
        peventEvent->uiKind = E_DONE;
        break;
      case M_QUIT :
        cnt_stack_destroy((CNT_STACK*)win_get_add_info(winWindow));
        peventEvent->uiKind = E_DONE;
        break;
      case M_HELP_CONTEXT_CHANGED :
        cnt_stack_put((CNT_STACK*)win_get_add_info(winWindow), peventEvent->ulAddInfo);
        peventEvent->uiKind = E_DONE;
        break;
      case M_CLOSE :
        win_delete(winWindow);
        peventEvent->uiKind = E_DONE;
        break;
      case M_HELP_HELP :
        dlg_help_box_set_values(win_get_element(winWindow, BOX_HELP_HELP), CTX_BOX_HELP);
        cnt_stack_put((CNT_STACK*)win_get_add_info(winWindow), CTX_BOX_HELP);
        break;
      case M_HELP_BACK :
        dwContext = cnt_stack_get((CNT_STACK*)win_get_add_info(winWindow));
        if (!cnt_stack_is_empty((CNT_STACK*)win_get_add_info(winWindow)))
        {
          dwContext = cnt_stack_get((CNT_STACK*)win_get_add_info(winWindow));
          dlg_help_box_set_values(win_get_element(winWindow, BOX_HELP_HELP), dwContext);
          cnt_stack_put((CNT_STACK*)win_get_add_info(winWindow), dwContext);
        }
        else
        {
          cnt_stack_put((CNT_STACK*)win_get_add_info(winWindow), dwContext);
          utl_beep();
        }
        break;
      case M_CHANGE_CONTEXT :
        cnt_stack_put((CNT_STACK*)win_get_add_info(winWindow), peventEvent->ulAddInfo);
        dlg_help_box_set_values(win_get_element(winWindow, BOX_HELP_HELP), peventEvent->ulAddInfo);
        peventEvent->uiKind = E_DONE;
        break;
    }
  }
}     

static void int_box_edit_window (WINDOW win, UTL_EVENT *peventEvent)
{
  if (peventEvent->uiKind == E_MESSAGE)
  {
    switch (peventEvent->uiMessage)
    {
      case M_DRAW :
      case M_REDRAW :
        dlg_editor_set_display(win_get_element(win, BOX_EDIT_EDIT), 0, 0, win->iWidth, win->iHeight);
        win_draw_elements(win);
      case M_INIT :
        peventEvent->uiKind = E_DONE;
        break;
      case M_SHOW :
      case M_ACTIVATE :
        win_set_focus(win);
        peventEvent->uiKind = E_DONE;
        break;
      case M_GET_FOCUS :
        win_show(win);
        /* sts_new_status_line(aitemHelpItems); */
        peventEvent->uiKind = E_DONE;
        break;
      case M_LOST_FOCUS :
        /* sts_del_status_line(); */
        peventEvent->uiKind = E_DONE;
        break;
      case M_QUIT :
        peventEvent->uiKind = E_DONE;
        break;
      case M_CLOSE :
        win_delete(win);
        peventEvent->uiKind = E_DONE;
        break;
    }
  }
}     

static void int_box_files_read (CHAR *pcMask, BOX_FILE_WIN *pfwin)
{
  INT i;

  if (pfwin->ppcFirstLine)
    utl_free(pfwin->ppcFirstLine);
  utl_get_files(pcMask, pfwin->pcBuffer, pfwin->pdeTable, 8192, 512, TRUE);
  for (i = 0; pfwin->pdeTable[i].pcName; i++)
    ;
  pfwin->ppcFirstLine = utl_alloc((i + 1) * sizeof(CHAR*));
  for (i = 0; pfwin->pdeTable[i].pcName; i++)
    pfwin->ppcFirstLine[i] = pfwin->pdeTable[i].pcName;
  pfwin->ppcFirstLine[i] = NULL;
}

static void int_box_write_path (WINDOW win, CHAR *pcPath)
{
  CHAR acTemp[28];
  
  win_sw_z(win, 14, 4, 27, 1, K_SPACE);
  if (strlen(pcPath) > 27)
  {
    strncpy(acTemp, pcPath, 3);
    strcpy(acTemp + 3, "...");
    strncpy(acTemp + 6, pcPath + strlen(pcPath) - 21, 21);
    acTemp[27] = 0;
  }
  else
    strcpy(acTemp, pcPath);
  win_ss(win, 14, 4, acTemp);
}

static void int_box_file_window (WINDOW winWindow, UTL_EVENT *peventEvent)
{
         BOX_FILE_WIN *pfwin;
         CHAR          acTemp[128],
                       acDrive[3],
                       acDir[128],
                       acName[64],
                       acExt[32];
         INT           iPos;
  static UINT          suiLastButton;

  pfwin = (BOX_FILE_WIN*)win_get_add_info(winWindow);
  switch (peventEvent->uiKind)
  {
    case E_MESSAGE :
      switch (peventEvent->uiMessage)
      {
        case M_REDRAW :
          win_cls(winWindow);
          win_draw_elements(winWindow);
          int_box_write_path(winWindow, pfwin->pcPath);
          peventEvent->uiKind = E_DONE;
          break;
        case M_QUERY_VALUES :
          *((UINT*)peventEvent->ulAddInfo) = suiLastButton;
          peventEvent->uiKind = E_DONE;
          break;
        case M_ACT_BUT_CLICKED :
          suiLastButton = ((DLG_ELEMENT*)peventEvent->pFrom)->uiID;
          switch (((DLG_ELEMENT*)peventEvent->pFrom)->uiID)
          {
            case BOX_DRIVES :
              box_drives();
              dlg_text_field_set_values(win_get_element(winWindow, BOX_FILE_NAME), pfwin->pcMask);
              utl_get_path(pfwin->pcPath);
              int_box_write_path(winWindow, pfwin->pcPath);
            case BOX_OK :
              if (utl_filename_valid(pfwin->pcFileName))
              {
                utl_split_path(pfwin->pcFileName, acDrive, acDir, acName, acExt);
                if (isalpha(acDrive[0]))
                  utl_set_drive(utl_upper(acDrive[0]) - 'A');
                if (acDir[0])
                {
                  acDir[strlen(acDir) - 1] = 0;
                  chdir(acDir);
                }
                strcpy(acTemp, acName);
                strcat(acTemp, acExt);
                if (!acTemp[0])
                  strcpy(acTemp, pfwin->pcMask);
                dlg_text_field_set_values(win_get_element(winWindow, BOX_FILE_NAME), acTemp);
                if (strchr(acTemp, '*') || strchr(acTemp, '?'))
                {
                  int_box_files_read(acTemp, pfwin);
                  dlg_list_box_new_list(win_get_element(winWindow, BOX_FILES), pfwin->ppcFirstLine);
                  utl_get_path(pfwin->pcPath);
                  int_box_write_path(winWindow, pfwin->pcPath);
                }
                else
                {
                  strcpy(pfwin->pcMask, pfwin->pcFileName);
                  bExit = TRUE;
                }
              }
              else
                box_info(BOX_INFO, BOX_OK, pcFileInvalid, 0);
              peventEvent->uiKind = E_DONE;
              break;
            case BOX_HELP :
              box_modal_help(CTX_BOX_LADEN_SPEICHERN);
              peventEvent->uiKind = E_DONE;
              break;
            case BOX_CANCEL :
              bExit = TRUE;
              peventEvent->uiKind = E_DONE;
              break;
          }
          break;
        case M_LIST_BOX_CLICKED :
          dlg_list_box_query_values(win_get_element(winWindow, BOX_FILES), &iPos);
          if (pfwin->ppcFirstLine[iPos] == NULL)
            break;
          if (!strcmp("", pfwin->ppcFirstLine[iPos]))
            break;
          if (pfwin->pdeTable[iPos].bSubDir)
          {
            if (utl_filename_valid(pfwin->pcFileName))
            {
              chdir(pfwin->ppcFirstLine[iPos]);
              utl_split_path(pfwin->pcFileName, acDrive, acDir, acName, acExt);
              strcpy(acTemp, acName);
              strcat(acTemp, acExt);
              int_box_files_read(acTemp, pfwin);
              dlg_list_box_new_list(win_get_element(winWindow, BOX_FILES), pfwin->ppcFirstLine);
              utl_get_path(pfwin->pcPath);
              int_box_write_path(winWindow, pfwin->pcPath);
            }
            else
              box_info(BOX_INFO, BOX_OK, pcFileInvalid, 0);
          }
          else
          {
            strcpy(pfwin->pcMask, pfwin->ppcFirstLine[iPos]);
            suiLastButton = BOX_OK;
            bExit = TRUE;
          }
          peventEvent->uiKind = E_DONE;
          break;
      }
      break;
  }
  if (peventEvent->uiKind != E_DONE)
    win_std_handler(winWindow, peventEvent);
}


/***************************************************************************
 *                       Allgemeine Funktionen                             *
 ***************************************************************************/

void box_init (void)
{
  win_register_class("info_info", PAL_WIN_INFO, B_EEEE, TITLE_T_C,
                     WIN_FL_SHADOW | WIN_FL_CURSOR | WIN_FL_MOVEABLE | WIN_FL_MODAL | WIN_FL_FOCUSABLE,
                     win_std_handler);
  win_register_class("info_warning", PAL_WIN_WARNING, B_EEEE, TITLE_T_C,
                     WIN_FL_SHADOW | WIN_FL_CURSOR | WIN_FL_MOVEABLE | WIN_FL_MODAL | WIN_FL_FOCUSABLE,
                     win_std_handler);
  win_register_class("info_error", PAL_WIN_ERROR, B_EEEE, TITLE_T_C,
                     WIN_FL_SHADOW | WIN_FL_CURSOR | WIN_FL_MOVEABLE | WIN_FL_MODAL | WIN_FL_FOCUSABLE,
                     win_std_handler);
  win_register_class("load_save_box", PAL_WIN_DLG, B_EEEE, TITLE_T_C,
                     WIN_FL_SHADOW | WIN_FL_CURSOR | WIN_FL_MOVEABLE | WIN_FL_MODAL | WIN_FL_FOCUSABLE,
                     int_box_file_window);
  win_register_class("color_box", PAL_WIN_DLG, B_EEEE, TITLE_T_C,
                     WIN_FL_SHADOW | WIN_FL_CURSOR | WIN_FL_MOVEABLE | WIN_FL_MODAL | WIN_FL_FOCUSABLE,
                     int_box_color_window);
  win_register_class("modal_help_box", PAL_WIN_DLG, B_EEEE, TITLE_T_C,
                     WIN_FL_SHADOW | WIN_FL_CURSOR | WIN_FL_MOVEABLE | WIN_FL_MODAL | WIN_FL_FOCUSABLE,
                     int_box_modal_help_window);
  win_register_class("help_box", PAL_WIN_HELP, B_EEEE, TITLE_T_C,
                     WIN_FL_SHADOW | WIN_FL_CURSOR | WIN_FL_MOVEABLE | WIN_FL_SIZEABLE | WIN_FL_CLOSEABLE |
                     WIN_FL_FOCUSABLE, int_box_help_window);
  win_register_class("edit_box", PAL_WIN_DLG, B_EEEE, TITLE_T_C,
                     WIN_FL_SHADOW | WIN_FL_CURSOR | WIN_FL_MOVEABLE | WIN_FL_SIZEABLE | WIN_FL_CLOSEABLE |
                     WIN_FL_FOCUSABLE, int_box_edit_window);
}

UINT box_info (UCHAR ucType, UINT uiFlags, CHAR *pcText, DWORD dwContext)
{
  INT     iLines        = 0,
          iChars        = 0,
          iLeast        = 0,
          iCounter;
  BOOL    bEnd          = FALSE,
          bShorts;
  UINT    uiID,
          uiReturnVar;
  CHAR   *pcActChar,
         *pcType;
  WINDOW  winWindow;

  if (pcText)
  {
    bShorts = utl_short_cuts(FALSE);
    switch (ucType)
    {
      case BOX_INFO :
        pcType = "info_info";
        break;
      case BOX_WARNING :
        pcType = "info_warning";
        break;
      case BOX_ERROR :
        pcType = "info_error";
        break;
    }
    pcActChar = pcText;
    for (; *(pcActChar - 1) == '\n' || iLines == 0; pcActChar++)
    {
      for (iCounter = 0; *pcActChar != '\n' && *pcActChar; pcActChar++)
      {
        iCounter++;
        if (iChars < iCounter)
          iChars = iCounter;
      }
      iLines++;
    }
    if (uiFlags & BOX_OK)
      iLeast = 6;
    if (uiFlags & BOX_CANCEL)
    {
      if (iLeast != 0)
        iLeast += 13;
      else
        iLeast = 11;
    }
    if (uiFlags & BOX_HELP)
    {
      if (iLeast != 0)
        iLeast += 11;
      else
        iLeast = 9;
    }
    if (iLeast > iChars)
      iChars = iLeast;
    if (!(winWindow = win_new(37 - iChars / 2, 9 - iLines / 2, iChars + 6, iLines + 6, pcType, 0)))
      return 0;
    iCounter = 0;
    if (uiFlags & BOX_OK)
    {
      win_add_element(winWindow, dlg_init_act_button(3, iLines + 4, "OK", K_ENTER, pcInfoOK, BOX_OK, TRUE,
                                                     NULL));
      iCounter++;
    }
    if (uiFlags & BOX_CANCEL)
      win_add_element(winWindow, dlg_init_act_button(3 + (iCounter * 8), iLines + 4, "Abbruch", K_ESC,
                                                     pcInfoCancel, BOX_CANCEL, TRUE, NULL));
    if (uiFlags & BOX_HELP)
      win_add_element(winWindow, dlg_init_act_button(iChars - 6, iLines + 4, "Hilfe", K_F1, pcHelp, BOX_HELP,
                                                     TRUE, NULL));
    win_add_element(winWindow, dlg_init_border(1, iLines + 3, iChars + 4, 1, 0));
    pcActChar = pcText;
    for (iLines = 0; *(pcActChar - 1) == '\n' || iLines == 0; pcActChar++, iLines++)
      for (iCounter = 0; *pcActChar != '\n' && *pcActChar; pcActChar++, iCounter++)
        win_s_z(winWindow, 3 + iCounter, 2 + iLines, *pcActChar);
    win_cursor(winWindow, TRUE);
    do
    {
      glb_execute(winWindow);
      win_get_last_button(winWindow, &uiID);
      switch (uiID)
      {
        case BOX_OK :
          bEnd = TRUE;
          uiReturnVar = BOX_OK;
          break;
        case BOX_CANCEL :
          bEnd = TRUE;
          uiReturnVar = BOX_CANCEL;
          break;
        case BOX_HELP :
          box_modal_help(dwContext);
          break;
      }
    } while (!bEnd);
    if (!win_delete(winWindow))
      return 0;
    utl_short_cuts(bShorts);
    return uiReturnVar;
  }
  return 0;
}

BOOL box_mouse (void)
{
#ifdef _WINNT
  box_info(BOX_INFO, BOX_OK, pcNAWinNT, 0);
  return TRUE;
#else
  INT          iPos;
  BOOL         bEnd            = FALSE,
               bShorts;
  UINT         uiSpeed,
               uiID;
  WINDOW       winWindow;
  DLG_ELEMENT *pelement;

  bShorts = utl_short_cuts(FALSE);
  if (!(winWindow = win_new(22, 8, 36, 10, "standard_dialogue", 0)))
    return FALSE;
  if (!win_title(winWindow, " Mauseinstellungen ", TITLE_T_C))
    return FALSE;
  uiSpeed = uiMouseSpeed;
  for (iPos = 10; uiSpeed != 0; uiSpeed >>= 1)
    iPos--;
  pelement = dlg_init_scroll(16, 3, 10, iPos, DLG_HORIZONTAL, pcMouseSpeed, BOX_MOUSE_SPEED, TRUE, NULL);
  win_add_element(winWindow, dlg_init_label(3, 3, "#Mausgeschw.", pelement, 0));
  win_add_element(winWindow, pelement);
  pelement = dlg_init_scroll(16, 5, 10, (INT)(10 - ulDoubleClick / 50), DLG_HORIZONTAL, pcMouseClick,
                             BOX_MOUSE_CLICK, TRUE, NULL);
  win_add_element(winWindow, dlg_init_label(3, 5, "#Doppelklick", pelement, 0));
  win_add_element(winWindow, pelement);
  win_add_element(winWindow, dlg_init_act_button(3, 8, "OK", K_ENTER, pcMouseOK, BOX_OK, TRUE, NULL));
  win_add_element(winWindow, dlg_init_act_button(11, 8, "Abbruch", K_ESC, pcMouseCancel, BOX_CANCEL, TRUE,
                                                 NULL));
  win_add_element(winWindow, dlg_init_act_button(24, 8, "Hilfe", K_F1, pcHelp, BOX_HELP, TRUE, NULL));
  win_add_element(winWindow, dlg_init_label(13, 2, "langsam", 0, NULL));
  win_add_element(winWindow, dlg_init_label(24, 2, "schnell", 0, NULL));
  win_add_element(winWindow, dlg_init_border(1, 7, 34, 1, 0));
  win_cursor(winWindow, TRUE);
  do
  {
    glb_execute(winWindow);
    win_get_last_button(winWindow, &uiID);
    switch (uiID)
    {
      case BOX_OK :
        dlg_scroll_query_values(win_get_element(winWindow, BOX_MOUSE_SPEED), &iPos);
        uiMouseSpeed = (1 << (9 - iPos));
        dlg_scroll_query_values(win_get_element(winWindow, BOX_MOUSE_CLICK), &iPos);
        ulDoubleClick = (ULONG)((10 - iPos) * 50);
        msm_set_mickeys(uiMouseSpeed, uiMouseSpeed * 2);
      case BOX_CANCEL :
        bEnd = TRUE;
        break;
      case BOX_HELP :
        box_modal_help(CTX_BOX_MOUSE);
        break;
    }
  } while (!bEnd);
  if (!win_delete(winWindow))
    return FALSE;
  utl_short_cuts(bShorts);
  return TRUE;
#endif
}

BOOL box_palette (void)
{
  BOOL         bEnd           = FALSE,
               bShorts;
  INT          iPalette;
  UINT         uiID;
  WINDOW       winWindow;
  DLG_BUTTON   abutButtons[]  = {
                                  {
                                    3,
                                    2,
                                    0,
                                    "#blaue Palette"
                                  },
                                  {
                                    3,
                                    3,
                                    0,
                                    "#wei""\xe1""e Palette"
                                  },
                                  {
                                    3,
                                    4,
                                    0,
                                    "#kobalt Palette"
                                  },
                                  {
                                    3,
                                    5,
                                    0,
                                    "#schwarze Palette"
                                  },
                                  {
                                    3,
                                    6,
                                    0,
                                    "#gr\x81ne Palette"
                                  },
                                  {
                                    3,
                                    7,
                                    0,
                                    "#monochrom Palette"
                                  },
                                  {
                                    3,
                                    8,
                                    0,
                                    "#eigene Palette"
                                  },
                                  DLG_RADIO_END
                                };

  bShorts = utl_short_cuts(FALSE);
  if (!(winWindow = win_new(22, 6, 36, 13, "standard_dialogue", 0)))
    return FALSE;
  if (!win_title(winWindow, " Farbpalette ", TITLE_T_C))
    return FALSE;
  win_add_element(winWindow, dlg_init_radio_button(abutButtons, ucActPalette, pcPalette, BOX_PALETTE, TRUE,
                                                   NULL));
  win_add_element(winWindow, dlg_init_act_button(3, 11, "OK", K_ENTER, pcPaletteOK, BOX_OK, TRUE, NULL));
  win_add_element(winWindow, dlg_init_act_button(11, 11, "Abbruch", K_ESC, pcPaletteCancel, BOX_CANCEL, TRUE,
                                                 NULL));
  win_add_element(winWindow, dlg_init_act_button(24, 11, "Hilfe", K_F1, pcHelp, BOX_HELP, TRUE, NULL));
  win_add_element(winWindow, dlg_init_border(1, 10, 34, 1, 0));
  win_cursor(winWindow, TRUE);
  do
  {
    glb_execute(winWindow);
    win_get_last_button(winWindow, &uiID);
    switch (uiID)
    {
      case BOX_OK :
        dlg_radio_button_query_values(win_get_element(winWindow, BOX_PALETTE), &iPalette);
      case BOX_CANCEL :
        bEnd = TRUE;
        break;
      case BOX_HELP :
        box_modal_help(CTX_BOX_FARBPALETTE);
        break;
    }
  } while (!bEnd);
  if (!win_delete(winWindow))
    return FALSE;
  glb_set_palette((UCHAR)iPalette);
  utl_short_cuts(bShorts);
  return TRUE;
}

BOOL box_drives (void)
{
  BOOL         bShorts,
               bReturnVar = FALSE,
               bEnd       = FALSE;
  INT          iCounter,
               iPos;
  UCHAR        ucDrive;
  UINT         uiID;
  CHAR        *apcFirstLine[27],
              *pcDrives;
  WINDOW       winWindow;
  DLG_ELEMENT *pelement;

  bShorts = utl_short_cuts(FALSE);
  if (!(winWindow = win_new(22, 6, 36, 13, "standard_dialogue", 0)))
    return FALSE;
  if (!win_title(winWindow, " Laufwerkswahl ", TITLE_T_C))
    return FALSE;
  if (!(pcDrives = utl_get_drives()))
    return FALSE;
  for (iCounter = 0; iCounter < 26 && pcDrives[3 * iCounter]; iCounter++)
    apcFirstLine[iCounter] = (pcDrives + (3 * iCounter));
  apcFirstLine[iCounter] = NULL;
  ucDrive = utl_get_drive();
  for (iCounter = 0; apcFirstLine[iCounter]; iCounter++)
    if (*apcFirstLine[iCounter] - 'A' == ucDrive)
    {
      ucDrive = (UCHAR)iCounter;
      break;
    }
  pelement = dlg_init_list_box(14, 2, 6, 7, apcFirstLine, TRUE, pcDrive, BOX_DRIVE, TRUE, NULL);
  win_add_element(winWindow, dlg_init_label(3, 2, "#Laufwerke", pelement, 0));
  win_add_element(winWindow, pelement);
  win_add_element(winWindow, dlg_init_act_button(3, 11, "OK", K_ENTER, pcDriveOK, BOX_OK, TRUE, NULL));
  win_add_element(winWindow, dlg_init_act_button(11, 11, "Abbruch", K_ESC, pcDriveCancel, BOX_CANCEL, TRUE,
                                                 NULL));
  win_add_element(winWindow, dlg_init_act_button(24, 11, "Hilfe", K_F1, pcHelp, BOX_HELP, TRUE, NULL));
  win_add_element(winWindow, dlg_init_border(1, 10, 34, 1, 0));
  win_cursor(winWindow, TRUE);
  do
  {
    glb_execute(winWindow);
    win_get_last_button(winWindow, &uiID);
    switch (uiID)
    {
      case BOX_DRIVE :
      case BOX_OK :
        dlg_list_box_query_values(win_get_element(winWindow, BOX_DRIVE), &iPos);
        utl_set_drive(*apcFirstLine[iPos] - 'A');
        bReturnVar = TRUE;
      case BOX_CANCEL :
        bEnd = TRUE;
        break;
      case BOX_HELP :
        box_modal_help(CTX_BOX_LAUFWERKE);
        break;
    }
  } while (!bEnd);
  if (!win_delete(winWindow))
    return FALSE;
  utl_free(pcDrives);
  utl_short_cuts(bShorts);
  return bReturnVar;
}

BOOL box_load_save (CHAR *pcTitle, CHAR *pcMask)
{

  BOOL                 bReturnVar       = TRUE,
                       bShorts;
  UINT                 uiID;
  WINDOW               winWindow;
  BOX_FILE_WIN         fwin;
  DLG_ELEMENT         *pelement;

  bShorts = utl_short_cuts(FALSE);
  if (!(fwin.pcFileName = utl_alloc(251)))
    return FALSE;
  if (!(fwin.pcPath = utl_alloc(251)))
    return FALSE;
  if (!(fwin.pcBuffer = utl_alloc(8192)))
    return FALSE;
  if (!(fwin.pdeTable = utl_alloc(512 * sizeof(UTL_DIRECTORY_ENTRY))))
    return FALSE;
  fwin.pcMask = pcMask;
  strcpy(fwin.pcFileName, pcMask);
  fwin.ppcFirstLine = NULL;
  int_box_files_read(pcMask, &fwin);
  if (!(winWindow = win_new(14, 3, 51, 18, "load_save_box", (ULONG)&fwin)))
    return FALSE;
  if (!win_title(winWindow, pcTitle, TITLE_T_C))
    return FALSE;
  pelement = dlg_init_text_field(13, 2, 26, 250, FALSE, fwin.pcFileName, pcFileName, BOX_FILE_NAME, TRUE, NULL);
  win_add_element(winWindow, dlg_init_label(3, 2, "#Dateiname", pelement, 0));
  win_add_element(winWindow, pelement);
  pelement = dlg_init_list_box(9, 5, 32, 10, fwin.ppcFirstLine, TRUE, pcFileFiles, BOX_FILES, TRUE, NULL);
  win_add_element(winWindow, dlg_init_label(3, 4, "Dateien #in", pelement, 0));
  win_add_element(winWindow, pelement);
  win_add_element(winWindow, dlg_init_act_button(3, 16, "OK", K_ENTER, pcFileOK, BOX_OK, TRUE, NULL));
  win_add_element(winWindow, dlg_init_act_button(11, 16, "Abbruch", K_ESC, pcFileCancel, BOX_CANCEL, TRUE,
                                                 NULL));
  win_add_element(winWindow, dlg_init_act_button(24, 16, "#Laufwerke", 0, pcFileDrives, BOX_DRIVES, TRUE,
                                                 NULL));
  win_add_element(winWindow, dlg_init_act_button(39, 16, "Hilfe", K_F1, pcHelp, BOX_HELP, TRUE, NULL));
  win_add_element(winWindow, dlg_init_border(1, 15, 49, 1, 0));
  win_cursor(winWindow, TRUE);
  utl_get_path(fwin.pcPath);
  glb_send_message(NULL, winWindow, M_REDRAW, 0);
  glb_execute(winWindow);
  win_get_last_button(winWindow, &uiID);
  if (uiID == BOX_OK)
    bReturnVar = TRUE;
  else
    bReturnVar = FALSE;
  if (!win_delete(winWindow))
    return FALSE;
  utl_free(fwin.pcFileName);
  utl_free(fwin.ppcFirstLine);
  utl_free(fwin.pcPath);
  utl_free(fwin.pcBuffer);
  utl_free(fwin.pdeTable);
  utl_short_cuts(bShorts);
  return bReturnVar;
}

BOOL box_beep (BOOL bLoadOnCall)
{
  WINDOW       winWindow;
  BOOL         bReturnVar          = FALSE,
               bShorts,
               bDummy,
               bEnd                = FALSE;
  INT          iFrequency,
               iDuration;
  CHAR         acDigiName[128],
               acDir[128],
               acBuffer[128],
               acTemp[128];
  UINT         uiID;
  void        *pVOC;
  DLG_ELEMENT *pelement;
#if defined(_OS2) || defined(_WINNT) || defined(_UNIX)
  BOOL         bVOCBeep            = FALSE;
#endif

  bShorts = utl_short_cuts(FALSE);
  if (!(winWindow = win_new(11, 4, 58, 16, "standard_dialogue", 0)))
    return FALSE;
  if (!win_title(winWindow, " Warnton ", TITLE_T_C))
    return FALSE;

#if defined(_OS2) || defined(_WINNT) || defined(_UNIX)
  strcpy(acDigiName, "not yet available");
#else
  strcpy(acDigiName, acBeepVOC);
#endif
  win_add_element(winWindow, dlg_init_push_button(3, 2, "#Warnton ein", bBeep, pcSound, BOX_SOUND, TRUE, NULL));
  win_add_element(winWindow, dlg_init_push_button(3, 3, "#Digi-Warnton ein", bVOCBeep, pcSoundDigi,
                                                  BOX_SOUND_DIGI, TRUE, NULL));
  iDuration = ulBeepDuration / 55 - 1;
  pelement = dlg_init_scroll(12, 6, 20, iDuration, DLG_HORIZONTAL, pcSoundLength, BOX_SOUND_LENGTH, TRUE, NULL);
  win_add_element(winWindow, dlg_init_label(3, 6, "#L\x84nge", pelement, 0));
  win_add_element(winWindow, pelement);
  iFrequency = ulBeepFrequency / 50 - 1;
  pelement = dlg_init_scroll(12, 9, 20, iFrequency, DLG_HORIZONTAL, pcSoundFreq, BOX_SOUND_FREQ, TRUE, NULL);
  win_add_element(winWindow, dlg_init_label(3, 9, "#Frequenz", pelement, 0));
  win_add_element(winWindow, pelement);
  pelement = dlg_init_text_field(13, 11, 40, 127, FALSE, acDigiName, pcSoundDigiName, BOX_DIGI_NAME, TRUE,
                                 NULL);
  win_add_element(winWindow, dlg_init_label(3, 11, "#VOC-Datei", pelement, 0));
  win_add_element(winWindow, pelement);
  win_add_element(winWindow, dlg_init_act_button(3, 14, "OK", K_ENTER, pcSoundOK, BOX_OK, TRUE, NULL));
  win_add_element(winWindow, dlg_init_act_button(11, 14, "Abbruch", K_ESC, pcSoundCancel, BOX_CANCEL, TRUE,
                                                 NULL));
  win_add_element(winWindow, dlg_init_act_button(24, 14, "#Suchen", 0, pcSoundSearchDigi, BOX_SEARCH_DIGI,
                                                 TRUE, NULL));
  win_add_element(winWindow, dlg_init_act_button(36, 14, "#Test", 0, pcSoundTest, BOX_SOUND_TEST, TRUE, NULL));
  win_add_element(winWindow, dlg_init_act_button(46, 14, "Hilfe", K_F1, pcHelp, BOX_HELP, TRUE, NULL));
  win_add_element(winWindow, dlg_init_label(10, 5, "55 ms", 0, NULL));
  win_add_element(winWindow, dlg_init_label(30, 5, "1100 ms", 0, NULL));
  win_add_element(winWindow, dlg_init_label(10, 8, "50 Hz", 0, NULL));
  win_add_element(winWindow, dlg_init_label(30, 8, "1000 Hz", 0, NULL));
  win_add_element(winWindow, dlg_init_border(1, 13, 56, 1, 0));
  win_cursor(winWindow, TRUE);
  do
  {
    glb_execute(winWindow);
    win_get_last_button(winWindow, &uiID);
    switch (uiID)
    {
      case BOX_OK :
        dlg_push_button_query_values(win_get_element(winWindow, BOX_SOUND), &bBeep);
        dlg_push_button_query_values(win_get_element(winWindow, BOX_SOUND_DIGI), &bVOCBeep);
        dlg_scroll_query_values(win_get_element(winWindow, BOX_SOUND_LENGTH), &iDuration);
        dlg_scroll_query_values(win_get_element(winWindow, BOX_SOUND_FREQ), &iFrequency);
        ulBeepDuration = (iDuration + 1) * 55;
        ulBeepFrequency = (iFrequency + 1) * 50;
        utl_set_voc_beep(acDigiName, bLoadOnCall);
        bReturnVar = TRUE;
      case BOX_CANCEL :
        bEnd = TRUE;
        break;
#ifdef _MSDOS
      case BOX_SEARCH_DIGI :
        strcpy(acBuffer, "*.VOC");
        utl_get_path(acDir);
        if (box_load_save(" Digi-Sound suchen ", acBuffer))
        {
          utl_get_path(acTemp);
          if (acTemp[strlen(acTemp) - 1] != '\\')
            strcat(acTemp, "\\");
          strcat(acTemp, acBuffer);
          dlg_text_field_set_values(win_get_element(winWindow, BOX_DIGI_NAME), acTemp);
        }
        utl_set_path(acDir);
        break;
#endif
      case BOX_SOUND_TEST :
        dlg_push_button_query_values(win_get_element(winWindow, BOX_SOUND), &bDummy);
        if (bDummy)
        {
#ifdef _MSDOS
          dlg_push_button_query_values(win_get_element(winWindow, BOX_SOUND_DIGI), &bDummy);
          if (bDummy && bSoundBlaster)
          {
            pVOC = sbl_load_voc(acDigiName);
            if (pVOC)
            {
              sbl_start_voc(pVOC);
              while (wStatusWord)
                ;
              utl_free(pVOC);
            }
            else
              box_info(BOX_WARNING, BOX_INFO,
                       "VOC-Datei konnte nicht\ngeladen werden", 0);
          }
          else
#endif
          {
            dlg_scroll_query_values(win_get_element(winWindow, BOX_SOUND_LENGTH), &iDuration);
            dlg_scroll_query_values(win_get_element(winWindow, BOX_SOUND_FREQ), &iFrequency);
            utl_tone((iFrequency + 1) * 50, (iDuration + 1) * 55);
          }
        }
        break;
      case BOX_HELP :
        box_modal_help(CTX_BOX_WARNTON);
        break;
    }
  } while (!bEnd);
  if (!win_delete(winWindow))
    return FALSE;
  utl_short_cuts(bShorts);
  return bReturnVar;
}

void static int_box_color_window (WINDOW winWindow, UTL_EVENT *peventEvent)
{
         CHAR   cChar,
                cBack,
                cColor;
         INT    iWindow,
                iColor;
         CHAR  *pcPalette;
  static UINT   suiLastButton;

  pcPalette = (CHAR*)win_get_add_info(winWindow);
  switch (peventEvent->uiKind)
  {                 
    case E_MESSAGE :
      switch (peventEvent->uiMessage)
      {                  
        case M_REDRAW :
          win_cls(winWindow);
          win_draw_elements(winWindow);
          break;
        case M_COLOR_FIELD_CHANGED :
          dlg_color_field_query_values(win_get_element(winWindow, BOX_COLOR_CHAR), &cChar);
          dlg_color_field_query_values(win_get_element(winWindow, BOX_COLOR_BACK), &cBack);
          dlg_list_box_query_values(win_get_element(winWindow, BOX_COLOR_WINDOW), &iWindow);
          dlg_list_box_query_values(win_get_element(winWindow, BOX_COLOR), &iColor);
          pcPalette[iWindow * PAL_COLORS + iColor] = cChar + (cBack << 4);
          peventEvent->uiKind = E_DONE;
          break;
        case M_LIST_BOX_CHANGED :
          switch (((DLG_ELEMENT*)peventEvent->pFrom)->uiID)
          {
            case BOX_COLOR :
              dlg_list_box_query_values(win_get_element(winWindow, BOX_COLOR_WINDOW), &iWindow);
              iColor = peventEvent->ulAddInfo;
              cColor = pcPalette[iWindow * PAL_COLORS + iColor];
              cChar = cColor & 0x0f;
              cBack = cColor >> 4;
              dlg_color_field_set_values(win_get_element(winWindow, BOX_COLOR_CHAR), cChar);
              dlg_color_field_set_values(win_get_element(winWindow, BOX_COLOR_BACK), cBack);
              peventEvent->uiKind = E_DONE;
              break;
            case BOX_COLOR_WINDOW :
              dlg_list_box_new_list(win_get_element(winWindow, BOX_COLOR),
                                    aapcPalColors[peventEvent->ulAddInfo]);
              iWindow = peventEvent->ulAddInfo;
              iColor = 0;
              cColor = pcPalette[iWindow * PAL_COLORS + iColor];
              cChar = cColor & 0x0f;
              cBack = cColor >> 4;
              dlg_color_field_set_values(win_get_element(winWindow, BOX_COLOR_CHAR), cChar);
              dlg_color_field_set_values(win_get_element(winWindow, BOX_COLOR_BACK), cBack);
              peventEvent->uiKind = E_DONE;
              break;
          }
          break;    
        case M_ACT_BUT_CLICKED :
          suiLastButton = ((DLG_ELEMENT*)peventEvent->pFrom)->uiID;
          switch (suiLastButton)
          {
            case BOX_OK :
            case BOX_CANCEL :
              bExit = TRUE;
              break;
            case BOX_HELP :
              box_modal_help(CTX_BOX_CUSTOM_FARBEN);
              break;
          }
          peventEvent->uiKind = E_DONE;
          break;
        case M_QUERY_VALUES :
          *((UINT*)peventEvent->ulAddInfo) = suiLastButton;
          peventEvent->uiKind = E_DONE;
          break;
      }
      break;
  }
  if (peventEvent->uiKind != E_DONE)
    win_std_handler(winWindow, peventEvent);
}                   

BOOL box_custom_palette (void)
{
  DLG_ELEMENT *pelement;
  BOOL         bShorts,
               bEnd       = FALSE,
               bReturnVar = FALSE;
  UINT         uiID;
  CHAR        *pcPalette;
  WINDOW       winWindow;

  bShorts = utl_short_cuts(FALSE);
  if (!(pcPalette = utl_alloc(PAL_COLORS * PAL_WINDOWS)))
    return FALSE;
  if (!(winWindow = win_new(18, 2, 43, 23, "color_box", (ULONG)pcPalette)))
    return FALSE;
  if (!win_title(winWindow, " Farbpalette \x84ndern ", TITLE_T_C))
    return FALSE;
  memcpy(pcPalette, prgProgram.pcPalette, PAL_COLORS * PAL_WINDOWS);
  pelement = dlg_init_list_box(5, 3, 19, 7, apcPalWindows, TRUE, pcColorWindow, BOX_COLOR_WINDOW, TRUE, NULL);
  win_add_element(winWindow, dlg_init_label(3, 2, "#Fenstertyp", pelement, 0));
  win_add_element(winWindow, pelement);
  pelement = dlg_init_list_box(5, 12, 35, 7, *aapcPalColors, TRUE, pcColor, BOX_COLOR, TRUE, NULL);
  win_add_element(winWindow, dlg_init_label(3, 11, "F#arbe", pelement, 0));
  win_add_element(winWindow, pelement);
  pelement = dlg_init_color_field(28, 3, TRUE, *pcPalette & 0x0f, pcColorChar, BOX_COLOR_CHAR, TRUE, NULL);
  win_add_element(winWindow, dlg_init_label(26, 2, "#Text", pelement, 0));
  win_add_element(winWindow, pelement);
  pelement = dlg_init_color_field(28, 9, FALSE, *pcPalette >> 4, pcColorBack, BOX_COLOR_BACK, TRUE, NULL);
  win_add_element(winWindow, dlg_init_label(26, 8, "#Hintergrund", pelement, 0));
  win_add_element(winWindow, pelement);
  win_add_element(winWindow, dlg_init_act_button(3, 21, "OK", K_ENTER, pcColorOK, BOX_OK, TRUE, NULL));
  win_add_element(winWindow, dlg_init_act_button(11, 21, "Abbruch", K_ESC, pcColorCancel, BOX_CANCEL, TRUE,
                                                 NULL));
  win_add_element(winWindow, dlg_init_act_button(31, 21, "Hilfe", K_F1, pcHelp, BOX_HELP, TRUE, NULL));
  win_add_element(winWindow, dlg_init_border(1, 20, 41, 1, 0));
  do
  {
    glb_execute(winWindow);
    win_get_last_button(winWindow, &uiID);
    switch (uiID)
    {
      case BOX_OK :
        memcpy(prgProgram.pcPalette, pcPalette, PAL_COLORS * PAL_WINDOWS);
        bReturnVar = TRUE;
      case BOX_CANCEL :
        bEnd = TRUE;
        break;
    }
  } while (!bEnd);
  if (!win_delete(winWindow))
    return FALSE;
  utl_free(pcPalette);
  if (bReturnVar)        
    glb_update_windows();
  utl_short_cuts(bShorts);
  return bReturnVar;
}                            

BOOL box_screen_saver (void)
{
  DLG_ELEMENT *pelement;
  BOOL         bShorts,
               bEnd        = FALSE,
               bReturnVar  = FALSE;
  INT          iScroll;
  UINT         uiID;
  WINDOW       winWindow;

  bShorts = utl_short_cuts(FALSE);
  if (!(winWindow = win_new(22, 7, 36, 10, "standard_dialogue", 0)))
    return FALSE;
  if (!win_title(winWindow, " Bildschirmschoner ", TITLE_T_C))
    return FALSE;
  win_add_element(winWindow, dlg_init_push_button(3, 2, "#Bildschirmschoner aktiv", bSaver, pcSaverActive,
                                                  BOX_SAVER_ACTIVE, TRUE, NULL));
  pelement = dlg_init_scroll(9, 5, 20, ulScreenTimeout / 30000 - 1, DLG_HORIZONTAL, pcSaverTime,
                             BOX_SAVER_TIME, TRUE, NULL);
  win_add_element(winWindow, dlg_init_label(3, 5, "#Zeit", pelement, 0));
  win_add_element(winWindow, pelement);
  win_add_element(winWindow, dlg_init_act_button(3, 8, "OK", K_ENTER, pcSaverOK, BOX_OK, TRUE, NULL));
  win_add_element(winWindow, dlg_init_act_button(11, 8, "Abbruch", K_ESC, pcSaverCancel, BOX_CANCEL, TRUE,
                                                 NULL));
  win_add_element(winWindow, dlg_init_act_button(24, 8, "Hilfe", K_F1, pcHelp, BOX_HELP, TRUE, NULL));
  win_add_element(winWindow, dlg_init_border(1, 7, 34, 1, 0));
  win_add_element(winWindow, dlg_init_label(5, 4, "« Minute", NULL, 0));
  win_add_element(winWindow, dlg_init_label(23, 4, "10 Minuten", NULL, 0));
  do
  {
    glb_execute(winWindow);
    win_get_last_button(winWindow, &uiID);
    switch (uiID)
    {
      case BOX_OK :
        dlg_push_button_query_values(win_get_element(winWindow, BOX_SAVER_ACTIVE), &bSaver);
        dlg_scroll_query_values(win_get_element(winWindow, BOX_SAVER_TIME), &iScroll);
        ulScreenTimeout = (ULONG)(iScroll + 1) * 30000;
        bReturnVar = TRUE;
      case BOX_CANCEL :
        bEnd = TRUE;
        break;
      case BOX_HELP :
        box_modal_help(CTX_BOX_SCREEN_SAVER);
        break;
    }
  } while (!bEnd);
  if (!win_delete(winWindow))
    return FALSE;
  utl_short_cuts(bShorts);
  return bReturnVar;
}

BOOL box_modal_help (DWORD dwContext)
{
  BOOL   bShorts;
  WINDOW winWindow;

  if (!hlp_context_available(dwContext))
  {
    box_info(BOX_INFO, BOX_OK, "Zu diesem Thema existiert keine Hilfe!", 0);
    return FALSE;
  }
  bShorts = utl_short_cuts(FALSE);
  if (!(winWindow = win_new(10, 2, 66, 21, "modal_help_box", 0)))
    return FALSE;
  if (!win_title(winWindow, " Hilfe ", TITLE_T_C))
    return FALSE;
  win_add_element(winWindow, dlg_init_help_box(3, 2, 60, 15, dwContext, TRUE, pcHelpHelp, BOX_HELP_HELP, TRUE,
                                               NULL));
  win_add_element(winWindow, dlg_init_act_button(3, 19, "Abbruch", K_ESC, pcHelpCancel, BOX_CANCEL, TRUE,
                                                 NULL));
  win_add_element(winWindow, dlg_init_act_button(16, 19, "Zur""\x81""ck", K_A_F1, pcHelpBack, BOX_HELP_BACK, TRUE,
                                                 NULL));
  win_add_element(winWindow, dlg_init_act_button(54, 19, "Hilfe", K_F1, pcHelp, BOX_HELP, TRUE, NULL));
  win_add_element(winWindow, dlg_init_border(1, 18, 64, 1, 0));
  psHelpBack = cnt_stack_make();
  cnt_stack_put(psHelpBack, dwContext);
  glb_execute(winWindow);
  cnt_stack_destroy(psHelpBack);
  if (!win_delete(winWindow))
    return FALSE;
  utl_short_cuts(bShorts);
  return TRUE;
}             

BOOL box_help (DWORD dwContext)
{
  WINDOW winWindow;

  if (!hlp_context_available(dwContext))
  {
    box_info(BOX_INFO, BOX_OK, "Zu diesem Thema existiert keine Hilfe!", 0);
    return(FALSE);
  }
  winWindow = win_find_class_window("help_box");
  if (winWindow)
  {
    glb_send_message(NULL, winWindow, M_CHANGE_CONTEXT, dwContext);
    win_show(winWindow);
    return TRUE;
  }
  if (!(winWindow = win_new(10, 2, 66, 21, "help_box", 0)))
    return FALSE;
  if (!win_title(winWindow, " Hilfe ", TITLE_T_C))
    return FALSE;
  win_set_size_limits(winWindow, 20, 5, iSizeX, iSizeY - 2);
  win_add_element(winWindow, dlg_init_help_box(0, 0, 66, 21, dwContext, FALSE, NULL, BOX_HELP_HELP, TRUE,
                                               NULL));
  win_set_add_info(winWindow, (ULONG)cnt_stack_make());
  cnt_stack_put((CNT_STACK*)win_get_add_info(winWindow), dwContext);
  win_show(winWindow);
  return TRUE;
}

BOOL box_edit (CHAR *pcTitle, CHAR *pcBuffer, ULONG ulBufferLength, BOOL bCanChange, ULONG ulAddInfo)
{
  WINDOW winWindow;

  if (!(winWindow = win_new(10, 2, 66, 21, "edit_box", ulAddInfo)))
    return FALSE;
  if (!win_title(winWindow, pcTitle, TITLE_T_C))
    return FALSE;
  win_set_size_limits(winWindow, 20, 5, iSizeX, iSizeY - 2);
  win_add_element(winWindow, dlg_init_editor(0, 0, 66, 21, ulBufferLength, pcBuffer, bCanChange, FALSE, NULL,
                                             BOX_EDIT_EDIT, TRUE, NULL));
  win_show(winWindow);
  return TRUE;
}
