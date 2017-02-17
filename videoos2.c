#define INCL_SUB
#include <os2.h>
#include <conio.h>
#include <string.h>
#include <stdlib.h>
#include <gemein.h>
#include <video.h>
#include <utility.h>

static CHARACTER     *pcharBuffer = NULL;
       INT            iSizeX,
                      iSizeY;
static INT            iCellHeight;

/* The following variables are used for handling the virtual screen */

static INT            iPercent;
static CHARACTER     *pcharVirPage;
static BOOL           bVirtual;
static USHORT         usCursorX,
                      usCursorY;

void vio_init (void)
{
  VIOMODEINFO vmi;
  VIOFONTINFO vfi;

  iPercent = 20;
  vmi.cb = sizeof(VIOMODEINFO);
  VioGetMode(&vmi, 0);
  iSizeX = vmi.col;
  iSizeY = vmi.row;
  if (pcharBuffer == NULL)
    pcharBuffer = utl_alloc(iSizeX * iSizeY * sizeof(CHARACTER));
  bVirtual = FALSE;

  vfi.cb = sizeof(VIOFONTINFO);
  vfi.type = VGFI_GETCURFONT;
  vfi.pbData = NULL;
  vfi.cbData = 0;
  vfi.cyCell = 0;
  VioGetFont(&vfi, 0);
  if (vfi.cyCell == 0)
    iCellHeight = 16;                       /* we are in a windowed session */
  else
    iCellHeight = vfi.cyCell;
}

void vio_set_vir_page (CHARACTER *pcharPage)
{
  if (!bVirtual)
  {
    VioGetCurPos(&usCursorY, &usCursorX, 0);
    bVirtual = TRUE;
  }
  pcharVirPage = pcharPage;
}

void vio_set_phys_page (void)
{
  if (!bVirtual)
    return;
  bVirtual = FALSE;
  vio_set_cursor_type(iPercent);
  VioSetCurPos(usCursorY, usCursorX, 0);
}

WORD vio_get_page (void)
{
  return 0;
}

void vio_set_page (WORD wPage)
{
}

BYTE vio_get_mode (void)
{
  return 3;
}

void vio_set_mode (BYTE byMode)
{
}

BOOL vio_color (void)
{
  return TRUE;
}

void vio_get_cursor (INT *piX, INT *piY, INT *piPercent)
{
  if (!bVirtual)
    VioGetCurPos(&usCursorY, &usCursorX, 0);
  *piX = usCursorX + 1;
  *piY = usCursorY + 1;
  *piPercent = iPercent;
}

void vio_set_cursor_pos (INT iX, INT iY)
{
  if (bVirtual)
  {
    usCursorX = iX - 1;
    usCursorY = iY - 1;
  }
  else
    VioSetCurPos(iY - 1, iX - 1, 0);
}

void vio_set_cursor_type (INT iNewPercent)
{
  VIOCURSORINFO vci;

  iPercent = iNewPercent;

  if (bVirtual)
    return;

  VioGetCurType(&vci, 0);

  if (iNewPercent == 100)
    iNewPercent--;
  if (iNewPercent != 0)
  {
    vci.cEnd = iCellHeight - 1;
    vci.yStart = iCellHeight - (iNewPercent * iCellHeight) / 100;
  }
  
  if (iNewPercent == 0)
    vci.attr = 0xffff;
  else
    vci.attr = 0x0000;
  VioSetCurType(&vci, 0);
}

void vio_s_z (INT iX, INT iY, CHAR cChar)
{
  if (!bVirtual)
    VioWrtCharStr(&cChar, 1, iY - 1, iX - 1, 0);
  else
    VIO_BUFFER_POS(iX,iY)->character = cChar;
}

void vio_s_a (INT iX, INT iY, CHAR cAttri)
{
  if (!bVirtual)
    VioWrtNAttr(&cAttri, 1, iY - 1, iX - 1, 0);
  else
    VIO_BUFFER_POS(iX,iY)->attribute = cAttri;
}

void vio_s_za (INT iX, INT iY, CHAR cChar, CHAR cAttri)
{
  if (!bVirtual)
    VioWrtCharStrAtt(&cChar, 1, iY - 1, iX - 1, &cAttri, 0);
  else
  {
    CHARACTER *pchar;
  
    pchar = VIO_BUFFER_POS(iX,iY);
    pchar->character = cChar;
    pchar->attribute = cAttri;
  }
}

