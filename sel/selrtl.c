/***************************************************************************
 ***************************************************************************
 ***                                                                     ***
 ***             Schani Electronics Language - Interpreter               ***
 ***                                                                     ***
 ***                          RunTime Library                            ***
 ***                                                                     ***
 ***                  (c) 1993 by Schani Electronics                     ***
 ***                                                                     ***
 ***************************************************************************
 ***************************************************************************/

#include <stdio.h>
#ifndef _UNIX
#include <conio.h>
#endif
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <fcntl.h>
#ifdef __TURBOC__
#include <alloc.h>
#endif
#ifdef _WINNT
#include <windows.h>
#endif
#include <gemein.h>
#include <video.h>
#include <mouse.h>
#include <sthelp.h>
#include <dlg.h>
#include <window.h>
#include <utility.h>
#include <global.h>
#include <contain.h>
#include <buffers.h>
#include "sel.h"

#ifdef _WINNT
HANDLE hStdOut = INVALID_HANDLE_VALUE;
#endif

void rtl_write_single_char (CHAR cChar)
{
  
  CHAR acBuffer[3];
  
  if (cChar == '\n')
  {
    acBuffer[0] = '\r';
    acBuffer[1] = '\n';
    acBuffer[2] = 0;
  }
  else
  {
    acBuffer[0] = cChar;
    acBuffer[1] = 0;
  }
  fputs(acBuffer, stdout);
}
                                                        
SEL_EXPR* rtl_write_int (SEL_PROGRAM *pprogProgram)
{                   
  
  TYPE_INT      iInt;
  SEL_VARIABLE *pvarVariable;
                
  pvarVariable = man_get_variable(pprogProgram, "i");
  iInt = (TYPE_INT)pvarVariable->ulValue;
  printf("%ld", iInt);
  return(NULL);
}                

SEL_EXPR* rtl_read_int (SEL_PROGRAM *pprogProgram)
{                   
  
  TYPE_INT      iInt         = 0;
  UCHAR         ucSign       = FALSE;
  UINT          uiPos        = 0;
  CHAR          cChar;
                 
read_sign_again :
  cChar = getchar();
  if (cChar == '-')
  {
    ucSign = TRUE;
    cChar = getchar();  
    uiPos++;
  }
  while ((cChar >= 48 && cChar < 58) || cChar == 8)
  {                                             
    if (cChar == 8)
    {
      iInt /= 10;
      if (uiPos > 0)
        uiPos--;
      if (uiPos == 0)
      {
        ucSign = FALSE;
        goto read_sign_again;
      }
    }                        
    else   
    {
      iInt = iInt * 10 + cChar - 48;
      uiPos++;
    }
    cChar = getchar();
  }
  if (ucSign)
    iInt = -iInt;
#ifndef _UNIX
  puts("");
#endif
  return(man_int_expr(iInt, man_alloc_expr()));
}                

SEL_EXPR* rtl_write_float (SEL_PROGRAM *pprogProgram)
{                   
  
  SEL_VARIABLE *pvarVariable;
                
  pvarVariable = man_get_variable(pprogProgram, "f");
  printf("%f", pvarVariable->fValue);
  return(NULL);
}                

SEL_EXPR* rtl_read_float (SEL_PROGRAM *pprogProgram)
{
  
  TYPE_FLOAT fFloat;
  
  scanf("%f", &fFloat);
  return(man_float_expr(fFloat, man_alloc_expr()));
}

SEL_EXPR* rtl_write_char (SEL_PROGRAM *pprogProgram)
{

  rtl_write_single_char(man_get_variable(pprogProgram, "c")->ulValue);
  return(NULL);
}

SEL_EXPR* rtl_read_char (SEL_PROGRAM *pprogProgram)
{
  return(man_char_expr(getchar(), man_alloc_expr()));
}

SEL_EXPR* rtl_write_string (SEL_PROGRAM *pprogProgram)
{
   
  TYPE_CHAR    *pcString;
  SEL_VARIABLE *pvarVariable;
  
  pvarVariable = man_get_variable(pprogProgram, "s");
  pcString = (TYPE_CHAR*)pvarVariable->ulValue;
  for (; *pcString; pcString++)
    rtl_write_single_char(*pcString);
  return(NULL);
}

SEL_EXPR* rtl_read_string (SEL_PROGRAM *pprogProgram)
{
  
  TYPE_CHAR    *pcString;
  SEL_VARIABLE *pvarVariable;
  SEL_EXPR     *pexprExpr;

  pvarVariable = man_get_variable(pprogProgram, "s");
  pcString = (TYPE_CHAR*)pvarVariable->ulValue;
  pexprExpr = man_alloc_expr();
  if (!gets(pcString))
    return(man_int_expr(0, pexprExpr));
  return(man_int_expr(strlen(pcString) + 1, pexprExpr));
}

SEL_EXPR* rtl_randomize (SEL_PROGRAM *pprogProgram)
{

  time_t timeTime;

  srand(time(&timeTime));
  return(NULL);
}

