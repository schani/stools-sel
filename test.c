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

#define M_HELP             2000
#define M_HELP_HELP        2001
#define M_HELP_ABOUT       2002

#define BUFFER_SIZE        8192

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
                /* sts_new_status_line(aitemHelpItems); */
                peventEvent->uiKind = E_DONE;
                break;
            case M_LOST_FOCUS :
                /* sts_del_status_line(); */
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
                                              mnu_new_item(M_FILE_QUIT, "#Quit   Alt-X", NULL, K_A_X, 0, NULL, NULL))))))))),
                                 mnu_new_item(M_HELP, "#Help", NULL, 0, 0, mnu_new_window(
                                              mnu_new_item(M_HELP_HELP, "#Help...   F1", NULL, K_F1, 0, NULL,
                                              mnu_new_line(
                                              mnu_new_item(M_HELP_ABOUT, "#About...", NULL, 0, 0, NULL, NULL)))),
                                              NULL)));
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

void window_handler (WINDOW win, UTL_EVENT *pevent)
{
  switch (pevent->uiKind)
  {
    case E_MESSAGE :
      switch (pevent->uiMessage)
      {
        case M_DRAW :
          dlg_border_set_display(win_get_element(win, 1000), 1, 1, win->iWidth - 2, win->iHeight - 2);
          break;
      }
      break;
  }
  if (pevent->uiKind != E_DONE)
    win_std_handler(win, pevent);
}   

int main (void)
{
  WINDOW       winWindow;
  DLG_BUTTON   abut[]    = 
                           {
                             { { 3, 7 }, 0, "Radio #1" },
                             { { 3, 8 }, 0, "Radio #2" },
                             { { 3, 9 }, 0, "Radio #3" },
                             DLG_RADIO_END
               };
  DLG_ELEMENT *pelement;
  CHAR         acInput[]  = "This is a text!",
               acName[40] = "*.*",
              *pcBuffer,
              *apcList[]  = 
                            {
                              "List-Box Item 1",
                              "List-Box Item 2",
                              "List-Box Item 3",
                              "List-Box Item 4",
                              "List-Box Item 5",
                              "List-Box Item 6",
                              "List-Box Item 7",
                              "List-Box Item 8",
                              "List-Box Item 9",
                              "List-Box Item 10",
                              "List-Box Item 11",
                              "List-Box Item 12",
                              "List-Box Item 13",
                              "List-Box Item 14",
                              "List-Box Item 15",
                              "List-Box Item 16",
                              "List-Box Item 17",
                              "List-Box Item 18",
                              "List-Box Item 19",
                              "List-Box Item 20",
                              "List-Box Item 21",
                              "List-Box Item 22",
                              "List-Box Item 23",
                              "List-Box Item 24",
                              "List-Box Item 25",
                              "List-Box Item 26",
                              "List-Box Item 27",
                              "List-Box Item 28",
                              NULL
                            };


  glb_init(program_handler);

#if 0
  winWindow = win_new(10, 5, 40, 10, "standard_dialogue", 0);
  win_add_element(winWindow, dlg_init_act_button(3, 3, "OK", K_ENTER, NULL, 101, TRUE, NULL));
  win_add_element(winWindow, dlg_init_act_button(11, 3, "Abbruch", K_ESC, NULL, 102, TRUE, NULL));
  win_add_element(winWindow, dlg_init_push_button(3, 5, "#Push-Button", FALSE, NULL, 103, TRUE, NULL));
  win_add_element(winWindow, dlg_init_radio_button(abut, 0, NULL, 104, TRUE, NULL));
  pelement = dlg_init_scroll(5, 12, 20, 0, DLG_HORIZONTAL, NULL, 105, TRUE, NULL);
  win_add_element(winWindow, dlg_init_label(3, 11, "#Scroll-Bar", pelement, 106));
  win_add_element(winWindow, pelement);
  pelement = dlg_init_text_field(5, 15, 20, 50, FALSE, acInput, NULL, 107, TRUE, NULL);
  win_add_element(winWindow, dlg_init_label(3, 14, "#Text-Field", pelement, 108));
  win_add_element(winWindow, pelement);
  pelement = dlg_init_list_box(35, 4, 30, 12, apcList, TRUE, NULL, 109, TRUE, NULL);
  win_add_element(winWindow, dlg_init_label(33, 3, "#List-Box", pelement, 110));
  win_add_element(winWindow, pelement);

  //win_add_element(winWindow, dlg_init_border(1, 1, 38, 8, 1000));
  win_sizeable(winWindow, TRUE);
  win_cursor(winWindow, TRUE);
  win_show(winWindow);
#endif

    glb_run();
    return 0;
}

/*
Local Variables:
compile-command: "wmake -f test.mk -h -e"
End:
*/
