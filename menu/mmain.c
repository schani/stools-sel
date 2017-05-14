/***************************************************************************
 ***************************************************************************
 ***                                                                     ***
 ***                             SE-Menu                                 ***
 ***                                                                     ***
 ***                            Hauptteil                                ***
 ***                                                                     ***
 ***                (c) 1990-92 by Schani Electronics                    ***
 ***                                                                     ***
 ***************************************************************************
 ***************************************************************************/

#define NO_STARTUP
#include <stools.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <fcntl.h>
#include <time.h>
#include <assert.h>
#include "menu.h"

#define WIN_CLASS_MENU  "semenu_menu"

extern FILE *pfileMemo;

UINT               uiUserID          = 0,
                   uiUsers;
UCHAR             *pucTempBatch      = "TEMP.BAT",
		  *pucUeber          = "      S E  -  M e n u  V2.8\n"
                                       "           Beta Release\n"
				       "\n"
				       "(c) 1991-93 by Schani Electronics\n"
				       "     written by Mark Probst\n"
				       "\n"
				       "   Schani Tools (c) 1990-93 by\n"
				       "       Schani Electronics",
		  *pucPark           = "Die K”pfe Ihrer Festplatte sind\n"
				       "jetzt geparkt. Sie k”nnen den\n"
				       "Computer nun ausschalten.",
		  *pucMaxEintraege   = "Die maximale Anzahl der\n"
				       "Eintr„ge ist schon erreicht!",
		  *pucGruppenName    = "Der Name der Gruppendatei\n"
				       "ist falsch!",
		  *pucWirklichGrpDel = "Wollen Sie die Programmgruppe\n"
				       "wirklich l”schen?",
		  *pucWirklichPrgDel = "Wollen Sie das Programm\n"
				       "wirklich l”schen?",
                  *pucDoNotMatch     = "Sie haben verschiedene\n"
                                       "Paáw”rter eingegeben!",
                  *pucWrongDate      = "Das Datum hat nicht das\n"
                                       "richtige Format: TT.MM.JJJJ",
                  *pucWrongTime      = "Die Zeit hat nicht das\n"
                                       "richtige Format: HH:MM",
                  *pucTerminDel      = "Wollen Sie den Termin\n"
                                       "wirklich l”schen?",
                  *pucNoTermin       = "Sie haben heute und\n"
                                       "morgen keine Termine!",
		  *pucLeer           = "",
		   ucUhr             = TRUE,
		   ucButtonBar       = TRUE,
                   ucTaeglich        = FALSE,
                   ucWoechentlich    = FALSE,
                   ucMonatlich       = FALSE,
                   ucPlayingStart    = FALSE,
                   ucRememberList    = REM_NONE,
                   ucTerminWarning   = TRUE;
_MNU_MENU         *pmenuHauptMenu,
		  *pmenuAktMenu;
MNU_USER           userAktUser;
MNU_TERMIN         trmNext;
UCHAR              ucIsTermin        = FALSE;
void              *pStartVOC         = NULL;
CHAR               acStartVOC[128]   = "",
                   acLogoutVOC[128]  = "";

void execute (UCHAR* pucDir, UCHAR* pucName)
{

  FILE  *pfileDatei;
  UCHAR *pucTemp;

  pfileDatei = fopen(pucTempBatch, "wt");
  pucTemp = pucDir;
  if (pucTemp[1] == ':')
  {
    fwrite(pucTemp, 2, 1, pfileDatei);
    fprintf(pfileDatei, "\n");
    pucTemp += 2;
  }
  if (*pucTemp)
    fprintf(pfileDatei, "cd %s\n", pucTemp);
  fprintf(pfileDatei, "%s\n", pucName);
  pucTemp = utl_alloc(131);
  utl_get_path(pucTemp);
  fwrite(pucTemp, 2, 1, pfileDatei);
  fprintf(pfileDatei, "\n");
  pucTemp += 2;
  fprintf(pfileDatei, "cd %s\nsemenu\n", pucTemp);
  fclose(pfileDatei);
  ende(10);
}

UINT create_menu (UCHAR *pucWinTitel)
{

  UCHAR      aucWinTitel[51];
  UINT       uiGruppe;
  _MNU_MENU *pmenuMenu;

  uiGruppe = get_next_free();
  pmenuMenu = utl_alloc(sizeof(_MNU_MENU));
  pmenuMenu->ucEintraege = 0;
  pmenuMenu->uiGruppe = uiGruppe;
  pmenuMenu->winWindow = win_new(20, 5, strlen(pucWinTitel) + 12, 5, WIN_CLASS_MENU, 0);
  strcpy(aucWinTitel, " ");
  strcat(aucWinTitel, pucWinTitel);
  strcat(aucWinTitel, " ");
  win_title(pmenuMenu->winWindow, aucWinTitel, TITLE_T_C);
  pmenuMenu->ucChanged = FALSE;
  write_menu(pmenuMenu);
  win_delete(pmenuMenu->winWindow);
  utl_free(pmenuMenu);
  return(uiGruppe);
}

