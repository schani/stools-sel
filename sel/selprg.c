/***************************************************************************
 ***************************************************************************
 ***                                                                     ***
 ***             Schani Electronics Language - Interpreter               ***
 ***                                                                     ***
 ***                      Programmglobaler Teil                          ***
 ***                                                                     ***
 ***                  (c) 1993 by Schani Electronics                     ***
 ***                                                                     ***
 ***************************************************************************
 ***************************************************************************/

#include <stdio.h>
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
 
void prg_init_rtl_func (SEL_PROGRAM *pprogProgram, CHAR *pcName,
                        UCHAR *pucReturnType, SEL_PARAM *pparamFirst,
                        SEL_RTL_FUNC rtl_func)
{
  
  SEL_FUNC      *pfuncFunction;
  SEL_FUNC_LIST *pfunclList;

  pfunclList = utl_alloc(sizeof(SEL_FUNC_LIST));
  pfunclList->pfuncFunction = pfuncFunction = utl_alloc(sizeof(SEL_FUNC));
  pfuncFunction->pcName = pcName;
  pfuncFunction->pucReturnType = pucReturnType;
  pfuncFunction->pparamFirst = pparamFirst;
  pfuncFunction->ucType = SEL_FUNC_RTL;
  pfuncFunction->rtl_func = rtl_func;
  pfuncFunction->uiMP = 0;
  pfunclList->pfunclNext = pprogProgram->ppfunclFuncs[prg_hash(pcName)];
  pprogProgram->ppfunclFuncs[prg_hash(pcName)] = pfunclList;
}

SEL_PARAM* prg_param (CHAR *pcName, UCHAR *pucType, SEL_PARAM *pparamNext)
{
  
  SEL_PARAM *pparamReturnVar;
  
  pparamReturnVar = utl_alloc(sizeof(SEL_PARAM));
  pparamReturnVar->pcName = pcName;
  pparamReturnVar->pucType = pucType;
  pparamReturnVar->pparamNext = pparamNext;
  return(pparamReturnVar);
}
                                
void prg_init_run_time_lib (SEL_PROGRAM *pprogProgram)
{
  prg_init_rtl_func(pprogProgram, "WriteInt", SEL_VOID,
                    prg_param("i", SEL_INT, NULL), rtl_write_int);
  prg_init_rtl_func(pprogProgram, "ReadInt", SEL_INT, NULL, rtl_read_int);
  prg_init_rtl_func(pprogProgram, "WriteFloat", SEL_VOID,
                    prg_param("f", SEL_FLOAT, NULL), rtl_write_float);
  prg_init_rtl_func(pprogProgram, "ReadFloat",
                    SEL_FLOAT, NULL, rtl_read_float);
  prg_init_rtl_func(pprogProgram, "WriteChar", SEL_VOID,
                    prg_param("c", SEL_CHAR, NULL), rtl_write_char);
  prg_init_rtl_func(pprogProgram, "ReadChar", SEL_CHAR, NULL, rtl_read_char);
  prg_init_rtl_func(pprogProgram, "WriteString", SEL_VOID,
                    prg_param("s", SEL_STRING, NULL), rtl_write_string);
  prg_init_rtl_func(pprogProgram, "ReadString", SEL_INT, 
                    prg_param("s", SEL_STRING, NULL), rtl_read_string);
  prg_init_rtl_func(pprogProgram, "Randomize", SEL_VOID, NULL, rtl_randomize);
  prg_init_rtl_func(pprogProgram, "Random", SEL_INT, 
                    prg_param("i", SEL_INT, NULL), rtl_random);           
  prg_init_rtl_func(pprogProgram, "MemAlloc", SEL_POINTER,
                    prg_param("s", SEL_INT, NULL), rtl_mem_alloc);
  prg_init_rtl_func(pprogProgram, "MemReAlloc", SEL_POINTER,
                    prg_param("p", SEL_POINTER,
                    prg_param("s", SEL_INT, NULL)), rtl_mem_re_alloc);
  prg_init_rtl_func(pprogProgram, "MemFree", SEL_VOID,
                    prg_param("p", SEL_POINTER, NULL), rtl_mem_free);
  prg_init_rtl_func(pprogProgram, "MemAvail", SEL_INT, NULL, rtl_mem_avail);
  prg_init_rtl_func(pprogProgram, "VioWPCA", SEL_VOID, 
                    prg_param("c", SEL_CHAR, prg_param("a", SEL_CHAR, NULL)),
                    rtl_vio_w_p_c_a);
  prg_init_rtl_func(pprogProgram, "VioWWCA", SEL_VOID,
                    prg_param("x", SEL_INT,
                    prg_param("y", SEL_INT,
                    prg_param("w", SEL_INT,
                    prg_param("h", SEL_INT,
                    prg_param("c", SEL_CHAR,
                    prg_param("a", SEL_CHAR, NULL)))))), rtl_vio_w_w_c_a);
  prg_init_rtl_func(pprogProgram, "VioRedraw", SEL_VOID, NULL, rtl_vio_redraw);
}

