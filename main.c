#include <stdlib.h>
#include <signal.h>
#include <curses.h>
#include <stdio.h>
#include <unistd.h>

// The current Workspace, 0 for dlx-editor, 1 for memory,2 for register
static int current_WS = 0;
// Character that represents the cursor.
static const char cursor = 0x2588;

/**
 * @brief      Draw's boarder around a ncurses WINDOW
 *
 * @details    Draw a Ascii boarder around your ncurses Window. Uses '|','-' and '+'
 *
 * @param      WINDOW * 
 *
 * @return     void
 */
void draw_borders(WINDOW *screen) {
  int x, y, i;

  getmaxyx(screen, y, x);

  // 4 corners
  mvwprintw(screen, 0, 0, "+");
  mvwprintw(screen, y - 1, 0, "+");
  mvwprintw(screen, 0, x - 1, "+");
  mvwprintw(screen, y - 1, x - 1, "+");

  // sides
  for (i = 1; i < (y - 1); i++) {
    mvwprintw(screen, i, 0, "|");
    mvwprintw(screen, i, x - 1, "|");
  }

  // top and bottom
  for (i = 1; i < (x - 1); i++) {
    mvwprintw(screen, 0, i, "-");
    mvwprintw(screen, y - 1, i, "-");
  }
}

int main(int argc, char *argv[]) {
  if (initscr() == NULL){
    perror("Error initialising ncurses");
    exit(EXIT_FAILURE);
  }

  int parent_x, parent_y, new_x, new_y;
  int register_size = 10;
  int memory_size = 130;

  noecho();
  curs_set(FALSE);

  // set up initial windows
  getmaxyx(stdscr, parent_y, parent_x);

  // Window for our Editor
  WINDOW *editor_window = newwin(parent_y - register_size, parent_x - memory_size, 0, 0);
  // Window for our Registers
  WINDOW *register_window = newwin(register_size, parent_x, parent_y - register_size, 0);
  // Window for Memory locations
  WINDOW *memory_window = newwin(parent_y - register_size, memory_size, 0 , parent_x - memory_size);

  draw_borders(editor_window);
  draw_borders(register_window);
  draw_borders(memory_window);

  //Main loop
  while(1) {
    getmaxyx(stdscr, new_y, new_x);

    // resiye grahics if the terminal changed size
    if (new_y != parent_y || new_x != parent_x) {
      parent_x = new_x;
      parent_y = new_y;

      wresize(editor_window, new_y - register_size, new_x - memory_size);
      wresize(register_window, register_size, new_x);
      mvwin(register_window, new_y - register_size, 0);
      wresize(memory_window, new_y - register_size, memory_size);
      mvwin(memory_window, 0, new_x - memory_size);

      wclear(stdscr);
      wclear(editor_window);
      wclear(register_window);
      wclear(memory_window);

      draw_borders(editor_window);
      draw_borders(register_window);
      draw_borders(memory_window);
    }


    // draw to our windows
    int row = 1;
    int column = 0;
    for (int i =1; i <= 32; i++) {
      mvwprintw(register_window, row * 2, (column * 23) + 1, "R[%d]" ,i);
      if (i % 4 == 0){
	column = column + 1;
	row = 0;
      }
      row++;
    }

    mvwprintw(editor_window, 1, 1, "Editor");

    row = 1;
    column = 0;
    for (int i = 1 ; i <= 400; i++){
      mvwprintw(memory_window, row, (column * 11) +1, "%x",0x1000 + 4*i);
      if (i % 40 == 0){
	column = column + 1;
	row = 0;
      }
      row ++;
    }

    // refresh each window
    wrefresh(editor_window);
    wrefresh(register_window);
    wrefresh(memory_window);
  }

  endwin();

  return 0;
}
