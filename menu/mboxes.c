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

static UINT
dlg_aktivieren(WINDOW winWindow)
{
    UINT uiID;
    glb_execute(winWindow);
    win_get_last_button(winWindow, &uiID);
    return uiID;
}

static DLG_ELEMENT*
dlg_add_text_field (WINDOW win, INT iX, INT iY, INT iDispLength, INT iRealLength,
                    CHAR *pcLabel, CHAR *pcInput, CHAR *pcHelpLine, BOOL bPassword)
{
    DLG_ELEMENT *pelement = dlg_init_text_field(iX + strlen(pcLabel), iY, iDispLength, iRealLength, bPassword,
                                                pcInput, pcHelpLine, 0, TRUE, NULL);
    win_add_element(win, dlg_init_label(iX, iY, pcLabel, pelement, 0));
    win_add_element(win, pelement);
    return pelement;
}

static DLG_ELEMENT*
dlg_add_list_box (WINDOW win, INT iX, INT iY, INT iWidth, INT iHeight, INT iLabelX, INT iLabelY,
                  INT iPos, BOOL bBorder, UINT uiID, CHAR *pcLabel, CHAR **ppcFirstLine, CHAR *pcHelpLine)
{
    DLG_ELEMENT *pelement = dlg_init_list_box(iX, iY, iWidth, iHeight, ppcFirstLine,
                                              bBorder, pcHelpLine, uiID, TRUE, NULL);
    win_add_element(win, dlg_init_label(3, 2, pcLabel, pelement, 0));
    win_add_element(win, pelement);
    dlg_list_box_set_values(pelement, iPos);
    return pelement;
}

static INT
dlg_ask_list_box (WINDOW win, UINT uiID)
{
    INT iValue;
    dlg_list_box_query_values(win_get_element(win, uiID), &iValue);
    return iValue;
}

static UCHAR
dlg_ask_push_button (WINDOW win, UINT uiID)
{
    BOOL bValue;
    dlg_push_button_query_values(win_get_element(win, uiID), &bValue);
    return (UCHAR)bValue;
}

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
  winWindow = win_new(18, 8, 43, 8, NULL, 0);
    win_title(winWindow, " Programm oder Gruppe hinzufgen ", TITLE_T_C);
  win_add_element(winWindow, dlg_init_act_button(3, 6, "#Programm", 0,
					     pucProgramm, BOX_PROGRAMM, TRUE, NULL));
  win_add_element(winWindow, dlg_init_act_button(17, 6, "#Gruppe", 0,
					     pucGruppe, BOX_GRUPPE, TRUE, NULL));
  win_add_element(winWindow, dlg_init_act_button(29, 6, "Abbruch", K_ESC,
					     pucAbbruch, BOX_CANCEL, TRUE, NULL));
  win_show(winWindow);
  win_ss(NULL, 3, 2, pucZeile1);
  win_ss(NULL, 3, 3, pucZeile2);
  win_sw_za(NULL, 1, 5, 41, 1, 'Ä', win_get_color(NULL, PAL_COL_BORDER));
  uiReturnVar = dlg_aktivieren(winWindow);
  win_delete(winWindow);
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

  ucShorts = utl_short_cuts(FALSE);
  winWindow = win_new(13, 8, 53, 8, NULL, 0);
    win_title(winWindow, pucTitel, TITLE_T_C);
  dlg_add_text_field(winWindow, 3, 2, 32, 60, "#Name        ", pucName, pucGrpName, FALSE);
  dlg_add_text_field(winWindow, 3, 3, 32, 78, "#Beschreibung", pucBeschreibung, pucHilfe, FALSE);
  win_add_element(winWindow, dlg_init_act_button(3, 6, "OK", K_ENTER,
					     pucOK, BOX_OK, TRUE, NULL));
  win_add_element(winWindow, dlg_init_act_button(11, 6, "Abbruch", K_ESC,
					     pucAbbruch, BOX_CANCEL, TRUE, NULL));
  win_show(winWindow);
  win_sw_za(NULL, 1, 5, 51, 1, 'Ä', win_get_color(NULL, PAL_COL_BORDER));
  do
  {
    switch (dlg_aktivieren(winWindow))
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
      case BOX_CANCEL :
	ucEnde = TRUE;
	break;
    }
  } while (!ucEnde);
  win_delete(winWindow);
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

  ucShorts = utl_short_cuts(FALSE);
  winWindow = win_new(13, 7, 53, 11, NULL, 0);
    win_title(winWindow, pucTitel, TITLE_T_C);
  dlg_add_text_field(winWindow, 3, 2, 32, 60, "#Name        ", pucName, pucPrgrName, FALSE);
  dlg_add_text_field(winWindow, 3, 3, 32, 78, "#Beschreibung", pucBeschreibung, pucHilfe, FALSE);
  dlg_add_text_field(winWindow, 3, 5, 32, 128, "#Verzeichnis ", pucWorkDir, pucVerzeichnis, FALSE);
  dlg_add_text_field(winWindow, 3, 6, 32, 128, "#Aufruf      ", pucDatei, pucHAufruf, FALSE);
  win_add_element(winWindow, dlg_init_act_button(3, 9, "OK", K_ENTER,
					     pucOK, BOX_OK, TRUE, NULL));
  win_add_element(winWindow, dlg_init_act_button(11, 9, "Abbruch", K_ESC,
					     pucAbbruch, BOX_CANCEL, TRUE, NULL));
  win_add_element(winWindow, dlg_init_act_button(24, 9, "#Durchsuchen", 0,
					     pucSearch, BOX_SEARCH, TRUE, NULL));
  win_show(winWindow);
  win_sw_za(NULL, 1, 8, 51, 1, 'Ä', win_get_color(NULL, PAL_COL_BORDER));
  do
  {
    switch (dlg_aktivieren(winWindow))
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
	utl_get_path(aucAktPfad);
	strcpy(aucMaske, "*.*");
	if (box_load_save(" Datei suchen ", aucMaske))
	{
	  if (strstr(aucMaske, ".bat"))
	  {
	    strcpy(pucDatei, "CALL ");
	    strcat(pucDatei, aucMaske);
	  }
	  else
	    strcpy(pucDatei, aucMaske);
	  utl_get_path(pucWorkDir);
	}
	utl_set_path(aucAktPfad);
	break;
      case BOX_CANCEL :
	ucEnde = TRUE;
	break;
    }
  } while (!ucEnde);
  win_delete(winWindow);
  utl_short_cuts(ucShorts);
  return(ucReturnVar);
}

