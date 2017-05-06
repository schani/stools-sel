/***************************************************************************
 ***************************************************************************
 ***                                                                     ***
 ***                             SE-Menu                                 ***
 ***                                                                     ***
 ***                (c) 1990-92 by Schani Electronics                    ***
 ***                                                                     ***
 ***************************************************************************
 ***************************************************************************/

#include <stools.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dos.h>
#include <ctype.h>
#include <fcntl.h>
#include "menu.h"

UCHAR             *pucHauptDatei     = "HAUPT",
		  *pucConfigDatei    = "SEMENU.CFG",
		  *pucTempBatch      = "TEMP.BAT",
		  *pucUeber          = "      S E  -  M e n u  V2.2\n"
				       "\n"
				       "(c) 1991-92 by Schani Electronics\n"
				       "     written by Mark Probst\n"
				       "\n"
				       "   Schani Tools (c) 1990-92 by\n"
				       "       Schani Electronics",
		  *pucPark           = "Die K”pfe Ihrer Festplatte sind\n"
				       "jetzt geparkt. Sie k”nnen den\n"
				       "Computer nun ausschalten.",
		  *pucMaxEintraege   = "Die maximale Anzahl der\n"
				       "Eintr„ge ist schon erreicht!",
		  *pucGruppenName    = "Der Name der Gruppendatei\n"
				       "ist falsch!",
		  *pucInvDir         = "Das Verzeichnis ist falsch!",
		  *pucPrgName        = "Sie mssen einen Namen eingeben!",
		  *pucAufruf         = "Sie mssen einen Aufruf eingeben!",
		  *pucWirklichGrpDel = "Wollen Sie die Programmgruppe\n"
				       "wirklich l”schen?",
		  *pucWirklichPrgDel = "Wollen Sie das Programm\n"
				       "wirklich l”schen?",
		  *pucMnuExtension   = ".MNU",
                  *pucTag            = "CALL TAG.BAT",
                  *pucWoche          = "CALL WOCHE.BAT",
                  *pucMonat          = "CALL MONAT.BAT",
		  *pucLeer           = "",
		   ucUhr             = TRUE,
		   ucButtonBar       = TRUE,
                   ucTaeglich        = FALSE,
                   ucWoechentlich    = FALSE,
                   ucMonatlich       = FALSE,
                   aucDaysPerMonth[] = {
                                         31,
                                         28,
                                         31,
                                         30,
                                         31,
                                         30,
                                         31,
                                         31,
                                         30,
                                         31,
                                         30,
                                         31
                                       };
_MNU_MENU         *pmenuHauptMenu,
		  *pmenuAktMenu;
STS_ITEM           aitemStatusLine[] =
				       {
					 {
					   "~+~ Hinzufgen",
					   ID_DAZUFUEGEN,
					   '+'
					 },
					 {
					   "~Einfg~ Einfgen",
					   ID_EINFUEGEN,
					   T_INS
					 },
					 {
					   "~Space~ Bearbeiten",
					   ID_BEARBEITEN,
					   T_SPACE
					 },
					 {
					   "~Entf~ L”schen",
					   ID_LOESCHEN,
					   T_DEL
					 },
					 {
					   "~Alt-X~ Ende",
					   ID_ENDE,
					   T_A_X
					 },
					 STS_ENDE
				       };

void save_menus (void)
{

  _MNU_MENU *pmenuMenu;

  for (pmenuMenu = pmenuHauptMenu; pmenuMenu;
       pmenuMenu = pmenuMenu->pmenuUnterMenu)
    if (pmenuMenu->ucChanged)
    {
      if (pmenuMenu->pmenuOberMenu)
	write_menu(pmenuMenu->pmenuOberMenu->aucMnuName,
		   pmenuMenu);
      else
	write_menu(pucHauptDatei, pmenuMenu);
    }
}

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
  pucPos = (UCHAR*)strchr(pucString, '.');
  if (pucPos)
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

void uhr_zeigen (void)
{

  struct time  tTime;
  UCHAR        aucPuffer[10];
  static UCHAR ucLastSecond;

  if (ucUhr && !ucSaverAktiv)
  {
    gettime(&tTime);
    if (tTime.ti_sec != ucLastSecond)
    {
      sprintf(aucPuffer, "%2d:%02d:%02d",
	      tTime.ti_hour, tTime.ti_min, tTime.ti_sec);
      msm_cursor_off();
      vio_ss(73, 1, aucPuffer);
      msm_cursor_on();
      ucLastSecond = tTime.ti_sec;
    }
  }
  else
    ucLastSecond = 62;
}

void park (void)
{

  union REGS regs;
  UCHAR      ucZaehler;

  for (ucZaehler = 0x80; ucZaehler < 0x85; ucZaehler++)
  {
    regs.h.ah = 0x11;
    regs.h.dl = ucZaehler;
    int86(0x13, &regs, &regs);
  }
}

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
  utl_get_pfad(pucTemp);
  fwrite(pucTemp, 2, 1, pfileDatei);
  fprintf(pfileDatei, "\n");
  pucTemp += 2;
  fprintf(pfileDatei, "cd %s\nsemenu\n", pucTemp);
  fclose(pfileDatei);
  write_config();
  save_menus();
  exit(10);
}

UCHAR today (struct dosdate_t *pdDate)
{

  struct dosdate_t dAktDate;

  _dos_getdate(&dAktDate);
  return(pdDate->day == dAktDate.day &&
         pdDate->month == dAktDate.month &&
         pdDate->year == dAktDate.year);
}

UCHAR this_week (struct dosdate_t *pdDate)
{

  struct dosdate_t dAktDate;

  _dos_getdate(&dAktDate);
  if (pdDate->year == dAktDate.year)
  {
    if (pdDate->month == dAktDate.month)
    {
      if (pdDate->day == dAktDate.day)
        return(TRUE);
      return((dAktDate.day - dAktDate.dayofweek + 1) <= pdDate->day);
    }
    if ((dAktDate.month - pdDate->month) > 1)
      return(FALSE);
    return((aucDaysPerMonth[pdDate->month - 1] - pdDate->day + dAktDate.day)
           < dAktDate.dayofweek);
  }
  if (pdDate->month < 12 || dAktDate.month > 1)
    return(FALSE);
  return((aucDaysPerMonth[pdDate->month - 1] - pdDate->day + dAktDate.day) <
         dAktDate.dayofweek);
}

UCHAR this_month (struct dosdate_t *pdDate)
{

  struct dosdate_t dAktDate;

  _dos_getdate(&dAktDate);
  return(pdDate->month == dAktDate.month &&
         pdDate->year == dAktDate.year);
}

