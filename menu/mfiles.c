/***************************************************************************
 ***************************************************************************
 ***                                                                     ***
 ***                             SE-Menu                                 ***
 ***                                                                     ***
 ***                        Dateimanipulation                            ***
 ***                                                                     ***
 ***                (c) 1990-92 by Schani Electronics                    ***
 ***                                                                     ***
 ***************************************************************************
 ***************************************************************************/

#define NO_STARTUP
#include <stools.h>
#include <stdio.h>
#include <string.h>
#include "menu.h"

extern UCHAR  ucUhr,
              ucButtonBar,
              ucTaeglich,
              ucWoechentlich,
              ucMonatlich,
              ucRememberList,
              ucTerminWarning;
extern UCHAR *pucLeer,
              aucDaysPerMonth[];
extern UINT   uiUserID,
              uiUsers;
extern CHAR   acBeepVOC[128],
              acStartVOC[128],
              acLogoutVOC[128];

UCHAR        *pucHauptDatei     = "SEMENU.MNU",
             *pucUserDatei      = "SEMENU.USR",
             *pucConfigDatei    = "SEMENU.CFG",
             *pucMessageDatei   = "SEMENU.MSG",
             *pucTerminDatei    = "SEMENU.TRM",
             *pucMemoDatei      = "SEMENU.MEM",
             *pucAll            = "ALLE",
             *pucTag            = "CALL TAG.BAT",
             *pucWoche          = "CALL WOCHE.BAT",
             *pucMonat          = "CALL MONAT.BAT";
FILE         *pfileMenu         = NULL,
             *pfileUsers,
             *pfileMessage,
             *pfileTermin,
             *pfileMemo;

void decrypt_password (UCHAR *pucPassword)
{

  UCHAR ucCounter;

  for (ucCounter = 0; ucCounter < 21; ucCounter++)
    pucPassword[ucCounter] = rotate_left(pucPassword[ucCounter], 4) ^ 85;
}

void encrypt_password (UCHAR *pucPassword)
{

  UCHAR ucCounter;

  for (ucCounter = 0; ucCounter < 21; ucCounter++)
    pucPassword[ucCounter] = rotate_left(pucPassword[ucCounter] ^ 85, 4);
}

UCHAR rotate_left (UCHAR ucBitmap, UCHAR ucBits)
{

  UCHAR ucCounter;

  for (ucCounter = 0; ucCounter < ucBits; ucCounter++)
    ucBitmap = (ucBitmap << 1) | ((ucBitmap & 0x80) >> 7);
  return(ucBitmap);
}

UINT get_next_free (void)
{

  UCHAR ucBitmap;
  UINT  uiCounter = 0;

  fseek(pfileMenu, 2049, SEEK_SET);
  while (((ucBitmap = fgetc(pfileMenu)) == 0xff) && uiCounter < 8192)
    uiCounter++;
  if (uiCounter == 8192)
    return(0);
  uiCounter *= 8;
  while (ucBitmap & 1)
  {
    uiCounter++;
    ucBitmap >>= 1;
  }
  return(uiCounter + 1);
}

void set_usage (UINT uiEntry, UCHAR ucStatus)
{

  UCHAR ucBitmap;

  uiEntry--;
  if (ucStatus)
    ucStatus = 1;
  fseek(pfileMenu, 2049 + uiEntry / 8, SEEK_SET);
  ucBitmap = fgetc(pfileMenu);
  ucBitmap &= ~(1 << (uiEntry % 8));
  ucBitmap |= ucStatus << (uiEntry % 8);
  fseek(pfileMenu, 2049 + uiEntry / 8, SEEK_SET);
  fputc(ucBitmap, pfileMenu);
}

void set_menu (UINT uiUser, UINT uiEntry)
{
  fseek(pfileMenu, 1 + uiUser * sizeof(UINT), SEEK_SET);
  fwrite(&uiEntry, sizeof(UINT), 1, pfileMenu);
}

UCHAR entry_used (UINT uiEntry)
{
  uiEntry--;
  fseek(pfileMenu, 2049 + uiEntry / 8, SEEK_SET);
  return((fgetc(pfileMenu) >> (uiEntry % 8)) & 1);
}

void read_entry (UINT uiEntry, void *pEntry)
{
  fseek(pfileMenu, 10241 + (uiEntry - 1) * sizeof(MNU_DISK_EINTRAG),
        SEEK_SET);
  fread(pEntry, sizeof(MNU_DISK_EINTRAG), 1, pfileMenu);
}

void write_entry (UINT uiEntry, void *pEntry)
{
  fseek(pfileMenu, 10241 + (uiEntry - 1) * sizeof(MNU_DISK_EINTRAG),
        SEEK_SET);
  fwrite(pEntry, sizeof(MNU_DISK_EINTRAG), 1, pfileMenu);
  set_usage(uiEntry, TRUE);
}

