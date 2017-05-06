#include <stools.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>

#define IN_CLICK_FIELD   ((eventEvent.uiHor > (win_get_aktiv()->ucX + 1)) && \
                         (eventEvent.uiHor < (win_get_aktiv()->ucX + \
                         win_get_aktiv()->ucBreite - 2)) && \
                         (eventEvent.uiVer > (win_get_aktiv()->ucY + 1)) && \
                         (eventEvent.uiVer < (win_get_aktiv()->ucY + \
                         win_get_aktiv()->ucHoehe - 2)))

#define HAUPTMENU             1

#define   DATEI            1000
#define     ID_OEFFNEN     1001
#define     ID_SCHLIESSEN  1002
       /***********************/
#define     ID_UEBER       1003
       /***********************/
#define     ID_ENDE        1004

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

#define   OPTIONEN         1200
#define     ID_FARBPALETTE 1201
#define     ID_MAUSTEMPO   1202
#define     ID_WARNTON     1203

typedef struct
{
  UCHAR aucName[61];
  UCHAR aucDatei[9];
  UCHAR aucHelpLine[79];
} MNU_HAUPT_EINTRAG;

typedef struct
{
  UCHAR aucName[61];
  UCHAR aucDatei[131];
  UCHAR aucWorkDir[131];
  UCHAR aucHelpLine[79];
} MNU_EINTRAG;

typedef struct
{
  UCHAR *pucName;
  UCHAR *pucHelpLine;
} MNU_ZEILE;

void  read_haupt       (void);
void  read_gruppe      (UCHAR*, UCHAR*, UCHAR*);
UCHAR get_longest_name (void);
UCHAR get_longest_grp  (UCHAR*);
void  write_haupt      (UCHAR, UCHAR);
void  write_gruppe     (UCHAR, UCHAR);
void  init_screen      (void);
void  run              (void);
void  load_haupt       (MNU_ZEILE*);
void  load_gruppe      (MNU_ZEILE*);
void  bewegen          (MNU_ZEILE*, MNU_ZEILE**, UCHAR);
UCHAR auswahl          (MNU_ZEILE*, UINT*, UCHAR*, UCHAR);
UCHAR gruppen_box      (UCHAR*, UCHAR*, UCHAR*, UCHAR*);
UCHAR programm_box     (UCHAR*, UCHAR*, UCHAR*, UCHAR*, UCHAR*);
UCHAR zeichen_gueltig  (UCHAR);
UCHAR datei_gueltig    (UCHAR*);
void  ende             (void);
void  write_help       (UCHAR*);
void  del_gruppe       (UCHAR*);
void  haupt_window     (UCHAR, UCHAR);
void  gruppen_window   (UCHAR, UCHAR, UCHAR*);

UCHAR             *pucHauptDatei     = "HAUPT.MNU",
                  *pucTempBatch      = "TEMP.BAT",
                  *pucUeber          = "    S E  -  M e n u  V1.01\n"
                                       "\n"
                                       "(c) 1991 by Schani Electronics",
                  *pucMaxEintraege   = "Die maximale Anzahl der\n"
                                       "EintrÑge ist schon erreicht!",
                  *pucGruppenName    = "Der Name der Gruppendatei\n"
                                       "ist falsch!",
                  *pucInvDir         = "Das Verzeichnis ist falsch!",
                  *pucPrgName        = "Sie mÅssen einen Namen eingeben!",
                  *pucAufruf         = "Sie mÅssen einen Aufruf eingeben!",
                  *pucWirklichGrpDel = "Wollen Sie die Programmgruppe\n"
                                       "wirklich lîschen?",
                  *pucWirklichPrgDel = "Wollen Sie das Programm\n"
                                       "wirklich lîschen?",
                  *pucGrpExtension   = ".GRP",
                  *pucLeer           = "",
                   aucAktGrp[9]      = "",
                   aucWinTitel[63],
                   ucHauptEintraege  = 0,
                   ucGrpOffen        = FALSE,
                   ucGrpEintraege    = 0;
MNU_HAUPT_EINTRAG  ameHauptMenue[20];
MNU_EINTRAG        ameGrpMenue[20];
WINDOW             winHauptWindow    = NULL,
                   winGrpWindow      = NULL;

UCHAR int_zeichen_gueltig (UCHAR ucZeichen)
{

  UCHAR *pucUngueltig = "\\/[]{}:|<>+=,;.\"?*";

  if (ucZeichen <= 32)
    return(FALSE);
  for (; *pucUngueltig; pucUngueltig++)
    if (ucZeichen == *pucUngueltig)
      return(FALSE);
  return(TRUE);
}

UCHAR int_string_gueltig (UCHAR *pucString, UCHAR ucName)
{

  UCHAR  ucPunkt = FALSE,
	*pucPos;

  if (strlen(pucString) > 12)
    return(FALSE);
  if (pucPos = (UCHAR*)strchr(pucString, '.'))
  {
    if ((pucString + strlen(pucString) - 1 - pucPos) > 3)
      return(FALSE);
    if ((pucPos - pucString) > 8)
      return(FALSE);
  }
  while (*pucString)
  {
    if (int_zeichen_gueltig(*pucString))
      pucString++;
    else
      switch (*pucString)
      {
	case '.' :
	  if (ucPunkt)
	    return(FALSE);
	  else
	  {
	    ucPunkt = TRUE;
	    pucString++;
	  }
	  break;
	case '*' :
	case '?' :
	  if (ucName)
	    pucString++;
	  else
	    return(FALSE);
	  break;
	default :
	  return(FALSE);
      }
  }
  return(TRUE);
}

UCHAR name_gueltig (UCHAR *pucName)
{

  UCHAR  ucReturnVar = TRUE,
	 ucName      = TRUE,
	*pucText,
	*pucFirstChar,
	*pucZeiger,
	*pucLastChar;

  if (!(pucText = utl_alloc(strlen(pucName) + 1)))
    return(FALSE);
  pucFirstChar = strcpy(pucText, pucName);
  pucLastChar = (pucFirstChar + strlen(pucFirstChar));
  if (isalpha(*pucFirstChar) && (*(pucFirstChar + 1) == ':'))
    pucFirstChar += 2;
  if (!(*pucFirstChar))
    return(TRUE);
  for (pucZeiger = pucFirstChar; pucZeiger < pucLastChar; pucZeiger++)
    if (*pucZeiger == '\\')
      *pucZeiger = 0;
  for (pucLastChar--; pucLastChar >= pucFirstChar; pucLastChar--)
    if (!(*pucLastChar) || (pucLastChar == pucFirstChar))
    {
      if (*pucLastChar)
	ucReturnVar = (ucReturnVar && int_string_gueltig
					(pucLastChar, ucName));
      else
	ucReturnVar = (ucReturnVar && int_string_gueltig
					(pucLastChar + 1, ucName));
      ucName = FALSE;
    }
  utl_free(pucText);
  return(ucReturnVar);
}

