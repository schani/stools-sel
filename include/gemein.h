/***************************************************************************
 ***************************************************************************
 ***                                                                     ***
 ***                       Schani-Tools fÅr C                            ***
 ***                                                                     ***
 ***                     Allgemeine Headerdatei                          ***
 ***                                                                     ***
 ***               (c) 1990-93 by Schani Electronics                     ***
 ***                                                                     ***
 ***************************************************************************
 ***************************************************************************/

#ifndef _STOOLS_COMMON
#define _STOOLS_COMMON

#include <stdint.h>

#define min(a,b)	((a)<(b)?(a):(b))
#define max(a,b)	((a)>(b)?(a):(b))

/***************************************************************************
 *                               Typen                                     *
 ***************************************************************************/

typedef        char             CHAR;
typedef        unsigned char    UCHAR;
typedef        int16_t          INT;
typedef        uint16_t         UINT;
typedef        int64_t          LONG;
typedef        uint64_t         ULONG;
typedef        int16_t          BOOL;

typedef        uint8_t          BYTE;
typedef        uint16_t         WORD;
typedef        uint32_t         DWORD;
typedef        int8_t           SBYTE;
typedef        int16_t          SWORD;
typedef        int32_t          SDWORD;

typedef        float            FLOAT;
typedef        double           DOUBLE;

#define far

/***************************************************************************
 *                           Farbpaletten                                  *
 ***************************************************************************/

#define PAL_WINDOWS                           7
#define PAL_COLORS                           22

#define PAL_COL_BACKGROUND                    0

#define PAL_COL_BORDER                        1
#define PAL_COL_DSK_MENU_BAR                  1

#define PAL_COL_TITLE                         2
#define PAL_COL_DSK_MENU_ACT_ITEM             2

#define PAL_COL_ICON                          3
#define PAL_COL_DSK_MENU_HOT_KEY              3

#define PAL_COL_ACT_BUT                       4
#define PAL_COL_DSK_MENU_SEL_ACT_ITEM         4
#define PAL_COL_WIN_ACT_ITEM                  4

#define PAL_COL_ACT_BUT_HOT_KEY               5
#define PAL_COL_DSK_MENU_SEL_HOT_KEY          5
#define PAL_COL_WIN_HOT_KEY                   5

#define PAL_COL_ACT_BUT_INVERS                6
#define PAL_COL_DSK_MENU_INACT_ITEM           6
#define PAL_COL_WIN_SEL_ACT_ITEM              6

#define PAL_COL_LABEL                         7
#define PAL_COL_DSK_MENU_SEL_INACT_ITEM       7
#define PAL_COL_WIN_SEL_HOT_KEY               7

#define PAL_COL_LABEL_HOT_KEY                 8
#define PAL_COL_DSK_STATUS_LINE               8
#define PAL_COL_WIN_INACT_ITEM                8

#define PAL_COL_LIST_BOX_ITEM                 9
#define PAL_COL_DSK_STATUS_ACT_ITEM           9
#define PAL_COL_WIN_SEL_INACT_ITEM            9

#define PAL_COL_LIST_BOX_SEL_ITEM            10
#define PAL_COL_DSK_STATUS_HOT_KEY           10

#define PAL_COL_SCROLL_BAR                   11
#define PAL_COL_DSK_STATUS_SEL_ACT_ITEM      11

#define PAL_COL_MARKED_TEXT                  12
#define PAL_COL_DSK_STATUS_SEL_HOT_KEY       12

#define PAL_COL_HELP                         13
#define PAL_COL_DSK_STATUS_INACT_ITEM        13

#define PAL_COL_HELP_BOLD                    14
#define PAL_COL_DSK_STATUS_SEL_INACT_ITEM    14

#define PAL_COL_HELP_ITALIC                  15

#define PAL_COL_HELP_UNDERLINE               16

#define PAL_COL_HELP_BOLD_ITALIC             17

#define PAL_COL_HELP_BOLD_UNDERLINE          18

#define PAL_COL_HELP_ITALIC_UNDERLINE        19

#define PAL_COL_HELP_BOLD_ITALIC_UNDERLINE   20

#define PAL_COL_HELP_SEL_HOT_SPOT            21

