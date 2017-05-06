/***************************************************************************
 ***************************************************************************
 ***                                                                     ***
 ***                             SE-Menu                                 ***
 ***                                                                     ***
 ***                         Utility-Routinen                            ***
 ***                                                                     ***
 ***                (c) 1990-92 by Schani Electronics                    ***
 ***                                                                     ***
 ***************************************************************************
 ***************************************************************************/

#define NO_STARTUP
#include <stools.h>
#include <string.h>
#include <stdlib.h>
#include <dos.h>
#include "menu.h"

extern _MNU_MENU *pmenuHauptMenu;
extern MNU_USER   userAktUser;
extern UINT       uiUsers;

UCHAR aucDaysPerMonth[] = {
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

void save_menus (void)
{

  _MNU_MENU *pmenuMenu;

  for (pmenuMenu = pmenuHauptMenu; pmenuMenu;
       pmenuMenu = pmenuMenu->pmenuUnterMenu)
    if (pmenuMenu->ucChanged)
      write_menu(pmenuMenu);
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

INT datecmp (MNU_DATE *pdt1, MNU_DATE *pdt2)
{
  if (pdt1->uiYear > pdt2->uiYear)
    return(1);
  else
  {
    if (pdt1->uiYear == pdt2->uiYear)
    {
      if (pdt1->ucMonth > pdt2->ucMonth)
        return(1);
      else
      {
        if (pdt1->ucMonth == pdt2->ucMonth)
        {
          if (pdt1->ucDay > pdt2->ucDay)
            return(1);
          else
          {
            if (pdt1->ucDay == pdt2->ucDay)
            {
              if (pdt1->ucHour > pdt2->ucHour)
                return(1);
              else
              {
                if (pdt1->ucHour == pdt2->ucHour)
                {
                  if (pdt1->ucMinute > pdt2->ucMinute)
                    return(1);
                  else
                  {
                    if (pdt1->ucMinute == pdt2->ucMinute)
                      return(0);
                    else
                      return(-1);
                  }
                }
                else
                  return(-1);
              }
            }
            else
              return(-1);
          }
        }
        else
          return(-1);
      }
    }
    else
      return(-1);
  }
}

UINT list_length (MNU_TERMIN *ptrmTermin)
{

  UINT uiCounter = 0;

  for (; ptrmTermin; ptrmTermin = ptrmTermin->ptrmNext)
    uiCounter++;
  return(uiCounter);
}

void date_to_string (MNU_DATE *pdtDate, CHAR *pcString)
{

  CHAR acBuffer[10];

  strcpy(pcString, "");
  itoa((INT)pdtDate->ucDay, acBuffer, 10);
  if (strlen(acBuffer) == 1)
    strcat(pcString, "0");
  strcat(pcString, acBuffer);
  strcat(pcString, ".");
  itoa((INT)pdtDate->ucMonth, acBuffer, 10);
  if (strlen(acBuffer) == 1)
    strcat(pcString, "0");
  strcat(pcString, acBuffer);
  strcat(pcString, ".");
  itoa((INT)pdtDate->uiYear, acBuffer, 10);
  strcat(pcString, acBuffer);
}

UCHAR string_to_date (MNU_DATE *pdtDate, CHAR *pcString)
{

  UCHAR ucDay,
        ucMonth;
  UINT  uiYear;
  CHAR  acTemp[5];

  if (strlen(pcString) != 10)
    return(FALSE);
  strncpy(acTemp, pcString, 2);
  ucDay = atoi(acTemp);
  strncpy(acTemp, pcString + 3, 2);
  ucMonth = atoi(acTemp);
  strncpy(acTemp, pcString + 6, 4);
  uiYear = atoi(acTemp);
  if (!ucMonth || ucMonth > 12)
    return(FALSE);
  if (!ucDay || ucDay > aucDaysPerMonth[ucMonth - 1])
    return(FALSE);
  pdtDate->ucDay = ucDay;
  pdtDate->ucMonth = ucMonth;
  pdtDate->uiYear = uiYear;
  return(TRUE);
}

void time_to_string (MNU_DATE *pdtDate, CHAR *pcString)
{

  CHAR acBuffer[10];

  strcpy(pcString, "");
  itoa((INT)pdtDate->ucHour, acBuffer, 10);
  if (strlen(acBuffer) == 1)
    strcat(pcString, "0");
  strcat(pcString, acBuffer);
  strcat(pcString, ":");
  itoa((INT)pdtDate->ucMinute, acBuffer, 10);
  if (strlen(acBuffer) == 1)
    strcat(pcString, "0");
  strcat(pcString, acBuffer);
}

UCHAR string_to_time (MNU_DATE *pdtDate, CHAR *pcString)
{

  UCHAR ucHour,
        ucMinute;
  CHAR  acTemp[3];

  if (strlen(pcString) != 5)
    return(FALSE);
  strncpy(acTemp, pcString, 2);
  ucHour = atoi(acTemp);
  strncpy(acTemp, pcString + 3, 2);
  ucMinute = atoi(acTemp);
  if (ucHour > 23)
    return(FALSE);
  if (ucMinute > 59)
    return(FALSE);
  pdtDate->ucHour = ucHour;
  pdtDate->ucMinute = ucMinute;
  return(TRUE);
}

CHAR** list_to_array (MNU_TERMIN *ptrmTermin)
{

  UINT   uiCounter    = 0;
  CHAR **ppcReturnVar;
  CHAR   acBuffer[11];

  if (list_length(ptrmTermin) == 0)
  {
    ppcReturnVar = utl_alloc(sizeof(CHAR*) * 2);
    ppcReturnVar[0] = utl_alloc(2);
    strcpy(ppcReturnVar[0], " ");
    ppcReturnVar[1] = NULL;
    return(ppcReturnVar);
  }
  ppcReturnVar = utl_alloc(sizeof(CHAR*) * (list_length(ptrmTermin) + 1));
  for (; ptrmTermin; ptrmTermin = ptrmTermin->ptrmNext)
  {
    ppcReturnVar[uiCounter] = utl_alloc(41);
    strncpy(ppcReturnVar[uiCounter], ptrmTermin->acText, 24);
    while (strlen(ppcReturnVar[uiCounter]) < 25)
      strcat(ppcReturnVar[uiCounter], " ");
    date_to_string(&(ptrmTermin->dtDate), acBuffer);
    strcat(ppcReturnVar[uiCounter], acBuffer);
    strcat(ppcReturnVar[uiCounter], " ");
    time_to_string(&(ptrmTermin->dtDate), acBuffer);
    strcat(ppcReturnVar[uiCounter], acBuffer);
    uiCounter++;
  }
  ppcReturnVar[uiCounter] = NULL;
  return(ppcReturnVar);
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
  if (!dAktDate.dayofweek)
    dAktDate.dayofweek = 6;
  else
    dAktDate.dayofweek--;
  if (pdDate->year == dAktDate.year)
  {
    if (pdDate->month == dAktDate.month)
    {
      if (pdDate->day == dAktDate.day)
        return(TRUE);
      return((dAktDate.day - dAktDate.dayofweek) <= pdDate->day);
    }
    if ((dAktDate.month - pdDate->month) > 1)
      return(FALSE);
    return((aucDaysPerMonth[pdDate->month - 1] - pdDate->day + dAktDate.day)
           <= dAktDate.dayofweek);
  }
  if (pdDate->month < 12 || dAktDate.month > 1)
    return(FALSE);
  return((aucDaysPerMonth[pdDate->month - 1] - pdDate->day + dAktDate.day)
         <= dAktDate.dayofweek);
}

UCHAR this_month (struct dosdate_t *pdDate)
{

  struct dosdate_t dAktDate;

  _dos_getdate(&dAktDate);
  return(pdDate->month == dAktDate.month &&
         pdDate->year == dAktDate.year);
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

void get_items (void)
{

  UCHAR      ucCounter = 0;
  _MNU_MENU *pmenuMenu;

  for (pmenuMenu = pmenuHauptMenu; pmenuMenu->pmenuUnterMenu;
       pmenuMenu = pmenuMenu->pmenuUnterMenu, ucCounter++)
    userAktUser.aucItems[ucCounter] = pmenuMenu->ucAktItem;
  userAktUser.aucItems[ucCounter] = 0xff;
}

void bewegen (_MNU_MENU *pmenuMenu, MNU_EINTRAG **ppmeAktZeile, UCHAR ucPos)
{

  MNU_EINTRAG  mePufferZeile,
	      *pmeMenu,
	      *pmeZaehler;

  pmeMenu = pmenuMenu->ameEintrag;
  if (pmeMenu + ucPos == *ppmeAktZeile)
  {
    win_sw_a(NULL, 2,
             2 + (*ppmeAktZeile - pmeMenu), win_get_aktiv()->uiWidth - 4,
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
  win_sw_a(NULL, 2, 2 + ucPos, win_get_aktiv()->uiWidth - 4, 1,
	   AKT_PALETTE.ucDlgLstBoxActItem);
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

void user_list (UCHAR ***pppucUser)
{

  UINT     uiCounter;
  MNU_USER userUser;

  *pppucUser = utl_alloc((uiUsers + 1) * sizeof(UCHAR*));
  for (uiCounter = 0; uiCounter < uiUsers; uiCounter++)
  {
    read_user_info(uiCounter, &userUser);
    (*pppucUser)[uiCounter] = utl_alloc(strlen(userUser.aucName) + 1);
    strcpy((*pppucUser)[uiCounter], userUser.aucName);
  }
  (*pppucUser)[uiUsers] = NULL;
}
