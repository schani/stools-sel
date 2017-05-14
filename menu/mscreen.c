/***************************************************************************
 ***************************************************************************
 ***                                                                     ***
 ***                             SE-Menu                                 ***
 ***                                                                     ***
 ***                        Bildschirmausgabe                            ***
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
#include <time.h>
#include <context.h>
#include "menu.h"

extern _MNU_MENU *pmenuAktMenu;
extern _MNU_MENU *pmenuHauptMenu;
extern UCHAR      ucUhr;
extern UCHAR      ucButtonBar;
extern MNU_USER   userAktUser;

STS_ITEM aitemStatusLine[] =
		             {
			       {
			         "~+~ Hinzufgen",
				 ID_DAZUFUEGEN,
				 '+',
                                 TRUE
			       },
			       {
			         "~Einfg~ Einfgen",
				 ID_EINFUEGEN,
				 K_INS,
                                 TRUE
			       },
			       {
			         "~Space~ Bearbeiten",
				 ID_BEARBEITEN,
				 K_SPACE,
                                 TRUE
			       },
			       {
			         "~Entf~ L”schen",
				 ID_LOESCHEN,
				 K_DEL,
                                 TRUE
			       },
			       {
                                 "~Alt-X~ Ende",
				 ID_ENDE,
				 K_A_X,
                                 TRUE
			       },
			       STS_END
			     };

void clear_desktop (void)
{

  _MNU_MENU *pmenuMenu,
            *pmenuBuffer;

  save_menus();
  for (pmenuMenu = pmenuAktMenu; pmenuMenu; )
  {
    win_delete(pmenuMenu->winWindow);
    pmenuBuffer = pmenuMenu->pmenuOberMenu;
    utl_free(pmenuMenu);
    pmenuMenu = pmenuBuffer;
  }
}

void uhr_zeigen (void)
{

  time_t       tTime;
  struct tm    *ptmLocal;
  UCHAR        aucPuffer[10];
  static INT   iLastSecond;
  ULONG        ulHeap;
  static ULONG ulLastHeap;

  if (ucUhr && !bSaverActive)
  {
    tTime = time(NULL);
    ptmLocal = localtime(&tTime);
    if (ptmLocal->tm_sec != iLastSecond)
    {
      sprintf(aucPuffer, "%2d:%02d:%02d",
	      ptmLocal->tm_hour, ptmLocal->tm_min, ptmLocal->tm_sec);
      msm_cursor_off();
      dsk_ss(73, 1, aucPuffer);
      msm_cursor_on();
      iLastSecond = ptmLocal->tm_sec;
    }
  }
  else
  {
    ulLastHeap = 0;
    iLastSecond = 62;
  }
}

void init_screen (void)
{
    mnu_new_menu(HAUPTMENU, 1,
                  mnu_new_item(DATEI, "#Datei", " Programmstart, Programminformationen, Programmende", 0, 0, mnu_new_window(
                  mnu_new_item(ID_OEFFNEN, "#™ffnen                 Enter", " Startet das Programm bzw. ”ffnet die Gruppe", K_ENTER, 0, NULL,
                  mnu_new_item(ID_SCHLIESSEN, "#Schlieáen                Esc", " Schlieát die Programmgruppe", K_ESC, 0, NULL,
			      mnu_new_line(
			      mnu_new_item(ID_UEBER, "#šber...", " Programminformationen", 0, 0, NULL,
			      mnu_new_line(
			      mnu_new_item(ID_SHUTDOWN, "#Beenden der Sitzung...", " Parkt die K”pfe der Festplatte", 0, 0, NULL,
			      mnu_new_item(ID_ENDE, "#Ende                   Alt-X", " Programmende", K_A_X, 0, NULL,
			      NULL)))))))),
		 mnu_new_item(PROGRAMM, "#Programm", " Programme einrichten und l”schen", 0, 0, mnu_new_window(
			      mnu_new_item(ID_DAZUFUEGEN, "#Hinzufgen...              +", " Richtet ein neues Programm am Ende der Liste ein", '+', 0, NULL,
			      mnu_new_item(ID_EINFUEGEN, "#Einfgen...            Einfg", " Fgt ein neues Programm vor dem Auswahlcursor ein", K_INS, 0, NULL,
			      mnu_new_item(ID_BEARBEITEN, "#Bearbeiten...          Space", " Bearbeitet das aktuelle Programm", K_SPACE, 0, NULL,
			      mnu_new_item(ID_PRG_BEWEGEN, "Be#wegen", " Bewegt das aktuelle Programm an eine beliebige Position", 0, 0, NULL,
			      mnu_new_line(
			      mnu_new_item(ID_LOESCHEN, "#L”schen...              Entf", " L”scht das aktuelle Programm", K_DEL, 0, NULL,
			      mnu_new_line(
			      mnu_new_item(ID_SORT_ALPHA, "#Alphabetisch sortieren", " Sortiert die Programme dem Namen nach", 0, 0, NULL,
			      mnu_new_line(
			      mnu_new_item(ID_BEWEGEN, "#Fenster bewegen", " Bewegt das aktuelle Fenster", 0, 0, NULL,
			      mnu_new_item(ID_TITEL, "Fenster#titel...", " Setzt den Titel des aktuellen Fensters", 0, 0, NULL,
			      NULL)))))))))))),
         mnu_new_item(TERMINE, "#Termine", " Terminkalender, Remember-Liste, Optionen", 0, 0, mnu_new_window(
			      mnu_new_item(ID_KALENDER, "#Terminkalender...", " Zeigt alle gespeicherten Termine", 0, 0, NULL,
			      mnu_new_item(ID_REMEMBER, "#Remember-Liste...", " Zeigt die Termine von heute und morgen", 0, 0, NULL,
			      mnu_new_line(
			      mnu_new_item(ID_OPTIONS, "#Optionen...", " Optionen die Termine betreffend", 0, 0, NULL,
			      NULL))))),
         mnu_new_item(USER, "#User", " Paáwort „ndern, Nachrichten schicken und User-Editor", 0, 0, mnu_new_window(
			      mnu_new_item(ID_LOGOUT, "#Ausloggen", " Loggt aus dem Men aus", 0, 0, NULL,
			      mnu_new_line(
			      mnu_new_item(ID_PASSWORD, "#Paáwort „ndern...", " Žndert das Paáwort des aktiven Users", 0, 0, NULL,
			      mnu_new_item(ID_SEND_MAIL, "#Nachrichten schicken...", " Schickt einem anderen User eine Nachricht", 0, 0, NULL,
			      mnu_new_line(
			      mnu_new_item(ID_USER_EDIT, "#User-Editor...", " Editiert die vorhandenen User", 0, 0, NULL,
			      NULL))))))),
		 mnu_new_item(OPTIONEN, "#Optionen", " Programmeinstellungen", 0, 0, mnu_new_window(
			      mnu_new_item(ID_FARBPALETTE, "#Farbpalette...", " Wahl der Farbpalette", 0, CTX_BOX_FARBPALETTE, NULL,
			      mnu_new_item(ID_CUST_COLORS, "#Eigene Farbpalette...", " Definition einer eigenen Farbpalette", 0, CTX_BOX_CUSTOM_FARBEN, NULL,
			      mnu_new_item(ID_MAUSTEMPO, "#Maustempo...", " Wahl des Maustempos", 0, CTX_BOX_MOUSE, NULL,
			      mnu_new_item(ID_WARNTON, "#Warnton...", " Wahl der Dauer und der Frequenz des Warntons", 0, CTX_BOX_WARNTON, NULL,
			      mnu_new_item(ID_SCR_SAVER, "#Bildschirmschoner...", " Wahl der Zeitspanne des Bildschirmschoners", 0, CTX_BOX_SCREEN_SAVER, NULL,
			      mnu_new_item(ID_SONSTIGES, "#Sonstiges...", " Uhr und Statuszeile", 0, 0, NULL,
			      mnu_new_line(
			      mnu_new_item(ID_SUPER_OPT, "S#upervisor-Optionen...", " Kontrolle ber die Ausfhrung der Batchdateien", 0, 0, NULL,
			      NULL))))))))),
         NULL))))));

  utl_del_null_event(uhr_zeigen);
  msm_set_mickeys(uiMouseSpeed, uiMouseSpeed * 2);
  sts_del_status_line();
  if (ucButtonBar)
    sts_new_status_line(aitemStatusLine);
  show_windows();
  utl_insert_null_event(uhr_zeigen);
  if (!(userAktUser.uiRights & RIGHT_SUPERVISOR))
  {
    mnu_item_active(ID_USER_EDIT, FALSE);
    mnu_item_active(ID_SUPER_OPT, FALSE);
  }
  if (!(userAktUser.uiRights & RIGHT_CHANGE))
  {
    mnu_item_active(PROGRAMM, FALSE);
    sts_active(ID_DAZUFUEGEN, FALSE);
    sts_active(ID_EINFUEGEN, FALSE);
    sts_active(ID_BEARBEITEN, FALSE);
    sts_active(ID_LOESCHEN, FALSE);
  }
  else
  {
    sts_active(ID_DAZUFUEGEN, TRUE);
    sts_active(ID_EINFUEGEN, TRUE);
    sts_active(ID_BEARBEITEN, TRUE);
    sts_active(ID_LOESCHEN, TRUE);
  }
  if (!(userAktUser.uiRights & RIGHT_PWD_CHANGE))
    mnu_item_active(ID_PASSWORD, FALSE);
  if (!(userAktUser.uiRights & RIGHT_QUIT))
  {
    mnu_item_active(ID_ENDE, FALSE);
    mnu_item_active(ID_SHUTDOWN, FALSE);
    sts_active(ID_ENDE, FALSE);
  }
  else
    sts_active(ID_ENDE, TRUE);
}

void show_window (_MNU_MENU* pmenuMenu)
{
    UCHAR ucZaehler;
    WINDOW win = pmenuMenu->winWindow;
    
    win_show(win);
    win_cls(win);
    for (ucZaehler = 0; ucZaehler < pmenuMenu->ucEintraege; ucZaehler++)
        win_ss(win, 3, 2 + ucZaehler, pmenuMenu->ameEintrag[ucZaehler].aucName);
    win_sw_a(win, 2,
             2 + pmenuMenu->ucAktItem, win->iWidth - 4,
             1, win_get_color(win, PAL_COL_LIST_BOX_SEL_ITEM));
}

void show_windows (void)
{
    _MNU_MENU *pmenuMenu;

    for (pmenuMenu = pmenuAktMenu; pmenuMenu != NULL; pmenuMenu = pmenuMenu->pmenuOberMenu)
        win_hide(pmenuMenu->winWindow);
    
    pmenuMenu = pmenuHauptMenu;
    do
    {
        show_window(pmenuMenu);
        pmenuMenu = pmenuMenu->pmenuUnterMenu;
    } while (pmenuMenu);
}

void write_help (UCHAR *pucHelpLine)
{

  UCHAR aucTemp[80];

  if (!ucButtonBar)
  {
    strcpy(aucTemp, " ");
    strcat(aucTemp, pucHelpLine);
    sts_write_help(aucTemp);
  }
}