void read_haupt (void)
{

  FILE  *pfileDatei;
  UCHAR  ucWinX       = 8,
         ucWinY       = 5;

  if (!(pfileDatei = fopen(pucHauptDatei, "r")))
    write_haupt(ucWinX, ucWinY);
  else
  {
    fread(&ucBeep, sizeof(UCHAR), 1, pfileDatei);
    fread(&uiBeepDauer, sizeof(UINT), 1, pfileDatei);
    fread(&uiBeepFrequenz, sizeof(UINT), 1, pfileDatei);
    fread(&ucAktPalette, sizeof(UCHAR), 1, pfileDatei);
    fread(&uiMouseSpeed, sizeof(UINT), 1, pfileDatei);
    fread(&uiDoubleClick, sizeof(UINT), 1, pfileDatei);
    fread(&ucHauptEintraege, sizeof(UCHAR), 1, pfileDatei);
    fread(&ucWinX, sizeof(UCHAR), 1, pfileDatei);
    fread(&ucWinY, sizeof(UCHAR), 1, pfileDatei);
    fread(ameHauptMenue, sizeof(MNU_HAUPT_EINTRAG), ucHauptEintraege,
          pfileDatei);
    fclose(pfileDatei);
  }
  haupt_window(ucWinX, ucWinY);
  msm_set_mickeys(uiMouseSpeed, uiMouseSpeed * 2);
}

void read_gruppe (UCHAR *pucName, UCHAR *pucWinX, UCHAR *pucWinY)
{

  FILE  *pfileDatei;
  UCHAR  aucDatei[13];

  strcpy(aucDatei, pucName);
  strcat(aucDatei, pucGrpExtension);
  if (!(pfileDatei = fopen(aucDatei, "r")))
  {
    utl_cls(HELLGRAU);
    vio_set_cursor_pos(1, 1);
    printf("Gruppendatei kann nicht geîffnet werden\n");
    exit(1);
  }
  fread(&ucGrpEintraege, sizeof(UCHAR), 1, pfileDatei);
  fread(pucWinX, sizeof(UCHAR), 1, pfileDatei);
  fread(pucWinY, sizeof(UCHAR), 1, pfileDatei);
  fread(ameGrpMenue, sizeof(MNU_EINTRAG), ucGrpEintraege, pfileDatei);
  fclose(pfileDatei);
  strcpy(aucAktGrp, pucName);
}

UCHAR get_longest_name (void)
{

  UCHAR ucReturnVar = 19,
        ucZaehler;

  for (ucZaehler = 0; ucZaehler < ucHauptEintraege; ucZaehler++)
  {
    if (strlen(ameHauptMenue[ucZaehler].aucName) > ucReturnVar)
      ucReturnVar = strlen(ameHauptMenue[ucZaehler].aucName);
  }
  return(ucReturnVar);
}

UCHAR get_longest_grp (UCHAR *pucName)
{

  UCHAR ucReturnVar,
        ucZaehler;

  ucReturnVar = strlen(pucName) + 8;
  for (ucZaehler = 0; ucZaehler < ucGrpEintraege; ucZaehler++)
  {
    if (strlen(ameGrpMenue[ucZaehler].aucName) > ucReturnVar)
      ucReturnVar = strlen(ameGrpMenue[ucZaehler].aucName);
  }
  return(ucReturnVar);
}

void write_haupt (UCHAR ucWinX, UCHAR ucWinY)
{

  FILE  *pfileDatei;

  if (!(pfileDatei = fopen(pucHauptDatei, "w")))
  {
    utl_cls(HELLGRAU);
    vio_set_cursor_pos(1, 1);
    printf("Hauptdatei kann nicht geîffnet werden\n");
    exit(1);
  }
  fwrite(&ucBeep, sizeof(UCHAR), 1, pfileDatei);
  fwrite(&uiBeepDauer, sizeof(UINT), 1, pfileDatei);
  fwrite(&uiBeepFrequenz, sizeof(UINT), 1, pfileDatei);
  fwrite(&ucAktPalette, sizeof(UCHAR), 1, pfileDatei);
  fwrite(&uiMouseSpeed, sizeof(UINT), 1, pfileDatei);
  fwrite(&uiDoubleClick, sizeof(UINT), 1, pfileDatei);
  fwrite(&ucHauptEintraege, sizeof(UCHAR), 1, pfileDatei);
  fwrite(&ucWinX, sizeof(UCHAR), 1, pfileDatei);
  fwrite(&ucWinY, sizeof(UCHAR), 1, pfileDatei);
  fwrite(ameHauptMenue, sizeof(MNU_HAUPT_EINTRAG), ucHauptEintraege,
         pfileDatei);
  fclose(pfileDatei);
}

void write_gruppe (UCHAR ucWinX, UCHAR ucWinY)
{

  FILE  *pfileDatei;
  UCHAR  aucDatei[13];

  strcpy(aucDatei, aucAktGrp);
  strcat(aucDatei, pucGrpExtension);
  if (!(pfileDatei = fopen(aucDatei, "w")))
  {
    utl_cls(HELLGRAU);
    vio_set_cursor_pos(1, 1);
    printf("Gruppendatei kann nicht geîffnet werden\n");
    exit(1);
  }
  fwrite(&ucGrpEintraege, sizeof(UCHAR), 1, pfileDatei);
  fwrite(&ucWinX, sizeof(UCHAR), 1, pfileDatei);
  fwrite(&ucWinY, sizeof(UCHAR), 1, pfileDatei);
  fwrite(ameGrpMenue, sizeof(MNU_EINTRAG), ucGrpEintraege, pfileDatei);
  fclose(pfileDatei);
}

