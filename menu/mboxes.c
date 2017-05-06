/***************************************************************************
 ***************************************************************************
 ***                                                                     ***
 ***                             SE-Menu                                 ***
 ***                                                                     ***
 ***                           Dialogboxen                               ***
 ***                                                                     ***
 ***                (c) 1990-92 by Schani Electronics                    ***
 ***                                                                     ***
 ***************************************************************************
 ***************************************************************************/

#define NO_STARTUP
#include <stools.h>
#include <string.h>
#include <stdio.h>
#include <dos.h>
#include "menu.h"

extern UCHAR    *pucLeer;
extern UCHAR    *pucDoNotMatch;
extern UCHAR    *pucWrongDate;
extern UCHAR    *pucWrongTime;
extern UCHAR    *pucTerminDel;
extern UCHAR    *pucNoTermin;
extern UCHAR     ucUhr;
extern UCHAR     ucButtonBar;
extern UCHAR     ucTaeglich;
extern UCHAR     ucWoechentlich;
extern UCHAR     ucMonatlich;
extern UCHAR     ucRememberList;
extern UCHAR     ucTerminWarning;
extern UINT      uiUserID;
extern MNU_USER  userAktUser;
extern UINT      uiUsers;
extern STS_ITEM  aitemStatusLine[];
extern CHAR      acStartVOC[128];
extern CHAR      acLogoutVOC[128];
extern FILE     *pfileMemo;

UCHAR *pucPrgName        = "Sie mssen einen Namen eingeben!",
      *pucInvDir         = "Das Verzeichnis ist falsch!",
      *pucAufruf         = "Sie mssen einen Aufruf eingeben!",
      *pucCantUsrDel     = "Der gerade aktive User kann\n"
                           "nicht gel”scht werden!",
      *pucWirklichUsrDel = "Wollen Sie den User\n"
                           "wirklich l”schen?",
      *pucTheSame        = "Sie k”nnen ein Men nicht zum\n"
                           "selben User kopieren bzw. verbinden!",
      *pucNoMemo         = "Zu diesem Termin gibt es\n"
                           "kein Memo!";

UCHAR get_grp_prg (void)
{

  DLG_ELEMENT *apelementElemente[4];
  WINDOW       winWindow;
  UCHAR       *pucZeile1            = "Programm oder Gruppe",
	      *pucZeile2            = "hinzufgen bzw. einfgen ?",
	       ucShorts,
	      *pucProgramm          = " Fgt ein Programm hinzu bzw. ein",
	      *pucGruppe            = " Fgt eine Gruppe hinzu bzw. ein",
	      *pucAbbruch           = " Bricht die Aktion ab";
  UINT         uiReturnVar;

  ucShorts = utl_short_cuts(FALSE);
  winWindow = win_einrichten(18, 8, 43, 8);
  apelementElemente[0] = dlg_init_act_button(3, 6, "#Programm", 0,
					     BOX_PROGRAMM, pucProgramm);
  apelementElemente[1] = dlg_init_act_button(17, 6, "#Gruppe", 0, BOX_GRUPPE,
					     pucGruppe);
  apelementElemente[2] = dlg_init_act_button(29, 6, "Abbruch", T_ESC,
					     BOX_ABBRUCH, pucAbbruch);
  apelementElemente[3] = NULL;
  win_aktivieren(winWindow);
  win_ss(NULL, 3, 2, pucZeile1);
  win_ss(NULL, 3, 3, pucZeile2);
  win_sw_za(NULL, 1, 5, 41, 1, 'Ä', AKT_PALETTE.ucDlgBorder);
  uiReturnVar = dlg_aktivieren(apelementElemente, NULL, FALSE);
  win_entfernen(winWindow);
  dlg_del_act_button(apelementElemente[0]);
  dlg_del_act_button(apelementElemente[1]);
  dlg_del_act_button(apelementElemente[2]);
  utl_short_cuts(ucShorts);
  return((UCHAR)uiReturnVar);
}

UCHAR gruppen_box (UCHAR *pucTitel, UCHAR *pucName, UCHAR *pucBeschreibung)
{

  UCHAR        ucEnde               = FALSE,
	       ucReturnVar          = FALSE,
	       ucShorts,
	      *pucGrpName           = " Name der Gruppe",
	      *pucHilfe             = " Kurzbeschreibung der Gruppe",
	      *pucOK                = " Best„tigt die eingegebenen Werte",
	      *pucAbbruch           = " Bricht die Aktion ab";
  WINDOW       winWindow;
  DLG_ELEMENT *apelementElemente[5];

  ucShorts = utl_short_cuts(FALSE);
  winWindow = win_einrichten(13, 8, 53, 8);
  win_titel(winWindow, pucTitel, TITEL_O_Z);
  apelementElemente[0] = dlg_init_text_field(3, 2, 32, 60, "#Name        ",
					     pucName, pucGrpName);
  apelementElemente[1] = dlg_init_text_field(3, 3, 32, 78, "#Beschreibung",
					     pucBeschreibung, pucHilfe);
  apelementElemente[2] = dlg_init_act_button(3, 6, "OK", T_RETURN, BOX_OK,
					     pucOK);
  apelementElemente[3] = dlg_init_act_button(11, 6, "Abbruch", T_ESC,
					     BOX_ABBRUCH, pucAbbruch);
  apelementElemente[4] = NULL;
  win_aktivieren(winWindow);
  win_sw_za(NULL, 1, 5, 51, 1, 'Ä', AKT_PALETTE.ucDlgBorder);
  do
  {
    switch (dlg_aktivieren(apelementElemente, NULL, FALSE))
    {
      case BOX_OK :
	if (!(*pucName))
	{
	  box_info(BOX_INFO, BOX_OK, pucPrgName, 0);
	  break;
	}
	ucEnde = TRUE;
	ucReturnVar = TRUE;
	break;
      case BOX_ABBRUCH :
	ucEnde = TRUE;
	break;
    }
  } while (!ucEnde);
  win_entfernen(winWindow);
  dlg_del_text_field(apelementElemente[0]);
  dlg_del_text_field(apelementElemente[1]);
  dlg_del_act_button(apelementElemente[2]);
  dlg_del_act_button(apelementElemente[3]);
  utl_short_cuts(ucShorts);
  return(ucReturnVar);
}

