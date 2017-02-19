#include <stdlib.h>
#include <assert.h>
#include <stdint.h>

#include <SDL2/SDL.h>
#include <SDL2_ttf/SDL_ttf.h>

#include <gemein.h>
#include <video.h>
#include <mouse.h>
#include <utility.h>

typedef CHAR attribute_t;

static inline int
attribute_foreground (attribute_t attr)
{
	return attr & 0xf;
}

static inline int
attribute_background (attribute_t attr)
{
	return (attr >> 4) & 0xf;
}

#define TTF_FONT_SIZE 16
#define FONT_WIDTH    8
#define FONT_HEIGHT   16
#define TEXT_COLUMNS  80
#define TEXT_ROWS     25
#define NUM_COLORS    16
#define WINDOW_WIDTH  (FONT_WIDTH * TEXT_COLUMNS)
#define WINDOW_HEIGHT (FONT_HEIGHT * TEXT_ROWS)

static Uint16 codepage_437_to_utf16[] = {
	0x0020, 0x263A, 0x263B, 0x2665, 0x2666, 0x2663, 0x2660, 0x2022,
	0x25D8, 0x25CB, 0x25D9, 0x2642, 0x2640, 0x266A, 0x266B, 0x263C,
	0x25BA, 0x25C4, 0x2195, 0x203C, 0x00B6, 0x00A7, 0x25AC, 0x21A8,
	0x2191, 0x2193, 0x2192, 0x2190, 0x221F, 0x2194, 0x25B2, 0x25BC,
	0x0020, 0x0021, 0x0022, 0x0023, 0x0024, 0x0025, 0x0026, 0x0027,
	0x0028, 0x0029, 0x002A, 0x002B, 0x002C, 0x002D, 0x002E, 0x002F,
	0x0030, 0x0031, 0x0032, 0x0033, 0x0034, 0x0035, 0x0036, 0x0037,
	0x0038, 0x0039, 0x003A, 0x003B, 0x003C, 0x003D, 0x003E, 0x003F,
	0x0040, 0x0041, 0x0042, 0x0043, 0x0044, 0x0045, 0x0046, 0x0047,
	0x0048, 0x0049, 0x004A, 0x004B, 0x004C, 0x004D, 0x004E, 0x004F,
	0x0050, 0x0051, 0x0052, 0x0053, 0x0054, 0x0055, 0x0056, 0x0057,
	0x0058, 0x0059, 0x005A, 0x005B, 0x005C, 0x005D, 0x005E, 0x005F,
	0x0060, 0x0061, 0x0062, 0x0063, 0x0064, 0x0065, 0x0066, 0x0067,
	0x0068, 0x0069, 0x006A, 0x006B, 0x006C, 0x006D, 0x006E, 0x006F,
	0x0070, 0x0071, 0x0072, 0x0073, 0x0074, 0x0075, 0x0076, 0x0077,
	0x0078, 0x0079, 0x007A, 0x007B, 0x007C, 0x007D, 0x007E, 0x2302,
	0x00C7, 0x00FC, 0x00E9, 0x00E2, 0x00E4, 0x00E0, 0x00E5, 0x00E7,
	0x00EA, 0x00EB, 0x00E8, 0x00EF, 0x00EE, 0x00EC, 0x00C4, 0x00C5,
	0x00C9, 0x00E6, 0x00C6, 0x00F4, 0x00F6, 0x00F2, 0x00FB, 0x00F9,
	0x00FF, 0x00D6, 0x00DC, 0x00A2, 0x00A3, 0x00A5, 0x20A7, 0x0192,
	0x00E1, 0x00ED, 0x00F3, 0x00FA, 0x00F1, 0x00D1, 0x00AA, 0x00BA,
	0x00BF, 0x2310, 0x00AC, 0x00BD, 0x00BC, 0x00A1, 0x00AB, 0x00BB,
	0x2591, 0x2592, 0x2593, 0x2502, 0x2524, 0x2561, 0x2562, 0x2556,
	0x2555, 0x2563, 0x2551, 0x2557, 0x255D, 0x255C, 0x255B, 0x2510,
	0x2514, 0x2534, 0x252C, 0x251C, 0x2500, 0x253C, 0x255E, 0x255F,
	0x255A, 0x2554, 0x2569, 0x2566, 0x2560, 0x2550, 0x256C, 0x2567,
	0x2568, 0x2564, 0x2565, 0x2559, 0x2558, 0x2552, 0x2553, 0x256B,
	0x256A, 0x2518, 0x250C, 0x2588, 0x2584, 0x258C, 0x2590, 0x2580,
	0x03B1, 0x00DF, 0x0393, 0x03C0, 0x03A3, 0x03C3, 0x00B5, 0x03C4,
	0x03A6, 0x0398, 0x03A9, 0x03B4, 0x221E, 0x03C6, 0x03B5, 0x2229,
	0x2261, 0x00B1, 0x2265, 0x2264, 0x2320, 0x2321, 0x00F7, 0x2248,
	0x00B0, 0x2219, 0x00B7, 0x221A, 0x207F, 0x00B2, 0x25A0, 0x00A0,
	0x0000
};