void read_config (void)
{

  FILE             *pfileDatei;
  UCHAR             ucMouse;
  fpos_t            pPos;
  struct dosdate_t  dLastDate,
                    dAktDate;

  if (!(pfileDatei = fopen(pucConfigDatei, "r+")))
    write_config();
  else
  {
    _dos_getdate(&dAktDate);
    fread(&ucBeep, sizeof(UCHAR), 1, pfileDatei);
    fread(&uiBeepDauer, sizeof(UINT), 1, pfileDatei);
    fread(&uiBeepFrequenz, sizeof(UINT), 1, pfileDatei);
    fread(&ucAktPalette, sizeof(UCHAR), 1, pfileDatei);
    fread(&uiMouseSpeed, sizeof(UINT), 1, pfileDatei);
    fread(&uiDoubleClick, sizeof(UINT), 1, pfileDatei);
    fread(&ucUhr, sizeof(UCHAR), 1, pfileDatei);
    fread(&ucButtonBar, sizeof(UCHAR), 1, pfileDatei);
    fread(apalPalette + P_USER_DEFINED, sizeof(PALETTE), 1, pfileDatei);
    fread(&ucSaver, sizeof(UCHAR), 1, pfileDatei);
    fread(&uiScreenTicks, sizeof(UINT), 1, pfileDatei);
    fread(&ucTaeglich, sizeof(UCHAR), 1, pfileDatei);
    fread(&ucWoechentlich, sizeof(UCHAR), 1, pfileDatei);
    fread(&ucMonatlich, sizeof(UCHAR), 1, pfileDatei);
    fread(&dLastDate, sizeof(struct dosdate_t), 1, pfileDatei);
    if (!today(&dLastDate) && ucTaeglich)
    {
      fgetpos(pfileDatei, &pPos);
      pPos -= sizeof(struct dosdate_t);
      fseek(pfileDatei, pPos, SEEK_SET);
      fwrite(&dAktDate, sizeof(struct dosdate_t), 1, pfileDatei);
      fclose(pfileDatei);
      execute(pucLeer, pucTag);
    }
    fread(&dLastDate, sizeof(struct dosdate_t), 1, pfileDatei);
    if (!this_week(&dLastDate) && ucWoechentlich)
    {
      fgetpos(pfileDatei, &pPos);
      pPos -= sizeof(struct dosdate_t);
      fseek(pfileDatei, pPos, SEEK_SET);
      fwrite(&dAktDate, sizeof(struct dosdate_t), 1, pfileDatei);
      fclose(pfileDatei);
      execute(pucLeer, pucWoche);
    }
    fread(&dLastDate, sizeof(struct dosdate_t), 1, pfileDatei);
    if (!this_month(&dLastDate) && ucMonatlich)
    {
      fgetpos(pfileDatei, &pPos);
      pPos -= sizeof(struct dosdate_t);
      fseek(pfileDatei, pPos, SEEK_SET);
      fwrite(&dAktDate, sizeof(struct dosdate_t), 1, pfileDatei);
      fclose(pfileDatei);
      execute(pucLeer, pucMonat);
    }
    fclose(pfileDatei);
  }
  msm_set_mickeys(uiMouseSpeed, uiMouseSpeed * 2);
  ucMouse = msm_cursor_off();
  vio_sw_a(1, 2, 80, 23, AKT_PALETTE.ucAppSpace);
  if (ucMouse)
    msm_cursor_on();
}

void write_config (void)
{

  FILE             *pfileDatei;
  struct dosdate_t  dDate;
  UCHAR             ucStartup  = FALSE;

  if (!(pfileDatei = fopen(pucConfigDatei, "r+")))
  {
    if (!(pfileDatei = fopen(pucConfigDatei, "w+")))
    {
      utl_cls(HELLGRAU);
      vio_set_cursor_pos(1, 1);
      printf("Konfigurationsdatei kann nicht ge”ffnet werden\n");
      utl_get_taste();
      exit(1);
    }
    ucStartup = TRUE;
  }
  _dos_getdate(&dDate);
  fwrite(&ucBeep, sizeof(UCHAR), 1, pfileDatei);
  fwrite(&uiBeepDauer, sizeof(UINT), 1, pfileDatei);
  fwrite(&uiBeepFrequenz, sizeof(UINT), 1, pfileDatei);
  fwrite(&ucAktPalette, sizeof(UCHAR), 1, pfileDatei);
  fwrite(&uiMouseSpeed, sizeof(UINT), 1, pfileDatei);
  fwrite(&uiDoubleClick, sizeof(UINT), 1, pfileDatei);
  fwrite(&ucUhr, sizeof(UCHAR), 1, pfileDatei);
  fwrite(&ucButtonBar, sizeof(UCHAR), 1, pfileDatei);
  fwrite(apalPalette + P_USER_DEFINED, sizeof(PALETTE), 1, pfileDatei);
  fwrite(&ucSaver, sizeof(UCHAR), 1, pfileDatei);
  fwrite(&uiScreenTicks, sizeof(UINT), 1, pfileDatei);
  fwrite(&ucTaeglich, sizeof(UCHAR), 1, pfileDatei);
  fwrite(&ucWoechentlich, sizeof(UCHAR), 1, pfileDatei);
  fwrite(&ucMonatlich, sizeof(UCHAR), 1, pfileDatei);
  if (feof(pfileDatei) || ucStartup)
    fwrite(&dDate, sizeof(struct dosdate_t), 1, pfileDatei);
  else
    fseek(pfileDatei, sizeof(struct dosdate_t), SEEK_CUR);
  if (feof(pfileDatei) || ucStartup)
    fwrite(&dDate, sizeof(struct dosdate_t), 1, pfileDatei);
  else
    fseek(pfileDatei, sizeof(struct dosdate_t), SEEK_CUR);
  if (feof(pfileDatei) || ucStartup)
    fwrite(&dDate, sizeof(struct dosdate_t), 1, pfileDatei);
  else
    fseek(pfileDatei, sizeof(struct dosdate_t), SEEK_CUR);
  fclose(pfileDatei);
}

