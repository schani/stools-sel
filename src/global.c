/***************************************************************************
 ***************************************************************************
 ***                                                                     ***
 ***                       Schani-Tools fÅr C                            ***
 ***                                                                     ***
 ***                   Datei fÅr globale Variablen                       ***
 ***                                                                     ***
 ***                (c) 1990-93 by Schani Electronics                    ***
 ***                                                                     ***
 ***************************************************************************
 ***************************************************************************/

#include <gemein.h>
#include <video.h>
#include <mouse.h>
#include <utility.h>
#include <contain.h>
#include <menu.h>
#include <status.h>
#include <sthelp.h>
#include <dlg.h>
#include <window.h>
#include <boxes.h>
#include <global.h>
#include <stdlib.h>
#include <sound.h>

/***************************************************************************
 *                           Rahmentypen                                   *
 ***************************************************************************/

CHAR aacBorder[21][8] =
{
  "⁄ø¿Ÿƒƒ≥≥",                                                    /* R_EEEE */
  "÷ø”Ÿƒƒ∫≥",                                                    /* R_EEED */
  "⁄ø‘æƒÕ≥≥",                                                    /* R_EEDE */
  "÷ø»æƒÕ∫≥",                                                    /* R_EEDD */
  "⁄∑¿Ωƒƒ≥∫",                                                    /* R_EDEE */
  "÷∑”Ωƒƒ∫∫",                                                    /* R_EDED */
  "⁄∑‘ºƒÕ≥∫",                                                    /* R_EDDE */
  "÷∑»ºƒÕ∫∫",                                                    /* R_EDDD */
  "’∏¿ŸÕƒ≥≥",                                                    /* R_DEEE */
  "…∏”ŸÕƒ∫≥",                                                    /* R_DEED */
  "’∏‘æÕÕ≥≥",                                                    /* R_DEDE */
  "…∏»æÕÕ∫≥",                                                    /* R_DEDD */
  "’ª¿ΩÕƒ≥∫",                                                    /* R_DDEE */
  "…ª”ΩÕƒ∫∫",                                                    /* R_DDED */
  "’ª‘ºÕÕ≥∫",                                                    /* R_DDDE */
  "…ª»ºÕÕ∫∫",                                                    /* R_DDDD */

  "        ",                                                    /* R_000P */
  "∞∞∞∞∞∞∞∞",                                                    /* R_025P */
  "±±±±±±±±",                                                    /* R_050P */
  "≤≤≤≤≤≤≤≤",                                                    /* R_075P */
  "€€€€€€€€",                                                    /* R_100P */
};

/***************************************************************************
 *                          Farbpaletten                                   *
 ***************************************************************************/
                   
CHAR *apcPalWindows[]               =
                                      {
                                        "Desktop",
                                        "MenÅfenster",
                                        "Dialogfenster",
                                        "Hilfefenster",
                                        "Infofenster",
                                        "Warnungsfenster",
                                        "Fehlerfenster",
                                        NULL
                                      };
