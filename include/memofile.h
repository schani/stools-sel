/***************************************************************************
 ***************************************************************************
 ***                                                                     ***
 ***                       Schani-Tools fÅr C                            ***
 ***                                                                     ***
 ***                    Headerdatei fÅr Memo-Teil                        ***
 ***                                                                     ***
 ***                (c) 1990-93 by Schani Electronics                    ***
 ***                                                                     ***
 ***************************************************************************
 ***************************************************************************/

/***************************************************************************
 *                      Interne Datenstrukturen                            *
 ***************************************************************************/

typedef struct
{
  UINT uiLength;
  UINT uiNext;
  CHAR acDummy[8];
} MEM_HEADER;

/***************************************************************************
 *                       Funktionsprototypen                               *
 ***************************************************************************/

#ifdef __cplusplus
extern "C"
{
#endif
void  static int_mem_set_usage     (FILE*, UINT, UCHAR);
UINT  static int_mem_get_next_free (FILE*);
void  static int_mem_read_header   (FILE*, UINT, MEM_HEADER*);
void  static int_mem_write_header  (FILE*, UINT, MEM_HEADER*);
FILE*        mem_open_file         (CHAR*);
CHAR*        mem_read              (FILE*, UINT);
void         mem_delete            (FILE*, UINT);
UINT         mem_write             (FILE*, CHAR*, UINT);
#ifdef __cplusplus
}
#endif
