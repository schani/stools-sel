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
#include <alloc.h>
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
				 T_INS,
                                 TRUE
			       },
			       {
			         "~Space~ Bearbeiten",
				 ID_BEARBEITEN,
				 T_SPACE,
                                 TRUE
			       },
			       {
			         "~Entf~ L”schen",
				 ID_LOESCHEN,
				 T_DEL,
                                 TRUE
			       },
			       {
                                 "~Alt-X~ Ende",
				 ID_ENDE,
				 T_A_X,
                                 TRUE
			       },
			       STS_ENDE
			     };

void clear_desktop (void)
{

  _MNU_MENU *pmenuMenu,
            *pmenuBuffer;

  save_menus();
  for (pmenuMenu = pmenuAktMenu; pmenuMenu; )
  {
    win_entfernen(pmenuMenu->winWindow);
    pmenuBuffer = pmenuMenu->pmenuOberMenu;
    utl_free(pmenuMenu);
    pmenuMenu = pmenuBuffer;
  }
}

void uhr_zeigen (void)
{

  struct time  tTime;
  UCHAR        aucPuffer[10];
  static UCHAR ucLastSecond;
  ULONG        ulHeap;
  static ULONG ulLastHeap;

  if (ucUhr && !ucSaverAktiv)
  {
    gettime(&tTime);
    if (tTime.ti_sec != ucLastSecond)
    {
      sprintf(aucPuffer, "%2d:%02d:%02d",
	      tTime.ti_hour, tTime.ti_min, tTime.ti_sec);
      msm_cursor_off();
      dsk_ss(73, 1, aucPuffer);
      msm_cursor_on();
      ucLastSecond = tTime.ti_sec;
    }
    ulHeap = coreleft();
    if (heapcheck() == _HEAPCORRUPT)
    {
      dsk_sw_z(50, 1, 10, 1, ' ');
      dsk_ss(50, 1, "CORRUPT!!!");
    }
    else
    {
      if (ulHeap != ulLastHeap)
      {
        ltoa(ulHeap, aucPuffer, 10);
        msm_cursor_off();
        dsk_sw_z(50, 1, 10, 1, ' ');
        dsk_ss(50, 1, aucPuffer);
        msm_cursor_on();
        ulLastHeap = ulHeap;
      }
    }
  }
  else
  {
    ulLastHeap = 0;
    ucLastSecond = 62;
  }
}

