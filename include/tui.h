#include "./book.h"
#include <locale.h>
#include <ncurses.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <wchar.h>

#pragma once
#ifndef INCLUDE_TUI_H_
#define INCLUDE_TUI_H_

void print_header(WINDOW *win, int id_width, int isbn_width, int title_width,
                  int author_width, int copies_width);
void print_footer(WINDOW *win, char *user, int logged_in);
void show_error_message(WINDOW *win, const char *message);
void display_books(WINDOW *win, Book *library, int MAX_BOOKS, char *user,
                   int logged_in);
void print_book(WINDOW *win, int row, Book book, int highlight, int id_width,
                int isbn_width, int title_width, int author_width,
                int copies_width, Book *library);
void update_highlight(WINDOW *win, int highlight, int prev_highlight,
                      Book *library);
void issue_tui(Book book, char *username, int user_type, WINDOW *win,
               int logged_in);

void remove_book(WINDOW *win, Book book);
void edit_book(WINDOW *win, Book book);
#endif