void read_menu (UCHAR *pucFileName, _MNU_MENU* pmenuOwner,
		_MNU_MENU *pmenuMenu)
{

  FILE *pfileDatei;
  UCHAR ucWinX,
	ucWinY,
	aucTitel[51],
	aucFileName[13];

  pmenuMenu->ucChanged = FALSE;
  strcpy(aucFileName, pucFileName);
  strcat(aucFileName, pucMnuExtension);
  if (!(pfileDatei = fopen(aucFileName, "r")))
  {
    if (pucFileName == pucHauptDatei)
    {
      pmenuMenu->ucEintraege = 0;
      strcpy(pmenuMenu->aucMnuName, pucLeer);
      pmenuMenu->pmenuOberMenu = NULL;
      pmenuMenu->pmenuUnterMenu = NULL;
      make_window(pmenuMenu, 10, 5, " Hauptmen ");
      write_menu(pucFileName, pmenuMenu);
      return;
    }
    utl_cls(HELLGRAU);
    vio_set_cursor_pos(1, 1);
    printf("Mendatei kann nicht ge”ffnet werden\n");
    utl_get_taste();
    exit(1);
  }
  else
  {
    fread(&(pmenuMenu->ucEintraege), sizeof(UCHAR), 1, pfileDatei);
    fread(&(pmenuMenu->ameEintrag), sizeof(MNU_EINTRAG),
	  pmenuMenu->ucEintraege, pfileDatei);
    fread(&(pmenuMenu->aucMnuName), 9, 1, pfileDatei);
    fread(&ucWinX, sizeof(UCHAR), 1, pfileDatei);
    fread(&ucWinY, sizeof(UCHAR), 1, pfileDatei);
    fread(aucTitel, 51, 1, pfileDatei);
    fclose(pfileDatei);
    make_window(pmenuMenu, ucWinX, ucWinY, aucTitel);
    pmenuMenu->pmenuOberMenu = pmenuOwner;
    pmenuMenu->pmenuUnterMenu = NULL;
  }
}

void write_menu (UCHAR *pucFileName, _MNU_MENU *pmenuMenu)
{

  FILE *pfileDatei;
  UCHAR aucFileName[13];

  pmenuMenu->ucChanged = FALSE;
  strcpy(aucFileName, pucFileName);
  strcat(aucFileName, pucMnuExtension);
  if (!(pfileDatei = fopen(aucFileName, "w")))
  {
    utl_cls(HELLGRAU);
    vio_set_cursor_pos(1, 1);
    printf("Mendatei kann nicht ge”ffnet werden\n");
    utl_get_taste();
    exit(1);
  }
  else
  {
    fwrite(&(pmenuMenu->ucEintraege), sizeof(UCHAR), 1, pfileDatei);
    fwrite(&(pmenuMenu->ameEintrag), sizeof(MNU_EINTRAG),
	   pmenuMenu->ucEintraege, pfileDatei);
    fwrite(&(pmenuMenu->aucMnuName), 9, 1, pfileDatei);
    fwrite(&(pmenuMenu->winWindow->ucX), sizeof(UCHAR), 1, pfileDatei);
    fwrite(&(pmenuMenu->winWindow->ucY), sizeof(UCHAR), 1, pfileDatei);
    fwrite(pmenuMenu->winWindow->pucTitel, 51, 1, pfileDatei);
    fclose(pfileDatei);
  }
}

void create_menu (UCHAR *pucFileName, UCHAR *pucWinTitel)
{

  UCHAR      aucWinTitel[51];
  _MNU_MENU *pmenuMenu;

  pmenuMenu = utl_alloc(sizeof(_MNU_MENU));
  pmenuMenu->ucEintraege = 0;
  pmenuMenu->winWindow = win_einrichten(20, 5,
					strlen(pucWinTitel) + 12, 5);
  strcpy(aucWinTitel, " ");
  strcat(aucWinTitel, pucWinTitel);
  strcat(aucWinTitel, " ");
  win_titel(pmenuMenu->winWindow, aucWinTitel, TITEL_O_Z);
  strcpy(pmenuMenu->aucMnuName, pucLeer);
  pmenuMenu->ucChanged = FALSE;
  write_menu(pucFileName, pmenuMenu);
  win_entfernen(pmenuMenu->winWindow);
  utl_free(pmenuMenu);
}

void chain_menu (_MNU_MENU *pmenuOwnerMenu, UCHAR ucItem)
{

  _MNU_MENU *pmenuMenu;

  pmenuMenu = utl_alloc(sizeof(_MNU_MENU));
  read_menu(pmenuOwnerMenu->ameEintrag[ucItem].aucDatei, pmenuOwnerMenu,
	    pmenuMenu);
  pmenuOwnerMenu->pmenuUnterMenu = pmenuMenu;
  strcpy(pmenuOwnerMenu->aucMnuName,
	 pmenuOwnerMenu->ameEintrag[ucItem].aucDatei);
  pmenuOwnerMenu->ucChanged = TRUE;
}

void rename_menu (UCHAR *pucOldName, UCHAR *pucNewName)
{

  UCHAR        aucOldName[13],
	       aucNewName[13];
  union REGS   regs;
  struct SREGS sregs;

  strcpy(aucOldName, pucOldName);
  strcat(aucOldName, pucMnuExtension);
  strcpy(aucNewName, pucNewName);
  strcat(aucNewName, pucMnuExtension);
  regs.h.ah = 0x56;
  regs.x.dx = FP_OFF((void far*)aucOldName);
  regs.x.di = FP_OFF((void far*)aucNewName);
  sregs.ds = FP_SEG((void far*)aucOldName);
  sregs.es = FP_SEG((void far*)aucNewName);
  intdosx(&regs, &regs, &sregs);
}

void make_window (_MNU_MENU *pmenuMenu, UCHAR ucX, UCHAR ucY,
		  UCHAR *pucTitel)
{

  UCHAR  ucBreite;
  UCHAR *pucWinTitel;

  ucBreite = get_longest_name(pmenuMenu, pucTitel) + 6;
  if (pmenuMenu->ucEintraege)
    pmenuMenu->winWindow = win_einrichten(ucX, ucY, ucBreite,
					  pmenuMenu->ucEintraege + 4);
  else
    pmenuMenu->winWindow = win_einrichten(ucX, ucY, ucBreite, 5);
  win_set_close(pmenuMenu->winWindow, TRUE);
  pucWinTitel = utl_alloc(strlen(pucTitel) + 1);
  strcpy(pucWinTitel, pucTitel);
  win_titel(pmenuMenu->winWindow, pucWinTitel, TITEL_O_Z);
}

UCHAR get_longest_name (_MNU_MENU *pmenuMenu, UCHAR *pucTitel)
{

  UCHAR ucReturnVar,
	ucZaehler;

  ucReturnVar = strlen(pucTitel) + 6;
  for (ucZaehler = 0; ucZaehler < pmenuMenu->ucEintraege; ucZaehler++)
  {
    if (strlen(pmenuMenu->ameEintrag[ucZaehler].aucName) > ucReturnVar)
      ucReturnVar = strlen(pmenuMenu->ameEintrag[ucZaehler].aucName);
  }
  return(ucReturnVar);
}