void init_screen (void)
{

  ITEM aitemDatei[]    = {
                           {
                             "#Datei",
                             " Programmstart, Programminformationen, Programmende",
                             DATEI
                           },
                           {
                             "#ôffnen      Enter",
                             " Startet das Programm bzw. îffnet die Gruppe",
                             ID_OEFFNEN
                           },
                           {
                             "#Schlie·en     Esc",
                             " Schlie·t die Programmgruppe",
                             ID_SCHLIESSEN
                           },
                           MNU_TRENNER,
                           {
                             "#öber...",
                             " Programminformationen",
                             ID_UEBER
                           },
                           MNU_TRENNER,
                           {
                             "#Ende        Alt-X",
                             " Programmende",
                             ID_ENDE
                           },
                           MNU_ENDE
                         },
       aitemProgramm[] = {
                           {
                             "#Programm",
                             " Programme einrichten und lîschen",
                             PROGRAMM
                           },
                           {
                             "#HinzufÅgen...",
                             " Richtet ein neues Programm am Ende der Liste ein",
                             ID_DAZUFUEGEN
                           },
                           {
                             "#EinfÅgen...",
                             " FÅgt ein neues Programm vor dem Auswahlcursor ein",
                             ID_EINFUEGEN
                           },
                           {
                             "#Bearbeiten...",
                             " Bearbeitet das aktuelle Programm",
                             ID_BEARBEITEN
                           },
                           {
                             "Be#wegen",
                             " Bewegt das aktuelle Programm an eine beliebige Position",
                             ID_PRG_BEWEGEN
                           },
                           MNU_TRENNER,
                           {
                             "#Lîschen...",
                             " Lîscht das aktuelle Programm",
                             ID_LOESCHEN
                           },
                           MNU_TRENNER,
                           {
                             "#Alphabetisch sortieren",
                             " Sortiert die Programme dem Namen nach",
                             ID_SORT_ALPHA
                           },
                           MNU_TRENNER,
                           {
                             "#Fenster bewegen",
                             " Bewegt das aktuelle Fenster",
                             ID_BEWEGEN
                           },
                           MNU_ENDE
                         },
       aitemOptionen[] = {
                           {
                             "#Optionen",
                             " Programmeinstellungen",
                             OPTIONEN
                           },
                           {
                             "#Farbpalette...",
                             " Wahl der Farbpalette",
                             ID_FARBPALETTE
                           },
                           {
                             "#Maustempo...",
                             " Wahl des Maustempos",
                             ID_MAUSTEMPO
                           },
                           {
                             "#Warnton...",
                             " Wahl der Dauer und der Frequenz des Warntons",
                             ID_WARNTON
                           },
                           MNU_ENDE
                         };

  mnu_neu(HAUPTMENU);
  mnu_neu_window(HAUPTMENU, aitemDatei);
  mnu_neu_window(HAUPTMENU, aitemProgramm);
  mnu_neu_window(HAUPTMENU, aitemOptionen);
  mnu_aktiv(HAUPTMENU, ID_SCHLIESSEN, FALSE);
  vio_sp_za('±', AKT_PALETTE.ucAppSpace);
  vio_sw_za(1, 25, 80, 1, T_SPACE, AKT_PALETTE.ucHelpLine);
  msm_cursor_on();
  mnu_aktivieren(HAUPTMENU);
  win_aktivieren(winHauptWindow);
}

