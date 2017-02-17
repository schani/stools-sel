#include <stools.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define KEY_NONE          0
#define KEY_CONTEXT       1
#define KEY_FREEZE        2

typedef struct _NAME
{
  CHAR          acName[21];
  struct _NAME *pnameNext;
} NAME;

typedef struct _CONTEXT
{
  DWORD            dwContext;
  NAME            *pnameName;
  WORD             wFreeze;
  struct _CONTEXT *pctxNext;
} CONTEXT;

typedef struct _COLOR
{
  HLP_COLOR      clrColor;
  struct _COLOR *pcolNext;
} COLOR;

typedef struct _LINK
{
  HLP_LINK      lnkLink;
  struct _LINK *plinkNext;
} LINK;

FILE      *pfileSource,
          *pfileQuickHelp,
          *pfileHeader;
CONTEXT   *pctxFirst      = NULL;
COLOR     *pcolFirst      = NULL;
LINK      *plinkFirst     = NULL;
HLP_COLOR *pclrColors;
HLP_LINK  *plnkLinks;
CHAR      *pcHead         = "/***************************************************************************\n"
                            " ***************************************************************************\n"
                            " ***                                                                     ***\n"
                            " ***                   Headerdatei fr Hilfekontexte                     ***\n"
                            " ***                                                                     ***\n"
                            " ***           Schani Tools (c) 1990-93 by Schani Electronics            ***\n"
                            " ***                                                                     ***\n"
                            " ***************************************************************************\n"
                            " ***************************************************************************/\n\n";
DWORD      dwContexts     = 0,
           dwColors,
           dwLinks;

DWORD get_context (CHAR *pcName)
{
  CONTEXT *pctxContext;
  NAME    *pnameName;

  for (pctxContext = pctxFirst; pctxContext; pctxContext = pctxContext->pctxNext)
  {
    for (pnameName = pctxContext->pnameName; pnameName; pnameName = pnameName->pnameNext)
      if (!strcmp(pnameName->acName, pcName))
        return(pctxContext->dwContext);
  }
  return 0;
}

void append_color (BYTE byColor, WORD wCount)
{
  COLOR *pcolColor;

  if (!pcolFirst)
    pcolColor = pcolFirst = utl_alloc(sizeof(COLOR));
  else
  {
    for (pcolColor = pcolFirst; pcolColor->pcolNext; pcolColor = pcolColor->pcolNext)
      ;
    pcolColor = pcolColor->pcolNext = utl_alloc(sizeof(COLOR));
  }
  pcolColor->clrColor.byColor = byColor;
  pcolColor->clrColor.wCount = wCount;
  pcolColor->pcolNext = NULL;
}

void make_colors (void)
{
  INT    iCounter  = 0;
  COLOR *pcolColor;

  dwColors = 0;
  for (pcolColor = pcolFirst; pcolColor; pcolColor = pcolColor->pcolNext)
    dwColors++;
  pclrColors = utl_alloc(sizeof(HLP_COLOR) * dwColors);
  for (pcolColor = pcolFirst; pcolColor; pcolColor = pcolColor->pcolNext)
  {
    pclrColors[iCounter].byColor = pcolColor->clrColor.byColor;
    pclrColors[iCounter].wCount = pcolColor->clrColor.wCount;
    iCounter++;
  }
}

void delete_colors (void)
{
  COLOR *pcolColor,
        *pcolNext;

  pcolColor = pcolFirst;
  while (pcolColor)
  {
    pcolNext = pcolColor->pcolNext;
    utl_free(pcolColor);
    pcolColor = pcolNext;
  }
  pcolFirst = NULL;
}

void append_link (SWORD swX, SWORD swY, SWORD swWidth, DWORD dwContext)
{
  LINK *plinkLink;

  if (!plinkFirst)
    plinkLink = plinkFirst = utl_alloc(sizeof(LINK));
  else
  {
    for (plinkLink = plinkFirst; plinkLink->plinkNext; plinkLink = plinkLink->plinkNext)
      ;
    plinkLink = plinkLink->plinkNext = utl_alloc(sizeof(LINK));
  }
  plinkLink->lnkLink.swX = swX;
  plinkLink->lnkLink.swY = swY;
  plinkLink->lnkLink.swWidth = swWidth;
  plinkLink->lnkLink.swHeight = 1;
  plinkLink->lnkLink.dwContext = dwContext;
  plinkLink->plinkNext = NULL;
}