void read_config (void)
{

  FILE             *pfileDatei;
  fpos_t            pPos;
  struct dosdate_t  dLastDate,
                    dAktDate;

  if (!(pfileDatei = fopen(pucConfigDatei, "r+")))
    write_config();
  else
  {
    _dos_getdate(&dAktDate);
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
      utl_cls(vio_attri(7, 0));
      vio_set_cursor_pos(1, 1);
      printf("Konfigurationsdatei kann nicht geîffnet werden\n");
      utl_get_key();
      exit(1);
    }
    ucStartup = TRUE;
  }
  _dos_getdate(&dDate);
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

void read_menu (UINT uiGruppe, _MNU_MENU* pmenuOwner, _MNU_MENU *pmenuMenu)
{

  MNU_DISK_MENU    dmenuMenu;
  MNU_DISK_EINTRAG dmeEintrag;
  UINT             uiNext,
                   ucCounter  = 0;

  pmenuMenu->uiGruppe = uiGruppe;
  read_entry(uiGruppe, &dmenuMenu);
  uiNext = dmenuMenu.uiFirst;
  while (uiNext)
  {
    read_entry(uiNext, &dmeEintrag);
    strcpy(pmenuMenu->ameEintrag[ucCounter].aucName, dmeEintrag.aucName);
    strcpy(pmenuMenu->ameEintrag[ucCounter].aucHelpLine,
           dmeEintrag.aucHelpLine);
    pmenuMenu->ameEintrag[ucCounter].ucProgramm = dmeEintrag.ucProgramm;
    if (dmeEintrag.ucProgramm)
    {
      strcpy(pmenuMenu->ameEintrag[ucCounter].aucWorkDir,
             dmeEintrag.aucWorkDir);
      strcpy(pmenuMenu->ameEintrag[ucCounter].aucCmdLine,
             dmeEintrag.aucCmdLine);
    }
    else
      pmenuMenu->ameEintrag[ucCounter].uiGruppe = dmeEintrag.uiGruppe;
    uiNext = dmeEintrag.uiNext;
    ucCounter++;
  }
  pmenuMenu->ucEintraege = ucCounter;
  pmenuMenu->ucChanged = FALSE;
  pmenuMenu->pmenuOberMenu = pmenuOwner;
  pmenuMenu->pmenuUnterMenu = NULL;
  pmenuMenu->ucAktItem = 0;
  make_window(pmenuMenu, dmenuMenu.cWinX, dmenuMenu.cWinY,
              dmenuMenu.aucTitel);
}

void write_menu (_MNU_MENU *pmenuMenu)
{

  MNU_DISK_MENU    dmenuMenu;
  MNU_DISK_EINTRAG dmeEintrag;
  UINT             uiNext,
                   uiRealNext = 0;
  UCHAR            ucCounter  = 0,
                   ucNextNull = FALSE;

  if (entry_used(pmenuMenu->uiGruppe))
  {
    read_entry(pmenuMenu->uiGruppe, &dmenuMenu);
    uiNext = dmenuMenu.uiFirst;
  }
  else
    uiNext = 0;
  dmenuMenu.cWinX = pmenuMenu->winWindow->iX;
  dmenuMenu.cWinY = pmenuMenu->winWindow->iY;
  strcpy(dmenuMenu.aucTitel, pmenuMenu->winWindow->pcTitle);
  if (!pmenuMenu->ucEintraege)
    dmenuMenu.uiFirst = 0;
  if (pmenuMenu->ucEintraege && !dmenuMenu.uiFirst)
    dmenuMenu.uiFirst = get_next_free();
  write_entry(pmenuMenu->uiGruppe, &dmenuMenu);
  dmeEintrag.uiNext = uiNext;
  while (ucCounter < pmenuMenu->ucEintraege)
  {
    if (!dmeEintrag.uiNext)
    {
      uiNext = get_next_free();
      set_usage(uiNext, TRUE);
      dmeEintrag.uiNext = get_next_free();
      ucNextNull = TRUE;
    }
    else
    {
      uiNext = dmeEintrag.uiNext;
      read_entry(uiNext, &dmeEintrag);
      if (!dmeEintrag.uiNext)
      {
        dmeEintrag.uiNext = get_next_free();
        ucNextNull = TRUE;
      }
    }
    strcpy(dmeEintrag.aucName, pmenuMenu->ameEintrag[ucCounter].aucName);
    strcpy(dmeEintrag.aucHelpLine,
           pmenuMenu->ameEintrag[ucCounter].aucHelpLine);
    dmeEintrag.ucProgramm = pmenuMenu->ameEintrag[ucCounter].ucProgramm;
    if (dmeEintrag.ucProgramm)
    {
      strcpy(dmeEintrag.aucWorkDir,
             pmenuMenu->ameEintrag[ucCounter].aucWorkDir);
      strcpy(dmeEintrag.aucCmdLine,
             pmenuMenu->ameEintrag[ucCounter].aucCmdLine);
    }
    else
      dmeEintrag.uiGruppe = pmenuMenu->ameEintrag[ucCounter].uiGruppe;
    if ((ucCounter + 1) == pmenuMenu->ucEintraege)
    {
      if (entry_used(dmeEintrag.uiNext) && dmeEintrag.uiNext)
        uiRealNext = dmeEintrag.uiNext;
      else
        uiRealNext = 0;
      dmeEintrag.uiNext = 0;
    }
    write_entry(uiNext, &dmeEintrag);
    if (ucNextNull)
    {
      dmeEintrag.uiNext = 0;
      ucNextNull = FALSE;
    }
    ucCounter++;
  }
  pmenuMenu->ucChanged = FALSE;
  while (uiRealNext)
  {
    read_entry(uiRealNext, &dmeEintrag);
    set_usage(uiRealNext, FALSE);
    uiRealNext = dmeEintrag.uiNext;
  }
}