void chain_menu (_MNU_MENU *pmenuOwnerMenu, UCHAR ucItem)
{

  _MNU_MENU *pmenuMenu;

  pmenuMenu = utl_alloc(sizeof(_MNU_MENU));
  read_menu(pmenuOwnerMenu->ameEintrag[ucItem].uiGruppe, pmenuOwnerMenu,
	    pmenuMenu);
  pmenuOwnerMenu->pmenuUnterMenu = pmenuMenu;
}

void make_window (_MNU_MENU *pmenuMenu, CHAR cX, CHAR cY,
		  UCHAR *pucTitel)
{

  UCHAR  ucBreite;

  ucBreite = get_longest_name(pmenuMenu, pucTitel) + 6;
  if (pmenuMenu->ucEintraege)
    pmenuMenu->winWindow = win_new(cX, cY, ucBreite,
					  pmenuMenu->ucEintraege + 4, WIN_CLASS_MENU, 0);
  else
    pmenuMenu->winWindow = win_new(cX, cY, ucBreite, 5, WIN_CLASS_MENU, 0);
  win_title(pmenuMenu->winWindow, pucTitel, TITLE_T_C);
}

void termin_warning (void)
{
    CHAR        acWarning[76] = "Sie haben einen Termin:\n";

  if (ucIsTermin && ucTerminWarning)
  {
      time_t t = time(NULL);
      struct tm *tm;
      tm = localtime(&t);
    if (tm->tm_mday == trmNext.dtDate.ucDay &&
        tm->tm_mon == trmNext.dtDate.ucMonth &&
        tm->tm_year == trmNext.dtDate.uiYear &&
        tm->tm_hour == trmNext.dtDate.ucHour &&
        tm->tm_min == trmNext.dtDate.ucMinute)
    {
      strcat(acWarning, trmNext.acText);
      ucIsTermin = next_termin(&trmNext);
      utl_beep();
      box_info(BOX_INFO, BOX_OK, acWarning, 0);
    }
  }
}

void login_user (void)
{

  UCHAR aucName[21],
        aucPassword[21],
        ucCounter,
        ucRemind,
        ucPasswordRight = FALSE;
  UINT  uiUser;

  do
  {
    if (uiUsers > 1)
      do
      {
        user_box(aucName);
      } while (!get_user(aucName, &uiUser));
    else
      uiUser = 0;
    if (!has_password(uiUser))
      break;
    for (ucCounter = 0; ucCounter < 3; ucCounter++)
    {
      if (!password_box(aucPassword, " Paáwort eingeben "))
        break;
      ucPasswordRight = password_right(uiUser, aucPassword);
      if (ucPasswordRight)
        break;
    }
  } while (!ucPasswordRight);
  uiUserID = uiUser;
  ucRemind = read_user(uiUserID, &userAktUser);
  ucIsTermin = next_termin(&trmNext);
  utl_insert_null_event(termin_warning);
  if (ucRememberList == REM_START ||
      (ucRememberList == REM_DAILY && ucRemind))
    remember_box();
}

void init_haupt_menu (void)
{

  UINT  uiHauptMenu;
  UCHAR ucMouse,
        ucCounter;

  ucMouse = msm_cursor_off();
  //dsk_sw_a(1, 2, 80, 23, AKT_PALETTE.ucAppSpace);
  if (ucMouse)
    msm_cursor_on();
  uiHauptMenu = open_menu_file(uiUserID);
  pmenuHauptMenu = utl_alloc(sizeof(_MNU_MENU));
  read_menu(uiHauptMenu, NULL, pmenuHauptMenu);
  pmenuAktMenu = pmenuHauptMenu;
  show_window(pmenuAktMenu);
  for (ucCounter = 0; userAktUser.aucItems[ucCounter] != 0xff; ucCounter++)
  {
    pmenuAktMenu->ucAktItem = userAktUser.aucItems[ucCounter];
    chain_menu(pmenuAktMenu, pmenuAktMenu->ucAktItem);
    pmenuAktMenu = pmenuAktMenu->pmenuUnterMenu;
    show_window(pmenuAktMenu);
    mnu_item_active(ID_SCHLIESSEN, TRUE);
  }
}

static void
activate_current_entry (void)
{
    UCHAR ucAktItem = pmenuAktMenu->ucAktItem;
    if (pmenuAktMenu->ameEintrag[ucAktItem].ucProgramm)
    {
        get_items();
        write_user(uiUserID, &userAktUser);
        execute(pmenuAktMenu->ameEintrag[ucAktItem].aucWorkDir,
                pmenuAktMenu->ameEintrag[ucAktItem].aucCmdLine);
    }
    else
    {
        pmenuAktMenu->ucAktItem = ucAktItem;
        chain_menu(pmenuAktMenu, ucAktItem);
        pmenuAktMenu = pmenuAktMenu->pmenuUnterMenu;
        show_window(pmenuAktMenu);
        ucAktItem = 0;
        mnu_item_active(ID_SCHLIESSEN, TRUE);
    }
}