UINT prg_hash (CHAR *pcString)
{
  
  UINT uiReturnVar = 0;
  
  for (; *pcString; pcString++)
    uiReturnVar = (uiReturnVar * 256 + *pcString) % SEL_HASH_SIZE;
  return(uiReturnVar);
}     

SEL_EXPR* prg_call_func (SEL_PROGRAM *pprogProgram, CHAR *pcName,
                         SEL_EXPR *pexprFirst)
{
  
  SEL_FUNC      *pfuncFunction;             
  SEL_FUNC_LIST *pfunclCounter;
  SEL_EXPR      *pexprCounter;
  SEL_PARAM     *pparamCounter;
  SEL_VARIABLE  *pvarVariable;
  SEL_MODULE    *pmodModule;
  UINT           uiHashValue;
  
  pmodModule = pprogProgram->ppmodModules[pprogProgram->uiMP];
  uiHashValue = prg_hash(pcName);
  for (pfunclCounter = pmodModule->ppfunclFuncs[uiHashValue];
       pfunclCounter; pfunclCounter = pfunclCounter->pfunclNext)
    if (!strcmp(pcName, pfunclCounter->pfuncFunction->pcName))
      break;  
  if (!pfunclCounter)
    for (pfunclCounter = pprogProgram->ppfunclFuncs[uiHashValue];
         pfunclCounter; pfunclCounter = pfunclCounter->pfunclNext)
      if (!strcmp(pcName, pfunclCounter->pfuncFunction->pcName))
        break;
  if (!pfunclCounter || pfunclCounter->pfuncFunction->uiMP == (UINT)-1)
    man_error(pprogProgram, 14, pcName);
  pfuncFunction = pfunclCounter->pfuncFunction;
  pprogProgram->pfstkFuncStack[pprogProgram->uiFP].pfuncFunction =
    pfuncFunction;
  pprogProgram->pfstkFuncStack[pprogProgram->uiFP].uiMP = pprogProgram->uiMP;
  pprogProgram->pfstkFuncStack[pprogProgram->uiFP].ulIP = pprogProgram->ulIP;
  pprogProgram->pfstkFuncStack[pprogProgram->uiFP].pvartVariables = NULL;
  pprogProgram->uiFP++;
  pprogProgram->ucBlock = SEL_BLOCK_FUNC;
  for (pparamCounter = pfuncFunction->pparamFirst, pexprCounter = pexprFirst;
       pparamCounter;
       pparamCounter = pparamCounter->pparamNext,
       pexprCounter = pexprCounter->pexprNext)
  {
    pvarVariable = man_add_variable(pprogProgram, pparamCounter->pcName,
                                    man_copy_type(pparamCounter->pucType));
    man_assign_variable(pprogProgram, pvarVariable, pexprCounter);
  }
  pprogProgram->ucReason = 0;
  if (pfuncFunction->ucType == SEL_FUNC_SEL)
  {
    pprogProgram->uiMP = pfuncFunction->uiMP;
    pprogProgram->ulIP = pfuncFunction->ulIP;    
    par_func_vars(pprogProgram);
    par_get_token(pprogProgram, BEGIN, 18);
    pexprCounter = par_statement(pprogProgram);
  }
  else
    pexprCounter = pfuncFunction->rtl_func(pprogProgram);
  if (pfuncFunction->pucReturnType[1] == SEL_TYPE_VOID && pexprCounter)
  {
    man_delete_expr(pexprCounter);
    man_free_expr(pexprCounter);
    pexprCounter = NULL;
  }
  else                                                                
    if (pexprCounter)
      man_cast(pprogProgram, pexprCounter,
               man_copy_type(pfuncFunction->pucReturnType));
  pprogProgram->uiFP--;
  pprogProgram->uiMP = pprogProgram->pfstkFuncStack[pprogProgram->uiFP].uiMP;
  pprogProgram->ulIP = pprogProgram->pfstkFuncStack[pprogProgram->uiFP].ulIP;
  if (pprogProgram->pfstkFuncStack[pprogProgram->uiFP].pvartVariables)
    man_delete_var_tree(pprogProgram->pfstkFuncStack[pprogProgram->uiFP].
                          pvartVariables);
  return(pexprCounter);
}

