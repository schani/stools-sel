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

/***************************************************************************
 *                               Typen                                     *
 ***************************************************************************/

#if !defined(_WINDEF_) && !defined(__OS2DEF__)
typedef        char             CHAR;
typedef        unsigned char    UCHAR;
typedef        int              INT;
typedef        unsigned int     UINT;
typedef        long             LONG;
typedef        unsigned long    ULONG;
typedef        int              BOOL;
#endif

#if defined(_WINNT) || defined(_LINUX)
#ifndef _WINDEF_
typedef        unsigned char    BYTE;
typedef        unsigned short   WORD;
typedef        unsigned long    DWORD;
#endif
typedef        signed char      SBYTE;
typedef        signed short     SWORD;
typedef        signed long      SDWORD;
#endif

#ifdef _OS2
#ifndef __OS2DEF__
typedef        unsigned char    BYTE;
#endif
typedef        unsigned short   WORD;
typedef        unsigned long    DWORD;
typedef        signed char      SBYTE;
typedef        signed short     SWORD;
typedef        signed long      SDWORD;
#endif

#ifdef _MSDOS
typedef        unsigned char    BYTE;
typedef        unsigned short   WORD;
typedef        unsigned long    DWORD;
typedef        signed char      SBYTE;
typedef        signed short     SWORD;
typedef        signed long      SDWORD;
#endif

typedef        float            FLOAT;
typedef        double           DOUBLE;

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

#ifdef _MSDOS                                             /* DOS keycodes */
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
#endif
#ifdef _WINNT                                               /* Windows NT keycodes */
#define K_ALT           0x100
#define K_CTRL          0x200
#define K_SHIFT         0x400
#define K_VK            0x800

#define K_F1            112
#define K_F2            113
#define K_F3            114
#define K_F4            115
#define K_F5            116
#define K_F6            117
#define K_F7            118
#define K_F8            119
#define K_F9            120
#define K_F10           121
#define K_F11           122
#define K_F12           123

#define K_S_F1          (112 | K_SHIFT)
#define K_S_F2          (113 | K_SHIFT)
#define K_S_F3          (114 | K_SHIFT)
#define K_S_F4          (115 | K_SHIFT)
#define K_S_F5          (116 | K_SHIFT)
#define K_S_F6          (117 | K_SHIFT)
#define K_S_F7          (118 | K_SHIFT)
#define K_S_F8          (119 | K_SHIFT)
#define K_S_F9          (120 | K_SHIFT)
#define K_S_F10         (121 | K_SHIFT)
#define K_S_F11         (122 | K_SHIFT)
#define K_S_F12         (123 | K_SHIFT)

#define K_C_F1          (112 | K_CTRL)
#define K_C_F2          (113 | K_CTRL)
#define K_C_F3          (114 | K_CTRL)
#define K_C_F4          (115 | K_CTRL)
#define K_C_F5          (116 | K_CTRL)
#define K_C_F6          (117 | K_CTRL)
#define K_C_F7          (118 | K_CTRL)
#define K_C_F8          (119 | K_CTRL)
#define K_C_F9          (120 | K_CTRL)
#define K_C_F10         (121 | K_CTRL)
#define K_C_F11         (122 | K_CTRL)
#define K_C_F12         (123 | K_CTRL)

#define K_A_F1          (112 | K_ALT)
#define K_A_F2          (113 | K_ALT)
#define K_A_F3          (114 | K_ALT)
#define K_A_F4          (115 | K_ALT)
#define K_A_F5          (116 | K_ALT)
#define K_A_F6          (117 | K_ALT)
#define K_A_F7          (118 | K_ALT)
#define K_A_F8          (119 | K_ALT)
#define K_A_F9          (120 | K_ALT)
#define K_A_F10         (121 | K_ALT)
#define K_A_F11         (122 | K_ALT)
#define K_A_F12         (123 | K_ALT)

#define K_HOME          (36 | K_VK)
#define K_END           (35 | K_VK)
#define K_PGUP          (33 | K_VK)
#define K_PGDN          (34 | K_VK)
#define K_LEFT          (37 | K_VK)
#define K_RIGHT         (39 | K_VK)
#define K_UP            (38 | K_VK)
#define K_DOWN          (40 | K_VK)
#define K_INS           (45 | K_VK)
#define K_DEL           (46 | K_VK)