static void
close_current_menu (void)
{
    if (pmenuAktMenu->pmenuOberMenu)
    {
        if (pmenuAktMenu->ucChanged)
            write_menu(pmenuAktMenu);
        win_delete(pmenuAktMenu->winWindow);
        pmenuAktMenu = pmenuAktMenu->pmenuOberMenu;
        utl_free(pmenuAktMenu->pmenuUnterMenu);
        pmenuAktMenu->pmenuUnterMenu = NULL;
        pmenuAktMenu->ucChanged = TRUE;
        if (pmenuAktMenu == pmenuHauptMenu)
            mnu_item_active(ID_SCHLIESSEN, FALSE);
    }
}

UCHAR ucBewegen = FALSE;

static void
global_event_handler (GLB_PROGRAM *prg, UTL_EVENT *event)
{
    UCHAR             aucName[61],
    aucHelpLine[79],
    aucDatei[129],
    aucWorkDir[129],
    aucPassword[21],
    aucPasswordVerify[21],
    ucZaehler,
    ucWinX,
    ucWinY,
    *pucTemp;
    //UTL_EVENT         eventEvent;
    FILE             *pfileDatei;
    UCHAR ucAktItem;
    MNU_EINTRAG *pmeAktEintrag;
    UCHAR        ucItems,
    ucAngegriffen = FALSE;
    ULONG        ulLastClick   = 0;
    WINDOW winAkt;
    
    if (pmenuAktMenu == NULL)
    {
        assert(event->uiKind == E_MESSAGE && event->uiMessage == M_INIT);
        return;
    }
    
    ucAktItem = pmenuAktMenu->ucAktItem;
    pmeAktEintrag = &pmenuAktMenu->ameEintrag[ucAktItem];
    winAkt = pmenuAktMenu->winWindow;
    
    if (pmenuAktMenu->ucEintraege)
    {
        ucItems = TRUE;
        win_sw_a(winAkt, 2, 2 + pmenuAktMenu->ucAktItem, winAkt->iWidth - 4, 1,
                 win_get_color(winAkt, PAL_COL_LIST_BOX_SEL_ITEM));
        if (!ucBewegen)
            write_help(pmeAktEintrag->aucHelpLine);
    }
    else
    {
        ucItems = FALSE;
        win_sw_a(winAkt, 2, 2, winAkt->iWidth - 4, 1,
                 win_get_color(pmenuAktMenu->winWindow, PAL_COL_LIST_BOX_SEL_ITEM));
        write_help(pucLeer);
    }
    
    switch (event->uiKind)
    {
        case E_MESSAGE :
            switch (event->uiMessage)
        {
            case ID_OEFFNEN :
                if (ucItems)
                    activate_current_entry();
                break;
            case ID_SCHLIESSEN :
                close_current_menu();
                break;
            case ID_UEBER :
                box_info(BOX_INFO, BOX_OK, pucUeber, 0);
                break;
            case ID_SHUTDOWN :
                get_items();
                write_user(uiUserID, &userAktUser);
                write_config();
                save_menus();
                utl_del_null_event(termin_warning);
                box_info(BOX_INFO, BOX_CANCEL, pucPark, 0);
                utl_insert_null_event(termin_warning);
                break;
            case ID_ENDE :
                if (userAktUser.uiRights & RIGHT_QUIT)
                {
                    get_items();
                    write_user(uiUserID, &userAktUser);
                    ende(0);
                }
                break;
            case ID_LOGOUT :
                get_items();
                write_user(uiUserID, &userAktUser);
                clear_desktop();
                write_config();
                utl_del_null_event(termin_warning);
                bExit = TRUE;
                break;
            case ID_DAZUFUEGEN :
                if (!(userAktUser.uiRights & RIGHT_CHANGE))
                    break;
                if (pmenuAktMenu->ucEintraege >= 20)
                {
                    box_info(BOX_INFO, BOX_OK, pucMaxEintraege, 0);
                    break;
                }
                switch (get_grp_prg())
            {
                case BOX_PROGRAMM :
                    strcpy(pmenuAktMenu->ameEintrag[pmenuAktMenu->ucEintraege].
                           aucName, pucLeer);
                    strcpy(pmenuAktMenu->ameEintrag[pmenuAktMenu->ucEintraege].
                           aucHelpLine, pucLeer);
                    strcpy(pmenuAktMenu->ameEintrag[pmenuAktMenu->ucEintraege].
                           aucCmdLine, pucLeer);
                    strcpy(pmenuAktMenu->ameEintrag[pmenuAktMenu->ucEintraege].
                           aucWorkDir, pucLeer);
                    if (programm_box(" Programm hinzufgen ",
                                     pmenuAktMenu->ameEintrag
                                     [pmenuAktMenu->ucEintraege].aucName,
                                     pmenuAktMenu->ameEintrag
                                     [pmenuAktMenu->ucEintraege].aucHelpLine,
                                     pmenuAktMenu->ameEintrag
                                     [pmenuAktMenu->ucEintraege].aucCmdLine,
                                     pmenuAktMenu->ameEintrag
                                     [pmenuAktMenu->ucEintraege].aucWorkDir))
                    {
                        pmenuAktMenu->ameEintrag[pmenuAktMenu->ucEintraege].
                        ucProgramm = TRUE;
                        pmenuAktMenu->ucEintraege++;
                        pucTemp = winAkt->pcTitle;
                        make_window(pmenuAktMenu, min(winAkt->iX,
                                                      73 - get_longest_name
                                                      (pmenuAktMenu, pucTemp)),
                                    min(winAkt->iY,
                                        21 - pmenuAktMenu->
                                        ucEintraege), pucTemp);
                        win_delete(winAkt);
                        show_window(pmenuAktMenu);
                        pmenuAktMenu->ucChanged = TRUE;
                    }
                    break;
                case BOX_GRUPPE :
                    strcpy(pmenuAktMenu->ameEintrag[pmenuAktMenu->ucEintraege].
                           aucName, pucLeer);
                    strcpy(pmenuAktMenu->ameEintrag[pmenuAktMenu->ucEintraege].
                           aucHelpLine, pucLeer);
                    if (gruppen_box(" Gruppe hinzufgen ",
                                    pmenuAktMenu->ameEintrag
                                    [pmenuAktMenu->ucEintraege].aucName,
                                    pmenuAktMenu->ameEintrag
                                    [pmenuAktMenu->ucEintraege].aucHelpLine))
                    {
                        pmenuAktMenu->ameEintrag[pmenuAktMenu->ucEintraege].
                        ucProgramm = FALSE;
                        pmenuAktMenu->ucEintraege++;
                        pucTemp = winAkt->pcTitle;
                        make_window(pmenuAktMenu, min(winAkt->iX,
                                                      73 - get_longest_name
                                                      (pmenuAktMenu, pucTemp)),
                                    min(winAkt->iY,
                                        21 - pmenuAktMenu->
                                        ucEintraege), pucTemp);
                        win_delete(winAkt);
                        show_window(pmenuAktMenu);
                        pmenuAktMenu->ucChanged = TRUE;
                        pmenuAktMenu->ameEintrag[pmenuAktMenu->ucEintraege - 1].
                        uiGruppe = create_menu(pmenuAktMenu->ameEintrag
                                               [pmenuAktMenu->ucEintraege - 1].aucName);
                    }
                    break;
            }
                break;
            case ID_EINFUEGEN :
                if (!(userAktUser.uiRights & RIGHT_CHANGE))
                    break;
                if (pmenuAktMenu->ucEintraege >= 20)
                {
                    box_info(BOX_INFO, BOX_OK, pucMaxEintraege, 0);
                    break;
                }
                switch (get_grp_prg())
            {
                case BOX_PROGRAMM :
                    strcpy(aucName, pucLeer);
                    strcpy(aucHelpLine, pucLeer);
                    strcpy(aucDatei, pucLeer);
                    strcpy(aucWorkDir, pucLeer);
                    if (programm_box(" Programm einfgen ", aucName, aucHelpLine,
                                     aucDatei, aucWorkDir))
                    {
                        memmove(pmenuAktMenu->ameEintrag + ucAktItem + 1,
                                pmenuAktMenu->ameEintrag + ucAktItem,
                                sizeof(MNU_EINTRAG) *
                                (pmenuAktMenu->ucEintraege - ucAktItem));
                        pmenuAktMenu->ucEintraege++;
                        pmenuAktMenu->ameEintrag[ucAktItem].ucProgramm = TRUE;
                        strcpy(pmenuAktMenu->ameEintrag[ucAktItem].aucName,
                               aucName);
                        strcpy(pmenuAktMenu->ameEintrag[ucAktItem].aucHelpLine,
                               aucHelpLine);
                        strcpy(pmenuAktMenu->ameEintrag[ucAktItem].aucCmdLine,
                               aucDatei);
                        strcpy(pmenuAktMenu->ameEintrag[ucAktItem].aucWorkDir,
                               aucWorkDir);
                        pucTemp = winAkt->pcTitle;
                        make_window(pmenuAktMenu, min(winAkt->iX,
                                                      73 - get_longest_name
                                                      (pmenuAktMenu, pucTemp)),
                                    min(winAkt->iY,
                                        21 - pmenuAktMenu->
                                        ucEintraege), pucTemp);
                        win_delete(winAkt);
                        show_window(pmenuAktMenu);
                        pmenuAktMenu->ucChanged = TRUE;
                    }
                    break;
                case BOX_GRUPPE :
                    strcpy(aucName, pucLeer);
                    strcpy(aucHelpLine, pucLeer);
                    if (gruppen_box(" Gruppe einfgen ", aucName, aucHelpLine))
                    {
                        memmove(pmenuAktMenu->ameEintrag + ucAktItem + 1,
                                pmenuAktMenu->ameEintrag + ucAktItem,
                                sizeof(MNU_EINTRAG) *
                                (pmenuAktMenu->ucEintraege - ucAktItem));
                        pmenuAktMenu->ucEintraege++;
                        pmenuAktMenu->ameEintrag[ucAktItem].ucProgramm = FALSE;
                        strcpy(pmenuAktMenu->ameEintrag[ucAktItem].aucName,
                               aucName);
                        strcpy(pmenuAktMenu->ameEintrag[ucAktItem].aucHelpLine,
                               aucHelpLine);
                        pucTemp = winAkt->pcTitle;
                        make_window(pmenuAktMenu, min(winAkt->iX,
                                                      73 - get_longest_name
                                                      (pmenuAktMenu, pucTemp)),
                                    min(winAkt->iY,
                                        21 - pmenuAktMenu->
                                        ucEintraege), pucTemp);
                        win_delete(winAkt);
                        show_window(pmenuAktMenu);
                        pmenuAktMenu->ucChanged = TRUE;
                        pmenuAktMenu->ameEintrag[ucAktItem].uiGruppe =
                        create_menu(pmenuAktMenu->ameEintrag[ucAktItem].aucName);
                    }
                    break;
            }
                break;
            case ID_BEARBEITEN :
                if (!(userAktUser.uiRights & RIGHT_CHANGE))
                    break;
                if (pmenuAktMenu->ucEintraege)
                {
                    if (pmenuAktMenu->ameEintrag[ucAktItem].ucProgramm)
                    {
                        strcpy(aucName, pmenuAktMenu->ameEintrag[ucAktItem].aucName);
                        strcpy(aucHelpLine,
                               pmenuAktMenu->ameEintrag[ucAktItem].aucHelpLine);
                        strcpy(aucDatei,
                               pmenuAktMenu->ameEintrag[ucAktItem].aucCmdLine);
                        strcpy(aucWorkDir,
                               pmenuAktMenu->ameEintrag[ucAktItem].aucWorkDir);
                        if (programm_box(" Programm bearbeiten ", aucName, aucHelpLine,
                                         aucDatei, aucWorkDir))
                        {
                            strcpy(pmenuAktMenu->ameEintrag[ucAktItem].aucName,
                                   aucName);
                            strcpy(pmenuAktMenu->ameEintrag[ucAktItem].aucHelpLine,
                                   aucHelpLine);
                            strcpy(pmenuAktMenu->ameEintrag[ucAktItem].aucCmdLine,
                                   aucDatei);
                            strcpy(pmenuAktMenu->ameEintrag[ucAktItem].aucWorkDir,
                                   aucWorkDir);
                            pucTemp = winAkt->pcTitle;
                            make_window(pmenuAktMenu, min(winAkt->iX,
                                                          73 - get_longest_name
                                                          (pmenuAktMenu, pucTemp)),
                                        min(winAkt->iY,
                                            21 - pmenuAktMenu->
                                            ucEintraege), pucTemp);
                            win_delete(winAkt);
                            show_window(pmenuAktMenu);
                            pmenuAktMenu->ucChanged = TRUE;
                        }
                    }
                    else
                    {
                        strcpy(aucName, pmenuAktMenu->ameEintrag[ucAktItem].aucName);
                        strcpy(aucHelpLine,
                               pmenuAktMenu->ameEintrag[ucAktItem].aucHelpLine);
                        if (gruppen_box(" Gruppe bearbeiten ", aucName, aucHelpLine))
                        {
                            strcpy(pmenuAktMenu->ameEintrag[ucAktItem].aucName,
                                   aucName);
                            strcpy(pmenuAktMenu->ameEintrag[ucAktItem].aucHelpLine,
                                   aucHelpLine);
                            pucTemp = winAkt->pcTitle;
                            make_window(pmenuAktMenu, min(winAkt->iX,
                                                          73 - get_longest_name
                                                          (pmenuAktMenu, pucTemp)),
                                        min(winAkt->iY,
                                            21 - pmenuAktMenu->
                                            ucEintraege), pucTemp);
                            win_delete(winAkt);
                            show_window(pmenuAktMenu);
                            pmenuAktMenu->ucChanged = TRUE;
                        }
                    }
                }
                break;
            case ID_PRG_BEWEGEN :
                if (pmenuAktMenu->ucEintraege < 2)
                {
                    utl_beep();
                    break;
                }
                sts_write_help(" Bitte Cursortasten oder Maus zum Bewegen des"
                               " Programms benutzen");
                // FIXME: implement this modally, or just set ucBewegen?
                //auswahl(&uiWahl, &ucAktItem, TRUE);
                pmenuAktMenu->ucChanged = TRUE;
                if (ucButtonBar)
                    sts_clear_help();
                break;
            case ID_SORT_ALPHA :
                qsort(pmenuAktMenu->ameEintrag, pmenuAktMenu->ucEintraege,
                      sizeof(MNU_EINTRAG),
                      (int (*) (const void*, const void*))strcmp);
                pmenuAktMenu->ucChanged = TRUE;
                show_window(pmenuAktMenu);
                break;
            case ID_LOESCHEN :
                if (!(userAktUser.uiRights & RIGHT_CHANGE))
                    break;
                if (!(pmenuAktMenu->ucEintraege))
                {
                    utl_beep();
                    break;
                }
                if (pmenuAktMenu->ameEintrag[ucAktItem].ucProgramm)
                {
                    if (box_info(BOX_INFO, BOX_OK | BOX_CANCEL, pucWirklichPrgDel,
                                 0) != BOX_OK)
                        break;
                }
                else
                {
                    if (box_info(BOX_INFO, BOX_OK | BOX_CANCEL, pucWirklichGrpDel,
                                 0) != BOX_OK)
                        break;
                    del_gruppe(pmenuAktMenu->ameEintrag[ucAktItem].uiGruppe);
                }
                memmove(pmenuAktMenu->ameEintrag + ucAktItem,
                        pmenuAktMenu->ameEintrag + ucAktItem + 1,
                        sizeof(MNU_EINTRAG) *
                        (pmenuAktMenu->ucEintraege - 1 - ucAktItem));
                pmenuAktMenu->ucEintraege--;
                if (ucAktItem == pmenuAktMenu->ucEintraege)
                {
                    pmenuAktMenu->ucAktItem--;
                    ucAktItem--;
                }
                pucTemp = winAkt->pcTitle;
                make_window(pmenuAktMenu, winAkt->iX, winAkt->iY, pucTemp);
                win_delete(winAkt);
                show_window(pmenuAktMenu);
                pmenuAktMenu->ucChanged = TRUE;
                break;
            case ID_BEWEGEN :
                sts_write_help(" Bitte benutzen Sie die Cursortasten und Enter");
                utl_move_win();
                if (ucButtonBar)
                    sts_clear_help();
                pmenuAktMenu->ucChanged = TRUE;
                break;
            case ID_TITEL :
                strcpy(aucName, winAkt->pcTitle);
                if (titel_box(aucName))
                {
                    pucTemp = aucName;
                    make_window(pmenuAktMenu, min(winAkt->iX,
                                                  73 - get_longest_name
                                                  (pmenuAktMenu, pucTemp)),
                                min(winAkt->iY,
                                    21 - pmenuAktMenu->
                                    ucEintraege), pucTemp);
                    win_delete(winAkt);
                    show_window(pmenuAktMenu);
                    pmenuAktMenu->ucChanged = TRUE;
                }
                break;
            case ID_KALENDER :
                termin_box();
                ucIsTermin = next_termin(&trmNext);
                break;
            case ID_REMEMBER :
                remember_box();
                break;
            case ID_OPTIONS :
                termin_option_box();
                break;
            case ID_PASSWORD :
                if (password_box(aucPassword, " Paáwort eingeben "))
                    if (password_box(aucPasswordVerify, " Paáwort verifizieren "))
                    {
                        if (!strcmp(utl_str_upper(aucPassword),
                                    utl_str_upper(aucPasswordVerify)))
                        {
                            strcpy(userAktUser.aucPassword, aucPassword);
                            encrypt_password(userAktUser.aucPassword);
                            write_user_info(uiUserID, &userAktUser);
                        }
                        else
                            box_info(BOX_INFO, BOX_OK, pucDoNotMatch, 0);
                    }
                break;
            case ID_SEND_MAIL :
                send_mail();
                break;
            case ID_USER_EDIT :
                user_editor();
                break;
            case ID_CUST_COLORS :
                box_custom_palette();
                //mnu_set_palette();
                //while (win_get_active())
                //    win_hide(win_get_active());
                //msm_cursor_off();
                //dsk_sw_a(1, 2, 80, 23, AKT_PALETTE.ucAppSpace);
                //msm_cursor_on();
                show_windows();
                //sts_set_palette();
                break;
            case ID_FARBPALETTE :
                box_palette();
                //mnu_set_palette();
                //while (win_get_active())
                //    win_hide(win_get_active());
                //msm_cursor_off();
                //dsk_sw_a(1, 2, 80, 23, AKT_PALETTE.ucAppSpace);
                //msm_cursor_on();
                show_windows();
                //sts_set_palette();
                break;
            case ID_MAUSTEMPO :
                box_mouse();
                break;
            case ID_WARNTON :
                box_beep(TRUE);
                break;
            case ID_SCR_SAVER :
                box_screen_saver();
                break;
            case ID_SONSTIGES :
                sonstiges_box();
                break;
            case ID_SUPER_OPT :
                supervisor_box();
                break;
            default:
                return;
        }
            break;
        case E_MSM_L_DOWN :
            if (IN_CLICK_FIELD)
            {
                if (((event->dwTimer - ulLastClick) <= (ulDoubleClick / 55)) &&
                    (pmeAktEintrag == (pmenuAktMenu->ameEintrag +
                                       event->iVer - winAkt->iY - 2)) && ucItems)
                {
                    activate_current_entry();
                }
                else
                {
                    ucAngegriffen = TRUE;
                    ulLastClick = event->dwTimer;
                    win_sw_a(winAkt, 2, 2 + (pmeAktEintrag - pmenuAktMenu->ameEintrag),
                             winAkt->iWidth - 4, 1, win_get_color(NULL, PAL_COL_BACKGROUND));
                    if (ucBewegen)
                        bewegen(pmenuAktMenu, &pmeAktEintrag,
                                event->iVer - winAkt->iY - 2);
                    else
                    {
                        pmeAktEintrag = pmenuAktMenu->ameEintrag + event->iVer - winAkt->iY - 2;
                        win_sw_a(winAkt, 2,
                                 2 + (pmeAktEintrag - pmenuAktMenu->ameEintrag),
                                 winAkt->iWidth - 4, 1,
                                 win_get_color(winAkt, PAL_COL_LIST_BOX_SEL_ITEM));
                        if (ucItems)
                            write_help(pmeAktEintrag->aucHelpLine);
                    }
                }
            }
            break;
        case E_MSM_L_UP :
            ucAngegriffen = FALSE;
            break;
        case E_MSM_R_DOWN :
            if (!ucBewegen)
                if (pmenuAktMenu != pmenuHauptMenu)
                    close_current_menu();
            break;
        case E_MSM_MOVE :
            if (ucAngegriffen && IN_CLICK_FIELD)
            {
                if (ucBewegen)
                    bewegen(pmenuAktMenu, &pmeAktEintrag,
                            event->iVer - winAkt->iY - 2);
                else
                {
                    win_sw_a(winAkt, 2, 2 + (pmeAktEintrag - pmenuAktMenu->ameEintrag),
                             winAkt->iWidth - 4, 1, win_get_color(winAkt, PAL_COL_BACKGROUND));
                    pmeAktEintrag = pmenuAktMenu->ameEintrag + event->iVer - winAkt->iY - 2;
                    win_sw_a(winAkt, 2, 2 + (pmeAktEintrag - pmenuAktMenu->ameEintrag),
                             winAkt->iWidth - 4, 1,
                             win_get_color(winAkt, PAL_COL_LIST_BOX_SEL_ITEM));
                    if (ucItems)
                        write_help(pmeAktEintrag->aucHelpLine);
                }
            }
            break;
        case E_KEY :
            switch (event->wKey)
        {
            case K_ENTER :
                activate_current_entry();
                break;
            case K_UP :
                if (!ucItems)
                {
                    utl_beep();
                    break;
                }
                if (pmeAktEintrag != pmenuAktMenu->ameEintrag)
                {
                    pmenuAktMenu->ucAktItem--;
                    if (ucBewegen)
                        bewegen(pmenuAktMenu, &pmeAktEintrag,
                                pmeAktEintrag - pmenuAktMenu->ameEintrag - 1);
                    else
                    {
                        win_sw_a(winAkt, 2,
                                 2 + (pmeAktEintrag - pmenuAktMenu->ameEintrag),
                                 winAkt->iWidth - 4, 1,
                                 win_get_color(winAkt, PAL_COL_BACKGROUND));
                        pmeAktEintrag--;
                        win_sw_a(winAkt, 2,
                                 2 + (pmeAktEintrag - pmenuAktMenu->ameEintrag),
                                 winAkt->iWidth - 4, 1,
                                 win_get_color(winAkt, PAL_COL_LIST_BOX_SEL_ITEM));
                        if (ucItems)
                            write_help(pmeAktEintrag->aucHelpLine);
                    }
                }
                else
                    utl_beep();
                break;
            case K_HOME :
                if (!ucItems)
                {
                    utl_beep();
                    break;
                }
                if (pmeAktEintrag != pmenuAktMenu->ameEintrag)
                {
                    pmenuAktMenu->ucAktItem = 0;
                    if (ucBewegen)
                        bewegen(pmenuAktMenu, &pmeAktEintrag, 0);
                    else
                    {
                        win_sw_a(winAkt, 2,
                                 2 + (pmeAktEintrag - pmenuAktMenu->ameEintrag),
                                 winAkt->iWidth - 4, 1,
                                 win_get_color(winAkt, PAL_COL_BACKGROUND));
                        pmeAktEintrag = pmenuAktMenu->ameEintrag;
                        win_sw_a(winAkt, 2,
                                 2 + (pmeAktEintrag - pmenuAktMenu->ameEintrag),
                                 winAkt->iWidth - 4, 1,
                                 win_get_color(winAkt, PAL_COL_LIST_BOX_SEL_ITEM));
                        if (ucItems)
                            write_help(pmeAktEintrag->aucHelpLine);
                    }
                }
                else
                    utl_beep();
                break;
            case K_DOWN :
                if (!ucItems)
                {
                    utl_beep();
                    break;
                }
                if (pmeAktEintrag - pmenuAktMenu->ameEintrag + 1 <
                    pmenuAktMenu->ucEintraege)
                {
                    pmenuAktMenu->ucAktItem++;
                    if (ucBewegen)
                        bewegen(pmenuAktMenu, &pmeAktEintrag, pmeAktEintrag -
                                pmenuAktMenu->ameEintrag + 1);
                    else
                    {
                        win_sw_a(winAkt, 2,
                                 2 + (pmeAktEintrag - pmenuAktMenu->ameEintrag),
                                 winAkt->iWidth - 4, 1,
                                 win_get_color(winAkt, PAL_COL_BACKGROUND));
                        pmeAktEintrag++;
                        win_sw_a(winAkt, 2,
                                 2 + (pmeAktEintrag - pmenuAktMenu->ameEintrag),
                                 winAkt->iWidth - 4, 1,
                                 win_get_color(winAkt, PAL_COL_LIST_BOX_SEL_ITEM));
                        if (ucItems)
                            write_help(pmeAktEintrag->aucHelpLine);
                    }
                }
                else
                    utl_beep();
                break;
            case K_END :
                if (!ucItems)
                {
                    utl_beep();
                    break;
                }
                if (pmeAktEintrag - pmenuAktMenu->ameEintrag + 1 <
                    pmenuAktMenu->ucEintraege)
                {
                    pmenuAktMenu->ucAktItem = pmenuAktMenu->ucEintraege - 1;
                    if (ucBewegen)
                        bewegen(pmenuAktMenu, &pmeAktEintrag,
                                pmenuAktMenu->ucEintraege - 1);
                    else
                    {
                        win_sw_a(winAkt, 2,
                                 2 + (pmeAktEintrag - pmenuAktMenu->ameEintrag),
                                 winAkt->iWidth - 4, 1,
                                 win_get_color(winAkt, PAL_COL_BACKGROUND));
                        pmeAktEintrag = pmenuAktMenu->ameEintrag +
                        pmenuAktMenu->ucEintraege - 1;
                        win_sw_a(winAkt, 2,
                                 2 + (pmeAktEintrag - pmenuAktMenu->ameEintrag),
                                 winAkt->iWidth - 4, 1,
                                 win_get_color(winAkt, PAL_COL_LIST_BOX_SEL_ITEM));
                        if (ucItems)
                            write_help(pmeAktEintrag->aucHelpLine);
                    }
                }
                else
                    utl_beep();
                break;
            default:
                return;
        }
            break;
        default:
            return;
    }
    event->uiKind = E_DONE;
}