void del_gruppe (UINT uiGruppe)
{

  MNU_DISK_MENU    dmenuMenu;
  MNU_DISK_EINTRAG dmeEintrag;
  UINT             uiNext;

  read_entry(uiGruppe, &dmenuMenu);
  set_usage(uiGruppe, FALSE);
  uiNext = dmenuMenu.uiFirst;
  while (uiNext)
  {
    read_entry(uiNext, &dmeEintrag);
    if (!dmeEintrag.ucProgramm)
      del_gruppe(dmeEintrag.uiGruppe);
    set_usage(uiNext, FALSE);
    uiNext = dmeEintrag.uiNext;
  }
}

UINT open_menu_file (UINT uiUser)
{

  UINT          uiCounter,
                uiHauptMenu;
  MNU_DISK_MENU dmenuMenu =
                            {
                              0,
                              " HauptmenÅ ",
                              10,
                              5,
                              ""
                            };

  if (pfileMenu)
    fclose(pfileMenu);
  if (!(pfileMenu = fopen(pucHauptDatei, "r+")))
  {
    if (!(pfileMenu = fopen(pucHauptDatei, "w+")))
    {
      utl_cls(vio_attri(7, 0));
      vio_set_cursor_pos(1, 1);
      printf("MenÅdatei kann nicht geîffnet werden\n");
      utl_get_key();
      exit(1);
    }
    fputc(1, pfileMenu);
    fputc(1, pfileMenu);
    for (uiCounter = 0; uiCounter < 2047; uiCounter++)
      fputc(0, pfileMenu);
    fputc(1, pfileMenu);
    for (uiCounter = 0; uiCounter < 8191; uiCounter++)
      fputc(0, pfileMenu);
    fwrite(&dmenuMenu, sizeof(MNU_DISK_MENU), 1, pfileMenu);
    return(1);
  }
  else
  {
    if (fgetc(pfileMenu) != 1)
    {
      utl_cls(vio_attri(7, 0));
      vio_set_cursor_pos(1, 1);
      printf("Falsche Version des Dateiformates\n");
      utl_get_key();
      exit(1);
    }
  }
  fseek(pfileMenu, uiUser * 2 + 1, SEEK_SET);
  fread(&uiHauptMenu, sizeof(UINT), 1, pfileMenu);
  return(uiHauptMenu);
}

UCHAR get_user (UCHAR *pucName, UINT *puiUser)
{

  UCHAR    *pucNameGross;
  UINT      uiCounter,
            uiUsers;
  MNU_USER  userUser;

  pucNameGross = utl_alloc(strlen(pucName) + 1);
  utl_str_upper(strcpy(pucNameGross, pucName));
  fseek(pfileUsers, 0, SEEK_SET);
  fread(&uiUsers, sizeof(UINT), 1, pfileUsers);
  for (uiCounter = 0; uiCounter < uiUsers; uiCounter++)
  {
    fseek(pfileUsers, 2 + USER_SIZE * uiCounter, SEEK_SET);
    fread(&userUser, sizeof(MNU_USER), 1, pfileUsers);
    if (!strcmp(pucNameGross, userUser.aucName))
    {
      utl_free(pucNameGross);
      *puiUser = uiCounter;
      return(TRUE);
    }
  }
  utl_free(pucNameGross);
  return(FALSE);
}

UCHAR password_right (UINT uiUser, UCHAR *pucPassword)
{

  UCHAR     ucCounter;
  MNU_USER  userUser;
  UCHAR    *pucPasswordGross;

  pucPasswordGross = utl_alloc(strlen(pucPassword) + 1);
  utl_str_upper(strcpy(pucPasswordGross, pucPassword));
  read_user_info(uiUser, &userUser);
  decrypt_password(userUser.aucPassword);
  if (!strcmp(pucPasswordGross, userUser.aucPassword))
    return(TRUE);
  return(FALSE);
}

UCHAR has_password (UINT uiUser)
{

  MNU_USER userUser;

  read_user_info(uiUser, &userUser);
  decrypt_password(userUser.aucPassword);
  return(strcmp(userUser.aucPassword, pucLeer));
}

void read_user_info (UINT uiUser, MNU_USER *puserUser)
{
  fseek(pfileUsers, 2 + USER_SIZE * uiUser, SEEK_SET);
  fread(puserUser, sizeof(MNU_USER), 1, pfileUsers);
}

