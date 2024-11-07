// Copyright [2024]
// Gautam Singh
// Rishab P. Hariharan

#include "../include/tui.h"
#include <locale.h>
#include <ncurses.h>
#include <stdarg.h>
#include <stddef.h>
#include <string.h>
#include <unistd.h>

#define EXIT_SUCCESS 0

int main(int argc, char *argv[]) {
  setlocale(LC_ALL, "");
  // init ncurses
  initscr();
  cbreak();
  noecho();
  keypad(stdscr, TRUE);

  curs_set(0);
  begin();

  endwin();

  return EXIT_SUCCESS;
}

void clear_line() {
  int y;
  y = getcury(stdscr);
  move(y, 0);
  clrtoeol();
}

void loading_bar() {
  int width, y;
  width = getmaxx(stdscr) - 15;
  y = getcury(stdscr);
  for (int i = 0; i < width; ++i) {
    mvprintw(y, 0, "[%d %% loading] ", i * 100 / width);
    for (int j = 0; j < i; ++j) {
      printw("█");
      usleep(100);
    }
    refresh();
    clear_line();
  }
}

void slow_print(char *str, int n) {
  for (int i = 0; i < strlen(str); i++) {
    printw("%c", str[i]);
    usleep(n);
    refresh();
  }
}

void begin() {
  slow_print("Starting library system.\n", 1000);
  loading_bar();
  clear();
  refresh();
  slow_print(R"EOF(
      _/  _/  _/
     _/      _/_/_/    _/  _/_/    _/_/_/  _/  _/_/  _/    _/
    _/  _/  _/    _/  _/_/      _/    _/  _/_/      _/    _/  _/_/_/_/_/
   _/  _/  _/    _/  _/        _/    _/  _/        _/    _/
  _/  _/  _/_/_/    _/          _/_/_/  _/          _/_/_/
                                                       _/
                                                  _/_/

                                      _/
       _/_/_/  _/    _/    _/_/_/  _/_/_/_/    _/_/    _/_/_/  _/_/
    _/_/      _/    _/  _/_/        _/      _/_/_/_/  _/    _/    _/
       _/_/  _/    _/      _/_/    _/      _/        _/    _/    _/
  _/_/_/      _/_/_/  _/_/_/        _/_/    _/_/_/  _/    _/    _/
                 _/
            _/_/

  )EOF",
             1000);
  sleep(1);
  clear();
  refresh();
}

void home() {}