CHAR vio_l_z (INT iX, INT iY)
{
  if (!bVirtual)
  {
    CHARACTER charChar;
    USHORT    usSize    = 2;

    VioReadCellStr((PCH)&charChar, &usSize, iY - 1, iX - 1, 0);
    return charChar.character;
  }

  return VIO_BUFFER_POS(iX,iY)->character;
}

CHAR vio_l_a (INT iX, INT iY)
{
  if (!bVirtual)
  {
    CHARACTER charChar;
    USHORT    usSize    = 2;

    VioReadCellStr((PCH)&charChar, &usSize, iY - 1, iX - 1, 0);
    return charChar.attribute;
  }

  return VIO_BUFFER_POS(iX,iY)->attribute;
}

CHARACTER vio_l_za (INT iX, INT iY)
{
  if (!bVirtual)
  {
    CHARACTER charReturnVar;
    USHORT    usSize         = 2;

    VioReadCellStr((PCH)&charReturnVar, &usSize, iY - 1, iX - 1, 0);
    return charReturnVar;
  }

  return *VIO_BUFFER_POS(iX,iY);
}

void vio_ss (INT iX, INT iY, CHAR *pcString)
{
  if (!bVirtual)
    VioWrtCharStr(pcString, strlen(pcString), iY - 1, iX - 1, 0);
  else
  {
    CHARACTER *pchar;

    pchar = VIO_BUFFER_POS(iX,iY);
    for (; *pcString; pcString++, pchar++)
      pchar->character = *pcString;
  }
}

void vio_ss_a (INT iX, INT iY, CHAR *pcString, CHAR cAttri)
{
  if (!bVirtual)
    VioWrtCharStrAtt(pcString, strlen(pcString), iY - 1, iX - 1, &cAttri, 0);
  else
  {
    CHARACTER *pchar;

    pchar = VIO_BUFFER_POS(iX,iY);
    for (; *pcString; pcString++, pchar++)
    {
      pchar->character = *pcString;
      pchar->attribute = cAttri;
    }
  }
} 

void vio_sp_z (CHAR cChar)
{
  if (!bVirtual)
    VioWrtNChar((PCH)&cChar, iSizeX * iSizeY, 0, 0, 0);
  else
  {
    INT        iCells,
               iCounter;
    CHARACTER *pchar;

    iCells = iSizeX * iSizeY;
    pchar = pcharVirPage;
    for (iCounter = 0; iCounter < iCells; iCounter++, pchar++)
      pchar->character = cChar;
  }
}

void vio_sp_a (CHAR cAttri)
{
  if (!bVirtual)
    VioWrtNAttr((PCH)&cAttri, iSizeX * iSizeY, 0, 0, 0);
  else
  {
    INT        iCells,
               iCounter;
    CHARACTER *pchar;

    iCells = iSizeX * iSizeY;
    pchar = pcharVirPage;
    for (iCounter = 0; iCounter < iCells; iCounter++, pchar++)
      pchar->attribute = cAttri;
  }
}

void vio_sp_za (CHAR cChar, CHAR cAttri)
{
  CHARACTER charChar;

  charChar.character = cChar;
  charChar.attribute = cAttri;
  VioWrtNCell((PCH)&charChar, iSizeX * iSizeY, 0, 0, 0);
}

void vio_sw_z (INT iX, INT iY, INT iWidth, INT iHeight, CHAR cChar)
{
  if (!bVirtual)
  {
    if (iWidth == iSizeX)
      VioWrtNChar((PCH)&cChar, iWidth * iHeight, iY - 1, iX - 1, 0);
    else
    {
      INT iCounter;

      for (iCounter = 0; iCounter < iHeight; iCounter++)
        VioWrtNChar((PCH)&cChar, iWidth, iY - 1 + iCounter, iX - 1, 0);
    }
  }
  else
  {
    INT        iJump,
               iCounterY;
    CHARACTER *pcharCounter,
              *pcharLimit;
        
    iJump = iSizeX - iWidth;
    pcharCounter = VIO_BUFFER_POS(iX, iY);
    for (iCounterY = 0; iCounterY < iHeight; iCounterY++)
    {
      pcharLimit = pcharCounter + iWidth;
      for (; pcharCounter < pcharLimit; pcharCounter++)
        pcharCounter->character = cChar;
      pcharCounter += iJump;
    }
  }  
}

