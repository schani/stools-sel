/***************************************************************************
 ***************************************************************************
 ***                                                                     ***
 ***                       Schani-Tools fÅr C                            ***
 ***                                                                     ***
 ***                          Buffer-Teil                                ***
 ***                                                                     ***
 ***                (c) 1990-93 by Schani Electronics                    ***
 ***                                                                     ***
 ***************************************************************************
 ***************************************************************************/

#include <gemein.h>
#include <utility.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <buffers.h>
#include <assert.h>

/***************************************************************************
 *                       Allgemeine Funktionen                             *
 ***************************************************************************/

BUFFER* buf_open_mem (CHAR *pcBuffer, ULONG ulPhysLength,
                      ULONG ulVirtLength)
{

  BUFFER *pbufReturnVar;

  if (!(pbufReturnVar = utl_alloc(sizeof(BUFFER))))
    return(NULL);                   
  pbufReturnVar->uiType = BUF_MEMORY_BUFFER;
  if (!(pbufReturnVar->pbmMemBuffer = utl_alloc(sizeof(BUF_MEM))))
    return(NULL);
  if (pcBuffer)
  {
    pbufReturnVar->pbmMemBuffer->pcBuffer = pcBuffer;
    pbufReturnVar->pbmMemBuffer->ucDiscardable = FALSE;
    pbufReturnVar->pbmMemBuffer->ulPhysLength = ulPhysLength;
    pbufReturnVar->pbmMemBuffer->ulVirtLength = ulVirtLength;
  }
  else
  {                                      
    if (!(pbufReturnVar->pbmMemBuffer->pcBuffer =
          utl_alloc(sizeof(BUF_MEMORY_INIT_SIZE))))
      return(NULL);
    pbufReturnVar->pbmMemBuffer->ucDiscardable = TRUE;
    pbufReturnVar->pbmMemBuffer->ulPhysLength = BUF_MEMORY_INIT_SIZE;
    pbufReturnVar->pbmMemBuffer->ulVirtLength = 0;
  }
  pbufReturnVar->pbmMemBuffer->ulPos = 0;
  return(pbufReturnVar);
}

BUFFER* buf_open_file (FILE *pfileFile, CHAR *pcFileName, CHAR *pcOpenMode)
{

  BUFFER *pbufReturnVar;

  if (!(pbufReturnVar = utl_alloc(sizeof(BUFFER))))
    return(NULL);                   
  pbufReturnVar->uiType = BUF_FILE_BUFFER;
  if (pfileFile)
    pbufReturnVar->pfileFileBuffer = pfileFile;
  else
  {
    pbufReturnVar->pfileFileBuffer = fopen(pcFileName, pcOpenMode);
    if (!pbufReturnVar->pfileFileBuffer)
      return(NULL);
  }
  return(pbufReturnVar);
}

void buf_close (BUFFER *pbufBuffer)
{
  BUF_SWITCH
  {
    case BUF_MEMORY_BUFFER :
      if (BUF_MB->ucDiscardable)
        utl_free(BUF_MB->pcBuffer);
      utl_free(BUF_MB);
      break;
    case BUF_FILE_BUFFER :
      fclose(BUF_FB);
      break;
  }
  utl_free(pbufBuffer);
}                         

UCHAR buf_eof (BUFFER *pbufBuffer)
{
  BUF_SWITCH
  {
    case BUF_MEMORY_BUFFER :
      if (BUF_MB->ulPos == BUF_MB->ulVirtLength)
        return(TRUE);
      return(FALSE);
    case BUF_FILE_BUFFER :
      return(feof(BUF_FB));
    default:
      assert (FALSE);
      return FALSE;
  }
}

ULONG buf_write (CHAR *pcSource, ULONG ulSize, ULONG ulN,
                 BUFFER *pbufBuffer)
{

  ULONG ulNs;
                             
  BUF_SWITCH
  {
    case BUF_MEMORY_BUFFER :
      if (BUF_MB->ulPos + ulSize * ulN >= BUF_MB->ulPhysLength)
        if (!utl_realloc(BUF_MB->pcBuffer, BUF_MB->ulPhysLength +=
            ((ulSize * ulN) / BUF_MEMORY_EXPAND_SIZE + 1) *
            BUF_MEMORY_EXPAND_SIZE))
          return(0);
      memcpy(BUF_MB->pcBuffer + BUF_MB->ulPos, pcSource, ulN * ulSize);
      BUF_MB->ulPos += ulN * ulSize;
      BUF_MB->ulVirtLength = max(BUF_MB->ulVirtLength, BUF_MB->ulPos);
      return(ulN);
    case BUF_FILE_BUFFER :
      return(fwrite(pcSource, ulSize, ulN, BUF_FB));
  }
  return(0);
}

