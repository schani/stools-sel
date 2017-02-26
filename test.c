#include <stools.h>
#include <string.h>
#include <stdio.h>

extern int sel_run (const char *program);

#define MENU_MAIN   1

#define M_FILE             1000
#define M_FILE_NEW         1001
#define M_FILE_OPEN        1002
#define M_FILE_SAVE        1003
#define M_FILE_SAVE_AS     1004
#define M_FILE_RUN         1005
#define M_FILE_QUIT        1006

#define M_SETTINGS         2000
#define M_SETTINGS_PALETTE 2001

#define M_HELP             3000
#define M_HELP_HELP        3001
#define M_HELP_ABOUT       3002

#define BUFFER_SIZE        8192

static STS_ITEM aitemEditorItems[] =
{
    {
        "~Alt-R~ Run",
        M_FILE_RUN,
        K_A_R,
        TRUE
    },
    {
        "~Alt-S~ Save",
        M_FILE_SAVE,
        K_A_S,
        TRUE
    },
    STS_END
};

typedef struct {
    char *dir;
    char *filename;
} edit_data_t;

static FILE*
open_file (const char *filename, const char *mode)
{
    FILE *f = fopen(filename, mode);
    if (f)
        return f;
    box_info(BOX_ERROR, BOX_OK, "Could not open file", 0);
    return NULL;
}

static FILE*
load_save (const char *mode, edit_data_t *data)
{
    CHAR filename[256] = "*.sel";
    if (box_load_save("Open", filename) != BOX_OK)
        return NULL;

    FILE *f = open_file(filename, mode);
    if (!f)
        return NULL;

    char *dir = utl_alloc(128);
    utl_get_path(dir);
    char *fn = utl_alloc(strlen(filename) + 1);
    strcpy(fn, filename);
    data->dir = dir;
    data->filename = fn;
    return f;
}

static edit_data_t*
alloc_edit_data (void)
{
    edit_data_t *data = utl_alloc(sizeof(edit_data_t));
    memset(data, 0, sizeof(edit_data_t));
    return data;
}

static void
free_edit_data (edit_data_t *data)
{
    utl_free(data->dir);
    utl_free(data->filename);
    utl_free(data);
}

static void
code_editor_callback (WINDOW win, UTL_EVENT *peventEvent)
{
    DLG_ELEMENT *editor = win_get_element(win, BOX_EDIT_EDIT);
    edit_data_t *data = (edit_data_t*)win_get_add_info(win);
    
    if (peventEvent->uiKind == E_MESSAGE)
    {
        switch (peventEvent->uiMessage)
        {
            case M_DRAW :
            case M_REDRAW :
                dlg_editor_set_display(editor, 0, 0, win->iWidth, win->iHeight);
                win_draw_elements(win);
            case M_INIT :
                peventEvent->uiKind = E_DONE;
                break;
            case M_SHOW :
            case M_ACTIVATE :
                win_set_focus(win);
                peventEvent->uiKind = E_DONE;
                break;
            case M_GET_FOCUS :
                win_show(win);
                sts_new_status_line(aitemEditorItems);
                peventEvent->uiKind = E_DONE;
                break;
            case M_LOST_FOCUS :
                sts_del_status_line();
                peventEvent->uiKind = E_DONE;
                break;
            case M_QUIT :
                peventEvent->uiKind = E_DONE;
                break;
            case M_CLOSE :
                win_delete(win);
                peventEvent->uiKind = E_DONE;
                break;
            case M_FILE_SAVE: {
                if (!data)
                    goto save_as;
                utl_set_path(data->dir);
                FILE *f = open_file(data->filename, "w");
                if (f) {
                    CHAR *text = dlg_editor_get_text(editor);
                    size_t len = strlen(text);
                    if (fwrite(text, 1, len, f) != len)
                        box_info(BOX_ERROR, BOX_OK, "Could not save file", 0);
                    fclose(f);
                    utl_free(text);
                }
                break;
            }
            case M_FILE_SAVE_AS: {
                edit_data_t *new_data;
                if (data)
                    utl_set_path(data->dir);
            save_as:
                new_data = alloc_edit_data();
                FILE *f = load_save("w", new_data);
                if (f) {
                    CHAR *text = dlg_editor_get_text(editor);
                    size_t len = strlen(text);
                    if (data)
                        free_edit_data(data);
                    win_set_add_info(win, (ULONG)new_data);
                    win_title(win, new_data->filename, TITLE_T_C);
                    if (fwrite(text, 1, len, f) != len)
                        box_info(BOX_ERROR, BOX_OK, "Could not save file", 0);
                    fclose(f);
                    utl_free(text);
                } else {
                    free_edit_data(new_data);
                }
                break;
            }
            case M_FILE_RUN: {
                CHAR *text = dlg_editor_get_text(editor);
                if (data)
                    utl_set_path(data->dir);
                CHARACTER *buffer = utl_alloc(sizeof(CHARACTER) * iSizeX * iSizeY);
                vio_lw(1, 1, iSizeX, iSizeY, buffer);
                vio_sp_za(' ', vio_attri(7, 0));
                int result = sel_run(text);
                utl_free(text);
                printf("result: %d\n", result);
                vio_sw(1, 1, iSizeX, iSizeY, buffer);
                break;
            }
        }
    }
}