UCHAR read_user (UINT uiUser, MNU_USER *puserUser)
{

  CHAR        acVOC[128];
  struct date dateDate;
  UCHAR       ucDay,
              ucMonth;
  UINT        uiYear;

  read_user_info(uiUser, puserUser);
  fread(&bBeep, sizeof(BOOL), 1, pfileUsers);
  fread(&ulBeepDuration, sizeof(ULONG), 1, pfileUsers);
  fread(&ulBeepFrequency, sizeof(ULONG), 1, pfileUsers);
  fread(&ucActPalette, sizeof(UCHAR), 1, pfileUsers);
  fread(&uiMouseSpeed, sizeof(UINT), 1, pfileUsers);
  fread(&ulDoubleClick, sizeof(ULONG), 1, pfileUsers);
  fread(&ucUhr, sizeof(UCHAR), 1, pfileUsers);
  fread(&ucButtonBar, sizeof(UCHAR), 1, pfileUsers);
  //fread(aapcPalColors + PAL_USER_DEFINED, PAL_COLORS, 1, pfileUsers);
  fread(&bSaver, sizeof(BOOL), 1, pfileUsers);
  fread(&ulScreenTimeout, sizeof(ULONG), 1, pfileUsers);
  fread(acStartVOC, 128, 1, pfileUsers);
  fread(acLogoutVOC, 128, 1, pfileUsers);
  fread(&ucRememberList, sizeof(UCHAR), 1, pfileUsers);
  fread(&ucTerminWarning, sizeof(UCHAR), 1, pfileUsers);
  fread(&ucDay, sizeof(UCHAR), 1, pfileUsers);
  fread(&ucMonth, sizeof(UCHAR), 1, pfileUsers);
  fread(&uiYear, sizeof(UINT), 1, pfileUsers);
  get_local_date(&dateDate);
  if (ucDay != dateDate.da_day || ucMonth != dateDate.da_mon ||
      uiYear != dateDate.da_year)
    return(TRUE);
  return(FALSE);
}

void write_user_info (UINT uiUser, MNU_USER *puserUser)
{
  if (uiUser == uiUsers)
  {
    uiUsers++;
    fseek(pfileUsers, 0, SEEK_SET);
    fwrite(&uiUsers, sizeof(UINT), 1, pfileUsers);
  }
  fseek(pfileUsers, 2 + USER_SIZE * uiUser, SEEK_SET);
  fwrite(puserUser, sizeof(MNU_USER), 1, pfileUsers);
}

void write_user (UINT uiUser, MNU_USER *puserUser)
{

  struct date dateDate;

  write_user_info(uiUser, puserUser);
  fwrite(&bBeep, sizeof(BOOL), 1, pfileUsers);
  fwrite(&ulBeepDuration, sizeof(ULONG), 1, pfileUsers);
  fwrite(&ulBeepFrequency, sizeof(ULONG), 1, pfileUsers);
  fwrite(&ucActPalette, sizeof(UCHAR), 1, pfileUsers);
  fwrite(&uiMouseSpeed, sizeof(UINT), 1, pfileUsers);
  fwrite(&ulDoubleClick, sizeof(ULONG), 1, pfileUsers);
  fwrite(&ucUhr, sizeof(UCHAR), 1, pfileUsers);
  fwrite(&ucButtonBar, sizeof(UCHAR), 1, pfileUsers);
  //fwrite(apalPalette + P_USER_DEFINED, sizeof(PALETTE), 1, pfileUsers);
  fwrite(&bSaver, sizeof(BOOL), 1, pfileUsers);
  fwrite(&ulScreenTimeout, sizeof(ULONG), 1, pfileUsers);
  fwrite(acStartVOC, 128, 1, pfileUsers);
  fwrite(acLogoutVOC, 128, 1, pfileUsers);
  fwrite(&ucRememberList, sizeof(UCHAR), 1, pfileUsers);
  fwrite(&ucTerminWarning, sizeof(UCHAR), 1, pfileUsers);
  get_local_date(&dateDate);
  fwrite(&(dateDate.da_day), sizeof(UCHAR), 1, pfileUsers);
  fwrite(&(dateDate.da_mon), sizeof(UCHAR), 1, pfileUsers);
  fwrite(&(dateDate.da_year), sizeof(UINT), 1, pfileUsers);
}

UINT get_menu (UINT uiUser)
{

  UINT uiGruppe;

  fseek(pfileMenu, 1 + uiUser * sizeof(UINT), SEEK_SET);
  fread(&uiGruppe, sizeof(UINT), 1, pfileMenu);
  return(uiGruppe);
}