UCHAR programm_box (UCHAR *pucTitel, UCHAR *pucName, UCHAR *pucBeschreibung,
		    UCHAR *pucDatei, UCHAR *pucWorkDir)
{

  UCHAR        ucEnde               = FALSE,
	       ucReturnVar          = FALSE,
	       ucShorts,
	       aucAktPfad[256],
	       aucMaske[256],
	      *pucPrgrName          = " Name des Programmes",
	      *pucHilfe             = " Kurze Beschreibung des Programmes",
	      *pucVerzeichnis       = " Verzeichnis, in dem sich das Programm befindet",
	      *pucHAufruf           = " Aufruf des Programmes (Kommandozeile)",
	      *pucOK                = " Best„tigt die eingegebenen Werte",
	      *pucAbbruch           = " Bricht die Aktion ab",
	      *pucSearch            = " Suche nach dem Programmes mit Hilfe einer Dateiauswahlbox";
  WINDOW       winWindow;
  DLG_ELEMENT *apelementElemente[8];

  ucShorts = utl_short_cuts(FALSE);
  winWindow = win_einrichten(13, 7, 53, 11);
  win_titel(winWindow, pucTitel, TITEL_O_Z);
  apelementElemente[0] = dlg_init_text_field(3, 2, 32, 60, "#Name        ",
					     pucName, pucPrgrName);
  apelementElemente[1] = dlg_init_text_field(3, 3, 32, 78, "#Beschreibung",
					     pucBeschreibung, pucHilfe);
  apelementElemente[2] = dlg_init_text_field(3, 5, 32, 128, "#Verzeichnis ",
					     pucWorkDir, pucVerzeichnis);
  apelementElemente[3] = dlg_init_text_field(3, 6, 32, 128, "#Aufruf      ",
					     pucDatei, pucHAufruf);
  apelementElemente[4] = dlg_init_act_button(3, 9, "OK", T_RETURN, BOX_OK,
					     pucOK);
  apelementElemente[5] = dlg_init_act_button(11, 9, "Abbruch", T_ESC,
					     BOX_ABBRUCH, pucAbbruch);
  apelementElemente[6] = dlg_init_act_button(24, 9, "#Durchsuchen", 0,
					     BOX_SEARCH, pucSearch);
  apelementElemente[7] = NULL;
  win_aktivieren(winWindow);
  win_sw_za(NULL, 1, 8, 51, 1, 'Ä', AKT_PALETTE.ucDlgBorder);
  do
  {
    switch (dlg_aktivieren(apelementElemente, NULL, FALSE))
    {
      case BOX_OK :
	if (!name_gueltig(pucWorkDir) || strchr(pucWorkDir, '*') ||
	    strchr(pucWorkDir, '?'))
	{
	  box_info(BOX_INFO, BOX_OK, pucInvDir, 0);
	  break;
	}
	if (!(*pucDatei))
	{
	  box_info(BOX_INFO, BOX_OK, pucAufruf, 0);
	  break;
	}
	if (!(*pucName))
	{
	  box_info(BOX_INFO, BOX_OK, pucPrgName, 0);
	  break;
	}
	ucEnde = TRUE;
	ucReturnVar = TRUE;
	break;
      case BOX_SEARCH :
	utl_get_pfad(aucAktPfad);
	strcpy(aucMaske, "*.*");
	if (box_laden_speichern(" Datei suchen ", aucMaske))
	{
	  if (strstr(aucMaske, ".bat"))
	  {
	    strcpy(pucDatei, "CALL ");
	    strcat(pucDatei, aucMaske);
	  }
	  else
	    strcpy(pucDatei, aucMaske);
	  utl_get_pfad(pucWorkDir);
	}
	utl_set_pfad(aucAktPfad);
	break;
      case BOX_ABBRUCH :
	ucEnde = TRUE;
	break;
    }
  } while (!ucEnde);
  win_entfernen(winWindow);
  dlg_del_text_field(apelementElemente[0]);
  dlg_del_text_field(apelementElemente[1]);
  dlg_del_text_field(apelementElemente[2]);
  dlg_del_text_field(apelementElemente[3]);
  dlg_del_act_button(apelementElemente[4]);
  dlg_del_act_button(apelementElemente[5]);
  dlg_del_act_button(apelementElemente[6]);
  utl_short_cuts(ucShorts);
  return(ucReturnVar);
}

UCHAR titel_box (UCHAR *pucTitel)
{

  WINDOW       winWindow;
  DLG_ELEMENT *apelementElemente[4];
  UCHAR        aucTitel[51],
	       ucShorts,
	       ucReturnVar,
	      *pucTitelHelp         = " Titel des Fensters",
	      *pucOK                = " Setzt den neuen Fenstertitel",
	      *pucAbbruch           = " Bel„át den alten Fenstertitel";

  strcpy(aucTitel, pucTitel + 1);
  aucTitel[strlen(aucTitel) - 1] = 0;
  ucShorts = utl_short_cuts(FALSE);
  winWindow = win_einrichten(8, 9, 63, 7);
  win_titel(winWindow, " Fenstertitel „ndern ", TITEL_O_Z);
  apelementElemente[0] = dlg_init_text_field(3, 2, 48, 48, "#Titel",
					     aucTitel, pucTitelHelp);
  apelementElemente[1] = dlg_init_act_button(3, 5, "OK", T_RETURN, BOX_OK,
					     pucOK);
  apelementElemente[2] = dlg_init_act_button(11, 5, "Abbruch", T_ESC,
					     BOX_ABBRUCH, pucAbbruch);
  apelementElemente[3] = NULL;
  win_aktivieren(winWindow);
  win_sw_za(NULL, 1, 4, 61, 1, 'Ä', AKT_PALETTE.ucDlgBorder);
  switch (dlg_aktivieren(apelementElemente, NULL, FALSE))
  {
    case BOX_OK :
      strcpy(pucTitel, " ");
      strcat(pucTitel, aucTitel);
      strcat(pucTitel, " ");
      ucReturnVar = TRUE;
      break;
    case BOX_ABBRUCH :
      ucReturnVar = FALSE;
      break;
  }
  win_entfernen(winWindow);
  dlg_del_text_field(apelementElemente[0]);
  dlg_del_act_button(apelementElemente[1]);
  dlg_del_act_button(apelementElemente[2]);
  utl_short_cuts(ucShorts);
  return(ucReturnVar);
}

void sonstiges_box (void)
{

  DLG_BUTTON   abutButtons[]        =
				      {
					{
					  5,
					  5,
					  0,
					  "#Hilfezeile"
					},
					{
					  5,
					  6,
					  0,
					  "#Knopfleiste"
					},
					DLG_RADIO_ENDE
				      };
  DLG_ELEMENT *apelementElemente[5];
  WINDOW       winWindow;
  UCHAR       *pucUhr               = " Angabe, ob die Uhr in der rechten oberen Ecke aktiv ist",
	      *pucLastLine          = " Verwendung der letzten Zeile",
	      *pucOK                = " šbernimmt die eingestellten Werte",
	      *pucAbbruch           = " Bel„át die alten Werte",
	       ucMouse;

  winWindow = win_einrichten(20, 7, 40, 11);
  win_titel(winWindow, " Sonstige Einstellungen ", TITEL_O_Z);
  apelementElemente[0] = dlg_init_push_button(3, 2, "#Uhr in der rechten oberen Ecke",
					      ucUhr, pucUhr);
  apelementElemente[1] = dlg_init_radio_button(abutButtons, ucButtonBar,
					       pucLastLine);
  apelementElemente[2] = dlg_init_act_button(3, 9, "OK", T_RETURN, BOX_OK,
					     pucOK);
  apelementElemente[3] = dlg_init_act_button(11, 9, "Abbruch", T_ESC,
					     BOX_ABBRUCH, pucAbbruch);
  apelementElemente[4] = NULL;
  win_aktivieren(winWindow);
  win_ss(NULL, 3, 4, "Letzte Zeile als");
  win_sw_za(NULL, 1, 8, 38, 1, 'Ä', AKT_PALETTE.ucDlgBorder);
  switch (dlg_aktivieren(apelementElemente, NULL, FALSE))
  {
    case BOX_OK :
      ucUhr = dlg_ask_push_button(apelementElemente[0]);
      if (!ucUhr)
      {
	ucMouse = msm_cursor_off();
	dsk_sw_z(73, 1, 8, 1, T_SPACE);
	if (ucMouse)
	  msm_cursor_on();
      }
      ucButtonBar = dlg_ask_radio_button(apelementElemente[1]);
      sts_del_status_line();
      if (ucButtonBar)
	sts_new_status_line(aitemStatusLine);
      break;
  }
  win_entfernen(winWindow);
  dlg_del_push_button(apelementElemente[0]);
  dlg_del_radio_button(apelementElemente[1]);
  dlg_del_act_button(apelementElemente[2]);
  dlg_del_act_button(apelementElemente[3]);
}

