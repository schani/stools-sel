/***************************************************************************
 ***************************************************************************
 ***                                                                     ***
 ***                      Schani-Tools fÅr C                             ***
 ***                                                                     ***
 ***               Headerdatei fÅr Soundblaster-Teil                     ***
 ***                                                                     ***
 ***               (c) 1990-93 by Schani Electronics                     ***
 ***                                                                     ***
 ***************************************************************************
 ***************************************************************************/

/***************************************************************************
 *                       Funktionsprototypen                               *
 ***************************************************************************/

void  sbl_init            (void);
void  sbl_done            (void);
UCHAR sbl_init_driver     (void);
void  sbl_remove_driver   (void);
void  sbl_speaker         (UCHAR);
void* sbl_load_voc        (CHAR*);
void  sbl_start_voc       (void*);
void  sbl_abort_voc       (void);
void  sbl_stop_voc        (void);
void  sbl_continue_voc    (void);
void  sbl_stop_loop       (UCHAR);
void  sbl_set_status_word (void);