SEL_EXPR* rtl_random (SEL_PROGRAM *pprogProgram)
{
  
  TYPE_INT      iInt;
  SEL_VARIABLE *pvarVariable;            
                
  pvarVariable = man_get_variable(pprogProgram, "i");
  iInt = (TYPE_INT)pvarVariable->ulValue;
  return(man_int_expr(rand() % iInt, man_alloc_expr()));
}

SEL_EXPR* rtl_mem_alloc (SEL_PROGRAM *pprogProgram)
{
  
  TYPE_INT  iInt;
  void     *pPointer;

  iInt = (TYPE_INT)man_get_variable(pprogProgram, "s")->ulValue;
  pPointer = utl_alloc((UINT)iInt);
  return(man_pointer_expr(pPointer, man_copy_type(SEL_VOID),
                          man_alloc_expr()));
}

SEL_EXPR* rtl_mem_re_alloc (SEL_PROGRAM *pprogProgram)
{
  
  TYPE_INT  iInt;
  void     *pPointer;

  iInt = (TYPE_INT)man_get_variable(pprogProgram, "s")->ulValue;
  pPointer = (void*)man_get_variable(pprogProgram, "p")->ulValue;
  pPointer = utl_realloc(pPointer, (UINT)iInt);
  return(man_pointer_expr(pPointer, man_copy_type(SEL_VOID),
                          man_alloc_expr()));
}

SEL_EXPR* rtl_mem_free (SEL_PROGRAM *pprogProgram)
{
  utl_free((void*)man_get_variable(pprogProgram, "p")->ulValue);
  return(NULL);
}

SEL_EXPR* rtl_mem_avail (SEL_PROGRAM *pprogProgram)
{
#ifdef __TURBOC__
  return(man_int_expr(coreleft(), man_alloc_expr()));
#else
#ifdef _WINNT
  MEMORYSTATUS mstMemStat;

  mstMemStat.dwLength = sizeof(MEMORYSTATUS);
  GlobalMemoryStatus(&mstMemStat);
  return(man_int_expr(mstMemStat.dwAvailPageFile, man_alloc_expr()));
#else
  return(man_int_expr(0, man_alloc_expr()));
#endif
#endif
}

SEL_EXPR* rtl_vio_w_p_c_a (SEL_PROGRAM *pprogProgram)
{

  TYPE_CHAR                  cChar,
                             cAttri;
#ifdef _WINNT
  COORD                      coord;
  CONSOLE_SCREEN_BUFFER_INFO csbi;
  DWORD                      dwCells,
                             dwWritten;
#endif

  cChar = (TYPE_CHAR)(man_get_variable(pprogProgram, "c")->ulValue);
  cAttri = (TYPE_CHAR)(man_get_variable(pprogProgram, "a")->ulValue);
#ifdef _WINNT
  coord.X = 0;
  coord.Y = 0;
  if (hStdOut == INVALID_HANDLE_VALUE)
    hStdOut = GetStdHandle(STD_OUTPUT_HANDLE);
  GetConsoleScreenBufferInfo(hStdOut, &csbi);
  dwCells = csbi.dwSize.X * csbi.dwSize.Y;
  FillConsoleOutputCharacter(hStdOut, cChar, dwCells, coord, &dwWritten);
  FillConsoleOutputAttribute(hStdOut, cAttri, dwCells, coord, &dwWritten);
#elif defined(_UNIX)
    vio_sp_za(cChar, cAttri);
#else
    printf("vioWPCA not supported\n");
#endif
  return(NULL);
}

SEL_EXPR* rtl_vio_w_w_c_a (SEL_PROGRAM *pprogProgram)
{
        
  TYPE_INT  iX,
            iY,
            iWidth,
            iHeight;
  TYPE_CHAR cChar,
            cAttri;
#ifdef _WINNT
  COORD     coord;
  DWORD     dwWritten;
#endif
                
  iX = (TYPE_INT)(man_get_variable(pprogProgram, "x")->ulValue);
  iY = (TYPE_INT)(man_get_variable(pprogProgram, "y")->ulValue);
  iWidth = (TYPE_INT)(man_get_variable(pprogProgram, "w")->ulValue);
  iHeight = (TYPE_INT)(man_get_variable(pprogProgram, "h")->ulValue);
  cChar = (TYPE_CHAR)(man_get_variable(pprogProgram, "c")->ulValue);
  cAttri = (TYPE_CHAR)(man_get_variable(pprogProgram, "a")->ulValue);
#ifdef _WINNT
  coord.X = iX - 1;
  iHeight += --iY;
  if (hStdOut == INVALID_HANDLE_VALUE)
    hStdOut = GetStdHandle(STD_OUTPUT_HANDLE);
  for (; iY < iHeight; iY++)
  {
    coord.Y = (SHORT)iY;
    FillConsoleOutputCharacter(hStdOut, cChar, (DWORD)iWidth, coord, &dwWritten);
    FillConsoleOutputAttribute(hStdOut, cAttri, (DWORD)iWidth, coord, &dwWritten);
  }
#elif defined(_UNIX)
    vio_sw_za(iX, iY, iWidth, iHeight, cChar, cAttri);
#else
    printf("vioWWCA not supported\n");
#endif
  return(NULL);
}

SEL_EXPR* rtl_vio_redraw (SEL_PROGRAM *pprogProgram)
{
    vio_redraw();
    return NULL;
}