void make_links (void)
{
  INT   iCounter  = 0;
  LINK *plinkLink;

  dwLinks = 0;
  for (plinkLink = plinkFirst; plinkLink; plinkLink = plinkLink->plinkNext)
    dwLinks++;
  plnkLinks = utl_alloc(sizeof(HLP_LINK) * dwLinks);
  for (plinkLink = plinkFirst; plinkLink; plinkLink = plinkLink->plinkNext)
  {
    plnkLinks[iCounter].swX = plinkLink->lnkLink.swX;
    plnkLinks[iCounter].swY = plinkLink->lnkLink.swY;
    plnkLinks[iCounter].swWidth = plinkLink->lnkLink.swWidth;
    plnkLinks[iCounter].swHeight = plinkLink->lnkLink.swHeight;
    plnkLinks[iCounter].dwContext = plinkLink->lnkLink.dwContext;
    iCounter++;
  }
}

void delete_links (void)
{
  LINK *plinkLink,
       *plinkNext;

  plinkLink = plinkFirst;
  while (plinkLink)
  {
    plinkNext = plinkLink->plinkNext;
    utl_free(plinkLink);
    plinkLink = plinkNext;
  }
  plinkFirst = NULL;
}

INT get_keyword (CHAR *pcBuffer, CHAR *pcName)
{
  CHAR acBuffer[9];

  pcBuffer++;
  strncpy(acBuffer, pcBuffer, 8);
  acBuffer[8] = 0;
  if (!strcmp(acBuffer, "context "))
  {
    pcBuffer += 8;
    strncpy(pcName, pcBuffer, 20);
    return KEY_CONTEXT;
  }
  acBuffer[7] = 0;
  if (!strcmp(acBuffer, "freeze "))
  {
    pcBuffer += 7;
    strncpy(pcName, pcBuffer, 2);
    return KEY_FREEZE;
  }
  return KEY_NONE;
}

void get_line (CHAR *pcBuffer)
{
  fgets(pcBuffer, 255, pfileSource);
  if (pcBuffer[strlen(pcBuffer) - 1] == '\n')
    pcBuffer[strlen(pcBuffer) - 1] = 0;
}

void analyze_source (void)
{
  CHAR     acBuffer[256],
           acName[128];
  CONTEXT *pctxContext;
  NAME    *pnameName;

  win_info("Analysiere Source");
  get_line(acBuffer);
  while (!feof(pfileSource))
  {
    if (!pctxFirst)
      pctxContext = pctxFirst = utl_alloc(sizeof(CONTEXT));
    else
      pctxContext = pctxContext->pctxNext = utl_alloc(sizeof(CONTEXT));
    pctxContext->dwContext = ++dwContexts;
    pctxContext->pctxNext = NULL;
    pctxContext->pnameName = NULL;
    pctxContext->wFreeze = 0;
    while (*acBuffer == '.')
    {
      switch (get_keyword(acBuffer, acName))
      {
        case KEY_CONTEXT :
          if (!pctxContext->pnameName)
            pnameName = pctxContext->pnameName = utl_alloc(sizeof(NAME));
          else
            pnameName = pnameName->pnameNext = utl_alloc(sizeof(NAME));
          pnameName->pnameNext = NULL;
          strncpy(pnameName->acName, acName, 20);
          break;
        case KEY_FREEZE :
          pctxContext->swFreeze = atoi(acName);
          break;
      }
      get_line(acBuffer);
    }
    while (*acBuffer != '.' && !feof(pfileSource))
      get_line(acBuffer);
  }
  fseek(pfileSource, 0, SEEK_SET);
  win_info(NULL);
}