#define PAL_BLUE                              0
#define PAL_WHITE                             1
#define PAL_CYAN                              2
#define PAL_BLACK                             3
#define PAL_GREEN                             4
#define PAL_MONO                              5
#define PAL_USER_DEFINED                      6

#define PAL_WIN_DESKTOP                       0
#define PAL_WIN_MENU                          1
#define PAL_WIN_DLG                           2
#define PAL_WIN_HELP                          3
#define PAL_WIN_INFO                          4
#define PAL_WIN_WARNING                       5
#define PAL_WIN_ERROR                         6

#define glb_get_color(w,c)                    *(prgProgram.pcPalette +    \
                                              w * PAL_COLORS + c)

/***************************************************************************
 *                              Boolean                                    *
 ***************************************************************************/

#define        FALSE              0
#define        TRUE               1
#define        FLIP               2
#define        QUERY              3

#ifndef NULL
#define        NULL               0L
#endif

/***************************************************************************
 *                               Tasten                                    *
 ***************************************************************************/

/* DOS keycodes */
#define EXT_CODE        0x100

#define K_F1    ( 59 |  EXT_CODE)
#define K_F2    ( 60 |  EXT_CODE)
#define K_F3    ( 61 |  EXT_CODE)
#define K_F4    ( 62 |  EXT_CODE)
#define K_F5    ( 63 |  EXT_CODE)
#define K_F6    ( 64 |  EXT_CODE)
#define K_F7    ( 65 |  EXT_CODE)
#define K_F8    ( 66 |  EXT_CODE)
#define K_F9    ( 67 |  EXT_CODE)
#define K_F10   ( 68 |  EXT_CODE)

#define K_S_F1   ( 84 | EXT_CODE)
#define K_S_F2   ( 85 | EXT_CODE)
#define K_S_F3   ( 86 | EXT_CODE)
#define K_S_F4   ( 87 | EXT_CODE)
#define K_S_F5   ( 88 | EXT_CODE)
#define K_S_F6   ( 89 | EXT_CODE)
#define K_S_F7   ( 90 | EXT_CODE)
#define K_S_F8   ( 91 | EXT_CODE)
#define K_S_F9   ( 92 | EXT_CODE)
#define K_S_F10  ( 93 | EXT_CODE)

#define K_C_F1   ( 94 | EXT_CODE)
#define K_C_F2   ( 95 | EXT_CODE)
#define K_C_F3   ( 96 | EXT_CODE)
#define K_C_F4   ( 97 | EXT_CODE)
#define K_C_F5   ( 98 | EXT_CODE)
#define K_C_F6   ( 99 | EXT_CODE)
#define K_C_F7   (100 | EXT_CODE)
#define K_C_F8   (101 | EXT_CODE)
#define K_C_F9   (102 | EXT_CODE)
#define K_C_F10  (103 | EXT_CODE)

#define K_A_F1   (104 | EXT_CODE)
#define K_A_F2   (105 | EXT_CODE)
#define K_A_F3   (106 | EXT_CODE)
#define K_A_F4   (107 | EXT_CODE)
#define K_A_F5   (108 | EXT_CODE)
#define K_A_F6   (109 | EXT_CODE)
#define K_A_F7   (110 | EXT_CODE)
#define K_A_F8   (111 | EXT_CODE)
#define K_A_F9   (112 | EXT_CODE)
#define K_A_F10  (113 | EXT_CODE)

#define K_HOME     (71 | EXT_CODE)
#define K_END      (79 | EXT_CODE)
#define K_PGUP     (73 | EXT_CODE)
#define K_PGDN     (81 | EXT_CODE)
#define K_LEFT     (75 | EXT_CODE)
#define K_RIGHT    (77 | EXT_CODE)
#define K_UP       (72 | EXT_CODE)
#define K_DOWN     (80 | EXT_CODE)
#define K_INS      (82 | EXT_CODE)
#define K_DEL      (83 | EXT_CODE)

#define K_C_HOME  (119 | EXT_CODE)
#define K_C_END   (117 | EXT_CODE)
#define K_C_PGUP  (132 | EXT_CODE)
#define K_C_PGDN  (118 | EXT_CODE)
#define K_C_LEFT  (115 | EXT_CODE)
#define K_C_RIGHT (116 | EXT_CODE)