SEL_EXPR* prg_run_func (SEL_PROGRAM *pprogProgram, CHAR *pcName,
                        SEL_EXPR *pexprFirst)
{
  if (setjmp(pprogProgram->jbErrorJump))
    return(NULL);                 
  return(prg_call_func(pprogProgram, pcName, pexprFirst));
}

SEL_PROGRAM* prg_init (CHAR **ppcModules)
{
  
  SEL_PROGRAM  *pprogReturnVar;                    
  UINT          uiGlobalStructs      = 0,
                uiLocalStructs       = 0,
                uiCounter;
  
  pprogReturnVar = utl_alloc(sizeof(SEL_PROGRAM));
  pprogReturnVar->pptypelTypes = utl_calloc(SEL_HASH_SIZE,
                                            sizeof(SEL_TYPE_LIST*));
  pprogReturnVar->ppstructlStructs = utl_calloc(SEL_HASH_SIZE,
                                                sizeof(SEL_STRUCT_LIST*));
  pprogReturnVar->ppfunclFuncs = utl_calloc(SEL_HASH_SIZE,
                                            sizeof(SEL_FUNC_LIST*));
  pprogReturnVar->ppvarlVariables = utl_calloc(SEL_HASH_SIZE,
                                               sizeof(SEL_VAR_LIST*));
  pprogReturnVar->pfstkFuncStack = utl_alloc(SEL_FUNC_STACK_SIZE *
                                             sizeof(SEL_FUNC_STACK));
  pprogReturnVar->uiFP = pprogReturnVar->uiPP = 0;
  pprogReturnVar->ucBlock = SEL_BLOCK_NONE;
  pprogReturnVar->uiStructs = 0;
  for (pprogReturnVar->uiModules = 0; ppcModules[pprogReturnVar->uiModules];
       pprogReturnVar->uiModules++)
    ;           
  pprogReturnVar->ppcModules =
    utl_alloc(sizeof(CHAR*) * pprogReturnVar->uiModules);
  for (uiCounter = 0; uiCounter < pprogReturnVar->uiModules; uiCounter++)
  {
    pprogReturnVar->ppcModules[uiCounter] =
      utl_alloc(strlen(ppcModules[uiCounter]) + 1);
    strcpy(pprogReturnVar->ppcModules[uiCounter], ppcModules[uiCounter]);
  }
  pprogReturnVar->ppmodModules = utl_alloc(pprogReturnVar->uiModules * 
                                           sizeof(SEL_MODULE*));
  if (setjmp(pprogReturnVar->jbErrorJump))
    return(NULL);                 
  for (uiCounter = 0; uiCounter < pprogReturnVar->uiModules; uiCounter++)
  {
    pprogReturnVar->ppmodModules[uiCounter] = mod_init(pprogReturnVar,
                                                       ppcModules[uiCounter],
                                                       &uiGlobalStructs,
                                                       &uiLocalStructs);
    if (!pprogReturnVar->ppmodModules[uiCounter])
      return(NULL);
  }
  pprogReturnVar->pstructStructs = utl_alloc(uiGlobalStructs *
                                             sizeof(SEL_STRUCT*));
  for (uiCounter = uiGlobalStructs = 0; uiCounter < pprogReturnVar->uiModules;
       uiCounter++) 
  {
    pprogReturnVar->uiMP = uiCounter;
    mod_init_locals(pprogReturnVar);
    mod_init_globals(pprogReturnVar);
    mod_init_funcs(pprogReturnVar);
  }
  prg_init_run_time_lib(pprogReturnVar);
  return(pprogReturnVar);
}