void init_screen (void)
{

  ITEM aitemDatei[]    = {
			   {
			     "#Datei",
			     " Programmstart, Programminformationen, Programmende",
                             0,
			     DATEI
			   },
			   {
			     "#™ffnen                 Enter",
			     " Startet das Programm bzw. ”ffnet die Gruppe",
                             0,
			     ID_OEFFNEN
			   },
			   {
			     "#Schlieáen                Esc",
			     " Schlieát die Programmgruppe",
                             0,
			     ID_SCHLIESSEN
			   },
			   MNU_TRENNER,
			   {
			     "#šber...",
			     " Programminformationen",
                             0,
			     ID_UEBER
			   },
			   MNU_TRENNER,
			   {
			     "#Beenden der Sitzung...",
			     " Parkt die K”pfe der Festplatte",
                             0,
			     ID_SHUTDOWN
			   },
			   {
			     "#Ende                   Alt-X",
			     " Programmende",
                             0,
			     ID_ENDE
			   },
			   MNU_ENDE
			 },
       aitemProgramm[] = {
			   {
			     "#Programm",
			     " Programme einrichten und l”schen",
                             0,
			     PROGRAMM
			   },
			   {
			     "#Hinzufgen...              +",
			     " Richtet ein neues Programm am Ende der Liste ein",
                             0,
			     ID_DAZUFUEGEN
			   },
			   {
			     "#Einfgen...            Einfg",
			     " Fgt ein neues Programm vor dem Auswahlcursor ein",
                             0,
			     ID_EINFUEGEN
			   },
			   {
			     "#Bearbeiten...          Space",
			     " Bearbeitet das aktuelle Programm",
                             0,
			     ID_BEARBEITEN
			   },
			   {
			     "Be#wegen",
			     " Bewegt das aktuelle Programm an eine beliebige Position",
                             0,
			     ID_PRG_BEWEGEN
			   },
			   MNU_TRENNER,
			   {
			     "#L”schen...              Entf",
			     " L”scht das aktuelle Programm",
                             0,
			     ID_LOESCHEN
			   },
			   MNU_TRENNER,
			   {
			     "#Alphabetisch sortieren",
			     " Sortiert die Programme dem Namen nach",
                             0,
			     ID_SORT_ALPHA
			   },
			   MNU_TRENNER,
			   {
			     "#Fenster bewegen",
			     " Bewegt das aktuelle Fenster",
                             0,
			     ID_BEWEGEN
			   },
			   {
			     "Fenster#titel...",
			     " Setzt den Titel des aktuellen Fensters",
                             0,
			     ID_TITEL
			   },
			   MNU_ENDE
			 },
       aitemTermine[]  = {
                           {
                             "#Termine",
                             " Terminkalender, Remember-Liste, Optionen",
                             0,
                             TERMINE
                           },
                           {
                             "#Terminkalender...",
                             " Zeigt alle gespeicherten Termine",
                             0,
                             ID_KALENDER
                           },
                           {
                             "#Remember-Liste...",
                             " Zeigt die Termine von heute und morgen",
                             0,
                             ID_REMEMBER
                           },
                           MNU_TRENNER,
                           {
                             "#Optionen...",
                             " Optionen die Termine betreffend",
                             0,
                             ID_OPTIONS
                           },
                           MNU_ENDE
                         },
       aitemUser[]     = {
                           {
                             "#User",
                             " Paáwort „ndern, Nachrichten schicken und User-Editor",
                             0,
                             USER
                           },
                           {
                             "#Ausloggen",
                             " Loggt aus dem Men aus",
                             0,
                             ID_LOGOUT
                           },
                           MNU_TRENNER,
                           {
                             "#Paáwort „ndern...",
                             " Žndert das Paáwort des aktiven Users",
                             0,
                             ID_PASSWORD
                           },
                           {
                             "#Nachrichten schicken...",
                             " Schickt einem anderen User eine Nachricht",
                             0,
                             ID_SEND_MAIL
                           },
                           MNU_TRENNER,
                           {
                             "#User-Editor...",
                             " Editiert die vorhandenen User",
                             0,
                             ID_USER_EDIT
                           },
                           MNU_ENDE
                         },
       aitemOptionen[] = {
			   {
			     "#Optionen",
			     " Programmeinstellungen",
                             0,
			     OPTIONEN
			   },
			   {
			     "#Farbpalette...",
			     " Wahl der Farbpalette",
                             CTX_BOX_FARBPALETTE,
			     ID_FARBPALETTE
			   },
			   {
			     "#Eigene Farbpalette...",
			     " Definition einer eigenen Farbpalette",
                             CTX_BOX_CUSTOM_FARBEN,
			     ID_CUST_COLORS
			   },
			   {
			     "#Maustempo...",
			     " Wahl des Maustempos",
                             CTX_BOX_MOUSE,
			     ID_MAUSTEMPO
			   },
			   {
			     "#Warnton...",
			     " Wahl der Dauer und der Frequenz des Warntons",
                             CTX_BOX_WARNTON,
			     ID_WARNTON
			   },
			   {
			     "#Bildschirmschoner...",
			     " Wahl der Zeitspanne des Bildschirmschoners",
                             CTX_BOX_SCREEN_SAVER,
			     ID_SCR_SAVER
			   },
			   {
			     "#Sonstiges...",
			     " Uhr und Statuszeile",
                             0,
			     ID_SONSTIGES
			   },
                           {
                             "S#ounds...",
                             " Login- und Logout-Sounds",
                             0,
                             ID_SOUNDS
                           },
                           MNU_TRENNER,
                           {
                             "S#upervisor-Optionen...",
                             " Kontrolle ber die Ausfhrung der Batchdateien",
                             0,
                             ID_SUPER_OPT
                           },
			   MNU_ENDE
			 };

  utl_del_null_event(uhr_zeigen);
  msm_set_mickeys(uiMouseSpeed, uiMouseSpeed * 2);
  mnu_entfernen(HAUPTMENU);
  mnu_neu(HAUPTMENU);
  mnu_neu_window(HAUPTMENU, aitemDatei);
  mnu_neu_window(HAUPTMENU, aitemProgramm);
  mnu_neu_window(HAUPTMENU, aitemTermine);
  mnu_neu_window(HAUPTMENU, aitemUser);
  mnu_neu_window(HAUPTMENU, aitemOptionen);
  mnu_aktiv(HAUPTMENU, ID_SCHLIESSEN, FALSE);
  mnu_aktivieren(HAUPTMENU);
  sts_del_status_line();
  if (ucButtonBar)
    sts_new_status_line(aitemStatusLine);
  show_windows();
  utl_insert_null_event(uhr_zeigen);
  if (!(userAktUser.uiRights & RIGHT_SUPERVISOR))
  {
    mnu_aktiv(HAUPTMENU, ID_USER_EDIT, FALSE);
    mnu_aktiv(HAUPTMENU, ID_SUPER_OPT, FALSE);
  }
  if (!(userAktUser.uiRights & RIGHT_CHANGE))
  {
    mnu_aktiv(HAUPTMENU, PROGRAMM, FALSE);
    sts_aktiv(ID_DAZUFUEGEN, FALSE);
    sts_aktiv(ID_EINFUEGEN, FALSE);
    sts_aktiv(ID_BEARBEITEN, FALSE);
    sts_aktiv(ID_LOESCHEN, FALSE);
  }
  else
  {
    sts_aktiv(ID_DAZUFUEGEN, TRUE);
    sts_aktiv(ID_EINFUEGEN, TRUE);
    sts_aktiv(ID_BEARBEITEN, TRUE);
    sts_aktiv(ID_LOESCHEN, TRUE);
  }
  if (!(userAktUser.uiRights & RIGHT_PWD_CHANGE))
    mnu_aktiv(HAUPTMENU, ID_PASSWORD, FALSE);
  if (!(userAktUser.uiRights & RIGHT_QUIT))
  {
    mnu_aktiv(HAUPTMENU, ID_ENDE, FALSE);
    mnu_aktiv(HAUPTMENU, ID_SHUTDOWN, FALSE);
    sts_aktiv(ID_ENDE, FALSE);
  }
  else
    sts_aktiv(ID_ENDE, TRUE);
}

void show_window (_MNU_MENU* pmenuMenu)
{

  UCHAR ucZaehler;

  win_color(pmenuMenu->winWindow, AKT_PALETTE.ucDlgBorder, AKT_PALETTE.ucDlg,
	    AKT_PALETTE.ucDlgBorder);
  win_aktivieren(pmenuMenu->winWindow);
  win_cls(NULL);
  for (ucZaehler = 0; ucZaehler < pmenuMenu->ucEintraege; ucZaehler++)
    win_ss(NULL, 3, 2 + ucZaehler, pmenuMenu->ameEintrag[ucZaehler].aucName);
  win_sw_a(NULL, 2,
           2 + pmenuMenu->ucAktItem, pmenuMenu->winWindow->uiWidth - 4,
           1, AKT_PALETTE.ucDlgLstBoxActItem);
}

void show_windows (void)
{

  _MNU_MENU *pmenuMenu;

  while (win_get_aktiv() != winDesktop)
    win_verstecken(win_get_aktiv());
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