UINT copy_menu (UINT uiMenu)
{

  MNU_DISK_MENU    dmenuMenu;
  MNU_DISK_EINTRAG dmeEintrag;
  UINT             uiFirst,
                   uiNext,
                   uiEntry;

  read_entry(uiMenu, &dmenuMenu);
  uiMenu = get_next_free();
  set_usage(uiMenu, TRUE);
  uiFirst = dmenuMenu.uiFirst;
  if (uiFirst)
  {
    dmenuMenu.uiFirst = get_next_free();
    for (uiNext = uiFirst; uiNext; )
    {
      read_entry(uiNext, &dmeEintrag);
      uiEntry = get_next_free();
      set_usage(uiEntry, TRUE);
      uiNext = dmeEintrag.uiNext;
      if (!dmeEintrag.ucProgramm)
        dmeEintrag.uiGruppe = copy_menu(dmeEintrag.uiGruppe);
      if (uiNext)
        dmeEintrag.uiNext = get_next_free();
      write_entry(uiEntry, &dmeEintrag);
    }
  }
  write_entry(uiMenu, &dmenuMenu);
  return(uiMenu);
}

void delete_menu (UINT uiUser)
{

  UINT uiGruppe;

  fseek(pfileMenu, 1 + uiUser * sizeof(UINT), SEEK_SET);
  fread(&uiGruppe, sizeof(UINT), 1, pfileMenu);
  del_gruppe(uiGruppe);
}

void delete_user (UINT uiUser)
{

  UCHAR aucUser[USER_SIZE];
  UINT  uiCounter;

  delete_menu(uiUser);
  for (uiCounter = uiUser + 1; uiCounter < uiUsers; uiCounter++)
  {
    fseek(pfileUsers, 2 + uiCounter * USER_SIZE, SEEK_SET);
    fread(aucUser, USER_SIZE, 1, pfileUsers);
    fseek(pfileUsers, 2 + (uiCounter - 1) * USER_SIZE, SEEK_SET);
    fwrite(aucUser, USER_SIZE, 1, pfileUsers);
  }
  uiUsers--;
  fseek(pfileUsers, 0, SEEK_SET);
  fwrite(&uiUsers, sizeof(UINT), 1, pfileUsers);
}

UINT open_user_file (void)
{

  MNU_USER userUser;
  UCHAR    ucDummy  = 0,
           uiDummy  = 0;
  UINT     uiUsers  = 1;

  if (!(pfileUsers = fopen(pucUserDatei, "r+")))
  {
    if (!(pfileUsers = fopen(pucUserDatei, "w+")))
    {
      utl_cls(vio_attri(7, 0));
      vio_set_cursor_pos(1, 1);
      printf("Userdatei kann nicht geîffnet werden\n");
      utl_get_key();
      exit(1);
    }
    strcpy(userUser.aucName, "SUPERVISOR");
    userUser.aucPassword[0] = 85;
    userUser.aucItems[0] = 0xff;
    userUser.uiRights = RIGHT_SUPERVISOR | RIGHT_CHANGE | RIGHT_PWD_CHANGE |
                        RIGHT_QUIT;
    fwrite(&uiUsers, sizeof(UINT), 1, pfileUsers);
    fwrite(&userUser, sizeof(MNU_USER), 1, pfileUsers);
    fwrite(&bBeep, sizeof(BOOL), 1, pfileUsers);
    fwrite(&ulBeepDuration, sizeof(ULONG), 1, pfileUsers);
    fwrite(&ulBeepFrequency, sizeof(ULONG), 1, pfileUsers);
    fwrite(&ucActPalette, sizeof(UCHAR), 1, pfileUsers);
    fwrite(&uiMouseSpeed, sizeof(UINT), 1, pfileUsers);
    fwrite(&ulDoubleClick, sizeof(ULONG), 1, pfileUsers);
    fwrite(&ucUhr, sizeof(UCHAR), 1, pfileUsers);
    fwrite(&ucButtonBar, sizeof(UCHAR), 1, pfileUsers);
    //fwrite(apalPalette + P_USER_DEFINED, sizeof(PALETTE), 1, pfileUsers);
    fwrite(&bSaver, sizeof(BOOL), 1, pfileUsers);
    fwrite(&ulScreenTimeout, sizeof(ULONG), 1, pfileUsers);
    fwrite(acStartVOC, 128, 1, pfileUsers);
    fwrite(acLogoutVOC, 128, 1, pfileUsers);
    fwrite(&ucRememberList, sizeof(UCHAR), 1, pfileUsers);
    fwrite(&ucTerminWarning, sizeof(UCHAR), 1, pfileUsers);
    fwrite(&ucDummy, sizeof(UCHAR), 1, pfileUsers);
    fwrite(&ucDummy, sizeof(UCHAR), 1, pfileUsers);
    fwrite(&uiDummy, sizeof(UINT), 1, pfileUsers);
    return(1);
  }
  fread(&uiUsers, sizeof(UINT), 1, pfileUsers);
  return(uiUsers);
}

