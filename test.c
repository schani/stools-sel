#include <stools.h>
#include <string.h>

void program_handler (GLB_PROGRAM *pprogProgram, UTL_EVENT *peventEvent)
{
  switch (peventEvent->uiKind)
  {
    case E_KEY :
      switch (peventEvent->wKey)
      {
        case K_A_X :
          bExit = TRUE;
          peventEvent->uiKind = E_DONE;
          break;
      }
      break;
  }
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

/*
  hlp_open_file("d:\\programm\\bc31\\stools\\stools.hlp");
  box_help(CTX_BOX_HELP);
  pcBuffer = utl_alloc(8192);
  strcpy(pcBuffer, "This is a demo text!");
  box_edit(" Edit ", pcBuffer, 8192, TRUE, 0);
  glb_run();
*/


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

  win_add_element(winWindow, dlg_init_border(1, 1, 38, 8, 1000));
  win_sizeable(winWindow, TRUE);
  win_cursor(winWindow, TRUE);
  win_show(winWindow);
  glb_run();


  return 0;
}

/*
Local Variables:
compile-command: "wmake -f test.mk -h -e"
End:
*/
