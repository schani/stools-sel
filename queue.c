/***************************************************************************
 ***************************************************************************
 ***                                                                     ***
 ***                       Schani-Tools fÅr C                            ***
 ***                                                                     ***
 ***                        Container Queue                              ***
 ***                                                                     ***
 ***                (c) 1990-93 by Schani Electronics                    ***
 ***                                                                     ***
 ***************************************************************************
 ***************************************************************************/

#include <gemein.h>
#include <utility.h>
#include <contain.h>

CNT_QUEUE* cnt_queue_make (void)
{
  CNT_QUEUE *pqReturnVar;

  pqReturnVar = utl_alloc(sizeof(CNT_QUEUE));
  pqReturnVar->pnHead = pqReturnVar->pnTail = NULL;
  return pqReturnVar;
}

void cnt_queue_put (CNT_QUEUE *pqQueue, ULONG ulKey)
{
  CNT_NODE *pnNode;

  pnNode = utl_alloc(sizeof(CNT_NODE));
  pnNode->ulKey = ulKey;
  if (pqQueue->pnHead == NULL)
    pqQueue->pnHead = pqQueue->pnTail = pnNode;
  pqQueue->pnHead->pnNext = pnNode;
  pqQueue->pnHead = pnNode;
  pnNode->pnNext = NULL;
}

ULONG cnt_queue_get (CNT_QUEUE *pqQueue)
{
  ULONG     ulReturnVar;
  CNT_NODE *pnNode;

  if (pqQueue->pnHead == NULL)
    return 0;
  ulReturnVar = pqQueue->pnTail->ulKey;
  if (pqQueue->pnHead == pqQueue->pnTail)
  {
    utl_free(pqQueue->pnHead);
    pqQueue->pnHead = pqQueue->pnTail = NULL;
  }
  else
  {
    pnNode = pqQueue->pnTail->pnNext;
    utl_free(pqQueue->pnTail);
    pqQueue->pnTail = pnNode;
  }
  return ulReturnVar;
}

BOOL cnt_queue_is_empty (CNT_QUEUE *pqQueue)
{
  return pqQueue->pnHead == NULL;
}

void cnt_queue_destroy (CNT_QUEUE *pqQueue)
{
  while (!cnt_queue_is_empty(pqQueue))
    cnt_queue_get(pqQueue);
  utl_free(pqQueue->pnTail);
  utl_free(pqQueue);
}
