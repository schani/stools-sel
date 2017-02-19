/***************************************************************************
 ***************************************************************************
 ***                                                                     ***
 ***                      Schani-Tools fÅr C                             ***
 ***                                                                     ***
 ***                   Headerdatei fÅr Hilfe-Teil                        ***
 ***                                                                     ***
 ***               (c) 1990-93 by Schani Electronics                     ***
 ***                                                                     ***
 ***************************************************************************
 ***************************************************************************/

/***************************************************************************
 *                            Konstanten                                   *
 ***************************************************************************/

#define COLOR_PLAIN                 0
#define COLOR_BOLD                  1
#define COLOR_ITALIC                2
#define COLOR_UNDERLINE             4
#define COLOR_BOLD_ITALIC           3
#define COLOR_BOLD_UNDERLINE        5
#define COLOR_ITALIC_UNDERLINE      6
#define COLOR_BOLD_ITALIC_UNDERLINE 7

/***************************************************************************
 *                          Datenstrukturen                                *
 ***************************************************************************/

typedef struct
{
  BYTE  byColor;
  SWORD swCount;
} HLP_COLOR;

typedef struct
{
  SWORD swX;
  SWORD swY;
  SWORD swWidth;
  SWORD swHeight;
  DWORD dwContext;
} HLP_LINK;

typedef struct
{
  SWORD       swFreeze;
  SWORD       swLines;
  HLP_COLOR **ppclrColors;
  DWORD       dwLinks;
  HLP_LINK   *plnkLinks;
  CHAR      **ppcText;
} HLP_CONTEXT;

/***************************************************************************
 *                        Funktionsprototypen                              *
 ***************************************************************************/

#ifdef __cplusplus
extern "C"
{
#endif
BOOL                hlp_open_file         (CHAR*);
void                hlp_close_file        (void);
BOOL                hlp_context_available (DWORD);
HLP_CONTEXT*        hlp_get_context       (DWORD);
void                hlp_release_context   (HLP_CONTEXT*);
void                hlp_set_context       (DWORD);
void                hlp_help_handler      (void);
#ifdef __cplusplus
}
#endif