INT iSizeX, iSizeY;

static BOOL need_redraw = TRUE;
static CHARACTER *pcharPhysPage = NULL, *pcharVirPage = NULL, *pcharBuffer = NULL;
static INT iCursorX = 1, iCursorY = 1, iCursorPercent = 20;

static SDL_Window *window;
static SDL_Renderer *renderer;
static SDL_Texture *font_texture;

static SDL_Color colors [NUM_COLORS];

static inline CHARACTER*
VIO_BUFFER_POS (INT x, INT y)
{
	assert (x > 0 && x <= iSizeX);
	assert (y > 0 && y <= iSizeY);

	return pcharVirPage + (x-1 + (y-1) * iSizeX);
}

static void
make_colors (void)
{
	int i;
	for (i = 0; i < NUM_COLORS; i++) {
		colors [i].r = 0xaa * ((i >> 2) & 1);
		colors [i].g = 0xaa * ((i >> 1) & 1);
		colors [i].b = 0xaa * ( i       & 1);
		colors [i].a = 0;
		if (i >= 8) {
			colors [i].r += 0x55;
			colors [i].g += 0x55;
			colors [i].b += 0x55;
		}
	}
}

static SDL_Texture*
make_font_texture (TTF_Font *font)
{
	SDL_Surface *surface;
	Uint32 rmask, gmask, bmask, amask;
	/* SDL interprets each pixel as a 32-bit number, so our masks must depend
	   on the endianness (byte order) of the machine */
#if SDL_BYTEORDER == SDL_BIG_ENDIAN
	rmask = 0xff000000;
	gmask = 0x00ff0000;
	bmask = 0x0000ff00;
	amask = 0x000000ff;
#else
	rmask = 0x000000ff;
	gmask = 0x0000ff00;
	bmask = 0x00ff0000;
	amask = 0xff000000;
#endif

	surface = SDL_CreateRGBSurface (0, 256 * FONT_WIDTH, NUM_COLORS * FONT_HEIGHT, 32, rmask, gmask, bmask, amask);
	assert (surface);

	for (int color = 0; color < NUM_COLORS; color++) {
		for (int c = 0; c < 256; c++) {
			Uint16 utf16[] = { codepage_437_to_utf16 [c], 0 };
			SDL_Surface *line_surface = TTF_RenderUNICODE_Solid (font, utf16, colors [color]);
			SDL_Rect src_rect = { 0, 0, FONT_WIDTH, FONT_HEIGHT };
			SDL_Rect dst_rect = { c * FONT_WIDTH, color * FONT_HEIGHT, FONT_WIDTH, FONT_HEIGHT };
			SDL_BlitSurface (line_surface, &src_rect, surface, &dst_rect);
			SDL_FreeSurface (line_surface);
		}
	}
	SDL_Texture *texture = SDL_CreateTextureFromSurface (renderer, surface);
	SDL_FreeSurface (surface);
	return texture;
}

