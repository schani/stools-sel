/***************************************************************************
 ***************************************************************************
 ***                                                                     ***
 ***             Schani Electronics Language - Interpreter               ***
 ***                                                                     ***
 ***                        Modulglobaler Teil                           ***
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

void mod_init_locals (SEL_PROGRAM *pprogProgram)
{                
  
  SEL_MODULE *pmodModule;
  UINT        uiCounter;
     
  pmodModule = pprogProgram->ppmodModules[pprogProgram->uiMP];
  if (pmodModule->uiLocals)
    pprogProgram->ucBlock = SEL_BLOCK_LOCAL;
  else
    return;
  for (uiCounter = 0; uiCounter < pmodModule->uiLocals; uiCounter++)
  {
    pprogProgram->ulIP = pmodModule->pulLocals[uiCounter];
    par_local(pprogProgram);
  }                                    
  pprogProgram->ucBlock = SEL_BLOCK_NONE;
}

void mod_init_globals (SEL_PROGRAM *pprogProgram)
{                
  
  SEL_MODULE *pmodModule;
  UINT        uiCounter;
     
  pmodModule = pprogProgram->ppmodModules[pprogProgram->uiMP];
  if (pmodModule->uiGlobals)
    pprogProgram->ucBlock = SEL_BLOCK_GLOBAL;
  else
    return;
  for (uiCounter = 0; uiCounter < pmodModule->uiGlobals; uiCounter++)
  {
    pprogProgram->ulIP = pmodModule->pulGlobals[uiCounter];
    par_global(pprogProgram);
  }              
  pprogProgram->ucBlock = SEL_BLOCK_NONE;
}                                           

void mod_add_func (SEL_PROGRAM *pprogProgram, SEL_FUNC *pfuncFunction)
{
  
  SEL_MODULE    *pmodModule;
  SEL_FUNC_LIST *pfunclList;
  SEL_PARAM     *pparam1,
                *pparam2;
  UINT           uiHashValue;

  pmodModule = pprogProgram->ppmodModules[pprogProgram->uiMP];
  uiHashValue = prg_hash(pfuncFunction->pcName);
  for (pfunclList = pmodModule->ppfunclFuncs[uiHashValue];
       pfunclList; pfunclList = pfunclList->pfunclNext)
    if (!strcmp(pfunclList->pfuncFunction->pcName, pfuncFunction->pcName))
    {
      pfunclList->pfuncFunction->uiMP = pfuncFunction->uiMP;
      pfunclList->pfuncFunction->ulIP = pfuncFunction->ulIP;
      for (pparam1 = pfunclList->pfuncFunction->pparamFirst,
           pparam2 = pfuncFunction->pparamFirst; pparam1 && pparam2;
           pparam1 = pparam1->pparamNext, pparam2 = pparam2->pparamNext)
        pparam1->pcName = pparam2->pcName;
      break;
    }
  if (!pfunclList)
  {
    for (pfunclList = pprogProgram->ppfunclFuncs[uiHashValue];
         pfunclList; pfunclList = pfunclList->pfunclNext)
      if (!strcmp(pfunclList->pfuncFunction->pcName, pfuncFunction->pcName))
      {
        pfunclList->pfuncFunction->uiMP = pfuncFunction->uiMP;
        pfunclList->pfuncFunction->ulIP = pfuncFunction->ulIP;
        for (pparam1 = pfunclList->pfuncFunction->pparamFirst,
             pparam2 = pfuncFunction->pparamFirst; pparam1 && pparam2;
             pparam1 = pparam1->pparamNext, pparam2 = pparam2->pparamNext)
          pparam1->pcName = pparam2->pcName;
        break;
      }
  }
  if (!pfunclList)
  {
    pfunclList = utl_alloc(sizeof(SEL_FUNC_LIST));
    pfunclList->pfuncFunction = pfuncFunction;
    pfunclList->pfunclNext = pmodModule->ppfunclFuncs[uiHashValue];
    pmodModule->ppfunclFuncs[uiHashValue] = pfunclList;
  }
  else
    man_delete_func(pfuncFunction);
}
  
void mod_init_funcs (SEL_PROGRAM *pprogProgram)
{                                             
  
  SEL_MODULE    *pmodModule;
  UINT           uiCounter;
  
  pmodModule = pprogProgram->ppmodModules[pprogProgram->uiMP];
  for (uiCounter = 0; uiCounter < pmodModule->uiFuncs; uiCounter++)
  {
    pprogProgram->ulIP = pmodModule->pulFuncs[uiCounter];
    mod_add_func(pprogProgram, par_sub(pprogProgram));
  }
}

SEL_MODULE* mod_init (SEL_PROGRAM *pprogProgram, CHAR *pcFileName,
                      UINT *puiGlobalStructs, UINT *puiLocalStructs)
{
         
  SEL_MODULE *pmodReturnVar;
  FILE       *pfileModule;
  UCHAR       ucVersion;
  ULONG       ulSymbolLength,
              ulSourceLength;
  UINT        uiSymbols,
              uiCounter,
              uiStructs;
  CHAR       *pcCounter;
                                              
  lex_tokenize_source(pcFileName, "temp.so");
  pfileModule = fopen("temp.so", "r");
  fread(&ucVersion, sizeof(UCHAR), 1, pfileModule);
  if (ucVersion != SEL_FILE_FORMAT)
    return(NULL);
  pmodReturnVar = utl_alloc(sizeof(SEL_MODULE));
  pmodReturnVar->ppvarlVariables = utl_calloc(SEL_HASH_SIZE,
                                              sizeof(SEL_VAR_LIST));
  pmodReturnVar->ppstructlStructs = utl_calloc(SEL_HASH_SIZE, 
                                               sizeof(SEL_STRUCT_LIST));
  pmodReturnVar->ppfunclFuncs = utl_calloc(SEL_HASH_SIZE,
                                           sizeof(SEL_FUNC_LIST*));
  fread(&uiSymbols, sizeof(UINT), 1, pfileModule);
  pmodReturnVar->psymSymbols = utl_alloc(uiSymbols * sizeof(SEL_SYMBOL));
  fread(&ulSymbolLength, sizeof(ULONG), 1, pfileModule);
  pmodReturnVar->pcSymbolTable = utl_alloc(ulSymbolLength);
  fread(pmodReturnVar->pcSymbolTable, sizeof(CHAR), ulSymbolLength,
        pfileModule);                                              
  for (uiCounter = 0, pcCounter = pmodReturnVar->pcSymbolTable;
       uiCounter < uiSymbols; uiCounter++)
  {
    pmodReturnVar->psymSymbols[uiCounter].pcName = pcCounter;
    pmodReturnVar->psymSymbols[uiCounter].uiHashValue = prg_hash(pcCounter);
    pcCounter += strlen(pcCounter) + 1;
  }
  pmodReturnVar->uiFirstStruct = *puiLocalStructs + 20000;
  fread(&uiStructs, sizeof(UINT), 1, pfileModule);
  pmodReturnVar->pstructStructs = utl_alloc(uiStructs * sizeof(SEL_STRUCT));
  *puiLocalStructs += uiStructs;
  fread(&uiStructs, sizeof(UINT), 1, pfileModule);
  *puiGlobalStructs += uiStructs;
  fread(&(pmodReturnVar->uiLocals), sizeof(UINT), 1, pfileModule);
  pmodReturnVar->pulLocals = utl_alloc(pmodReturnVar->uiLocals *
                                       sizeof(ULONG));
  fread(pmodReturnVar->pulLocals, sizeof(ULONG), pmodReturnVar->uiLocals,
        pfileModule);
  fread(&(pmodReturnVar->uiGlobals), sizeof(UINT), 1, pfileModule);
  pmodReturnVar->pulGlobals = utl_alloc(pmodReturnVar->uiGlobals *
                                        sizeof(ULONG));
  fread(pmodReturnVar->pulGlobals, sizeof(ULONG), pmodReturnVar->uiGlobals,
        pfileModule);
  fread(&(pmodReturnVar->uiFuncs), sizeof(UINT), 1, pfileModule);
  pmodReturnVar->pulFuncs = utl_alloc(pmodReturnVar->uiFuncs * sizeof(ULONG));
  fread(pmodReturnVar->pulFuncs, sizeof(ULONG), pmodReturnVar->uiFuncs,
        pfileModule);
  fread(&(pmodReturnVar->uiLines), sizeof(UINT), 1, pfileModule);        
  pmodReturnVar->pucLines = utl_alloc(pmodReturnVar->uiLines);
  fread(pmodReturnVar->pucLines, pmodReturnVar->uiLines, 1, pfileModule);
  fread(&ulSourceLength, sizeof(ULONG), 1, pfileModule);
  pmodReturnVar->pucSource = utl_alloc(ulSourceLength);
  fread(pmodReturnVar->pucSource, sizeof(UCHAR), ulSourceLength, pfileModule);
  pmodReturnVar->uiStructs = 0;
  return(pmodReturnVar);
}