void supervisor_box (void)
{

  DLG_ELEMENT *apelementElemente[6];
  WINDOW       winWindow;
  UCHAR       *pucTaeglich          = " Angabe, ob t„glich die Batchdatei TAG.BAT gestartet werden soll",
              *pucWoechentlich      = " Angabe, ob w”chentlich die Batchdatei WOCHE.BAT gestartet werden soll",
              *pucMonatlich         = " Angabe, ob monatlich die Batchdatei MONAT.BAT gestartet werden soll",
	      *pucOK                = " šbernimmt die eingestellten Werte",
	      *pucAbbruch           = " Bel„át die alten Werte",
	       ucMouse;

  winWindow = win_einrichten(24, 8, 32, 9);
  win_titel(winWindow, " Supervisor-Optionen ", TITEL_O_Z);
  apelementElemente[0] = dlg_init_push_button(3, 2, "Batchdatei #t„glich",
                                              ucTaeglich, pucTaeglich);
  apelementElemente[1] = dlg_init_push_button(3, 3, "Batchdatei #w”chentlich",
                                              ucWoechentlich,
                                              pucWoechentlich);
  apelementElemente[2] = dlg_init_push_button(3, 4, "Batchdatei #monatlich",
                                              ucMonatlich, pucMonatlich);
  apelementElemente[3] = dlg_init_act_button(3, 7, "OK", T_RETURN, BOX_OK,
					     pucOK);
  apelementElemente[4] = dlg_init_act_button(11, 7, "Abbruch", T_ESC,
					     BOX_ABBRUCH, pucAbbruch);
  apelementElemente[5] = NULL;
  win_aktivieren(winWindow);
  win_sw_za(NULL, 1, 6, 30, 1, 'Ä', AKT_PALETTE.ucDlgBorder);
  switch (dlg_aktivieren(apelementElemente, NULL, FALSE))
  {
    case BOX_OK :
      ucTaeglich = dlg_ask_push_button(apelementElemente[0]);
      ucWoechentlich = dlg_ask_push_button(apelementElemente[1]);
      ucMonatlich = dlg_ask_push_button(apelementElemente[2]);
      break;
  }
  win_entfernen(winWindow);
  dlg_del_push_button(apelementElemente[0]);
  dlg_del_push_button(apelementElemente[1]);
  dlg_del_push_button(apelementElemente[2]);
  dlg_del_act_button(apelementElemente[3]);
  dlg_del_act_button(apelementElemente[4]);
}

void user_box (UCHAR *pucName)
{

  WINDOW       winWindow;
  DLG_ELEMENT *apelementElemente[3];
  UCHAR        ucShorts,
	      *pucNameHelp          = " Ihr Username",
	      *pucOK                = " Best„tigt den Usernamen";

  strcpy(pucName, pucLeer);
  ucShorts = utl_short_cuts(FALSE);
  winWindow = win_einrichten(23, 9, 33, 7);
  win_titel(winWindow, " Username eingeben ", TITEL_O_Z);
  apelementElemente[0] = dlg_init_text_field(3, 2, 20, 20, "#Name",
					     pucName, pucNameHelp);
  apelementElemente[1] = dlg_init_act_button(3, 5, "OK", T_RETURN, BOX_OK,
					     pucOK);
  apelementElemente[2] = NULL;
  win_aktivieren(winWindow);
  win_sw_za(NULL, 1, 4, 31, 1, 'Ä', AKT_PALETTE.ucDlgBorder);
  dlg_aktivieren(apelementElemente, NULL, FALSE);
  win_entfernen(winWindow);
  dlg_del_text_field(apelementElemente[0]);
  dlg_del_act_button(apelementElemente[1]);
  utl_short_cuts(ucShorts);
}

UCHAR password_box (UCHAR *pucPassword, UCHAR *pucTitel)
{

  WINDOW       winWindow;
  DLG_ELEMENT *apelementElemente[4];
  UCHAR        ucShorts,
               ucReturnVar,
	      *pucPasswordHelp      = " Passwort",
	      *pucOK                = " Best„tigt das Passwort",
              *pucAbbruch           = " Bricht die Aktion ab";

  strcpy(pucPassword, pucLeer);
  ucShorts = utl_short_cuts(FALSE);
  winWindow = win_einrichten(22, 9, 36, 7);
  win_titel(winWindow, pucTitel, TITEL_O_Z);
  apelementElemente[0] = dlg_init_password(3, 2, 20, 20, "#Paáwort",
			                   pucPassword, pucPasswordHelp);
  apelementElemente[1] = dlg_init_act_button(3, 5, "OK", T_RETURN, BOX_OK,
					     pucOK);
  apelementElemente[2] = dlg_init_act_button(11, 5, "Abbruch", T_ESC,
                                             BOX_ABBRUCH, pucAbbruch);
  apelementElemente[3] = NULL;
  win_aktivieren(winWindow);
  win_sw_za(NULL, 1, 4, 34, 1, 'Ä', AKT_PALETTE.ucDlgBorder);
  if (dlg_aktivieren(apelementElemente, NULL, FALSE) == BOX_OK)
    ucReturnVar = TRUE;
  else
    ucReturnVar = FALSE;
  win_entfernen(winWindow);
  dlg_del_text_field(apelementElemente[0]);
  dlg_del_act_button(apelementElemente[1]);
  dlg_del_act_button(apelementElemente[2]);
  utl_short_cuts(ucShorts);
  return(ucReturnVar);
}

void user_editor (void)
{

  UCHAR         ucShorts,
                ucEnde               = FALSE,
              **ppucUser,
               *pucUserHelp          = " Liste der verfgbaren User",
               *pucOKHelp            = " Best„tigt die Žnderungen",
               *pucEditHelp          = " Ver„ndert den angew„hlten User",
               *pucDeleteHelp        = " L”scht den angew„hlten User",
               *pucAddHelp           = " Fgt einen neuen User hinzu",
               *pucLinkHelp          = " Verbindet das Men eines anderen Users mit dem Men des aktivierten",
               *pucCopyHelp          = " Kopiert das Men eines anderen Users in das Men des aktivierten";
  UINT          uiCounter,
                uiUser,
                uiSource,
                uiEntry;
  WINDOW        winWindow;
  DLG_ELEMENT  *apelementElemente[8];
  MNU_USER      userUser;
  MNU_DISK_MENU dmenuMenu =
                            {
                              0,
                              " Hauptmen ",
                              10,
                              5,
                              ""
                            };

  ucShorts = utl_short_cuts(FALSE);
  user_list(&ppucUser);
  winWindow = win_einrichten(15, 5, 49, 14);
  win_titel(winWindow, " User-Editor ", TITEL_O_Z);
  apelementElemente[0] = dlg_init_list_box(3, 4, 24, 8, 3, 2, uiUserID, TRUE,
                                           BOX_EDIT, "#Vorhandene User",
                                           (CHAR**)ppucUser, pucUserHelp);
  apelementElemente[1] = dlg_init_act_button(32, 2, "OK", T_RETURN, BOX_OK,
                                             pucOKHelp);
  apelementElemente[2] = dlg_init_act_button(32, 5, "Ž#ndern", 0, BOX_EDIT,
                                             pucEditHelp);
  apelementElemente[3] = dlg_init_act_button(32, 6, "#L”schen", 0,
                                             BOX_DELETE, pucDeleteHelp);
  apelementElemente[4] = dlg_init_act_button(32, 7, "#Hinzufgen", 0,
                                             BOX_ADD, pucAddHelp);
  apelementElemente[5] = dlg_init_act_button(32, 10, "V#erbinden", 0,
                                             BOX_LINK, pucLinkHelp);
  apelementElemente[6] = dlg_init_act_button(32, 11, "#Kopieren", 0,
                                             BOX_COPY, pucCopyHelp);
  apelementElemente[7] = NULL;
  win_aktivieren(winWindow);
  win_sw_za(NULL, 29, 1, 1, 12, '³', AKT_PALETTE.ucDlgBorder);
  win_ss(NULL, 32, 9, "Men");
  do
  {
    switch (dlg_aktivieren(apelementElemente, NULL, FALSE))
    {
      case BOX_OK :
        ucEnde = TRUE;
        break;
      case BOX_EDIT :
        uiUser = dlg_ask_list_box(apelementElemente[0]);
        read_user_info(uiUser, &userUser);
        if (edit_user(" User ver„ndern ", &userUser))
        {
          write_user_info(uiUser, &userUser);
          if (uiUser == uiUserID)
            read_user(uiUser, &userAktUser);
          dlg_del_list_box(apelementElemente[0]);
          for (uiCounter = 0; ppucUser[uiCounter]; uiCounter++)
            utl_free(ppucUser[uiCounter]);
          utl_free(ppucUser);
          user_list(&ppucUser);
          apelementElemente[0] = dlg_init_list_box(3, 4, 24, 8, 3, 2,
                                                   uiUser, TRUE, BOX_EDIT,
                                                   "#Vorhandene User",
                                                   (CHAR**)ppucUser,
                                                   pucUserHelp);
        }
        break;
      case BOX_DELETE :
        uiUser = dlg_ask_list_box(apelementElemente[0]);
        if (uiUser == uiUserID)
        {
          box_info(BOX_INFO, BOX_OK, pucCantUsrDel, 0);
          break;
        }
        if (box_info(BOX_INFO, BOX_OK | BOX_ABBRUCH, pucWirklichUsrDel, 0)
	    != BOX_OK)
	  break;
        delete_user(uiUser);
        dlg_del_list_box(apelementElemente[0]);
        for (uiCounter = 0; ppucUser[uiCounter]; uiCounter++)
          utl_free(ppucUser[uiCounter]);
        utl_free(ppucUser);
        user_list(&ppucUser);
        if (uiUser == uiUsers)
          uiUser--;
        apelementElemente[0] = dlg_init_list_box(3, 4, 24, 8, 3, 2, uiUser,
                                                 TRUE, BOX_EDIT,
                                                 "#Vorhandene User",
                                                 (CHAR**)ppucUser,
                                                 pucUserHelp);
        break;
      case BOX_ADD :
        strcpy(userUser.aucName, pucLeer);
        strcpy(userUser.aucPassword, pucLeer);
        encrypt_password(userUser.aucPassword);
        userUser.aucItems[0] = 0xff;
        userUser.uiRights = 0;
        if (edit_user(" User hinzufgen ", &userUser))
        {
          write_user(uiUsers, &userUser);
          dlg_del_list_box(apelementElemente[0]);
          for (uiCounter = 0; ppucUser[uiCounter]; uiCounter++)
            utl_free(ppucUser[uiCounter]);
          utl_free(ppucUser);
          user_list(&ppucUser);
          apelementElemente[0] = dlg_init_list_box(3, 4, 24, 8, 3, 2,
                                                   uiUsers - 1, TRUE,
                                                   BOX_EDIT,
                                                   "#Vorhandene User",
                                                   (CHAR**)ppucUser,
                                                   pucUserHelp);
          uiEntry = get_next_free();
          set_menu(uiUsers - 1, uiEntry);
          write_entry(uiEntry, &dmenuMenu);
        }
        break;
      case BOX_LINK :
        uiUser = dlg_ask_list_box(apelementElemente[0]);
        uiSource = select_user(" Men verbinden ", "Verbinden #von");
        if (uiSource != CANCEL)
        {
          if (uiSource == uiUser)
            box_info(BOX_INFO, BOX_OK, pucTheSame, 0);
          else
          {
            delete_menu(uiUser);
            set_menu(uiUser, get_menu(uiSource));
          }
        }
        break;
      case BOX_COPY :
        uiUser = dlg_ask_list_box(apelementElemente[0]);
        uiSource = select_user(" Men kopieren ", "Kopieren #von");
        if (uiSource != CANCEL)
        {
          if (uiSource == uiUser)
            box_info(BOX_INFO, BOX_OK, pucTheSame, 0);
          else
          {
            delete_menu(uiUser);
            set_menu(uiUser, copy_menu(get_menu(uiSource)));
          }
        }
        break;
    }
  }
  while (!ucEnde);
  win_entfernen(winWindow);
  dlg_del_list_box(apelementElemente[0]);
  dlg_del_act_button(apelementElemente[1]);
  dlg_del_act_button(apelementElemente[2]);
  dlg_del_act_button(apelementElemente[3]);
  dlg_del_act_button(apelementElemente[4]);
  for (uiCounter = 0; ppucUser[uiCounter]; uiCounter++)
    utl_free(ppucUser[uiCounter]);
  utl_free(ppucUser);
  utl_short_cuts(ucShorts);
}

