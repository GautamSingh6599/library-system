#pragma once
#ifndef INCLUDE_TUI_H_
#define INCLUDE_TUI_H_
#include "../include/issuing.h"
#include "./book.h"
#include <locale.h>
#include <ncurses.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <wchar.h>
void print_header(WINDOW *win, int id_width, int isbn_width, int title_width,
                  int author_width, int copies_width);

void print_issued_header(WINDOW *win, int isbn_width, int date_width);

void print_footer(WINDOW *win, char *user, int logged_in);
void admin_print_footer(WINDOW *win, char *user, int logged_in);
void show_error_message(WINDOW *win, const char *message);
void print_book(WINDOW *win, int row, Book book, int highlight, int id_width,
                int isbn_width, int title_width, int author_width,
                int copies_width, Book *library);
void print_issued_book(WINDOW *win, int row, IssuedBook book, int highlight,
                       int isbn_width, int date_width, IssuedBook *library);
void display_issued_books(WINDOW *win, IssuedBook *library, int MAX_BOOKS,
                          char *user, int logged_in);
void update_issued_highlight(WINDOW *win, int highlight, int prev_highlight,
                             IssuedBook *library);
void update_books(WINDOW *win, Book *library, int MAX_BOOKS);
void display_books(WINDOW *win, Book *library, int MAX_BOOKS, char *user,
                   int logged_in);
void display_books_admin(WINDOW *win, Book *library, int MAX_BOOKS, char *user,
                         int logged_in);
void update_highlight(WINDOW *win, int highlight, int prev_highlight,
                      Book *library);
void command_mode(WINDOW *win, char *user, int logged_in, int user_type);
void command_mode_admin(WINDOW *win, char *user, int logged_in);
void login_tui(WINDOW *win, int *logged_in, char *user, int *user_type);
void log_out(WINDOW *win, char *user, int *logged_in, int *user_type);
void signup_tui(WINDOW *win, char *user, int *logged_in, int *user_type);
void issue_tui(Book book, char *username, int user_type, WINDOW *win,
               int logged_in);
void add_books(WINDOW *win);
void remove_book(WINDOW *win, Book book);
void edit_book(WINDOW *win, Book book);
void admin_mode(char *x, char *user, int user_type);
void chat(WINDOW *win, char *user);
#endif
