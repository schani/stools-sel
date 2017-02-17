/***************************************************************************
 ***************************************************************************
 ***                                                                     ***
 ***                       Schani-Tools fÅr C                            ***
 ***                                                                     ***
 ***                 Headerdatei fÅr globale Variablen                   ***
 ***                                                                     ***
 ***                (c) 1990-93 by Schani Electronics                    ***
 ***                                                                     ***
 ***************************************************************************
 ***************************************************************************/

typedef struct _GLB_PROGRAM
{
  void     (*handle_event)(struct _GLB_PROGRAM*, UTL_EVENT*);
  WINDOW     winFirst;
  WINDOW     winLast;
  WINDOW     winFocussed;
  UTL_EVENT  eventEvent;
  CHAR      *pcPalette;
} GLB_PROGRAM;

typedef struct _GLB_MSG_OBJECT
{
  void (*handle_event)(struct _GLB_MSG_OBJECT*, UTL_EVENT*);
} GLB_MSG_OBJECT;

/***************************************************************************
 *                             Makros                                      *
 ***************************************************************************/

#define glb_send_message(f,t,m,i)    int_glb_send_message((GLB_MSG_OBJECT*)(f),(GLB_MSG_OBJECT*)(t),m,(ULONG)i)

/***************************************************************************
 *                       Funktionsprototypen                               *
 ***************************************************************************/

#ifdef __cplusplus
extern "C"
{
#endif
BOOL  int_glb_send_message (GLB_MSG_OBJECT*, GLB_MSG_OBJECT*, UINT, ULONG);
void  glb_update_windows   (void);
void  glb_set_palette      (UCHAR);
void  glb_execute          (WINDOW);
void  glb_pre_process      (UTL_EVENT*);
void  glb_run              (void);
void  glb_done             (void);
void  glb_init             (void(*)(GLB_PROGRAM*, UTL_EVENT*));
#ifdef __cplusplus
}
#endif
