/***************************************************************************
 ***************************************************************************
 ***                                                                     ***
 ***             Schani Electronics Language - Interpreter               ***
 ***                                                                     ***
 ***                              Parser                                 ***
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
        
void par_get_token (SEL_PROGRAM *pprogProgram, UCHAR ucShouldBe, UINT uiError)
{
  
  SEL_MODULE *pmodModule;
  UINT        uiLength;
  
  pmodModule = pprogProgram->ppmodModules[pprogProgram->uiMP];
  SEL_ACT_TOKEN.ucType = pmodModule->pucSource[pprogProgram->ulIP];
  if (ucShouldBe)
    if (SEL_ACT_TOKEN.ucType != ucShouldBe)
      man_error(pprogProgram, uiError, NULL);
  pprogProgram->ulIP += sizeof(UCHAR);
  if (SEL_ACT_TOKEN.ucType >= RETURN)
    return;
  if (SEL_ACT_TOKEN.ucType >= IF && SEL_ACT_TOKEN.ucType <= DEFAULT)
  {
    SEL_ACT_TOKEN.ulNext = 
      *((ULONG*)(pmodModule->pucSource + pprogProgram->ulIP));
    pprogProgram->ulIP += sizeof(ULONG);
  }
  else
    switch (SEL_ACT_TOKEN.ucType)
    {
      case CHAR_CONSTANT :
        SEL_ACT_TOKEN.cChar = pmodModule->pucSource[pprogProgram->ulIP];
        pprogProgram->ulIP += sizeof(TYPE_CHAR);
        break;
      case INT_CONSTANT :
        SEL_ACT_TOKEN.iInt =
          *((TYPE_INT*)(pmodModule->pucSource + pprogProgram->ulIP));
        pprogProgram->ulIP += sizeof(TYPE_INT);
        break;
      case FLOAT_CONSTANT :
        SEL_ACT_TOKEN.fFloat =
          *((TYPE_FLOAT*)(pmodModule->pucSource + pprogProgram->ulIP));
        pprogProgram->ulIP += sizeof(TYPE_FLOAT);
        break;
      case STRING_CONSTANT :
        uiLength = *((UINT*)(pmodModule->pucSource + pprogProgram->ulIP));
        pprogProgram->ulIP += sizeof(UINT);
        SEL_ACT_TOKEN.pcString = pmodModule->pucSource + pprogProgram->ulIP;
        pprogProgram->ulIP += uiLength;
        break;
      case IDENTIFIER :
        SEL_ACT_TOKEN.ulNext = 
          *((UINT*)(pmodModule->pucSource + pprogProgram->ulIP));
        pprogProgram->ulIP += sizeof(UINT);
        break;
    }
}                                                          

void par_matching_bracket (SEL_PROGRAM *pprogProgram)
{
  
  UINT uiLevel = 1;

  do
  {
    par_get_token(pprogProgram, 0, 0);
    switch (SEL_ACT_TOKEN.ucType)
    {
      case OPENbracket :
        uiLevel++;
        break;
      case CLOSEbracket :
        uiLevel--;
        break;
    }
  } while (uiLevel);
}   

void par_search_token (SEL_PROGRAM *pprogProgram, UCHAR ucType)
{
  
  SEL_MODULE *pmodModule;
  
  pmodModule = pprogProgram->ppmodModules[pprogProgram->uiMP];
  while (SEL_LOOK_AHEAD != ucType)
    par_get_token(pprogProgram, 0, 0);
}

UCHAR* par_type (SEL_PROGRAM *pprogProgram)
{
  
  UCHAR *pucType;
  UCHAR  ucUsed     = 1;
  
  pucType = man_alloc_type();
  while (TRUE)
  {
    par_get_token(pprogProgram, 0, 0);
    switch (SEL_ACT_TOKEN.ucType)
    {
      case IDENTIFIER :
        pucType[ucUsed++] = SEL_TYPE_STRUCT;
        *((UINT*)(pucType + ucUsed)) = man_struct_token2number(pprogProgram, 
                                                               SEL_ACT_TOKEN.
                                                                 ulNext);
        *pucType = ucUsed + sizeof(UINT) - sizeof(UCHAR);
        return(pucType);
      case OPENindex :
        pucType[ucUsed++] = SEL_TYPE_ARRAY;
        par_get_token(pprogProgram, INT_CONSTANT, 6);
        *((UINT*)(pucType + ucUsed)) = SEL_ACT_TOKEN.iInt;
        ucUsed += sizeof(UINT);
        par_get_token(pprogProgram, CLOSEindex, 2);
        break;
      case T_INT :
        pucType[ucUsed] = SEL_TYPE_INT;
        pucType[0] = ucUsed;
        return(pucType);
      case T_FLOAT :
        pucType[ucUsed] = SEL_TYPE_FLOAT;
        pucType[0] = ucUsed;
        return(pucType);
      case T_CHAR :
        pucType[ucUsed] = SEL_TYPE_CHAR;
        pucType[0] = ucUsed;
        return(pucType);
      case T_VOID :
        pucType[ucUsed] = SEL_TYPE_VOID;
        pucType[0] = ucUsed;
        return(pucType);
      case MULT :
        pucType[ucUsed++] = SEL_TYPE_POINTER;
        break;                        
      default :
        man_error(pprogProgram, 13, NULL);
    }
  }
}   

UCHAR* par_formal_type (SEL_PROGRAM *pprogProgram)
{
  
  SEL_MODULE *pmodModule;
  UCHAR      *pucType,
             *pucReturnVar;
  
  pmodModule = pprogProgram->ppmodModules[pprogProgram->uiMP];
  if (SEL_LOOK_AHEAD == AND)
  {
    par_get_token(pprogProgram, 0, 0);
    pucType = par_type(pprogProgram);
    pucReturnVar = man_type2reference(pucType);
    man_free_type(pucType);
  }
  else
    return(par_type(pprogProgram));
  return(pucReturnVar);
}

SEL_PARAM* par_formal_specs (SEL_PROGRAM *pprogProgram)
{
  
  SEL_MODULE *pmodModule;
  SEL_PARAM  *pparamReturnVar = NULL,
             *pparamCounter;
            
  pmodModule = pprogProgram->ppmodModules[pprogProgram->uiMP];
  while (TRUE)
  { 
    if (!pparamReturnVar)
      pparamReturnVar = pparamCounter = utl_alloc(sizeof(SEL_PARAM));
    par_get_token(pprogProgram, IDENTIFIER, 5);
    pparamCounter->pcName = pmodModule->psymSymbols[SEL_ACT_TOKEN.ulNext].
                              pcName;
    par_get_token(pprogProgram, TYPEspec, 3);
    pparamCounter->pucType = par_formal_type(pprogProgram);
    if (SEL_LOOK_AHEAD == COMMA)             
    {
      par_get_token(pprogProgram, 0, 0);
      pparamCounter->pparamNext = utl_alloc(sizeof(SEL_PARAM));
      pparamCounter = pparamCounter->pparamNext;
    }
    else
    {
      pparamCounter->pparamNext = NULL;
      return(pparamReturnVar);
    }
  }
}          

SEL_PARAM* par_formal_params (SEL_PROGRAM *pprogProgram)
{
  
  SEL_MODULE *pmodModule;
  
  pmodModule = pprogProgram->ppmodModules[pprogProgram->uiMP];
  if (SEL_LOOK_AHEAD == IDENTIFIER)
    return(par_formal_specs(pprogProgram));
  return(NULL);
}
 
SEL_FUNC* par_sub (SEL_PROGRAM *pprogProgram)
{
  
  SEL_MODULE *pmodModule;
  SEL_FUNC   *pfuncReturnVar;
  
  pmodModule = pprogProgram->ppmodModules[pprogProgram->uiMP];                           
  pfuncReturnVar = utl_alloc(sizeof(SEL_FUNC));
  pfuncReturnVar->ucType = SEL_FUNC_SEL;
  pfuncReturnVar->uiMP = pprogProgram->uiMP;
  par_get_token(pprogProgram, SUB, 11);
  par_get_token(pprogProgram, IDENTIFIER, 5);
  pfuncReturnVar->pcName = pmodModule->psymSymbols[SEL_ACT_TOKEN.ulNext].
                             pcName;
  par_get_token(pprogProgram, OPENbracket, 0);
  pfuncReturnVar->pparamFirst = par_formal_params(pprogProgram);
  par_get_token(pprogProgram, CLOSEbracket, 1);
  par_get_token(pprogProgram, TYPEspec, 3);
  pfuncReturnVar->pucReturnType = par_type(pprogProgram);
  pfuncReturnVar->ulIP = pprogProgram->ulIP;
  return(pfuncReturnVar);
}   

SEL_STRUCT_ELEMENT* par_var_decls (SEL_PROGRAM *pprogProgram)
{
  
  SEL_MODULE         *pmodModule;
  SEL_STRUCT_ELEMENT *pelemReturnVar = NULL,
                     *pelemCounter   = NULL;

  pmodModule = pprogProgram->ppmodModules[pprogProgram->uiMP];
  while (TRUE)
  { 
    if (!pelemReturnVar)
      pelemReturnVar = pelemCounter = utl_alloc(sizeof(SEL_STRUCT_ELEMENT));
    par_get_token(pprogProgram, IDENTIFIER, 5);
    pelemCounter->pcName = pmodModule->psymSymbols[SEL_ACT_TOKEN.ulNext].
                             pcName;
    par_get_token(pprogProgram, TYPEspec, 3);
    pelemCounter->pucType = par_type(pprogProgram);
    par_get_token(pprogProgram, SEMICOLON, 4);
    if (SEL_LOOK_AHEAD == IDENTIFIER)             
    {
      pelemCounter->pelemNext = utl_alloc(sizeof(SEL_STRUCT_ELEMENT));
      pelemCounter = pelemCounter->pelemNext;
    }
    else
    {
      pelemCounter->pelemNext = NULL;
      return(pelemReturnVar);
    }
  }
}                                                           

SEL_PARAM* par_param_types (SEL_PROGRAM *pprogProgram)
{
  
  SEL_MODULE *pmodModule;
  SEL_PARAM  *pparamReturnVar = NULL,
             *pparamCounter;
                            
  pmodModule = pprogProgram->ppmodModules[pprogProgram->uiMP];
  while (SEL_LOOK_AHEAD != CLOSEbracket)
  {
    if (!pparamReturnVar)
      pparamReturnVar = pparamCounter = utl_alloc(sizeof(SEL_PARAM));
    else
    {
      pparamCounter->pparamNext = utl_alloc(sizeof(SEL_PARAM));
      pparamCounter = pparamCounter->pparamNext;           
    }
    pparamCounter->pparamNext = NULL;
    pparamCounter->pcName = NULL;
    pparamCounter->pucType = par_formal_type(pprogProgram);
    if (SEL_LOOK_AHEAD == COMMA)
      par_get_token(pprogProgram, 0, 0);
  }                
  return(pparamReturnVar);
}

SEL_FUNC* par_prototype (SEL_PROGRAM *pprogProgram)
{

  SEL_MODULE *pmodModule;
  SEL_FUNC   *pfuncReturnVar;
                            
  pmodModule = pprogProgram->ppmodModules[pprogProgram->uiMP];
  par_get_token(pprogProgram, SUB, 11);
  pfuncReturnVar = utl_alloc(sizeof(SEL_FUNC));
  par_get_token(pprogProgram, IDENTIFIER, 5);
  pfuncReturnVar->pcName = pmodModule->psymSymbols[SEL_ACT_TOKEN.ulNext].
                             pcName;
  par_get_token(pprogProgram, OPENbracket, 0);
  pfuncReturnVar->pparamFirst = par_param_types(pprogProgram);
  par_get_token(pprogProgram, CLOSEbracket, 1);
  par_get_token(pprogProgram, TYPEspec, 3);
  pfuncReturnVar->pucReturnType = par_type(pprogProgram);
  pfuncReturnVar->ucType = SEL_FUNC_SEL;
  pfuncReturnVar->rtl_func = NULL;
  pfuncReturnVar->uiMP = (UINT)-1;
  return(pfuncReturnVar);
}

void par_global_decl (SEL_PROGRAM *pprogProgram)
{

  SEL_MODULE         *pmodModule;
  SEL_EXPR           *pexprExpr;
  SEL_VARIABLE       *pvarVariable;
  SEL_STRUCT_ELEMENT *pelemFirst;
  SEL_FUNC_LIST      *pfunclNode,
                     *pfunclCounter;
  UCHAR              *pucType,
                      ucInsert;
  UINT                uiToken,
                      uiHashValue;
                        
  pmodModule = pprogProgram->ppmodModules[pprogProgram->uiMP];
  while (TRUE)
  {                     
    switch (SEL_LOOK_AHEAD)
    {
      case STRUCT :
        par_get_token(pprogProgram, 0, 0);
        par_get_token(pprogProgram, IDENTIFIER, 5 );
        uiToken = SEL_ACT_TOKEN.ulNext;
        pelemFirst = par_var_decls(pprogProgram);
        par_get_token(pprogProgram, END, 7);
        man_add_struct(pprogProgram, uiToken, pelemFirst);
        break;
      case EXTERN :
        if (pprogProgram->ucBlock != SEL_BLOCK_GLOBAL)
          man_error(pprogProgram, 16, NULL);
        par_search_token(pprogProgram, SEMICOLON);
        par_get_token(pprogProgram, 0, 0);
        break;
      case SUB :
        pfunclNode = utl_alloc(sizeof(SEL_FUNC_LIST));
        pfunclNode->pfuncFunction = par_prototype(pprogProgram);
        uiHashValue = prg_hash(pfunclNode->pfuncFunction->pcName);
        ucInsert = TRUE;
        if (pprogProgram->ucBlock == SEL_BLOCK_GLOBAL)
        {
          for (pfunclCounter = pprogProgram->ppfunclFuncs[uiHashValue];
               pfunclCounter; pfunclCounter = pfunclCounter->pfunclNext)
            if (!strcmp(pfunclNode->pfuncFunction->pcName,
                        pfunclCounter->pfuncFunction->pcName))
            {
              man_free_function(pfunclNode->pfuncFunction);
              utl_free(pfunclNode);
              ucInsert = FALSE;
              break;
            }
          if (ucInsert)
          {
            pfunclNode->pfunclNext = pprogProgram->ppfunclFuncs[uiHashValue];
            pprogProgram->ppfunclFuncs[uiHashValue] = pfunclNode;
          }
        }
        else
        {
          for (pfunclCounter = pmodModule->ppfunclFuncs[uiHashValue];
               pfunclCounter; pfunclCounter = pfunclCounter->pfunclNext)
            if (!strcmp(pfunclNode->pfuncFunction->pcName,
                        pfunclCounter->pfuncFunction->pcName))
            {
              man_free_function(pfunclNode->pfuncFunction);
              utl_free(pfunclNode);
              ucInsert = FALSE;
              break;
            }
          if (ucInsert)
          {
            pfunclNode->pfunclNext = pmodModule->ppfunclFuncs[uiHashValue];
            pmodModule->ppfunclFuncs[uiHashValue] = pfunclNode;
          }
        }
        break;
      default :
        par_get_token(pprogProgram, IDENTIFIER, 5);
        uiToken = SEL_ACT_TOKEN.ulNext;
        par_get_token(pprogProgram, TYPEspec, 3);
        pucType = par_type(pprogProgram);
        pvarVariable = man_add_variable(pprogProgram,
                                        pmodModule->psymSymbols[uiToken].
                                          pcName,
                                        pucType);
        if (SEL_LOOK_AHEAD != SEMICOLON)
        {                          
          par_get_token(pprogProgram, ASSIGN, 4);
          par_constant(pprogProgram);
          pexprExpr = man_alloc_expr();
          switch (SEL_ACT_TOKEN.ucType)
          {
            case INT_CONSTANT :
              man_int_expr(SEL_ACT_TOKEN.iInt, pexprExpr);
              break;
            case CHAR_CONSTANT :
              man_char_expr(SEL_ACT_TOKEN.cChar, pexprExpr);
              break;
            case FLOAT_CONSTANT :
              man_float_expr(SEL_ACT_TOKEN.fFloat, pexprExpr);
              break;
            case STRING_CONSTANT :
              man_string_expr(SEL_ACT_TOKEN.pcString, pexprExpr);
              break;
            default :
              man_error(pprogProgram, 23, NULL);
              break;
          }
          man_assign_variable(pprogProgram, pvarVariable, pexprExpr);
          man_free_expr(pexprExpr);
        }
        par_get_token(pprogProgram, SEMICOLON, 4);
        break;
    }
    if (SEL_LOOK_AHEAD != STRUCT && SEL_LOOK_AHEAD != IDENTIFIER &&
        SEL_LOOK_AHEAD != EXTERN && SEL_LOOK_AHEAD != SUB)
      return;
  }
}

void par_local (SEL_PROGRAM *pprogProgram)
{
  
  SEL_MODULE *pmodModule;
  
  pmodModule = pprogProgram->ppmodModules[pprogProgram->uiMP];
  par_get_token(pprogProgram, LOCAL, 8);
  if (SEL_LOOK_AHEAD == END)
  {
    par_get_token(pprogProgram, 0, 0);
    return;
  }
  par_global_decl(pprogProgram);
  par_get_token(pprogProgram, END, 7);
}            

void par_global (SEL_PROGRAM *pprogProgram)
{
  
  SEL_MODULE *pmodModule;
  
  pmodModule = pprogProgram->ppmodModules[pprogProgram->uiMP];
  par_get_token(pprogProgram, GLOBAL, 9);
  if (SEL_LOOK_AHEAD == END)
  {
    par_get_token(pprogProgram, 0, 0);
    return;
  }                                      
  par_global_decl(pprogProgram);
  par_get_token(pprogProgram, END, 7);
}          

SEL_EXPR* par_arguments (SEL_PROGRAM *pprogProgram)
{
  
  SEL_MODULE *pmodModule;
  SEL_EXPR   *pexprReturnVar = NULL,
             *pexprExpr;
  UCHAR       ucWorking      = TRUE;
  
  pmodModule = pprogProgram->ppmodModules[pprogProgram->uiMP];
  if (SEL_LOOK_AHEAD == CLOSEbracket)
    return(NULL);   
  do
  {             
    if (!pexprReturnVar)
      pexprExpr = pexprReturnVar = par_expr(pprogProgram);
    else                               
    {
      pexprExpr->pexprNext = par_expr(pprogProgram);
      pexprExpr = pexprExpr->pexprNext;
    }                               
    if (SEL_LOOK_AHEAD == COMMA)
      par_get_token(pprogProgram, 0, 0);
    else
      ucWorking = FALSE;
  } while (ucWorking);
  return(pexprReturnVar);
}

void par_func_vars (SEL_PROGRAM *pprogProgram)
{
                       
  SEL_MODULE *pmodModule;
  SEL_EXPR   *pexprExpr,
             *pexprValue;
  UINT        uiToken;
  UCHAR      *pucType;
  
  pmodModule = pprogProgram->ppmodModules[pprogProgram->uiMP];
  while (TRUE)
  {
    if (SEL_LOOK_AHEAD != IDENTIFIER)
      return;
    par_get_token(pprogProgram, 0, 0);
    uiToken = SEL_ACT_TOKEN.ulNext;
    par_get_token(pprogProgram, TYPEspec, 3);
    pucType = par_type(pprogProgram);
    man_add_variable(pprogProgram,
                     pmodModule->psymSymbols[uiToken].pcName, pucType);
    if (SEL_LOOK_AHEAD == ASSIGN)
    {                                                     
      pexprExpr = man_alloc_expr();
      man_token2reference(pprogProgram, uiToken, pexprExpr);
      par_get_token(pprogProgram, 0, 0);
      pexprValue = par_expr(pprogProgram);
      pucType = man_alloc_type();
      *pucType = *(pexprExpr->pucType) - 1;
      memcpy(pucType + 1, pexprExpr->pucType + 2, *pucType);
      man_cast(pprogProgram, pexprValue, pucType);
      man_assign(pprogProgram, man_expr_reference(pexprExpr), pexprValue);
      man_delete_expr(pexprValue);
      man_free_expr(pexprValue);
      man_delete_expr(pexprExpr);
      man_free_expr(pexprExpr);
    }                                                                   
    par_get_token(pprogProgram, SEMICOLON, 4);
  }
}

void par_constant (SEL_PROGRAM *pprogProgram)
{                             
  par_get_token(pprogProgram, 0, 0);
  if (SEL_ACT_TOKEN.ucType == MINUS)
  {
    par_get_token(pprogProgram, 0, 0);
    switch (SEL_ACT_TOKEN.ucType)
    {
      case INT_CONSTANT :
        SEL_ACT_TOKEN.iInt = -SEL_ACT_TOKEN.iInt;
        break;
      case FLOAT_CONSTANT :
        SEL_ACT_TOKEN.fFloat = -SEL_ACT_TOKEN.fFloat;
        break;
      default :
        man_error(pprogProgram, 22, NULL);
        break;
    }
  }
}

SEL_EXPR* par_primary (SEL_PROGRAM *pprogProgram)
{
  
  SEL_MODULE *pmodModule;                                     
  SEL_EXPR   *pexprExpr,
             *pexprReturnVar;
  UCHAR      *pucType,
              ucWorking      = TRUE;
  UINT        uiToken;

  pmodModule = pprogProgram->ppmodModules[pprogProgram->uiMP];
  switch (SEL_LOOK_AHEAD)
  {
    case IDENTIFIER :                   
      par_get_token(pprogProgram, 0, 0);
      if (SEL_LOOK_AHEAD == OPENbracket)
      {
        uiToken = SEL_ACT_TOKEN.ulNext;
        par_get_token(pprogProgram, 0, 0);
        pexprExpr = par_arguments(pprogProgram);
        par_get_token(pprogProgram, CLOSEbracket, 1);
        pexprReturnVar = prg_call_func(pprogProgram,
                                       pmodModule->psymSymbols[uiToken].pcName,
                                       pexprExpr);                           
        if (pexprExpr)
        {
          man_delete_expr(pexprExpr);
          man_free_expr(pexprExpr);
        }
      }
      else                             
      {
        pexprReturnVar = man_alloc_expr();
        if (!man_token2reference(pprogProgram, SEL_ACT_TOKEN.ulNext,
                                 pexprReturnVar))
          man_error(pprogProgram, 15,
                    pmodModule->psymSymbols[SEL_ACT_TOKEN.ulNext].pcName);
      }
      break;       
    case OPENbracket :
      par_get_token(pprogProgram, 0, 0);
      pexprReturnVar = par_expr(pprogProgram);
      par_get_token(pprogProgram, CLOSEbracket, 1);
      break;
    case SIZEOF :
      par_get_token(pprogProgram, 0, 0);
      pucType = par_type(pprogProgram);       
      pexprReturnVar = man_alloc_expr();
      man_int_expr(man_sizeof(pprogProgram, pucType + 1), pexprReturnVar);
      man_free_type(pucType);
      break;
    case NEW :
      par_get_token(pprogProgram, 0, 0);
      pucType = par_type(pprogProgram);
      pexprReturnVar = man_alloc_expr();
      man_pointer_expr(utl_alloc(man_sizeof(pprogProgram, pucType + 1)),
                       pucType, pexprReturnVar);
      break;
    case INT_CONSTANT :
      par_get_token(pprogProgram, 0, 0);
      pexprReturnVar = man_alloc_expr();
      man_int_expr(SEL_ACT_TOKEN.iInt, pexprReturnVar);
      break;    
    case CHAR_CONSTANT :
      par_get_token(pprogProgram, 0, 0);
      pexprReturnVar = man_alloc_expr();
      man_char_expr(SEL_ACT_TOKEN.cChar, pexprReturnVar);
      break;
    case FLOAT_CONSTANT :
      par_get_token(pprogProgram, 0, 0);
      pexprReturnVar = man_alloc_expr();
      man_float_expr(SEL_ACT_TOKEN.fFloat, pexprReturnVar);
      break;
    case STRING_CONSTANT :
      par_get_token(pprogProgram, 0, 0);
      pexprReturnVar = man_alloc_expr();
      man_string_expr(SEL_ACT_TOKEN.pcString, pexprReturnVar);
      break;              
    default :
      man_error(pprogProgram, 19, NULL);
      break;
  }
  do
  {
    switch (SEL_LOOK_AHEAD)
    {
      case ARROW :
        par_get_token(pprogProgram, 0, 0);
        par_get_token(pprogProgram, IDENTIFIER, 5);
        if (pexprReturnVar->pucType[1] == SEL_TYPE_REFERENCE)
          man_remove_reference(pexprReturnVar);
        if (pexprReturnVar->pucType[1] == SEL_TYPE_POINTER)
          man_dereference(pexprReturnVar);
        man_struct_element(pprogProgram, pexprReturnVar,
                           pmodModule->psymSymbols[SEL_ACT_TOKEN.ulNext].
                             pcName);
        break;                       
      case TYPEspec :
        par_get_token(pprogProgram, 0, 0);
        pucType = par_type(pprogProgram);
        man_cast(pprogProgram, pexprReturnVar, pucType);
        break;
      case OPENindex :
        par_get_token(pprogProgram, 0, 0);
        pexprExpr = par_expr(pprogProgram);
        par_get_token(pprogProgram, CLOSEindex, 2);
        man_array_element(pprogProgram, pexprReturnVar,
                          man_int_value(pexprExpr));
        man_delete_expr(pexprExpr);
        man_free_expr(pexprExpr);
        break;          
      default :
        ucWorking = FALSE;
        break;
    }
  } while (ucWorking);   
  return(pexprReturnVar);
}                                       

SEL_EXPR* par_unary (SEL_PROGRAM *pprogProgram)
{
  
  CNT_STACK  *psStack        = NULL;
  SEL_MODULE *pmodModule;                                     
  SEL_EXPR   *pexprReturnVar;
  UCHAR       ucWorking      = TRUE;
  TYPE_INT    iInt;
  TYPE_FLOAT  fFloat;
  TYPE_CHAR   cChar;
  
  pmodModule = pprogProgram->ppmodModules[pprogProgram->uiMP];
  do
  {
    switch (SEL_LOOK_AHEAD)
    {
      case MINUS :
        par_get_token(pprogProgram, 0, 0);
        if (!psStack)
          psStack = cnt_stack_make();
        if (cnt_stack_look_back(psStack, 0) == MINUS)
          cnt_stack_get(psStack);
        else
          cnt_stack_put(psStack, MINUS);
        break;
      case MULT :                              
        par_get_token(pprogProgram, 0, 0);
        if (!psStack)
          psStack = cnt_stack_make();
        cnt_stack_put(psStack, MULT);
        break;
      case AND :
        par_get_token(pprogProgram, 0, 0);
        if (!psStack)
          psStack = cnt_stack_make();
        cnt_stack_put(psStack, AND);
        break;
      default :
        pexprReturnVar = par_primary(pprogProgram);
        ucWorking = FALSE;
        break;
    }
  } while (ucWorking);
  if (psStack)
  {
    while (!cnt_stack_is_empty(psStack))
    {
      switch (cnt_stack_get(psStack))
      {
        case MINUS :
          switch (man_type(pexprReturnVar->pucType))
          {
            case SEL_TYPE_INT :
              iInt = -man_int_value(pexprReturnVar);
              man_delete_expr(pexprReturnVar);
              man_int_expr(iInt, pexprReturnVar);
              break;
            case SEL_TYPE_CHAR :
              cChar = -man_char_value(pexprReturnVar);
              man_delete_expr(pexprReturnVar);
              man_char_expr(cChar, pexprReturnVar);
              break;
            case SEL_TYPE_FLOAT :
              fFloat = -man_float_value(pexprReturnVar);
              man_delete_expr(pexprReturnVar);
              man_float_expr(fFloat, pexprReturnVar);
              break;
          }
          break;
        case MULT :
          man_dereference(pexprReturnVar);
          break;
        case AND :
          man_reference(pexprReturnVar);
          break;
      }
    }                     
    cnt_stack_destroy(psStack);
  }
  return(pexprReturnVar);
}

SEL_EXPR* par_level1 (SEL_PROGRAM *pprogProgram)
{
  
  CNT_STACK  *psStack        = NULL;
  SEL_MODULE *pmodModule;                                     
  SEL_EXPR   *pexprReturnVar;
  UCHAR       ucWorking      = TRUE;
  TYPE_INT    iInt;
  
  pmodModule = pprogProgram->ppmodModules[pprogProgram->uiMP];
  do
  {
    switch (SEL_LOOK_AHEAD)
    {
      case LNOT :                        
        par_get_token(pprogProgram, 0, 0);
        if (!psStack)
          psStack = cnt_stack_make();
        if (cnt_stack_look_back(psStack, 0) == LNOT)
          cnt_stack_get(psStack);
        else
          cnt_stack_put(psStack, LNOT);
        break;
      case NOT :                               
        par_get_token(pprogProgram, 0, 0);
        if (!psStack)
          psStack = cnt_stack_make();
        if (cnt_stack_look_back(psStack, 0) == NOT)
          cnt_stack_get(psStack);
        else
          cnt_stack_put(psStack, NOT);
        break;
      default :
        pexprReturnVar = par_unary(pprogProgram);
        ucWorking = FALSE;
        break;
    }
  } while (ucWorking);
  if (psStack)
  {
    while (!cnt_stack_is_empty(psStack))
    {
      switch (cnt_stack_get(psStack))
      {
        case LNOT :
          man_cast(pprogProgram, pexprReturnVar, 
                   man_copy_type(man_trivial_type(SEL_TYPE_INT)));
          iInt = !man_int_value(pexprReturnVar);
          man_delete_expr(pexprReturnVar);
          man_int_expr(iInt, pexprReturnVar);
          break;
        case NOT :
          man_cast(pprogProgram, pexprReturnVar, 
                   man_copy_type(man_trivial_type(SEL_TYPE_INT)));
          iInt = ~man_int_value(pexprReturnVar);
          man_delete_expr(pexprReturnVar);
          man_int_expr(iInt, pexprReturnVar);
          break;
      }
    }
    cnt_stack_destroy(psStack);
  }
  return(pexprReturnVar);
}

SEL_EXPR* par_level2 (SEL_PROGRAM *pprogProgram)
{

  SEL_MODULE *pmodModule;                                     
  SEL_EXPR   *pexprReturnVar,
             *pexprExpr;
  UCHAR       ucType,
              ucTokenType;
  TYPE_INT    iInt;
  TYPE_FLOAT  fFloat;
  TYPE_CHAR   cChar;
                                               
  pmodModule = pprogProgram->ppmodModules[pprogProgram->uiMP];
  pexprReturnVar = par_level1(pprogProgram);
  if (pexprReturnVar)
    ucType = man_type(pexprReturnVar->pucType);
  while (SEL_LOOK_AHEAD == MULT || SEL_LOOK_AHEAD == DIV ||
         SEL_LOOK_AHEAD == MOD)
  {        
    par_get_token(pprogProgram, 0, 0);
    ucTokenType = SEL_ACT_TOKEN.ucType;
    pexprExpr = par_level1(pprogProgram);
    if (ucType != man_type(pexprExpr->pucType))
    {
      if (man_type_greater(ucType, man_type(pexprExpr->pucType)))
        man_cast(pprogProgram, pexprExpr,
                 man_copy_type(man_trivial_type(ucType)));
      else
      {
        ucType = man_type(pexprExpr->pucType);
        man_cast(pprogProgram, pexprReturnVar,
                 man_copy_type(man_trivial_type(ucType)));
      }
    }
    switch (ucTokenType)
    {
      case MULT :
        switch (ucType)
        {
          case SEL_TYPE_INT :
            iInt = man_int_value(pexprReturnVar);
            man_delete_expr(pexprReturnVar);
            man_int_expr(iInt * man_int_value(pexprExpr), pexprReturnVar);
            break;
          case SEL_TYPE_FLOAT :
            fFloat = man_float_value(pexprReturnVar);
            man_delete_expr(pexprReturnVar);
            man_float_expr(fFloat * man_float_value(pexprExpr),
                           pexprReturnVar);
            break;
          case SEL_TYPE_CHAR :
            cChar = man_char_value(pexprReturnVar);
            man_delete_expr(pexprReturnVar);
            man_char_expr(cChar * man_char_value(pexprExpr), pexprReturnVar);
            break;                
        }
        break;
      case DIV :
        switch (ucType)
        {
          case SEL_TYPE_INT :
            iInt = man_int_value(pexprReturnVar);
            man_delete_expr(pexprReturnVar);
            man_int_expr(iInt / man_int_value(pexprExpr), pexprReturnVar);
            break;
          case SEL_TYPE_FLOAT :
            fFloat = man_float_value(pexprReturnVar);
            man_delete_expr(pexprReturnVar);
            man_float_expr(fFloat / man_float_value(pexprExpr),
                           pexprReturnVar);
            break;
          case SEL_TYPE_CHAR :
            cChar = man_char_value(pexprReturnVar);
            man_delete_expr(pexprReturnVar);
            man_char_expr(cChar / man_char_value(pexprExpr), pexprReturnVar);
            break;                
        }
        break;
      case MOD :
        switch (ucType)
        {
          case SEL_TYPE_INT :
            iInt = man_int_value(pexprReturnVar);
            man_delete_expr(pexprReturnVar);
            man_int_expr(iInt % man_int_value(pexprExpr), pexprReturnVar);
            break;
          case SEL_TYPE_CHAR :
            cChar = man_char_value(pexprReturnVar);
            man_delete_expr(pexprReturnVar);
            man_char_expr(cChar % man_char_value(pexprExpr), pexprReturnVar);
            break;                
        }
        break;
    }
    man_delete_expr(pexprExpr);
    man_free_expr(pexprExpr);
  }
  return(pexprReturnVar);
}      

SEL_EXPR* par_level3 (SEL_PROGRAM *pprogProgram)
{

  SEL_MODULE *pmodModule;                                     
  SEL_EXPR   *pexprReturnVar,
             *pexprExpr;
  UCHAR       ucType,
              ucTokenType;
  TYPE_INT    iInt;
  TYPE_FLOAT  fFloat;
  TYPE_CHAR   cChar;
                                               
  pmodModule = pprogProgram->ppmodModules[pprogProgram->uiMP];
  pexprReturnVar = par_level2(pprogProgram);
  if (pexprReturnVar)
    ucType = man_type(pexprReturnVar->pucType);
  while (SEL_LOOK_AHEAD == PLUS || SEL_LOOK_AHEAD == MINUS)
  {        
    par_get_token(pprogProgram, 0, 0);
    ucTokenType = SEL_ACT_TOKEN.ucType;
    pexprExpr = par_level2(pprogProgram);
    if (ucType == SEL_TYPE_POINTER)
    {
      if (man_type(pexprExpr->pucType) != SEL_TYPE_INT)
        man_cast(pprogProgram, pexprExpr,
                 man_copy_type(man_trivial_type(SEL_TYPE_INT)));
    }
    else
      if (ucType != man_type(pexprExpr->pucType))
      {
        if (man_type_greater(ucType, man_type(pexprExpr->pucType)))
          man_cast(pprogProgram, pexprExpr,
                   man_copy_type(man_trivial_type(ucType)));
        else
        {
          ucType = man_type(pexprExpr->pucType);
          man_cast(pprogProgram, pexprReturnVar,
                   man_copy_type(man_trivial_type(ucType)));
        }
      }
    switch (ucTokenType)
    {
      case PLUS :
        switch (ucType)
        {
          case SEL_TYPE_INT :
            iInt = man_int_value(pexprReturnVar);
            man_delete_expr(pexprReturnVar);
            man_int_expr(iInt + man_int_value(pexprExpr), pexprReturnVar);
            break;
          case SEL_TYPE_FLOAT :
            fFloat = man_float_value(pexprReturnVar);
            man_delete_expr(pexprReturnVar);
            man_float_expr(fFloat + man_float_value(pexprExpr),
                           pexprReturnVar);
            break;
          case SEL_TYPE_CHAR :
            cChar = man_char_value(pexprReturnVar);
            man_delete_expr(pexprReturnVar);
            man_char_expr(cChar + man_char_value(pexprExpr), pexprReturnVar);
            break;     
          case SEL_TYPE_POINTER :
            if (pexprReturnVar->pucType[1] == SEL_TYPE_REFERENCE)
              man_remove_reference(pexprReturnVar);
            pexprReturnVar->ulValue += man_int_value(pexprExpr) *
              man_sizeof(pprogProgram, pexprReturnVar->pucType + 2);
            break;
        }
        break;
      case MINUS :
        switch (ucType)
        {
          case SEL_TYPE_INT :
            iInt = man_int_value(pexprReturnVar);
            man_delete_expr(pexprReturnVar);
            man_int_expr(iInt - man_int_value(pexprExpr), pexprReturnVar);
            break;
          case SEL_TYPE_FLOAT :
            fFloat = man_float_value(pexprReturnVar);
            man_delete_expr(pexprReturnVar);
            man_float_expr(fFloat - man_float_value(pexprExpr),
                           pexprReturnVar);
            break;
          case SEL_TYPE_CHAR :
            cChar = man_char_value(pexprReturnVar);
            man_delete_expr(pexprReturnVar);
            man_char_expr(cChar - man_char_value(pexprExpr), pexprReturnVar);
            break;     
          case SEL_TYPE_POINTER :
            if (pexprReturnVar->pucType[1] == SEL_TYPE_REFERENCE)
              man_remove_reference(pexprReturnVar);
            pexprReturnVar->ulValue -= man_int_value(pexprExpr) *
              man_sizeof(pprogProgram, pexprReturnVar->pucType + 2);
            break;
        }
        break;
    }
    man_delete_expr(pexprExpr);
    man_free_expr(pexprExpr);
  }
  return(pexprReturnVar);
}      

SEL_EXPR* par_level4 (SEL_PROGRAM *pprogProgram)
{

  SEL_MODULE *pmodModule;                                     
  SEL_EXPR   *pexprReturnVar,
             *pexprExpr;
  TYPE_INT    iInt;
  UCHAR       ucTokenType;
                                               
  pmodModule = pprogProgram->ppmodModules[pprogProgram->uiMP];
  pexprReturnVar = par_level3(pprogProgram);
  while (SEL_LOOK_AHEAD == AND || SEL_LOOK_AHEAD == OR ||
         SEL_LOOK_AHEAD == XOR || SEL_LOOK_AHEAD == LS ||
         SEL_LOOK_AHEAD == RS)
  {                          
    if (man_type(pexprReturnVar->pucType) != SEL_TYPE_INT)
      man_cast(pprogProgram, pexprReturnVar,
               man_copy_type(man_trivial_type(SEL_TYPE_INT)));
    par_get_token(pprogProgram, 0, 0);
    ucTokenType = SEL_ACT_TOKEN.ucType;
    pexprExpr = par_level3(pprogProgram);
    if (man_type(pexprExpr->pucType) != SEL_TYPE_INT)
      man_cast(pprogProgram, pexprReturnVar,
               man_copy_type(man_trivial_type(SEL_TYPE_INT)));
    switch (ucTokenType)
    {
      case AND :
        iInt = man_int_value(pexprReturnVar);
        man_delete_expr(pexprReturnVar);
        man_int_expr(iInt & man_int_value(pexprExpr), pexprReturnVar);
        break;
      case OR :
        iInt = man_int_value(pexprReturnVar);
        man_delete_expr(pexprReturnVar);
        man_int_expr(iInt | man_int_value(pexprExpr), pexprReturnVar);
        break;
      case XOR :
        iInt = man_int_value(pexprReturnVar);
        man_delete_expr(pexprReturnVar);
        man_int_expr(iInt ^ man_int_value(pexprExpr), pexprReturnVar);
        break;
      case LS :
        iInt = man_int_value(pexprReturnVar);
        man_delete_expr(pexprReturnVar);
        man_int_expr(iInt << man_int_value(pexprExpr), pexprReturnVar);
        break;
      case RS :
        iInt = man_int_value(pexprReturnVar);
        man_delete_expr(pexprReturnVar);
        man_int_expr(iInt >> man_int_value(pexprExpr), pexprReturnVar);
        break;
    }
    man_delete_expr(pexprExpr);
    man_free_expr(pexprExpr);
  }
  return(pexprReturnVar);
}      

SEL_EXPR* par_level5 (SEL_PROGRAM *pprogProgram)
{

  SEL_MODULE *pmodModule;                                     
  SEL_EXPR   *pexprReturnVar,
             *pexprExpr;
  UCHAR       ucType,
              ucTokenType;
  TYPE_INT    iInt;
  TYPE_FLOAT  fFloat;
  TYPE_CHAR   cChar;
                                               
  pmodModule = pprogProgram->ppmodModules[pprogProgram->uiMP];
  pexprReturnVar = par_level4(pprogProgram);
  if (pexprReturnVar)
    ucType = man_type(pexprReturnVar->pucType);
  while (SEL_LOOK_AHEAD == E || SEL_LOOK_AHEAD == L ||
         SEL_LOOK_AHEAD == G || SEL_LOOK_AHEAD == LE ||
         SEL_LOOK_AHEAD == GE || SEL_LOOK_AHEAD == NE)
  {
    if (ucType == SEL_TYPE_POINTER || ucType == SEL_TYPE_ARRAY)
    {
      man_cast(pprogProgram, pexprReturnVar,
               man_copy_type(man_trivial_type(SEL_TYPE_INT)));
      ucType = SEL_TYPE_INT;                                  
    }
    par_get_token(pprogProgram, 0, 0);
    ucTokenType = SEL_ACT_TOKEN.ucType;
    pexprExpr = par_level4(pprogProgram);
    if (man_type(pexprExpr->pucType) == SEL_TYPE_POINTER ||
        man_type(pexprExpr->pucType) == SEL_TYPE_ARRAY)
      man_cast(pprogProgram, pexprReturnVar,
               man_copy_type(man_trivial_type(SEL_TYPE_INT)));
    if (ucType != man_type(pexprExpr->pucType))
    {
      if (man_type_greater(ucType, man_type(pexprExpr->pucType)))
        man_cast(pprogProgram, pexprExpr,
                 man_copy_type(man_trivial_type(ucType)));
      else
      {
        ucType = man_type(pexprExpr->pucType);
        man_cast(pprogProgram, pexprReturnVar,
                 man_copy_type(man_trivial_type(ucType)));
      }
    }
    switch (ucTokenType)
    {
      case E :
        switch (ucType)
        {
          case SEL_TYPE_INT :
            iInt = man_int_value(pexprReturnVar);
            man_delete_expr(pexprReturnVar);
            man_int_expr(iInt == man_int_value(pexprExpr), pexprReturnVar);
            break;
          case SEL_TYPE_FLOAT :
            fFloat = man_float_value(pexprReturnVar);
            man_delete_expr(pexprReturnVar);
            man_int_expr(fFloat == man_float_value(pexprExpr), pexprReturnVar);
            break;
          case SEL_TYPE_CHAR :
            cChar = man_char_value(pexprReturnVar);
            man_delete_expr(pexprReturnVar);
            man_int_expr(cChar == man_char_value(pexprExpr), pexprReturnVar);
            break;     
        }
        break;
      case L :
        switch (ucType)
        {
          case SEL_TYPE_INT :
            iInt = man_int_value(pexprReturnVar);
            man_delete_expr(pexprReturnVar);
            man_int_expr(iInt < man_int_value(pexprExpr), pexprReturnVar);
            break;
          case SEL_TYPE_FLOAT :
            fFloat = man_float_value(pexprReturnVar);
            man_delete_expr(pexprReturnVar);
            man_int_expr(fFloat < man_float_value(pexprExpr), pexprReturnVar);
            break;
          case SEL_TYPE_CHAR :
            cChar = man_char_value(pexprReturnVar);
            man_delete_expr(pexprReturnVar);
            man_int_expr(cChar < man_char_value(pexprExpr), pexprReturnVar);
            break;     
        }
        break;
      case G :
        switch (ucType)
        {
          case SEL_TYPE_INT :
            iInt = man_int_value(pexprReturnVar);
            man_delete_expr(pexprReturnVar);
            man_int_expr(iInt > man_int_value(pexprExpr), pexprReturnVar);
            break;
          case SEL_TYPE_FLOAT :
            fFloat = man_float_value(pexprReturnVar);
            man_delete_expr(pexprReturnVar);
            man_int_expr(fFloat > man_float_value(pexprExpr), pexprReturnVar);
            break;
          case SEL_TYPE_CHAR :
            cChar = man_char_value(pexprReturnVar);
            man_delete_expr(pexprReturnVar);
            man_int_expr(cChar > man_char_value(pexprExpr), pexprReturnVar);
            break;     
        }
        break;
      case LE :
        switch (ucType)
        {
          case SEL_TYPE_INT :
            iInt = man_int_value(pexprReturnVar);
            man_delete_expr(pexprReturnVar);
            man_int_expr(iInt <= man_int_value(pexprExpr), pexprReturnVar);
            break;
          case SEL_TYPE_FLOAT :
            fFloat = man_float_value(pexprReturnVar);
            man_delete_expr(pexprReturnVar);
            man_int_expr(fFloat <= man_float_value(pexprExpr), pexprReturnVar);
            break;
          case SEL_TYPE_CHAR :
            cChar = man_char_value(pexprReturnVar);
            man_delete_expr(pexprReturnVar);
            man_int_expr(cChar <= man_char_value(pexprExpr), pexprReturnVar);
            break;     
        }
        break;
      case GE :
        switch (ucType)
        {
          case SEL_TYPE_INT :
            iInt = man_int_value(pexprReturnVar);
            man_delete_expr(pexprReturnVar);
            man_int_expr(iInt >= man_int_value(pexprExpr), pexprReturnVar);
            break;
          case SEL_TYPE_FLOAT :
            fFloat = man_float_value(pexprReturnVar);
            man_delete_expr(pexprReturnVar);
            man_int_expr(fFloat >= man_float_value(pexprExpr), pexprReturnVar);
            break;
          case SEL_TYPE_CHAR :
            cChar = man_char_value(pexprReturnVar);
            man_delete_expr(pexprReturnVar);
            man_int_expr(cChar >= man_char_value(pexprExpr), pexprReturnVar);
            break;     
        }
        break;
      case NE :
        switch (ucType)
        {
          case SEL_TYPE_INT :
            iInt = man_int_value(pexprReturnVar);
            man_delete_expr(pexprReturnVar);
            man_int_expr(iInt != man_int_value(pexprExpr), pexprReturnVar);
            break;
          case SEL_TYPE_FLOAT :
            fFloat = man_float_value(pexprReturnVar);
            man_delete_expr(pexprReturnVar);
            man_int_expr(fFloat != man_float_value(pexprExpr), pexprReturnVar);
            break;
          case SEL_TYPE_CHAR :
            cChar = man_char_value(pexprReturnVar);
            man_delete_expr(pexprReturnVar);
            man_int_expr(cChar != man_char_value(pexprExpr), pexprReturnVar);
            break;     
        }
        break;
    }
    man_delete_expr(pexprExpr);
    man_free_expr(pexprExpr);
  }
  return(pexprReturnVar);
}      

SEL_EXPR* par_level6 (SEL_PROGRAM *pprogProgram)
{

  SEL_MODULE *pmodModule;                                     
  TYPE_INT    iExpr1,
              iExpr2;
  SEL_EXPR   *pexprReturnVar,
             *pexprExpr;
  TYPE_INT    iInt;
  UCHAR       ucTokenType;
                                               
  pmodModule = pprogProgram->ppmodModules[pprogProgram->uiMP];
  pexprReturnVar = par_level5(pprogProgram);
  while (SEL_LOOK_AHEAD == LAND || SEL_LOOK_AHEAD == LOR ||
         SEL_LOOK_AHEAD == LXOR)
  {                          
    if (man_type(pexprReturnVar->pucType) != SEL_TYPE_INT)
      man_cast(pprogProgram, pexprReturnVar,
               man_copy_type(man_trivial_type(SEL_TYPE_INT)));
    par_get_token(pprogProgram, 0, 0);
    ucTokenType = SEL_ACT_TOKEN.ucType;
    pexprExpr = par_level5(pprogProgram);
    if (man_type(pexprExpr->pucType) != SEL_TYPE_INT)
      man_cast(pprogProgram, pexprReturnVar,
               man_copy_type(man_trivial_type(SEL_TYPE_INT)));
    switch (ucTokenType)
    {
      case LAND :
        iInt = man_int_value(pexprReturnVar);
        man_delete_expr(pexprReturnVar);
        man_int_expr(iInt && man_int_value(pexprExpr), pexprReturnVar);
        break;
      case LOR :
        iInt = man_int_value(pexprReturnVar);
        man_delete_expr(pexprReturnVar);
        man_int_expr(iInt || man_int_value(pexprExpr), pexprReturnVar);
        break;
      case LXOR :   
        iExpr1 = man_int_value(pexprReturnVar);
        iExpr2 = man_int_value(pexprExpr);
        man_delete_expr(pexprReturnVar);
        man_int_expr((iExpr1 && !iExpr2) || (iExpr2 && !iExpr1),
                     pexprReturnVar);
        break;
    }
    man_delete_expr(pexprExpr);
    man_free_expr(pexprExpr);
  }
  return(pexprReturnVar);
}      

SEL_EXPR* par_expr (SEL_PROGRAM *pprogProgram)
{
  
  CNT_STACK  *psStack        = NULL;
  SEL_MODULE *pmodModule;                                     
  SEL_EXPR   *pexprReturnVar,
             *pexprExpr;
  UCHAR       ucWorking      = TRUE,
             *pucType;
  
  pmodModule = pprogProgram->ppmodModules[pprogProgram->uiMP];
  if (SEL_LOOK_AHEAD == DELETE)
  {
    par_get_token(pprogProgram, 0, 0);
    pexprExpr = par_level6(pprogProgram);
    if (pexprExpr->pucType[1] == SEL_TYPE_REFERENCE)
      man_remove_reference(pexprExpr);
    utl_free((void*)pexprExpr->ulValue);
    man_delete_expr(pexprExpr);
    man_free_expr(pexprExpr);
    return(NULL);
  }
  pexprReturnVar = par_level6(pprogProgram);
  do
  {
    switch (SEL_LOOK_AHEAD)
    {
      case ASSIGN :        
        if (!psStack)
          psStack = cnt_stack_make();
        cnt_stack_put(psStack, (ULONG)pexprReturnVar);
        par_get_token(pprogProgram, 0, 0);
        pexprReturnVar = par_level6(pprogProgram);
        break;
      default :
        ucWorking = FALSE;
        break;
    }
  } while (ucWorking);
  if (psStack)
  {
    while (!cnt_stack_is_empty(psStack))
    {                  
      pexprExpr = (SEL_EXPR*)cnt_stack_get(psStack);
      pucType = man_alloc_type();
      *pucType = *(pexprExpr->pucType) - 1;
      memcpy(pucType + 1, pexprExpr->pucType + 2, *pucType);
      man_cast(pprogProgram, pexprReturnVar, pucType);
      man_assign(pprogProgram, man_expr_reference(pexprExpr), pexprReturnVar);
      man_delete_expr(pexprReturnVar);
      man_free_expr(pexprReturnVar);
      pexprReturnVar = pexprExpr;
    }
    cnt_stack_destroy(psStack);
  }
  return(pexprReturnVar);
}

SEL_EXPR* par_statement (SEL_PROGRAM *pprogProgram)
{
  SEL_MODULE *pmodModule;
  SEL_EXPR   *pexprExpr;
  TYPE_INT   *piReference,
              iLimit,
              iIncrement;
  UCHAR       ucEqual,
              ucDown;
  ULONG       ulNext,
              ulCounter1;
  
  pmodModule = pprogProgram->ppmodModules[pprogProgram->uiMP];
  while (TRUE)
  {
    switch (SEL_LOOK_AHEAD)
    {
      case SEMICOLON :
        par_get_token(pprogProgram, 0, 0);
        break;
      case CASE :
        par_get_token(pprogProgram, 0, 0);
      case DEFAULT :
        par_get_token(pprogProgram, 0, 0);
        par_get_token(pprogProgram, TYPEspec, 3);
        break;
      case ELSE :
      case END :
      case UNTIL :
        return(NULL);
      case BREAK :
        pprogProgram->ucReason = BREAK;
        return(NULL);
      case RETURN :                
        par_get_token(pprogProgram, 0, 0);
        if (SEL_LOOK_AHEAD == SEMICOLON)
        {
          pexprExpr = man_alloc_expr();
          return(man_int_expr(0, pexprExpr));
        }
        pexprExpr = par_expr(pprogProgram);
        par_get_token(pprogProgram, SEMICOLON, 4);
        return(pexprExpr);
      case IF :
        par_get_token(pprogProgram, 0, 0);
        ulNext = SEL_ACT_TOKEN.ulNext;
        par_get_token(pprogProgram, OPENbracket, 0);
        pexprExpr = par_expr(pprogProgram);  
        man_cast(pprogProgram, pexprExpr, 
                 man_copy_type(man_trivial_type(SEL_TYPE_INT)));
        if (man_int_value(pexprExpr))
        {                 
          man_delete_expr(pexprExpr);
          man_free_expr(pexprExpr);      
          par_get_token(pprogProgram, CLOSEbracket, 1);
          pexprExpr = par_statement(pprogProgram);
          if (pexprExpr)
            return(pexprExpr);
          else
            if (pprogProgram->ucReason == BREAK)
              return(NULL);
          if (SEL_LOOK_AHEAD == ELSE)
          {
            par_get_token(pprogProgram, 0, 0);
            pprogProgram->ulIP = SEL_ACT_TOKEN.ulNext;
          }
          par_get_token(pprogProgram, END, 7);
        }
        else
        {
          man_delete_expr(pexprExpr);
          man_free_expr(pexprExpr);
          pprogProgram->ulIP = ulNext;
          par_get_token(pprogProgram, 0, 0);
          if (SEL_ACT_TOKEN.ucType == ELSE)
          {
            pexprExpr = par_statement(pprogProgram);
            if (pexprExpr)
              return(pexprExpr);
            else
              if (pprogProgram->ucReason == BREAK)
                return(NULL);
            par_get_token(pprogProgram, END, 7);
          }
        }
        break;
      case WHILE :
        par_get_token(pprogProgram, 0, 0);
        ulNext = SEL_ACT_TOKEN.ulNext;
        while (TRUE)
        {
          par_get_token(pprogProgram, OPENbracket, 0);
          pexprExpr = par_expr(pprogProgram);
          man_cast(pprogProgram, pexprExpr,
                   man_copy_type(man_trivial_type(SEL_TYPE_INT)));
          par_get_token(pprogProgram, CLOSEbracket, 1);
          if (man_int_value(pexprExpr))
          {
            man_delete_expr(pexprExpr);
            man_free_expr(pexprExpr);
            pexprExpr = par_statement(pprogProgram);
            if (pexprExpr)
              return(pexprExpr);
            else
              if (pprogProgram->ucReason == BREAK)
                return(NULL);
            par_get_token(pprogProgram, END, 7);
            pprogProgram->ulIP = SEL_ACT_TOKEN.ulNext;
          }
          else
          {
            man_delete_expr(pexprExpr);
            man_free_expr(pexprExpr);
            pprogProgram->ulIP = ulNext;
            par_get_token(pprogProgram, 0, 0);
            break;
          } 
        }
        break;
      case FOR :
        par_get_token(pprogProgram, 0, 0);
        ulNext = SEL_ACT_TOKEN.ulNext;
        par_get_token(pprogProgram, OPENbracket, 0);
        pexprExpr = par_primary(pprogProgram);
        if (pexprExpr->pucType[1] != SEL_TYPE_REFERENCE)
          man_error(pprogProgram, 24, NULL);
        piReference = (TYPE_INT*)pexprExpr->ulValue;
        man_delete_expr(pexprExpr);
        man_free_expr(pexprExpr);
        par_get_token(pprogProgram, ASSIGN, 25);
        pexprExpr = par_expr(pprogProgram);
        man_cast(pprogProgram, pexprExpr,
                 man_copy_type(man_trivial_type(SEL_TYPE_INT)));
        *piReference = man_int_value(pexprExpr);
        man_delete_expr(pexprExpr);
        man_free_expr(pexprExpr);
        par_get_token(pprogProgram, 0, 0);
        if (SEL_ACT_TOKEN.ucType == TO)
          ucDown = FALSE;
        else
          if (SEL_ACT_TOKEN.ucType = DOWNTO)
            ucDown = TRUE;
          else
            man_error(pprogProgram, 26, NULL);
        pexprExpr = par_expr(pprogProgram);
        man_cast(pprogProgram, pexprExpr,
                 man_copy_type(man_trivial_type(SEL_TYPE_INT)));
        iLimit = man_int_value(pexprExpr);
        man_delete_expr(pexprExpr);
        man_free_expr(pexprExpr);
        if (SEL_LOOK_AHEAD == STEP)
        {
          par_get_token(pprogProgram, 0, 0);
          pexprExpr = par_expr(pprogProgram);
          man_cast(pprogProgram, pexprExpr,
                   man_copy_type(man_trivial_type(SEL_TYPE_INT)));
          iIncrement = man_int_value(pexprExpr);
          man_delete_expr(pexprExpr);
          man_free_expr(pexprExpr);
        }
        else
          iIncrement = 1;
        if (ucDown)
          iIncrement = -iIncrement;
        par_get_token(pprogProgram, CLOSEbracket, 1);
        ulCounter1 = pprogProgram->ulIP;
        while (TRUE)
        {
          if (ucDown)
          {
            if (*piReference < iLimit)
              break;
          }
          else
            if (*piReference > iLimit)
              break;
          pexprExpr = par_statement(pprogProgram);
          if (pexprExpr)
            return(pexprExpr);
          else
            if (pprogProgram->ucReason == BREAK)
              return(NULL);
          *piReference += iIncrement;
          pprogProgram->ulIP = ulCounter1;
        }
        pprogProgram->ulIP = ulNext;
        par_get_token(pprogProgram, END, 7);
        break;
      case REPEAT :
        par_get_token(pprogProgram, 0, 0);
        ulNext = pprogProgram->ulIP;
        while (TRUE)
        {
          pexprExpr = par_statement(pprogProgram);
          if (pexprExpr)
            return(pexprExpr);
          else
            if (pprogProgram->ucReason == BREAK)
              return(NULL);
          par_get_token(pprogProgram, UNTIL, 10);
          par_get_token(pprogProgram, OPENbracket, 0);
          pexprExpr = par_expr(pprogProgram);
          man_cast(pprogProgram, pexprExpr,
                   man_copy_type(man_trivial_type(SEL_TYPE_INT)));
          if (man_int_value(pexprExpr))
          {
            man_delete_expr(pexprExpr);
            man_free_expr(pexprExpr);
            par_get_token(pprogProgram, CLOSEbracket, 1);
            break;
          }
          else
          {
            man_delete_expr(pexprExpr);
            man_free_expr(pexprExpr);
            pprogProgram->ulIP = ulNext;
          }
        }
        break;
      case SWITCH :
        par_get_token(pprogProgram, 0, 0);
        ulNext = SEL_ACT_TOKEN.ulNext;
        par_get_token(pprogProgram, OPENbracket, 0);
        pexprExpr = par_expr(pprogProgram);
        while (TRUE)
        {
          pprogProgram->ulIP = ulNext;
          par_get_token(pprogProgram, 0, 0);
          ulNext = SEL_ACT_TOKEN.ulNext;
          ucEqual = FALSE;
          if (SEL_ACT_TOKEN.ucType == END)
          {
            man_delete_expr(pexprExpr);
            man_free_expr(pexprExpr);
            break;                   
          }
          switch (SEL_ACT_TOKEN.ucType)
          {
            case CASE :
              par_constant(pprogProgram);
              switch (SEL_ACT_TOKEN.ucType)
              {
                case INT_CONSTANT :
                  ucEqual = SEL_ACT_TOKEN.iInt == man_int_value(pexprExpr);
                  break;
                case FLOAT_CONSTANT :
                  ucEqual = SEL_ACT_TOKEN.fFloat == man_float_value(pexprExpr);
                  break;
                case CHAR_CONSTANT :
                  ucEqual = SEL_ACT_TOKEN.cChar == man_char_value(pexprExpr);
                  break;         
              }
              break;
            case DEFAULT :
              ucEqual = TRUE;
              break;               
            default :
              man_error(pprogProgram, 12, NULL);
          }
          if (ucEqual)
          { 
            man_delete_expr(pexprExpr);
            man_free_expr(pexprExpr);
            par_get_token(pprogProgram, TYPEspec, 3);
            pexprExpr = par_statement(pprogProgram);
            if (pexprExpr)
              return(pexprExpr);
            pprogProgram->ucReason = 0;
            pprogProgram->ulIP = ulNext;
            par_get_token(pprogProgram, 0, 0);
            while (SEL_ACT_TOKEN.ucType != END)
            {                             
              pprogProgram->ulIP = SEL_ACT_TOKEN.ulNext;
              par_get_token(pprogProgram, 0, 0);
            }
            break;
          }
        }
        break;
      default :
        pexprExpr = par_expr(pprogProgram);
        if (pexprExpr)
        {
          man_delete_expr(pexprExpr);
          man_free_expr(pexprExpr);
        }
        par_get_token(pprogProgram, SEMICOLON, 4);
        break;
    }
  }
}