void vio_sw_a (INT iX, INT iY, INT iWidth, INT iHeight, CHAR cAttri)
{
  if (!bVirtual)
  {
    if (iWidth == iSizeX)
      VioWrtNAttr((PCH)&cAttri, iWidth * iHeight, iY - 1, iX - 1, 0);
    else
    {
      INT iCounter;

      for (iCounter = 0; iCounter < iHeight; iCounter++)
        VioWrtNAttr((PCH)&cAttri, iWidth, iY - 1 + iCounter, iX - 1, 0);
    }
  }
  else
  {
    INT        iJump,
               iCounterY;
    CHARACTER *pcharCounter,
              *pcharLimit;
        
    iJump = iSizeX - iWidth;
    pcharCounter = VIO_BUFFER_POS(iX, iY);
    for (iCounterY = 0; iCounterY < iHeight; iCounterY++)
    {
      pcharLimit = pcharCounter + iWidth;
      for (; pcharCounter < pcharLimit; pcharCounter++)
        pcharCounter->attribute = cAttri;
      pcharCounter += iJump;
    }
  }  
}

void vio_sw_za (INT iX, INT iY, INT iWidth, INT iHeight, CHAR cChar, CHAR cAttri)
{
  if (!bVirtual)
  {
    CHARACTER charChar;

    charChar.character = cChar;
    charChar.attribute = cAttri;
    if (iWidth == iSizeX)
      VioWrtNCell((PCH)&charChar, iWidth * iHeight, iY - 1, iX - 1, 0);
    else
    {
      INT iCounter;

      for (iCounter = 0; iCounter < iHeight; iCounter++)
        VioWrtNCell((PCH)&charChar, iWidth, iY - 1 + iCounter, iX - 1, 0);
    }
  }
  else
  {
    INT        iJump,
               iCounterY;
    CHARACTER *pcharCounter,
              *pcharLimit;
        
    iJump = iSizeX - iWidth;
    pcharCounter = VIO_BUFFER_POS(iX, iY);
    for (iCounterY = 0; iCounterY < iHeight; iCounterY++)
    {
      pcharLimit = pcharCounter + iWidth;
      for (; pcharCounter < pcharLimit; pcharCounter++)
      {
        pcharCounter->character = cChar;
        pcharCounter->attribute = cAttri;
      }
      pcharCounter += iJump;
    }
  }  
}

void vio_scroll_up (INT iX, INT iY, INT iWidth, INT iHeight, CHAR cAttri, INT iRows)
{
  if (!bVirtual)
  {
    CHARACTER charChar;

    charChar.character = ' ';
    charChar.attribute = cAttri;
    if (iRows <= 0)
      iRows = -1;
    VioScrollUp(iY - 1, iX - 1, iY + iHeight - 2, iX + iWidth - 2, iRows, (PCH)&charChar, 0);
  }
  else
  {
    if (iRows == 0 || iRows >= iHeight)
      vio_sw_za(iX, iY, iWidth, iHeight, ' ', cAttri);
    else
    {
      vio_lw(iX, iY + iRows, iWidth, iHeight - iRows, pcharBuffer);
      vio_sw(iX, iY, iWidth, iHeight - iRows, pcharBuffer);
      vio_sw_za(iX, iY + iHeight - iRows, iWidth, iRows, ' ', cAttri);
    }
  }
}

void vio_scroll_down (INT iX, INT iY, INT iWidth, INT iHeight, CHAR cAttri, INT iRows)
{
  if (!bVirtual)
  {
    CHARACTER charChar;

    charChar.character = ' ';
    charChar.attribute = cAttri;
    if (iRows <= 0)
      iRows = -1;
    VioScrollDn(iY - 1, iX - 1, iY + iHeight - 2, iX + iWidth - 2, iRows, (PCH)&charChar, 0);
  }
  else
  {
    if (iRows == 0 || iRows >= iHeight)
      vio_sw_za(iX, iY, iWidth, iHeight, ' ', cAttri);
    else
    {
      vio_lw(iX, iY, iWidth, iHeight - iRows, pcharBuffer);
      vio_sw(iX, iY + iRows, iWidth, iHeight - iRows, pcharBuffer);
      vio_sw_za(iX, iY, iWidth, iRows, ' ', cAttri);
    }
  }
}