CHAR *aapcPalColors[][PAL_COLORS+1] = {
                                        {
                                          "Hintergrund",
                                          "MenÅzeile",
                                          "  aktives Item",
                                          "  Hot-Key",
                                          "  selektiertes aktives Item",
                                          "  selektierter Hot-Key",
                                          "  inaktives Item",
                                          "  selektiertes inaktives Item",
                                          "Hilfezeile",
                                          "  aktives Item",
                                          "  Hot-Key",
                                          "  selektiertes aktives Item",
                                          "  selektierter Hot-Key",
                                          "  inaktives Item",
                                          "  selektiertes inaktives Item",
                                          NULL,
                                          NULL,
                                          NULL,
                                          NULL,
                                          NULL,
                                          NULL,
                                          NULL,
                                          NULL
                                        },
                                        {
                                          "Hintergrund",
                                          "Rahmen",
                                          "[nicht benutzt]",
                                          "Icons",
                                          "aktives Item",
                                          "  Hot-Key",
                                          "  selektiertes aktives Item",
                                          "  selektierter Hot-Key",
                                          "inaktives Item",
                                          "  selektiertes inaktives Item",
                                          NULL,
                                          NULL,
                                          NULL,
                                          NULL,
                                          NULL,
                                          NULL,
                                          NULL,
                                          NULL,
                                          NULL,
                                          NULL,
                                          NULL,
                                          NULL,
                                          NULL
                                        },
                                        {
                                          "Hintergrund",
                                          "Rahmen",
                                          "Titel",
                                          "Icons",
                                          "Action-Button",
                                          "  Hot-Key",
                                          "  inverser Button",
                                          "Label",
                                          "  Hot-Key",
                                          "Item (List-Box)",
                                          "  selektiertes Item",
                                          "Scroll-Bars",
                                          "markierter Text",
                                          "Hilfetext",
                                          "  fett",
                                          "  kursiv",
                                          "  unterstrichen",
                                          "  fett-kursiv",
                                          "  fett-unterstrichen",
                                          "  kursiv-unterstrichen",
                                          "  fett-kursiv-unterstrichen",
                                          "  selektierter Hot-Spot",
                                          NULL
                                        },
                                        {
                                          "Hintergrund",
                                          "Rahmen",
                                          "Titel",
                                          "Icons",
                                          "[nicht benutzt]",
                                          "[nicht benutzt]",
                                          "[nicht benutzt]",
                                          "[nicht benutzt]",
                                          "[nicht benutzt]",
                                          "[nicht benutzt]",
                                          "[nicht benutzt]",
                                          "Scroll-Bars",
                                          "[nicht benutzt]",
                                          "Hilfetext",
                                          "  fett",
                                          "  kursiv",
                                          "  unterstrichen",
                                          "  fett-kursiv",
                                          "  fett-unterstrichen",
                                          "  kursiv-unterstrichen",
                                          "  fett-kursiv-unterstrichen",
                                          "  selektierter Hot-Spot",
                                          NULL
                                        },
                                        {
                                          "Hintergrund",
                                          "Rahmen",
                                          "Titel",
                                          "[nicht benutzt]",
                                          "Action-Button",
                                          "  Hot-Key",
                                          "  inverser Button",
                                          NULL,
                                          NULL,
                                          NULL,
                                          NULL,
                                          NULL,
                                          NULL,
                                          NULL,
                                          NULL,
                                          NULL,
                                          NULL,
                                          NULL,
                                          NULL,
                                          NULL,
                                          NULL,
                                          NULL,
                                          NULL
                                        },
                                        {
                                          "Hintergrund",
                                          "Rahmen",
                                          "Titel",
                                          "[nicht benutzt]",
                                          "Action-Button",
                                          "  Hot-Key",
                                          "  inverser Button",
                                          NULL,
                                          NULL,
                                          NULL,
                                          NULL,
                                          NULL,
                                          NULL,
                                          NULL,
                                          NULL,
                                          NULL,
                                          NULL,
                                          NULL,
                                          NULL,
                                          NULL,
                                          NULL,
                                          NULL,
                                          NULL
                                        },
                                        {
                                          "Hintergrund",
                                          "Rahmen",
                                          "Titel",
                                          "[nicht benutzt]",
                                          "Action-Button",
                                          "  Hot-Key",
                                          "  inverser Button",
                                          NULL,
                                          NULL,
                                          NULL,
                                          NULL,
                                          NULL,
                                          NULL,
                                          NULL,
                                          NULL,
                                          NULL,
                                          NULL,
                                          NULL,
                                          NULL,
                                          NULL,
                                          NULL,
                                          NULL,
                                          NULL
                                        }
                                      };              
                                      