ULONG buf_read (CHAR *pcDest, ULONG ulSize, ULONG ulN,
                BUFFER *pbufBuffer)
{

  ULONG ulNs;
           
  BUF_SWITCH
  {
    case BUF_MEMORY_BUFFER :
      ulN = min((BUF_MB->ulVirtLength - BUF_MB->ulPos) / ulSize, ulN);
      memcpy(pcDest, BUF_MB->pcBuffer + BUF_MB->ulPos, ulN * ulSize);
      BUF_MB->ulPos += ulN * ulSize;
      return(ulN);       
    case BUF_FILE_BUFFER :
      return(fread(pcDest, ulSize, ulN, BUF_FB));
  }
  return(0);
}

INT buf_putc (CHAR cChar, BUFFER *pbufBuffer)
{
  if (buf_write(&cChar, 1, 1, pbufBuffer))
    return(0);
  return(EOF);
}

INT buf_getc (BUFFER *pbufBuffer)
{

  CHAR cReturnVar;

  if (buf_read(&cReturnVar, 1, 1, pbufBuffer))
    return(cReturnVar);
  return(EOF);
}

UCHAR buf_flush (BUFFER *pbufBuffer)
{
  BUF_SWITCH
  {
    case BUF_FILE_BUFFER :
      if (fflush(BUF_FB))
        return(FALSE);
  }
  return(TRUE);
}

UCHAR buf_set_pos (BUFFER *pbufBuffer, ULONG ulPos)
{               
  BUF_SWITCH
  {
    case BUF_MEMORY_BUFFER :
      if (ulPos <= BUF_MB->ulVirtLength)
      {
        BUF_MB->ulPos = ulPos;
        return(TRUE);
      }            
      break;
    case BUF_FILE_BUFFER :
      if (fsetpos(BUF_FB, &ulPos))
        return(FALSE);
      return(TRUE);
  }        
  return(FALSE);
}

UCHAR buf_get_pos (BUFFER *pbufBuffer, ULONG *pulPos)
{               
  BUF_SWITCH
  {
    case BUF_MEMORY_BUFFER :
      *pulPos = BUF_MB->ulPos;
      return(TRUE);
    case BUF_FILE_BUFFER :
      if (fgetpos(BUF_FB, pulPos))
        return(FALSE);
      return(TRUE);
  }
  return(FALSE);
}

UCHAR buf_seek (BUFFER *pbufBuffer, LONG lOffset, INT iOrigin)
{                     
  BUF_SWITCH
  {
    case BUF_MEMORY_BUFFER :
      switch (iOrigin)
      {
        case SEEK_CUR :
          if ((lOffset < 0 && (-lOffset) > BUF_MB->ulPos) ||
              (lOffset >= 0 &&
               lOffset > BUF_MB->ulVirtLength - BUF_MB->ulPos + 1))
            return(FALSE);
          BUF_MB->ulPos += lOffset;
          return(TRUE);
        case SEEK_END :
          if (lOffset < 0 || lOffset > BUF_MB->ulVirtLength)
            return(FALSE);
          BUF_MB->ulPos = BUF_MB->ulVirtLength - lOffset;
          return(TRUE);
        case SEEK_SET :
          if (lOffset < 0 || lOffset > BUF_MB->ulVirtLength)
            return(FALSE);
          BUF_MB->ulPos = lOffset;
          return(TRUE);
      }
      break;
    case BUF_FILE_BUFFER :
      if (fseek(BUF_FB, lOffset, iOrigin))
        return(FALSE);
      return(TRUE);
  }         
  return(FALSE);
}

ULONG buf_tell (BUFFER *pbufBuffer)
{
  BUF_SWITCH
  {
    case BUF_MEMORY_BUFFER :
      return(BUF_MB->ulPos);
    case BUF_FILE_BUFFER :
      return(ftell(BUF_FB));
  }
  return(-1);
}

void buf_rewind (BUFFER *pbufBuffer)
{           
  BUF_SWITCH
  {
    case BUF_MEMORY_BUFFER :
      BUF_MB->ulPos = 0;
      break;
    case BUF_FILE_BUFFER :
      fseek(BUF_FB, 0, SEEK_SET);
      break;
  }
}