#define K_S_HOME        (36 | K_VK | K_SHIFT)
#define K_S_END         (35 | K_VK | K_SHIFT)
#define K_S_PGUP        (33 | K_VK | K_SHIFT)
#define K_S_PGDN        (34 | K_VK | K_SHIFT)
#define K_S_LEFT        (37 | K_VK | K_SHIFT)
#define K_S_RIGHT       (39 | K_VK | K_SHIFT)
#define K_S_UP          (38 | K_VK | K_SHIFT)
#define K_S_DOWN        (40 | K_VK | K_SHIFT)
#define K_S_INS         (45 | K_VK | K_SHIFT)
#define K_S_DEL         (46 | K_VK | K_SHIFT)

#define K_C_HOME        (36 | K_VK | K_CTRL)
#define K_C_END         (35 | K_VK | K_CTRL)
#define K_C_PGUP        (33 | K_VK | K_CTRL)
#define K_C_PGDN        (34 | K_VK | K_CTRL)
#define K_C_LEFT        (37 | K_VK | K_CTRL)
#define K_C_RIGHT       (39 | K_VK | K_CTRL)
#define K_C_UP          (38 | K_VK | K_CTRL)
#define K_C_DOWN        (40 | K_VK | K_CTRL)
#define K_C_INS         (45 | K_VK | K_CTRL)
#define K_C_DEL         (46 | K_VK | K_CTRL)

#define K_C_A           (1 | K_CTRL)
#define K_C_B           (2 | K_CTRL)
#define K_C_C           (3 | K_CTRL)
#define K_C_D           (4 | K_CTRL)
#define K_C_E           (5 | K_CTRL)
#define K_C_F           (6 | K_CTRL)
#define K_C_G           (7 | K_CTRL)
#define K_C_H           (8 | K_CTRL)
#define K_C_I           (9 | K_CTRL)
#define K_C_J           (10 | K_CTRL)
#define K_C_K           (11 | K_CTRL)
#define K_C_L           (12 | K_CTRL)
#define K_C_M           (13 | K_CTRL)
#define K_C_N           (14 | K_CTRL)
#define K_C_O           (15 | K_CTRL)
#define K_C_P           (16 | K_CTRL)
#define K_C_Q           (17 | K_CTRL)
#define K_C_R           (18 | K_CTRL)
#define K_C_S           (19 | K_CTRL)
#define K_C_T           (20 | K_CTRL)
#define K_C_U           (21 | K_CTRL)
#define K_C_V           (22 | K_CTRL)
#define K_C_W           (23 | K_CTRL)
#define K_C_X           (24 | K_CTRL)
#define K_C_Y           (25 | K_CTRL)
#define K_C_Z           (26 | K_CTRL)

#define K_A_A           (98 | K_ALT)
#define K_A_B           (99 | K_ALT)
#define K_A_C           (90 | K_ALT)
#define K_A_D           (100 | K_ALT)
#define K_A_E           (101 | K_ALT)
#define K_A_F           (102 | K_ALT)
#define K_A_G           (103 | K_ALT)
#define K_A_H           (104 | K_ALT)
#define K_A_I           (105 | K_ALT)
#define K_A_J           (106 | K_ALT)
#define K_A_K           (107 | K_ALT)
#define K_A_L           (108 | K_ALT)
#define K_A_M           (109 | K_ALT)
#define K_A_N           (110 | K_ALT)
#define K_A_O           (111 | K_ALT)
#define K_A_P           (112 | K_ALT)
#define K_A_Q           (113 | K_ALT)
#define K_A_R           (114 | K_ALT)
#define K_A_S           (115 | K_ALT)
#define K_A_T           (116 | K_ALT)
#define K_A_U           (117 | K_ALT)
#define K_A_V           (118 | K_ALT)
#define K_A_W           (119 | K_ALT)
#define K_A_X           (120 | K_ALT)
#define K_A_Y           (121 | K_ALT)
#define K_A_Z           (122 | K_ALT)

#define K_A_1           (49 | K_ALT)
#define K_A_2           (50 | K_ALT)
#define K_A_3           (51 | K_ALT)
#define K_A_4           (52 | K_ALT)
#define K_A_5           (53 | K_ALT)
#define K_A_6           (54 | K_ALT)
#define K_A_7           (55 | K_ALT)
#define K_A_8           (56 | K_ALT)
#define K_A_9           (57 | K_ALT)
#define K_A_0           (48 | K_ALT)
#define K_A_DASH        (189 | K_ALT)
#define K_A_EQUAL       (48 | K_ALT | K_SHIFT)

