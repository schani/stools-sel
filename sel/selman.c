/***************************************************************************
 ***************************************************************************
 ***                                                                     ***
 ***             Schani Electronics Language - Interpreter               ***
 ***                                                                     ***
 ***                            Management                               ***
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

UCHAR aucTypeInt[]      = {
                            1,
                            SEL_TYPE_INT
                          },
      aucTypeFloat[]    = {
                            1,
                            SEL_TYPE_FLOAT
                          },
      aucTypeChar[]     = {
                            1,
                            SEL_TYPE_CHAR
                          },
      aucTypeVoid[]     = {
                            1,
                            SEL_TYPE_VOID
                          },
      aucTypeString[]   = {
                            2,
                            SEL_TYPE_POINTER,
                            SEL_TYPE_CHAR
                          },
      aucTypePointer[]  = {
                            2,
                            SEL_TYPE_POINTER,
                            SEL_TYPE_VOID
                          };
CHAR *acErrorMessages[] = {
                            "\'(\' expected",
                            "\')\' expected",
                            "\']\' expected",
                            "\':\' expected",
                            "\';\' expected",
                            "identifier expected",
                            "integer constant expected",
                            "\'end\' expected",
                            "\'local\' expected",
                            "\'global\' expected",
                            "\'until\' expected",
                            "\'function\' expected",
                            "\'case\' or \'default\' expected",
                            "type expected",
                            "unknown function %s",
                            "unknown variable %s",
                            "\'extern\' can only be used in global blocks",
                            "cannot cast from %s",
                            "\'begin\' expected",
                            "expression expected",
                            "block register overflow",
                            "stack register overflow",
                            "cannot negate char's or string's",
                            "constant expected",
                            "reference expected",
                            "\'=\' expected",
                            "\'to\' or \'downto\' expected"
                          },
     *acFatalMessages[] = {
                            "type-pool exceeded",
                            "expression-pool exceeded"
                          };
SEL_TYPES      tTypes;                                
SEL_EXPRS      exprsExprs;
SEL_VAR_BLOCKS vbVarBlocks;
                             
UCHAR* man_alloc_type (void)
{     
  
  UCHAR *pucReturnVar;
                 
  if (tTypes.uiNextFree == SEL_TYPE_POOL_SIZE)
    man_fatal_error(0, NULL);
  pucReturnVar = tTypes.pucTypes + tTypes.puiFreeTypes[tTypes.uiNextFree++] * 
                                     SEL_TYPE_SIZE;
  return(pucReturnVar);
}

void man_free_type (UCHAR *pucType)
{ 
  if (pucType != aucTypeInt && pucType != aucTypeChar &&
      pucType != aucTypeFloat && pucType != aucTypeVoid &&
      pucType != aucTypeString && pucType != aucTypePointer)
    tTypes.puiFreeTypes[--tTypes.uiNextFree] =
      (pucType - tTypes.pucTypes) / SEL_TYPE_SIZE;
}

SEL_EXPR* man_alloc_expr (void)
{     
  
  SEL_EXPR *pexprReturnVar;
  
  if (exprsExprs.uiNextFree == SEL_EXPR_POOL_SIZE)
    man_fatal_error(1, NULL);
  pexprReturnVar = exprsExprs.pexprExprs +
                     exprsExprs.puiFreeExprs[exprsExprs.uiNextFree++];
  return(pexprReturnVar);
}

void man_free_expr (SEL_EXPR *pexprExpr)
{
  if (pexprExpr->pexprLeft)
    man_free_expr(pexprExpr->pexprLeft);
  if (pexprExpr->pexprRight)
    man_free_expr(pexprExpr->pexprRight);
  if (pexprExpr->pexprNext)
    man_free_expr(pexprExpr->pexprNext);
  exprsExprs.puiFreeExprs[--exprsExprs.uiNextFree] =
    pexprExpr - exprsExprs.pexprExprs;
}

void man_free_function (SEL_FUNC *pfuncFunction)
{

  SEL_PARAM *pparamCounter;

  man_free_type(pfuncFunction->pucReturnType);
  for (pparamCounter = pfuncFunction->pparamFirst; pparamCounter;
       pparamCounter = pparamCounter->pparamNext)
    man_free_type(pparamCounter->pucType);
  utl_free(pfuncFunction);
}

SEL_VAR_TREE* man_alloc_var_tree ()
{
  if (vbVarBlocks.uiTreesUsed)
    return(vbVarBlocks.ppvartVarTrees[--vbVarBlocks.uiTreesUsed]);
  return(utl_alloc(sizeof(SEL_VAR_TREE)));
}

void man_free_var_tree (SEL_VAR_TREE *pvartTree)
{
  if (vbVarBlocks.uiTreesUsed == SEL_VAR_POOL_SIZE)
    utl_free(pvartTree);
  else
    vbVarBlocks.ppvartVarTrees[vbVarBlocks.uiTreesUsed++] = pvartTree;
}

UCHAR* man_trivial_type (UCHAR ucType)
{
  switch (ucType)
  {
    case SEL_TYPE_INT :
      return(aucTypeInt);
    case SEL_TYPE_FLOAT :
      return(aucTypeFloat);
    case SEL_TYPE_CHAR :
      return(aucTypeChar);
    case SEL_TYPE_VOID :
      return(aucTypeVoid);     
    case SEL_TYPE_STRING :
      return(aucTypeString);
    case SEL_TYPE_POINTER :
      return(aucTypePointer);
  }
  return(NULL);
}

UCHAR man_type_greater (UCHAR uc1, UCHAR uc2)
{
  if (uc1 == SEL_TYPE_CHAR)
    return(FALSE);
  if (uc2 == SEL_TYPE_FLOAT)
    return(FALSE);
  if (uc1 == SEL_TYPE_FLOAT)
    return(TRUE);
  if (uc2 == SEL_TYPE_CHAR)
    return(TRUE);
  return(FALSE);
} 

UCHAR* man_copy_type (UCHAR *pucType)
{
  
  UCHAR *pucReturnVar;
  
  pucReturnVar = man_alloc_type();
  memcpy(pucReturnVar, pucType, *pucType + 1);
  return(pucReturnVar);
}
                                                                        
UINT man_line (SEL_PROGRAM *pprogProgram)
{
  
  SEL_MODULE *pmodModule;
  UINT        uiCounter;
  ULONG       ulLineEnd   = 0;
  
  pmodModule = pprogProgram->ppmodModules[pprogProgram->uiMP];
  for (uiCounter = 0; uiCounter < pmodModule->uiLines; )
  {
    ulLineEnd += pmodModule->pucLines[uiCounter];
    uiCounter++;
    if (pprogProgram->ulIP < ulLineEnd)
      return(uiCounter);
  }
  return(uiCounter + 1);
}
  
void man_error (SEL_PROGRAM *pprogProgram, UINT uiError, CHAR *pcString)
{              
  printf("Error %u (%s,%u): ", uiError,
         pprogProgram->ppcModules[pprogProgram->uiMP], man_line(pprogProgram));
  if (pcString)
    printf(acErrorMessages[uiError], pcString);
  else
    printf(acErrorMessages[uiError]);
  printf("\n");  
  longjmp(pprogProgram->jbErrorJump, 1);
}          

void man_fatal_error (UINT uiError, CHAR *pcString)
{                          
  printf("Fatal error %u: ", uiError);
  if (pcString)
    printf(acFatalMessages[uiError], pcString);
  else
    printf(acErrorMessages[uiError]);
  exit(1);
}

SEL_STRUCT* man_struct_in_table (CHAR *pcName,
                                 SEL_STRUCT_LIST **ppstructlHashTable)
{
  
  SEL_STRUCT_LIST *pstructlCounter;
  
  for (pstructlCounter = ppstructlHashTable[prg_hash(pcName)];
       pstructlCounter; pstructlCounter = pstructlCounter->pstructlNext)
    if (!strcmp(pstructlCounter->pstructStruct->pcName, pcName))
      return(pstructlCounter->pstructStruct);
  return(NULL);
}          
 
UINT man_sizeof (SEL_PROGRAM *pprogProgram, UCHAR *pucType)
{     
  
  UINT uiNumber = 1;

  do
  {
    switch (*pucType)
    {                          
      case SEL_TYPE_VOID :
        return(0);
      case SEL_TYPE_POINTER :
      case SEL_TYPE_REFERENCE :
        return(uiNumber * sizeof(void*));
      case SEL_TYPE_INT :
        return(uiNumber * sizeof(TYPE_INT));
      case SEL_TYPE_FLOAT :
        return(uiNumber * sizeof(TYPE_FLOAT));
      case SEL_TYPE_CHAR :
        return(uiNumber * sizeof(TYPE_CHAR));
      case SEL_TYPE_ARRAY :
        uiNumber *= *((UINT*)(pucType + 1));
        pucType += 1 + sizeof(UINT);
        break;
      case SEL_TYPE_STRUCT :
        return(uiNumber *
               man_get_struct_by_number(pprogProgram,
                                        *((UINT*)(pucType + 1)))->uiSizeof);
    }
  } while (TRUE);
  return(0);
}

UINT man_struct_offsets (SEL_PROGRAM *pprogProgram,
                         SEL_STRUCT_ELEMENT *pelemFirst)
{
  
  UINT uiOffset = 0;
  
  for (; pelemFirst; pelemFirst = pelemFirst->pelemNext)
  {
    pelemFirst->uiOffset = uiOffset;
    uiOffset += man_sizeof(pprogProgram, pelemFirst->pucType + 1);
  }
  return(uiOffset);
}

UINT man_add_struct (SEL_PROGRAM *pprogProgram, UINT uiToken,
                     SEL_STRUCT_ELEMENT *pelemFirst)
{
  
  SEL_MODULE       *pmodModule;
  SEL_STRUCT_LIST  *pstructlList,
                  **ppstructlHashTable;
  SEL_STRUCT       *pstructStruct;
  UINT              uiHashValue;

  pmodModule = pprogProgram->ppmodModules[pprogProgram->uiMP];
  uiHashValue = pmodModule->psymSymbols[uiToken].uiHashValue;
  if (pprogProgram->ucBlock == SEL_BLOCK_LOCAL)
    ppstructlHashTable = pmodModule->ppstructlStructs;
  else
    ppstructlHashTable = pprogProgram->ppstructlStructs;
  pstructStruct = man_struct_in_table(pmodModule->psymSymbols[uiToken].pcName,
                                      ppstructlHashTable);
  if (pstructStruct)
  {
    pstructStruct->pelemFirst = pelemFirst;
    pstructStruct->uiSizeof = man_struct_offsets(pprogProgram, pelemFirst);
  }
  else
  {
    if (pprogProgram->ucBlock == SEL_BLOCK_LOCAL)
    {
      pstructStruct = pmodModule->pstructStructs + pmodModule->uiStructs;
      pstructStruct->pcName = pmodModule->psymSymbols[uiToken].pcName;
      pstructStruct->pelemFirst = pelemFirst;
      pstructStruct->uiSizeof = man_struct_offsets(pprogProgram, pelemFirst);
      pstructStruct->uiNumber = pmodModule->uiFirstStruct +
                                pmodModule->uiStructs++;
    }
    else
    {
      pstructStruct = pprogProgram->pstructStructs + pprogProgram->uiStructs;
      pstructStruct->pcName = pmodModule->psymSymbols[uiToken].pcName;
      pstructStruct->pelemFirst = pelemFirst;
      pstructStruct->uiSizeof = man_struct_offsets(pprogProgram, pelemFirst);
      pstructStruct->uiNumber = pprogProgram->uiStructs++;
    }
    pstructlList = utl_alloc(sizeof(SEL_STRUCT_LIST));
    pstructlList->pstructStruct = pstructStruct;        
    pstructlList->pstructlNext = ppstructlHashTable[uiHashValue];
    ppstructlHashTable[uiHashValue] = pstructlList;
  }
  return(pstructStruct->uiNumber);
}
 
UINT man_add_empty_struct (SEL_PROGRAM *pprogProgram, UINT uiToken)
{
  
  SEL_MODULE       *pmodModule;
  SEL_STRUCT_LIST  *pstructlList,
                  **ppstructlHashTable;
  SEL_STRUCT       *pstructStruct;
  UINT              uiHashValue;
  
  pmodModule = pprogProgram->ppmodModules[pprogProgram->uiMP];
  uiHashValue = pmodModule->psymSymbols[uiToken].uiHashValue;
  if (pprogProgram->ucBlock == SEL_BLOCK_LOCAL)
  {
    pstructStruct = pmodModule->pstructStructs + pmodModule->uiStructs;
    pstructStruct->pcName = pmodModule->psymSymbols[uiToken].pcName;
    pstructStruct->pelemFirst = NULL;
    pstructStruct->uiSizeof = 0;
    pstructStruct->uiNumber = pmodModule->uiFirstStruct +
                              pmodModule->uiStructs++;
    ppstructlHashTable = pmodModule->ppstructlStructs;
  }
  else
  {
    pstructStruct = pprogProgram->pstructStructs + pprogProgram->uiStructs;
    pstructStruct->pcName = pmodModule->psymSymbols[uiToken].pcName;
    pstructStruct->pelemFirst = NULL;
    pstructStruct->uiSizeof = 0;
    pstructStruct->uiNumber = pprogProgram->uiStructs++;
    ppstructlHashTable = pprogProgram->ppstructlStructs;
  }
  pstructlList = utl_alloc(sizeof(SEL_STRUCT_LIST));
  pstructlList->pstructStruct = pstructStruct;        
  pstructlList->pstructlNext = ppstructlHashTable[uiHashValue];
  ppstructlHashTable[uiHashValue] = pstructlList;
  return(pstructStruct->uiNumber);
}

UINT man_struct_token2number (SEL_PROGRAM *pprogProgram, UINT uiToken)
{
  
  SEL_MODULE      *pmodModule;
  SEL_STRUCT_LIST *pstructlCounter;
  
  pmodModule = pprogProgram->ppmodModules[pprogProgram->uiMP];
  pstructlCounter = pmodModule->ppstructlStructs
                      [pmodModule->psymSymbols[uiToken].uiHashValue];
  for (; pstructlCounter; pstructlCounter = pstructlCounter->pstructlNext)
  {
    if (!strcmp(pstructlCounter->pstructStruct->pcName,
                pmodModule->psymSymbols[uiToken].pcName))
      return(pstructlCounter->pstructStruct->uiNumber);
  }
  pstructlCounter = pprogProgram->ppstructlStructs
                      [pmodModule->psymSymbols[uiToken].uiHashValue];
  for (; pstructlCounter; pstructlCounter = pstructlCounter->pstructlNext)
  {
    if (!strcmp(pstructlCounter->pstructStruct->pcName,
                pmodModule->psymSymbols[uiToken].pcName))
      return(pstructlCounter->pstructStruct->uiNumber);
  }
  return(man_add_empty_struct(pprogProgram, uiToken));
}         

UCHAR* man_type2reference (UCHAR *pucType)
{
  
  UCHAR *pucReturnVar;
  
  pucReturnVar = man_alloc_type();
  memcpy(pucReturnVar + 1, pucType, *pucType + 1);
  *pucReturnVar = *pucType + 1;
  pucReturnVar[1] = SEL_TYPE_REFERENCE;
  return(pucReturnVar);
}                                                               

ULONG man_var_reference (SEL_VARIABLE *pvarVariable)
{
  switch (pvarVariable->pucType[1])
  {
    case SEL_TYPE_ARRAY :
    case SEL_TYPE_STRUCT :
      return(pvarVariable->ulValue);
    case SEL_TYPE_FLOAT :
      return((ULONG)&(pvarVariable->fValue));
    default :
      return((ULONG)&(pvarVariable->ulValue));
  }
}                        

ULONG man_expr_reference (SEL_EXPR *pexprExpr)
{
  switch (pexprExpr->pucType[1])
  {
    case SEL_TYPE_ARRAY :
    case SEL_TYPE_STRUCT :
    case SEL_TYPE_REFERENCE :
      return(pexprExpr->ulValue);
    case SEL_TYPE_FLOAT :
      return((ULONG)&(pexprExpr->fValue));
    default :
      return((ULONG)&(pexprExpr->ulValue));
  }
}                    

void man_remove_reference (SEL_EXPR *pexprExpr)
{
  switch (pexprExpr->pucType[2])
  {                      
    case SEL_TYPE_ARRAY :
    case SEL_TYPE_STRUCT :
      break;
    case SEL_TYPE_FLOAT :
      pexprExpr->fValue = *((TYPE_FLOAT*)pexprExpr->ulValue);
      break;
    case SEL_TYPE_INT :
      pexprExpr->ulValue = *((TYPE_INT*)pexprExpr->ulValue);
      break;
    case SEL_TYPE_CHAR :
      pexprExpr->ulValue = *((TYPE_CHAR*)pexprExpr->ulValue);
      break;
    case SEL_TYPE_POINTER :
      pexprExpr->ulValue = *((ULONG*)pexprExpr->ulValue);
      break;
  }
  memmove(pexprExpr->pucType + 1, pexprExpr->pucType + 2,
          pexprExpr->pucType[0] - 1);
  pexprExpr->pucType[0]--;
}

void man_dereference (SEL_EXPR *pexprExpr)
{
  if (pexprExpr->pucType[1] == SEL_TYPE_REFERENCE)
    man_remove_reference(pexprExpr);
  if (pexprExpr->pucType[1] == SEL_TYPE_ARRAY)
    memmove(pexprExpr->pucType + 2, pexprExpr->pucType + 2 + sizeof(UINT),
            *(pexprExpr->pucType) - sizeof(UINT));
  pexprExpr->pucType[1] = SEL_TYPE_REFERENCE;   
}

void man_reference (SEL_EXPR *pexprExpr)
{
  if (pexprExpr->ucDiscardable)
    utl_free((void*)pexprExpr->ulValue);
  if (pexprExpr->pucType[1] == SEL_TYPE_REFERENCE)
    pexprExpr->pucType[1] = SEL_TYPE_POINTER;
  else
  {        
    pexprExpr->ulValue = man_expr_reference(pexprExpr);
    pexprExpr->pucType = utl_realloc(pexprExpr->pucType,
                                     *(pexprExpr->pucType) + 2);
    memmove(pexprExpr->pucType + 2, pexprExpr->pucType + 1,
            *(pexprExpr->pucType));
    pexprExpr->pucType[1] = SEL_TYPE_POINTER;
    (*(pexprExpr->pucType))++;
  }
}

UCHAR man_token2reference (SEL_PROGRAM *pprogProgram, UINT uiToken,
                           SEL_EXPR *pexprExpr)
{
  
  SEL_MODULE   *pmodModule; 
  SEL_VAR_TREE *pvartCounter;
  SEL_VAR_LIST *pvarlCounter;
  CHAR         *pcName;
  UINT          uiHashValue;
  INT           iCompResult;
  
  pmodModule = pprogProgram->ppmodModules[pprogProgram->uiMP];
  pcName = pmodModule->psymSymbols[uiToken].pcName;
  uiHashValue = pmodModule->psymSymbols[uiToken].uiHashValue;
  pexprExpr->ucType = SEL_EXPR_EXPR;
  pexprExpr->ucDiscardable = FALSE;
  pexprExpr->pexprLeft = pexprExpr->pexprRight = pexprExpr->pexprNext = NULL;
  if (pprogProgram->ucBlock == SEL_BLOCK_FUNC)
  {
    pvartCounter = pprogProgram->pfstkFuncStack[pprogProgram->uiFP - 1].
                     pvartVariables;
    while (pvartCounter)
    {
      iCompResult = strcmp(pcName, pvartCounter->varVariable.pcName);
      if (!iCompResult)
      { 
        if (pvartCounter->varVariable.pucType[1] == SEL_TYPE_REFERENCE)
        {
          pexprExpr->pucType =
            man_copy_type(pvartCounter->varVariable.pucType);
          pexprExpr->ulValue = pvartCounter->varVariable.ulValue;
        }
        else
        {
          pexprExpr->pucType =
            man_type2reference(pvartCounter->varVariable.pucType);
          pexprExpr->ulValue = man_var_reference(&(pvartCounter->varVariable));
        }
        return(TRUE);
      }
      if (iCompResult < 0)
      {
        if (pvartCounter->pvartLeft)
          pvartCounter = pvartCounter->pvartLeft;
        else
          break;
      }
      else
        if (pvartCounter->pvartRight)
          pvartCounter = pvartCounter->pvartRight;
        else
          break;     
    }
  }
  if (pprogProgram->ucBlock != SEL_BLOCK_GLOBAL)
  {
    for (pvarlCounter = pmodModule->ppvarlVariables[uiHashValue];
         pvarlCounter; pvarlCounter = pvarlCounter->pvarlNext)
      if (!strcmp(pcName, pvarlCounter->varVariable.pcName))
      {
        if (pvarlCounter->varVariable.pucType[1] == SEL_TYPE_REFERENCE)
        {
          pexprExpr->pucType =
            man_copy_type(pvarlCounter->varVariable.pucType);
          pexprExpr->ulValue = pvarlCounter->varVariable.ulValue;
        }
        else
        {
          pexprExpr->pucType =
            man_type2reference(pvarlCounter->varVariable.pucType);
          pexprExpr->ulValue = man_var_reference(&(pvarlCounter->varVariable));
        }
        return(TRUE);
      }
  }
  for (pvarlCounter = pprogProgram->ppvarlVariables[uiHashValue];
       pvarlCounter; pvarlCounter = pvarlCounter->pvarlNext)
    if (!strcmp(pcName, pvarlCounter->varVariable.pcName))
    {
      if (pvarlCounter->varVariable.pucType[1] == SEL_TYPE_REFERENCE)
      {
        pexprExpr->pucType =
          man_copy_type(pvarlCounter->varVariable.pucType);
        pexprExpr->ulValue = pvarlCounter->varVariable.ulValue;
      }
      else
      {
        pexprExpr->pucType =
          man_type2reference(pvarlCounter->varVariable.pucType);
        pexprExpr->ulValue = man_var_reference(&(pvarlCounter->varVariable));
      }
      return(TRUE);
    }
  return(FALSE);
}                                                 

void man_delete_func (SEL_FUNC *pfuncFunction)
{          
  
  SEL_PARAM *pparam1,
            *pparam2;
                                    
  pparam1 = pfuncFunction->pparamFirst;
  for (; pparam1; pparam1 = pparam2)
  {
    pparam2 = pparam1->pparamNext;
    man_free_type(pparam1->pucType);
    utl_free(pparam1);
  }
  man_free_type(pfuncFunction->pucReturnType);
  utl_free(pfuncFunction);
}

void man_delete_var_tree (SEL_VAR_TREE *pvartTree)
{
  switch (pvartTree->varVariable.pucType[1])
  {
    case SEL_TYPE_ARRAY :
    case SEL_TYPE_STRUCT :
      utl_free((void*)pvartTree->varVariable.ulValue);
      break;
  }
  man_free_type(pvartTree->varVariable.pucType);
  if (pvartTree->pvartLeft)
    man_delete_var_tree(pvartTree->pvartLeft);
  if (pvartTree->pvartRight)
    man_delete_var_tree(pvartTree->pvartRight);
  man_free_var_tree(pvartTree);
}

SEL_VARIABLE* man_get_variable (SEL_PROGRAM *pprogProgram, CHAR *pcName)
{
  
  SEL_MODULE   *pmodModule; 
  SEL_VAR_TREE *pvartCounter;
  SEL_VAR_LIST *pvarlCounter;
  UINT          uiHashValue;
  INT           iCompResult;
  
  pmodModule = pprogProgram->ppmodModules[pprogProgram->uiMP];
  uiHashValue = prg_hash(pcName);
  if (pprogProgram->ucBlock == SEL_BLOCK_FUNC)
  {
    pvartCounter = pprogProgram->pfstkFuncStack[pprogProgram->uiFP - 1].
                     pvartVariables;
    while (pvartCounter)
    {
      iCompResult = strcmp(pcName, pvartCounter->varVariable.pcName);
      if (!iCompResult)
        return(&(pvartCounter->varVariable));
      if (iCompResult < 0)
      {
        if (pvartCounter->pvartLeft)
          pvartCounter = pvartCounter->pvartLeft;
        else
          break;
      }
      else
        if (pvartCounter->pvartRight)
          pvartCounter = pvartCounter->pvartRight;
        else
          break;     
    }
  }
  if (pprogProgram->ucBlock != SEL_BLOCK_GLOBAL)
  {
    for (pvarlCounter = pmodModule->ppvarlVariables[uiHashValue];
         pvarlCounter; pvarlCounter = pvarlCounter->pvarlNext)
      if (!strcmp(pcName, pvarlCounter->varVariable.pcName))
        return(&(pvarlCounter->varVariable));
  }
  for (pvarlCounter = pprogProgram->ppvarlVariables[uiHashValue];
       pvarlCounter; pvarlCounter = pvarlCounter->pvarlNext)
    if (!strcmp(pcName, pvarlCounter->varVariable.pcName))
      return(&(pvarlCounter->varVariable));
  return(NULL);
}

SEL_EXPR* man_int_expr (TYPE_INT iInt, SEL_EXPR *pexprExpr)
{
  pexprExpr->ucType = SEL_EXPR_EXPR;
  pexprExpr->ucDiscardable = FALSE;
  pexprExpr->pucType = man_alloc_type();
  pexprExpr->pucType[0] = 1;
  pexprExpr->pucType[1] = SEL_TYPE_INT;
  pexprExpr->ulValue = iInt;
  pexprExpr->pexprLeft = pexprExpr->pexprRight = pexprExpr->pexprNext = NULL;
  return(pexprExpr);
}                       

TYPE_INT man_int_value (SEL_EXPR *pexprExpr)
{
  if (pexprExpr->pucType[1] == SEL_TYPE_REFERENCE)
    return(*((TYPE_INT*)pexprExpr->ulValue));
  return((TYPE_INT)pexprExpr->ulValue);
}

SEL_EXPR* man_char_expr (TYPE_CHAR cChar, SEL_EXPR *pexprExpr)
{
  pexprExpr->ucType = SEL_EXPR_EXPR;
  pexprExpr->ucDiscardable = FALSE;
  pexprExpr->pucType = man_alloc_type();
  pexprExpr->pucType[0] = 1;
  pexprExpr->pucType[1] = SEL_TYPE_CHAR;
  pexprExpr->ulValue = cChar;
  pexprExpr->pexprLeft = pexprExpr->pexprRight = pexprExpr->pexprNext = NULL;
  return(pexprExpr);
}

TYPE_CHAR man_char_value (SEL_EXPR *pexprExpr)
{
  if (pexprExpr->pucType[1] == SEL_TYPE_REFERENCE)
    return(*((TYPE_CHAR*)pexprExpr->ulValue));
  return((TYPE_CHAR)pexprExpr->ulValue);
}

SEL_EXPR* man_float_expr (TYPE_FLOAT fFloat, SEL_EXPR *pexprExpr)
{
  pexprExpr->ucType = SEL_EXPR_EXPR;
  pexprExpr->ucDiscardable = FALSE;
  pexprExpr->pucType = man_alloc_type();
  pexprExpr->pucType[0] = 1;
  pexprExpr->pucType[1] = SEL_TYPE_FLOAT;
  pexprExpr->fValue = fFloat;
  pexprExpr->pexprLeft = pexprExpr->pexprRight = pexprExpr->pexprNext = NULL;
  return(pexprExpr);
}

TYPE_FLOAT man_float_value (SEL_EXPR *pexprExpr)
{
  if (pexprExpr->pucType[1] == SEL_TYPE_REFERENCE)
    return(*((TYPE_FLOAT*)pexprExpr->ulValue));
  return(pexprExpr->fValue);
}

SEL_EXPR* man_string_expr (TYPE_CHAR *pcString, SEL_EXPR *pexprExpr)
{
  pexprExpr->ucType = SEL_EXPR_EXPR;
  pexprExpr->ucDiscardable = FALSE;
  pexprExpr->pucType = man_alloc_type();
  pexprExpr->pucType[0] = 4;
  pexprExpr->pucType[1] = SEL_TYPE_ARRAY;
  *((UINT*)&(pexprExpr->pucType[2])) = strlen(pcString) + 1;
  pexprExpr->pucType[4] = SEL_TYPE_CHAR;
  pexprExpr->ulValue = (ULONG)pcString;
  pexprExpr->pexprLeft = pexprExpr->pexprRight = pexprExpr->pexprNext = NULL;
  return(pexprExpr);
} 

UCHAR* man_make_pointer (UCHAR *pucType)
{
  
  UCHAR *pucReturnVar;
  
  pucReturnVar = man_alloc_type();
  *pucReturnVar = *pucType + 1;
  pucReturnVar[1] = SEL_TYPE_POINTER;
  memmove(pucReturnVar + 2, pucType + 1, *pucType);
  man_free_type(pucType);
  return(pucReturnVar);
}

SEL_EXPR* man_pointer_expr (void *pPointer, UCHAR *pucType,
                            SEL_EXPR *pexprExpr)
{
  pexprExpr->ucType = SEL_EXPR_EXPR;
  pexprExpr->ucDiscardable = FALSE;
  pexprExpr->pucType = man_make_pointer(pucType);
  pexprExpr->ulValue = (ULONG)pPointer;
  pexprExpr->pexprLeft = pexprExpr->pexprRight = pexprExpr->pexprNext = NULL;
  return(pexprExpr);
}

void* man_pointer_value (SEL_EXPR *pexprExpr)
{
  if (pexprExpr->pucType[1] == SEL_TYPE_REFERENCE)
    return(*((void**)pexprExpr->ulValue));
  return((void*)pexprExpr->ulValue);
}

void man_delete_expr (SEL_EXPR *pexprExpr)
{    
  if (pexprExpr->pucType != aucTypeInt &&
      pexprExpr->pucType != aucTypeFloat &&
      pexprExpr->pucType != aucTypeChar &&
      pexprExpr->pucType != aucTypeVoid)
    man_free_type(pexprExpr->pucType);
  if (pexprExpr->ucDiscardable)
    utl_free((void*)pexprExpr->ulValue);
  if (pexprExpr->pexprLeft)
    man_delete_expr(pexprExpr->pexprLeft);
  if (pexprExpr->pexprRight)
    man_delete_expr(pexprExpr->pexprRight);
  if (pexprExpr->pexprNext)
    man_delete_expr(pexprExpr->pexprNext);
}
 
SEL_STRUCT* man_get_struct_by_number (SEL_PROGRAM *pprogProgram, UINT uiNumber)
{
  
  SEL_MODULE *pmodModule;
  
  if (uiNumber < 20000)
    return(pprogProgram->pstructStructs + uiNumber);
  else
  {
    pmodModule = pprogProgram->ppmodModules[pprogProgram->uiMP];
    return(pmodModule->pstructStructs +
           (uiNumber - pmodModule->uiFirstStruct));
  }
}

void man_struct_element (SEL_PROGRAM *pprogProgram, SEL_EXPR *pexprStruct,
                         CHAR *pcElement)
{
  
  SEL_STRUCT         *pstructStruct;
  SEL_STRUCT_ELEMENT *pelemCounter;
  UINT                uiNumber;
                       
  if (pexprStruct->pucType[1] == SEL_TYPE_STRUCT)
    uiNumber = *((UINT*)(pexprStruct->pucType + 2));
  else
    uiNumber = *((UINT*)(pexprStruct->pucType + 3));
  pstructStruct = man_get_struct_by_number(pprogProgram, uiNumber);
  for (pelemCounter = pstructStruct->pelemFirst; pelemCounter;
       pelemCounter = pelemCounter->pelemNext)
    if (!strcmp(pcElement, pelemCounter->pcName))
    {
      man_free_type(pexprStruct->pucType);
      pexprStruct->pucType = man_type2reference(pelemCounter->pucType);
      pexprStruct->ulValue += pelemCounter->uiOffset;
      return;
    }     
}

void man_array_element (SEL_PROGRAM *pprogProgram, SEL_EXPR *pexprArray,
                        UINT uiElement)
{
  
  UINT   uiSizeOfElement;
  ULONG  ulAddress;
                                            
  if (pexprArray->ucDiscardable)
  { 
    ulAddress = ((CHAR*)pexprArray->ulValue)[uiElement];
    utl_free((void*)pexprArray->ulValue);
    memmove(pexprArray->pucType + 1, pexprArray->pucType + 2 + sizeof(UINT),
            pexprArray->pucType[0] - 1 - sizeof(UINT));
    pexprArray->pucType[0] -= 1 + sizeof(UINT);
    pexprArray->ulValue = ulAddress;
  }
  else
  {
    switch (pexprArray->pucType[1])
    {
      case SEL_TYPE_ARRAY :
      case SEL_TYPE_POINTER :
        ulAddress = pexprArray->ulValue;
        if (pexprArray->pucType[1] == SEL_TYPE_POINTER)
          uiSizeOfElement = man_sizeof(pprogProgram, pexprArray->pucType + 2);
        else
          uiSizeOfElement = man_sizeof(pprogProgram,
                                       pexprArray->pucType + 2 + sizeof(UINT));
        if (pexprArray->pucType[1] != SEL_TYPE_POINTER)
        {
          memmove(pexprArray->pucType + 2, 
                  pexprArray->pucType + 3 + sizeof(UINT),
                  *(pexprArray->pucType) - 2 - sizeof(UINT));
          pexprArray->pucType[0] -= 1 + sizeof(UINT);
        }
        pexprArray->pucType[1] = SEL_TYPE_REFERENCE;
        break;                                      
      case SEL_TYPE_REFERENCE :
        if (pexprArray->pucType[2] == SEL_TYPE_POINTER)
          ulAddress = *((ULONG*)pexprArray->ulValue);
        else
          ulAddress = pexprArray->ulValue;
        if (pexprArray->pucType[2] == SEL_TYPE_POINTER)
        {
          uiSizeOfElement = man_sizeof(pprogProgram, pexprArray->pucType + 3);
          memmove(pexprArray->pucType + 2, pexprArray->pucType + 3,
                  pexprArray->pucType[0] - 2);
          pexprArray->pucType[0]--;
        }
        else
        {
          uiSizeOfElement = man_sizeof(pprogProgram,
                                       pexprArray->pucType + 3 + sizeof(UINT));
          memmove(pexprArray->pucType + 2,
                  pexprArray->pucType + 3 + sizeof(UINT),
                  pexprArray->pucType[0] - 2 - sizeof(UINT));
          pexprArray->pucType[0] -= 1 + sizeof(UINT);
        }
        pexprArray->pucType[1] = SEL_TYPE_REFERENCE;
        break;
    }
    pexprArray->ulValue = ulAddress + uiSizeOfElement * uiElement;
  }
}          

UCHAR man_same_type (UCHAR *puc1, UCHAR *puc2)
{        
  if (*puc1 != *puc2)
    return(FALSE);
  if (memcmp(puc1 + 1, puc2 + 1, *puc1))
    return(FALSE);
  return(TRUE);
}

CHAR* man_type2sel_form (SEL_PROGRAM *pprogProgram, UCHAR *pucType)
{
                                                 
  CHAR       *pcReturnVar,
             *pcIndex;
  
  pcReturnVar = pcIndex = utl_alloc(81);
  pucType++;
  while (TRUE)
  {
    switch (*pucType)
    {                        
      case SEL_TYPE_REFERENCE :
        *(pcIndex++) = '&';
        pucType++;
        break;
      case SEL_TYPE_INT :
        strcpy(pcIndex, "int");
        return(pcReturnVar);
      case SEL_TYPE_FLOAT :
        strcpy(pcIndex, "float");
        return(pcReturnVar);
      case SEL_TYPE_CHAR :
        strcpy(pcIndex, "char");
        return(pcReturnVar);
      case SEL_TYPE_VOID :
        strcpy(pcIndex, "void");
        return(pcReturnVar);
      case SEL_TYPE_STRUCT :
        pucType++;
        strcpy(pcIndex,
               man_get_struct_by_number(pprogProgram,
                                        *((UINT*)(pucType)))->pcName);
        return(pcReturnVar);
      case SEL_TYPE_POINTER :
        *(pcIndex++) = '*';
        pucType++;
        break;
      case SEL_TYPE_ARRAY :
        *(pcIndex++) = '[';
        pucType++;
	sprintf(pcIndex, "%d", *((UINT*)(pucType)));
        pucType += sizeof(UINT);
        pcIndex += strlen(pcIndex);
        *(pcIndex++) = ']';
        break;
    }
  }
  return(NULL);
}

void man_cast (SEL_PROGRAM *pprogProgram, SEL_EXPR *pexprExpr, UCHAR *pucType)
{         
  
  CHAR *pcBuffer;
     
  if (man_same_type(pexprExpr->pucType, pucType))
  {
    man_free_type(pexprExpr->pucType);
    pexprExpr->pucType = pucType;
    return;                      
  }
  if (pucType[1] == SEL_TYPE_REFERENCE)
  {   
    pcBuffer = utl_alloc(128);
    sprintf(pcBuffer, "%s to %s",
            man_type2sel_form(pprogProgram, pexprExpr->pucType),
            man_type2sel_form(pprogProgram, pucType));
    man_error(pprogProgram, 17, pcBuffer);
  }                      
  switch (pexprExpr->pucType[1])
  {
    case SEL_TYPE_POINTER :    
    case SEL_TYPE_ARRAY :
      switch (pucType[1])
      {
        case SEL_TYPE_POINTER :
        case SEL_TYPE_ARRAY :
          man_free_type(pexprExpr->pucType);
          pexprExpr->pucType = pucType;
          break;
        case SEL_TYPE_INT :
          if (pexprExpr->ucDiscardable)
            utl_free((void*)pexprExpr->ulValue);
          man_free_type(pexprExpr->pucType);
          pexprExpr->pucType = pucType;
          break;
        default :
          pcBuffer = utl_alloc(128);
          sprintf(pcBuffer, "%s to %s",
                  man_type2sel_form(pprogProgram, pexprExpr->pucType),
                  man_type2sel_form(pprogProgram, pucType));
          man_error(pprogProgram, 17, pcBuffer);
          break;
      }
      break;
    case SEL_TYPE_CHAR :
    case SEL_TYPE_INT :
      switch (pucType[1])
      {         
        case SEL_TYPE_INT :
          pexprExpr->ulValue = pexprExpr->ulValue;
          man_free_type(pexprExpr->pucType);
          pexprExpr->pucType = pucType;
          break;
        case SEL_TYPE_FLOAT :
          pexprExpr->fValue = (TYPE_FLOAT)((TYPE_INT)pexprExpr->ulValue);
          man_free_type(pexprExpr->pucType);
          pexprExpr->pucType = pucType;
          break;
        case SEL_TYPE_CHAR :
          pexprExpr->ulValue = (TYPE_CHAR)((TYPE_INT)pexprExpr->ulValue);
          man_free_type(pexprExpr->pucType);
          pexprExpr->pucType = pucType;
          break;
        case SEL_TYPE_POINTER :
        case SEL_TYPE_ARRAY :
          man_free_type(pexprExpr->pucType);
          pexprExpr->pucType = pucType;
          break;
        default :
          pcBuffer = utl_alloc(128);
          sprintf(pcBuffer, "%s to %s",
                  man_type2sel_form(pprogProgram, pexprExpr->pucType),
                  man_type2sel_form(pprogProgram, pucType));
          man_error(pprogProgram, 17, pcBuffer);
          break;
      }
      break;
    case SEL_TYPE_FLOAT :
      switch (pucType[1])
      {
        case SEL_TYPE_INT :
          pexprExpr->ulValue = (TYPE_INT)pexprExpr->fValue;
          man_free_type(pexprExpr->pucType);
          pexprExpr->pucType = pucType;
          break;
        case SEL_TYPE_CHAR :
          pexprExpr->ulValue = (TYPE_CHAR)pexprExpr->fValue;
          man_free_type(pexprExpr->pucType);
          pexprExpr->pucType = pucType;
          break;              
        default :
          pcBuffer = utl_alloc(128);
          sprintf(pcBuffer, "%s to %s",
                  man_type2sel_form(pprogProgram, pexprExpr->pucType),
                  man_type2sel_form(pprogProgram, pucType));
          man_error(pprogProgram, 17, pcBuffer);
          break;
      }
      break;
    case SEL_TYPE_REFERENCE :
      man_remove_reference(pexprExpr);
      man_cast(pprogProgram, pexprExpr, pucType);
      break;                                     
  }
}                     

UCHAR man_type (UCHAR *pucType)
{
  if (pucType[1] == SEL_TYPE_REFERENCE)
    return(pucType[2]);
  else
    return(pucType[1]);
}

void man_init_variable (SEL_PROGRAM *pprogProgram, SEL_VARIABLE *pvarVariable)
{
  switch (pvarVariable->pucType[1])
  {
    case SEL_TYPE_VOID :
    case SEL_TYPE_CHAR :
    case SEL_TYPE_INT :
    case SEL_TYPE_POINTER :
    case SEL_TYPE_REFERENCE :
      pvarVariable->ulValue = 0;
      break;
    case SEL_TYPE_FLOAT :
      pvarVariable->fValue = 0.0;
      break;
    default :
      pvarVariable->ulValue =
        (ULONG)utl_alloc(man_sizeof(pprogProgram, pvarVariable->pucType + 1));
      break;
  }
}

SEL_VARIABLE* man_add_variable (SEL_PROGRAM *pprogProgram, CHAR *pcName,
                                UCHAR *pucType)
{
  
  SEL_MODULE    *pmodModule;
  SEL_VAR_LIST  *pvarlList,
               **ppvarlHashTable;
  SEL_VAR_TREE  *pvartTree,
                *pvartCounter;
  SEL_VARIABLE  *pvarVariable;
  UINT           uiHashValue;
  
  pmodModule = pprogProgram->ppmodModules[pprogProgram->uiMP];
  if (pprogProgram->ucBlock == SEL_BLOCK_LOCAL ||
      pprogProgram->ucBlock == SEL_BLOCK_GLOBAL)
  {
    pvarlList = utl_alloc(sizeof(SEL_VAR_LIST));
    pvarVariable = &(pvarlList->varVariable);
  }
  else
  {
    pvartTree = man_alloc_var_tree();
    pvartTree->pvartLeft = pvartTree->pvartRight = NULL;
    pvarVariable = &(pvartTree->varVariable);
  }
  pvarVariable->pcName = pcName;
  pvarVariable->pucType = pucType;
  man_init_variable(pprogProgram, pvarVariable);
  if (pprogProgram->ucBlock == SEL_BLOCK_FUNC)
  {      
    if (!(pprogProgram->pfstkFuncStack[pprogProgram->uiFP - 1].pvartVariables))
      pprogProgram->pfstkFuncStack[pprogProgram->uiFP - 1].pvartVariables =
        pvartTree;
    else                              
    {
      pvartCounter = pprogProgram->pfstkFuncStack[pprogProgram->uiFP - 1].
                       pvartVariables;
      while (TRUE)
      {
        if (strcmp(pvarVariable->pcName, pvartCounter->varVariable.pcName) < 0)
        { 
          if (pvartCounter->pvartLeft)
            pvartCounter = pvartCounter->pvartLeft;
          else
          {
            pvartCounter->pvartLeft = pvartTree;
            return(pvarVariable);
          }
        }
        else
          if (pvartCounter->pvartRight)
            pvartCounter = pvartCounter->pvartRight;
          else
          {
            pvartCounter->pvartRight = pvartTree;
            return(pvarVariable);
          }
      }
    }
  }
  else
  {
    if (pprogProgram->ucBlock == SEL_BLOCK_LOCAL)
      ppvarlHashTable = pmodModule->ppvarlVariables;
    else
      ppvarlHashTable = pprogProgram->ppvarlVariables;
    uiHashValue = prg_hash(pcName);
    pvarlList->pvarlNext = ppvarlHashTable[uiHashValue];
    ppvarlHashTable[uiHashValue] = pvarlList;
  }
  return(pvarVariable);
}                   

void man_assign (SEL_PROGRAM *pprogProgram, ULONG ulReference,
                 SEL_EXPR *pexprExpr)       
{                          
  
  ULONG ulExprReference;
                
  if (pexprExpr->pucType[1] == SEL_TYPE_REFERENCE)
    ulExprReference = (ULONG)&(pexprExpr->ulValue);
  else
    ulExprReference = man_expr_reference(pexprExpr);
  memcpy((void*)ulReference, (void*)ulExprReference,
         man_sizeof(pprogProgram, pexprExpr->pucType + 1));
}

void man_assign_variable (SEL_PROGRAM *pprogProgram,
                          SEL_VARIABLE *pvarVariable, SEL_EXPR *pexprExpr)
{
  if (!man_same_type(pvarVariable->pucType, pexprExpr->pucType))
    man_cast(pprogProgram, pexprExpr, man_copy_type(pvarVariable->pucType));
  man_assign(pprogProgram, man_var_reference(pvarVariable), pexprExpr);
}