#define K_C_A         1
#define K_C_B         2
#define K_C_C         3
#define K_C_D         4
#define K_C_E         5
#define K_C_F         6
#define K_C_G         7
#define K_C_H         8
#define K_C_I         9
#define K_C_J        10
#define K_C_K        11
#define K_C_L        12
#define K_C_M        13
#define K_C_N        14
#define K_C_O        15
#define K_C_P        16
#define K_C_Q        17
#define K_C_R        18
#define K_C_S        19
#define K_C_T        20
#define K_C_U        21
#define K_C_V        22
#define K_C_W        23
#define K_C_X        24
#define K_C_Y        25
#define K_C_Z        26

#define K_A_A        (30 | EXT_CODE)
#define K_A_B        (48 | EXT_CODE)
#define K_A_C        (46 | EXT_CODE)
#define K_A_D        (32 | EXT_CODE)
#define K_A_E        (18 | EXT_CODE)
#define K_A_F        (33 | EXT_CODE)
#define K_A_G        (34 | EXT_CODE)
#define K_A_H        (35 | EXT_CODE)
#define K_A_I        (23 | EXT_CODE)
#define K_A_J        (36 | EXT_CODE)
#define K_A_K        (37 | EXT_CODE)
#define K_A_L        (38 | EXT_CODE)
#define K_A_M        (50 | EXT_CODE)
#define K_A_N        (49 | EXT_CODE)
#define K_A_O        (24 | EXT_CODE)
#define K_A_P        (25 | EXT_CODE)
#define K_A_Q        (16 | EXT_CODE)
#define K_A_R        (19 | EXT_CODE)
#define K_A_S        (31 | EXT_CODE)
#define K_A_T        (20 | EXT_CODE)
#define K_A_U        (22 | EXT_CODE)
#define K_A_V        (47 | EXT_CODE)
#define K_A_W        (17 | EXT_CODE)
#define K_A_X        (45 | EXT_CODE)
#define K_A_Y        (21 | EXT_CODE)
#define K_A_Z        (44 | EXT_CODE)

#define K_A_1         (120 | EXT_CODE)
#define K_A_2         (121 | EXT_CODE)
#define K_A_3         (122 | EXT_CODE)
#define K_A_4         (123 | EXT_CODE)
#define K_A_5         (124 | EXT_CODE)
#define K_A_6         (125 | EXT_CODE)
#define K_A_7         (126 | EXT_CODE)
#define K_A_8         (127 | EXT_CODE)
#define K_A_9         (128 | EXT_CODE)
#define K_A_0         (129 | EXT_CODE)
#define K_A_DASH      (130 | EXT_CODE)
#define K_A_EQUAL     (131 | EXT_CODE)

#define K_ESC           27
#define K_SPACE         32
#define K_TAB           K_C_I
#define K_SHIFT_TAB     (15 | EXT_CODE)
#define K_BACKSPACE     K_C_H
#define K_C_BACKSPACE   127
#define K_C_PTRSC       (114 | EXT_CODE)
#define K_ENTER         13
#define K_S_ENTER       13
#define K_C_ENTER       10

/***************************************************************************
 *                      Konstanten fÅr  Rahmentypen                        *
 ***************************************************************************/

#define B_EEEE           0
#define B_EEED           1
#define B_EEDE           2
#define B_EEDD           3
#define B_EDEE           4
#define B_EDED           5
#define B_EDDE           6
#define B_EDDD           7
#define B_DEEE           8
#define B_DEED           9
#define B_DEDE          10
#define B_DEDD          11
#define B_DDEE          12
#define B_DDED          13
#define B_DDDE          14
#define B_DDDD          15
#define B_000P          16
#define B_025P          17
#define B_050P          18
#define B_075P          19
#define B_100P          20

/***************************************************************************
 *                           Error Trapping                                *
 ***************************************************************************/
 
#define LOGFILE(n)                   FILE *pfileLog;                        \
                                     pfileLog = fopen(n, "w");              \
                                     fprintf(pfileLog,"Starting Log:\n")
#define WRITE_LOG                    if (heapcheck() == _HEAPCORRUPT)       \
                                     fprintf(pfileLog,"Error: %s Line %d\n",\
                                             __FILE__, __LINE__)
#define CLOSE_LOG                    fclose(pfileLog)

#endif