UCHAR titel_box (UCHAR *pucTitel)
{

  WINDOW       winWindow;
  UCHAR        aucTitel[51],
	       ucShorts,
	       ucReturnVar,
	      *pucTitelHelp         = " Titel des Fensters",
	      *pucOK                = " Setzt den neuen Fenstertitel",
	      *pucAbbruch           = " Bel„át den alten Fenstertitel";

  strcpy(aucTitel, pucTitel + 1);
  aucTitel[strlen(aucTitel) - 1] = 0;
  ucShorts = utl_short_cuts(FALSE);
  winWindow = win_new(8, 9, 63, 7, NULL, 0);
  win_title(winWindow, " Fenstertitel „ndern ", TITLE_T_C);
  dlg_add_text_field(winWindow, 3, 2, 48, 48, "#Titel", aucTitel, pucTitelHelp, FALSE);
  win_add_element(winWindow, dlg_init_act_button(3, 5, "OK", K_ENTER,
					     pucOK, BOX_OK, TRUE, NULL));
  win_add_element(winWindow, dlg_init_act_button(11, 5, "Abbruch", K_ESC,
					     pucAbbruch, BOX_CANCEL, TRUE, NULL));
  win_show(winWindow);
  win_sw_za(NULL, 1, 4, 61, 1, 'Ä', win_get_color(NULL, PAL_COL_BORDER));
  switch (dlg_aktivieren(winWindow))
  {
    case BOX_OK :
      strcpy(pucTitel, " ");
      strcat(pucTitel, aucTitel);
      strcat(pucTitel, " ");
      ucReturnVar = TRUE;
      break;
    case BOX_CANCEL :
      ucReturnVar = FALSE;
      break;
  }
  win_delete(winWindow);
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
					DLG_RADIO_END
				      };
  WINDOW       winWindow;
  UCHAR       *pucUhr               = " Angabe, ob die Uhr in der rechten oberen Ecke aktiv ist",
	      *pucLastLine          = " Verwendung der letzten Zeile",
	      *pucOK                = " šbernimmt die eingestellten Werte",
	      *pucAbbruch           = " Bel„át die alten Werte",
	       ucMouse;
  BOOL bValue;
  INT iValue;

  winWindow = win_new(20, 7, 40, 11, NULL, 0);
  win_title(winWindow, " Sonstige Einstellungen ", TITLE_T_C);
  win_add_element(winWindow, dlg_init_push_button(3, 2, "#Uhr in der rechten oberen Ecke",
					      ucUhr, pucUhr, 100, TRUE, NULL));
  win_add_element(winWindow, dlg_init_radio_button(abutButtons, ucButtonBar,
					       pucLastLine, 101, TRUE, NULL));
  win_add_element(winWindow, dlg_init_act_button(3, 9, "OK", K_ENTER,
					     pucOK, BOX_OK, TRUE, NULL));
  win_add_element(winWindow, dlg_init_act_button(11, 9, "Abbruch", K_ESC,
					     pucAbbruch, BOX_CANCEL, TRUE, NULL));
  win_show(winWindow);
  win_ss(NULL, 3, 4, "Letzte Zeile als");
  win_sw_za(NULL, 1, 8, 38, 1, 'Ä', win_get_color(NULL, PAL_COL_BORDER));
  switch (dlg_aktivieren(winWindow))
  {
    case BOX_OK :
      ucUhr = dlg_ask_push_button(winWindow, 100);
      if (!ucUhr)
      {
	ucMouse = msm_cursor_off();
	dsk_sw_z(73, 1, 8, 1, ' ');
	if (ucMouse)
	  msm_cursor_on();
      }
      dlg_radio_button_query_values(win_get_element(winWindow, 101), &iValue);
      ucButtonBar = iValue;
      sts_del_status_line();
      if (ucButtonBar)
	sts_new_status_line(aitemStatusLine);
      break;
  }
  win_delete(winWindow);
}