CHAR apcColors[] = /* Blaue Palette */
                   "\x30\x70\x70\x71\x4f\x4f\x78\x47\x71\x70\x71\x4f\x4f\x78\x47\x00\x00\x00\x00\x00\x00\x00"
                   "\x70\x70\x00\x70\x70\x71\x4f\x4f\x78\x47\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00"
                   "\x17\x17\x17\x1b\x17\x1f\x71\x17\x1f\x17\x4f\x70\x71\x17\x1f\x1a\x19\x1e\x1d\x1b\x1c\x71"
                   "\x17\x17\x17\x1b\x00\x00\x00\x00\x00\x00\x00\x70\x00\x17\x1f\x1a\x19\x1e\x1d\x1b\x1c\x71"
                   "\x17\x17\x17\x00\x17\x1f\x71\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00"
                   "\x60\x60\x60\x00\x60\x6e\x06\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00"
                   "\x4f\x4f\x4f\x00\x4f\x4e\x74\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00"
                   /* Weisse Palette */
                   "\x70\x70\x70\x7c\x0f\x0c\x78\x07\x70\x70\x7c\x0f\x0c\x78\x07\x00\x00\x00\x00\x00\x00\x00"
                   "\x70\x70\x00\x7a\x70\x7c\x0f\x0c\x78\x07\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00"
                   "\x70\x7f\x7f\x7a\x70\x7f\x0f\x70\x7f\x70\x4f\x07\x07\x70\x7f\x7a\x79\x7e\x7d\x7b\x7c\x0f"
                   "\x70\x7f\x7f\x7a\x00\x00\x00\x00\x00\x00\x00\x07\x00\x70\x7f\x7a\x79\x7e\x7d\x7b\x7c\x0f"
                   "\x70\x7f\x7f\x00\x70\x7f\x0f\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00"
                   "\x60\x60\x60\x00\x60\x6e\x06\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00"
                   "\x4f\x4f\x4f\x00\x4f\x4e\x74\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00"
                   /* Tuerkis Palette */
                   "\x10\x30\x30\x3c\x1f\x1c\x38\x17\x30\x30\x3c\x1f\x1c\x38\x17\x00\x00\x00\x00\x00\x00\x00"
                   "\x30\x30\x00\x3b\x30\x3c\x1f\x1c\x38\x17\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00"
                   "\x70\x71\x71\x79\x70\x7f\x0f\x70\x7f\x70\x1f\x07\x07\x70\x7f\x7a\x79\x7e\x7d\x7b\x7c\x0f"
                   "\x30\x3f\x3f\x3a\x00\x00\x00\x00\x00\x00\x00\x07\x00\x30\x3f\x3a\x39\x3e\x3d\x3b\x3c\x1e"
                   "\x70\x7f\x7f\x00\x70\x7f\x1f\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00"
                   "\x60\x60\x60\x00\x60\x6e\x06\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00"
                   "\x4f\x4f\x4f\x00\x4f\x4e\x74\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00"
                   /* Schwarze Palette */
                   "\x70\x70\x70\x79\x0f\x0f\x78\x07\x70\x70\x79\x0f\x09\x78\x07\x00\x00\x00\x00\x00\x00\x00"
                   "\x70\x70\x00\x7a\x70\x79\x0f\x0f\x78\x07\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00"
                   "\x07\x0f\x0f\x0a\x07\x0f\x70\x07\x0f\x07\x70\x70\x70\x07\x0f\x0a\x09\x0e\x0d\x0b\x0c\x70"
                   "\x07\x0f\x0f\x0a\x00\x00\x00\x00\x00\x00\x00\x70\x00\x07\x0f\x0a\x09\x0e\x0d\x0b\x0c\x70"
                   "\x07\x0f\x0f\x00\x07\x0f\x70\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00"
                   "\x60\x60\x60\x00\x60\x6e\x06\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00"
                   "\x4f\x4f\x4f\x00\x4f\x4e\x74\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00"
                   /* Gruene Palette */
                   "\x71\x70\x70\x74\x20\x24\x78\x28\x70\x70\x74\x20\x24\x78\x28\x00\x00\x00\x00\x00\x00\x00"
                   "\x70\x70\x00\x7a\x70\x74\x20\x24\x78\x28\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00"
                   "\x70\x7f\x7f\x7a\x70\x7e\x20\x70\x7e\x70\x2f\x13\x2f\x30\x3f\x3a\x39\x3e\x3d\x3b\x3c\x1e"
                   "\x30\x3f\x3f\x3a\x00\x00\x00\x00\x00\x00\x00\x13\x00\x30\x3f\x3a\x39\x3e\x3d\x3b\x3c\x1e"
                   "\x17\x17\x17\x00\x17\x1f\x71\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00"
                   "\x60\x60\x60\x00\x60\x6e\x06\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00"
                   "\x4f\x4f\x4f\x00\x4f\x4e\x74\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00"
                   /* Monochrome Palette */
                   "\x70\x02\x0a\x09\x70\x79\x02\x40\x02\x0a\x09\x70\x7a\x02\x70\x69\x6e\x61\x6b\x74\x69\x76"
                   "\x02\x02\x20\x0a\x0a\x09\x70\x7a\x02\x70\x73\x65\x6c\x65\x6b\x74\x69\x65\x72\x74\x65\x73"
                   "\x70\x79\x7e\x7a\x70\x7a\x02\x70\x7a\x70\x02\x02\x02\x02\x0a\x0a\x01\x0a\x09\x09\x09\x7a"
                   "\x02\x0a\x0a\x0a\x64\x00\x52\x61\x68\x6d\x65\x02\x00\x02\x0a\x0a\x01\x0a\x09\x09\x09\x7a"
                   "\x70\x70\x70\x41\x70\x7a\x02\x6f\x6e\x2d\x42\x75\x74\x74\x6f\x6e\x00\x20\x20\x48\x6f\x74"
                   "\x7a\x7a\x7a\x79\x70\x7a\x0a\x69\x6e\x76\x65\x72\x73\x65\x72\x20\x42\x75\x74\x74\x6f\x6e"
                   "\x0a\x0a\x0a\x62\x02\x0a\x70\x20\x20\x48\x6f\x74\x2d\x4b\x65\x79\x00\x49\x74\x65\x6d\x20"
                   /* Benutzerdefinierte Palette */
                   "\x30\x70\x70\x71\x4f\x4f\x78\x47\x71\x70\x71\x4f\x4f\x78\x47\x00\x00\x00\x00\x00\x00\x00"
                   "\x70\x70\x00\x70\x70\x71\x4f\x4f\x78\x47\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00"
                   "\x17\x17\x17\x1b\x17\x1f\x71\x17\x1f\x17\x4f\x70\x71\x17\x1f\x1a\x19\x1e\x1d\x1b\x1c\x71"
                   "\x17\x17\x17\x1b\x00\x00\x00\x00\x00\x00\x00\x70\x00\x17\x1f\x1a\x19\x1e\x1d\x1b\x1c\x71"
                   "\x17\x17\x17\x00\x17\x1f\x71\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00"
                   "\x60\x60\x60\x00\x60\x6e\x06\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00"
                   "\x4f\x4f\x4f\x00\x4f\x4e\x74\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00";
                   