UCHAR* get_message (MNU_MESSAGE *pmsgMessage)
{

  MNU_USER userUser;
  UCHAR*   pucBuffer;
  UINT     uiCounter;
  ULONG    ulPos     = 8192;
  LONG     lLength;

  read_user_info(uiUserID, &userUser);
  utl_str_upper(userUser.aucName);
  for (uiCounter = 0; uiCounter < 2048; uiCounter++)
  {
    fseek(pfileMessage, uiCounter * sizeof(LONG), SEEK_SET);
    fread(&lLength, sizeof(LONG), 1, pfileMessage);
    if (!lLength)
      return(NULL);
    if (lLength > 0)
    {
      fseek(pfileMessage, ulPos, SEEK_SET);
      fread(pmsgMessage, sizeof(MNU_MESSAGE), 1, pfileMessage);
      if (!strcmp(userUser.aucName, pmsgMessage->aucTo) ||
          !strcmp(pmsgMessage->aucTo, pucAll))
      {
        pucBuffer = utl_alloc(pmsgMessage->uiLength + 10);
        fread(pucBuffer, pmsgMessage->uiLength, 1, pfileMessage);
        lLength = -lLength;
        fseek(pfileMessage, uiCounter * sizeof(LONG), SEEK_SET);
        fwrite(&lLength, sizeof(LONG), 1, pfileMessage);
        return(pucBuffer);
      }
    }
    else
      lLength = -lLength;
    ulPos += lLength;
  }
  return(NULL);
}

void write_message (MNU_MESSAGE *pmsgMessage, UCHAR *pucMsgText)
{

  ULONG ulPos    = 8192,
        ulBuffer;
  LONG  lLength  = 0;

  pmsgMessage->uiLength = strlen(pucMsgText) + 1;
  fseek(pfileMessage, 0, SEEK_SET);
  do
  {
    if (lLength > 0)
      ulPos += lLength;
    else
      ulPos -= lLength;
    fread(&lLength, sizeof(LONG), 1, pfileMessage);
  } while (-lLength < pmsgMessage->uiLength + sizeof(MNU_MESSAGE) &&
           lLength);
  if (lLength)
    lLength = -lLength;
  else
    lLength = pmsgMessage->uiLength + sizeof(MNU_MESSAGE);
  ulBuffer = ftell(pfileMessage);
  ulBuffer -= 4;
  fseek(pfileMessage, ulBuffer, SEEK_SET);
  fwrite(&lLength, sizeof(LONG), 1, pfileMessage);
  fseek(pfileMessage, ulPos, SEEK_SET);
  fwrite(pmsgMessage, sizeof(MNU_MESSAGE), 1, pfileMessage);
  fwrite(pucMsgText, pmsgMessage->uiLength, 1, pfileMessage);
}

void open_message_file (void)
{

  LONG  lDummy    = 0;
  UINT  uiCounter;

  if (!(pfileMessage = fopen(pucMessageDatei, "r+")))
  {
    if (!(pfileMessage = fopen(pucMessageDatei, "w+")))
    {
      utl_cls(vio_attri(7, 0));
      vio_set_cursor_pos(1, 1);
      printf("Messagedatei kann nicht geîffnet werden\n");
      utl_get_key();
      exit(1);
    }
    for (uiCounter = 0; uiCounter < 2048; uiCounter++)
      fwrite(&lDummy, sizeof(LONG), 1, pfileMessage);
  }
}

UINT next_free_termin (void)
{

  UINT            uiCounter  = 1;
  MNU_DISK_TERMIN dtrmTermin;

  fseek(pfileTermin, 4096, SEEK_SET);
  while (fread(&dtrmTermin, sizeof(MNU_DISK_TERMIN), 1, pfileTermin) == 1)
  {
    if (dtrmTermin.dtDate.ucDay == 0)
      return(uiCounter);
    uiCounter++;
  }
  return(uiCounter);
}

UINT get_termin (UINT uiUser)
{

  UINT uiReturnVar;

  fseek(pfileTermin, uiUser * sizeof(UINT), SEEK_SET);
  fread(&uiReturnVar, sizeof(UINT), 1, pfileTermin);
  return(uiReturnVar);
}

void set_termin (UINT uiUser, UINT uiTermin)
{
  fseek(pfileTermin, uiUser * sizeof(UINT), SEEK_SET);
  fwrite(&uiTermin, sizeof(UINT), 1, pfileTermin);
}

void read_termin (UINT uiTermin, MNU_DISK_TERMIN *pdtrmTermin)
{
  fseek(pfileTermin, 4096 + (uiTermin - 1) * sizeof(MNU_DISK_TERMIN),
        SEEK_SET);
  fread(pdtrmTermin, sizeof(MNU_DISK_TERMIN), 1, pfileTermin);
}

void write_termin (UINT uiTermin, MNU_DISK_TERMIN *pdtrmTermin)
{
  fseek(pfileTermin, 4096 + (uiTermin - 1) * sizeof(MNU_DISK_TERMIN),
        SEEK_SET);
  fwrite(pdtrmTermin, sizeof(MNU_DISK_TERMIN), 1, pfileTermin);
}