#define K_ESC           27
#define K_SPACE         32
#define K_TAB           9
#define K_SHIFT_TAB     (9 | K_SHIFT)
#define K_BACKSPACE     8
#define K_C_BACKSPACE   (127 | K_CTRL)
#define K_C_PTRSC       0               /* not recognized under Windows NT */
#define K_ENTER         13
#define K_S_ENTER       (13 | K_SHIFT)
#define K_C_ENTER       (10 | K_CTRL)
#endif
#ifdef _OS2                                    /* OS/2 keycodes */
#define K_ALT           0x100
#define K_CTRL          0x200
#define K_SHIFT         0x400
#define K_VK            0x800

#define K_F1            (59 | K_VK)
#define K_F2            (60 | K_VK)
#define K_F3            (61 | K_VK)
#define K_F4            (62 | K_VK)
#define K_F5            (63 | K_VK)
#define K_F6            (64 | K_VK)
#define K_F7            (65 | K_VK)
#define K_F8            (66 | K_VK)
#define K_F9            (67 | K_VK)
#define K_F10           (68 | K_VK)
#define K_F11           (133 | K_VK)
#define K_F12           (134 | K_VK)

#define K_S_F1          (84 | K_VK | K_SHIFT)
#define K_S_F2          (85 | K_VK | K_SHIFT)
#define K_S_F3          (86 | K_VK | K_SHIFT)
#define K_S_F4          (87 | K_VK | K_SHIFT)
#define K_S_F5          (88 | K_VK | K_SHIFT)
#define K_S_F6          (89 | K_VK | K_SHIFT)
#define K_S_F7          (90 | K_VK | K_SHIFT)
#define K_S_F8          (91 | K_VK | K_SHIFT)
#define K_S_F9          (92 | K_VK | K_SHIFT)
#define K_S_F10         (93 | K_VK | K_SHIFT)
#define K_S_F11         (135 | K_VK | K_SHIFT)
#define K_S_F12         (136 | K_VK | K_SHIFT)

#define K_C_F1          (94 | K_VK | K_CTRL)
#define K_C_F2          (95 | K_VK | K_CTRL)
#define K_C_F3          (96 | K_VK | K_CTRL)
#define K_C_F4          (97 | K_VK | K_CTRL)
#define K_C_F5          (98 | K_VK | K_CTRL)
#define K_C_F6          (99 | K_VK | K_CTRL)
#define K_C_F7          (100 | K_VK | K_CTRL)
#define K_C_F8          (101 | K_VK | K_CTRL)
#define K_C_F9          (102 | K_VK | K_CTRL)
#define K_C_F10         (103 | K_VK | K_CTRL)
#define K_C_F11         (137 | K_VK | K_CTRL)
#define K_C_F12         (138 | K_VK | K_CTRL)

#define K_A_F1          (104 | K_VK | K_ALT)
#define K_A_F2          (105 | K_VK | K_ALT)
#define K_A_F3          (106 | K_VK | K_ALT)
#define K_A_F4          (107 | K_VK | K_ALT)
#define K_A_F5          (108 | K_VK | K_ALT)
#define K_A_F6          (109 | K_VK | K_ALT)
#define K_A_F7          (110 | K_VK | K_ALT)
#define K_A_F8          (111 | K_VK | K_ALT)
#define K_A_F9          (112 | K_VK | K_ALT)
#define K_A_F10         (113 | K_VK | K_ALT)
#define K_A_F11         (139 | K_VK | K_ALT)
#define K_A_F12         (140 | K_VK | K_ALT)

#define K_HOME          (71 | K_VK)
#define K_END           (79 | K_VK)
#define K_PGUP          (73 | K_VK)
#define K_PGDN          (81 | K_VK)
#define K_LEFT          (75 | K_VK)
#define K_RIGHT         (77 | K_VK)
#define K_UP            (72 | K_VK)
#define K_DOWN          (80 | K_VK)
#define K_INS           (82 | K_VK)
#define K_DEL           (83 | K_VK)

#define K_S_HOME        (71 | K_VK | K_SHIFT)
#define K_S_END         (79 | K_VK | K_SHIFT)
#define K_S_PGUP        (73 | K_VK | K_SHIFT)
#define K_S_PGDN        (81 | K_VK | K_SHIFT)
#define K_S_LEFT        (75 | K_VK | K_SHIFT)
#define K_S_RIGHT       (77 | K_VK | K_SHIFT)
#define K_S_UP          (72 | K_VK | K_SHIFT)
#define K_S_DOWN        (80 | K_VK | K_SHIFT)
#define K_S_INS         (82 | K_VK | K_SHIFT)
#define K_S_DEL         (83 | K_VK | K_SHIFT)

