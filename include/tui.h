// Copyright [2024]
// Gautam Singh
// Rishab P. Hariharan
#include <ncurses.h>
#pragma once
#ifndef INCLUDE_TUI_H_
#define INCLUDE_TUI_H_

void begin();
void loading_bar();
void clear_line();
void slow_print(char *str, int n);
WINDOW *create_newwin(int height, int width, int starty, int startx);
void destroy_win(WINDOW *local_win);
void home();
#endif // !INCLUDE_TUI_H_
