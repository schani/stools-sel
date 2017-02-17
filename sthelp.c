/***************************************************************************
 ***************************************************************************
 ***                                                                     ***
 ***                       Schani-Tools fÅr C                            ***
 ***                                                                     ***
 ***                           Hilfe-Teil                                ***
 ***                                                                     ***
 ***                (c) 1990-93 by Schani Electronics                    ***
 ***                                                                     ***
 ***************************************************************************
 ***************************************************************************/

#include <stdio.h>
#include <string.h>
#include <gemein.h>
#include <video.h>
#include <mouse.h>
#include <sthelp.h>
#include <dlg.h>
#include <window.h>
#include <utility.h>
#include <boxes.h>

FILE *pfileHelpFile = NULL;
DWORD dwContexts    = 0,
      dwActContext  = 0;

/***************************************************************************
 *                       Interne Funktionen                                *
 ***************************************************************************/

void static int_hlp_set_file_pos (DWORD dwContext)
{
  DWORD dwContextPos;

  fseek(pfileHelpFile, dwContext * sizeof(DWORD), SEEK_SET);
  fread(&dwContextPos, sizeof(DWORD), 1, pfileHelpFile);
  fseek(pfileHelpFile, dwContextPos, SEEK_SET);
}

void static int_hlp_set_colors (HLP_COLOR *pclrBuffer, HLP_COLOR **ppclrFarben, SWORD swLines)
{
  INT iCounter;

  for (iCounter = 0; iCounter < swLines; iCounter++)
  {
    ppclrFarben[iCounter] = pclrBuffer;
    for (; pclrBuffer->swCount != -1; pclrBuffer++)
      ;
    pclrBuffer++;
  }
}

void static int_hlp_set_text (CHAR *pcBuffer, CHAR **ppcText, SWORD swLines)
{
  INT iCounter;

  for (iCounter = 0; iCounter < swLines; iCounter++)
  {
    ppcText[iCounter] = pcBuffer;
    for (; *pcBuffer != '\n' && *pcBuffer; pcBuffer++)
      ;
    *pcBuffer = 0;
    pcBuffer++;
  }
}

/***************************************************************************
 *                       Allgemeine Funktionen                             *
 ***************************************************************************/

BOOL hlp_open_file (CHAR *pcFileName)
{
  if (pfileHelpFile = fopen(pcFileName, "rb"))
  {
    fread(&dwContexts, sizeof(DWORD), 1, pfileHelpFile);
    return TRUE;
  }
  return FALSE;
}

void hlp_close_file (void)
{
  if (pfileHelpFile)
  {
    fclose(pfileHelpFile);
    dwContexts = 0;
  }
}

BOOL hlp_context_available (DWORD dwContext)
{
  if (dwContext > dwContexts)
    return(FALSE);
  return(TRUE);
}

HLP_CONTEXT* hlp_get_context (DWORD dwContext)
{
  HLP_CONTEXT *pctxReturnVar;
  HLP_COLOR   *pclrColorBuffer;
  CHAR        *pcText;
  DWORD        dwColors,
               dwTextSize;

  if (!pfileHelpFile)
    return NULL;
  if (dwContext > dwContexts)
    return NULL;
  if (!(pctxReturnVar = utl_alloc(sizeof(HLP_CONTEXT))))
    return NULL;
  int_hlp_set_file_pos(dwContext);
  fread(&pctxReturnVar->swFreeze, sizeof(SWORD), 1, pfileHelpFile);
  fread(&pctxReturnVar->swLines, sizeof(SWORD), 1, pfileHelpFile);
  if (!(pctxReturnVar->ppclrColors = utl_alloc(sizeof(HLP_COLOR*) * pctxReturnVar->swLines)) &&
      pctxReturnVar->swLines)
    return NULL;
  fread(&dwColors, sizeof(DWORD), 1, pfileHelpFile);
  if (!(pclrColorBuffer = utl_alloc(sizeof(HLP_COLOR) * dwColors)) && dwColors)
    return NULL;
  fread(pclrColorBuffer, sizeof(HLP_COLOR) * dwColors, 1, pfileHelpFile);
  int_hlp_set_colors(pclrColorBuffer, pctxReturnVar->ppclrColors, pctxReturnVar->swLines);
  fread(&pctxReturnVar->dwLinks, sizeof(DWORD), 1, pfileHelpFile);
  if (pctxReturnVar->dwLinks)
  {
    if (!(pctxReturnVar->plnkLinks = utl_alloc(sizeof(HLP_LINK) * pctxReturnVar->dwLinks)) &&
        pctxReturnVar->dwLinks)
      return FALSE;
    fread(pctxReturnVar->plnkLinks, sizeof(HLP_LINK) * pctxReturnVar->dwLinks, 1, pfileHelpFile);
  }
  else
    pctxReturnVar->plnkLinks = NULL;
  if (!(pctxReturnVar->ppcText = utl_alloc(sizeof(CHAR*) * pctxReturnVar->swLines)) && pctxReturnVar->swLines)
    return NULL;
  fread(&dwTextSize, sizeof(DWORD), 1, pfileHelpFile);
  if (!(pcText = utl_alloc(dwTextSize)))
    return NULL;
  fread(pcText, dwTextSize, 1, pfileHelpFile);
  int_hlp_set_text(pcText, pctxReturnVar->ppcText, pctxReturnVar->swLines);
  return pctxReturnVar;
}

void hlp_release_context (HLP_CONTEXT *pctxContext)
{
  utl_free(*(pctxContext->ppclrColors));
  utl_free(pctxContext->ppclrColors);
  utl_free(pctxContext->plnkLinks);
  utl_free(*(pctxContext->ppcText));
  utl_free(pctxContext->ppcText);
  utl_free(pctxContext);
}

void hlp_set_context (DWORD dwContext)
{
  dwActContext = dwContext;
}

void hlp_help_handler (void)
{
  if (dwActContext)
    box_help(dwActContext);
}

/*
Local Variables:
compile-command: "wmake -f stools.mk -h -e"
End:
*/
