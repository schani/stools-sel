#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <gemein.h>

typedef struct
{
  UCHAR aucName[61];
  UCHAR aucHelpLine[79];
  UCHAR aucWorkDir[131];
  UCHAR aucCmdLine[131];
  UCHAR aucDatei[9];
  UCHAR ucProgramm;
} MNU_EINTRAG;

typedef struct __MNU_MENU
{
  UCHAR              ucEintraege;
  MNU_EINTRAG        ameEintrag[20];
  UCHAR*             winWindow;
  UCHAR              aucMnuName[9];
  struct __MNU_MENU *pmenuUnterMenu;
  struct __MNU_MENU *pmenuOberMenu;
} _MNU_MENU;

typedef struct
{
  UCHAR ucProgramm;
  UCHAR aucName[61];
  UCHAR aucHelpLine[79];
  UCHAR aucWorkDir[129];
  UCHAR aucCmdLine[129];
  UINT  uiGruppe;
  UINT  uiNext;
} MNU_DISK_EINTRAG;

typedef struct
{
  UINT  uiFirst;
  UCHAR aucTitel[51];
  UCHAR ucWinX;
  UCHAR ucWinY;
  UCHAR aucDummy[348];
} MNU_DISK_MENU;

FILE *pfileMenu;

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

void open_menu_file (void)
{

  UINT          uiCounter;
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
  pfileMenu = fopen("SEMENU.MNU", "w+");
  fputc(1, pfileMenu);
  fputc(1, pfileMenu);
  for (uiCounter = 0; uiCounter < 2047; uiCounter++)
    fputc(0, pfileMenu);
  for (uiCounter = 0; uiCounter < 8192; uiCounter++)
    fputc(0, pfileMenu);
}

void convert_menu (UCHAR *pucName)
{

  UCHAR             aucFileName[13],
                    ucWinX,
                    ucWinY,
                    ucCounter,
                    aucWindowTitel[51];
  UINT              auiPos[20];
  _MNU_MENU         menuMenu;
  MNU_DISK_MENU     dmMenu;
  MNU_DISK_EINTRAG  deEintrag;
  FILE             *pfileFile;

  strcpy(aucFileName, pucName);
  strcat(aucFileName, ".MNU");
  pfileFile = fopen(aucFileName, "r");
  fread(&menuMenu.ucEintraege, sizeof(UCHAR), 1, pfileFile);
  fread(menuMenu.ameEintrag, sizeof(MNU_EINTRAG), menuMenu.ucEintraege,
        pfileFile);
  fread(aucWindowTitel, 9, 1, pfileFile);
  fread(&ucWinX, sizeof(UCHAR), 1, pfileFile);
  fread(&ucWinY, sizeof(UCHAR), 1, pfileFile);
  fread(aucWindowTitel, 51, 1, pfileFile);
  fclose(pfileFile);
  if (menuMenu.ucEintraege)
    dmMenu.uiFirst = get_next_free() + 1;
  else
    dmMenu.uiFirst = 0;
  strcpy(dmMenu.aucTitel, aucWindowTitel);
  dmMenu.ucWinX = ucWinX;
  dmMenu.ucWinY = ucWinY;
  write_entry(get_next_free(), &dmMenu);
  for (ucCounter = 0; ucCounter < menuMenu.ucEintraege; ucCounter++)
  {
    deEintrag.ucProgramm = menuMenu.ameEintrag[ucCounter].ucProgramm;
    strcpy(deEintrag.aucName, menuMenu.ameEintrag[ucCounter].aucName);
    strcpy(deEintrag.aucHelpLine, menuMenu.ameEintrag[ucCounter].aucHelpLine);
    if (deEintrag.ucProgramm)
    {
      strcpy(deEintrag.aucWorkDir, menuMenu.ameEintrag[ucCounter].aucWorkDir);
      strcpy(deEintrag.aucCmdLine, menuMenu.ameEintrag[ucCounter].aucCmdLine);
      auiPos[ucCounter] = 0;
    }
    else
      auiPos[ucCounter] = get_next_free();
    if (ucCounter < menuMenu.ucEintraege - 1)
      deEintrag.uiNext = get_next_free() + 1;
    else
      deEintrag.uiNext = 0;
    write_entry(get_next_free(), &deEintrag);
  }
  for (ucCounter = 0; ucCounter < menuMenu.ucEintraege; ucCounter++)
    if (auiPos[ucCounter])
    {
      read_entry(auiPos[ucCounter], &deEintrag);
      deEintrag.uiGruppe = get_next_free();
      write_entry(auiPos[ucCounter], &deEintrag);
      convert_menu(menuMenu.ameEintrag[ucCounter].aucDatei);
    }
}

main()
{
  _fmode = O_BINARY;
  open_menu_file();
  convert_menu("HAUPT");
  printf("MenÅsystem konvertiert!\n");
  return(0);
}