UCHAR edit_user (UCHAR *pucTitel, MNU_USER *puserUser)
{

  WINDOW       winWindow;
  UCHAR        aucName[21],
               aucPassword[21],
               aucPasswordVerify[21],
               ucShorts,
               ucEnde                = FALSE,
               ucReturnVar           = FALSE,
              *pucNameHelp           = " Name des Users",
              *pucSuperHelp          = " Angabe, ob der User das Recht hat, andere User zu ver„ndern",
              *pucMenuHelp           = " Angabe, ob der User das Recht hat, sein Men zu ver„ndern",
              *pucPwdHelp            = " Angabe, ob der User das Recht hat, sein Paáwort zu ver„ndern",
              *pucQuitHelp           = " Angabe, ob der User das Recht hat, SE-Menu zu beenden",
              *pucOKHelp             = " šbernimmt die Žnderungen",
              *pucAbbruchHelp        = " Verwirft die Žnderungen",
              *pucChangePwdHelp      = " Žndert das Paáwort des Users";
  DLG_ELEMENT *apelementElemente[9];

  ucShorts = utl_short_cuts(FALSE);
  winWindow = win_einrichten(17, 6, 45, 12);
  win_titel(winWindow, pucTitel, TITEL_O_Z);
  strcpy(aucName, puserUser->aucName);
  apelementElemente[0] = dlg_init_text_field(3, 2, 20, 20, "#Name", aucName,
                                             pucNameHelp);
  apelementElemente[1] = dlg_init_push_button(3, 4,
                                              "User hat #Supervisor-Rechte",
                                              puserUser->uiRights &
                                                RIGHT_SUPERVISOR,
                                              pucSuperHelp);
  apelementElemente[2] = dlg_init_push_button(3, 5,
                                              "User darf sein #Men „ndern",
                                              puserUser->uiRights &
                                                RIGHT_CHANGE,
                                              pucMenuHelp);
  apelementElemente[3] = dlg_init_push_button(3, 6,
                                              "User darf sein P#aáwort „ndern",
                                              puserUser->uiRights &
                                                RIGHT_PWD_CHANGE,
                                              pucPwdHelp);
  apelementElemente[4] = dlg_init_push_button(3, 7,
                                              "User darf SE-Menu #beenden",
                                              puserUser->uiRights &
                                                RIGHT_QUIT,
                                              pucQuitHelp);
  apelementElemente[5] = dlg_init_act_button(3, 10, "OK", T_RETURN, BOX_OK,
                                             pucOKHelp);
  apelementElemente[6] = dlg_init_act_button(11, 10, "Abbruch", T_ESC,
                                             BOX_ABBRUCH, pucAbbruchHelp);
  apelementElemente[7] = dlg_init_act_button(24, 10, "#Paáwort „ndern", 0,
                                             BOX_CHANGE_PWD,
                                             pucChangePwdHelp);
  apelementElemente[8] = NULL;
  win_aktivieren(winWindow);
  win_sw_za(NULL, 1, 9, 43, 1, 'Ä', AKT_PALETTE.ucDlgBorder);
  do
  {
    switch (dlg_aktivieren(apelementElemente, NULL, FALSE))
    {
      case BOX_OK :
        ucEnde = TRUE;
        ucReturnVar = TRUE;
        utl_str_gross(strcpy(puserUser->aucName, aucName));
        puserUser->uiRights = 0;
        if (dlg_ask_push_button(apelementElemente[1]))
          puserUser->uiRights |= RIGHT_SUPERVISOR;
        if (dlg_ask_push_button(apelementElemente[2]))
          puserUser->uiRights |= RIGHT_CHANGE;
        if (dlg_ask_push_button(apelementElemente[3]))
          puserUser->uiRights |= RIGHT_PWD_CHANGE;
        if (dlg_ask_push_button(apelementElemente[4]))
          puserUser->uiRights |= RIGHT_QUIT;
        break;
      case BOX_ABBRUCH :
        ucEnde = TRUE;
        break;
      case BOX_CHANGE_PWD :
        if (password_box(aucPassword, " Paáwort „ndern "))
          if (password_box(aucPasswordVerify, " Paáwort verifizieren "))
          {
            if (!strcmp(utl_str_gross(aucPassword),
                        utl_str_gross(aucPasswordVerify)))
            {
              strcpy(puserUser->aucPassword, aucPassword);
              encrypt_password(puserUser->aucPassword);
            }
            else
              box_info(BOX_INFO, BOX_OK, pucDoNotMatch, 0);
          }
        break;
    }
  } while (!ucEnde);
  win_entfernen(winWindow);
  dlg_del_text_field(apelementElemente[0]);
  dlg_del_push_button(apelementElemente[1]);
  dlg_del_push_button(apelementElemente[2]);
  dlg_del_push_button(apelementElemente[3]);
  dlg_del_push_button(apelementElemente[4]);
  dlg_del_act_button(apelementElemente[5]);
  dlg_del_act_button(apelementElemente[6]);
  dlg_del_act_button(apelementElemente[7]);
  return(ucReturnVar);
}

