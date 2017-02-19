#include <windows.h>
#include <gemein.h>
#include <stdlib.h>
#include <conio.h>
#include <stdio.h>
#include <string.h>
#include <video.h>
#include <stnt.h>

#define utl_alloc(s)            malloc(s)
#define utl_free(s)             free(s)

#define VIO_BUFFER_POS(x,y)     (pcharVirPage + ((x)-1+((y)-1)*iSizeX))


static HANDLE     hStdOut         = INVALID_HANDLE_VALUE;
       INT        iSizeX,
                  iSizeY;
static CHARACTER *pcharBuffer     = NULL;
static COORD      coordBufferSize;

/* The following variables are used for handling virtual screens */

static CHARACTER           *pcharVirPage = NULL;
static BOOL                 bVirtual     = FALSE;
static CONSOLE_CURSOR_INFO  cciVirtual;
static COORD                coordCursor;

void vio_init (void)
{
  CONSOLE_SCREEN_BUFFER_INFO csbi;

  if (hStdOut == INVALID_HANDLE_VALUE)
    hStdOut = GetStdHandle(STD_OUTPUT_HANDLE);
  GetConsoleScreenBufferInfo(hStdOut, &csbi);
  iSizeX = csbi.dwSize.X;
  iSizeY = csbi.dwSize.Y;
  if (pcharBuffer == NULL)
    pcharBuffer = utl_alloc(sizeof(CHARACTER) * iSizeX * iSizeY);
  coordBufferSize.X = iSizeX;
  coordBufferSize.Y = iSizeY;
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

void vio_set_vir_page (CHARACTER *pcharPage)
{
  CONSOLE_SCREEN_BUFFER_INFO csbi;

  if (!bVirtual)
  {
    GetConsoleCursorInfo(hStdOut, &cciVirtual);
    GetConsoleScreenBufferInfo(hStdOut, &csbi);
    coordCursor = csbi.dwCursorPosition;
    bVirtual = TRUE;
  }
  pcharVirPage = pcharPage;
}

void vio_set_phys_page (void)
{
  if (bVirtual)
  {
    SetConsoleCursorInfo(hStdOut, &cciVirtual);
    SetConsoleCursorPosition(hStdOut, coordCursor);
    bVirtual = FALSE;
    pcharVirPage = NULL;
  }
}

void vio_get_cursor (INT *piX, INT *piY, INT *piPercent)
{
  if (bVirtual)
  {
    *piX = coordCursor.X + 1;
    *piY = coordCursor.Y + 1;
    if (cciVirtual.bVisible)
      *piPercent = cciVirtual.dwSize;
    else
      *piPercent = 0;
  }
  else
  {
    CONSOLE_SCREEN_BUFFER_INFO csbi;
  
    GetConsoleCursorInfo(hStdOut, &cciVirtual);
    GetConsoleScreenBufferInfo(hStdOut, &csbi);
    *piX = csbi.dwCursorPosition.X + 1;
    *piY = csbi.dwCursorPosition.Y + 1;
    if (cciVirtual.bVisible)
      *piPercent = cciVirtual.dwSize;
    else
      *piPercent = 0;
  }
}

void vio_set_cursor_pos (INT iX, INT iY)
{
  coordCursor.X = iX - 1;
  coordCursor.Y = iY - 1;
  if (!bVirtual)
    SetConsoleCursorPosition(hStdOut, coordCursor);
}

void vio_set_cursor_type (INT iPercent)
{
  if (iPercent == 0)
    cciVirtual.bVisible = FALSE;
  else
  {
    cciVirtual.dwSize = iPercent;
    cciVirtual.bVisible = TRUE;
  }
  if (!bVirtual)
    SetConsoleCursorInfo(hStdOut, &cciVirtual);
}

CHAR vio_l_z (INT iX, INT iY)
{
  if (!bVirtual)
  {
    COORD coord;
    CHAR  cReturnVar;
    DWORD dwRead;

    coord.X = iX - 1;
    coord.Y = iY - 1;
    ReadConsoleOutputCharacter(hStdOut, &cReturnVar, 1, coord, &dwRead);
    return cReturnVar;
  }

  return VIO_BUFFER_POS(iX,iY)->character;
}

CHAR vio_l_a (INT iX, INT iY)
{
  if (!bVirtual)
  {
    COORD coord;
    WORD  wReturnVar;
    DWORD dwRead;

    coord.X = iX - 1;
    coord.Y = iY - 1;
    ReadConsoleOutputAttribute(hStdOut, &wReturnVar, 1, coord, &dwRead);
    return (CHAR)wReturnVar;
  }

  return (CHAR)(VIO_BUFFER_POS(iX,iY)->character);
}

CHARACTER vio_l_za (INT iX, INT iY)
{
  if (!bVirtual)
  {
    COORD     coord;
    CHARACTER charReturnVar;
    DWORD     dwRead;

    coord.X = iX - 1;
    coord.Y = iY - 1;
    ReadConsoleOutputCharacter(hStdOut, &charReturnVar.character, 1, coord, &dwRead);
    ReadConsoleOutputAttribute(hStdOut, (WORD*)&charReturnVar.attribute, 1, coord, &dwRead);
    return charReturnVar;
  }

  return *VIO_BUFFER_POS(iX,iY);
}

void vio_s_z (INT iX, INT iY, CHAR cChar)
{
  if (!bVirtual)
  {
    COORD coord;
    DWORD dwWritten;

    coord.X = iX - 1;
    coord.Y = iY - 1;
    WriteConsoleOutputCharacter(hStdOut, &cChar, 1, coord, &dwWritten);
  }
  else
    VIO_BUFFER_POS(iX,iY)->character = cChar;
}

void vio_s_a (INT iX, INT iY, CHAR cAttri)
{
  if (!bVirtual)
  {
    COORD coord;
    DWORD dwWritten;
    WORD  wAttri;

    coord.X = iX - 1;
    coord.Y = iY - 1;
    wAttri = (WORD)cAttri;
    WriteConsoleOutputAttribute(hStdOut, &wAttri, 1, coord, &dwWritten);
  }
  else
    VIO_BUFFER_POS(iX,iY)->attribute = cAttri;
}

void vio_s_za (INT iX, INT iY, CHAR cChar, CHAR cAttri)
{
  if (!bVirtual)
  {
    COORD coord;
    DWORD dwWritten;
    WORD  wAttri;

    coord.X = iX - 1;
    coord.Y = iY - 1;
    wAttri = (WORD)cAttri;
    WriteConsoleOutputCharacter(hStdOut, &cChar, 1, coord, &dwWritten);
    WriteConsoleOutputAttribute(hStdOut, &wAttri, 1, coord, &dwWritten);
  }
  else
  {
    CHARACTER *pchar;
    
    pchar = VIO_BUFFER_POS(iX,iY);
    pchar->character = cChar;
    pchar->attribute = cAttri;
  }
}

void vio_ss (INT iX, INT iY, CHAR *pcString)
{
  if (!bVirtual)
  {
    DWORD dwWritten;
    COORD coord;
    
    coord.X = iX - 1;
    coord.Y = iY - 1;
    WriteConsoleOutputCharacter(hStdOut, pcString, strlen(pcString), coord, &dwWritten);
  }
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
  {
    INT        i;
    COORD      coordPos;
    SMALL_RECT srctPos;

    for (i = 0; pcString[i]; i++)
    {
      pcharBuffer[i].character = pcString[i];
      pcharBuffer[i].attribute = cAttri;
    }
    srctPos.Left = iX - 1;
    srctPos.Top = iY - 1;
    srctPos.Right = iX + i - 2;
    srctPos.Bottom = iY - 1;
    coordPos.X = 0;
    coordPos.Y = 0;
    WriteConsoleOutput(hStdOut, (PCHAR_INFO)pcharBuffer, coordBufferSize, coordPos, &srctPos);
  }
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

void vio_sp_za (CHAR cChar, CHAR cAttri)
{
  DWORD dwCells;

  dwCells = iSizeX * iSizeY;
  if (!bVirtual)
  {
    COORD coord;
    DWORD dwWritten;

    coord.X = 0;
    coord.Y = 0;
    FillConsoleOutputCharacter(hStdOut, cChar, dwCells, coord, &dwWritten);
    FillConsoleOutputAttribute(hStdOut, cAttri, dwCells, coord, &dwWritten);
  }
  else
  {
    INT        i;
    CHARACTER *pchar;

    pchar = pcharVirPage;
    for (i = 0; i < (INT)dwCells; i++, pchar++)
    {
      pchar->character = cChar;
      pchar->attribute = cAttri;
    }
  }
}

void vio_sp_z (CHAR cChar)
{
  DWORD dwCells;

  dwCells = iSizeX * iSizeY;
  if (!bVirtual)
  {
    COORD coord;
    DWORD dwWritten;

    coord.X = 0;
    coord.Y = 0;
    FillConsoleOutputCharacter(hStdOut, cChar, dwCells, coord, &dwWritten);
  }
  else
  {
    INT        i;
    CHARACTER *pchar;

    pchar = pcharVirPage;
    for (i = 0; i < (INT)dwCells; i++, pchar++)
      pchar->character = cChar;
  }
}

void vio_sp_a (CHAR cAttri)
{
  DWORD dwCells;

  dwCells = iSizeX * iSizeY;
  if (!bVirtual)
  {
    COORD coord;
    DWORD dwWritten;

    coord.X = 0;
    coord.Y = 0;
    FillConsoleOutputAttribute(hStdOut, cAttri, dwCells, coord, &dwWritten);
  }
  else
  {
    INT        i;
    CHARACTER *pchar;

    pchar = pcharVirPage;
    for (i = 0; i < (INT)dwCells; i++, pchar++)
      pchar->attribute = cAttri;
  }
}                                        

void vio_sw_za (INT iX, INT iY, INT iWidth, INT iHeight,
                CHAR cChar, CHAR cAttri)
{
  if (!bVirtual)
  {
    COORD coord;
    DWORD dwWritten;
                
    coord.X = iX - 1;
    if (iWidth == iSizeX)
    {
      iWidth *= iHeight;
      coord.Y = iY - 1;
      FillConsoleOutputCharacter(hStdOut, cChar, iWidth, coord, &dwWritten);
      FillConsoleOutputAttribute(hStdOut, cAttri, iWidth, coord, &dwWritten);
    }
    else
    {
      iHeight += --iY;
      for (; iY < iHeight; iY++)
      {
        coord.Y = iY;
        FillConsoleOutputCharacter(hStdOut, cChar, iWidth, coord, &dwWritten);
        FillConsoleOutputAttribute(hStdOut, cAttri, iWidth, coord, &dwWritten);
      }
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

void vio_sw_z (INT iX, INT iY, INT iWidth, INT iHeight, CHAR cChar)
{
  if (!bVirtual)
  {
    COORD coord;
    DWORD dwWritten;
                
    coord.X = iX - 1;
    if (iWidth == iSizeX)
    {
      iWidth *= iHeight;
      coord.Y = iY - 1;
      FillConsoleOutputCharacter(hStdOut, cChar, iWidth, coord, &dwWritten);
    }
    else
    {
      iHeight += --iY;
      for (; iY < iHeight; iY++)
      {
        coord.Y = iY;
        FillConsoleOutputCharacter(hStdOut, cChar, iWidth, coord, &dwWritten);
      }
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
    COORD coord;
    DWORD dwWritten;
                
    coord.X = iX - 1;
    if (iWidth == iSizeX)
    {
      iWidth *= iHeight;
      coord.Y = iY - 1;
      FillConsoleOutputAttribute(hStdOut, cAttri, iWidth, coord, &dwWritten);
    }    
    else
    {
      iHeight += --iY;
      for (; iY < iHeight; iY++)
      {
        coord.Y = iY;
        FillConsoleOutputAttribute(hStdOut, cAttri, iWidth, coord, &dwWritten);
      }
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

void vio_scroll_up (INT iX, INT iY, INT iWidth, INT iHeight, CHAR cAttri, INT iRows)
{
  if (!bVirtual)
  {
    SMALL_RECT srct;
    COORD      coord;
    CHAR_INFO  chi;

    if (iRows == 0)
      iRows = iHeight;                                   // clear the whole window
    srct.Top = --iY;
    srct.Left = --iX;
    srct.Bottom = iY + iHeight - 1;
    srct.Right = iX + iWidth - 1;
    coord.X = iX;
    coord.Y = iY - iRows;
    chi.Char.AsciiChar = ' ';
    chi.Attributes = cAttri;
    ScrollConsoleScreenBuffer(hStdOut, &srct, &srct, coord, &chi);
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
    SMALL_RECT srct;
    COORD      coord;
    CHAR_INFO  chi;

    if (iRows == 0)
      iRows = iHeight;                                   // clear the whole window
    srct.Top = --iY;
    srct.Left = --iX;
    srct.Bottom = iY + iHeight - 1;
    srct.Right = iX + iWidth - 1;
    coord.X = iX;
    coord.Y = iY + iRows;
    chi.Char.AsciiChar = ' ';
    chi.Attributes = cAttri;
    ScrollConsoleScreenBuffer(hStdOut, &srct, &srct, coord, &chi);
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
    SMALL_RECT srct;
    COORD      coordSize,
               coordCoord;

    coordSize.X = iWidth;
    coordSize.Y = iHeight;
    coordCoord.X = coordCoord.Y = 0;
    srct.Top = --iY;
    srct.Left = --iX;
    srct.Bottom = iY + iHeight - 1;
    srct.Right = iX + iWidth - 1;
    ReadConsoleOutput(hStdOut, (PCHAR_INFO)pcharDest, coordSize, coordCoord, &srct);
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
    SMALL_RECT srct;
    COORD      coordSize,
               coordCoord;

    coordSize.X = iWidth;
    coordSize.Y = iHeight;
    coordCoord.X = coordCoord.Y = 0;
    srct.Top = --iY;
    srct.Left = --iX;
    srct.Bottom = iY + iHeight - 1;
    srct.Right = iX + iWidth - 1;
    WriteConsoleOutput(hStdOut, (PCHAR_INFO)pcharSource, coordSize, coordCoord, &srct);
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
    SMALL_RECT srct;
    COORD      coordSize,
               coordCoord;
    INT        iCounterX,
               iCounterY;
    DWORD      dwRead;
    CHARACTER *pcharCounter;

    iX--;
    iY--;
    memcpy(pcharBuffer, pcharSource, iWidth * iHeight * sizeof(CHARACTER));
    pcharCounter = pcharBuffer;
    for (iCounterY = 0; iCounterY < iHeight; iCounterY++)
      for (iCounterX = 0; iCounterX < iWidth; iCounterX++, pcharCounter++)
      {
        if (pcharCounter->character == (CHAR)0xff)
        {
          coordCoord.X = iX + iCounterX;
          coordCoord.Y = iY + iCounterY;
          ReadConsoleOutputCharacter(hStdOut, &pcharCounter->character, 1, coordCoord, &dwRead);
        }
        if (pcharCounter->attribute == (CHAR)0xff)
        {
          coordCoord.X = iX + iCounterX;
          coordCoord.Y = iY + iCounterY;
          ReadConsoleOutputAttribute(hStdOut, (WORD*)&pcharCounter->attribute, 1, coordCoord, &dwRead);
        }
      }
    coordSize.X = iWidth;
    coordSize.Y = iHeight;
    coordCoord.X = coordCoord.Y = 0;
    srct.Top = iY;
    srct.Left = iX;
    srct.Bottom = iY + iHeight - 1;
    srct.Right = iX + iWidth - 1;
    WriteConsoleOutput(hStdOut, (PCHAR_INFO)pcharBuffer, coordSize, coordCoord, &srct);
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
        if (pcharSource->attribute != (CHAR)0xff)
          pcharDest->attribute = pcharSource->attribute;
      }
      pcharDest += iJump;
    }
  }
}