void run (void)
{

  MNU_ZEILE         amzMenue[21];
  UCHAR             aucName[61],
                    aucHelpLine[79],
                    aucDatei[131],
                    aucWorkDir[131],
                    ucAktItem       = 0,
                    ucSaveItem,
                    ucZaehler,
                    ucWinX,
                    ucWinY,
                    ucEnde,
                   *pucTemp;
  UINT              uiWahl;
  FILE             *pfileDatei;
  WINDOW            winTemp;
  UTL_EVENT         eventEvent;
  MNU_EINTRAG       meGrpPuffer;
  MNU_HAUPT_EINTRAG meHauptPuffer;

  load_haupt(amzMenue);
  do
  {
    if (auswahl(amzMenue, &uiWahl, &ucAktItem, FALSE))
    {
      switch (uiWahl)
      {
        case ID_UEBER :
          box_info(BOX_INFO, BOX_OK, pucUeber);
          break;
        case ID_ENDE :
          ende();
          break;
        case ID_DAZUFUEGEN :
          if (!ucGrpOffen)
          {
            if (ucHauptEintraege < 20)
            {
              strcpy(ameHauptMenue[ucHauptEintraege].aucName, pucLeer);
              strcpy(ameHauptMenue[ucHauptEintraege].aucHelpLine, pucLeer);
              strcpy(ameHauptMenue[ucHauptEintraege].aucDatei, pucLeer);
              if (gruppen_box(" Gruppe hinzufÅgen ",
                              ameHauptMenue[ucHauptEintraege].aucName,
                              ameHauptMenue[ucHauptEintraege].aucHelpLine,
                              ameHauptMenue[ucHauptEintraege].aucDatei))
              {
                ucHauptEintraege++;
                haupt_window(min(winHauptWindow->ucX, 79 -
                                 get_longest_name() + 6),
                             min(winHauptWindow->ucY, 20 -
                                 ucHauptEintraege));
                win_aktivieren(winHauptWindow);
                write_haupt(winHauptWindow->ucX, winHauptWindow->ucY);
                load_haupt(amzMenue);
                ucGrpEintraege = 0;
                strcpy(aucAktGrp, ameHauptMenue[ucHauptEintraege - 1].aucDatei);
                write_gruppe(40, 5);
              }
            }
            else
              box_info(BOX_INFO, BOX_OK, pucMaxEintraege);
          }
          else
          {
            if (ucGrpEintraege < 20)
            {
              strcpy(ameGrpMenue[ucGrpEintraege].aucName, pucLeer);
              strcpy(ameGrpMenue[ucGrpEintraege].aucHelpLine, pucLeer);
              strcpy(ameGrpMenue[ucGrpEintraege].aucDatei, pucLeer);
              strcpy(ameGrpMenue[ucGrpEintraege].aucWorkDir, pucLeer);
              if (programm_box(" Programm hinzufÅgen ",
                               ameGrpMenue[ucGrpEintraege].aucName,
                               ameGrpMenue[ucGrpEintraege].aucHelpLine,
                               ameGrpMenue[ucGrpEintraege].aucDatei,
                               ameGrpMenue[ucGrpEintraege].aucWorkDir))
              {
                ucGrpEintraege++;
                gruppen_window(min(winGrpWindow->ucX, 79 -
                                   get_longest_grp(ameHauptMenue
                                     [ucSaveItem].aucName) + 6),
                               min(winGrpWindow->ucY, 20 -
                                   ucGrpEintraege),
                               ameHauptMenue[ucSaveItem].aucName);
                win_aktivieren(winGrpWindow);
                write_gruppe(winGrpWindow->ucX, winGrpWindow->ucY);
                load_gruppe(amzMenue);
              }
            }
            else
              box_info(BOX_INFO, BOX_OK, pucMaxEintraege);
          }
          break;
        case ID_EINFUEGEN :
          if (!ucGrpOffen)
          {
            if (ucHauptEintraege < 20)
            {
              strcpy(aucName, pucLeer);
              strcpy(aucHelpLine, pucLeer);
              strcpy(aucDatei, pucLeer);
              if (gruppen_box(" Gruppe einfÅgen ", aucName, aucHelpLine,
                              aucDatei))
              {
                memmove(ameHauptMenue + ucAktItem + 1,
                        ameHauptMenue + ucAktItem,
                        sizeof(MNU_HAUPT_EINTRAG) *
                          (ucHauptEintraege - ucAktItem));
                ucHauptEintraege++;
                strcpy(ameHauptMenue[ucAktItem].aucName, aucName);
                strcpy(ameHauptMenue[ucAktItem].aucHelpLine, aucHelpLine);
                strcpy(ameHauptMenue[ucAktItem].aucDatei, aucDatei);
                haupt_window(min(winHauptWindow->ucX, 79 -
                                 get_longest_name() + 6),
                             min(winHauptWindow->ucY, 20 -
                                 ucHauptEintraege));
                win_aktivieren(winHauptWindow);
                write_haupt(winHauptWindow->ucX, winHauptWindow->ucY);
                load_haupt(amzMenue);
                ucGrpEintraege = 0;
                strcpy(aucAktGrp, aucDatei);
                write_gruppe(40, 5);
                write_help(aucHelpLine);
              }
            }
          }
          else
          {
            if (ucGrpEintraege < 20)
            {
              strcpy(aucName, pucLeer);
              strcpy(aucHelpLine, pucLeer);
              strcpy(aucDatei, pucLeer);
              strcpy(aucWorkDir, pucLeer);
              if (programm_box(" Programm einfÅgen ", aucName, aucHelpLine,
                               aucDatei, aucWorkDir))
              {
                memmove(ameGrpMenue + ucAktItem + 1, ameGrpMenue + ucAktItem,
                        sizeof(MNU_EINTRAG) * (ucGrpEintraege - ucAktItem));
                ucGrpEintraege++;
                strcpy(ameGrpMenue[ucAktItem].aucName, aucName);
                strcpy(ameGrpMenue[ucAktItem].aucHelpLine, aucHelpLine);
                strcpy(ameGrpMenue[ucAktItem].aucDatei, aucDatei);
                strcpy(ameGrpMenue[ucAktItem].aucWorkDir, aucWorkDir);
                gruppen_window(min(winGrpWindow->ucX, 79 -
                                   get_longest_grp(ameHauptMenue
                                     [ucSaveItem].aucName) + 6),
                               min(winGrpWindow->ucY, 20 -
                                   ucGrpEintraege),
                               ameHauptMenue[ucSaveItem].aucName);
                win_aktivieren(winGrpWindow);
                write_gruppe(winGrpWindow->ucX, winGrpWindow->ucY);
                load_gruppe(amzMenue);
              }
            }
            else
              box_info(BOX_INFO, BOX_OK, pucMaxEintraege);
          }
          break;
        case ID_BEARBEITEN :
          if (!ucGrpOffen)
          {
            strcpy(aucName, ameHauptMenue[ucAktItem].aucName);
            strcpy(aucHelpLine, ameHauptMenue[ucAktItem].aucHelpLine);
            strcpy(aucDatei, ameHauptMenue[ucAktItem].aucDatei);
            if (gruppen_box(" Gruppe bearbeiten ", aucName, aucHelpLine,
                            aucDatei))
            {
              read_gruppe(ameHauptMenue[ucAktItem].aucDatei, &ucWinX,
                          &ucWinY);
              del_gruppe(ameHauptMenue[ucAktItem].aucDatei);
              strcpy(aucAktGrp, aucDatei);
              write_gruppe(ucWinX, ucWinY);
              strcpy(ameHauptMenue[ucAktItem].aucName, aucName);
              strcpy(ameHauptMenue[ucAktItem].aucHelpLine, aucHelpLine);
              strcpy(ameHauptMenue[ucAktItem].aucDatei, aucDatei);
              haupt_window(min(winHauptWindow->ucX, 79 -
                               get_longest_name() + 6), winHauptWindow->ucY);
              win_aktivieren(winHauptWindow);
              write_haupt(winHauptWindow->ucX, winHauptWindow->ucY);
              load_haupt(amzMenue);
              write_help(aucHelpLine);
            }
          }
          else
          {
            if (ucGrpEintraege < 20)
            {
              strcpy(aucName, ameGrpMenue[ucAktItem].aucName);
              strcpy(aucHelpLine, ameGrpMenue[ucAktItem].aucHelpLine);
              strcpy(aucDatei, ameGrpMenue[ucAktItem].aucDatei);
              strcpy(aucWorkDir, ameGrpMenue[ucAktItem].aucWorkDir);
              if (programm_box(" Programm bearbeiten ", aucName, aucHelpLine,
                               aucDatei, aucWorkDir))
              {
                strcpy(ameGrpMenue[ucAktItem].aucName, aucName);
                strcpy(ameGrpMenue[ucAktItem].aucHelpLine, aucHelpLine);
                strcpy(ameGrpMenue[ucAktItem].aucDatei, aucDatei);
                strcpy(ameGrpMenue[ucAktItem].aucWorkDir, aucWorkDir);
                gruppen_window(min(winGrpWindow->ucX, 79 -
                                   get_longest_grp(ameHauptMenue
                                     [ucSaveItem].aucName) + 6),
                               winGrpWindow->ucY,
                               ameHauptMenue[ucSaveItem].aucName);
                win_aktivieren(winGrpWindow);
                write_gruppe(winGrpWindow->ucX, winGrpWindow->ucY);
                load_gruppe(amzMenue);
              }
            }
          }
          break;
        case ID_PRG_BEWEGEN :
          if (ucGrpOffen && !ucGrpEintraege)
            break;
          if (!ucGrpOffen && !ucHauptEintraege)
            break;
          write_help("Bitte Cursortasten oder Maus zum Bewegen des Programms"
                     " benutzen");
          ucZaehler = ucAktItem;
          auswahl(amzMenue, &uiWahl, &ucAktItem, TRUE);
          if (ucZaehler == ucAktItem)
            break;
          if (!ucGrpOffen)
          {
            memmove(&meHauptPuffer, ameHauptMenue + ucZaehler,
                    sizeof(MNU_HAUPT_EINTRAG));
            if (ucAktItem < ucZaehler)
              memmove(ameHauptMenue + ucAktItem + 1,
                      ameHauptMenue + ucAktItem,
                      sizeof(MNU_HAUPT_EINTRAG) * (ucZaehler - ucAktItem));
            else
              memmove(ameHauptMenue + ucZaehler,
                      ameHauptMenue + ucZaehler + 1,
                      sizeof(MNU_HAUPT_EINTRAG) * (ucAktItem - ucZaehler));
            memmove(ameHauptMenue + ucAktItem, &meHauptPuffer,
                    sizeof(MNU_HAUPT_EINTRAG));
            write_haupt(win_get_aktiv()->ucX, win_get_aktiv()->ucY);
            load_haupt(amzMenue);
          }
          else
          {
            memmove(&meGrpPuffer, ameGrpMenue + ucZaehler,
                    sizeof(MNU_EINTRAG));
            if (ucAktItem < ucZaehler)
              memmove(ameGrpMenue + ucAktItem + 1,
                      ameGrpMenue + ucAktItem,
                      sizeof(MNU_EINTRAG) * (ucZaehler - ucAktItem));
            else
              memmove(ameGrpMenue + ucZaehler,
                      ameGrpMenue + ucZaehler + 1,
                      sizeof(MNU_EINTRAG) * (ucAktItem - ucZaehler));
            memmove(ameGrpMenue + ucAktItem, &meGrpPuffer,
                    sizeof(MNU_EINTRAG));
            write_gruppe(win_get_aktiv()->ucX, win_get_aktiv()->ucY);
            load_gruppe(amzMenue);
          }
          break;
        case ID_SORT_ALPHA :
          if (!ucGrpOffen)
          {
            qsort(ameHauptMenue, ucHauptEintraege,
                  sizeof(MNU_HAUPT_EINTRAG), strcmp);
            write_haupt(win_get_aktiv()->ucX, win_get_aktiv()->ucY);
          }
          else
          {
            qsort(ameGrpMenue, ucGrpEintraege,
                  sizeof(MNU_EINTRAG), strcmp);
            write_gruppe(win_get_aktiv()->ucX, win_get_aktiv()->ucY);
          }
          break;
        case ID_LOESCHEN :
          if (!ucGrpOffen)
          {
            if (ucHauptEintraege)
              if (box_info(BOX_INFO, BOX_OK | BOX_ABBRUCH, pucWirklichGrpDel)
                  == BOX_OK)
              {
                del_gruppe(ameHauptMenue[ucAktItem].aucDatei);
                memmove(ameHauptMenue + ucAktItem,
                        ameHauptMenue + ucAktItem + 1,
                        sizeof(MNU_HAUPT_EINTRAG) *
                          (ucHauptEintraege - 1 - ucAktItem));
                ucHauptEintraege--;
                if (ucAktItem == ucHauptEintraege)
                  ucAktItem--;
                haupt_window(winHauptWindow->ucX, winHauptWindow->ucY);
                win_aktivieren(winHauptWindow);
                write_haupt(winHauptWindow->ucX, winHauptWindow->ucY);
                load_haupt(amzMenue);
              }
          }
          else
          {
            if (ucGrpEintraege)
              if (box_info(BOX_INFO, BOX_OK | BOX_ABBRUCH, pucWirklichPrgDel)
                  == BOX_OK)
              {
                memmove(ameGrpMenue + ucAktItem,
                        ameGrpMenue + ucAktItem + 1,
                        sizeof(MNU_EINTRAG) *
                          (ucGrpEintraege - 1 - ucAktItem));
                ucGrpEintraege--;
                if (ucAktItem == ucGrpEintraege)
                  ucAktItem--;
                gruppen_window(min(winGrpWindow->ucX, 79 -
                                   get_longest_grp(ameHauptMenue
                                     [ucSaveItem].aucName) + 6),
                               min(winGrpWindow->ucY, 23 -
                                   ucHauptEintraege + 4),
                               ameHauptMenue[ucSaveItem].aucName);
                win_aktivieren(winGrpWindow);
                write_gruppe(winGrpWindow->ucX, winGrpWindow->ucY);
                load_gruppe(amzMenue);
              }
          }
          break;
        case ID_BEWEGEN :
          write_help("Bitte benutzen Sie die Cursortasten");
          utl_move_win();
          if (ucGrpOffen)
            write_gruppe(winGrpWindow->ucX, winGrpWindow->ucY);
          else
            write_haupt(winHauptWindow->ucX, winHauptWindow->ucY);
          break;
        case ID_FARBPALETTE :
          box_farbpalette();
          mnu_set_palette();
          if (ucGrpOffen)
            win_verstecken(winGrpWindow);
          win_verstecken(winHauptWindow);
          msm_cursor_off();
          vio_sw_a(1, 2, 80, 23, AKT_PALETTE.ucAppSpace);
          vio_sw_a(1, 25, 80, 1, AKT_PALETTE.ucHelpLine);
          msm_cursor_on();
          if (ucGrpOffen)
            win_color(winGrpWindow, AKT_PALETTE.ucDlg, AKT_PALETTE.ucDlg,
                      AKT_PALETTE.ucDlg);
          win_color(winHauptWindow, AKT_PALETTE.ucDlg, AKT_PALETTE.ucDlg,
                    AKT_PALETTE.ucDlg);
          win_aktivieren(winHauptWindow);
          if (ucGrpOffen)
          {
            win_cls();
            for (ucZaehler = 0; ameHauptMenue[ucZaehler].aucName[0];
                 ucZaehler++)
              win_ss(3, 2 + ucZaehler, ameHauptMenue[ucZaehler].aucName);
            win_sw_a(2, 2 + ucSaveItem, winHauptWindow->ucBreite - 4, 1,
                     AKT_PALETTE.ucDlgLstBoxActItem);
            win_aktivieren(winGrpWindow);
          }
          write_haupt(winHauptWindow->ucX, winHauptWindow->ucY);
          break;
        case ID_MAUSTEMPO :
          box_mouse();
          write_haupt(winHauptWindow->ucX, winHauptWindow->ucY);
          break;
        case ID_WARNTON :
          box_warnton();
          write_haupt(winHauptWindow->ucX, winHauptWindow->ucY);
          break;
      }
    }
    else
    {
      if (!ucGrpOffen)
      {
        if (uiWahl != 64000)
        {
          read_gruppe(ameHauptMenue[ucAktItem].aucDatei, &ucWinX, &ucWinY);
          load_gruppe(amzMenue);
          gruppen_window(ucWinX, ucWinY, ameHauptMenue[ucAktItem].aucName);
          win_aktivieren(winGrpWindow);
          ucSaveItem = ucAktItem;
          ucAktItem = 0;
          mnu_aktiv(HAUPTMENU, ID_SCHLIESSEN, TRUE);
          ucGrpOffen = TRUE;
        }
      }
      else
        if (uiWahl == 64000)
        {
          load_haupt(amzMenue);
          win_entfernen(winGrpWindow);
          winGrpWindow = NULL;
          ucAktItem = ucSaveItem;
          mnu_aktiv(HAUPTMENU, ID_SCHLIESSEN, FALSE);
          ucGrpOffen = FALSE;
        }
        else
        {
          pfileDatei = fopen(pucTempBatch, "w");
          pucTemp = ameGrpMenue[ucAktItem].aucWorkDir;
          if (pucTemp[1] == ':')
          {
            fwrite(pucTemp, 2, 1, pfileDatei);
            fprintf(pfileDatei, "\n");
            pucTemp += 2;
          }
          if (*pucTemp)
            fprintf(pfileDatei, "cd %s\n", pucTemp);
          fprintf(pfileDatei, "%s\n", ameGrpMenue[ucAktItem].aucDatei);
          pucTemp = utl_alloc(131);
          utl_get_pfad(pucTemp);
          fwrite(pucTemp, 2, 1, pfileDatei);
          fprintf(pfileDatei, "\n");
          pucTemp += 2;
          fprintf(pfileDatei, "cd %s\nsemenu\n", pucTemp);
          fclose(pfileDatei);
          if (ucGrpOffen)
            win_verstecken(winGrpWindow);
          win_verstecken(winHauptWindow);
          msm_cursor_off();
          utl_cls(HELLGRAU);
          vio_set_cursor_pos(1, 1);
          exit(1);
        }
    }
  } while (TRUE);
}