UCHAR* message_box (MNU_MESSAGE *pmsgMessage)
{

  WINDOW       winWindow;
  UCHAR        ucShorts,
              *pucSubjectHelp       = " Kurze Beschreibung des Inhalts",
              *pucEditHelp          = " Der Text der Nachricht",
              *pucOKHelp            = " Speichert die Nachricht ab",
              *pucAbbruchHelp       = " Bricht die Aktion ab",
              *pucBuffer;
  DLG_ELEMENT *apelementElemente[5];

  ucShorts = utl_short_cuts(FALSE);
  winWindow = win_einrichten(17, 3, 46, 18);
  win_titel(winWindow, " Nachricht eingeben ", TITEL_O_Z);
  pucBuffer = utl_alloc(32768);
  strcpy(pucBuffer, pucLeer);
  strcpy(pmsgMessage->aucSubject, pucLeer);
  apelementElemente[0] = dlg_init_text_field(3, 2, 31, 60, "#Inhalt",
                                             pmsgMessage->aucSubject,
                                             pucSubjectHelp);
  apelementElemente[1] = dlg_init_editor(3, 4, 40, 10, 32768, pucBuffer,
                                         TRUE, pucEditHelp);
  apelementElemente[2] = dlg_init_act_button(3, 16, "OK", T_RETURN, BOX_OK,
                                             pucOKHelp);
  apelementElemente[3] = dlg_init_act_button(11, 16, "Abbruch", T_ESC,
                                             BOX_ABBRUCH, pucAbbruchHelp);
  apelementElemente[4] = NULL;
  win_aktivieren(winWindow);
  win_sw_za(NULL, 1, 15, 44, 1, 'Ä', AKT_PALETTE.ucDlgBorder);
  if (dlg_aktivieren(apelementElemente, NULL, FALSE) == BOX_OK)
    dlg_ask_editor(apelementElemente[1]);
  else
  {
    free(pucBuffer);
    pucBuffer = NULL;
  }
  win_entfernen(winWindow);
  dlg_del_text_field(apelementElemente[0]);
  dlg_del_editor(apelementElemente[1]);
  dlg_del_act_button(apelementElemente[2]);
  dlg_del_act_button(apelementElemente[3]);
  utl_short_cuts(ucShorts);
  return(pucBuffer);
}

void send_mail (void)
{

  DLG_ELEMENT  *apelementElemente[4];
  WINDOW        winWindow;
  UINT          uiCounter;
  UCHAR         ucShorts,
              **ppucUser,
               *pucMsgText,
               *pucToHelp            = " W„hlen Sie den User, an den Sie die Message schicken wollen",
               *pucOKHelp            = " Best„tigt den ausgew„hlten User",
               *pucAbbruchHelp       = " Bricht die Aktion ab";
  MNU_MESSAGE   msgMessage;
  MNU_USER      userUser;
  struct date   dtDate;

  ucShorts = utl_short_cuts(FALSE);
  winWindow = win_einrichten(26, 5, 28, 15);
  win_titel(winWindow, " Nachricht senden ", TITEL_O_Z);
  user_list(&ppucUser);
  apelementElemente[0] = dlg_init_list_box(5, 3, 20, 8, 3, 2, 0, TRUE,
                                           BOX_OK, "Senden #an",
                                           (CHAR**)ppucUser, pucToHelp);
  apelementElemente[1] = dlg_init_act_button(3, 13, "OK", T_RETURN, BOX_OK,
                                             pucOKHelp);
  apelementElemente[2] = dlg_init_act_button(11, 13, "Abbruch", T_ESC,
                                             BOX_ABBRUCH, pucAbbruchHelp);
  apelementElemente[3] = NULL;
  win_aktivieren(winWindow);
  win_sw_za(NULL, 1, 12, 26, 1, 'Ä', AKT_PALETTE.ucDlgBorder);
  if (dlg_aktivieren(apelementElemente, NULL, FALSE) == BOX_OK)
  {
    pucMsgText = message_box(&msgMessage);
    if (pucMsgText)
    {
      read_user_info(uiUserID, &userUser);
      strcpy(msgMessage.aucFrom, userUser.aucName);
      strcpy(msgMessage.aucTo,
             ppucUser[dlg_ask_list_box(apelementElemente[0])]);
      getdate(&dtDate);
      msgMessage.ucDay = dtDate.da_day;
      msgMessage.ucMonth = dtDate.da_mon;
      msgMessage.uiYear = dtDate.da_year;
      write_message(&msgMessage, pucMsgText);
      free(pucMsgText);
    }
  }
  win_entfernen(winWindow);
  dlg_del_list_box(apelementElemente[0]);
  dlg_del_act_button(apelementElemente[1]);
  dlg_del_act_button(apelementElemente[2]);
  for (uiCounter = 0; ppucUser[uiCounter]; uiCounter++)
    utl_free(ppucUser[uiCounter]);
  utl_free(ppucUser);
  utl_short_cuts(ucShorts);
}

void show_message (UCHAR *pucText, MNU_MESSAGE *pmsgMessage)
{

  DLG_ELEMENT *apelementElemente[3];
  WINDOW       winWindow;
  UCHAR        ucShorts,
              *pucEditHelp          = " Der Nachrichtentext",
              *pucOKHelp            = " Beendet das Lesen der Nachricht";

  ucShorts = utl_short_cuts(FALSE);
  winWindow = win_einrichten(3, 3, 73, 20);
  win_titel(winWindow, " Nachricht lesen ", TITEL_O_Z);
  apelementElemente[0] = dlg_init_editor(3, 6, 67, 10,
                                         pmsgMessage->uiLength + 10, pucText,
                                         FALSE, pucEditHelp);
  apelementElemente[1] = dlg_init_act_button(3, 18, "OK", T_RETURN, BOX_OK,
                                             pucOKHelp);
  apelementElemente[2] = NULL;
  win_aktivieren(winWindow);
  win_ss(NULL, 3, 2, "Von  :");
  win_ss(NULL, 3, 3, "An   :");
  win_ss(NULL, 3, 4, "šber :");
  win_ss(NULL, 10, 2, pmsgMessage->aucFrom);
  win_ss(NULL, 10, 3, pmsgMessage->aucTo);
  win_ss(NULL, 10, 4, pmsgMessage->aucSubject);
  win_sw_za(NULL, 1, 17, 71, 1, 'Ä', AKT_PALETTE.ucDlgBorder);
  dlg_aktivieren(apelementElemente, NULL, FALSE);
  win_entfernen(winWindow);
  dlg_del_editor(apelementElemente[0]);
  dlg_del_act_button(apelementElemente[1]);
  utl_short_cuts(ucShorts);
}

