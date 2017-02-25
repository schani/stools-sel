/***************************************************************************
 ***************************************************************************
 ***                                                                     ***
 ***             Schani Electronics Language - Interpreter               ***
 ***                                                                     ***
 ***                            Hauptteil                                ***
 ***                                                                     ***
 ***                 (c) 1993,94 by Schani Electronics                   ***
 ***                                                                     ***
 ***************************************************************************
 ***************************************************************************/

#include <stdio.h>
#ifndef _UNIX
#include <conio.h>
#endif
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
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

#ifndef _UNIX
extern unsigned _stklen = 16384;
#endif

extern SEL_TYPES      tTypes;
extern SEL_EXPRS      exprsExprs;
extern SEL_VAR_BLOCKS vbVarBlocks;

void sel_init (void)
{
  
  UINT uiCounter;

  tTypes.pucTypes = utl_alloc(SEL_TYPE_SIZE * SEL_TYPE_POOL_SIZE);
  tTypes.puiFreeTypes = utl_alloc(sizeof(UINT) * SEL_TYPE_POOL_SIZE);
  for (uiCounter = 0; uiCounter < SEL_TYPE_POOL_SIZE; uiCounter++)
    tTypes.puiFreeTypes[uiCounter] = uiCounter;
  tTypes.uiNextFree = 0;
  exprsExprs.pexprExprs = utl_alloc(sizeof(SEL_EXPR) * SEL_EXPR_POOL_SIZE);
  exprsExprs.puiFreeExprs = utl_alloc(sizeof(UINT) * SEL_EXPR_POOL_SIZE);
  for (uiCounter = 0; uiCounter < SEL_EXPR_POOL_SIZE; uiCounter++)
    exprsExprs.puiFreeExprs[uiCounter] = uiCounter;
  exprsExprs.uiNextFree = 0;
  vbVarBlocks.ppvartVarTrees = utl_alloc(sizeof(SEL_VAR_TREE*) *
                                           SEL_VAR_POOL_SIZE);
  vbVarBlocks.uiTreesUsed = 0;
}

int
sel_run (const char *program)
{
    char *fns[] = { "/tmp/bla.sel", NULL };
    FILE *f = fopen(fns[0], "w");
    if (f == NULL)
        return -1;
    size_t len = strlen(program);
    size_t written = fwrite(program, 1, len, f);
    fclose(f);
    if (written != len)
        return -1;

    sel_init();
    reg_init(200);

    SEL_PROGRAM *prg = prg_init(fns);
    if (!prg)
        return -1;
    
    SEL_EXPR *result = prg_run_func(prg, "main", NULL);
    if (result)
        return (int)man_int_value(result);
    return 0;
}

#if 0
int main (int argc, char *argv[])
{         
  
  SEL_PROGRAM *pprogProgram;        
  SEL_EXPR    *pexprReturnVar;

  if (argc <= 1)
  {
    printf("SEL-Interpreter V 0.3\n"
           "(c) 1993,94 by Schani Electronics\n"
           "\n"
           "Syntax:\n"
#ifdef _MSDOS
           "  sel <file> [<file>...]\n"
#endif
#ifdef _WINNT
           "  selnt <file> [<file>...]\n"
#endif
#ifdef _OS2
           "  selos2 <file> [<file>...]\n"
#endif
           "\n"
           "  file ... SEL source file\n");
    return(1);
  }
#ifndef _UNIX
  _fmode = O_BINARY;
  vio_init();
#endif
  sel_init();
  reg_init(200);
  pprogProgram = prg_init(argv + 1);
  if (!pprogProgram)
  {
    printf("Error opening files...\n");
    return(1);                         
  }
  pexprReturnVar = prg_run_func(pprogProgram, "main", NULL);

#ifdef _DEBUG
  getch();
#endif

  if (pexprReturnVar)
    return(man_int_value(pexprReturnVar));
  return(0);
}
#endif