static void
menu_window_event_handler (WINDOW win, UTL_EVENT *event)
{
    switch (event->uiKind)
    {
        case E_MESSAGE:
            switch (event->uiMessage)
            {
                case M_MOVE:
                    pmenuAktMenu->ucChanged = TRUE;
                    goto done;
            }
    }

    win_std_handler(win, event);
    return;
    
done:
    event->uiKind = E_DONE;
}

void show_mail (void)
{

  UCHAR       *pucMsgText;
  MNU_MESSAGE  msgMessage;

  while ((pucMsgText = get_message(&msgMessage)))
  {
    show_message(pucMsgText, &msgMessage);
    utl_free(pucMsgText);
  }
}

void ende (UINT uiStatus)
{
  write_config();
  write_user(uiUserID, &userAktUser);
  save_menus();
  exit(uiStatus);
}

int
main(void)
{
    glb_init(global_event_handler);
    win_register_class(WIN_CLASS_MENU, PAL_WIN_DLG, B_EEEE, TITLE_T_C,
                       WIN_FL_SHADOW | WIN_FL_MOVEABLE | WIN_FL_FOCUSABLE,
                       win_std_handler);
    
    read_config();
    uiUsers = open_user_file();
    open_message_file();
    open_termin_file();
    pfileMemo = mem_open_file("SEMENU.MEM");
    hlp_open_file("SEMENU.HLP");
    while (TRUE)
    {
        login_user();
        init_haupt_menu();
        init_screen();
        show_mail();
        glb_run();
    }
    return(0);
}