void load_haupt (MNU_ZEILE *pmzMenue)
{

  UCHAR ucZaehler;

  for (ucZaehler = 0; ucZaehler < ucHauptEintraege; ucZaehler++)
  {
    pmzMenue->pucName = ameHauptMenue[ucZaehler].aucName;
    pmzMenue->pucHelpLine = ameHauptMenue[ucZaehler].aucHelpLine;
    pmzMenue++;
  }
  pmzMenue->pucName = NULL;
}

void load_gruppe (MNU_ZEILE *pmzMenue)
{

  UCHAR ucZaehler;

  for (ucZaehler = 0; ucZaehler < ucGrpEintraege; ucZaehler++)
  {
    pmzMenue->pucName = ameGrpMenue[ucZaehler].aucName;
    pmzMenue->pucHelpLine = ameGrpMenue[ucZaehler].aucHelpLine;
    pmzMenue++;
  }
  pmzMenue->pucName = NULL;
}

void bewegen (MNU_ZEILE *pmzMenue, MNU_ZEILE **pmzAktZeile, UCHAR ucPos)
{

  MNU_ZEILE  mzPufferZeile,
            *pmzZaehler;

  if (pmzMenue + ucPos == *pmzAktZeile)
  {
    win_sw_a(2, 2 + (*pmzAktZeile - pmzMenue), win_get_aktiv()->ucBreite - 4,
             1, AKT_PALETTE.ucDlgLstBoxActItem);
    return;
  }
  memmove(&mzPufferZeile, *pmzAktZeile, sizeof(MNU_ZEILE));
  if (pmzMenue + ucPos < *pmzAktZeile)
    memmove(pmzMenue + ucPos + 1, pmzMenue + ucPos,
            sizeof(MNU_ZEILE) * (*pmzAktZeile - pmzMenue - ucPos));
  else
    memmove(*pmzAktZeile, *pmzAktZeile + 1,
            sizeof(MNU_ZEILE) * (pmzMenue + ucPos - *pmzAktZeile));
  memmove(pmzMenue + ucPos, &mzPufferZeile, sizeof(MNU_ZEILE));
  *pmzAktZeile = pmzMenue + ucPos;
  win_cls();
  for (pmzZaehler = pmzMenue; pmzZaehler->pucName; pmzZaehler++)
    win_ss(3, 2 + (pmzZaehler - pmzMenue), pmzZaehler->pucName);
  win_sw_a(2, 2 + (*pmzAktZeile - pmzMenue), win_get_aktiv()->ucBreite - 4, 1,
           AKT_PALETTE.ucDlgLstBoxActItem);
}