void vio_lw (INT iX, INT iY, INT iWidth, INT iHeight, CHARACTER *pcharDest)
{
  if (!bVirtual)
  {
    INT    iCounter;
    USHORT usSize;

    if (iWidth == iSizeX)
    {
      usSize = iWidth * iHeight * 2;
      VioReadCellStr((PCH)pcharDest, &usSize, iY - 1, iX - 1, 0);
    }
    else
    {
      for (iCounter = 0; iCounter < iHeight; iCounter++)
      {
        usSize = iWidth * 2;
        VioReadCellStr((PCH)pcharDest, &usSize, iY - 1 + iCounter, iX - 1, 0);
        pcharDest += iWidth;
      }
    }
  }
  else
  {
    INT        i;
    CHARACTER *pcharSource;

    pcharSource = VIO_BUFFER_POS(iX, iY);
    for (i = 0; i < iHeight; i++)
    {
      memcpy(pcharDest, pcharSource, iWidth * sizeof(CHARACTER));
      pcharSource += iSizeX;
      pcharDest += iWidth;
    }
  }
}

void vio_sw (INT iX, INT iY, INT iWidth, INT iHeight, CHARACTER *pcharSource)
{
  if (!bVirtual)
  {
    INT    iCounter;
    USHORT usSize;

    if (iWidth == iSizeX)
      VioWrtCellStr((PCH)pcharSource, iWidth * iHeight * 2, iY - 1, iX - 1, 0);
    else
    {
      usSize = iWidth * 2;
      for (iCounter = 0; iCounter < iHeight; iCounter++)
      {
        VioWrtCellStr((PCH)pcharSource, usSize, iY - 1 + iCounter, iX - 1, 0);
        pcharSource += iWidth;
      }
    }
  }
  else
  {
    INT        i;
    CHARACTER *pcharDest;

    pcharDest = VIO_BUFFER_POS(iX, iY);
    for (i = 0; i < iHeight; i++)
    {
      memcpy(pcharDest, pcharSource, iWidth * sizeof(CHARACTER));
      pcharDest += iSizeX;
      pcharSource += iWidth;
    }
  }
}

void vio_sw_ff (INT iX, INT iY, INT iWidth, INT iHeight, CHARACTER *pcharSource)
{
  if (!bVirtual)
  {
    INT        iCounterX,
               iCounterY;
    USHORT     usSize;
    CHARACTER *pcharCounter = pcharBuffer,
               charChar;

    memcpy(pcharBuffer, pcharSource, iWidth * iHeight * sizeof(CHARACTER));
    for (iCounterY = 0; iCounterY < iHeight; iCounterY++)
      for (iCounterX = 0; iCounterX < iWidth; iCounterX++, pcharCounter++)
        if (pcharCounter->character == (CHAR)0xff || pcharCounter->attribute == (CHAR)0xff)
        {
	  usSize = 2;
  	  VioReadCellStr((PCH)&charChar, &usSize, iY - 1 + iCounterY, iX - 1 + iCounterX, 0);
	  if (pcharCounter->character == (CHAR)0xff)
	    pcharCounter->character = charChar.character;
	  if (pcharCounter->attribute == (CHAR)0xff)
	    pcharCounter->attribute = charChar.attribute;
        }
    vio_sw(iX, iY, iWidth, iHeight, pcharBuffer);
  }
  else
  {
    INT        iCounterY,
               iJump;
    CHARACTER *pcharDest,
              *pcharLimit;

    iJump = iSizeX - iWidth;
    pcharDest = VIO_BUFFER_POS(iX, iY);
    for (iCounterY = 0; iCounterY < iHeight; iCounterY++)
    {
      pcharLimit = pcharDest + iWidth;
      for (; pcharDest < pcharLimit; pcharDest++, pcharSource++)
      {
        if (pcharSource->character != (CHAR)0xff)
          pcharDest->character = pcharSource->character;
        if (pcharSource->attribute != 0xff)
          pcharDest->attribute = pcharSource->attribute;
      }
      pcharDest += iJump;
    }
  }
}