void make_quickhelp (void)
{
  CHAR       acBuffer[256],
            *pcText;
  CONTEXT   *pctxContext;
  CHAR       acLineBuffer[256],
             acContextName[51];
  BYTE       byActColor,
             byColor,
             abyColorBuffer[256];
  BOOL       bHotSpot            = FALSE,
             bInvisible          = FALSE;
  INT        iCounter,
             iIndex;
  SWORD      swHotSpotX,
             swColorCounter,
             swLines;
  WORD       wCount;
  DWORD      dwCounter,
             dwDummy             = 0,
             dwPos;
 
  win_info("Erzeuge Quick-Help Datei");
  fwrite(&dwContexts, sizeof(DWORD), 1, pfileQuickHelp);
  for (dwCounter = 0; dwCounter < dwContexts; dwCounter++)
    fwrite(&dwDummy, sizeof(DWORD), 1, pfileQuickHelp);
  pcText = utl_alloc(32768);
  pctxContext = pctxFirst;
  get_line(acBuffer);
  while (pctxContext)
  {
    swLines = 0;
    strcpy(pcText, "");
    dwPos = ftell(pfileQuickHelp);
    fseek(pfileQuickHelp, pctxContext->dwContext * sizeof(DWORD), SEEK_SET);
    fwrite(&dwPos, sizeof(DWORD), 1, pfileQuickHelp);
    fseek(pfileQuickHelp, dwPos, SEEK_SET);
    while (*acBuffer == '.')
      get_line(acBuffer);
    while (*acBuffer != '.' && !feof(pfileSource))
    {
      byActColor = COLOR_PLAIN;
      swLines++;
      swColorCounter = 0;
      for (iCounter = 0; acBuffer[iCounter]; iCounter++)
      {
        if (acBuffer[iCounter] == '\\')
        {
          iCounter++;
          switch (acBuffer[iCounter])
          {
            case 'a' :
            case 'A' :
              bHotSpot = TRUE;
              swHotSpotX = swColorCounter;
              break;
            case 'p' :
            case 'P' :
              byActColor = COLOR_PLAIN;
              break;
            case 'b' :
            case 'B' :
              byActColor ^= COLOR_BOLD;
              break;
            case 'i' :
            case 'I' :
              byActColor ^= COLOR_ITALIC;
              break;
            case 'u' :
            case 'U' :
              byActColor ^= COLOR_UNDERLINE;
              break;
            case 'd' :
            case 'D' :
              iCounter++;
              for (iIndex = 0; iIndex < 3; iIndex++, iCounter++)
                acContextName[iIndex] = acBuffer[iCounter];
              acContextName[iIndex] = 0;
              iCounter--;
              if (!bInvisible)
              {
                acLineBuffer[swColorCounter] = (CHAR)atoi(acContextName);
                abyColorBuffer[swColorCounter] = byActColor;
                swColorCounter++;
              }
              break;
            case '\\':
              acLineBuffer[swColorCounter] = '\\';
              abyColorBuffer[swColorCounter] = byActColor;
              swColorCounter++;
              break;
            case 'v' :
            case 'V' :
              if (bHotSpot)
              {
                iCounter++;
                for (iIndex = 0; acBuffer[iCounter] != '\\'; iIndex++, iCounter++)
                  acContextName[iIndex] = acBuffer[iCounter];
                acContextName[iIndex] = 0;
                acContextName[20] = 0;
                iCounter++;
                bHotSpot = FALSE;
                dwDummy = get_context(acContextName);
                if (dwDummy != 0)
                  append_link(swHotSpotX, swLines - 1, swColorCounter - swHotSpotX, dwDummy);
              }
              else
                bInvisible = !bInvisible;
              break;
          }
        }
        else
          if (!bInvisible)
          {
            acLineBuffer[swColorCounter] = acBuffer[iCounter];
            abyColorBuffer[swColorCounter] = byActColor;
            swColorCounter++;
          }
      }
      acLineBuffer[swColorCounter] = 0;
      utl_rtrim(acLineBuffer);
      byColor = *abyColorBuffer;
      wCount = 0;
      for (iCounter = 0; iCounter < swColorCounter; iCounter++)
      {
        if (abyColorBuffer[iCounter] == byColor)
          wCount++;
        else
        {
          append_color(byColor, wCount);
          byColor = abyColorBuffer[iCounter];
          wCount = 1;
        }
      }
      append_color(byColor, (WORD)-1);
      strcat(pcText, acLineBuffer);
      get_line(acBuffer);
      if (*acBuffer != '.')
        strcat(pcText, "\n");
    }
    make_colors();
    delete_colors();
    make_links();
    delete_links();
    fwrite(&pctxContext->swFreeze, sizeof(SWORD), 1, pfileQuickHelp);
    fwrite(&swLines, sizeof(SWORD), 1, pfileQuickHelp);
    fwrite(&dwColors, sizeof(DWORD), 1, pfileQuickHelp);
    fwrite(pclrColors, sizeof(HLP_COLOR) * dwColors, 1, pfileQuickHelp);
    utl_free(pclrColors);
    fwrite(&dwLinks, sizeof(DWORD), 1, pfileQuickHelp);
    fwrite(plnkLinks, sizeof(HLP_LINK) * dwLinks, 1, pfileQuickHelp);
    utl_free(plnkLinks);
    dwDummy = strlen(pcText) + 1;
    fwrite(&dwDummy, sizeof(DWORD), 1, pfileQuickHelp);
    fwrite(pcText, dwDummy, 1, pfileQuickHelp);
    pctxContext = pctxContext->pctxNext;
  }
  win_info(NULL);
}

