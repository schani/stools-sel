#include <windows.h>
#include <gemein.h>
#include <mouse.h>
#include <stnt.h>

static COORD coordMousePos  = {0, 0};
static DWORD dwMouseButtons = 0;
static WORD  wLeftPress     = 0,
             wLeftRelease   = 0,
             wRightPress    = 0,
             wRightRelease  = 0,
             wMiddlePress   = 0,
             wMiddleRelease = 0;

extern INT   iSizeX;
extern INT   iSizeY;

BOOL msm_init (void)
{
  return TRUE;
}

void msm_process_input_event (MOUSE_EVENT_RECORD *pmer)
{
  coordMousePos = pmer->dwMousePosition;
  if (coordMousePos.X < 0)
    coordMousePos.X = 0;
  else
    if (coordMousePos.X >= iSizeX)
      coordMousePos.X = iSizeX - 1;
  if (coordMousePos.Y < 0)
    coordMousePos.Y = 0;
  else
    if (coordMousePos.Y >= iSizeY)
      coordMousePos.Y = iSizeY - 1;

  if ((dwMouseButtons & MSM_B_LEFT) ^ (pmer->dwButtonState & MSM_B_LEFT))
  {
    if (dwMouseButtons & MSM_B_LEFT)
      wLeftRelease++;
    else
      wLeftPress++;
  }
  if ((dwMouseButtons & MSM_B_RIGHT) ^ (pmer->dwButtonState & MSM_B_RIGHT))
  {
    if (dwMouseButtons & MSM_B_RIGHT)
      wRightRelease++;
    else
      wRightPress++;
  }
  if ((dwMouseButtons & MSM_B_MIDDLE) ^ (pmer->dwButtonState & MSM_B_MIDDLE))
  {
    if (dwMouseButtons & MSM_B_MIDDLE)
      wMiddleRelease++;
    else
      wMiddlePress++;
  }
  dwMouseButtons = pmer->dwButtonState;
}

BOOL msm_cursor_on (void)
{
  return TRUE;
}

BOOL msm_cursor_off (void)
{
  return TRUE;
}

BOOL msm_cursor (void)
{
  return TRUE;
}

void msm_get_buttons (WORD *pwButtons, INT *piHor, INT *piVer)
{
  *pwButtons = (WORD)(dwMouseButtons & 0x07);
  *piHor = coordMousePos.X + 1;
  *piVer = coordMousePos.Y + 1;
}

void msm_set_position (INT iHor, INT iVer)
{
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

void msm_set_hor (INT iMin, INT iMax)
{
}

void msm_set_ver (INT iMin, INT iMax)
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

void msm_set_mickeys (WORD wHorMickeys, WORD wVerMickeys)
{
}  

void msm_get_mickeys (WORD *pwHorMickeys, WORD *pwVerMickeys)
{
  *pwHorMickeys = 8;
  *pwVerMickeys = 16;
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