void init_haupt_menu (void)
{
  pmenuHauptMenu = utl_alloc(sizeof(_MNU_MENU));
  read_menu(pucHauptDatei, NULL, pmenuHauptMenu);
  pmenuAktMenu = pmenuHauptMenu;
  while (strcmp(pmenuAktMenu->aucMnuName, pucLeer))
  {
    pmenuAktMenu->pmenuUnterMenu = utl_alloc(sizeof(_MNU_MENU));
    read_menu(pmenuAktMenu->aucMnuName, pmenuAktMenu,
	      pmenuAktMenu->pmenuUnterMenu);
    pmenuAktMenu = pmenuAktMenu->pmenuUnterMenu;
  }
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
			     "#™ffnen                 Enter",
			     " Startet das Programm bzw. ”ffnet die Gruppe",
			     ID_OEFFNEN
			   },
			   {
			     "#Schlieáen                Esc",
			     " Schlieát die Programmgruppe",
			     ID_SCHLIESSEN
			   },
			   MNU_TRENNER,
			   {
			     "#šber...",
			     " Programminformationen",
			     ID_UEBER
			   },
			   MNU_TRENNER,
			   {
			     "#Beenden der Sitzung...",
			     " Parkt die K”pfe der Festplatte",
			     ID_SHUTDOWN
			   },
			   {
			     "#Ende                   Alt-X",
			     " Programmende",
			     ID_ENDE
			   },
			   MNU_ENDE
			 },
       aitemProgramm[] = {
			   {
			     "#Programm",
			     " Programme einrichten und l”schen",
			     PROGRAMM
			   },
			   {
			     "#Hinzufgen...              +",
			     " Richtet ein neues Programm am Ende der Liste ein",
			     ID_DAZUFUEGEN
			   },
			   {
			     "#Einfgen...            Einfg",
			     " Fgt ein neues Programm vor dem Auswahlcursor ein",
			     ID_EINFUEGEN
			   },
			   {
			     "#Bearbeiten...          Space",
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
			     "#L”schen...              Entf",
			     " L”scht das aktuelle Programm",
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
			   {
			     "Fenster#titel...",
			     " Setzt den Titel des aktuellen Fensters",
			     ID_TITEL
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
			   {
			     "#Bildschirmschoner...",
			     " Wahl der Zeitspanne des Bildschirmschoners",
			     ID_SCR_SAVER
			   },
			   {
			     "#Eigene Farbpalette...",
			     " Definition einer eigenen Farbpalette",
			     ID_CUST_COLORS
			   },
			   {
			     "#Sonstiges...",
			     " Uhr und Statuszeile",
			     ID_SONSTIGES
			   },
			   MNU_ENDE
			 };

  mnu_neu(HAUPTMENU);
  mnu_neu_window(HAUPTMENU, aitemDatei);
  mnu_neu_window(HAUPTMENU, aitemProgramm);
  mnu_neu_window(HAUPTMENU, aitemOptionen);
  mnu_aktiv(HAUPTMENU, ID_SCHLIESSEN, FALSE);
  mnu_aktivieren(HAUPTMENU);
  if (ucButtonBar)
    sts_new_status_line(aitemStatusLine);
  show_windows();
  utl_insert_null_event(uhr_zeigen);
}

void show_window (_MNU_MENU* pmenuMenu)
{

  UCHAR ucZaehler;

  win_color(pmenuMenu->winWindow, AKT_PALETTE.ucDlgBorder, AKT_PALETTE.ucDlg,
	    AKT_PALETTE.ucDlgBorder);
  win_aktivieren(pmenuMenu->winWindow);
  win_cls();
  for (ucZaehler = 0; ucZaehler < pmenuMenu->ucEintraege; ucZaehler++)
    win_ss(3, 2 + ucZaehler, pmenuMenu->ameEintrag[ucZaehler].aucName);
  if (strcmp(pmenuMenu->aucMnuName, pucLeer))
  {
    for (ucZaehler = 0; ucZaehler < pmenuMenu->ucEintraege; ucZaehler++)
      if (!strcmp(pmenuMenu->ameEintrag[ucZaehler].aucDatei,
		  pmenuMenu->aucMnuName))
      {
	win_sw_a(2, 2 + ucZaehler, pmenuMenu->winWindow->ucBreite - 4, 1,
		 AKT_PALETTE.ucDlgLstBoxActItem);
	break;
      }
  }
}

void show_windows (void)
{

  _MNU_MENU *pmenuMenu;

  while (win_get_aktiv())
    win_verstecken(win_get_aktiv());
  pmenuMenu = pmenuHauptMenu;
  do
  {
    show_window(pmenuMenu);
    pmenuMenu = pmenuMenu->pmenuUnterMenu;
  } while (pmenuMenu);
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
  winWindow = win_einrichten(18, 8, 43, 8);
  apelementElemente[0] = dlg_init_act_button(3, 6, "#Programm", 0,
					     BOX_PROGRAMM, pucProgramm);
  apelementElemente[1] = dlg_init_act_button(17, 6, "#Gruppe", 0, BOX_GRUPPE,
					     pucGruppe);
  apelementElemente[2] = dlg_init_act_button(29, 6, "Abbruch", T_ESC,
					     BOX_ABBRUCH, pucAbbruch);
  apelementElemente[3] = NULL;
  win_aktivieren(winWindow);
  win_ss(3, 2, pucZeile1);
  win_ss(3, 3, pucZeile2);
  win_sw_za(1, 5, 41, 1, 'Ä', AKT_PALETTE.ucDlgBorder);
  uiReturnVar = dlg_aktivieren(apelementElemente);
  win_entfernen(winWindow);
  dlg_del_act_button(apelementElemente[0]);
  dlg_del_act_button(apelementElemente[1]);
  dlg_del_act_button(apelementElemente[2]);
  utl_short_cuts(ucShorts);
  return((UCHAR)uiReturnVar);
}

void run (void)
{

  UCHAR             aucName[61],
		    aucHelpLine[79],
		    aucDatei[131],
		    aucWorkDir[131],
		    ucAktItem       = 0,
		    ucZaehler,
		    ucWinX,
		    ucWinY,
		    ucEnde,
		   *pucTemp;
  UINT              uiWahl;
  UTL_EVENT         eventEvent;
  FILE             *pfileDatei;
  WINDOW            winTemp;

  do
  {
    if (auswahl(&uiWahl, &ucAktItem, FALSE))
    {
      switch (uiWahl)
      {
	case ID_UEBER :
	  box_info(BOX_INFO, BOX_OK, pucUeber);
	  break;
	case ID_SHUTDOWN :
	  write_config();
	  save_menus();
	  park();
	  box_info(BOX_INFO, BOX_ABBRUCH, pucPark);
	  break;
	case ID_ENDE :
	  write_config();
	  save_menus();
	  exit(0);
	  break;
	case ID_DAZUFUEGEN :
	  if (pmenuAktMenu->ucEintraege >= 18)
	  {
	    box_info(BOX_INFO, BOX_OK, pucMaxEintraege);
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
		winTemp = pmenuAktMenu->winWindow;
		pucTemp = pmenuAktMenu->winWindow->pucTitel;
		make_window(pmenuAktMenu, min(winTemp->ucX,
					      73 - get_longest_name
						     (pmenuAktMenu, pucTemp)),
					  min(winTemp->ucY,
					      20 - pmenuAktMenu->
						     ucEintraege), pucTemp);
		win_entfernen(winTemp);
		show_window(pmenuAktMenu);
		pmenuAktMenu->ucChanged = TRUE;
	      }
	      break;
	    case BOX_GRUPPE :
	      strcpy(pmenuAktMenu->ameEintrag[pmenuAktMenu->ucEintraege].
		     aucName, pucLeer);
	      strcpy(pmenuAktMenu->ameEintrag[pmenuAktMenu->ucEintraege].
		     aucHelpLine, pucLeer);
	      strcpy(pmenuAktMenu->ameEintrag[pmenuAktMenu->ucEintraege].
		     aucDatei, pucLeer);
	      if (gruppen_box(" Gruppe hinzufgen ",
			      pmenuAktMenu->ameEintrag
				[pmenuAktMenu->ucEintraege].aucName,
			      pmenuAktMenu->ameEintrag
				[pmenuAktMenu->ucEintraege].aucHelpLine,
			      pmenuAktMenu->ameEintrag
				[pmenuAktMenu->ucEintraege].aucDatei))
	      {
		pmenuAktMenu->ameEintrag[pmenuAktMenu->ucEintraege].
		  ucProgramm = FALSE;
		pmenuAktMenu->ucEintraege++;
		winTemp = pmenuAktMenu->winWindow;
		pucTemp = pmenuAktMenu->winWindow->pucTitel;
		make_window(pmenuAktMenu, min(winTemp->ucX,
					      73 - get_longest_name
						     (pmenuAktMenu, pucTemp)),
					  min(winTemp->ucY,
					      20 - pmenuAktMenu->
						     ucEintraege), pucTemp);
		win_entfernen(winTemp);
		show_window(pmenuAktMenu);
		pmenuAktMenu->ucChanged = TRUE;
		create_menu(pmenuAktMenu->ameEintrag
			      [pmenuAktMenu->ucEintraege - 1].aucDatei,
			    pmenuAktMenu->ameEintrag
			      [pmenuAktMenu->ucEintraege - 1].aucName);
	      }
	      break;
	  }
	  break;
	case ID_EINFUEGEN :
	  if (pmenuAktMenu->ucEintraege >= 18)
	  {
	    box_info(BOX_INFO, BOX_OK, pucMaxEintraege);
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
		winTemp = pmenuAktMenu->winWindow;
		pucTemp = pmenuAktMenu->winWindow->pucTitel;
		make_window(pmenuAktMenu, min(winTemp->ucX,
					      73 - get_longest_name
						     (pmenuAktMenu, pucTemp)),
					  min(winTemp->ucY,
					      20 - pmenuAktMenu->
						     ucEintraege), pucTemp);
		win_entfernen(winTemp);
		show_window(pmenuAktMenu);
		pmenuAktMenu->ucChanged = TRUE;
	      }
	      break;
	    case BOX_GRUPPE :
	      strcpy(aucName, pucLeer);
	      strcpy(aucHelpLine, pucLeer);
	      strcpy(aucDatei, pucLeer);
	      if (gruppen_box(" Gruppe einfgen ", aucName, aucHelpLine,
			      aucDatei))
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
		strcpy(pmenuAktMenu->ameEintrag[ucAktItem].aucDatei,
		       aucDatei);
		winTemp = pmenuAktMenu->winWindow;
		pucTemp = pmenuAktMenu->winWindow->pucTitel;
		make_window(pmenuAktMenu, min(winTemp->ucX,
					      73 - get_longest_name
						     (pmenuAktMenu, pucTemp)),
					  min(winTemp->ucY,
					      20 - pmenuAktMenu->
						     ucEintraege), pucTemp);
		win_entfernen(winTemp);
		show_window(pmenuAktMenu);
		pmenuAktMenu->ucChanged = TRUE;
		create_menu(pmenuAktMenu->ameEintrag[ucAktItem].aucDatei,
			    pmenuAktMenu->ameEintrag[ucAktItem].aucName);
	      }
	      break;
	  }
	  break;
	case ID_BEARBEITEN :
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
		winTemp = pmenuAktMenu->winWindow;
		pucTemp = pmenuAktMenu->winWindow->pucTitel;
		make_window(pmenuAktMenu, min(winTemp->ucX,
					      73 - get_longest_name
						     (pmenuAktMenu, pucTemp)),
					  min(winTemp->ucY,
					      20 - pmenuAktMenu->
						     ucEintraege), pucTemp);
		win_entfernen(winTemp);
		show_window(pmenuAktMenu);
		pmenuAktMenu->ucChanged = TRUE;
	      }
	    }
	    else
	    {
	      strcpy(aucName, pmenuAktMenu->ameEintrag[ucAktItem].aucName);
	      strcpy(aucHelpLine,
		     pmenuAktMenu->ameEintrag[ucAktItem].aucHelpLine);
	      strcpy(aucDatei,
		     pmenuAktMenu->ameEintrag[ucAktItem].aucDatei);
	      if (gruppen_box(" Gruppe bearbeiten ", aucName, aucHelpLine,
			      aucDatei))
	      {
		rename_menu(pmenuAktMenu->ameEintrag[ucAktItem].aucDatei,
			    aucDatei);
		strcpy(pmenuAktMenu->ameEintrag[ucAktItem].aucName,
		       aucName);
		strcpy(pmenuAktMenu->ameEintrag[ucAktItem].aucHelpLine,
		       aucHelpLine);
		strcpy(pmenuAktMenu->ameEintrag[ucAktItem].aucDatei,
		       aucDatei);
		winTemp = pmenuAktMenu->winWindow;
		pucTemp = pmenuAktMenu->winWindow->pucTitel;
		make_window(pmenuAktMenu, min(winTemp->ucX,
					      73 - get_longest_name
						     (pmenuAktMenu, pucTemp)),
					  min(winTemp->ucY,
					      20 - pmenuAktMenu->
						     ucEintraege), pucTemp);
		win_entfernen(winTemp);
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
	  sts_write_help(" Bitte Cursortasten oder Maus zum Bewegen des Programms benutzen");
	  auswahl(&uiWahl, &ucAktItem, TRUE);
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
	  if (!(pmenuAktMenu->ucEintraege))
	  {
	    utl_beep();
	    break;
	  }
	  if (pmenuAktMenu->ameEintrag[ucAktItem].ucProgramm)
	  {
	    if (box_info(BOX_INFO, BOX_OK | BOX_ABBRUCH, pucWirklichPrgDel)
		!= BOX_OK)
	      break;
	  }
	  else
	  {
	    if (box_info(BOX_INFO, BOX_OK | BOX_ABBRUCH, pucWirklichGrpDel)
		!= BOX_OK)
	      break;
	    del_gruppe(pmenuAktMenu->ameEintrag[ucAktItem].aucDatei);
	  }
	  memmove(pmenuAktMenu->ameEintrag + ucAktItem,
		  pmenuAktMenu->ameEintrag + ucAktItem + 1,
		  sizeof(MNU_EINTRAG) *
		    (pmenuAktMenu->ucEintraege - 1 - ucAktItem));
	  pmenuAktMenu->ucEintraege--;
	  if (ucAktItem == pmenuAktMenu->ucEintraege)
	    ucAktItem--;
	  winTemp = pmenuAktMenu->winWindow;
	  pucTemp = pmenuAktMenu->winWindow->pucTitel;
	  make_window(pmenuAktMenu, winTemp->ucX, winTemp->ucY, pucTemp);
	  win_entfernen(winTemp);
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
	  strcpy(aucName, win_get_aktiv()->pucTitel);
	  if (titel_box(aucName))
	  {
	    winTemp = pmenuAktMenu->winWindow;
	    pucTemp = aucName;
	    make_window(pmenuAktMenu, min(winTemp->ucX,
					  73 - get_longest_name
					  (pmenuAktMenu, pucTemp)),
				      min(winTemp->ucY,
					  20 - pmenuAktMenu->
					  ucEintraege), pucTemp);
	    win_entfernen(winTemp);
	    show_window(pmenuAktMenu);
	    pmenuAktMenu->ucChanged = TRUE;
	  }
	  break;
	case ID_CUST_COLORS :
	  box_custom_farben();
	  mnu_set_palette();
	  while (win_get_aktiv())
	    win_verstecken(win_get_aktiv());
	  msm_cursor_off();
	  vio_sw_a(1, 2, 80, 23, AKT_PALETTE.ucAppSpace);
	  msm_cursor_on();
	  show_windows();
	  sts_set_palette();
	  break;
	case ID_FARBPALETTE :
	  box_farbpalette();
	  mnu_set_palette();
	  while (win_get_aktiv())
	    win_verstecken(win_get_aktiv());
	  msm_cursor_off();
	  vio_sw_a(1, 2, 80, 23, AKT_PALETTE.ucAppSpace);
	  msm_cursor_on();
	  show_windows();
	  sts_set_palette();
	  break;
	case ID_MAUSTEMPO :
	  box_mouse();
	  break;
	case ID_WARNTON :
	  box_warnton();
	  break;
	case ID_SCR_SAVER :
	  box_screen_saver();
	  break;
	case ID_SONSTIGES :
	  sonstiges_box();
	  break;
      }
    }
    else
    {
      if (uiWahl != 64000)
      {
	if (pmenuAktMenu->ameEintrag[ucAktItem].ucProgramm)
          execute(pmenuAktMenu->ameEintrag[ucAktItem].aucWorkDir,
                  pmenuAktMenu->ameEintrag[ucAktItem].aucCmdLine);
	else
	{
	  chain_menu(pmenuAktMenu, ucAktItem);
	  pmenuAktMenu = pmenuAktMenu->pmenuUnterMenu;
	  show_window(pmenuAktMenu);
	  ucAktItem = 0;
	  mnu_aktiv(HAUPTMENU, ID_SCHLIESSEN, TRUE);
	}
      }
      else
      {
	if (pmenuAktMenu->pmenuOberMenu)
	{
	  if (pmenuAktMenu->ucChanged)
	    write_menu(pmenuAktMenu->pmenuOberMenu->aucMnuName,
		       pmenuAktMenu);
	  win_entfernen(pmenuAktMenu->winWindow);
	  pmenuAktMenu = pmenuAktMenu->pmenuOberMenu;
	  utl_free(pmenuAktMenu->pmenuUnterMenu);
	  pmenuAktMenu->pmenuUnterMenu = NULL;
	  for (ucZaehler = 0; ucZaehler < pmenuAktMenu->ucEintraege;
	       ucZaehler++)
	    if (!strcmp(pmenuAktMenu->ameEintrag[ucZaehler].aucDatei,
			pmenuAktMenu->aucMnuName))
	    {
	      ucAktItem = ucZaehler;
	      break;
	    }
	  strcpy(pmenuAktMenu->aucMnuName, pucLeer);
	  pmenuAktMenu->ucChanged = TRUE;
	  if (pmenuAktMenu == pmenuHauptMenu)
	    mnu_aktiv(HAUPTMENU, ID_SCHLIESSEN, FALSE);
	}
      }
    }
  } while (TRUE);
}

