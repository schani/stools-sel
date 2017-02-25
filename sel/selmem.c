/***************************************************************************
 ***************************************************************************
 ***                                                                     ***
 ***             Schani Electronics Language - Interpreter               ***
 ***                                                                     ***
 ***                        Memory Management                            ***
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
                             
void mem_init (SEL_PROGRAM *pprogProgram)
{
  pprogProgram->mbMemBlocks.uiBlockTop =
    pprogProgram->mbMemBlocks.uiStackTop = 0;
  pprogProgram->mbMemBlocks.ppBlocks =
    utl_calloc(sizeof(void*), SEL_BLOCK_NUMBER); 
  pprogProgram->mbMemBlocks.ppsStacks =
    utl_calloc(sizeof(CNT_STACK*), SEL_STACK_NUMBER);
}                      

void mem_collect_blocks (SEL_MEM_BLOCKS *pmbBlocks)
{
  
  UINT uiDest,
       uiSource;
       
  for (uiDest = 0, uiSource = 0; uiSource < pmbBlocks->uiBlockTop; uiSource++)
    if (pmbBlocks->ppBlocks[uiSource])
      pmbBlocks->ppBlocks[uiDest++] = pmbBlocks->ppBlocks[uiSource];
  pmbBlocks->uiBlockTop = uiDest;
}

void mem_collect_stacks (SEL_MEM_BLOCKS *pmbBlocks)
{
  
  UINT uiDest,
       uiSource;
       
  for (uiDest = 0, uiSource = 0; uiSource < pmbBlocks->uiStackTop; uiSource++)
    if (pmbBlocks->ppsStacks[uiSource])
      pmbBlocks->ppsStacks[uiDest++] = pmbBlocks->ppsStacks[uiSource];
  pmbBlocks->uiStackTop = uiDest;
}

void* mem_alloc (SEL_PROGRAM *pprogProgram, UINT uiSize)
{
  
  void *pReturnVar;
  
  if (pprogProgram->mbMemBlocks.uiBlockTop == SEL_BLOCK_NUMBER)
    man_error(pprogProgram, 20, NULL);
  pReturnVar = utl_alloc(uiSize);   
  pprogProgram->mbMemBlocks.ppBlocks[pprogProgram->mbMemBlocks.uiBlockTop++] =
    pReturnVar;
  if (pprogProgram->mbMemBlocks.uiBlockTop == SEL_BLOCK_NUMBER)
    mem_collect_blocks(&(pprogProgram->mbMemBlocks));
  return(pReturnVar);
}

void* mem_calloc (SEL_PROGRAM *pprogProgram, UINT uiSize, UINT uiNumber)
{
  
  void *pReturnVar;
  
  if (pprogProgram->mbMemBlocks.uiBlockTop == SEL_BLOCK_NUMBER)
    man_error(pprogProgram, 20, NULL);
  pReturnVar = utl_calloc(uiSize, uiNumber);
  pprogProgram->mbMemBlocks.ppBlocks[pprogProgram->mbMemBlocks.uiBlockTop++] =
    pReturnVar;
  if (pprogProgram->mbMemBlocks.uiBlockTop == SEL_BLOCK_NUMBER)
    mem_collect_blocks(&(pprogProgram->mbMemBlocks));
  return(pReturnVar);
}

void* mem_realloc (SEL_PROGRAM *pprogProgram, void *pBlock, UINT uiNewSize)
{     
  
  UINT uiIndex;

  for (uiIndex = pprogProgram->mbMemBlocks.uiBlockTop - 1;
       uiIndex < pprogProgram->mbMemBlocks.uiBlockTop; uiIndex--)
    if (pprogProgram->mbMemBlocks.ppBlocks[uiIndex] == pBlock)
    {
      pprogProgram->mbMemBlocks.ppBlocks[uiIndex] =
        utl_realloc(pBlock, uiNewSize);
      break;
    }
  return(pprogProgram->mbMemBlocks.ppBlocks[uiIndex]);
}

void mem_free (SEL_PROGRAM *pprogProgram, void *pBlock)
{     
  
  UINT uiIndex;

  for (uiIndex = pprogProgram->mbMemBlocks.uiBlockTop - 1;
       uiIndex < pprogProgram->mbMemBlocks.uiBlockTop; uiIndex--)
    if (pprogProgram->mbMemBlocks.ppBlocks[uiIndex] == pBlock)
    {        
      utl_free(pBlock);
      pprogProgram->mbMemBlocks.ppBlocks[uiIndex] = NULL;
      if (uiIndex == pprogProgram->mbMemBlocks.uiBlockTop - 1)
        pprogProgram->mbMemBlocks.uiBlockTop--;
      return;
    }
}

void mem_register_stack (SEL_PROGRAM *pprogProgram, CNT_STACK *pStack)
{
  if (pprogProgram->mbMemBlocks.uiStackTop == SEL_STACK_NUMBER)
    man_error(pprogProgram, 21, NULL);
  pprogProgram->mbMemBlocks.ppsStacks[pprogProgram->mbMemBlocks.uiStackTop++] =
    pStack;
  if (pprogProgram->mbMemBlocks.uiStackTop == SEL_STACK_NUMBER)
    mem_collect_stacks(&(pprogProgram->mbMemBlocks));
}

void mem_unregister_stack (SEL_PROGRAM *pprogProgram, CNT_STACK *psStack)
{     
  
  UINT uiIndex;

  for (uiIndex = pprogProgram->mbMemBlocks.uiStackTop - 1;
       uiIndex < pprogProgram->mbMemBlocks.uiStackTop; uiIndex--)
    if (pprogProgram->mbMemBlocks.ppsStacks[uiIndex] == psStack)
    {        
      pprogProgram->mbMemBlocks.ppsStacks[uiIndex] = NULL;
      if (uiIndex == pprogProgram->mbMemBlocks.uiStackTop - 1)
        pprogProgram->mbMemBlocks.uiStackTop--;
      return;
    }
}

void mem_delete_all (SEL_PROGRAM *pprogProgram)
{
  
  UINT uiIndex;
  
  for (uiIndex = 0; uiIndex < pprogProgram->mbMemBlocks.uiBlockTop; uiIndex++)
    if (pprogProgram->mbMemBlocks.ppBlocks[uiIndex])
      utl_free(pprogProgram->mbMemBlocks.ppBlocks[uiIndex]);
  for (uiIndex = 0; uiIndex < pprogProgram->mbMemBlocks.uiStackTop; uiIndex++)
    if (pprogProgram->mbMemBlocks.ppsStacks[uiIndex])
      cnt_stack_destroy(pprogProgram->mbMemBlocks.ppsStacks[uiIndex]);
  utl_free(pprogProgram->mbMemBlocks.ppBlocks);
  utl_free(pprogProgram->mbMemBlocks.ppsStacks);
}