#define K_C_HOME        (119 | K_VK | K_CTRL)
#define K_C_END         (117 | K_VK | K_CTRL)
#define K_C_PGUP        (132 | K_VK | K_CTRL)
#define K_C_PGDN        (118 | K_VK | K_CTRL)
#define K_C_LEFT        (115 | K_VK | K_CTRL)
#define K_C_RIGHT       (116 | K_VK | K_CTRL)
#define K_C_UP          (141 | K_VK | K_CTRL)
#define K_C_DOWN        (145 | K_VK | K_CTRL)
#define K_C_INS         (146 | K_VK | K_CTRL)
#define K_C_DEL         (147 | K_VK | K_CTRL)

#define K_C_A           (1 | K_CTRL)
#define K_C_B           (2 | K_CTRL)
#define K_C_C           (3 | K_CTRL)
#define K_C_D           (4 | K_CTRL)
#define K_C_E           (5 | K_CTRL)
#define K_C_F           (6 | K_CTRL)
#define K_C_G           (7 | K_CTRL)
#define K_C_H           (8 | K_CTRL)
#define K_C_I           (9 | K_CTRL)
#define K_C_J           (10 | K_CTRL)
#define K_C_K           (11 | K_CTRL)
#define K_C_L           (12 | K_CTRL)
#define K_C_M           (13 | K_CTRL)
#define K_C_N           (14 | K_CTRL)
#define K_C_O           (15 | K_CTRL)
#define K_C_P           (16 | K_CTRL)
#define K_C_Q           (17 | K_CTRL)
#define K_C_R           (18 | K_CTRL)
#define K_C_S           (19 | K_CTRL)
#define K_C_T           (20 | K_CTRL)
#define K_C_U           (21 | K_CTRL)
#define K_C_V           (22 | K_CTRL)
#define K_C_W           (23 | K_CTRL)
#define K_C_X           (24 | K_CTRL)
#define K_C_Y           (25 | K_CTRL)
#define K_C_Z           (26 | K_CTRL)

#define K_A_A           (30 | K_VK | K_ALT)
#define K_A_B           (48 | K_VK | K_ALT)
#define K_A_C           (46 | K_VK | K_ALT)
#define K_A_D           (32 | K_VK | K_ALT)
#define K_A_E           (18 | K_VK | K_ALT)
#define K_A_F           (33 | K_VK | K_ALT)
#define K_A_G           (34 | K_VK | K_ALT)
#define K_A_H           (35 | K_VK | K_ALT)
#define K_A_I           (23 | K_VK | K_ALT)
#define K_A_J           (36 | K_VK | K_ALT)
#define K_A_K           (37 | K_VK | K_ALT)
#define K_A_L           (38 | K_VK | K_ALT)
#define K_A_M           (50 | K_VK | K_ALT)
#define K_A_N           (49 | K_VK | K_ALT)
#define K_A_O           (24 | K_VK | K_ALT)
#define K_A_P           (25 | K_VK | K_ALT)
#define K_A_Q           (16 | K_VK | K_ALT)
#define K_A_R           (19 | K_VK | K_ALT)
#define K_A_S           (31 | K_VK | K_ALT)
#define K_A_T           (20 | K_VK | K_ALT)
#define K_A_U           (22 | K_VK | K_ALT)
#define K_A_V           (47 | K_VK | K_ALT)
#define K_A_W           (17 | K_VK | K_ALT)
#define K_A_X           (45 | K_VK | K_ALT)
#define K_A_Y           (21 | K_VK | K_ALT)
#define K_A_Z           (44 | K_VK | K_ALT)

#define K_A_1           (120 | K_VK | K_ALT)
#define K_A_2           (121 | K_VK | K_ALT)
#define K_A_3           (122 | K_VK | K_ALT)
#define K_A_4           (123 | K_VK | K_ALT)
#define K_A_5           (124 | K_VK | K_ALT)
#define K_A_6           (125 | K_VK | K_ALT)
#define K_A_7           (126 | K_VK | K_ALT)
#define K_A_8           (127 | K_VK | K_ALT)
#define K_A_9           (128 | K_VK | K_ALT)
#define K_A_0           (129 | K_VK | K_ALT)
#define K_A_DASH        (53 | K_VK | K_ALT)
#define K_A_EQUAL       (129 | K_VK | K_ALT | K_SHIFT)

#define K_ESC           27
#define K_SPACE         32
#define K_TAB           9
#define K_SHIFT_TAB     (15 | K_VK | K_SHIFT)
#define K_BACKSPACE     8
#define K_C_BACKSPACE   (127 | K_CTRL)
#define K_C_PTRSC       0                     /* not recognized under OS/2 */
#define K_ENTER         13
#define K_S_ENTER       (13 | K_SHIFT)
#define K_C_ENTER       (10 | K_CTRL)
#endif


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