static void
render_screen (void)
{
	for (int y = 1; y <= TEXT_ROWS; y++) {
		for (int x = 1; x <= TEXT_COLUMNS; x++) {
			CHARACTER *src = pcharPhysPage + (x-1 + (y-1) * iSizeX);
			attribute_t attr = src->attribute;
			SDL_Color bg = colors [attribute_background (attr)];
			SDL_SetRenderDrawColor (renderer, bg.r, bg.g, bg.b, bg.a);
			SDL_Rect dst_rect = { (x - 1) * FONT_WIDTH, (y - 1) * FONT_HEIGHT, FONT_WIDTH, FONT_HEIGHT };
			SDL_RenderFillRect (renderer, &dst_rect);
			int c = src->character & 0xff;
			SDL_Rect src_rect = { c * FONT_WIDTH, attribute_foreground (attr) * FONT_HEIGHT, FONT_WIDTH, FONT_HEIGHT };
			SDL_RenderCopy (renderer, font_texture, &src_rect, &dst_rect);
		}
	}

	if (iCursorPercent == 0)
		return;

	CHARACTER *cursor_char = pcharPhysPage + iCursorX-1 + (iCursorY-1) * iSizeX;
	SDL_Color cursor_color = colors [attribute_foreground (cursor_char->attribute)];
	int start_line = (100 - iCursorPercent) * FONT_HEIGHT / 100;
	SDL_Rect cursor_rect = { (iCursorX - 1) * FONT_WIDTH, (iCursorY - 1) * FONT_HEIGHT + start_line, FONT_WIDTH, FONT_HEIGHT - start_line };
	SDL_SetRenderDrawColor (renderer, cursor_color.r, cursor_color.g, cursor_color.b, cursor_color.a);
	SDL_RenderFillRect (renderer, &cursor_rect);
}

static void
set_redraw_if_phys (void)
{
	if (pcharVirPage == pcharPhysPage)
		need_redraw = TRUE;
}

void
vio_init (void)
{
	SDL_Init (SDL_INIT_TIMER | SDL_INIT_VIDEO);
	SDL_CreateWindowAndRenderer (WINDOW_WIDTH, WINDOW_HEIGHT, 0, &window, &renderer);
	TTF_Init();
	TTF_Font *font = TTF_OpenFont ("/Users/schani/Dropbox/Work/stools/Px437_IBM_VGA9.ttf", TTF_FONT_SIZE);
	if (font == NULL) {
		fprintf (stderr, "Error: font not found\n");
		exit(EXIT_FAILURE);
	}

	make_colors ();
	font_texture = make_font_texture (font);
	TTF_Quit();

	iSizeX = TEXT_COLUMNS;
	iSizeY = TEXT_ROWS;

	pcharVirPage = pcharPhysPage = malloc (sizeof (CHARACTER) * iSizeX * iSizeY);
	memset (pcharVirPage, 0, sizeof (CHARACTER) * iSizeX * iSizeY);

	pcharBuffer = malloc (sizeof (CHARACTER) * iSizeX * iSizeY);

	need_redraw = TRUE;
}

void
vio_redraw (void)
{
	if (!need_redraw)
		return;

	SDL_RenderClear (renderer);
	render_screen ();
	SDL_RenderPresent (renderer);

	need_redraw = FALSE;
}

WORD
vio_get_page (void)
{
	return 0;
}

void
vio_set_page (WORD wPage)
{
}

BYTE
vio_get_mode (void)
{
	return 3;
}

void
vio_set_mode (BYTE byMode)
{
}

BOOL
vio_color (void)
{
	return TRUE;
}

void
vio_set_vir_page (CHARACTER *pcharPage)
{
	pcharVirPage = pcharPage;
}

void
vio_set_phys_page (void)
{
	pcharVirPage = pcharPhysPage;
}

void
vio_get_cursor (INT *piX, INT *piY, INT *piPercent)
{
	*piX = iCursorX;
	*piY = iCursorY;
	*piPercent = iCursorPercent;
}

void
vio_set_cursor_pos (INT iX, INT iY)
{
	assert (iX > 0 && iX <= iSizeX);
	assert (iY > 0 && iY <= iSizeY);

	iCursorX = iX;
	iCursorY = iY;

	need_redraw = TRUE;
}

void
vio_set_cursor_type (INT iPercent)
{
	assert (iPercent >= 0 && iPercent <= 100);

	iCursorPercent = iPercent;

	need_redraw = TRUE;
}

CHAR
vio_l_z (INT iX, INT iY)
{
	return VIO_BUFFER_POS(iX, iY)->character;
}

CHAR
vio_l_a (INT iX, INT iY)
{
	return (CHAR)(VIO_BUFFER_POS(iX, iY)->character);
}

CHARACTER
vio_l_za (INT iX, INT iY)
{
	return *VIO_BUFFER_POS(iX, iY);
}

void
vio_s_z (INT iX, INT iY, CHAR cChar)
{
	CHARACTER *pchar = VIO_BUFFER_POS(iX, iY);
	pchar->character = cChar;

	set_redraw_if_phys ();
}