UCHAR       ucActPalette    = PAL_BLUE;
BOOL        bExit,
            bBeep           = TRUE,
            bSaver          = TRUE,
            bSaverActive    = FALSE;
INT         iCursorPercent;
CHAR        acBeepVOC[128]  = "";
ULONG       ulScreenTimeout = 300000;
ULONG       ulDoubleClick   = 350;
GLB_PROGRAM prgProgram;
ULONG       ulBeepFrequency = 500,
            ulBeepDuration  = 100;

#if defined(_OS2) || defined(_MSDOS)
UINT        uiMouseSpeed    = 8;
#endif
#ifdef _MSDOS
BOOL        bSoundBlaster   = FALSE,
            bVOCBeep        = FALSE,
            bPlayingBeep    = FALSE;
WORD        wStatusWord     = 0;
#endif

extern WINDOW winDesktop;
extern WINDOW winWorking;
#ifndef _MSDOS
extern INT    iSizeX;
extern INT    iSizeY;
#else
INT iSizeX = 80;
INT iSizeY = 25;
#endif

BOOL int_glb_send_message (GLB_MSG_OBJECT *pmsgobjFrom, GLB_MSG_OBJECT *pmsgobjTo, UINT uiMessage, ULONG ulAddInfo)
{
  UTL_EVENT eventEvent;

  if (!pmsgobjTo)
    return FALSE;
  if (!pmsgobjTo->handle_event)
    return FALSE;
  eventEvent.uiKind = E_MESSAGE;
  msm_get_buttons(&eventEvent.wButtons, &eventEvent.iHor, &eventEvent.iVer);
  eventEvent.dwTimer = utl_get_timer();
  eventEvent.wKey = 0;
  eventEvent.uiMessage = uiMessage;
  eventEvent.pFrom = pmsgobjFrom;
  eventEvent.ulAddInfo = ulAddInfo;
  pmsgobjTo->handle_event(pmsgobjTo, &eventEvent);
  if (eventEvent.uiKind == E_DONE)
    return TRUE;
  return FALSE;
}