void make_header (void)
{
  CONTEXT *pctxContext;
  NAME    *pnameName;
  INT      iCounter;
  CHAR     acBuffer1[81],
           acBuffer2[21];

  win_info("Erzeuge Headerdatei");
  fwrite(pcHead, strlen(pcHead), 1, pfileHeader);
  for (pctxContext = pctxFirst; pctxContext; pctxContext = pctxContext->pctxNext)
  {
    for (pnameName = pctxContext->pnameName; pnameName; pnameName = pnameName->pnameNext)
    {
      strcpy(acBuffer1, "#define CTX_");
      strcpy(acBuffer2, utl_str_upper(pnameName->acName));
      strcat(acBuffer1, acBuffer2);
      for (iCounter = 0; iCounter < 30 - strlen(acBuffer2); iCounter++)
        strcat(acBuffer1, " ");
      itoa(pctxContext->dwContext, acBuffer2, 10);
      strcat(acBuffer1, acBuffer2);
      strcat(acBuffer1, "\n");
      fwrite(acBuffer1, strlen(acBuffer1), 1, pfileHeader);
    }
  }
  win_info(NULL);
}

void main (int argc, char *argv[])
{
  CHAR acFileName[256];

  if (argc != 2)
  {
    printf("***    Schani-Tools Hilfe-Compiler    ***\n"
           "*** (c) 1992-93 by Schani Electronics ***\n"
           "\n"
           "  Syntax:\n"
           "    STHC <helpfile>\n"
           "\n"
           "    <helpfile> ist der Name der zu kompilierenden Hilfedatei\n"
           "               ohne Dateierweiterung");
    exit(1);
  }
  strcpy(acFileName, argv[1]);
  strcat(acFileName, ".txt");
  pfileSource = fopen(acFileName, "rt");
  strcpy(acFileName, argv[1]);
  strcat(acFileName, ".hlp");
  pfileQuickHelp = fopen(acFileName, "wb");
  strcpy(acFileName, argv[1]);
  strcat(acFileName, ".h");
  pfileHeader = fopen(acFileName, "wt");
  if (!(pfileSource && pfileQuickHelp && pfileHeader))
  {
    printf("Dateien k”nnen nicht ge”ffnet werden!");
    exit(1);
  }
  glb_init(NULL);
  analyze_source();
  make_quickhelp();
  make_header();
  fclose(pfileHeader);
  fclose(pfileQuickHelp);
  fclose(pfileSource);
}

/*
Local Variables:
compile-command: "wmake -f sthc.mk -h -e"
End:
*/
