/***************************************************************************
 ***************************************************************************
 ***                                                                     ***
 ***                       Schani-Tools fÅr C                            ***
 ***                                                                     ***
 ***                 Headerdatei fÅr Container Queue                     ***
 ***                                                                     ***
 ***                (c) 1990-93 by Schani Electronics                    ***
 ***                                                                     ***
 ***************************************************************************
 ***************************************************************************/

/***************************************************************************
 *                          Datenstrukturen                                *
 ***************************************************************************/

typedef struct
{
  CNT_NODE *pnHead;
  CNT_NODE *pnTail;
} CNT_QUEUE;

/***************************************************************************
 *                        Funktionsprototypen                              *
 ***************************************************************************/

#ifdef __cplusplus
extern "C" 
{
#endif
CNT_QUEUE* cnt_queue_make     (void);
void       cnt_queue_put      (CNT_QUEUE*, ULONG);
ULONG      cnt_queue_get      (CNT_QUEUE*);
BOOL       cnt_queue_is_empty (CNT_QUEUE*);
void       cnt_queue_destroy  (CNT_QUEUE*);
#ifdef __cplusplus
}
#endif