void
vio_s_a (INT iX, INT iY, CHAR cAttri)
{
	CHARACTER *pchar = VIO_BUFFER_POS(iX, iY);
	pchar->attribute = cAttri;

	set_redraw_if_phys ();
}

void
vio_s_za (INT iX, INT iY, CHAR cChar, CHAR cAttri)
{
	CHARACTER *pchar;

	pchar = VIO_BUFFER_POS(iX,iY);
	pchar->character = cChar;
	pchar->attribute = cAttri;

	set_redraw_if_phys ();
}

void
vio_ss (INT iX, INT iY, CHAR *pcString)
{
	CHARACTER *pchar;

	pchar = VIO_BUFFER_POS(iX,iY);
	for (; *pcString; pcString++, pchar++) {
		pchar->character = *pcString;
	}

	set_redraw_if_phys ();
}

void
vio_ss_a (INT iX, INT iY, CHAR *pcString, CHAR cAttri)
{
	CHARACTER *pchar;

	pchar = VIO_BUFFER_POS(iX,iY);
	for (; *pcString; pcString++, pchar++)
	{
		pchar->character = *pcString;
		pchar->attribute = cAttri;
	}

	set_redraw_if_phys ();
}

void
vio_sp_za (CHAR cChar, CHAR cAttri)
{
	DWORD dwCells;

	dwCells = iSizeX * iSizeY;

	INT        i;
	CHARACTER *pchar;

	pchar = pcharVirPage;
	for (i = 0; i < (INT)dwCells; i++, pchar++)
	{
		pchar->character = cChar;
		pchar->attribute = cAttri;
	}

	set_redraw_if_phys ();
}

void
vio_sp_z (CHAR cChar)
{
	DWORD dwCells;

	dwCells = iSizeX * iSizeY;

	INT        i;
	CHARACTER *pchar;

	pchar = pcharVirPage;
	for (i = 0; i < (INT)dwCells; i++, pchar++) {
		pchar->character = cChar;
	}

	set_redraw_if_phys ();
}

void
vio_sp_a (CHAR cAttri)
{
	DWORD dwCells;

	dwCells = iSizeX * iSizeY;
	INT        i;
	CHARACTER *pchar;

	pchar = pcharVirPage;
	for (i = 0; i < (INT)dwCells; i++, pchar++) {
		pchar->attribute = cAttri;
	}

	set_redraw_if_phys ();
}

void
vio_sw_za (INT iX, INT iY, INT iWidth, INT iHeight,
	   CHAR cChar, CHAR cAttri)
{
	INT        iJump,
		iCounterY;
	CHARACTER *pcharCounter,
		*pcharLimit;

	iJump = iSizeX - iWidth;
	pcharCounter = VIO_BUFFER_POS(iX, iY);
	for (iCounterY = 0; iCounterY < iHeight; iCounterY++)
	{
		pcharLimit = pcharCounter + iWidth;
		for (; pcharCounter < pcharLimit; pcharCounter++)
		{
			pcharCounter->character = cChar;
			pcharCounter->attribute = cAttri;
		}
		pcharCounter += iJump;
	}

	set_redraw_if_phys ();
}

void
vio_sw_z (INT iX, INT iY, INT iWidth, INT iHeight, CHAR cChar)
{
	INT        iJump,
		iCounterY;
	CHARACTER *pcharCounter,
		*pcharLimit;

	iJump = iSizeX - iWidth;
	pcharCounter = VIO_BUFFER_POS(iX, iY);
	for (iCounterY = 0; iCounterY < iHeight; iCounterY++)
	{
		pcharLimit = pcharCounter + iWidth;
		for (; pcharCounter < pcharLimit; pcharCounter++) {
			pcharCounter->character = cChar;
		}
		pcharCounter += iJump;
	}

	set_redraw_if_phys ();
}

void
vio_sw_a (INT iX, INT iY, INT iWidth, INT iHeight, CHAR cAttri)
{
	INT        iJump,
		iCounterY;
	CHARACTER *pcharCounter,
		*pcharLimit;

	iJump = iSizeX - iWidth;
	pcharCounter = VIO_BUFFER_POS(iX, iY);
	for (iCounterY = 0; iCounterY < iHeight; iCounterY++)
	{
		pcharLimit = pcharCounter + iWidth;
		for (; pcharCounter < pcharLimit; pcharCounter++) {
			pcharCounter->attribute = cAttri;
		}
		pcharCounter += iJump;
	}

	set_redraw_if_phys ();
}