void glb_update_windows (void)
{
  WINDOW winWindow;
  
  for (winWindow = prgProgram.winFirst; winWindow;
       winWindow = winWindow->winNext)
    win_update(winWindow);
}                   

void glb_set_palette (UCHAR ucPalette)
{
  ucActPalette = ucPalette;
  prgProgram.pcPalette = apcColors + ucActPalette * PAL_WINDOWS * PAL_COLORS;
  glb_update_windows();
}

void glb_execute (WINDOW winWindow)
{
  win_show(winWindow);
  win_modal(winWindow, TRUE);
  win_set_focus(winWindow);
  glb_run();
  bExit = FALSE;
}   

void glb_pre_process (UTL_EVENT *peventEvent)
{
  sts_pre_process(peventEvent);
  if (peventEvent->uiKind != E_DONE)
    mnu_pre_process(peventEvent);
}

void glb_run (void)
{
    WINDOW winWindow;
    
    do
    {
        bExit = FALSE;
        utl_event(&(prgProgram.eventEvent));
        if (!winWorking)
            if (!prgProgram.winFocussed->flFlags.binModal)
                glb_pre_process(&(prgProgram.eventEvent));
        if (prgProgram.eventEvent.uiKind != E_DONE)
        {
            if (winWorking)
                win_handle_event(winWorking, &(prgProgram.eventEvent));
            else
            {
                if (prgProgram.winFocussed->flFlags.binModal)
                    win_handle_event(prgProgram.winFocussed, &(prgProgram.eventEvent));
                else
                {
                    winWindow = win_which(&(prgProgram.eventEvent));
                    if (prgProgram.eventEvent.uiKind == E_MSM_L_DOWN &&
                        winWindow != prgProgram.winFocussed &&
                        winWindow->flFlags.binCanBeFocussed)
                        win_set_focus(winWindow);
                    win_handle_event(winWindow, &(prgProgram.eventEvent));
                }
            }
        }
    } while (!bExit);
}

void glb_done (void)
{                                        
  glb_send_message(NULL, &prgProgram, M_QUIT, 0);
  msm_cursor_off();
  vio_sp_za(' ', 7);
  vio_set_cursor_pos(1, 1);
#ifndef _MSDOS
  vio_set_cursor_type(iCursorPercent);
#endif
}

void glb_init (void (*handle_event)(GLB_PROGRAM*, UTL_EVENT*))
{
  vio_init();
  if (!vio_color())
    ucActPalette = PAL_MONO;
  prgProgram.pcPalette = apcColors + ucActPalette * PAL_WINDOWS * PAL_COLORS;
  msm_init();
#if defined(_MSDOS) || defined(_OS2)
  msm_set_mickeys(uiMouseSpeed, (uiMouseSpeed * 2));
#endif
  utl_init();
#ifdef _MSDOS
  //sbl_init();
#endif
  win_init();
  mnu_init();
  box_init();
/*  utl_insert_short(K_F1, hlp_help_handler);*/
  dsk_get_cursor_type(&iCursorPercent);
  dsk_set_cursor_pos(1, 1);
  dsk_set_cursor_type(20);
  dsk_cursor(FALSE);
  dsk_sp_za('±', dsk_get_color(PAL_COL_BACKGROUND));
  dsk_sw_za(1, 1, iSizeX, 1, ' ', dsk_get_color(PAL_COL_DSK_MENU_BAR));
  dsk_sw_za(1, iSizeY, iSizeX, 1, ' ', dsk_get_color(PAL_COL_DSK_STATUS_LINE));
  prgProgram.winFocussed = winDesktop;
  prgProgram.handle_event = handle_event;
  msm_cursor_on();
  glb_send_message(NULL, &prgProgram, M_INIT, 0);
  atexit(glb_done);
}