static BOOL
make_editor (CHAR *pcBuffer, edit_data_t *data)
{
    WINDOW winWindow;
    CHAR *pcTitle = data ? data->filename : "untitled";
    
    if (!(winWindow = win_new(10, 2, 66, 21, "code_editor", (ULONG)data)))
        return FALSE;
    if (!win_title(winWindow, pcTitle, TITLE_T_C))
        return FALSE;
    win_set_size_limits(winWindow, 20, 5, iSizeX, iSizeY - 2);
    win_add_element(winWindow, dlg_init_editor(0, 0, 66, 21, BUFFER_SIZE, pcBuffer, TRUE, FALSE, NULL,
                                               BOX_EDIT_EDIT, TRUE, NULL));
    win_show(winWindow);
    return TRUE;
}

static void
program_handler (GLB_PROGRAM *pprogProgram, UTL_EVENT *peventEvent)
{
    switch (peventEvent->uiKind) {
        case E_MESSAGE:
            switch (peventEvent->uiMessage) {
                case M_INIT:
                    win_register_class("code_editor", PAL_WIN_DLG, B_EEEE, TITLE_T_C,
                                       WIN_FL_SHADOW | WIN_FL_CURSOR | WIN_FL_MOVEABLE | WIN_FL_SIZEABLE | WIN_FL_CLOSEABLE |
                                       WIN_FL_FOCUSABLE, code_editor_callback);
                    
                    hlp_open_file("/Users/schani/Dropbox/Work/stools/stools.hlp");
                    
                    mnu_new_menu(MENU_MAIN, 1,
                                 mnu_new_item(M_FILE, "#File", NULL, 0, 0, mnu_new_window(
                                              mnu_new_item(M_FILE_NEW, "#New...", NULL, 0, 0, NULL,
                                              mnu_new_item(M_FILE_OPEN, "#Open...", NULL, 0, 0, NULL,
                                              mnu_new_item(M_FILE_SAVE, "#Save   Alt-S", NULL, K_A_S, 0, NULL,
                                              mnu_new_item(M_FILE_SAVE_AS, "Save #as...", NULL, 0, 0, NULL,
                                              mnu_new_line(
                                              mnu_new_item(M_FILE_RUN,  "#Run    Alt-R", NULL, K_A_R, 0, NULL,
                                              mnu_new_line(
                                              mnu_new_item(M_FILE_QUIT, "#Quit   Alt-X", NULL, K_A_X, 0, NULL,
                                              NULL))))))))),
                                 mnu_new_item(M_SETTINGS, "#Settings", NULL, 0, 0, mnu_new_window(
                                              mnu_new_item(M_SETTINGS_PALETTE, "#Palette...", NULL, 0, 0, NULL,
                                              NULL)),
                                 mnu_new_item(M_HELP, "#Help", NULL, 0, 0, mnu_new_window(
                                              mnu_new_item(M_HELP_HELP, "#Help...   F1", NULL, K_F1, 0, NULL,
                                              mnu_new_line(
                                              mnu_new_item(M_HELP_ABOUT, "#About...", NULL, 0, 0, NULL, NULL)))),
                                              NULL))));
                    break;
                
                case M_FILE_NEW: {
                    CHAR *pcBuffer = utl_alloc(BUFFER_SIZE);
                    strcpy(pcBuffer, "let main x =\n  x\nend");
                    make_editor(pcBuffer, NULL);
                    break;
                }
                    
                case M_FILE_OPEN: {
                    edit_data_t *data = alloc_edit_data();
                    FILE *f = load_save("r", data);
                    if (f) {
                        CHAR *buffer = utl_alloc(BUFFER_SIZE);
                        size_t num_read = fread(buffer, 1, BUFFER_SIZE - 1, f);
                        buffer[num_read] = 0;
                        make_editor(buffer, data);
                        fclose(f);
                    } else {
                        free_edit_data(data);
                    }
                    break;
                }
                    
                case M_FILE_QUIT:
                    bExit = TRUE;
                    break;
                    
                case M_SETTINGS_PALETTE:
                    box_palette();
                    break;
                    
                case M_HELP_ABOUT:
                    box_info(BOX_INFO, BOX_OK, "SEL IDE with STools", 0);
                    break;
                    
                case M_HELP_HELP:
                    box_help(CTX_BOX_HELP);
                    break;
                    
                default:
                    return;
            }
        case E_KEY :
            switch (peventEvent->wKey) {
                    /*
                case K_A_X :
                    bExit = TRUE;
                    peventEvent->uiKind = E_DONE;
                    break;
                     */
                    
                default:
                    return;
            }
            break;
        default:
            return;
    }
    peventEvent->uiKind = E_DONE;
}

int main (void)
{
    glb_init(program_handler);
    glb_run();
    return 0;
}