void
vio_scroll_up (INT iX, INT iY, INT iWidth, INT iHeight, CHAR cAttri, INT iRows)
{
	if (iRows == 0 || iRows >= iHeight)
		vio_sw_za(iX, iY, iWidth, iHeight, ' ', cAttri);
	else
	{
		vio_lw(iX, iY + iRows, iWidth, iHeight - iRows, pcharBuffer);
		vio_sw(iX, iY, iWidth, iHeight - iRows, pcharBuffer);
		vio_sw_za(iX, iY + iHeight - iRows, iWidth, iRows, ' ', cAttri);
	}

	set_redraw_if_phys ();
}

void
vio_scroll_down (INT iX, INT iY, INT iWidth, INT iHeight, CHAR cAttri, INT iRows)
{
	if (iRows == 0 || iRows >= iHeight)
		vio_sw_za(iX, iY, iWidth, iHeight, ' ', cAttri);
	else
	{
		vio_lw(iX, iY, iWidth, iHeight - iRows, pcharBuffer);
		vio_sw(iX, iY + iRows, iWidth, iHeight - iRows, pcharBuffer);
		vio_sw_za(iX, iY, iWidth, iRows, ' ', cAttri);
	}

	set_redraw_if_phys ();
}

void
vio_lw (INT iX, INT iY, INT iWidth, INT iHeight, CHARACTER *pcharDest)
{
	INT        i;
	CHARACTER *pcharSource;

	pcharSource = VIO_BUFFER_POS(iX, iY);
	for (i = 0; i < iHeight; i++)
	{
		memcpy(pcharDest, pcharSource, iWidth * sizeof(CHARACTER));
		pcharSource += iSizeX;
		pcharDest += iWidth;
	}
}

void
vio_sw (INT iX, INT iY, INT iWidth, INT iHeight, CHARACTER *pcharSource)
{
	INT        i;
	CHARACTER *pcharDest;

	pcharDest = VIO_BUFFER_POS(iX, iY);
	for (i = 0; i < iHeight; i++)
	{
		memcpy(pcharDest, pcharSource, iWidth * sizeof(CHARACTER));
		pcharDest += iSizeX;
		pcharSource += iWidth;
	}

	set_redraw_if_phys ();
}

void vio_sw_ff (INT iX, INT iY, INT iWidth, INT iHeight, CHARACTER *pcharSource)
{
	INT        iCounterY,
		iJump;
	CHARACTER *pcharDest,
		*pcharLimit;

	iJump = iSizeX - iWidth;
	pcharDest = VIO_BUFFER_POS(iX, iY);
	for (iCounterY = 0; iCounterY < iHeight; iCounterY++)
	{
		pcharLimit = pcharDest + iWidth;
		for (; pcharDest < pcharLimit; pcharDest++, pcharSource++)
		{
			if (pcharSource->character != (CHAR)0xff) {
				pcharDest->character = pcharSource->character;
			}
			if (pcharSource->attribute != (CHAR)0xff) {
				pcharDest->attribute = pcharSource->attribute;
			}
		}
		pcharDest += iJump;
	}

	set_redraw_if_phys ();
}

static INT iLastMouseHor = 1, iLastMouseVer = 1;
static WORD wMouseButtons = 0;
UINT uiMouseSpeed = 0;

BOOL
msm_init (void)
{
	return TRUE;
}

BOOL
msm_cursor_on (void)
{
	return FALSE;
}

BOOL
msm_cursor_off (void)
{
	return FALSE;
}

BOOL
msm_cursor (void)
{
	return FALSE;
}

void
msm_get_buttons (WORD *pwButtons, INT *piHor, INT *piVer)
{
	*pwButtons = wMouseButtons;
	*piHor = iLastMouseHor;
	*piVer = iLastMouseVer;
}

void
msm_set_mickeys (WORD wHorMickeys, WORD wVerMickeys)
{
}

BYTE
utl_keyb_status (void)
{
	BYTE byReturnVar = 0;
	SDL_Keymod mod = SDL_GetModState ();

	if (mod & KMOD_ALT)
		byReturnVar |= ALT;
	if (mod & KMOD_CTRL)
		byReturnVar |= CTRL;
	if (mod & KMOD_SHIFT)
		byReturnVar |= SHIFT_RIGHT;
	if (mod & KMOD_NUM)
		byReturnVar |= NUM_LOCK;
	if (mod & KMOD_CAPS)
		byReturnVar |= CAPS_LOCK;
	// FIXME: INSERT !

	return byReturnVar;
}

