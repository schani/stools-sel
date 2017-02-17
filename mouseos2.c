#define INCL_SUB
#include <os2.h>
#include <gemein.h>
#include <mouse.h>
#include <stos2.h>

extern INT iSizeX;
extern INT iSizeY;

       HMOU hmouMouse      = NULL;
static BOOL bCursorOn      = FALSE;
static WORD wButtons       = 0,
            wLeftPress     = 0,
            wLeftRelease   = 0,
            wRightPress    = 0,
            wRightRelease  = 0,
            wMiddlePress   = 0,
            wMiddleRelease = 0;

BOOL msm_init (void)
{
  if (MouOpen(NULL, &hmouMouse) == 0)
  {
    USHORT usEventMask = MOUSE_BN1_DOWN,
           usButtons;
    
    bCursorOn = FALSE;
    MouGetNumButtons(&usButtons, hmouMouse);
    if (usButtons > 1)
      usEventMask |= MOUSE_BN2_DOWN;
    if (usButtons > 2)
      usEventMask |= MOUSE_BN3_DOWN;
    MouSetEventMask(&usEventMask, hmouMouse);
    return TRUE;
  }
  return FALSE;
}

BOOL msm_cursor_on (void)
{
  BOOL bReturnVar = bCursorOn;

  MouDrawPtr(hmouMouse);
  bCursorOn = TRUE;
  return bReturnVar;
}

BOOL msm_cursor_off (void)
{
  BOOL      bReturnVar = bCursorOn;
  NOPTRRECT nprect;

  nprect.row = 0;
  nprect.col = 0;
  nprect.cRow = iSizeY - 1;
  nprect.cCol = iSizeX - 1;
  MouRemovePtr(&nprect, hmouMouse);
  bCursorOn = FALSE;
  return bReturnVar;
}

BOOL msm_cursor (void)
{
  return bCursorOn;
}

void msm_process_input_event (MOUEVENTINFO *pmei)
{
  WORD wNewButtons = 0;

  if (pmei->fs & MOUSE_BN1_DOWN)
    wNewButtons |= MSM_B_LEFT;
  if (pmei->fs & MOUSE_BN2_DOWN)
    wNewButtons |= MSM_B_RIGHT;
  if (pmei->fs & MOUSE_BN3_DOWN)
    wNewButtons |= MSM_B_MIDDLE;
  if ((wNewButtons & MSM_B_LEFT) != (wButtons & MSM_B_LEFT))
  {
    if (wNewButtons & MSM_B_LEFT)
      wLeftPress++;
    else
      wLeftRelease++;
  }
  if ((wNewButtons & MSM_B_RIGHT) != (wButtons & MSM_B_RIGHT))
  {
    if (wNewButtons & MSM_B_RIGHT)
      wRightPress++;
    else
      wRightRelease++;
  }
  if ((wNewButtons & MSM_B_MIDDLE) != (wButtons & MSM_B_MIDDLE))
  {
    if (wNewButtons & MSM_B_MIDDLE)
      wMiddlePress++;
    else
      wMiddleRelease++;
  }
  wButtons = wNewButtons;
}

void msm_get_buttons (WORD *pwButtons, INT *piHor, INT *piVer)
{
  PTRLOC loc;

  *pwButtons = wButtons;
  MouGetPtrPos(&loc, hmouMouse);
  *piHor = loc.col + 1;
  *piVer = loc.row + 1;
}

void msm_set_position (INT iHor, INT iVer)
{
  PTRLOC loc;

  loc.col = iHor - 1;
  loc.row = iVer - 1;
  MouSetPtrPos(&loc, hmouMouse);
}

void msm_button_press (WORD wButton, WORD *pwButtons, WORD *pwPress, INT *piHor, INT *piVer)
{
  msm_get_buttons(pwButtons, piHor, piVer);
  switch (wButton)
  {
    case MSM_B_LEFT :
      *pwPress = wLeftPress;
      wLeftPress = 0;
      break;
    case MSM_B_RIGHT :
      *pwPress = wRightPress;
      wRightPress = 0;
      break;
    case MSM_B_MIDDLE :
      *pwPress = wMiddlePress;
      wMiddlePress = 0;
      break;
  }
}

void msm_button_release (WORD wButton, WORD *pwButtons, WORD *pwRelease, INT *piHor, INT *piVer)
{
  msm_get_buttons(pwButtons, piHor, piVer);
  switch (wButton)
  {
    case MSM_B_LEFT :
      *pwRelease = wLeftRelease;
      wLeftRelease = 0;
      break;
    case MSM_B_RIGHT :
      *pwRelease = wRightRelease;
      wRightRelease = 0;
      break;
    case MSM_B_MIDDLE :
      *pwRelease = wMiddleRelease;
      wMiddleRelease = 0;
      break;
  }
}

void msm_set_hor (INT iLeft, INT iRight)
{
}

void msm_set_ver (INT iTop, INT iBottom)
{
}

void msm_set_soft_cursor (WORD wAnd, WORD wXor)
{
}

void msm_set_hard_cursor (INT iStart, INT iStop)
{
}

void msm_move (INT *piHor, INT *piVer)
{
  *piHor = *piVer = 0;
}

void msm_lightpen_on (void)
{
}

void msm_lightpen_off (void)
{
}

void msm_set_mickeys (WORD wHor, WORD wVer)
{
  SCALEFACT sf;

  sf.colScale = wHor;
  sf.rowScale = wVer;
  MouSetScaleFact(&sf, hmouMouse);
}

void msm_get_mickeys (WORD *pwHor, WORD *pwVer)
{
  SCALEFACT sf;

  MouGetScaleFact(&sf, hmouMouse);
  *pwHor = (WORD)sf.colScale;
  *pwVer = (WORD)sf.rowScale;
}

void msm_set_threshold (WORD wThreshold)
{
}

WORD msm_get_threshold (void)
{
  return 200;
}

void msm_set_page (WORD wPage)
{
}

WORD msm_get_page (void)
{
  return 0;
}

BYTE msm_get_version (BYTE *pbyVersionMajor, BYTE *pbyVersionMinor)
{
  *pbyVersionMajor = 6;
  *pbyVersionMinor = 11;
  return MSM_M_BUS;
}