void sounds_box (void)
{

  DLG_ELEMENT *apelementElemente[7];
  WINDOW       winWindow;
  UCHAR        ucShorts,
               ucEnde               = FALSE,
              *pucStartHelp         = " Sound, der beim Login ausgegeben wird",
              *pucLogoutHelp        = " Sound, der beim Logout ausgegeben wird",
              *pucSearchStartHelp   = " L„át Sie nach dem Login-Sound suchen",
              *pucSearchLogoutHelp  = " L„át Sie nach dem Logout-Sound suchen",
              *pucOKHelp            = " Speichert die Žnderungen",
              *pucCancelHelp        = " Bricht ohne Žnderung ab";
  CHAR         acStart[128],
               acLogout[128],
               acDir[128],
               acTemp[128];

  ucShorts = utl_short_cuts(FALSE);
  winWindow = win_einrichten(9, 8, 62, 8);
  win_titel(winWindow, " Sounds festsetzen ", TITEL_O_Z);
  strcpy(acStart, acStartVOC);
  strcpy(acLogout, acLogoutVOC);
  apelementElemente[0] = dlg_init_text_field(3, 2, 35, 127, "Log#in ",
                                             acStart, pucStartHelp);
  apelementElemente[1] = dlg_init_act_button(49, 2, "#Suchen", 0,
                                             BOX_SEARCH_START,
                                             pucSearchStartHelp);
  apelementElemente[2] = dlg_init_text_field(3, 3, 35, 127, "Log#out",
                                             acLogout, pucLogoutHelp);
  apelementElemente[3] = dlg_init_act_button(49, 3, "S#uchen", 0,
                                             BOX_SEARCH_LOGOUT,
                                             pucSearchLogoutHelp);
  apelementElemente[4] = dlg_init_act_button(3, 6, "OK", T_RETURN, BOX_OK,
                                             pucOKHelp);
  apelementElemente[5] = dlg_init_act_button(11, 6, "Abbruch", T_ESC,
                                             BOX_ABBRUCH, pucCancelHelp);
  apelementElemente[6] = NULL;
  win_aktivieren(winWindow);
  win_sw_za(NULL, 1, 5, 60, 1, 'Ä', AKT_PALETTE.ucDlgBorder);
  do
  {
    switch (dlg_aktivieren(apelementElemente, NULL, FALSE))
    {
      case BOX_SEARCH_START :
        utl_get_pfad(acDir);
        strcpy(acTemp, "*.VOC");
        if (box_laden_speichern(" Login-Sound suchen ", acTemp))
        {
          utl_get_pfad(acStart);
          if (acStart[strlen(acStart) - 1] != '\\')
            strcat(acStart, "\\");
          strcat(acStart, acTemp);
        }
        utl_set_pfad(acDir);
        break;
      case BOX_SEARCH_LOGOUT :
        utl_get_pfad(acDir);
        strcpy(acTemp, "*.VOC");
        if (box_laden_speichern(" Logout-Sound suchen ", acTemp))
        {
          utl_get_pfad(acLogout);
          if (acLogout[strlen(acLogout) - 1] != '\\')
            strcat(acLogout, "\\");
          strcat(acLogout, acTemp);
        }
        utl_set_pfad(acDir);
        break;
      case BOX_OK :
        ucEnde = TRUE;
        strcpy(acStartVOC, acStart);
        strcpy(acLogoutVOC, acLogout);
        break;
      case BOX_ABBRUCH :
        ucEnde = TRUE;
        break;
    }
  } while (!ucEnde);
  win_entfernen(winWindow);
  dlg_del_text_field(apelementElemente[0]);
  dlg_del_act_button(apelementElemente[1]);
  dlg_del_text_field(apelementElemente[2]);
  dlg_del_act_button(apelementElemente[3]);
  dlg_del_act_button(apelementElemente[4]);
  dlg_del_act_button(apelementElemente[5]);
  utl_short_cuts(ucShorts);
}

UINT select_user (CHAR *pcWindowTitle, CHAR *pcListBoxTitle)
{

  DLG_ELEMENT  *apelementElemente[4];
  WINDOW        winWindow;
  UINT          uiCounter,
                uiReturnVar;
  UCHAR         ucShorts,
              **ppucUser,
               *pucToHelp            = " W„hlen Sie einen User",
               *pucOKHelp            = " Best„tigt den ausgew„hlten User",
               *pucAbbruchHelp       = " Bricht die Aktion ab";

  ucShorts = utl_short_cuts(FALSE);
  winWindow = win_einrichten(26, 5, 28, 15);
  win_titel(winWindow, pcWindowTitle, TITEL_O_Z);
  user_list(&ppucUser);
  apelementElemente[0] = dlg_init_list_box(5, 3, 20, 8, 3, 2, 0, TRUE,
                                           BOX_OK, pcListBoxTitle,
                                           (CHAR**)ppucUser, pucToHelp);
  apelementElemente[1] = dlg_init_act_button(3, 13, "OK", T_RETURN, BOX_OK,
                                             pucOKHelp);
  apelementElemente[2] = dlg_init_act_button(11, 13, "Abbruch", T_ESC,
                                             BOX_ABBRUCH, pucAbbruchHelp);
  apelementElemente[3] = NULL;
  win_aktivieren(winWindow);
  win_sw_za(NULL, 1, 12, 26, 1, 'Ä', AKT_PALETTE.ucDlgBorder);
  if (dlg_aktivieren(apelementElemente, NULL, FALSE) == BOX_OK)
    uiReturnVar = dlg_ask_list_box(apelementElemente[0]);
  else
    uiReturnVar = CANCEL;
  win_entfernen(winWindow);
  dlg_del_list_box(apelementElemente[0]);
  dlg_del_act_button(apelementElemente[1]);
  dlg_del_act_button(apelementElemente[2]);
  for (uiCounter = 0; ppucUser[uiCounter]; uiCounter++)
    utl_free(ppucUser[uiCounter]);
  utl_free(ppucUser);
  utl_short_cuts(ucShorts);
  return(uiReturnVar);
}