void bewegen (_MNU_MENU *pmenuMenu, MNU_EINTRAG **ppmeAktZeile, UCHAR ucPos)
{

  MNU_EINTRAG  mePufferZeile,
	      *pmeMenu,
	      *pmeZaehler;

  pmeMenu = pmenuMenu->ameEintrag;
  if (pmeMenu + ucPos == *ppmeAktZeile)
  {
    win_sw_a(2, 2 + (*ppmeAktZeile - pmeMenu), win_get_aktiv()->ucBreite - 4,
	     1, AKT_PALETTE.ucDlgLstBoxActItem);
    return;
  }
  memmove(&mePufferZeile, *ppmeAktZeile, sizeof(MNU_EINTRAG));
  if (pmeMenu + ucPos < *ppmeAktZeile)
    memmove(pmeMenu + ucPos + 1, pmeMenu + ucPos,
	    sizeof(MNU_EINTRAG) * (*ppmeAktZeile - pmeMenu - ucPos));
  else
    memmove(*ppmeAktZeile, *ppmeAktZeile + 1,
	    sizeof(MNU_EINTRAG) * (pmeMenu + ucPos - *ppmeAktZeile));
  memmove(pmeMenu + ucPos, &mePufferZeile, sizeof(MNU_EINTRAG));
  *ppmeAktZeile = pmeMenu + ucPos;
  show_window(pmenuMenu);
  win_sw_a(2, 2 + ucPos, win_get_aktiv()->ucBreite - 4, 1,
	   AKT_PALETTE.ucDlgLstBoxActItem);
}