MNU_TERMIN* termin_list (void)
{

  UINT             uiID;
  MNU_TERMIN      *ptrmReturnVar = NULL,
                  *ptrmTermin,
                  *ptrmCounter;
  MNU_DISK_TERMIN  dtrmTermin;

  dtrmTermin.uiNext = get_termin(uiUserID);
  if (dtrmTermin.uiNext == 0)
    return(NULL);
  while (dtrmTermin.uiNext)
  {
    uiID = dtrmTermin.uiNext;
    read_termin(uiID, &dtrmTermin);
    ptrmTermin = utl_alloc(sizeof(MNU_TERMIN));
    ptrmTermin->uiID = uiID;
    memcpy(&(ptrmTermin->dtDate), &(dtrmTermin.dtDate), sizeof(MNU_DATE));
    strcpy(ptrmTermin->acText, dtrmTermin.acText);
    ptrmTermin->uiMemoID = dtrmTermin.uiMemoID;
    ptrmTermin->ptrmNext = NULL;
    if (!ptrmReturnVar)
      ptrmReturnVar = ptrmTermin;
    else
    {
      if (datecmp(&(ptrmReturnVar->dtDate), &(ptrmTermin->dtDate)) >= 0)
      {
        ptrmTermin->ptrmNext = ptrmReturnVar;
        ptrmReturnVar = ptrmTermin;
      }
      else
      {
        for (ptrmCounter = ptrmReturnVar; ptrmCounter->ptrmNext;
             ptrmCounter = ptrmCounter->ptrmNext)
          if (datecmp(&(ptrmCounter->dtDate), &(ptrmTermin->dtDate)) < 0 &&
              datecmp(&(ptrmCounter->ptrmNext->dtDate),
                      &(ptrmTermin->dtDate))
                >= 0)
          {
            ptrmTermin->ptrmNext = ptrmCounter->ptrmNext;
            ptrmCounter->ptrmNext = ptrmTermin;
            break;
          }
        if (!(ptrmCounter->ptrmNext))
          ptrmCounter->ptrmNext = ptrmTermin;
      }
    }
  }
  return(ptrmReturnVar);
}

MNU_TERMIN* remember_list (void)
{

  MNU_TERMIN      *ptrmReturnVar = NULL,
                  *ptrmTermin,
                  *ptrmCounter;
  MNU_DISK_TERMIN  dtrmTermin;
  UCHAR            ucTodayDay,
                   ucTomorrowDay,
                   ucTodayMonth,
                   ucTomorrowMonth;
  UINT             uiTodayYear,
                   uiTomorrowYear,
                   uiID;
  struct date      dateToday;

  get_local_date(&dateToday);
  ucTodayDay = dateToday.da_day;
  ucTodayMonth = dateToday.da_mon;
  uiTodayYear = dateToday.da_year;
  ucTomorrowDay = ucTodayDay + 1;
  ucTomorrowMonth = ucTodayMonth;
  if (ucTomorrowDay > aucDaysPerMonth[ucTodayMonth - 1])
  {
    ucTomorrowDay = 1;
    ucTomorrowMonth++;
  }
  uiTomorrowYear = uiTodayYear;
  if (ucTomorrowMonth > 12)
  {
    ucTomorrowMonth = 1;
    uiTomorrowYear++;
  }
  dtrmTermin.uiNext = get_termin(uiUserID);
  if (dtrmTermin.uiNext == 0)
    return(NULL);
  while (dtrmTermin.uiNext)
  {
    uiID = dtrmTermin.uiNext;
    read_termin(uiID, &dtrmTermin);
    if ((dtrmTermin.dtDate.ucDay == ucTodayDay &&
         dtrmTermin.dtDate.ucMonth == ucTodayMonth &&
         dtrmTermin.dtDate.uiYear == uiTodayYear) ||
        (dtrmTermin.dtDate.ucDay == ucTomorrowDay &&
         dtrmTermin.dtDate.ucMonth == ucTomorrowMonth &&
         dtrmTermin.dtDate.uiYear == uiTomorrowYear))
    {
      ptrmTermin = utl_alloc(sizeof(MNU_TERMIN));
      ptrmTermin->uiID = uiID;
      memcpy(&(ptrmTermin->dtDate), &(dtrmTermin.dtDate), sizeof(MNU_DATE));
      strcpy(ptrmTermin->acText, dtrmTermin.acText);
      ptrmTermin->uiMemoID = dtrmTermin.uiMemoID;
      ptrmTermin->ptrmNext = NULL;
      if (!ptrmReturnVar)
        ptrmReturnVar = ptrmTermin;
      else
      {
        if (datecmp(&(ptrmReturnVar->dtDate), &(ptrmTermin->dtDate)) >= 0)
        {
          ptrmTermin->ptrmNext = ptrmReturnVar;
          ptrmReturnVar = ptrmTermin;
        }
        else
        {
          for (ptrmCounter = ptrmReturnVar; ptrmCounter->ptrmNext;
               ptrmCounter = ptrmCounter->ptrmNext)
            if (datecmp(&(ptrmCounter->dtDate), &(ptrmTermin->dtDate)) < 0 &&
                datecmp(&(ptrmCounter->ptrmNext->dtDate),
                        &(ptrmTermin->dtDate))
                  >= 0)
            {
              ptrmTermin->ptrmNext = ptrmCounter->ptrmNext;
              ptrmCounter->ptrmNext = ptrmTermin;
            }
          if (!(ptrmCounter->ptrmNext))
            ptrmCounter->ptrmNext = ptrmTermin;
        }
      }
    }
  }
  return(ptrmReturnVar);
}

