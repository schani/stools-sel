/***************************************************************************
 ***************************************************************************
 ***                                                                     ***
 ***                       Schani-Tools fÅr C                            ***
 ***                                                                     ***
 ***                        Container Stack                              ***
 ***                                                                     ***
 ***                (c) 1990-93 by Schani Electronics                    ***
 ***                                                                     ***
 ***************************************************************************
 ***************************************************************************/

#include <gemein.h>
#include <utility.h>
#include <contain.h>

CNT_STACK* cnt_stack_make (void)
{
  CNT_STACK *psReturnVar;

  psReturnVar = utl_alloc(sizeof(CNT_STACK));
  psReturnVar->pnHead = NULL;
  return psReturnVar;
}

void cnt_stack_put (CNT_STACK *psStack, ULONG ulKey)
{
  CNT_NODE *pnNode;

  pnNode = utl_alloc(sizeof(CNT_NODE));
  pnNode->ulKey = ulKey;
  if (psStack->pnHead == NULL)
  {
    psStack->pnHead = pnNode;
    pnNode->pnNext = NULL;
  }
  else
  {
    pnNode->pnNext = psStack->pnHead;
    psStack->pnHead = pnNode;
  }
}

ULONG cnt_stack_get (CNT_STACK *psStack)
{
  ULONG     ulReturnVar;
  CNT_NODE *pnNode;

  if (psStack->pnHead == NULL)
    return(0);
  ulReturnVar = psStack->pnHead->ulKey;
  pnNode = psStack->pnHead;
  psStack->pnHead = pnNode->pnNext;
  utl_free(pnNode);
  return ulReturnVar;
}

ULONG cnt_stack_look_back (CNT_STACK *psStack, INT iElements)
{
  CNT_NODE *pnCounter;
  INT       iCounter;
  
  for (pnCounter = psStack->pnHead, iCounter = 0;
       pnCounter && iCounter < iElements;
       pnCounter = pnCounter->pnNext, iCounter++)
    ;
  if (pnCounter)
    return pnCounter->ulKey;
  return 0;
}

BOOL cnt_stack_is_empty (CNT_STACK *psStack)
{
  return psStack->pnHead == NULL;
}

void cnt_stack_destroy (CNT_STACK *psStack)
{
  while (!cnt_stack_is_empty(psStack))
    cnt_stack_get(psStack);
  utl_free(psStack);
}
