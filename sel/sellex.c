/***************************************************************************
 ***************************************************************************
 ***                                                                     ***
 ***             Schani Electronics Language - Interpreter               ***
 ***                                                                     ***
 ***                       Lexikalische Analyse                          ***
 ***                                                                     ***
 ***                  (c) 1993 by Schani Electronics                     ***
 ***                                                                     ***
 ***************************************************************************
 ***************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <ctype.h>
#include <gemein.h>
#include <video.h>
#include <mouse.h>
#include <sthelp.h>
#include <dlg.h>
#include <window.h>
#include <utility.h>
#include <global.h>
#include <contain.h>
#include <fcntl.h>
#ifdef __TURBOC__
#include <alloc.h>
#endif
#include <buffers.h>
#include "sel.h"

UINT             uiLine,
                 uiFuncs,
                 uiLocals,
                 uiGlobals,
                 uiLocalStructs,
                 uiGlobalStructs,
                 uiLineBufLength,
                 uiIncludeLevel;
UCHAR            ucLocal                         = FALSE,
                 ucGlobal                        = FALSE,
                *pucLines                        = NULL;
ULONG            ulLastLine;
SEL_TOKEN        tokToken;
SEL_LEX_SYMBOL  *psymFirst,
                *psymLast;
SEL_SYMBOL_TREE *psymtRoot;
SEL_DEFINE_TREE *pdeftRoot;
SEL_LEX_BUFFER  *aplbufStack[LEX_INCLUDE_LEVELS];
FILE            *pfileOut,
                *pfileTemp;
CNT_STACK       *psNext;
CNT_QUEUE       *pqFuncs,
                *pqLocals,
                *pqGlobals;
CHAR            *apcReserved[]                   = {
                                                     "if", "while", "for",
                                                     "switch", "repeat", "end",
                                                     "until", "else", "case",
                                                     "default", "return",
                                                     "break", "char", "int",
                                                     "float", "void", "global",
                                                     "local", "sizeof",
                                                     "function", "struct",
                                                     "begin", "new", "delete",
                                                     "extern", "to", "downto",
                                                     "step", NULL
                                                   },
                *apcPreprocessor[]               = {
                                                     "#include", "#define",
                                                     NULL
                                                   };

SEL_LEX_BUFFER* lex_create_buffer (CHAR *pcName)
{
  
  SEL_LEX_BUFFER *plbufReturnVar;
  
  if (!(plbufReturnVar = utl_alloc(sizeof(SEL_LEX_BUFFER))))
    return(NULL);
  if (!(plbufReturnVar->pcBuffer = utl_alloc(LEX_BUFFER_SIZE)))
    return(NULL);
  if (!(plbufReturnVar->pbufFile = buf_open_file(NULL, pcName, "r")))
    return(NULL);
  plbufReturnVar->uiBufferLength = buf_read(plbufReturnVar->pcBuffer, 1,
                                            LEX_BUFFER_SIZE,
                                            plbufReturnVar->pbufFile);
  plbufReturnVar->uiBufferPos = 0;
  return(plbufReturnVar);
}                        

void lex_delete_buffer (SEL_LEX_BUFFER *plbufBuffer)
{
  buf_close(plbufBuffer->pbufFile);
  utl_free(plbufBuffer->pcBuffer);
  utl_free(plbufBuffer);
}

UCHAR lex_next_char (SEL_LEX_BUFFER *plbufBuffer)
{
  if (plbufBuffer->uiBufferPos >= plbufBuffer->uiBufferLength - 1)
  {
    plbufBuffer->uiBufferLength = buf_read(plbufBuffer->pcBuffer, 1,
                                           LEX_BUFFER_SIZE,
                                           plbufBuffer->pbufFile);
    plbufBuffer->uiBufferPos = 0;
    if (!plbufBuffer->uiBufferLength)
      return(FALSE);
  }
  else
    plbufBuffer->uiBufferPos++;
  return(TRUE);
}

UCHAR lex_analyze_identifier (SEL_LEX_BUFFER *plbufBuffer)
{

  CHAR  *pcString,
       **ppcCounter;
  UINT   uiAllocated,
         uiUsed       = 0;

  pcString = utl_alloc(uiAllocated = 10);
  pcString[uiUsed++] = LEX_CHAR;
  while (!LEX_EOF)
  {
    if (!lex_next_char(plbufBuffer))
      break;
    if (isalnum(LEX_CHAR) || LEX_CHAR == '_')
    {
      if (uiUsed + 1 == uiAllocated)
        pcString = utl_realloc(pcString, uiAllocated += 10);
      pcString[uiUsed++] = LEX_CHAR;
    }
    else
      break;
  }
  pcString[uiUsed] = 0;
  tokToken.pcString = pcString;
  for (ppcCounter = apcReserved; *ppcCounter; ppcCounter++)
    if (!strcmp(pcString, *ppcCounter))
    {
      utl_free(pcString);
      return(tokToken.ucType = ppcCounter - apcReserved + 6);
    }
  return(tokToken.ucType = IDENTIFIER);
}

UCHAR lex_analyze_number (SEL_LEX_BUFFER *plbufBuffer)
{
  
  CHAR  *pcString;
  UINT   uiAllocated,
         uiUsed          = 0;
  UCHAR  ucDecimalPoint  = FALSE,
         ucExponent      = FALSE,
         ucExponentMinus = FALSE;

  pcString = utl_alloc(uiAllocated = 10);
  do
  {           
    if (LEX_CHAR == '.')
    {
      if (ucDecimalPoint || ucExponent)
        break;
      ucDecimalPoint = TRUE;
    }             
    else
      if (LEX_CHAR == 'e' || LEX_CHAR == 'E')
      {
        if (ucExponent)
          break;
        ucExponent = TRUE;
      }
      else
        if (LEX_CHAR == '-')
        {
          if (ucExponentMinus || !ucExponent)
            break;
          ucExponentMinus = TRUE;
        }
        else
          if (!isdigit(LEX_CHAR))
            break;
    if (uiUsed + 1 == uiAllocated)
      pcString = utl_realloc(pcString, uiAllocated += 10);
    pcString[uiUsed++] = LEX_CHAR;
    if (!lex_next_char(plbufBuffer))
      break;
  } while (!LEX_EOF);
  pcString[uiUsed] = 0;
  if (ucDecimalPoint || ucExponent)
  {
    tokToken.fFloat = atof(pcString);
    utl_free(pcString);
    return(tokToken.ucType = FLOAT_CONSTANT);
  }
  tokToken.iInt = atol(pcString);
  utl_free(pcString);
  return(tokToken.ucType = INT_CONSTANT);
}

UCHAR lex_analyze_preprocessor (SEL_LEX_BUFFER *plbufBuffer)
{

  CHAR  *pcString,
       **ppcCounter;
  UINT   uiAllocated,
         uiUsed       = 0;

  pcString = utl_alloc(uiAllocated = 10);
  pcString[uiUsed++] = LEX_CHAR;
  while (!LEX_EOF)
  {
    if (!lex_next_char(plbufBuffer))
      break;
    if (isalpha(LEX_CHAR))
    {
      if (uiUsed + 1 == uiAllocated)
        pcString = utl_realloc(pcString, uiAllocated += 10);
      pcString[uiUsed++] = LEX_CHAR;
    }
    else
      break;
  }
  pcString[uiUsed] = 0;
  for (ppcCounter = apcPreprocessor; *ppcCounter; ppcCounter++)
    if (!strcmp(pcString, *ppcCounter))
    {
      utl_free(pcString);
      return(tokToken.ucType = ppcCounter - apcPreprocessor + INCLUDE);
    }
  return(LEX_ERROR);
}

UCHAR lex_analyze_string (SEL_LEX_BUFFER *plbufBuffer)
{
  
  CHAR  *pcString;
  UINT   uiAllocated,
         uiUsed       = 0;
  UCHAR  ucIgnoreNext = FALSE;
                                   
  pcString = utl_alloc(uiAllocated = 10);
  do
  {
    if (!lex_next_char(plbufBuffer))
    {
      utl_free(pcString);
      return(LEX_ERROR);
    }
    if (!ucIgnoreNext)
    {
      if (LEX_CHAR == '\\')
        ucIgnoreNext = TRUE;
      else
        if (LEX_CHAR == '\"')
        {
          lex_next_char(plbufBuffer);
          break;
        }
        else
          if (LEX_CHAR == '\n')
          {
            utl_free(pcString);
            return(LEX_ERROR);
          }
    }
    else
      ucIgnoreNext = FALSE;
    if (uiUsed + 1 == uiAllocated)
      pcString = utl_realloc(pcString, uiAllocated += 10);
    pcString[uiUsed++] = LEX_CHAR;
  } while (!LEX_EOF);
  pcString[uiUsed] = 0;
  lex_process_string(pcString);
  tokToken.pcString = pcString;
  return(tokToken.ucType = STRING_CONSTANT);
}       

UCHAR lex_analyze_special (SEL_LEX_BUFFER *plbufBuffer)
{             

  UCHAR ucReadChar = TRUE;  

  switch (LEX_CHAR)
  {
    case ':' :
      tokToken.ucType = TYPEspec;
      break;
    case ',' :
      tokToken.ucType = COMMA;
      break;
    case '(' :
      tokToken.ucType = OPENbracket;
      break;
    case ')' :
      tokToken.ucType = CLOSEbracket;
      break;
    case '[' :
      tokToken.ucType = OPENindex;
      break;
    case ']' :
      tokToken.ucType = CLOSEindex;
      break;
    case '\\' :
      tokToken.ucType = BACKSLASH;
      break;
    case ';' :
      tokToken.ucType = SEMICOLON;
      break;
    case '%' :
      tokToken.ucType = MOD;
      break;
    case '+' :
      tokToken.ucType = PLUS;
      break;            
    case '*' :
      tokToken.ucType = MULT;
      break;
    case '~' :
      tokToken.ucType = NOT;
      break;
    case '=' :
      tokToken.ucType = ASSIGN;
      if (!lex_next_char(plbufBuffer))
        break;
      if (LEX_CHAR == '=')
      {
        tokToken.ucType = E;
        break;
      }
      ucReadChar = FALSE;
      break;
    case '!' :
      tokToken.ucType = LNOT;
      if (!lex_next_char(plbufBuffer))
        break;
      if (LEX_CHAR == '=')
      {
        tokToken.ucType = NE;
        break;
      }
      ucReadChar = FALSE;
      break;
    case '-' :
      tokToken.ucType = MINUS;
      if (!lex_next_char(plbufBuffer))
        break;
      if (LEX_CHAR == '>')
      {
        tokToken.ucType = ARROW;
        break;
      }
      ucReadChar = FALSE;
      break;
    case '^' :
      tokToken.ucType = XOR;
      if (!lex_next_char(plbufBuffer))
        break;
      if (LEX_CHAR == '^')
      {
        tokToken.ucType = LXOR;
        break;
      }
      ucReadChar = FALSE;
      break;
    case '&' :
      tokToken.ucType = AND;
      if (!lex_next_char(plbufBuffer))
        break;
      if (LEX_CHAR == '&')
      {
        tokToken.ucType = LAND;
        break;
      }
      ucReadChar = FALSE;
      break;
    case '|' :
      tokToken.ucType = OR;
      if (!lex_next_char(plbufBuffer))
        break;
      if (LEX_CHAR == '|')
      {
        tokToken.ucType = LOR;
        break;
      }
      ucReadChar = FALSE;
      break;
    case '<' :
      tokToken.ucType = L;
      if (!lex_next_char(plbufBuffer))
        break;
      switch (LEX_CHAR)
      {
        case '=' :
          tokToken.ucType = LE;
          break;
        case '<' :
          tokToken.ucType = LS;
          break;
        default :
          ucReadChar = FALSE;
          break;
      }
      break;
    case '>' :
      tokToken.ucType = G;
      if (!lex_next_char(plbufBuffer))
        break;
      switch (LEX_CHAR)
      {
        case '=' :
          tokToken.ucType = GE;
          break;
        case '>' :
          tokToken.ucType = RS;
          break;
        default :
          ucReadChar = FALSE;
          break;
      }
      break;
    case '/' :
      tokToken.ucType = DIV;
      ucReadChar = FALSE;
      if (!lex_next_char(plbufBuffer))
        break;
      if (LEX_CHAR == '/')
      {
        do
        {
          if (!lex_next_char(plbufBuffer))
            return(0);
        } while (LEX_CHAR != '\n');
        tokToken.ucType = LEX_NONE;
      }
      break;
  }
  if (ucReadChar)
    lex_next_char(plbufBuffer);
  return(tokToken.ucType);
}

UCHAR lex_get_token (SEL_LEX_BUFFER *plbufBuffer)
{                    
  
  CHAR  acChar[2];
  UCHAR ucType;
  
  do
  {
    tokToken.ucType = LEX_NONE;
    if (LEX_EOF)
      return(0);
    if (isalpha(LEX_CHAR))
      return(lex_analyze_identifier(plbufBuffer));
    if (isdigit(LEX_CHAR) || LEX_CHAR == '.')
      return(lex_analyze_number(plbufBuffer));
    switch (LEX_CHAR)
    {
      case '#' :
        return(lex_analyze_preprocessor(plbufBuffer));
      case '\"' :
        return(lex_analyze_string(plbufBuffer));
      case '\'' :
        if (!lex_next_char(plbufBuffer))
          return(LEX_ERROR);
        if ((acChar[0] = LEX_CHAR) == '\\')
        {
          if (!lex_next_char(plbufBuffer))
            return(LEX_ERROR);
          acChar[1] = LEX_CHAR;
        }
        if (!lex_next_char(plbufBuffer))
          return(LEX_ERROR);
        if (LEX_CHAR != '\'')
          return(LEX_ERROR);
        lex_next_char(plbufBuffer);
        tokToken.cChar = lex_process_char(acChar);
        return(tokToken.ucType = CHAR_CONSTANT);
      case ' ' :
      case '\t' :
        lex_next_char(plbufBuffer);
        break;
      case '\n' :
        lex_next_char(plbufBuffer);
        if (!uiIncludeLevel)
        {
          if (uiLine == uiLineBufLength)
          {
            uiLineBufLength += 1024;
            pucLines = utl_realloc(pucLines, uiLineBufLength);
          }
          pucLines[uiLine] = ftell(pfileTemp) - ulLastLine;
          ulLastLine = ftell(pfileTemp);
          uiLine++;
        }
        tokToken.ucType = NEWLINE;
        return(NEWLINE);
      default :
        if ((ucType = lex_analyze_special(plbufBuffer)))
          return(ucType);
        break;
    }
  } while (TRUE);
}
 
CHAR lex_process_char (CHAR *pcChar)
{
  if (*pcChar != '\\')
    return(*pcChar);
  pcChar++;
  switch (*pcChar)
  {
    case 'a' :
      return('\a');
    case 'v' :
      return('\v');
    case 'b' :
      return('\b');
    case 'f' :
      return('\f');
    case 'n' :
      return('\n');
    case 'r' :
      return('\r');
    case 't' :
      return('\t');
  }
  return(*pcChar);
}
                       
void lex_process_string (CHAR *pcString)
{       
  
  CHAR  *pcCounter;
  UCHAR  ucSpecial = FALSE;

  for (pcCounter = pcString; *pcString; pcCounter++)
  {     
    if (*pcString == '\\')
      ucSpecial = TRUE;
    *pcCounter = lex_process_char(pcString);
    if (ucSpecial)
    {
      pcString += 2;
      ucSpecial = FALSE;
    }
    else
      pcString++;
  }  
  *pcCounter = 0;
}

UINT lex_add_symbol (CHAR *pcName)
{                                 
  
  SEL_LEX_SYMBOL  *psymSymbol;
  SEL_SYMBOL_TREE *psymtSymbol,
                  *psymtNode;
  INT              iCompResult;
  
  psymSymbol = utl_alloc(sizeof(SEL_LEX_SYMBOL));
  psymSymbol->pcName = utl_alloc(strlen(pcName) + 1);
  strcpy(psymSymbol->pcName, pcName);
  psymSymbol->psymNext = NULL;
  if (psymLast)
  {
    psymSymbol->uiNumber = psymLast->uiNumber + 1;
    psymLast->psymNext = psymSymbol;
    psymLast = psymSymbol;
  }
  else
  {
    psymSymbol->uiNumber = 0;
    psymFirst = psymLast = psymSymbol;
  }
  psymtSymbol = utl_alloc(sizeof(SEL_SYMBOL_TREE));
  psymtSymbol->psymSymbol = psymSymbol;
  psymtSymbol->psymtLeft = psymtSymbol->psymtRight = NULL;
  if (psymtRoot)
  {
    for (psymtNode = psymtRoot; ; )
    {
      iCompResult = strcmp(pcName, psymtNode->psymSymbol->pcName);
      if (iCompResult < 0)
      {
        if (psymtNode->psymtLeft)
          psymtNode = psymtNode->psymtLeft;
        else
        {
          psymtNode->psymtLeft = psymtSymbol;
          return(psymtSymbol->psymSymbol->uiNumber);
        }
      }                     
      else
      {
        if (psymtNode->psymtRight)
          psymtNode = psymtNode->psymtRight;
        else
        {
          psymtNode->psymtRight = psymtSymbol;
          return(psymtSymbol->psymSymbol->uiNumber);
        }    
      }
    }
  }
  psymtRoot = psymtSymbol;
  return(psymtSymbol->psymSymbol->uiNumber);
}

UINT lex_get_symbol_number (CHAR *pcName)
{
  
  SEL_SYMBOL_TREE *psymtNode;
  INT              iCompResult;
  
  psymtNode = psymtRoot;
  while (psymtNode)
  {
    iCompResult = strcmp(pcName, psymtNode->psymSymbol->pcName);
    if (iCompResult < 0)
      psymtNode = psymtNode->psymtLeft;
    else
      if (iCompResult > 0)
        psymtNode = psymtNode->psymtRight;
      else
        return(psymtNode->psymSymbol->uiNumber);
  }
  return(lex_add_symbol(pcName));
}          

void lex_put_token (SEL_TOKEN *ptokToken)
{              
  
  ULONG           ulPos1,
                  ulPos2,
                  ulDummy;
  UINT            uiLength,
                  uiNumber;
  SEL_TOKEN_LIST *ptoklToken,
                 *ptoklCounter;

  switch (ptokToken->ucType)
  {
    case NEWLINE :
    case BACKSLASH :
      break;         
    case IF :
    case WHILE :
    case FOR :
    case SWITCH :
    case REPEAT :
      fwrite(&(ptokToken->ucType), sizeof(UCHAR), 1, pfileTemp);
      fwrite(&ulPos1, sizeof(ULONG), 1, pfileTemp);
      cnt_stack_put(psNext, ftell(pfileTemp));
      break;
    case END :
    case UNTIL :
      fwrite(&(ptokToken->ucType), sizeof(UCHAR), 1, pfileTemp);
      ulDummy = cnt_stack_get(psNext);
      switch (ulDummy)
      {
        case LOCAL_GLOBAL :
          ucLocal = ucGlobal = FALSE;
        case NO_ACTION :
          fwrite(&ulPos1, sizeof(ULONG), 1, pfileTemp);
          break;
        default :
          ulPos1 = ulDummy;
          fwrite(&ulPos1, sizeof(ULONG), 1, pfileTemp);
          ulPos2 = ftell(pfileTemp) - sizeof(ULONG) - sizeof(UCHAR);
          fseek(pfileTemp, ulPos1 - sizeof(ULONG), SEEK_SET);
          fwrite(&ulPos2, sizeof(ULONG), 1, pfileTemp);
          fseek(pfileTemp, 0, SEEK_END);
          break;
      }
      break;
    case ELSE :
    case CASE :
    case DEFAULT :
      fwrite(&(ptokToken->ucType), sizeof(UCHAR), 1, pfileTemp);
      ulPos1 = cnt_stack_get(psNext);
      fwrite(&ulPos1, sizeof(ULONG), 1, pfileTemp);
      ulPos2 = ftell(pfileTemp) - sizeof(ULONG) - sizeof(UCHAR);
      fseek(pfileTemp, ulPos1 - sizeof(ULONG), SEEK_SET);
      fwrite(&ulPos2, sizeof(ULONG), 1, pfileTemp);
      fseek(pfileTemp, 0, SEEK_END);
      cnt_stack_put(psNext, ftell(pfileTemp));
      break;
    case SUB :   
      if (!ucLocal && !ucGlobal)
      {
        ulPos1 = ftell(pfileTemp);
        cnt_queue_put(pqFuncs, ulPos1);
        uiFuncs++;
        cnt_stack_put(psNext, NO_ACTION);
      }
      fwrite(&(ptokToken->ucType), sizeof(UCHAR), 1, pfileTemp);
      break;
    case LOCAL :
      ulPos1 = ftell(pfileTemp);
      cnt_queue_put(pqLocals, ulPos1);
      ucLocal = TRUE;
      uiLocals++;
      fwrite(&(ptokToken->ucType), sizeof(UCHAR), 1, pfileTemp);
      cnt_stack_put(psNext, LOCAL_GLOBAL);
      break;               
    case GLOBAL :
      ulPos1 = ftell(pfileTemp);
      cnt_queue_put(pqGlobals, ulPos1);
      ucGlobal = TRUE;
      uiGlobals++;
      fwrite(&(ptokToken->ucType), sizeof(UCHAR), 1, pfileTemp);
      cnt_stack_put(psNext, LOCAL_GLOBAL);
      break;
    case STRUCT :
      fwrite(&(ptokToken->ucType), sizeof(UCHAR), 1, pfileTemp);
      if (ucLocal)
        uiLocalStructs++;
      else
        uiGlobalStructs++;         
      cnt_stack_put(psNext, NO_ACTION);
      break;
    case CHAR_CONSTANT :
      fwrite(&(ptokToken->ucType), sizeof(UCHAR), 1, pfileTemp);
      fwrite(&(ptokToken->cChar), sizeof(TYPE_CHAR), 1, pfileTemp);
      break;
    case INT_CONSTANT :
      fwrite(&(ptokToken->ucType), sizeof(UCHAR), 1, pfileTemp);
      fwrite(&(ptokToken->iInt), sizeof(TYPE_INT), 1, pfileTemp);
      break;
    case FLOAT_CONSTANT :
      fwrite(&(ptokToken->ucType), sizeof(UCHAR), 1, pfileTemp);
      fwrite(&(ptokToken->fFloat), sizeof(TYPE_FLOAT), 1, pfileTemp);
      break;                                     
    case STRING_CONSTANT :
      fwrite(&(ptokToken->ucType), sizeof(UCHAR), 1, pfileTemp);
      uiLength = strlen(ptokToken->pcString) + 1;
      fwrite(&uiLength, sizeof(UINT), 1, pfileTemp);
      fwrite(ptokToken->pcString, uiLength, 1, pfileTemp);
      break;
    case IDENTIFIER :
      ptoklToken = lex_get_define(ptokToken->pcString);
      if (ptoklToken)
        for (ptoklCounter = ptoklToken; ptoklCounter;
             ptoklCounter = ptoklCounter->ptoklNext)
          lex_put_token(&(ptoklCounter->tokToken));
      else
      {
        uiNumber = lex_get_symbol_number(ptokToken->pcString);
        fwrite(&(ptokToken->ucType), sizeof(UCHAR), 1, pfileTemp);
        fwrite(&uiNumber, sizeof(UINT), 1, pfileTemp);
      }
      break;
    default :
      fwrite(&(ptokToken->ucType), sizeof(UCHAR), 1, pfileTemp);
      break;
  }
}                                

void lex_include (void)
{                                   
  LEX_GET_TOKEN;
  if (tokToken.ucType != STRING_CONSTANT)
    lex_exit("String constant expected");
  if (++uiIncludeLevel >= LEX_INCLUDE_LEVELS)
    lex_exit("#include's nested too deeply");
  if (!(LEX_INPUT = lex_create_buffer(tokToken.pcString)))
    lex_exit("#include not found");
  utl_free(tokToken.pcString);
}

void lex_add_define (void)
{
  
  SEL_DEFINE_TREE *pdeftDefine, 
                  *pdeftNode;
  SEL_TOKEN_LIST  *ptoklLast;
  INT              iCompResult;
                  
  pdeftDefine = utl_alloc(sizeof(SEL_DEFINE_TREE));
  LEX_GET_TOKEN;
  if (tokToken.ucType != IDENTIFIER)
    lex_exit("Identifier expected");
  pdeftDefine->pcName = tokToken.pcString;
  pdeftDefine->pdeftLeft = pdeftDefine->pdeftRight = NULL;
  ptoklLast = NULL;
  for (LEX_GET_TOKEN; tokToken.ucType != NEWLINE; LEX_GET_TOKEN)
  {
    if (tokToken.ucType == BACKSLASH)
      for (LEX_GET_TOKEN; tokToken.ucType != NEWLINE; LEX_GET_TOKEN)
      {
        if (tokToken.ucType == IDENTIFIER ||
            tokToken.ucType == STRING_CONSTANT)
          utl_free(tokToken.pcString);
      }
    else   
    {
      if (tokToken.ucType == LEX_NONE)
        continue;
      if (ptoklLast)
      {
        ptoklLast->ptoklNext = utl_alloc(sizeof(SEL_TOKEN_LIST));
        memcpy(&(ptoklLast->ptoklNext->tokToken), &tokToken,
               sizeof(SEL_TOKEN));
        ptoklLast = ptoklLast->ptoklNext;
      }
      else
      {
        pdeftDefine->ptoklToken = ptoklLast =
          utl_alloc(sizeof(SEL_TOKEN_LIST));
        memcpy(&(ptoklLast->tokToken), &tokToken, sizeof(SEL_TOKEN));
      }
    }
  }
  ptoklLast->ptoklNext = NULL;    
  if (pdeftRoot)
  {
    for (pdeftNode = pdeftRoot; ; )
    {
      iCompResult = strcmp(pdeftDefine->pcName, pdeftNode->pcName);
      if (iCompResult < 0)
      {
        if (pdeftNode->pdeftLeft)
          pdeftNode = pdeftNode->pdeftLeft;
        else
        {
          pdeftNode->pdeftLeft = pdeftDefine;
          return;
        }
      }                     
      else
        if (iCompResult > 0)
        {
          if (pdeftNode->pdeftRight)
            pdeftNode = pdeftNode->pdeftRight;
          else
          {
            pdeftNode->pdeftRight = pdeftDefine;
            return;
          }
        }  
        else
          lex_exit("Symbol defined twice");
    }
  }
  pdeftRoot = pdeftDefine;
}       

SEL_TOKEN_LIST* lex_get_define (CHAR *pcName)
{
  
  SEL_DEFINE_TREE *pdeftNode;
  INT              iCompResult;

  pdeftNode = pdeftRoot;
  while (pdeftNode)
  {
    iCompResult = strcmp(pcName, pdeftNode->pcName);
    if (iCompResult < 0)
      pdeftNode = pdeftNode->pdeftLeft;
    else
      if (iCompResult > 0)
        pdeftNode = pdeftNode->pdeftRight;
      else
        return(pdeftNode->ptoklToken);
  }
  return(NULL);
}                

void lex_exit (CHAR *pcError)
{                  
  printf(pcError);
  exit(2);
}                          

UINT lex_get_symbol_table (ULONG *pulLength)
{ 
  
  SEL_LEX_SYMBOL *psymCounter;
  UINT            uiNumber    = 0;

  *pulLength = 0;             
  for (psymCounter = psymFirst; psymCounter;
       psymCounter = psymCounter->psymNext)
  {
    uiNumber++;
    *pulLength += strlen(psymCounter->pcName) + 1;
  }
  return(uiNumber);
}                  

void lex_write_symbol_table (void)
{
  
  SEL_LEX_SYMBOL *psymCounter;
  
  for (psymCounter = psymFirst; psymCounter;
       psymCounter = psymCounter->psymNext)
    fwrite(psymCounter->pcName, strlen(psymCounter->pcName) + 1, 1, pfileOut);
}                           

void lex_write_queue (CNT_QUEUE *pqQueue)
{
  
  ULONG ulValue;
  
  while (!cnt_queue_is_empty(pqQueue))
  {
    ulValue = cnt_queue_get(pqQueue);
    fwrite(&ulValue, sizeof(ULONG), 1, pfileOut);
  }
}

void lex_write_output (void)
{
  
  UCHAR  ucVersion      = SEL_FILE_FORMAT;
  UINT   uiSymbols;
  INT    iLength;
  ULONG  ulSymbolLength,
         ulLength;
  UCHAR *pucBuffer;

  fwrite(&ucVersion, sizeof(UCHAR), 1, pfileOut);
  uiSymbols = lex_get_symbol_table(&ulSymbolLength);
  fwrite(&uiSymbols, sizeof(UINT), 1, pfileOut);
  fwrite(&ulSymbolLength, sizeof(ULONG), 1, pfileOut);
  lex_write_symbol_table();
  fwrite(&uiLocalStructs, sizeof(UINT), 1, pfileOut);
  fwrite(&uiGlobalStructs, sizeof(UINT), 1, pfileOut);
  fwrite(&uiLocals, sizeof(UINT), 1, pfileOut);
  lex_write_queue(pqLocals);
  fwrite(&uiGlobals, sizeof(UINT), 1, pfileOut);
  lex_write_queue(pqGlobals);
  fwrite(&uiFuncs, sizeof(UINT), 1, pfileOut);
  lex_write_queue(pqFuncs);
  fwrite(&uiLine, sizeof(UINT), 1, pfileOut);
  fwrite(pucLines, uiLine, 1, pfileOut);
  ulLength = ftell(pfileTemp);
  fwrite(&ulLength, sizeof(ULONG), 1, pfileOut);
  pucBuffer = utl_alloc(8192);
  fseek(pfileTemp, 0, SEEK_SET);
  do
  {
    iLength = fread(pucBuffer, 1, 8192, pfileTemp);
    if (iLength > 0)
      fwrite(pucBuffer, 1, iLength, pfileOut);
  } while (iLength == 8192);  
  utl_free(pucBuffer);
}                                   
 
void lex_delete_token_list (SEL_TOKEN_LIST *ptoklNode)
{                     
  if (ptoklNode->tokToken.ucType == STRING_CONSTANT ||
      ptoklNode->tokToken.ucType == IDENTIFIER)
    utl_free(ptoklNode->tokToken.pcString);
  if (ptoklNode->ptoklNext)
    lex_delete_token_list(ptoklNode->ptoklNext);
  utl_free(ptoklNode);
}

void lex_delete_define_tree (SEL_DEFINE_TREE *pdeftNode)
{ 
  if (pdeftNode->pdeftLeft)
    lex_delete_define_tree(pdeftNode->pdeftLeft);
  if (pdeftNode->pdeftRight)
    lex_delete_define_tree(pdeftNode->pdeftRight);
  utl_free(pdeftNode->pcName);
  lex_delete_token_list(pdeftNode->ptoklToken);
  utl_free(pdeftNode);
}

void lex_delete_symbol_tree (SEL_SYMBOL_TREE *psymtNode)
{
  if (psymtNode->psymtLeft)
    lex_delete_symbol_tree(psymtNode->psymtLeft);
  if (psymtNode->psymtRight)
    lex_delete_symbol_tree(psymtNode->psymtRight);
  utl_free(psymtNode->psymSymbol->pcName);
  utl_free(psymtNode->psymSymbol);
  utl_free(psymtNode);
}

void lex_tokenize_source (CHAR *pcSourceName, CHAR *pcDestName)
{       
  
  UCHAR ucFinished = FALSE;
                  
  uiLine = uiFuncs = uiLocals = uiGlobals =
    uiLocalStructs = uiGlobalStructs = ulLastLine = 0;
  uiLineBufLength = 1024;
  psymtRoot = pdeftRoot = psymFirst = psymLast = NULL;
  psNext = cnt_stack_make();
  pqFuncs = cnt_queue_make();
  pqLocals = cnt_queue_make();
  pqGlobals = cnt_queue_make();   
  pucLines = utl_alloc(uiLineBufLength);
#ifndef _UNIX
  _fmode = O_BINARY;
#endif
  aplbufStack[uiIncludeLevel = 0] = lex_create_buffer(pcSourceName);
  pfileTemp = fopen("temp.lex", "w+");
  do
  {
    while (LEX_GET_TOKEN)
    {          
      switch (tokToken.ucType)
      {
        case LEX_NONE :
          break;
        case DEFINE :
          lex_add_define();
          break;
        case INCLUDE :
          lex_include();
          break;
        default :  
          lex_put_token(&tokToken);
          if (tokToken.ucType == IDENTIFIER ||
              tokToken.ucType == STRING_CONSTANT)
            utl_free(tokToken.pcString);
          break;
      }
    }
    lex_delete_buffer(LEX_INPUT);
    if (uiIncludeLevel)
      uiIncludeLevel--;   
    else
      ucFinished = TRUE;
  } while (!ucFinished);
  pfileOut = fopen(pcDestName, "w");
  lex_write_output();               
  if (pdeftRoot)
    lex_delete_define_tree(pdeftRoot);
  if (psymtRoot)
    lex_delete_symbol_tree(psymtRoot);
  fclose(pfileTemp);
  fclose(pfileOut);
  utl_free(pucLines);
  cnt_queue_destroy(pqGlobals);
  cnt_queue_destroy(pqLocals);
  cnt_queue_destroy(pqFuncs);
  cnt_stack_destroy(psNext);
}  
  
/*         
int main (int argc, char *argv[])
{                    
  
  CHAR acBuffer[12];

  printf("SEL-Interpreter Tokenizer V 0.2\n");
  if (argc != 3)
  {
    printf("\n"
           "Syntax:\n"
           "  sellex <infile> <outfile>\n"
           "\n"
           "  infile  ... SEL-source-code\n"
           "  outfile ... tokenized code\n"
           "\n"
           "Example:\n"
           "  sellex sort.sel sort.so\n"
           "\n"
           "Exit-code:\n"
           "  0 ... everything OK\n"
           "  1 ... invalid Parameters\n"
           "  2 ... error in source-code\n");
    return(1);
  }
  printf("%s\n", ltoa(coreleft(), acBuffer, 10));
  lex_tokenize_source(argv[1], argv[2]);
  if (heapcheck() == _HEAPCORRUPT)
    printf("Heap is corrupt!\n");
  printf("%s\n", ltoa(coreleft(), acBuffer, 10));
  psNext = cnt_stack_make();
  pqFuncs = cnt_queue_make();
  pqLocals = cnt_queue_make();
  pqGlobals = cnt_queue_make();   
  pucLines = utl_alloc(uiLineBufLength);
  _fmode = O_BINARY;             
  pfileIn = fopen(argv[1], "r");
  yyin = pfileIn;
  pfileTemp = fopen("temp.lex", "w+");
  while (LEX_GET_TOKEN)
  {
    lex_put_token(&tokToken);
    if (tokToken.ucType == IDENTIFIER ||
        tokToken.ucType == STRING_CONSTANT)
      utl_free(tokToken.pcString);
  }
  fclose(pfileIn);
  pfileOut = fopen(argv[2], "w");
  lex_write_output();
  fclose(pfileTemp);
  fclose(pfileOut);
  return(0);
}
*/
