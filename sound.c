/***************************************************************************
 ***************************************************************************
 ***                                                                     ***
 ***                       Schani-Tools fÅr C                            ***
 ***                                                                     ***
 ***                       Soundblaster-Teil                             ***
 ***                                                                     ***
 ***                (c) 1990-93 by Schani Electronics                    ***
 ***                                                                     ***
 ***************************************************************************
 ***************************************************************************/

#include <gemein.h>
#include <utility.h>
#include <stdlib.h>
#include <string.h>
#include <io.h>
#include <dos.h>
#include <fcntl.h>
#include <sound.h>

extern BOOL bSoundBlaster;
extern WORD wStatusWord;

void       *pDriverBlock      = NULL;
void  (far *sbl_driver)(void) = NULL;

void sbl_init (void)
{

  CHAR     *pcPath,
            acName[128];
  INT       iHandle;
  ULONG     ulSize,
            ulDriver;
  CHAR     *pcDriver;

  pcPath = getenv("SOUND");
  if (pcPath)
  {
    strcpy(acName, pcPath);
    if (acName[strlen(acName)] == '\\')
      acName[strlen(acName)] = 0;
    strcat(acName, "\\CT-VOICE.DRV");
  }
  else
    strcpy(acName, "CT-VOICE.DRV");
  iHandle = open(acName, O_RDONLY | O_BINARY, 0);
  if (iHandle != -1)
  {
    ulSize = filelength(iHandle) + 16;
    pDriverBlock = utl_alloc(ulSize);
    pcDriver = pDriverBlock;
    ulDriver = (ULONG)((void far*)pcDriver);
    pcDriver += 16 - ulDriver & 0x000f;
    read(iHandle, pcDriver, filelength(iHandle));
    close(iHandle);
    ulDriver += ((ULONG)(((ulDriver & 0xffff) / 16) + 1)) << 16;
    ulDriver &= 0xffff0000;
    sbl_driver = (void(far*)(void))ulDriver;
    if (sbl_init_driver())
    {
      bSoundBlaster = TRUE;
      sbl_set_status_word();
      atexit(sbl_done);
    }
    else
      utl_free(pDriverBlock);
  }
}

void sbl_done (void)
{
  sbl_remove_driver();
  bSoundBlaster = FALSE;
}

UCHAR sbl_init_driver (void)
{

  UCHAR ucReturnVar = TRUE;

  asm {
    mov  bx,3
    call sbl_driver
    cmp  ax,0
    je   ende
    mov  ucReturnVar,FALSE
  }
ende:
  return(ucReturnVar);
}

void sbl_remove_driver (void)
{
  if (bSoundBlaster)
  {
    asm {
      mov  bx,9
      call sbl_driver
    }
    utl_free(pDriverBlock);
  }
}

void sbl_speaker (UCHAR ucStatus)
{
  if (bSoundBlaster)
  {
    if (ucStatus)
      ucStatus = 1;
    asm {
      mov  bx,4
      mov  al,ucStatus
      call sbl_driver
    }
  }
}

void* sbl_load_voc (CHAR *pcName)
{

  INT    iHandle;
  UINT   uiOffset;
  ULONG  ulSize;
  void  *pReturnVar = NULL;

  iHandle = open(pcName, O_RDONLY | O_BINARY, 0);
  if (iHandle != -1)
  {
    lseek(iHandle, 20, SEEK_SET);
    read(iHandle, &uiOffset, 2);
    ulSize = filelength(iHandle) - uiOffset;
    pReturnVar = utl_alloc(ulSize);
    lseek(iHandle, uiOffset, SEEK_SET);
    read(iHandle, pReturnVar, ulSize);
    close(iHandle);
  }
  return(pReturnVar);
}

void sbl_start_voc (void *pVOC)
{

  void far *pfVOC;
  UINT      uiSeg,
            uiOff;

  if (bSoundBlaster)
  {
    pfVOC = (void far*)pVOC;
    uiSeg = FP_SEG(pfVOC);
    uiOff = FP_OFF(pfVOC);
    asm {
      mov  bx,6
      mov  es,uiSeg
      mov  di,uiOff
      call sbl_driver
    }
  }
}

void sbl_abort_voc (void)
{
  if (bSoundBlaster)
  {
    asm {
      mov  bx,8
      call sbl_driver
    }
  }
}

void sbl_stop_voc (void)
{
  if (bSoundBlaster)
  {
    asm {
      mov  bx,10
      call sbl_driver
    }
  }
}

void sbl_continue_voc (void)
{
  if (bSoundBlaster)
  {
    asm {
      mov  bx,11
      call sbl_driver
    }
  }
}

void sbl_stop_loop (UCHAR ucRightNow)
{
  if (bSoundBlaster)
  {
    if (ucRightNow)
      ucRightNow = TRUE;
    asm {
      mov  bx,12
      mov  al,ucRightNow
      mov  ah,0
      call sbl_driver
    }
  }
}

void sbl_set_status_word (void)
{

  UINT  uiSegment,
        uiOffset;

  uiSegment = FP_SEG(((void far*)&wStatusWord));
  uiOffset = FP_OFF(((void far*)&wStatusWord));
  if (bSoundBlaster)
  {
    asm {
      mov  bx,5
      mov  es,uiSegment
      mov  di,uiOffset
      call sbl_driver
    }
  }
}