/***************************************************************************
 ***************************************************************************
 ***                                                                     ***
 ***                      Schani-Tools fÅr C                             ***
 ***                                                                     ***
 ***                  Headerdatei fÅr Buffer-Teil                        ***
 ***                                                                     ***
 ***               (c) 1990-93 by Schani Electronics                     ***
 ***                                                                     ***
 ***************************************************************************
 ***************************************************************************/

/***************************************************************************
 *                     Allgemeine Datenstrukturen                          *
 ***************************************************************************/
                      
#include <stdio.h>

typedef struct
{
  CHAR  *pcBuffer;
  UCHAR  ucDiscardable;
  ULONG  ulVirtLength;
  ULONG  ulPhysLength;
  ULONG  ulPos;
} BUF_MEM;

typedef struct
{
  UINT     uiType;
  BUF_MEM *pbmMemBuffer;
  FILE    *pfileFileBuffer;
} BUFFER;                     

#define BUF_MEMORY_BUFFER              1
#define BUF_FILE_BUFFER                2

#define BUF_MB                         (pbufBuffer->pbmMemBuffer)
#define BUF_FB                         (pbufBuffer->pfileFileBuffer)
#define BUF_SWITCH                     switch(pbufBuffer->uiType)

#define BUF_MEMORY_INIT_SIZE        1024
#define BUF_MEMORY_EXPAND_SIZE      1024

/***************************************************************************
 *                       Funktionsprototypen                               *
 ***************************************************************************/

#ifdef __cplusplus
extern "C"
{
#endif
BUFFER* buf_open_mem  (CHAR*, ULONG, ULONG);
BUFFER* buf_open_file (FILE*, CHAR*, CHAR*);
void    buf_close     (BUFFER*);
UCHAR   buf_eof       (BUFFER*);
ULONG   buf_write     (CHAR*, ULONG, ULONG, BUFFER*);
ULONG   buf_read      (CHAR*, ULONG, ULONG, BUFFER*);
INT     buf_putc      (CHAR, BUFFER*);
INT     buf_getc      (BUFFER*);
UCHAR   buf_flush     (BUFFER*);
UCHAR   buf_set_pos   (BUFFER*, ULONG);
UCHAR   buf_get_pos   (BUFFER*, ULONG*);
UCHAR   buf_seek      (BUFFER*, LONG, INT);
ULONG   buf_tell      (BUFFER*);
void    buf_rewind    (BUFFER*);
#ifdef __cplusplus
}
#endif