void termin_box (void)
{

  DLG_ELEMENT      *apelementElemente[6];
  WINDOW            winWindow;
  UINT              uiCounter,
                    uiID;
  MNU_TERMIN       *ptrmList,
                   *ptrmNext,
                    trmTermin;
  MNU_DISK_TERMIN   dtrmTermin;
  CHAR            **ppcList;
  UCHAR             ucShorts,
                    ucEnde               = FALSE,
                   *pucListHelp          = " W„hlen Sie einen Termin",
                   *pucOKHelp            = " Best„tigt die Žnderungen",
                   *pucChangeHelp        = " Žndert den angew„hlten Termin",
                   *pucAddHelp           = " Fgt einen neuen Termin hinzu",
                   *pucDeleteHelp        = " L”scht den angew„hlten Termin";

  ucShorts = utl_short_cuts(FALSE);
  winWindow = win_einrichten(14, 5, 52, 15);
  win_titel(winWindow, " Terminkalender ", TITEL_O_Z);
  ptrmList = termin_list();
  ppcList = list_to_array(ptrmList);
  apelementElemente[0] = dlg_init_list_box(5, 3, 45, 8, 3, 2, 0, TRUE,
                                           BOX_CHANGE, "#Termine",
                                           ppcList, pucListHelp);
  apelementElemente[1] = dlg_init_act_button(3, 13, "OK", T_RETURN, BOX_OK,
                                             pucOKHelp);
  apelementElemente[2] = dlg_init_act_button(11, 13, "Ž#ndern", 0,
                                             BOX_CHANGE, pucChangeHelp);
  apelementElemente[3] = dlg_init_act_button(23, 13, "#Einfgen", 0, BOX_ADD,
                                             pucAddHelp);
  apelementElemente[4] = dlg_init_act_button(37, 13, "#L”schen", 0,
                                             BOX_DELETE, pucDeleteHelp);
  apelementElemente[5] = NULL;
  win_aktivieren(winWindow);
  win_sw_za(NULL, 1, 12, 50, 1, 'Ä', AKT_PALETTE.ucDlgBorder);
  do
  {
    switch (dlg_aktivieren(apelementElemente, NULL, FALSE))
    {
      case BOX_OK :
        ucEnde = TRUE;
        break;
      case BOX_ADD :
        if (edit_termin_box(FALSE, &trmTermin))
        {
          memcpy(&(dtrmTermin.dtDate), &(trmTermin.dtDate),
                 sizeof(MNU_DATE));
          strcpy(dtrmTermin.acText, trmTermin.acText);
          dtrmTermin.uiMemoID = trmTermin.uiMemoID;
          dtrmTermin.uiNext = 0;
          add_termin(uiUserID, &dtrmTermin);
          for (uiCounter = 0; ppcList[uiCounter]; uiCounter++)
            utl_free(ppcList[uiCounter]);
          utl_free(ppcList);
          if (ptrmList)
          {
            for (ptrmNext = ptrmList->ptrmNext; ptrmNext;
                 ptrmNext = ptrmNext->ptrmNext)
            {
              utl_free(ptrmList);
              ptrmList = ptrmNext;
            }
            utl_free(ptrmList);
          }
          dlg_del_list_box(apelementElemente[0]);
          ptrmList = termin_list();
          ppcList = list_to_array(ptrmList);
          apelementElemente[0] = dlg_init_list_box(5, 3, 45, 8, 3, 2, 0,
                                                   TRUE, BOX_CHANGE,
                                                   "#Termine", ppcList,
                                                   pucListHelp);
        }
        break;
      case BOX_CHANGE :
        if (list_length(ptrmList) == 0)
          break;
        uiCounter = 0;
        uiID = dlg_ask_list_box(apelementElemente[0]);
        for (ptrmNext = ptrmList; uiCounter < uiID;
             ptrmNext = ptrmNext->ptrmNext)
          uiCounter++;
        if (edit_termin_box(TRUE, ptrmNext))
        {
          read_termin(ptrmNext->uiID, &dtrmTermin);
          memcpy(&(dtrmTermin.dtDate), &(ptrmNext->dtDate),
                 sizeof(MNU_DATE));
          strcpy(dtrmTermin.acText, ptrmNext->acText);
          dtrmTermin.uiMemoID = ptrmNext->uiMemoID;
          write_termin(ptrmNext->uiID, &dtrmTermin);
          for (uiCounter = 0; ppcList[uiCounter]; uiCounter++)
            utl_free(ppcList[uiCounter]);
          utl_free(ppcList);
          if (ptrmList)
          {
            for (ptrmNext = ptrmList->ptrmNext; ptrmNext;
                 ptrmNext = ptrmNext->ptrmNext)
            {
              utl_free(ptrmList);
              ptrmList = ptrmNext;
            }
            utl_free(ptrmList);
          }
          dlg_del_list_box(apelementElemente[0]);
          ptrmList = termin_list();
          ppcList = list_to_array(ptrmList);
          apelementElemente[0] = dlg_init_list_box(5, 3, 45, 8, 3, 2, 0,
                                                   TRUE, BOX_CHANGE,
                                                   "#Termine", ppcList,
                                                   pucListHelp);
        }
        break;
      case BOX_DELETE :
        if (list_length(ptrmList) == 0)
          break;
        uiCounter = 0;
        uiID = dlg_ask_list_box(apelementElemente[0]);
        for (ptrmNext = ptrmList; uiCounter < uiID;
             ptrmNext = ptrmNext->ptrmNext)
          uiCounter++;
        if (box_info(BOX_INFO, BOX_OK | BOX_ABBRUCH, pucTerminDel, 0) ==
              BOX_OK)
        {
          delete_termin(uiUserID, ptrmNext->uiID);
          for (uiCounter = 0; ppcList[uiCounter]; uiCounter++)
            utl_free(ppcList[uiCounter]);
          utl_free(ppcList);
          if (ptrmList)
          {
            for (ptrmNext = ptrmList->ptrmNext; ptrmNext;
                 ptrmNext = ptrmNext->ptrmNext)
            {
              utl_free(ptrmList);
              ptrmList = ptrmNext;
            }
            utl_free(ptrmList);
          }
          dlg_del_list_box(apelementElemente[0]);
          ptrmList = termin_list();
          ppcList = list_to_array(ptrmList);
          apelementElemente[0] = dlg_init_list_box(5, 3, 45, 8, 3, 2, 0,
                                                   TRUE, BOX_CHANGE,
                                                   "#Termine", ppcList,
                                                   pucListHelp);
        }
        break;
    }
  } while (!ucEnde);
  win_entfernen(winWindow);
  dlg_del_list_box(apelementElemente[0]);
  dlg_del_act_button(apelementElemente[1]);
  dlg_del_act_button(apelementElemente[2]);
  dlg_del_act_button(apelementElemente[3]);
  dlg_del_act_button(apelementElemente[4]);
  for (uiCounter = 0; ppcList[uiCounter]; uiCounter++)
    utl_free(ppcList[uiCounter]);
  utl_free(ppcList);
  if (ptrmList)
  {
    for (ptrmNext = ptrmList->ptrmNext; ptrmNext;
         ptrmNext = ptrmNext->ptrmNext)
    {
      utl_free(ptrmList);
      ptrmList = ptrmNext;
    }
    utl_free(ptrmList);
  }
  utl_short_cuts(ucShorts);
}

UCHAR edit_termin_box (UCHAR ucEdit, MNU_TERMIN *ptrmTermin)
{


  DLG_ELEMENT  *apelementElemente[7];
  WINDOW        winWindow;
  CHAR          acDate[11],
                acTime[6],
                acDesc[51],
               *pcMemo,
               *pcBuffer;
  UCHAR         ucShorts,
                ucEnde               = FALSE,
                ucMemo               = FALSE,
                ucReturnVar          = FALSE,
               *pucDateHelp          = " Datum des Termins",
               *pucTimeHelp          = " Zeit des Termins",
               *pucDescHelp          = " Kurzbezeichnung fr den Termin",
               *pucOKHelp            = " Speichert den Termin ab",
               *pucAbbruchHelp       = " Bricht die Aktion ab",
               *pucMemoHelp          = " Editor fr eine l„ngere Bemerkung";

  ucShorts = utl_short_cuts(FALSE);
  pcMemo = utl_alloc(32000);
  if (ptrmTermin->uiMemoID && ucEdit)
  {
    pcBuffer = mem_read(pfileMemo, ptrmTermin->uiMemoID);
    strcpy(pcMemo, pcBuffer);
    utl_free(pcBuffer);
  }
  else
    strcpy(pcMemo, "");
  winWindow = win_einrichten(17, 7, 45, 10);
  if (ucEdit)
    win_titel(winWindow, " Termin „ndern ", TITEL_O_Z);
  else
    win_titel(winWindow, " Termin eingeben ", TITEL_O_Z);
  if (ucEdit)
  {
    date_to_string(&(ptrmTermin->dtDate), acDate);
    time_to_string(&(ptrmTermin->dtDate), acTime);
    strcpy(acDesc, ptrmTermin->acText);
  }
  else
  {
    strcpy(acDate, "TT.MM.JJJJ");
    strcpy(acTime, "HH:MM");
    strcpy(acDesc, pucLeer);
  }
  apelementElemente[0] = dlg_init_text_field(3, 2, 10, 10, "#Datum      ",
                                             acDate, pucDateHelp);
  apelementElemente[1] = dlg_init_text_field(3, 3, 5, 5, "#Zeit       ",
                                             acTime, pucTimeHelp);
  apelementElemente[2] = dlg_init_text_field(3, 5, 26, 50, "#Bezeichnung",
                                             acDesc, pucDescHelp);
  apelementElemente[3] = dlg_init_act_button(3, 8, "OK", T_RETURN, BOX_OK,
                                             pucOKHelp);
  apelementElemente[4] = dlg_init_act_button(11, 8, "Abbruch", T_ESC,
                                             BOX_ABBRUCH, pucAbbruchHelp);
  apelementElemente[5] = dlg_init_act_button(24, 8, "#Memo", 0, BOX_MEMO,
                                             pucMemoHelp);
  apelementElemente[6] = NULL;
  win_aktivieren(winWindow);
  win_sw_za(NULL, 1, 7, 43, 1, 'Ä', AKT_PALETTE.ucDlgBorder);
  do
  {
    switch (dlg_aktivieren(apelementElemente, NULL, FALSE))
    {
      case BOX_OK :
        if (!string_to_date(&(ptrmTermin->dtDate), acDate))
        {
          box_info(BOX_INFO, BOX_OK, pucWrongDate, 0);
          break;
        }
        if (!string_to_time(&(ptrmTermin->dtDate), acTime))
        {
          box_info(BOX_INFO, BOX_OK, pucWrongTime, 0);
          break;
        }
        strcpy(ptrmTermin->acText, acDesc);
        if (ucMemo)
          ptrmTermin->uiMemoID = mem_write(pfileMemo, pcMemo,
                                           strlen(pcMemo) + 1);
        else
          ptrmTermin->uiMemoID = 0;
        ucEnde = TRUE;
        ucReturnVar = TRUE;
        break;
      case BOX_ABBRUCH :
        ucEnde = TRUE;
        break;
      case BOX_MEMO :
        if (ptrmTermin->uiMemoID && ucEdit)
        {
          mem_delete(pfileMemo, ptrmTermin->uiMemoID);
          ptrmTermin->uiMemoID = 0;
        }
        ucMemo = memo_box(pcMemo, TRUE);
        break;
    }
  } while (!ucEnde);
  utl_free(pcMemo);
  win_entfernen(winWindow);
  dlg_del_text_field(apelementElemente[0]);
  dlg_del_text_field(apelementElemente[1]);
  dlg_del_text_field(apelementElemente[2]);
  dlg_del_act_button(apelementElemente[3]);
  dlg_del_act_button(apelementElemente[4]);
  dlg_del_act_button(apelementElemente[5]);
  utl_short_cuts(ucShorts);
  return(ucReturnVar);
}