UCHAR auswahl (UINT *puiWahl, UCHAR *pucAktItem, UCHAR ucBewegen)
{

  MNU_EVENT    eventEvent;
  MNU_EINTRAG *pmeAktEintrag;
  UCHAR        ucItems,
	       ucAngegriffen = FALSE;
  ULONG        ulLastClick   = 0;

  pmeAktEintrag = pmenuAktMenu->ameEintrag + *pucAktItem;
  if (pmenuAktMenu->ucEintraege)
  {
    ucItems = TRUE;
    win_sw_a(2, 2 + *pucAktItem, win_get_aktiv()->ucBreite - 4, 1,
	     AKT_PALETTE.ucDlgLstBoxActItem);
    if (!ucBewegen)
      write_help(pmeAktEintrag->aucHelpLine);
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
      sts_event(HAUPTMENU, &eventEvent);
    switch (eventEvent.ucArt)
    {
      case EVENT_WIN_MOVE :
	pmenuAktMenu->ucChanged = TRUE;
	break;
      case EVENT_L_DOWN :
	if (IN_CLICK_FIELD)
	{
	  if (((eventEvent.ulTicks - ulLastClick) <= (uiDoubleClick / 55)) &&
	      (pmeAktEintrag == (pmenuAktMenu->ameEintrag +
	      eventEvent.uiVer - win_get_aktiv()->ucY - 2)) && ucItems)
	  {
	    *puiWahl = 0;
	    *pucAktItem = pmeAktEintrag - pmenuAktMenu->ameEintrag;
	    return(FALSE);
	  }
	  else
	  {
	    ucAngegriffen = TRUE;
	    ulLastClick = eventEvent.ulTicks;
	    win_sw_a(2, 2 + (pmeAktEintrag - pmenuAktMenu->ameEintrag),
		     win_get_aktiv()->ucBreite - 4, 1, AKT_PALETTE.ucDlg);
	    if (ucBewegen)
	      bewegen(pmenuAktMenu, &pmeAktEintrag,
		      eventEvent.uiVer - win_get_aktiv()->ucY - 2);
	    else
	    {
	      pmeAktEintrag = pmenuAktMenu->ameEintrag + eventEvent.uiVer -
			      win_get_aktiv()->ucY - 2;
	      win_sw_a(2, 2 + (pmeAktEintrag - pmenuAktMenu->ameEintrag),
		       win_get_aktiv()->ucBreite - 4, 1,
		       AKT_PALETTE.ucDlgLstBoxActItem);
	      if (ucItems)
		write_help(pmeAktEintrag->aucHelpLine);
            }
	  }
	}
	break;
      case EVENT_L_UP :
	ucAngegriffen = FALSE;
	break;
      case EVENT_WIN_CLOSE :
	if (pmenuAktMenu != pmenuHauptMenu)
	{
	  *puiWahl = 64000;
	  *pucAktItem = pmeAktEintrag - pmenuAktMenu->ameEintrag;
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
	    bewegen(pmenuAktMenu, &pmeAktEintrag,
		    eventEvent.uiVer - win_get_aktiv()->ucY - 2);
	  else
	  {
	    win_sw_a(2, 2 + (pmeAktEintrag - pmenuAktMenu->ameEintrag),
		     win_get_aktiv()->ucBreite - 4, 1, AKT_PALETTE.ucDlg);
	    pmeAktEintrag = pmenuAktMenu->ameEintrag + eventEvent.uiVer -
			    win_get_aktiv()->ucY - 2;
	    win_sw_a(2, 2 + (pmeAktEintrag - pmenuAktMenu->ameEintrag),
		     win_get_aktiv()->ucBreite - 4, 1,
		     AKT_PALETTE.ucDlgLstBoxActItem);
	    if (ucItems)
	      write_help(pmeAktEintrag->aucHelpLine);
	  }
	}
	break;
      case EVENT_WAHL :
	switch (eventEvent.uiWahl)
	{
	  case ID_OEFFNEN :
	    if (ucItems)
	    {
	      *puiWahl = 0;
	      *pucAktItem = pmeAktEintrag - pmenuAktMenu->ameEintrag;
	      return(FALSE);
	    }
	    break;
	  case ID_SCHLIESSEN :
	    *puiWahl = 64000;
	    *pucAktItem = pmeAktEintrag - pmenuAktMenu->ameEintrag;
	    return(FALSE);
	  default :
	    *puiWahl = eventEvent.uiWahl;
	    *pucAktItem = pmeAktEintrag - pmenuAktMenu->ameEintrag;
	    return(TRUE);
	}
	break;
      case EVENT_TASTE :
	switch (eventEvent.uiTaste)
	{
	  case '+' :
	    *puiWahl = ID_DAZUFUEGEN;
	    *pucAktItem = pmeAktEintrag - pmenuAktMenu->ameEintrag;
	    return(TRUE);
	  case T_INS :
	    *puiWahl = ID_EINFUEGEN;
	    *pucAktItem = pmeAktEintrag - pmenuAktMenu->ameEintrag;
	    return(TRUE);
	  case T_SPACE :
	    *puiWahl = ID_BEARBEITEN;
	    *pucAktItem = pmeAktEintrag - pmenuAktMenu->ameEintrag;
	    return(TRUE);
	  case T_DEL :
	    *puiWahl = ID_LOESCHEN;
	    *pucAktItem = pmeAktEintrag - pmenuAktMenu->ameEintrag;
	    return(TRUE);
	  case T_A_X :
	    *puiWahl = ID_ENDE;
	    *pucAktItem = pmeAktEintrag - pmenuAktMenu->ameEintrag;
	    return(TRUE);
	  case T_ESC :
	    if (ucBewegen)
	      break;
	    if (pmenuAktMenu != pmenuHauptMenu)
	    {
	      *puiWahl = 64000;
	      *pucAktItem = pmeAktEintrag - pmenuAktMenu->ameEintrag;
	      return(FALSE);
	    }
	    break;
	  case T_RETURN :
	    if (ucItems)
	    {
	      *puiWahl = 0;
	      *pucAktItem = pmeAktEintrag - pmenuAktMenu->ameEintrag;
	      return(FALSE);
	    }
	    break;
	  case T_UP :
	    if (!ucItems)
	    {
	      utl_beep();
	      break;
	    }
	    if (pmeAktEintrag != pmenuAktMenu->ameEintrag)
	    {
	      if (ucBewegen)
		bewegen(pmenuAktMenu, &pmeAktEintrag,
			pmeAktEintrag - pmenuAktMenu->ameEintrag - 1);
	      else
	      {
		win_sw_a(2, 2 + (pmeAktEintrag - pmenuAktMenu->ameEintrag),
			 win_get_aktiv()->ucBreite - 4, 1,
			 AKT_PALETTE.ucDlg);
		pmeAktEintrag--;
		win_sw_a(2, 2 + (pmeAktEintrag - pmenuAktMenu->ameEintrag),
			 win_get_aktiv()->ucBreite - 4, 1,
			 AKT_PALETTE.ucDlgLstBoxActItem);
		if (ucItems)
		  write_help(pmeAktEintrag->aucHelpLine);
	      }
	    }
	    else
	      utl_beep();
	    break;
	  case T_DOWN :
	    if (!ucItems)
	    {
	      utl_beep();
	      break;
	    }
	    if (pmeAktEintrag - pmenuAktMenu->ameEintrag + 1 <
		pmenuAktMenu->ucEintraege)
	    {
	      if (ucBewegen)
		bewegen(pmenuAktMenu, &pmeAktEintrag, pmeAktEintrag -
			pmenuAktMenu->ameEintrag + 1);
	      else
	      {
		win_sw_a(2, 2 + (pmeAktEintrag - pmenuAktMenu->ameEintrag),
			 win_get_aktiv()->ucBreite - 4, 1,
			 AKT_PALETTE.ucDlg);
		pmeAktEintrag++;
		win_sw_a(2, 2 + (pmeAktEintrag - pmenuAktMenu->ameEintrag),
			 win_get_aktiv()->ucBreite - 4, 1,
			 AKT_PALETTE.ucDlgLstBoxActItem);
		if (ucItems)
		  write_help(pmeAktEintrag->aucHelpLine);
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
	       ucShorts,
	      *pucGrpName           = " Name der Gruppe",
	      *pucHilfe             = " Kurzbeschreibung der Gruppe",
	      *pucGrpDat            = " Dateiname der Gruppe",
	      *pucOK                = " Best„tigt die eingegebenen Werte",
	      *pucAbbruch           = " Bricht die Aktion ab";
  WINDOW       winWindow;
  DLG_ELEMENT *apelementElemente[6];

  ucShorts = utl_short_cuts(FALSE);
  winWindow = win_einrichten(13, 7, 53, 10);
  win_titel(winWindow, pucTitel, TITEL_O_Z);
  apelementElemente[0] = dlg_init_text_field(3, 2, 32, 60, "#Name        ",
					     pucName, pucGrpName);
  apelementElemente[1] = dlg_init_text_field(3, 3, 32, 78, "#Beschreibung",
					     pucBeschreibung, pucHilfe);
  apelementElemente[2] = dlg_init_text_field(3, 5, 8, 8, "#Gruppendatei",
					     pucDatei, pucGrpDat);
  apelementElemente[3] = dlg_init_act_button(3, 8, "OK", T_RETURN, BOX_OK,
					     pucOK);
  apelementElemente[4] = dlg_init_act_button(11, 8, "Abbruch", T_ESC,
					     BOX_ABBRUCH, pucAbbruch);
  apelementElemente[5] = NULL;
  win_aktivieren(winWindow);
  win_sw_za(1, 7, 51, 1, 'Ä', AKT_PALETTE.ucDlgBorder);
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
  apelementElemente[2] = dlg_init_text_field(3, 5, 32, 130, "#Verzeichnis ",
					     pucWorkDir, pucVerzeichnis);
  apelementElemente[3] = dlg_init_text_field(3, 6, 32, 130, "#Aufruf      ",
					     pucDatei, pucHAufruf);
  apelementElemente[4] = dlg_init_act_button(3, 9, "OK", T_RETURN, BOX_OK,
					     pucOK);
  apelementElemente[5] = dlg_init_act_button(11, 9, "Abbruch", T_ESC,
					     BOX_ABBRUCH, pucAbbruch);
  apelementElemente[6] = dlg_init_act_button(24, 9, "#Durchsuchen", 0,
					     BOX_SEARCH, pucSearch);
  apelementElemente[7] = NULL;
  win_aktivieren(winWindow);
  win_sw_za(1, 8, 51, 1, 'Ä', AKT_PALETTE.ucDlgBorder);
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
  win_sw_za(1, 4, 61, 1, 'Ä', AKT_PALETTE.ucDlgBorder);
  switch (dlg_aktivieren(apelementElemente))
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

void del_gruppe (UCHAR *pucName)
{

  UCHAR aucName[13];

  strcpy(aucName, pucName);
  strcat(aucName, pucMnuExtension);
  remove(aucName);
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
  DLG_ELEMENT *apelementElemente[8];
  WINDOW       winWindow;
  UCHAR       *pucUhr               = " Angabe, ob die Uhr in der rechten oberen Ecke aktiv ist",
	      *pucLastLine          = " Verwendung der letzten Zeile",
              *pucTaeglich          = " Angabe, ob t„glich die Batchdatei TAG.BAT gestartet werden soll",
              *pucWoechentlich      = " Angabe, ob w”chentlich die Batchdatei WOCHE.BAT gestartet werden soll",
              *pucMonatlich         = " Angabe, ob monatlich die Batchdatei MONAT.BAT gestartet werden soll",
	      *pucOK                = " šbernimmt die eingestellten Werte",
	      *pucAbbruch           = " Bel„át die alten Werte",
	       ucMouse;

  winWindow = win_einrichten(20, 5, 40, 15);
  win_titel(winWindow, " Sonstige Einstellungen ", TITEL_O_Z);
  apelementElemente[0] = dlg_init_push_button(3, 2, "#Uhr in der rechten oberen Ecke",
					      ucUhr, pucUhr);
  apelementElemente[1] = dlg_init_radio_button(abutButtons, ucButtonBar,
					       pucLastLine);
  apelementElemente[2] = dlg_init_push_button(3, 8, "Batchdatei #t„glich",
                                              ucTaeglich, pucTaeglich);
  apelementElemente[3] = dlg_init_push_button(3, 9, "Batchdatei #w”chentlich",
                                              ucWoechentlich,
                                              pucWoechentlich);
  apelementElemente[4] = dlg_init_push_button(3, 10, "Batchdatei #monatlich",
                                              ucMonatlich, pucMonatlich);
  apelementElemente[5] = dlg_init_act_button(3, 13, "OK", T_RETURN, BOX_OK,
					     pucOK);
  apelementElemente[6] = dlg_init_act_button(11, 13, "Abbruch", T_ESC,
					     BOX_ABBRUCH, pucAbbruch);
  apelementElemente[7] = NULL;
  win_aktivieren(winWindow);
  win_ss(3, 4, "Letzte Zeile als");
  win_sw_za(1, 12, 38, 1, 'Ä', AKT_PALETTE.ucDlgBorder);
  switch (dlg_aktivieren(apelementElemente))
  {
    case BOX_OK :
      ucUhr = dlg_ask_push_button(apelementElemente[0]);
      ucTaeglich = dlg_ask_push_button(apelementElemente[2]);
      ucWoechentlich = dlg_ask_push_button(apelementElemente[3]);
      ucMonatlich = dlg_ask_push_button(apelementElemente[4]);
      if (!ucUhr)
      {
	ucMouse = msm_cursor_off();
	vio_sw_z(73, 1, 8, 1, T_SPACE);
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

void ende (void)
{
  write_config();
  save_menus();
  exit(0);
}

main()
{

  UCHAR ucDummy;

  _fmode = O_BINARY;
  read_config();
  init_haupt_menu();
  init_screen();
  run();
  return(0);
}