UCHAR auswahl (MNU_ZEILE *pmzMenue, UINT *puiWahl, UCHAR *pucAktItem,
               UCHAR ucBewegen)
{

  MNU_EVENT  eventEvent;
  MNU_ZEILE *pmzAktZeile,
             mzPufferZeile;
  UCHAR      ucItems,
             ucAngegriffen = FALSE;
  ULONG      ulLastClick   = 0;

  win_cls();
  for (pmzAktZeile = pmzMenue; pmzAktZeile->pucName; pmzAktZeile++)
    win_ss(3, 2 + (pmzAktZeile - pmzMenue), pmzAktZeile->pucName);
  if (pmzMenue->pucName)
  {
    ucItems = TRUE;
    win_sw_a(2, 2 + *pucAktItem, win_get_aktiv()->ucBreite - 4, 1,
             AKT_PALETTE.ucDlgLstBoxActItem);
    pmzAktZeile = pmzMenue + *pucAktItem;
    if (!ucBewegen)
      write_help(pmzAktZeile->pucHelpLine);
  }
  else
  {
    ucItems = FALSE;
    win_sw_a(2, 2, win_get_aktiv()->ucBreite - 4, 1,
             AKT_PALETTE.ucDlgLstBoxActItem);
    write_help(pucLeer);
  }
  do
  {
    if (ucBewegen)
      utl_event((UTL_EVENT*)&eventEvent);
    else
      mnu_event(HAUPTMENU, &eventEvent);
    switch (eventEvent.ucArt)
    {
      case EVENT_WIN_MOVE :
        if (ucGrpOffen)
          write_gruppe(winGrpWindow->ucX, winGrpWindow->ucY);
        else
          write_haupt(winHauptWindow->ucX, winHauptWindow->ucY);
        break;
      case EVENT_L_DOWN :
        if (IN_CLICK_FIELD)
        {
          if (((eventEvent.ulTicks - ulLastClick) <= (uiDoubleClick / 55)) &&
              (pmzAktZeile == (pmzMenue + eventEvent.uiVer -
              win_get_aktiv()->ucY - 2)) && pmzMenue->pucName)
          {
            *puiWahl = 0;
            *pucAktItem = pmzAktZeile - pmzMenue;
            return(FALSE);
          }
          else
          {
            ucAngegriffen = TRUE;
            ulLastClick = eventEvent.ulTicks;
            win_sw_a(2, 2 + (pmzAktZeile - pmzMenue),
                     win_get_aktiv()->ucBreite - 4, 1, AKT_PALETTE.ucDlg);
            if (ucBewegen)
              bewegen(pmzMenue, &pmzAktZeile,
                      eventEvent.uiVer - win_get_aktiv()->ucY - 2);
            else
            {
              pmzAktZeile = pmzMenue + eventEvent.uiVer -
                            win_get_aktiv()->ucY - 2;
              win_sw_a(2, 2 + (pmzAktZeile - pmzMenue),
                       win_get_aktiv()->ucBreite - 4, 1,
                       AKT_PALETTE.ucDlgLstBoxActItem);
              if (pmzMenue->pucName)
                write_help(pmzAktZeile->pucHelpLine);
            }
          }
        }
        break;
      case EVENT_L_UP :
        ucAngegriffen = FALSE;
        break;
      case EVENT_WIN_CLOSE :
        if (win_get_aktiv() != winHauptWindow)
        {
          *puiWahl = 64000;
          *pucAktItem = pmzAktZeile - pmzMenue;
          return(FALSE);
        }
        else
        {
          *puiWahl = ID_ENDE;
          return(TRUE);
        }
        break;
      case EVENT_MSM_MOVE :
        if (ucAngegriffen && IN_CLICK_FIELD)
        {
          if (ucBewegen)
            bewegen(pmzMenue, &pmzAktZeile,
                    eventEvent.uiVer - win_get_aktiv()->ucY - 2);
          else
          {
            win_sw_a(2, 2 + (pmzAktZeile - pmzMenue),
                     win_get_aktiv()->ucBreite - 4, 1, AKT_PALETTE.ucDlg);
            pmzAktZeile = pmzMenue + eventEvent.uiVer -
                          win_get_aktiv()->ucY - 2;
            win_sw_a(2, 2 + (pmzAktZeile - pmzMenue),
                     win_get_aktiv()->ucBreite - 4, 1,
                     AKT_PALETTE.ucDlgLstBoxActItem);
            if (pmzMenue->pucName)
              write_help(pmzAktZeile->pucHelpLine);
          }
        }
        break;
      case EVENT_WAHL :
        switch (eventEvent.uiWahl)
        {
          case ID_OEFFNEN :
            if (pmzMenue->pucName)
            {
              *puiWahl = 0;
              *pucAktItem = pmzAktZeile - pmzMenue;
              return(FALSE);
            }
            break;
          case ID_SCHLIESSEN :
            *puiWahl = 64000;
            *pucAktItem = pmzAktZeile - pmzMenue;
            return(FALSE);
          default :
            *puiWahl = eventEvent.uiWahl;
            *pucAktItem = pmzAktZeile - pmzMenue;
            return(TRUE);
        }
        break;
      case EVENT_TASTE :
        switch (eventEvent.uiTaste)
        {
          case T_ESC :
            if (ucBewegen)
              break;
            if (win_get_aktiv() != winHauptWindow)
            {
              *puiWahl = 64000;
              *pucAktItem = pmzAktZeile - pmzMenue;
              return(FALSE);
            }
            else
            {
              *puiWahl = ID_ENDE;
              return(TRUE);
            }
            break;
          case T_RETURN :
            if (pmzMenue->pucName)
            {
              *puiWahl = 0;
              *pucAktItem = pmzAktZeile - pmzMenue;
              return(FALSE);
            }
            break;
          case T_UP :
            if (pmzAktZeile != pmzMenue)
            {
              if (ucBewegen)
                bewegen(pmzMenue, &pmzAktZeile, pmzAktZeile - pmzMenue - 1);
              else
              {
                win_sw_a(2, 2 + (pmzAktZeile - pmzMenue),
                         win_get_aktiv()->ucBreite - 4, 1,
                         AKT_PALETTE.ucDlg);
                pmzAktZeile--;
                win_sw_a(2, 2 + (pmzAktZeile - pmzMenue),
                         win_get_aktiv()->ucBreite - 4, 1,
                         AKT_PALETTE.ucDlgLstBoxActItem);
                if (pmzMenue->pucName)
                  write_help(pmzAktZeile->pucHelpLine);
              }
            }
            else
              utl_beep();
            break;
          case T_DOWN :
            if ((pmzAktZeile + 1)->pucName && pmzAktZeile->pucName)
            {
              if (ucBewegen)
                bewegen(pmzMenue, &pmzAktZeile, pmzAktZeile - pmzMenue + 1);
              else
              {
                win_sw_a(2, 2 + (pmzAktZeile - pmzMenue),
                         win_get_aktiv()->ucBreite - 4, 1,
                         AKT_PALETTE.ucDlg);
                pmzAktZeile++;
                win_sw_a(2, 2 + (pmzAktZeile - pmzMenue),
                         win_get_aktiv()->ucBreite - 4, 1,
                         AKT_PALETTE.ucDlgLstBoxActItem);
                if (pmzMenue->pucName)
                  write_help(pmzAktZeile->pucHelpLine);
              }
            }
            else
              utl_beep();
            break;
        }
        break;
    }
  } while (TRUE);
}