UCHAR next_termin (MNU_TERMIN *ptrmTermin)
{

  MNU_DISK_TERMIN  dtrmTermin;
  UCHAR            ucDay,
                   ucMonth,
                   ucHour,
                   ucMinute,
                   ucCopy;
  UINT             uiYear,
                   uiID;
  MNU_DATE         dtNow;
  struct date      dateToday;
  struct time      timeNow;

  get_local_date(&dateToday);
  get_local_time(&timeNow);
  dtNow.ucMinute = timeNow.ti_min;
  dtNow.ucHour = timeNow.ti_hour;
  dtNow.ucDay = dateToday.da_day;
  dtNow.ucMonth = dateToday.da_mon;
  dtNow.uiYear = dateToday.da_year;
  dtrmTermin.uiNext = get_termin(uiUserID);
  if (dtrmTermin.uiNext == 0)
    return(FALSE);
  ptrmTermin->dtDate.ucDay = 0;
  while (dtrmTermin.uiNext)
  {
    uiID = dtrmTermin.uiNext;
    read_termin(uiID, &dtrmTermin);
    ucCopy = FALSE;
    if (ptrmTermin->dtDate.ucDay == 0)
    {
      if (datecmp(&(dtrmTermin.dtDate), &dtNow) > 0)
        ucCopy = TRUE;
    }
    else
      if (datecmp(&(dtrmTermin.dtDate), &(ptrmTermin->dtDate)) > 0)
        ucCopy = TRUE;
    if (ucCopy)
    {
      ptrmTermin->uiID = uiID;
      memcpy(&(ptrmTermin->dtDate), &(dtrmTermin.dtDate), sizeof(MNU_DATE));
      strcpy(ptrmTermin->acText, dtrmTermin.acText);
      ptrmTermin->uiMemoID = dtrmTermin.uiMemoID;
      ptrmTermin->ptrmNext = NULL;
    }
  }
  if (ptrmTermin->dtDate.ucDay)
    return(TRUE);
  return(FALSE);
}

void add_termin (UINT uiUser, MNU_DISK_TERMIN *pdtrmTermin)
{

  UINT            uiID,
                  uiTermin;
  MNU_DISK_TERMIN dtrmTermin;

  pdtrmTermin->uiNext = 0;
  uiID = next_free_termin();
  if (!get_termin(uiUser))
    set_termin(uiUser, uiID);
  else
  {
    uiTermin = get_termin(uiUser);
    dtrmTermin.uiNext = uiTermin;
    do
    {
      uiTermin = dtrmTermin.uiNext;
      read_termin(uiTermin, &dtrmTermin);
    } while (dtrmTermin.uiNext);
    dtrmTermin.uiNext = uiID;
    write_termin(uiTermin, &dtrmTermin);
  }
  write_termin(uiID, pdtrmTermin);
}

void delete_termin (UINT uiUser, UINT uiTermin)
{

  UINT            uiID;
  MNU_DISK_TERMIN dtrmTermin,
                  dtrmNext;

  uiID = get_termin(uiUser);
  if (uiID == uiTermin)
  {
    read_termin(uiID, &dtrmTermin);
    set_termin(uiUser, dtrmTermin.uiNext);
    dtrmTermin.dtDate.ucDay = 0;
    write_termin(uiID, &dtrmTermin);
    if (dtrmTermin.uiMemoID)
      mem_delete(pfileMemo, dtrmTermin.uiMemoID);
  }
  else
  {
    dtrmTermin.uiNext = uiID;
    do
    {
      uiID = dtrmTermin.uiNext;
      read_termin(uiID, &dtrmTermin);
    } while (dtrmTermin.uiNext != uiTermin);
    read_termin(uiTermin, &dtrmNext);
    dtrmTermin.uiNext = dtrmNext.uiNext;
    write_termin(uiID, &dtrmTermin);
    dtrmNext.dtDate.ucDay = 0;
    write_termin(uiTermin, &dtrmNext);
    if (dtrmTermin.uiMemoID)
      mem_delete(pfileMemo, dtrmTermin.uiMemoID);
  }
}

void open_termin_file (void)
{

  UINT  uiDummy   = 0,
        uiCounter;

  if (!(pfileTermin = fopen(pucTerminDatei, "r+")))
  {
    if (!(pfileTermin = fopen(pucTerminDatei, "w+")))
    {
      utl_cls(vio_attri(7, 0));
      vio_set_cursor_pos(1, 1);
      printf("Termindatei kann nicht geîffnet werden\n");
      utl_get_key();
      exit(1);
    }
    for (uiCounter = 0; uiCounter < 2048; uiCounter++)
      fwrite(&uiDummy, sizeof(UINT), 1, pfileTermin);
  }
}