static void
print_event (UTL_EVENT *ev)
{
	struct { UINT kind; const char *name; } names[] =
							{
								{ E_NULL, "null" },
								{ E_MSM_MOVE, "mouse move" },
								{ E_MSM_L_UP, "mouse left up" },
								{ E_MSM_L_DOWN, "mouse left down" },
								{ E_MSM_M_UP, "mouse middle up" },
								{ E_MSM_M_DOWN, "mouse middle down" },
								{ E_MSM_R_UP, "mouse right up" },
								{ E_MSM_R_DOWN, "mouse right down" },
								{ 0, NULL }
							};

	for (int i = 0; names [i].name; i++) {
		if (ev->uiKind == names [i].kind) {
			printf ("Event: %s\n", names [i].name);
			return;
		}
	}
	printf ("Event: UNKNOWN (%d)\n", (int)ev->uiKind);
}

void
utl_event (UTL_EVENT *peventEvent)
{
	SDL_Event event;
	int event_result;

	vio_redraw ();

	event_result = SDL_WaitEvent (&event);

	utl_fill_event (peventEvent);
	peventEvent->uiKind = E_NULL;

	if (!event_result)
		return;

	switch (event.type) {
		case SDL_MOUSEMOTION:
			peventEvent->iHor = event.motion.x / FONT_WIDTH + 1;
			peventEvent->iVer = event.motion.y / FONT_HEIGHT + 1;
			if (iLastMouseHor == peventEvent->iHor && iLastMouseVer == peventEvent->iVer)
				break;
			peventEvent->uiKind = E_MSM_MOVE;
			iLastMouseHor = peventEvent->iHor;
			iLastMouseVer = peventEvent->iVer;
			break;

		case SDL_MOUSEBUTTONDOWN:
			// FIXME: double clicks
			if (event.button.button == SDL_BUTTON_LEFT) {
				peventEvent->uiKind = E_MSM_L_DOWN;
				wMouseButtons |= MSM_B_LEFT;
			} else if (event.button.button == SDL_BUTTON_MIDDLE) {
				peventEvent->uiKind = E_MSM_M_DOWN;
				wMouseButtons |= MSM_B_MIDDLE;
			} else if (event.button.button == SDL_BUTTON_RIGHT) {
				peventEvent->uiKind = E_MSM_R_DOWN;
				wMouseButtons |= MSM_B_RIGHT;
			} else {
				assert (FALSE);
			}
			peventEvent->wButtons = wMouseButtons;
			break;

		case SDL_MOUSEBUTTONUP:
			if (event.button.button == SDL_BUTTON_LEFT) {
				peventEvent->uiKind = E_MSM_L_UP;
				wMouseButtons &= ~MSM_B_LEFT;
			} else if (event.button.button == SDL_BUTTON_MIDDLE) {
				peventEvent->uiKind = E_MSM_M_UP;
				wMouseButtons &= ~MSM_B_MIDDLE;
			} else if (event.button.button == SDL_BUTTON_RIGHT) {
				peventEvent->uiKind = E_MSM_R_UP;
				wMouseButtons &= ~MSM_B_RIGHT;
			} else {
				assert (FALSE);
			}
			break;

		case SDL_QUIT:
			// FIXME: handle properly
			SDL_DestroyRenderer(renderer);
			SDL_DestroyWindow(window);
			SDL_Quit();
			exit (0);
	}

	/*
	if (peventEvent->uiKind != E_NULL)
		print_event(peventEvent);
	*/
}

/*
int main(int argc, char **argv) {
	SDL_Event event;
	int quit;

	vio_init ();

	quit = 0;
	int bg = 0;
	while (!quit) {
		while (SDL_PollEvent(&event) == 1) {
			if (event.type == SDL_QUIT) {
				quit = 1;
			}
		}

		for (int i = 0; i < NUM_COLORS; i++)
			vio_ss_a (1, i + 1, "01234567890123456789012345678901234567890123456789012345678901234567890123456789", vio_attri (i, bg));
		bg = (bg + 1) & (NUM_COLORS - 1);

		vio_redraw ();
	}

	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	SDL_Quit();
	return EXIT_SUCCESS;
}
*/
