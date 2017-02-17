/***************************************************************************
 ***************************************************************************
 ***                                                                     ***
 ***                       Schani-Tools fÅr C                            ***
 ***                                                                     ***
 ***                           Memo-Teil                                 ***
 ***                                                                     ***
 ***                (c) 1990-93 by Schani Electronics                    ***
 ***                                                                     ***
 ***************************************************************************
 ***************************************************************************/

#include <gemein.h>
#include <utility.h>
#include <stdio.h>
#include <stdlib.h>
#include <memofile.h>

void static int_mem_set_usage (FILE *pfileMemo, UINT uiMemoID,
                               UCHAR ucStatus)
{

  UCHAR ucBuffer;

  if (ucStatus)
    ucStatus = 1;
  fseek(pfileMemo, uiMemoID / 8, SEEK_SET);
  fread(&ucBuffer, sizeof(UCHAR), 1, pfileMemo);
  ucBuffer = (ucBuffer & ~(1 << (uiMemoID % 8))) ^
             (ucStatus << (uiMemoID % 8));
  fseek(pfileMemo, uiMemoID / 8, SEEK_SET);
  fwrite(&ucBuffer, sizeof(UCHAR), 1, pfileMemo);
}

UINT static int_mem_get_next_free (FILE *pfileMemo)
{

  UINT  uiCounter1,
        uiCounter2;
  UCHAR ucBuffer;

  fseek(pfileMemo, 0, SEEK_SET);
  for (uiCounter1 = 0; uiCounter1 < 8192; uiCounter1++)
  {
    fread(&ucBuffer, sizeof(UCHAR), 1, pfileMemo);
    for (uiCounter2 = 0; uiCounter2 < 8; uiCounter2++)
      if (!(ucBuffer & (1 << uiCounter2)))
        return(uiCounter1 * 8 + uiCounter2 + 1);
  }
  return(0);
}

void static int_mem_read_header (FILE *pfileMemo, UINT uiMemoID,
                                 MEM_HEADER *phdrHeader)
{
  fseek(pfileMemo, 8192 + uiMemoID * 512, SEEK_SET);
  fread(phdrHeader, sizeof(MEM_HEADER), 1, pfileMemo);
}

void static int_mem_write_header (FILE *pfileMemo, UINT uiMemoID,
                                  MEM_HEADER *phdrHeader)
{
  fseek(pfileMemo, 8192 + uiMemoID * 512, SEEK_SET);
  fwrite(phdrHeader, sizeof(MEM_HEADER), 1, pfileMemo);
}

FILE* mem_open_file (CHAR *pcFileName)
{

  FILE  *pfileReturnVar;
  UCHAR  ucDummy        = 0;
  UINT   uiCounter;

  if (!(pfileReturnVar = fopen(pcFileName, "r+")))
  {
    if (!(pfileReturnVar = fopen(pcFileName, "w+")))
      return(NULL);
    for (uiCounter = 0; uiCounter < 8192; uiCounter++)
      fwrite(&ucDummy, sizeof(UCHAR), 1, pfileReturnVar);
  }
  return(pfileReturnVar);
}

CHAR* mem_read (FILE *pfileMemo, UINT uiMemoID)
{

  MEM_HEADER  hdrHeader;
  UINT        uiLength,
              uiRead;
  CHAR       *pcReturnVar;

  uiMemoID--;
  int_mem_read_header(pfileMemo, uiMemoID, &hdrHeader);
  uiLength = hdrHeader.uiLength;
  if (!(pcReturnVar = utl_alloc(uiLength)))
    return(NULL);
  for (uiRead = 0; uiRead < uiLength; )
  {
    fread(pcReturnVar + uiRead, min(500, uiLength - uiRead), 1, pfileMemo);
    uiRead += min(500, uiLength - uiRead);
    int_mem_read_header(pfileMemo, hdrHeader.uiNext, &hdrHeader);
  }
  return(pcReturnVar);
}

void mem_delete (FILE *pfileMemo, UINT uiMemoID)
{

  MEM_HEADER hdrHeader;
  UINT       uiLength,
             uiRead;

  uiMemoID--;
  int_mem_read_header(pfileMemo, uiMemoID, &hdrHeader);
  int_mem_set_usage(pfileMemo, uiMemoID, FALSE);
  uiLength = hdrHeader.uiLength;
  for (uiRead = 0; uiRead < uiLength; )
  {
    uiRead += min(500, uiLength - uiRead);
    int_mem_read_header(pfileMemo, hdrHeader.uiNext, &hdrHeader);
    int_mem_set_usage(pfileMemo, hdrHeader.uiNext, FALSE);
  }
}

UINT mem_write (FILE *pfileMemo, CHAR *pcText, UINT uiLength)
{

  UINT       uiNext,
             uiReturnVar,
             uiWritten   = 0;
  MEM_HEADER hdrHeader;

  uiReturnVar = uiNext = int_mem_get_next_free(pfileMemo);
  uiNext--;
  hdrHeader.uiLength = uiLength;
  while (uiWritten < uiLength)
  {
    int_mem_set_usage(pfileMemo, uiNext, TRUE);
    hdrHeader.uiNext = int_mem_get_next_free(pfileMemo) - 1;
    int_mem_write_header(pfileMemo, uiNext, &hdrHeader);
    fwrite(pcText + uiWritten, 500, 1, pfileMemo);
    uiWritten += min(500, uiLength - uiWritten);
    uiNext = hdrHeader.uiNext;
  }
  return(uiReturnVar);
}