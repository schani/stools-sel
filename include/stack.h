/***************************************************************************
 ***************************************************************************
 ***                                                                     ***
 ***                       Schani-Tools fÅr C                            ***
 ***                                                                     ***
 ***                 Headerdatei fÅr Container Stack                     ***
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
} CNT_STACK;

/***************************************************************************
 *                        Funktionsprototypen                              *
 ***************************************************************************/

#ifdef __cplusplus
extern "C" 
{
#endif
CNT_STACK* cnt_stack_make      (void);
void       cnt_stack_put       (CNT_STACK*, ULONG);
ULONG      cnt_stack_get       (CNT_STACK*);
ULONG      cnt_stack_look_back (CNT_STACK*, INT);
BOOL       cnt_stack_is_empty  (CNT_STACK*);
void       cnt_stack_destroy   (CNT_STACK*);
#ifdef __cplusplus
}
#endif
