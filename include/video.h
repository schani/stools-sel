/***************************************************************************
 ***************************************************************************
 ***                                                                     ***
 ***                       Schani-Tools for C                            ***
 ***                                                                     ***
 ***                    Headerfile for Video-Part                        ***
 ***                                                                     ***
 ***               (c) 1990-94 by Schani Electronics                     ***
 ***                                                                     ***
 ***************************************************************************
 ***************************************************************************/

/***************************************************************************
 *                       Allgemeine Makros                                 *
 ***************************************************************************/

#define        vio_attri(ucV, ucH)       (UCHAR)(ucV + (ucH << 4))

typedef struct
{
	CHAR character;
	CHAR attribute;
    BYTE needsRedraw;
} CHARACTER;

typedef struct
{
  INT iX;
  INT iY;
} VIO_COORD;

typedef struct
{
  INT iX;
  INT iY;
  INT iWidth;
  INT iHeight;
} VIO_REGION;

/***************************************************************************
 *                      Funktionsprototypen                                *
 ***************************************************************************/

#ifdef __cplusplus
extern "C" 
{
#endif
void      vio_init            (void);
void      vio_redraw          (void);
void      vio_get_screen_size (INT*, INT*);
void      vio_set_page        (WORD);
WORD      vio_get_page        (void);
void      vio_set_vir_page    (CHARACTER far*);
void      vio_set_phys_page   (void);
void      vio_set_cursor_pos  (INT, INT);
void      vio_set_cursor_type (INT);
void      vio_get_cursor      (INT*, INT*, INT*);
void      vio_set_mode        (BYTE);
BYTE      vio_get_mode        (void);
void      vio_scroll_up       (INT, INT, INT, INT, CHAR, INT);
void      vio_scroll_down     (INT, INT, INT, INT, CHAR, INT);
CHAR      vio_l_a             (INT, INT);
CHAR      vio_l_z             (INT, INT);
CHARACTER vio_l_za            (INT, INT);
void      vio_s_a             (INT, INT, CHAR);
void      vio_s_z             (INT, INT, CHAR);
void      vio_s_za            (INT, INT, CHAR, CHAR);
void      vio_sp_a            (CHAR);
void      vio_sp_z            (CHAR);
void      vio_sp_za           (CHAR, CHAR);
void      vio_ss              (INT, INT, CHAR*);
void      vio_ss_a            (INT, INT, CHAR*, CHAR);
void      vio_sw_a            (INT, INT, INT, INT, CHAR);
void      vio_sw_z            (INT, INT, INT, INT, CHAR);
void      vio_sw_za           (INT, INT, INT, INT, CHAR, CHAR);
void      vio_lw              (INT, INT, INT, INT, CHARACTER*);
void      vio_sw              (INT, INT, INT, INT, CHARACTER*);
void      vio_sw_ff           (INT, INT, INT, INT, CHARACTER*);
BOOL      vio_color           (void);
#ifdef __cplusplus
}
#endif
