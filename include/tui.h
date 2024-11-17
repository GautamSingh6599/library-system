// Copyright [2024]
// Gautam Singh

#include "./book.h"
#include <locale.h>
#include <ncurses.h>
#include <stdio.h>
#include <string.h>
#include <wchar.h>
#pragma once
#ifndef INCLUDE_TUI_H_
#define INCLUDE_TUI_H_

void print_header(WINDOW *win, int id_width, int isbn_width, int title_width,
                  int author_width, int copies_width);
void print_footer(WINDOW *win);
void show_error_message(WINDOW *win, const char *message);
void display_books(WINDOW *win, Book *library, int MAX_BOOKS);
void print_book(WINDOW *win, int row, Book book, int highlight, int id_width,
                int isbn_width, int title_width, int author_width,
                int copies_width, Book *library);

#endif