UCHAR gruppen_box (UCHAR *pucTitel, UCHAR *pucName, UCHAR *pucBeschreibung,
                   UCHAR *pucDatei)
{

  UCHAR        ucEnde               = FALSE,
               ucReturnVar          = FALSE,
               ucShorts;
  WINDOW       winWindow;
  DLG_ELEMENT *apelementElemente[6];

  ucShorts = utl_short_cuts(FALSE);
  winWindow = win_einrichten(13, 7, 53, 10);
  win_titel(winWindow, pucTitel, TITEL_O_Z);
  win_schatten(winWindow, TRUE);
  apelementElemente[0] = dlg_init_text_field(3, 2, 32, 60, "#Name        ",
                                             pucName);
  apelementElemente[1] = dlg_init_text_field(3, 3, 32, 78, "#Beschreibung",
                                             pucBeschreibung);
  apelementElemente[2] = dlg_init_text_field(3, 5, 8, 8, "#Gruppendatei",
                                             pucDatei);
  apelementElemente[3] = dlg_init_act_button(3, 8, "OK", T_RETURN, BOX_OK);
  apelementElemente[4] = dlg_init_act_button(11, 8, "Abbruch", T_ESC,
                                             BOX_ABBRUCH);
  apelementElemente[5] = NULL;
  win_aktivieren(winWindow);
  win_sw_z(1, 7, 51, 1, 'ƒ');
  do
  {
    switch (dlg_aktivieren(apelementElemente))
    {
      case BOX_OK :
        if (!datei_gueltig(pucDatei) || !(*pucDatei))
        {
          box_info(BOX_INFO, BOX_OK, pucGruppenName);
          break;
        }
        if (!(*pucName))
        {
          box_info(BOX_INFO, BOX_OK, pucPrgName);
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
  dlg_del_text_field(apelementElemente[2]);
  dlg_del_act_button(apelementElemente[3]);
  dlg_del_act_button(apelementElemente[4]);
  utl_short_cuts(ucShorts);
  return(ucReturnVar);
}

UCHAR programm_box (UCHAR *pucTitel, UCHAR *pucName, UCHAR *pucBeschreibung,
                    UCHAR *pucDatei, UCHAR *pucWorkDir)
{

  UCHAR        ucEnde               = FALSE,
               ucReturnVar          = FALSE,
               ucShorts;
  WINDOW       winWindow;
  DLG_ELEMENT *apelementElemente[7];

  ucShorts = utl_short_cuts(FALSE);
  winWindow = win_einrichten(13, 7, 53, 11);
  win_titel(winWindow, pucTitel, TITEL_O_Z);
  win_schatten(winWindow, TRUE);
  apelementElemente[0] = dlg_init_text_field(3, 2, 32, 60, "#Name        ",
                                             pucName);
  apelementElemente[1] = dlg_init_text_field(3, 3, 32, 78, "#Beschreibung",
                                             pucBeschreibung);
  apelementElemente[2] = dlg_init_text_field(3, 5, 32, 130, "#Verzeichnis ",
                                             pucWorkDir);
  apelementElemente[3] = dlg_init_text_field(3, 6, 32, 130, "#Aufruf      ",
                                             pucDatei);
  apelementElemente[4] = dlg_init_act_button(3, 9, "OK", T_RETURN, BOX_OK);
  apelementElemente[5] = dlg_init_act_button(11, 9, "Abbruch", T_ESC,
                                             BOX_ABBRUCH);
  apelementElemente[6] = NULL;
  win_aktivieren(winWindow);
  win_sw_z(1, 8, 51, 1, 'ƒ');
  do
  {
    switch (dlg_aktivieren(apelementElemente))
    {
      case BOX_OK :
        if (!name_gueltig(pucWorkDir) || strchr(pucWorkDir, '*') ||
            strchr(pucWorkDir, '?'))
        {
          box_info(BOX_INFO, BOX_OK, pucInvDir);
          break;
        }
        if (!(*pucDatei))
        {
          box_info(BOX_INFO, BOX_OK, pucAufruf);
          break;
        }
        if (!(*pucName))
        {
          box_info(BOX_INFO, BOX_OK, pucPrgName);
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
  dlg_del_text_field(apelementElemente[2]);
  dlg_del_text_field(apelementElemente[3]);
  dlg_del_act_button(apelementElemente[4]);
  dlg_del_act_button(apelementElemente[5]);
  utl_short_cuts(ucShorts);
  return(ucReturnVar);
}

UCHAR zeichen_gueltig (UCHAR ucZeichen)
{

  UCHAR *pucUngueltig = "\\/[]{}:|<>+=,;.\"?*";

  if (ucZeichen <= 32)
    return(FALSE);
  for (; *pucUngueltig; pucUngueltig++)
    if (ucZeichen == *pucUngueltig)
      return(FALSE);
  return(TRUE);
}

UCHAR datei_gueltig (UCHAR *pucString)
{
  if (!strlen(pucString))
    return(FALSE);
  while (*pucString)
  {
    if (zeichen_gueltig(*pucString))
      pucString++;
    else
      return(FALSE);
  }
  return(TRUE);
}

void ende (void)
{
  if (ucGrpOffen)
    win_verstecken(winGrpWindow);
  win_verstecken(winHauptWindow);
  msm_cursor_off();
  utl_cls(HELLGRAU);
  vio_set_cursor_pos(1, 1);
  exit(0);
}

void write_help (UCHAR *pucHelpLine)
{
  msm_cursor_off();
  vio_sw_z(2, 25, 78, 1, T_SPACE);
  vio_ss(2, 25, pucHelpLine);
  msm_cursor_on();
}

void del_gruppe (UCHAR *pucName)
{

  UCHAR aucName[13];

  strcpy(aucName, pucName);
  strcat(aucName, pucGrpExtension);
  remove(aucName);
}

void haupt_window (UCHAR ucWinX, UCHAR ucWinY)
{
  if (winHauptWindow)
    win_entfernen(winHauptWindow);
  if (ucHauptEintraege)
    winHauptWindow = win_einrichten(ucWinX , ucWinY, get_longest_name() + 6,
                                    ucHauptEintraege + 4);
  else
    winHauptWindow = win_einrichten(ucWinX , ucWinY, get_longest_name() + 6,
                                    5);
  win_titel(winHauptWindow, " HauptmenÅ ", TITEL_O_Z);
  win_schatten(winHauptWindow, TRUE);
  win_set_close(winHauptWindow, TRUE);
  win_set_move(winHauptWindow, TRUE);
}

void gruppen_window (UCHAR ucWinX, UCHAR ucWinY, UCHAR *pucName)
{
  if (winGrpWindow)
    win_entfernen(winGrpWindow);
  if (ucGrpEintraege)
    winGrpWindow = win_einrichten(ucWinX , ucWinY, get_longest_grp(pucName) +
                                  6, ucGrpEintraege + 4);
  else
    winGrpWindow = win_einrichten(ucWinX , ucWinY, get_longest_grp(pucName) +
                                  6, 5);
  strcpy(aucWinTitel, " ");
  strcat(aucWinTitel, pucName);
  strcat(aucWinTitel, " ");
  win_titel(winGrpWindow, aucWinTitel, TITEL_O_Z);
  win_schatten(winGrpWindow, TRUE);
  win_set_close(winGrpWindow, TRUE);
  win_set_move(winGrpWindow, TRUE);
}

main()
{
  stools_init();
  utl_insert_short(T_A_X, ende);
  _fmode = O_BINARY;
  read_haupt();
  init_screen();
  run();
  return(0);
}