void remember_box (void)
{

  DLG_ELEMENT      *apelementElemente[4];
  WINDOW            winWindow;
  UINT              uiCounter,
                    uiID;
  MNU_TERMIN       *ptrmList,
                   *ptrmNext,
                    trmTermin;
  MNU_DISK_TERMIN   dtrmTermin;
  CHAR            **ppcList,
                   *pcMemo;
  UCHAR             ucShorts,
                    ucEnde               = FALSE,
                   *pucListHelp          = " Heutige und morgige Termine",
                   *pucOKHelp            = " Best„tigt die Informationen",
                   *pucMemoHelp          = " Zeigt das Memo zum angew„hlten Termin";

  ptrmList = remember_list();
  strcpy(pcMemo, "");
  if (list_length(ptrmList) == 0)
  {
    box_info(BOX_INFO, BOX_OK, pucNoTermin, 0);
    return;
  }
  ppcList = list_to_array(ptrmList);
  ucShorts = utl_short_cuts(FALSE);
  winWindow = win_einrichten(14, 5, 52, 15);
  win_titel(winWindow, " Remember-Liste ", TITEL_O_Z);
  apelementElemente[0] = dlg_init_list_box(5, 3, 45, 8, 3, 2, 0, TRUE,
                                           BOX_MEMO, "#Termine",
                                           ppcList, pucListHelp);
  apelementElemente[1] = dlg_init_act_button(3, 13, "OK", T_RETURN, BOX_OK,
                                             pucOKHelp);
  apelementElemente[2] = dlg_init_act_button(11, 13, "#Memo", 0,
                                             BOX_MEMO, pucMemoHelp);
  apelementElemente[3] = NULL;
  win_aktivieren(winWindow);
  win_sw_za(NULL, 1, 12, 50, 1, 'Ä', AKT_PALETTE.ucDlgBorder);
  do
  {
    switch (dlg_aktivieren(apelementElemente, NULL, FALSE))
    {
      case BOX_OK :
        ucEnde = TRUE;
        break;
      case BOX_MEMO :
        if (list_length(ptrmList) == 0)
          break;
        uiCounter = 0;
        uiID = dlg_ask_list_box(apelementElemente[0]);
        for (ptrmNext = ptrmList; uiCounter < uiID;
             ptrmNext = ptrmNext->ptrmNext)
          uiCounter++;
        if (ptrmNext->uiMemoID)
        {
          pcMemo = mem_read(pfileMemo, ptrmNext->uiMemoID);
          memo_box(pcMemo, FALSE);
          utl_free(pcMemo);
        }
        else
          box_info(BOX_INFO, BOX_OK, pucNoMemo, 0);
        break;
    }
  } while (!ucEnde);
  win_entfernen(winWindow);
  dlg_del_list_box(apelementElemente[0]);
  dlg_del_act_button(apelementElemente[1]);
  dlg_del_act_button(apelementElemente[2]);
  for (uiCounter = 0; ppcList[uiCounter]; uiCounter++)
    utl_free(ppcList[uiCounter]);
  utl_free(ppcList);
  if (ptrmList)
  {
    for (ptrmNext = ptrmList->ptrmNext; ptrmNext;
         ptrmNext = ptrmNext->ptrmNext)
    {
      utl_free(ptrmList);
      ptrmList = ptrmNext;
    }
    utl_free(ptrmList);
  }
  utl_short_cuts(ucShorts);
}

UCHAR memo_box (CHAR *pcBuffer, UCHAR ucEdit)
{

  DLG_ELEMENT  *apelementElemente[4];
  WINDOW        winWindow;
  UCHAR         ucShorts,
                ucReturnVar          = TRUE,
               *pucEditHelp          = " Memotext",
               *pucOKHelp            = " Best„tigt den Memotext",
               *pucAbbruchHelp       = " Bricht die Aktion ab";

  ucShorts = utl_short_cuts(FALSE);
  winWindow = win_einrichten(12, 3, 56, 19);
  if (ucEdit)
    win_titel(winWindow, " Memo editieren ", TITEL_O_Z);
  else
    win_titel(winWindow, " Memo ansehen ", TITEL_O_Z);
  apelementElemente[0] = dlg_init_editor(3, 2, 50, 13, 32000, pcBuffer,
                                         ucEdit, pucEditHelp);
  apelementElemente[1] = dlg_init_act_button(3, 17, "OK", T_RETURN, BOX_OK,
                                             pucOKHelp);
  if (ucEdit)
  {
    apelementElemente[2] = dlg_init_act_button(11, 17, "Abbruch", T_ESC,
                                               BOX_ABBRUCH, pucAbbruchHelp);
    apelementElemente[3] = NULL;
  }
  else
    apelementElemente[2] = NULL;
  win_aktivieren(winWindow);
  win_sw_za(NULL, 1, 16, 54, 1, 'Ä', AKT_PALETTE.ucDlgBorder);
  if (dlg_aktivieren(apelementElemente, NULL, FALSE) == BOX_ABBRUCH)
    ucReturnVar = FALSE;
  dlg_ask_editor(apelementElemente[0]);
  win_entfernen(winWindow);
  dlg_del_editor(apelementElemente[0]);
  dlg_del_act_button(apelementElemente[1]);
  if (ucEdit)
    dlg_del_act_button(apelementElemente[2]);
  utl_short_cuts(ucShorts);
  return(ucReturnVar);
}

void termin_option_box (void)
{

  DLG_BUTTON   abutButtons[]        =
				      {
					{
					  5,
					  3,
					  0,
					  "#nie"
					},
					{
					  5,
					  4,
					  0,
					  "#jeden Tag einmal"
					},
                                        {
                                          5,
                                          5,
                                          0,
                                          "#bei jedem Start"
                                        },
					DLG_RADIO_ENDE
				      };
  DLG_ELEMENT *apelementElemente[5];
  WINDOW       winWindow;
  UCHAR       *pucListHelp          = " Wie oft soll die Remember-Liste erscheinen?",
	      *pucRememberHelp      = " Wollen Sie zur rechten Zeit an Ihre Termine erinnert werden?",
	      *pucOK                = " šbernimmt die eingestellten Werte",
	      *pucAbbruch           = " Bel„át die alten Werte";

  winWindow = win_einrichten(25, 6, 30, 12);
  win_titel(winWindow, " Sonstige Einstellungen ", TITEL_O_Z);
  apelementElemente[0] = dlg_init_radio_button(abutButtons, ucRememberList,
                                               pucListHelp);
  apelementElemente[1] = dlg_init_push_button(3, 7, "#an Termine erinnern",
                                              ucTerminWarning,
                                              pucRememberHelp);
  apelementElemente[2] = dlg_init_act_button(3, 10, "OK", T_RETURN, BOX_OK,
					     pucOK);
  apelementElemente[3] = dlg_init_act_button(11, 10, "Abbruch", T_ESC,
					     BOX_ABBRUCH, pucAbbruch);
  apelementElemente[4] = NULL;
  win_aktivieren(winWindow);
  win_ss(NULL, 3, 2, "Remember-Liste");
  win_sw_za(NULL, 1, 9, 28, 1, 'Ä', AKT_PALETTE.ucDlgBorder);
  if (dlg_aktivieren(apelementElemente, NULL, FALSE) == BOX_OK)
  {
    ucRememberList = dlg_ask_radio_button(apelementElemente[0]);
    ucTerminWarning = dlg_ask_push_button(apelementElemente[1]);
  }
  win_entfernen(winWindow);
  dlg_del_radio_button(apelementElemente[0]);
  dlg_del_push_button(apelementElemente[1]);
  dlg_del_act_button(apelementElemente[2]);
  dlg_del_act_button(apelementElemente[3]);
}