void supervisor_box (void)
{
  WINDOW       winWindow;
  UCHAR       *pucTaeglich          = " Angabe, ob t„glich die Batchdatei TAG.BAT gestartet werden soll",
              *pucWoechentlich      = " Angabe, ob w”chentlich die Batchdatei WOCHE.BAT gestartet werden soll",
              *pucMonatlich         = " Angabe, ob monatlich die Batchdatei MONAT.BAT gestartet werden soll",
	      *pucOK                = " šbernimmt die eingestellten Werte",
	      *pucAbbruch           = " Bel„át die alten Werte",
	       ucMouse;
  BOOL bValue;

  winWindow = win_new(24, 8, 32, 9, NULL, 0);
  win_title(winWindow, " Supervisor-Optionen ", TITLE_T_C);
  win_add_element(winWindow, dlg_init_push_button(3, 2, "Batchdatei #t„glich",
                                              ucTaeglich, pucTaeglich, 100, TRUE, NULL));
  win_add_element(winWindow, dlg_init_push_button(3, 3, "Batchdatei #w”chentlich",
                                              ucWoechentlich, pucWoechentlich, 101, TRUE, NULL));
  win_add_element(winWindow, dlg_init_push_button(3, 4, "Batchdatei #monatlich",
                                              ucMonatlich, pucMonatlich, 102, TRUE, NULL));
  win_add_element(winWindow, dlg_init_act_button(3, 7, "OK", K_ENTER,
					     pucOK, BOX_OK, TRUE, NULL));
  win_add_element(winWindow, dlg_init_act_button(11, 7, "Abbruch", K_ESC,
					     pucAbbruch, BOX_CANCEL, TRUE, NULL));
  win_show(winWindow);
  win_sw_za(NULL, 1, 6, 30, 1, 'Ä', win_get_color(NULL, PAL_COL_BORDER));
  switch (dlg_aktivieren(winWindow))
  {
    case BOX_OK :
      ucTaeglich = dlg_ask_push_button(winWindow, 100);
      ucWoechentlich = dlg_ask_push_button(winWindow, 101);
      ucMonatlich = dlg_ask_push_button(winWindow, 102);
      break;
  }
  win_delete(winWindow);
}

void user_box (UCHAR *pucName)
{
  WINDOW       winWindow;
  UCHAR        ucShorts,
	      *pucNameHelp          = " Ihr Username",
	      *pucOK                = " Best„tigt den Usernamen";

  strcpy(pucName, pucLeer);
  ucShorts = utl_short_cuts(FALSE);
  winWindow = win_new(23, 9, 33, 7, NULL, 0);
  win_title(winWindow, " Username eingeben ", TITLE_T_C);
  dlg_add_text_field(winWindow, 3, 2, 20, 20, "#Name", pucName, pucNameHelp, FALSE);
  win_add_element(winWindow, dlg_init_act_button(3, 5, "OK", K_ENTER, pucOK, BOX_OK, TRUE, NULL));
  win_show(winWindow);
  win_sw_za(NULL, 1, 4, 31, 1, 'Ä', win_get_color(NULL, PAL_COL_BORDER));
  dlg_aktivieren(winWindow);
  win_delete(winWindow);
  utl_short_cuts(ucShorts);
}

