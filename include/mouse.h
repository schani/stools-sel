/***************************************************************************
 ***************************************************************************
 ***                                                                     ***
 ***                       Schani-Tools fÅr C                            ***
 ***                                                                     ***
 ***                    Headerdatei fÅr Maus-Teil                        ***
 ***                                                                     ***
 ***               (c) 1990-93 by Schani Electronics                     ***
 ***                                                                     ***
 ***************************************************************************
 ***************************************************************************/

/***************************************************************************
 *                       Konstanten fÅr Maustasten                         *
 ***************************************************************************/

#define MSM_B_LEFT                     1
#define MSM_B_RIGHT                    2
#define MSM_B_MIDDLE                   4

/***************************************************************************
 *                       Konstanten fÅr Maustypen                          *
 ***************************************************************************/

#define MSM_M_BUS                      1
#define MSM_M_SERIAL                   2
#define MSM_M_INPORT                   3
#define MSM_M_PS2                      4
#define MSM_M_HP                       5

/***************************************************************************
 *    Makros fÅr die vereinfachte Ein- und Ausblendung des Mauscursors     *
 ***************************************************************************/

#if defined(_MSDOS) || defined(_OS2)
#define MOUSE                          BOOL  m1 = FALSE;   \
                                       INT   m2,           \
                                             m3;           \
                                       WORD  m4
#define BEGIN_MOUSE                    m1 = msm_cursor_off()
#define BEGIN_MOUSE_IN(w)              msm_get_buttons(&m4,&m2,&m3); \
                                       if (IN_WINDOW(m2,m3,w) &&     \
                                           win_visible(w))           \
                                         m1 = msm_cursor_off()
#define BEGIN_MOUSE_ON(w)              msm_get_buttons(&m4,&m2,&m3); \
                                       if (ON_WINDOW(m2,m3,w) &&     \
                                           win_visible(w))           \
                                         m1 = msm_cursor_off()
#define END_MOUSE                      if (m1)                       \
                                         msm_cursor_on();
#endif
#if defined(_WINNT) || defined(_UNIX)
#define MOUSE                          struct __dummy_struct { int i; }
#define BEGIN_MOUSE
#define BEGIN_MOUSE_IN(w)
#define BEGIN_MOUSE_ON(w)
#define END_MOUSE
#endif

/***************************************************************************
 *                        Funktionsprototypen                              *
 ***************************************************************************/

#ifdef __cplusplus
extern "C" 
{
#endif
BOOL         msm_init            (void);
BOOL         msm_cursor_on       (void);
BOOL         msm_cursor_off      (void);
BOOL         msm_cursor          (void);
void         msm_get_buttons     (WORD*, INT*, INT*);
void         msm_set_position    (INT, INT);
void         msm_button_press    (WORD, WORD*, WORD*, INT*, INT*);
void         msm_button_release  (WORD, WORD*, WORD*, INT*, INT*);
void         msm_set_hor         (INT, INT);
void         msm_set_ver         (INT, INT);
void         msm_set_soft_cursor (WORD, WORD);
void         msm_set_hard_cursor (INT, INT);
void         msm_move            (INT*, INT*);
void         msm_lightpen_on     (void);
void         msm_lightpen_off    (void);
void         msm_set_mickeys     (WORD, WORD);
void         msm_get_mickeys     (WORD*, WORD*);
void         msm_set_threshold   (WORD);
WORD         msm_get_threshold   (void);
void         msm_set_page        (WORD);
WORD         msm_get_page        (void);
BYTE         msm_get_version     (BYTE*, BYTE*);
#ifdef __cplusplus
}
#endif