UCHAR password_box (UCHAR *pucPassword, UCHAR *pucTitel)
{
  WINDOW       winWindow;
  UCHAR        ucShorts,
               ucReturnVar,
	      *pucPasswordHelp      = " Passwort",
	      *pucOK                = " Best„tigt das Passwort",
              *pucAbbruch           = " Bricht die Aktion ab";

  strcpy(pucPassword, pucLeer);
  ucShorts = utl_short_cuts(FALSE);
  winWindow = win_new(22, 9, 36, 7, NULL, 0);
  win_title(winWindow, pucTitel, TITLE_T_C);
  dlg_add_text_field(winWindow, 3, 2, 20, 20, "#Paáwort", pucPassword, pucPasswordHelp, TRUE);
  win_add_element(winWindow, dlg_init_act_button(3, 5, "OK", K_ENTER, pucOK, BOX_OK, TRUE, NULL));
  win_add_element(winWindow, dlg_init_act_button(11, 5, "Abbruch", K_ESC, pucAbbruch, BOX_CANCEL, TRUE, NULL));
  win_show(winWindow);
  win_sw_za(NULL, 1, 4, 34, 1, 'Ä', win_get_color(NULL, PAL_COL_BORDER));
  if (dlg_aktivieren(winWindow) == BOX_OK)
    ucReturnVar = TRUE;
  else
    ucReturnVar = FALSE;
  win_delete(winWindow);
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
  winWindow = win_new(15, 5, 49, 14, NULL, 0);
  win_title(winWindow, " User-Editor ", TITLE_T_C);
  dlg_add_list_box(winWindow, 3, 4, 24, 8, 3, 2, uiUserID, TRUE,
                                           100, "#Vorhandene User",
                                           (CHAR**)ppucUser, pucUserHelp);
  win_add_element(winWindow, dlg_init_act_button(32, 2, "OK", K_ENTER,
                                             pucOKHelp, BOX_OK, TRUE, NULL));
  win_add_element(winWindow, dlg_init_act_button(32, 5, "Ž#ndern", 0,
                                             pucEditHelp, BOX_EDIT, TRUE, NULL));
  win_add_element(winWindow, dlg_init_act_button(32, 6, "#L”schen", 0,
                                             pucDeleteHelp, BOX_DELETE, TRUE, NULL));
  win_add_element(winWindow, dlg_init_act_button(32, 7, "#Hinzufgen", 0,
                                             pucAddHelp, BOX_ADD, TRUE, NULL));
  win_add_element(winWindow, dlg_init_act_button(32, 10, "V#erbinden", 0,
                                             pucLinkHelp, BOX_LINK, TRUE, NULL));
  win_add_element(winWindow, dlg_init_act_button(32, 11, "#Kopieren", 0,
                                             pucCopyHelp, BOX_COPY, TRUE, NULL));
  win_show(winWindow);
  win_sw_za(NULL, 29, 1, 1, 12, '³', win_get_color(NULL, PAL_COL_BORDER));
  win_ss(NULL, 32, 9, "Men");
  do
  {
    switch (dlg_aktivieren(winWindow))
    {
      case BOX_OK :
        ucEnde = TRUE;
        break;
      // FIXME: also list box item double-clicked!
      case BOX_EDIT :
        uiUser = dlg_ask_list_box(winWindow, 100);
        read_user_info(uiUser, &userUser);
        if (edit_user(" User ver„ndern ", &userUser))
        {
          write_user_info(uiUser, &userUser);
          if (uiUser == uiUserID)
            read_user(uiUser, &userAktUser);
          for (uiCounter = 0; ppucUser[uiCounter]; uiCounter++)
            utl_free(ppucUser[uiCounter]);
          utl_free(ppucUser);
          user_list(&ppucUser);
          dlg_list_box_new_list(win_get_element(winWindow, 100), ppucUser);
        }
        break;
      case BOX_DELETE :
        uiUser = dlg_ask_list_box(winWindow, 100);
        if (uiUser == uiUserID)
        {
          box_info(BOX_INFO, BOX_OK, pucCantUsrDel, 0);
          break;
        }
        if (box_info(BOX_INFO, BOX_OK | BOX_CANCEL, pucWirklichUsrDel, 0) != BOX_OK)
	  break;
        delete_user(uiUser);
        for (uiCounter = 0; ppucUser[uiCounter]; uiCounter++)
          utl_free(ppucUser[uiCounter]);
        utl_free(ppucUser);
        user_list(&ppucUser);
        if (uiUser == uiUsers)
          uiUser--;
        dlg_list_box_new_list(win_get_element(winWindow, 100), ppucUser);
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
          for (uiCounter = 0; ppucUser[uiCounter]; uiCounter++)
            utl_free(ppucUser[uiCounter]);
          utl_free(ppucUser);
          user_list(&ppucUser);
          dlg_list_box_new_list(win_get_element(winWindow, 100), ppucUser);
          uiEntry = get_next_free();
          set_menu(uiUsers - 1, uiEntry);
          write_entry(uiEntry, &dmenuMenu);
        }
        break;
      case BOX_LINK :
        uiUser = dlg_ask_list_box(winWindow, 100);
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
        uiUser = dlg_ask_list_box(winWindow, 100);
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
  win_delete(winWindow);
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

  ucShorts = utl_short_cuts(FALSE);
  winWindow = win_new(17, 6, 45, 12, NULL, 0);
  win_title(winWindow, pucTitel, TITLE_T_C);
  strcpy(aucName, puserUser->aucName);
  dlg_add_text_field(winWindow, 3, 2, 20, 20, "#Name", aucName,
                                             pucNameHelp, FALSE);
  win_add_element(winWindow, dlg_init_push_button(3, 4,
                                              "User hat #Supervisor-Rechte",
                                              puserUser->uiRights &
                                                RIGHT_SUPERVISOR,
                                              pucSuperHelp, 100, TRUE, NULL));
  win_add_element(winWindow, dlg_init_push_button(3, 5,
                                              "User darf sein #Men „ndern",
                                              puserUser->uiRights &
                                                RIGHT_CHANGE,
                                              pucMenuHelp, 101, TRUE, NULL));
  win_add_element(winWindow, dlg_init_push_button(3, 6,
                                              "User darf sein P#aáwort „ndern",
                                              puserUser->uiRights &
                                                RIGHT_PWD_CHANGE,
                                              pucPwdHelp, 102, TRUE, NULL));
  win_add_element(winWindow, dlg_init_push_button(3, 7,
                                              "User darf SE-Menu #beenden",
                                              puserUser->uiRights &
                                                RIGHT_QUIT,
                                              pucQuitHelp, 103, TRUE, NULL));
  win_add_element(winWindow, dlg_init_act_button(3, 10, "OK", K_ENTER,
                                             pucOKHelp, BOX_OK, TRUE, NULL));
  win_add_element(winWindow,  dlg_init_act_button(11, 10, "Abbruch", K_ESC,
                                             pucAbbruchHelp, BOX_CANCEL, TRUE, NULL));
  win_add_element(winWindow, dlg_init_act_button(24, 10, "#Paáwort „ndern", 0,
                                             pucChangePwdHelp, BOX_CHANGE_PWD, TRUE, NULL));
  win_show(winWindow);
  win_sw_za(NULL, 1, 9, 43, 1, 'Ä', win_get_color(NULL, PAL_COL_BORDER));
  do
  {
    switch (dlg_aktivieren(winWindow))
    {
      case BOX_OK :
        ucEnde = TRUE;
        ucReturnVar = TRUE;
        utl_str_upper(strcpy(puserUser->aucName, aucName));
        puserUser->uiRights = 0;
        if (dlg_ask_push_button(winWindow, 100))
          puserUser->uiRights |= RIGHT_SUPERVISOR;
        if (dlg_ask_push_button(winWindow, 101))
          puserUser->uiRights |= RIGHT_CHANGE;
        if (dlg_ask_push_button(winWindow, 102))
          puserUser->uiRights |= RIGHT_PWD_CHANGE;
        if (dlg_ask_push_button(winWindow, 103))
          puserUser->uiRights |= RIGHT_QUIT;
        break;
      case BOX_CANCEL :
        ucEnde = TRUE;
        break;
      case BOX_CHANGE_PWD :
        if (password_box(aucPassword, " Paáwort „ndern "))
          if (password_box(aucPasswordVerify, " Paáwort verifizieren "))
          {
            if (!strcmp(utl_str_upper(aucPassword),
                        utl_str_upper(aucPasswordVerify)))
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
  win_delete(winWindow);
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

  ucShorts = utl_short_cuts(FALSE);
  winWindow = win_new(17, 3, 46, 18, NULL, 0);
  win_title(winWindow, " Nachricht eingeben ", TITLE_T_C);
  pucBuffer = utl_alloc(32768);
  strcpy(pucBuffer, pucLeer);
  strcpy(pmsgMessage->aucSubject, pucLeer);
  dlg_add_text_field(winWindow, 3, 2, 31, 60, "#Inhalt",
                                             pmsgMessage->aucSubject,
                                             pucSubjectHelp, FALSE);
  win_add_element(winWindow, dlg_init_editor(3, 4, 40, 10, 32768, pucBuffer,
                                         TRUE, TRUE, pucEditHelp, 0, TRUE, NULL));
  win_add_element(winWindow, dlg_init_act_button(3, 16, "OK", K_ENTER,
                                             pucOKHelp, BOX_OK, TRUE, NULL));
  win_add_element(winWindow, dlg_init_act_button(11, 16, "Abbruch", K_ESC,
                                             pucAbbruchHelp, BOX_CANCEL, TRUE, NULL));
  win_show(winWindow);
  win_sw_za(NULL, 1, 15, 44, 1, 'Ä', win_get_color(NULL, PAL_COL_BORDER));
  if (dlg_aktivieren(winWindow) != BOX_OK)
  {
    free(pucBuffer);
    pucBuffer = NULL;
  }
  win_delete(winWindow);
  utl_short_cuts(ucShorts);
  return(pucBuffer);
}

void send_mail (void)
{
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
  winWindow = win_new(26, 5, 28, 15, NULL, 0);
  win_title(winWindow, " Nachricht senden ", TITLE_T_C);
  user_list(&ppucUser);
  dlg_add_list_box(winWindow, 5, 3, 20, 8, 3, 2, 0, TRUE,
                                           100, "Senden #an",
                                           (CHAR**)ppucUser, pucToHelp);
  win_add_element(winWindow, dlg_init_act_button(3, 13, "OK", K_ENTER,
                                             pucOKHelp, BOX_OK, TRUE, NULL));
  win_add_element(winWindow, dlg_init_act_button(11, 13, "Abbruch", K_ESC,
                                             pucAbbruchHelp, BOX_CANCEL, TRUE, NULL));
  win_show(winWindow);
  win_sw_za(NULL, 1, 12, 26, 1, 'Ä', win_get_color(NULL, PAL_COL_BORDER));
  if (dlg_aktivieren(winWindow) == BOX_OK)
  {
    pucMsgText = message_box(&msgMessage);
    if (pucMsgText)
    {
      read_user_info(uiUserID, &userUser);
      strcpy(msgMessage.aucFrom, userUser.aucName);
      strcpy(msgMessage.aucTo,
             ppucUser[dlg_ask_list_box(winWindow, 100)]);
      get_local_date(&dtDate);
      msgMessage.ucDay = dtDate.da_day;
      msgMessage.ucMonth = dtDate.da_mon;
      msgMessage.uiYear = dtDate.da_year;
      write_message(&msgMessage, pucMsgText);
      free(pucMsgText);
    }
  }
  win_delete(winWindow);
  for (uiCounter = 0; ppucUser[uiCounter]; uiCounter++)
    utl_free(ppucUser[uiCounter]);
  utl_free(ppucUser);
  utl_short_cuts(ucShorts);
}

void show_message (UCHAR *pucText, MNU_MESSAGE *pmsgMessage)
{
  WINDOW       winWindow;
  UCHAR        ucShorts,
              *pucEditHelp          = " Der Nachrichtentext",
              *pucOKHelp            = " Beendet das Lesen der Nachricht";

  ucShorts = utl_short_cuts(FALSE);
  winWindow = win_new(3, 3, 73, 20, NULL, 0);
  win_title(winWindow, " Nachricht lesen ", TITLE_T_C);
  win_add_element(winWindow, dlg_init_editor(3, 6, 67, 10,
                                         pmsgMessage->uiLength + 10, pucText, FALSE, TRUE,
                                         pucEditHelp, 0, TRUE, NULL));
  win_add_element(winWindow, dlg_init_act_button(3, 18, "OK", K_ENTER,
                                             pucOKHelp, BOX_OK, TRUE, NULL));
  win_show(winWindow);
  win_ss(NULL, 3, 2, "Von  :");
  win_ss(NULL, 3, 3, "An   :");
  win_ss(NULL, 3, 4, "šber :");
  win_ss(NULL, 10, 2, pmsgMessage->aucFrom);
  win_ss(NULL, 10, 3, pmsgMessage->aucTo);
  win_ss(NULL, 10, 4, pmsgMessage->aucSubject);
  win_sw_za(NULL, 1, 17, 71, 1, 'Ä', win_get_color(NULL, PAL_COL_BORDER));
  dlg_aktivieren(winWindow);
  win_delete(winWindow);
  utl_short_cuts(ucShorts);
}

UINT select_user (CHAR *pcWindowTitle, CHAR *pcListBoxTitle)
{
  WINDOW        winWindow;
  UINT          uiCounter,
                uiReturnVar;
  UCHAR         ucShorts,
              **ppucUser,
               *pucToHelp            = " W„hlen Sie einen User",
               *pucOKHelp            = " Best„tigt den ausgew„hlten User",
               *pucAbbruchHelp       = " Bricht die Aktion ab";

  ucShorts = utl_short_cuts(FALSE);
  winWindow = win_new(26, 5, 28, 15, NULL, 0);
  win_title(winWindow, pcWindowTitle, TITLE_T_C);
  user_list(&ppucUser);
  dlg_add_list_box(winWindow, 5, 3, 20, 8, 3, 2, 0, TRUE,
                                           100, pcListBoxTitle,
                                           (CHAR**)ppucUser, pucToHelp);
  win_add_element(winWindow, dlg_init_act_button(3, 13, "OK", K_ENTER,
                                             pucOKHelp, BOX_OK, TRUE, NULL));
  win_add_element(winWindow, dlg_init_act_button(11, 13, "Abbruch", K_ESC,
                                             pucAbbruchHelp, BOX_CANCEL, TRUE, NULL));
  win_show(winWindow);
  win_sw_za(NULL, 1, 12, 26, 1, 'Ä', win_get_color(NULL, PAL_COL_BORDER));
  if (dlg_aktivieren(winWindow) == BOX_OK)
    uiReturnVar = dlg_ask_list_box(winWindow, 100);
  else
    uiReturnVar = CANCEL;
  win_delete(winWindow);
  for (uiCounter = 0; ppucUser[uiCounter]; uiCounter++)
    utl_free(ppucUser[uiCounter]);
  utl_free(ppucUser);
  utl_short_cuts(ucShorts);
  return(uiReturnVar);
}

void termin_box (void)
{
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
  winWindow = win_new(14, 5, 52, 15, NULL, 0);
  win_title(winWindow, " Terminkalender ", TITLE_T_C);
  ptrmList = termin_list();
  ppcList = list_to_array(ptrmList);
  dlg_add_list_box(winWindow, 5, 3, 45, 8, 3, 2, 0, TRUE,
                                           100, "#Termine",
                                           ppcList, pucListHelp);
  win_add_element(winWindow, dlg_init_act_button(3, 13, "OK", K_ENTER,
                                             pucOKHelp, BOX_OK, TRUE, NULL));
  win_add_element(winWindow, dlg_init_act_button(11, 13, "Ž#ndern", 0,
                                             pucChangeHelp, BOX_CHANGE, TRUE, NULL));
  win_add_element(winWindow, dlg_init_act_button(23, 13, "#Einfgen", 0,
                                             pucAddHelp, BOX_ADD, TRUE, NULL));
  win_add_element(winWindow, dlg_init_act_button(37, 13, "#L”schen", 0,
                                             pucDeleteHelp, BOX_DELETE, TRUE, NULL));
  win_show(winWindow);
  win_sw_za(NULL, 1, 12, 50, 1, 'Ä', win_get_color(NULL, PAL_COL_BORDER));
  do
  {
    switch (dlg_aktivieren(winWindow))
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
          ptrmList = termin_list();
          ppcList = list_to_array(ptrmList);
          dlg_list_box_new_list(win_get_element(winWindow, 100), ppcList);
        }
        break;
      case BOX_CHANGE :
        if (list_length(ptrmList) == 0)
          break;
        uiCounter = 0;
        uiID = dlg_ask_list_box(winWindow, 100);
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
          ptrmList = termin_list();
          ppcList = list_to_array(ptrmList);
          dlg_list_box_new_list(win_get_element(winWindow, 100), ppcList);
        }
        break;
      case BOX_DELETE :
        if (list_length(ptrmList) == 0)
          break;
        uiCounter = 0;
        uiID = dlg_ask_list_box(winWindow, 100);
        for (ptrmNext = ptrmList; uiCounter < uiID;
             ptrmNext = ptrmNext->ptrmNext)
          uiCounter++;
        if (box_info(BOX_INFO, BOX_OK | BOX_CANCEL, pucTerminDel, 0) ==
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
          ptrmList = termin_list();
          ppcList = list_to_array(ptrmList);
          dlg_list_box_new_list(win_get_element(winWindow, 100), ppcList);
        }
        break;
    }
  } while (!ucEnde);
  win_delete(winWindow);
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
  winWindow = win_new(17, 7, 45, 10, NULL, 0);
  if (ucEdit)
    win_title(winWindow, " Termin „ndern ", TITLE_T_C);
  else
    win_title(winWindow, " Termin eingeben ", TITLE_T_C);
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
  dlg_add_text_field(winWindow, 3, 2, 10, 10, "#Datum      ",
                                             acDate, pucDateHelp, FALSE);
  dlg_add_text_field(winWindow, 3, 3, 5, 5, "#Zeit       ",
                                             acTime, pucTimeHelp, FALSE);
  dlg_add_text_field(winWindow, 3, 5, 26, 50, "#Bezeichnung",
                                             acDesc, pucDescHelp, FALSE);
  win_add_element(winWindow, dlg_init_act_button(3, 8, "OK", K_ENTER,
                                             pucOKHelp, BOX_OK, TRUE, NULL));
  win_add_element(winWindow, dlg_init_act_button(11, 8, "Abbruch", K_ESC,
                                             pucAbbruchHelp, BOX_CANCEL, TRUE, NULL));
  win_add_element(winWindow, dlg_init_act_button(24, 8, "#Memo", 0,
                                             pucMemoHelp, BOX_MEMO, TRUE, NULL));
  win_show(winWindow);
  win_sw_za(NULL, 1, 7, 43, 1, 'Ä', win_get_color(NULL, PAL_COL_BORDER));
  do
  {
    switch (dlg_aktivieren(winWindow))
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
      case BOX_CANCEL :
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
  win_delete(winWindow);
  utl_short_cuts(ucShorts);
  return(ucReturnVar);
}

void remember_box (void)
{
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
  if (list_length(ptrmList) == 0)
  {
    box_info(BOX_INFO, BOX_OK, pucNoTermin, 0);
    return;
  }
  ppcList = list_to_array(ptrmList);
  ucShorts = utl_short_cuts(FALSE);
  winWindow = win_new(14, 5, 52, 15, NULL, 0);
  win_title(winWindow, " Remember-Liste ", TITLE_T_C);
  dlg_add_list_box(winWindow, 5, 3, 45, 8, 3, 2, 0, TRUE,
                                           100, "#Termine",
                                           ppcList, pucListHelp);
  win_add_element(winWindow, dlg_init_act_button(3, 13, "OK", K_ENTER,
                                             pucOKHelp, BOX_OK, TRUE, NULL));
  win_add_element(winWindow, dlg_init_act_button(11, 13, "#Memo", 0,
                                             pucMemoHelp, BOX_MEMO, TRUE, NULL));
  win_show(winWindow);
  win_sw_za(NULL, 1, 12, 50, 1, 'Ä', win_get_color(NULL, PAL_COL_BORDER));
  do
  {
    switch (dlg_aktivieren(winWindow))
    {
      case BOX_OK :
        ucEnde = TRUE;
        break;
      case BOX_MEMO :
        if (list_length(ptrmList) == 0)
          break;
        uiCounter = 0;
        uiID = dlg_ask_list_box(winWindow, 100);
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
  win_delete(winWindow);
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
  WINDOW        winWindow;
  UCHAR         ucShorts,
                ucReturnVar          = TRUE,
               *pucEditHelp          = " Memotext",
               *pucOKHelp            = " Best„tigt den Memotext",
               *pucAbbruchHelp       = " Bricht die Aktion ab";

  ucShorts = utl_short_cuts(FALSE);
  winWindow = win_new(12, 3, 56, 19, NULL, 0);
  if (ucEdit)
    win_title(winWindow, " Memo editieren ", TITLE_T_C);
  else
    win_title(winWindow, " Memo ansehen ", TITLE_T_C);
  win_add_element(winWindow, dlg_init_editor(3, 2, 50, 13, 32000, pcBuffer,
                                         ucEdit, TRUE, pucEditHelp, 0, TRUE, NULL));
  win_add_element(winWindow, dlg_init_act_button(3, 17, "OK", K_ENTER,
                                             pucOKHelp, BOX_OK, TRUE, NULL));
  if (ucEdit)
  {
    win_add_element(winWindow, dlg_init_act_button(11, 17, "Abbruch", K_ESC,
                                               pucAbbruchHelp, BOX_CANCEL, TRUE, NULL));
  }
  win_show(winWindow);
  win_sw_za(NULL, 1, 16, 54, 1, 'Ä', win_get_color(NULL, PAL_COL_BORDER));
  if (dlg_aktivieren(winWindow) == BOX_CANCEL)
    ucReturnVar = FALSE;
  win_delete(winWindow);
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
					DLG_RADIO_END
				      };
  WINDOW       winWindow;
  UCHAR       *pucListHelp          = " Wie oft soll die Remember-Liste erscheinen?",
	      *pucRememberHelp      = " Wollen Sie zur rechten Zeit an Ihre Termine erinnert werden?",
	      *pucOK                = " šbernimmt die eingestellten Werte",
	      *pucAbbruch           = " Bel„át die alten Werte";

  winWindow = win_new(25, 6, 30, 12, NULL, 0);
  win_title(winWindow, " Sonstige Einstellungen ", TITLE_T_C);
  win_add_element(winWindow, dlg_init_radio_button(abutButtons, ucRememberList,
                                               pucListHelp, 100, TRUE, NULL));
  win_add_element(winWindow, dlg_init_push_button(3, 7, "#an Termine erinnern",
                                              ucTerminWarning,
                                              pucRememberHelp, 101, TRUE, NULL));
  win_add_element(winWindow, dlg_init_act_button(3, 10, "OK", K_ENTER,
					     pucOK, BOX_OK, TRUE, NULL));
  win_add_element(winWindow, dlg_init_act_button(11, 10, "Abbruch", K_ESC,
					     pucAbbruch, BOX_CANCEL, TRUE, NULL));
  win_show(winWindow);
  win_ss(NULL, 3, 2, "Remember-Liste");
  win_sw_za(NULL, 1, 9, 28, 1, 'Ä', win_get_color(NULL, PAL_COL_BORDER));
  if (dlg_aktivieren(winWindow) == BOX_OK)
  {
      INT iResult;
      dlg_radio_button_query_values(win_get_element(winWindow, 100), &iResult);
      ucRememberList = iResult;
      ucTerminWarning = dlg_ask_push_button(winWindow, 101);
  }
  win_delete(winWindow);
}
