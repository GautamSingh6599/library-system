#include "../include/tui.h"
#include "../include/accounts.h"
#include "../include/book.h"
#include "../include/bookrecords.h"
#include "../include/chatbot.h"
#include "../include/issuing.h"
#include <ncurses.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

// Function to print the header with dynamic column widths
void print_header(WINDOW *win, int id_width, int isbn_width, int title_width,
                  int author_width, int copies_width) {
  wattron(win, A_REVERSE | A_BOLD);
  mvwprintw(win, 0, 0, " %-*s %-*s %-*s %-*.*s %-*.*s ", id_width, "ID",
            isbn_width, "ISBN", copies_width, "Copies", title_width,
            title_width, "Title", author_width, author_width, "Author");
  wattroff(win, A_REVERSE | A_BOLD);
}

void print_issued_header(WINDOW *win, int isbn_width, int date_width) {
  wattron(win, A_REVERSE | A_BOLD);
  mvwprintw(win, 0, 0, " %-*s %-*s ", isbn_width, "ISBN", date_width,
            "Due Date");
  wattroff(win, A_REVERSE | A_BOLD);
}

// Function to print footer
void print_footer(WINDOW *win, char *user, int logged_in) {
  wattron(win, A_REVERSE | A_BOLD);
  int x, y;
  getmaxyx(stdscr, y, x);
  mvwprintw(win, y - 1, 0, " %-*s ", x - 2,
            "CHAT[?] EXIT[q] NEXTPAGE[n] PREVPAGE[N] SCROLLUP[k/UP] "
            "SCROLLDOWN[j/DOWN] SEARCH[:] LOGIN[l] SIGNUP[A] LOGOUT[X] "
            "ISSUE[i] RETURN[r] LIST[Y] ADMIN_MODE[a]");
  if (logged_in == 1) {
    mvwprintw(win, LINES - 1, COLS - 4 - strlen(user), " [%s] ", user);
    wrefresh(win);
  }
  wattroff(win, A_REVERSE | A_BOLD);
  wrefresh(win);
}

void admin_print_footer(WINDOW *win, char *user, int logged_in) {
  wattron(win, A_REVERSE | A_BOLD);
  int x, y;
  getmaxyx(stdscr, y, x);
  mvwprintw(
      win, y - 1, 0, " %-*s ", x - 2,
      "[ADMIN MODE] ADD_BOOK[A] REMOVE_BOOK[R] EDIT_BOOK[E] SEARCH[:] EXIT[X]");
  if (logged_in == 1) {
    mvwprintw(win, LINES - 1, COLS - 4 - strlen(user), " [%s] ", user);
    wrefresh(win);
  }
  wattroff(win, A_REVERSE | A_BOLD);
  wrefresh(win);
}

// Function to display error message in popup
void show_error_message(WINDOW *win, const char *message) {
  int rows, columns;
  getmaxyx(win, rows, columns);

  int msg_len = strlen(message);
  int msg_width = msg_len + 2;
  // Fixed width for error window
  int msg_heigth = 5;

  if (msg_width > columns) {
    msg_width = columns - 2;
  }

  // Centering
  int starty = (rows - msg_heigth) / 2;
  int startx = (columns = msg_width) / 2;

  // Create a new window for the error message
  WINDOW *error_win = newwin(msg_heigth, msg_width, starty, startx);
  box(error_win, 0, 0);

  // Display the error message in the middle of the window
  wattron(error_win, A_BOLD | A_STANDOUT);
  mvwprintw(error_win, 2, 2, "%.*s", msg_width - 4, message);
  wattroff(error_win, A_BOLD | A_STANDOUT);

  // Refresh the window to display the content
  wrefresh(error_win);

  // Wait for the user to press a key
  wgetch(error_win);

  // Clean up the window
  delwin(error_win);
}

void print_book(WINDOW *win, int row, Book book, int highlight, int id_width,
                int isbn_width, int title_width, int author_width,
                int copies_width, Book *library) {
  if (highlight) {
    wattron(win, A_STANDOUT);
  }
  mvwprintw(win, row, 0, " %-*lld %-*lld %-*d %-*.*ls %-*.*ls ", id_width,
            book.id, isbn_width, book.isbn, copies_width, book.copies,
            title_width, title_width, book.title, author_width, author_width,
            book.author);
  if (highlight) {
    wattroff(win, A_STANDOUT);
  }
}

void print_issued_book(WINDOW *win, int row, IssuedBook book, int highlight,
                       int isbn_width, int date_width, IssuedBook *library) {
  if (highlight) {
    wattron(win, A_STANDOUT);
  }
  mvwprintw(win, row, 0, " %-*s %-*s ", isbn_width, book.isbn, date_width,
            book.date);
  if (highlight) {
    wattroff(win, A_STANDOUT);
  }
}

void display_issued_books(WINDOW *win, IssuedBook *library, int MAX_BOOKS,
                          char *user, int logged_in) {
  werase(win);
  wrefresh(win);
  int term_cols, term_rows;
  getmaxyx(win, term_rows, term_cols);
  if (term_cols < 30 || term_rows < 3) {
    char *msg;
    asprintf(&msg, "Size of the terminal too small. x = %d, y = %d", term_cols,
             term_rows);
    show_error_message(stdscr, msg);
  }

  int isbn_width, date_width;
  isbn_width = 20;
  date_width = COLS - 23;
  print_issued_header(win, isbn_width, date_width);

  int row = 1;
  for (int i = 0; i < MAX_BOOKS; i++) {
    print_issued_book(win, row, library[i], 0, isbn_width, date_width, library);
    row++;
  }

  print_footer(win, user, logged_in);

  wmove(win, 1, 0);
  wrefresh(win);
  doupdate();
}

void update_issued_highlight(WINDOW *win, int highlight, int prev_highlight,
                             IssuedBook *library) {
  int row;
  int term_cols, term_rows;
  getmaxyx(win, term_rows, term_cols);
  if (term_cols < 30 || term_rows < 3) {
    char *msg;
    asprintf(&msg, "Size of the terminal too small. x = %d, y = %d", term_cols,
             term_rows);
    show_error_message(stdscr, msg);
  }

  int isbn_width, date_width;
  isbn_width = 20;
  date_width = COLS - 23;
  print_issued_header(win, isbn_width, date_width);

  if (highlight == -1 && prev_highlight == -1) {
    return;
  }

  if (prev_highlight != -1) {
    row = prev_highlight + 1;

    print_issued_book(win, row, library[prev_highlight], 0, isbn_width,
                      date_width, library);
  }

  row = highlight + 1;
  print_issued_book(win, row, library[highlight], 1, isbn_width, date_width,
                    library);

  // Highlight current book
  // Refresh the screen efficiently
  wnoutrefresh(win);
  doupdate();
}

void update_books(WINDOW *win, Book *library, int MAX_BOOKS) {
  werase(win);
  wrefresh(win);
  int term_cols, term_rows;
  getmaxyx(win, term_rows, term_cols);
  if (term_cols < 30 || term_rows < 3) {
    char *msg;
    asprintf(&msg, "Size of the terminal too small. x = %d, y = %d", term_cols,
             term_rows);
    show_error_message(stdscr, msg);
  }

  int id_width, isbn_width, title_width, author_width, copies_width;
  id_width = 6;
  isbn_width = 10;
  copies_width = 6;
  title_width = 0;
  author_width = 0;
  int remaining =
      COLS - ((id_width + 2) + (isbn_width + 2) + (copies_width + 2));
  if (remaining < 80) {
    title_width = remaining;
  } else {
    title_width = 80;
    author_width = getmaxx(win) -
                   ((id_width + 2) + (isbn_width + 2) + (copies_width + 2) +
                    (title_width + 2)) +
                   2;
  }

  print_header(win, id_width, isbn_width, title_width, author_width,
               copies_width);

  int row = 1;
  for (int i = 0; i < MAX_BOOKS; i++) {
    print_book(win, row, library[i], 0, id_width, isbn_width, title_width,
               author_width, copies_width, library);
    row++;
  }

  wmove(win, 1, 0);
  wrefresh(win);
  doupdate();
}

void display_books(WINDOW *win, Book *library, int MAX_BOOKS, char *user,
                   int logged_in) {
  int term_cols, term_rows;
  getmaxyx(win, term_rows, term_cols);
  if (term_cols < 30 || term_rows < 3) {
    char *msg;
    asprintf(&msg, "Size of the terminal too small. x = %d, y = %d", term_cols,
             term_rows);
    show_error_message(stdscr, msg);
  }

  int id_width, isbn_width, title_width, author_width, copies_width;
  id_width = 6;
  isbn_width = 10;
  copies_width = 6;
  title_width = 0;
  author_width = 0;
  int remaining =
      COLS - ((id_width + 2) + (isbn_width + 2) + (copies_width + 2));
  if (remaining < 80) {
    title_width = remaining;
  } else {
    title_width = 80;
    author_width = getmaxx(win) -
                   ((id_width + 2) + (isbn_width + 2) + (copies_width + 2) +
                    (title_width + 2)) +
                   2;
  }

  print_header(win, id_width, isbn_width, title_width, author_width,
               copies_width);

  int row = 1;
  for (int i = 0; i < MAX_BOOKS; i++) {
    print_book(win, row, library[i], 0, id_width, isbn_width, title_width,
               author_width, copies_width, library);
    row++;
  }

  print_footer(win, user, logged_in);

  wmove(win, 1, 0);
  wrefresh(win);
  doupdate();
}

void display_books_admin(WINDOW *win, Book *library, int MAX_BOOKS, char *user,
                         int logged_in) {
  int term_cols, term_rows;
  getmaxyx(win, term_rows, term_cols);
  if (term_cols < 30 || term_rows < 3) {
    char *msg;
    asprintf(&msg, "Size of the terminal too small. x = %d, y = %d", term_cols,
             term_rows);
    show_error_message(stdscr, msg);
  }

  int id_width, isbn_width, title_width, author_width, copies_width;
  id_width = 6;
  isbn_width = 10;
  copies_width = 6;
  title_width = 0;
  author_width = 0;
  int remaining =
      COLS - ((id_width + 2) + (isbn_width + 2) + (copies_width + 2));
  if (remaining < 80) {
    title_width = remaining;
  } else {
    title_width = 80;
    author_width = getmaxx(win) -
                   ((id_width + 2) + (isbn_width + 2) + (copies_width + 2) +
                    (title_width + 2)) +
                   2;
  }

  print_header(win, id_width, isbn_width, title_width, author_width,
               copies_width);

  int row = 1;
  for (int i = 0; i < MAX_BOOKS; i++) {
    print_book(win, row, library[i], 0, id_width, isbn_width, title_width,
               author_width, copies_width, library);
    row++;
  }

  admin_print_footer(win, user, logged_in);

  wmove(win, 1, 0);
  wrefresh(win);
  doupdate();
}

void update_highlight(WINDOW *win, int highlight, int prev_highlight,
                      Book *library) {
  int row;

  int term_cols, term_rows;
  getmaxyx(win, term_rows, term_cols);
  if (term_cols < 30 || term_rows < 3) {
    char *msg;
    asprintf(&msg, "Size of the terminal too small. x = %d, y = %d", term_cols,
             term_rows);
    show_error_message(stdscr, msg);
  }

  int id_width, isbn_width, title_width, author_width, copies_width;
  id_width = 6;
  isbn_width = 10;
  copies_width = 6;
  title_width = 0;
  author_width = 0;
  int remaining =
      COLS - ((id_width + 2) + (isbn_width + 2) + (copies_width + 2));
  if (remaining < 80) {
    title_width = remaining;
  } else {
    title_width = 80;
    author_width = getmaxx(win) -
                   ((id_width + 2) + (isbn_width + 2) + (copies_width + 2) +
                    (title_width + 2)) +
                   2;
  }

  if (highlight == -1 && prev_highlight == -1) {
    return;
  }

  if (prev_highlight != -1) {
    row = prev_highlight + 1;

    print_book(win, row, library[prev_highlight], 0, id_width, isbn_width,
               title_width, author_width, copies_width, library);
  }

  row = highlight + 1;
  print_book(win, row, library[highlight], 1, id_width, isbn_width, title_width,
             author_width, copies_width, library);

  // Highlight current book
  // Refresh the screen efficiently
  wnoutrefresh(win);
  doupdate();
}

void command_mode(WINDOW *win, char *user, int logged_in, int user_type) {
  echo();
  wattron(win, A_REVERSE | A_BOLD);
  int x, y;
  getmaxyx(stdscr, y, x);
  curs_set(1);
  mvwprintw(win, y - 1, 0, " %-*s ", x - 2, ":");
  wchar_t *cmd = (wchar_t *)malloc((x - 3) * sizeof(wchar_t));
  mvwscanw(win, y - 1, 2, "%ls", cmd);
  curs_set(0);
  if (cmd[0] == 'b' && cmd[1] == '/') {
    int len = wcslen(cmd) - 2;
    wchar_t *search_str = (wchar_t *)malloc((len + 1) * sizeof(wchar_t));
    wcsncpy(search_str, cmd + 2, len);
    search_str[len] = '\0';
    int row = 1;
    int MAX_BOOKS = getmaxy(win) - 2;
    int num_books_found = 0;
    Book *library = filter_books("./data/books.csv", search_str, row, "b",
                                 MAX_BOOKS, &num_books_found);
    update_books(win, library, num_books_found);
    wattron(win, A_REVERSE | A_BOLD);
    mvwprintw(win, y - 1, 0, "FILTER BOOKS BY: %-*ls", COLS, search_str);
    wattroff(win, A_REVERSE | A_BOLD);
    wnoutrefresh(win);
    doupdate();
    int highlight = -1;
    int prev_highlight = -1;
    int ch;

    int page_no = 1;
    long long *pagination = (long long *)malloc(page_no * sizeof(long long));
    pagination[0] = 1;
    noecho();
    while (1) {
      ch = wgetch(win);
      prev_highlight = highlight;
      switch (ch) {
      case 'k':
      case KEY_UP:
        if (highlight <= 0) {
          highlight = num_books_found - 1;
        } else {
          highlight--;
        }
        break;
      case 'j':
      case KEY_DOWN:
        if (highlight == num_books_found - 1) {
          highlight = 0;
        } else {
          highlight++;
        }
        break;
      case 'n':
        row = library[num_books_found - 1].id + 1;
        Book *lib_update = filter_books("./data/books.csv", search_str, row,
                                        "b", MAX_BOOKS, &num_books_found);
        if (lib_update == NULL) {
          row = pagination[page_no - 1];
          continue;
        }
        page_no++;
        pagination = (long long *)realloc(pagination, page_no * sizeof(long));
        pagination[page_no - 1] = row;
        update_books(win, lib_update, num_books_found);
        library = lib_update;
        wattron(win, A_REVERSE | A_BOLD);
        mvwprintw(win, y - 1, 0, "FILTER AUTHORS BY: %-*ls", COLS, search_str);
        wattroff(win, A_REVERSE | A_BOLD);
        highlight = -1;
        prev_highlight = -1;
        break;
      case 'N':
        page_no--;
        if (page_no == 0) {
          page_no = 1;
        }
        row = pagination[page_no - 1];
        Book *lib_update_1 = filter_books("./data/books.csv", search_str, row,
                                          "b", MAX_BOOKS, &num_books_found);
        if (lib_update_1 == NULL) {
          continue;
        }
        update_books(win, lib_update_1, num_books_found);
        library = lib_update_1;
        wattron(win, A_REVERSE | A_BOLD);
        mvwprintw(win, y - 1, 0, "FILTER AUTHORS BY: %-*ls", COLS, search_str);
        wattroff(win, A_REVERSE | A_BOLD);
        highlight = -1;
        prev_highlight = -1;
        break;
      case 'i':
        if (logged_in == 0) {
          wattron(win, A_REVERSE | A_BOLD);
          mvwprintw(win, LINES - 1, 0, " %-*s ", COLS - 2, "Not Logged In");
          wattroff(win, A_REVERSE | A_BOLD);
          wrefresh(win);
          sleep(1);
          print_footer(win, user, logged_in);
          continue;
        } else if (highlight != -1) {
          issue_tui(library[highlight], user, user_type, win, logged_in);
          print_footer(win, user, logged_in);
          library = filter_books("./data/books.csv", search_str, row, "b",
                                 MAX_BOOKS, &num_books_found);
          update_books(win, library, num_books_found);
          wattron(win, A_REVERSE | A_BOLD);
          mvwprintw(win, y - 1, 0, "FILTER AUTHORS BY: %-*ls", COLS,
                    search_str);
          wattroff(win, A_REVERSE | A_BOLD);
        }
        break;
      case 27:
        return;
      case ':':
        command_mode(win, user, logged_in, user_type);
        break;
      case 'q':
      case 'Q':
        endwin();
      default:
        continue;
      }
      update_highlight(win, highlight, prev_highlight, library);
    }
    free(search_str);
  }
  if (cmd[0] == 'a' && cmd[1] == '/') {
    int len = wcslen(cmd) - 2;
    wchar_t *search_str = (wchar_t *)malloc((len + 1) * sizeof(wchar_t));
    wcsncpy(search_str, cmd + 2, len);
    search_str[len] = '\0';
    int row = 1;
    int MAX_BOOKS = getmaxy(win) - 2;
    int num_books_found = 0;
    Book *library = filter_books("./data/books.csv", search_str, row, "a",
                                 MAX_BOOKS, &num_books_found);
    update_books(win, library, num_books_found);
    wattron(win, A_REVERSE | A_BOLD);
    mvwprintw(win, y - 1, 0, "FILTER AUTHORS BY: %-*ls", COLS, search_str);
    wattroff(win, A_REVERSE | A_BOLD);
    wnoutrefresh(win);
    doupdate();
    int highlight = -1;
    int prev_highlight = -1;
    int ch;
    int page_no = 1;
    long long *pagination = (long long *)malloc(page_no * sizeof(long long));
    pagination[0] = 1;
    noecho();
    while (1) {
      ch = wgetch(win);
      prev_highlight = highlight;
      switch (ch) {
      case 'k':
      case KEY_UP:
        if (highlight <= 0) {
          highlight = num_books_found - 1;
        } else {
          highlight--;
        }
        break;
      case 'j':
      case KEY_DOWN:
        if (highlight == num_books_found - 1) {
          highlight = 0;
        } else {
          highlight++;
        }
        break;
      case 'n':
        row = library[num_books_found - 1].id + 1;
        Book *lib_update = filter_books("./data/books.csv", search_str, row,
                                        "a", MAX_BOOKS, &num_books_found);
        if (lib_update == NULL) {
          row = pagination[page_no - 1];
          continue;
        }
        page_no++;
        pagination = (long long *)realloc(pagination, page_no * sizeof(long));
        pagination[page_no - 1] = row;
        update_books(win, lib_update, num_books_found);
        library = lib_update;
        wattron(win, A_REVERSE | A_BOLD);
        mvwprintw(win, y - 1, 0, "FILTER AUTHORS BY: %-*ls", COLS, search_str);
        wattroff(win, A_REVERSE | A_BOLD);
        highlight = -1;
        prev_highlight = -1;
        break;
      case 'N':
        page_no--;
        if (page_no == 0) {
          page_no = 1;
        }
        row = pagination[page_no - 1];
        Book *lib_update_1 = filter_books("./data/books.csv", search_str, row,
                                          "a", MAX_BOOKS, &num_books_found);
        if (lib_update_1 == NULL) {
          continue;
        }
        update_books(win, lib_update_1, num_books_found);
        library = lib_update_1;
        wattron(win, A_REVERSE | A_BOLD);
        mvwprintw(win, y - 1, 0, "FILTER AUTHORS BY: %-*ls", COLS, search_str);
        wattroff(win, A_REVERSE | A_BOLD);
        highlight = -1;
        prev_highlight = -1;
        break;
      case 'i':
        if (logged_in == 0) {
          wattron(win, A_REVERSE | A_BOLD);
          mvwprintw(win, LINES - 1, 0, " %-*s ", COLS - 2, "Not Logged In");
          wattroff(win, A_REVERSE | A_BOLD);
          wrefresh(win);
          sleep(1);
          print_footer(win, user, logged_in);
          continue;
        } else if (highlight != -1) {
          issue_tui(library[highlight], user, user_type, win, logged_in);
          print_footer(win, user, logged_in);
          library = filter_books("./data/books.csv", search_str, row, "a",
                                 MAX_BOOKS, &num_books_found);
          update_books(win, library, num_books_found);
          wattron(win, A_REVERSE | A_BOLD);
          mvwprintw(win, y - 1, 0, "FILTER AUTHORS BY: %-*ls", COLS,
                    search_str);
          wattroff(win, A_REVERSE | A_BOLD);
        }
        break;
      case 27:
        return;
      case ':':
        command_mode(win, user, logged_in, user_type);
        break;
      case 'q':
      case 'Q':
        endwin();
      default:
        continue;
      }
      update_highlight(win, highlight, prev_highlight, library);
    }
    free(search_str);
  }

  if ((cmd[0] == 'a' && cmd[1] == 'b') ||
      (cmd[0] == 'b' && cmd[1] == 'a') && cmd[2] == '/') {

    int len = wcslen(cmd) - 3;
    wchar_t *search_str = (wchar_t *)malloc((len + 1) * sizeof(wchar_t));
    wcsncpy(search_str, cmd + 3, len);
    search_str[len] = '\0';
    int row = 1;
    int MAX_BOOKS = getmaxy(win) - 2;
    int num_books_found = 0;
    Book *library = filter_books("./data/books.csv", search_str, row, "ab",
                                 MAX_BOOKS, &num_books_found);
    update_books(win, library, num_books_found);
    wattron(win, A_REVERSE | A_BOLD);
    mvwprintw(win, y - 1, 0, "FILTER BOOKS & AUTHORS BY: %-*ls", COLS,
              search_str);
    wattroff(win, A_REVERSE | A_BOLD);
    wnoutrefresh(win);
    doupdate();
    int highlight = -1;
    int prev_highlight = -1;
    int ch;

    int page_no = 1;
    long long *pagination = (long long *)malloc(page_no * sizeof(long long));
    pagination[0] = 1;
    noecho();
    while (1) {
      ch = wgetch(win);
      prev_highlight = highlight;
      switch (ch) {
      case 'k':
      case KEY_UP:
        if (highlight <= 0) {
          highlight = num_books_found - 1;
        } else {
          highlight--;
        }
        break;
      case 'j':
      case KEY_DOWN:
        if (highlight == num_books_found - 1) {
          highlight = 0;
        } else {
          highlight++;
        }
        break;
      case 'n':
        row = library[num_books_found - 1].id + 1;
        Book *lib_update = filter_books("./data/books.csv", search_str, row,
                                        "ab", MAX_BOOKS, &num_books_found);
        if (lib_update == NULL) {
          row = pagination[page_no - 1];
          continue;
        }
        page_no++;
        pagination = (long long *)realloc(pagination, page_no * sizeof(long));
        pagination[page_no - 1] = row;
        update_books(win, lib_update, num_books_found);
        library = lib_update;
        wattron(win, A_REVERSE | A_BOLD);
        mvwprintw(win, y - 1, 0, "FILTER AUTHORS BY: %-*ls", COLS, search_str);
        wattroff(win, A_REVERSE | A_BOLD);
        highlight = -1;
        prev_highlight = -1;
        break;
      case 'N':
        page_no--;
        if (page_no == 0) {
          page_no = 1;
        }
        row = pagination[page_no - 1];
        Book *lib_update_1 = filter_books("./data/books.csv", search_str, row,
                                          "ab", MAX_BOOKS, &num_books_found);
        if (lib_update_1 == NULL) {
          continue;
        }
        update_books(win, lib_update_1, num_books_found);
        library = lib_update_1;
        wattron(win, A_REVERSE | A_BOLD);
        mvwprintw(win, y - 1, 0, "FILTER AUTHORS BY: %-*ls", COLS, search_str);
        wattroff(win, A_REVERSE | A_BOLD);
        highlight = -1;
        prev_highlight = -1;
        break;
      case 'i':
        if (logged_in == 0) {
          wattron(win, A_REVERSE | A_BOLD);
          mvwprintw(win, LINES - 1, 0, " %-*s ", COLS - 2, "Not Logged In");
          wattroff(win, A_REVERSE | A_BOLD);
          wrefresh(win);
          sleep(1);
          print_footer(win, user, logged_in);
          continue;
        } else if (highlight != -1) {
          issue_tui(library[highlight], user, user_type, win, logged_in);
          print_footer(win, user, logged_in);
          library = filter_books("./data/books.csv", search_str, row, "ab",
                                 MAX_BOOKS, &num_books_found);
          update_books(win, library, num_books_found);
          wattron(win, A_REVERSE | A_BOLD);
          mvwprintw(win, y - 1, 0, "FILTER AUTHORS BY: %-*ls", COLS,
                    search_str);
          wattroff(win, A_REVERSE | A_BOLD);
        }
        break;
      case 27:
        return;
      case ':':
        command_mode(win, user, logged_in, user_type);
        break;
      case 'q':
      case 'Q':
        endwin();
      default:
        continue;
      }
      update_highlight(win, highlight, prev_highlight, library);
    }
    free(search_str);
  }
  wattroff(win, A_REVERSE | A_BOLD);
  print_footer(win, user, logged_in);
  free(cmd);
}

void command_mode_admin(WINDOW *win, char *user, int logged_in) {
  echo();
  wattron(win, A_REVERSE | A_BOLD);
  int x, y;
  getmaxyx(stdscr, y, x);
  curs_set(1);
  mvwprintw(win, y - 1, 0, " %-*s ", x - 2, ":");
  wchar_t *cmd = (wchar_t *)malloc((x - 3) * sizeof(wchar_t));
  mvwscanw(win, y - 1, 2, "%ls", cmd);
  curs_set(0);
  if (cmd[0] == 'b' && cmd[1] == '/') {
    int len = wcslen(cmd) - 2;
    wchar_t *search_str = (wchar_t *)malloc((len + 1) * sizeof(wchar_t));
    wcsncpy(search_str, cmd + 2, len);
    search_str[len] = '\0';
    int row = 1;
    int MAX_BOOKS = getmaxy(win) - 2;
    int num_books_found = 0;
    Book *library = filter_books("./data/books.csv", search_str, row, "b",
                                 MAX_BOOKS, &num_books_found);
    update_books(win, library, num_books_found);
    wattron(win, A_REVERSE | A_BOLD);
    mvwprintw(win, y - 1, 0, "FILTER BOOKS BY: %-*ls", COLS, search_str);
    wattroff(win, A_REVERSE | A_BOLD);
    wnoutrefresh(win);
    doupdate();
    int highlight = -1;
    int prev_highlight = -1;
    int ch;

    int page_no = 1;
    long long *pagination = (long long *)malloc(page_no * sizeof(long long));
    pagination[0] = 1;
    noecho();
    while (1) {
      ch = wgetch(win);
      prev_highlight = highlight;
      switch (ch) {
      case 'k':
      case KEY_UP:
        if (highlight <= 0) {
          highlight = num_books_found - 1;
        } else {
          highlight--;
        }
        break;
      case 'j':
      case KEY_DOWN:
        if (highlight == num_books_found - 1) {
          highlight = 0;
        } else {
          highlight++;
        }
        break;
      case 'n':
        row = library[num_books_found - 1].id + 1;
        Book *lib_update = filter_books("./data/books.csv", search_str, row,
                                        "b", MAX_BOOKS, &num_books_found);
        if (lib_update == NULL) {
          row = pagination[page_no - 1];
          continue;
        }
        page_no++;
        pagination = (long long *)realloc(pagination, page_no * sizeof(long));
        pagination[page_no - 1] = row;
        update_books(win, lib_update, num_books_found);
        library = lib_update;
        wattron(win, A_REVERSE | A_BOLD);
        mvwprintw(win, y - 1, 0, "FILTER BOOKS BY: %-*ls", COLS, search_str);
        wattroff(win, A_REVERSE | A_BOLD);
        highlight = -1;
        prev_highlight = -1;
        break;
      case 'N':
        page_no--;
        if (page_no == 0) {
          page_no = 1;
        }
        row = pagination[page_no - 1];
        Book *lib_update_1 = filter_books("./data/books.csv", search_str, row,
                                          "b", MAX_BOOKS, &num_books_found);
        if (lib_update_1 == NULL) {
          continue;
        }
        update_books(win, lib_update_1, num_books_found);
        library = lib_update_1;
        wattron(win, A_REVERSE | A_BOLD);
        mvwprintw(win, y - 1, 0, "FILTER BOOKS BY: %-*ls", COLS, search_str);
        wattroff(win, A_REVERSE | A_BOLD);
        highlight = -1;
        prev_highlight = -1;
        break;
      case 'R':
        remove_book(win, library[highlight]);
        library = filter_books("./data/books.csv", search_str, row, "b",
                               MAX_BOOKS, &num_books_found);
        if (library == NULL) {
          continue;
        }
        update_books(win, library, num_books_found);
        wattron(win, A_REVERSE | A_BOLD);
        mvwprintw(win, y - 1, 0, "FILTER BOOKS BY: %-*ls", COLS, search_str);
        wattroff(win, A_REVERSE | A_BOLD);
        highlight = -1;
        prev_highlight = -1;
        break;
      case 'E':
        if (highlight == -1) {
          continue;
        }
        edit_book(win, library[highlight]);
        library = filter_books("./data/books.csv", search_str, row, "b",
                               MAX_BOOKS, &num_books_found);
        if (library == NULL) {
          continue;
        }
        update_books(win, library, num_books_found);
        wattron(win, A_REVERSE | A_BOLD);
        mvwprintw(win, y - 1, 0, "FILTER BOOKS BY: %-*ls", COLS, search_str);
        wattroff(win, A_REVERSE | A_BOLD);
        break;
      case 27:
        return;
      case ':':
        command_mode_admin(win, user, logged_in);
        break;
      case 'q':
      case 'Q':
        endwin();
      default:
        continue;
      }
      update_highlight(win, highlight, prev_highlight, library);
    }
    free(search_str);
  }
  if (cmd[0] == 'a' && cmd[1] == '/') {
    int len = wcslen(cmd) - 2;
    wchar_t *search_str = (wchar_t *)malloc((len + 1) * sizeof(wchar_t));
    wcsncpy(search_str, cmd + 2, len);
    search_str[len] = '\0';
    int row = 1;
    int MAX_BOOKS = getmaxy(win) - 2;
    int num_books_found = 0;
    Book *library = filter_books("./data/books.csv", search_str, row, "a",
                                 MAX_BOOKS, &num_books_found);
    update_books(win, library, num_books_found);
    wattron(win, A_REVERSE | A_BOLD);
    mvwprintw(win, y - 1, 0, "FILTER AUTHORS BY: %-*ls", COLS, search_str);
    wattroff(win, A_REVERSE | A_BOLD);
    wnoutrefresh(win);
    doupdate();
    int highlight = -1;
    int prev_highlight = -1;
    int ch;
    int page_no = 1;
    long long *pagination = (long long *)malloc(page_no * sizeof(long long));
    pagination[0] = 1;
    noecho();
    while (1) {
      ch = wgetch(win);
      prev_highlight = highlight;
      switch (ch) {
      case 'k':
      case KEY_UP:
        if (highlight <= 0) {
          highlight = num_books_found - 1;
        } else {
          highlight--;
        }
        break;
      case 'j':
      case KEY_DOWN:
        if (highlight == num_books_found - 1) {
          highlight = 0;
        } else {
          highlight++;
        }
        break;
      case 'n':
        row = library[num_books_found - 1].id + 1;
        Book *lib_update = filter_books("./data/books.csv", search_str, row,
                                        "a", MAX_BOOKS, &num_books_found);
        if (lib_update == NULL) {
          row = pagination[page_no - 1];
          continue;
        }
        page_no++;
        pagination = (long long *)realloc(pagination, page_no * sizeof(long));
        pagination[page_no - 1] = row;
        update_books(win, lib_update, num_books_found);
        library = lib_update;
        wattron(win, A_REVERSE | A_BOLD);
        mvwprintw(win, y - 1, 0, "FILTER AUTHORS BY: %-*ls", COLS, search_str);
        wattroff(win, A_REVERSE | A_BOLD);
        highlight = -1;
        prev_highlight = -1;
        break;
      case 'N':
        page_no--;
        if (page_no == 0) {
          page_no = 1;
        }
        row = pagination[page_no - 1];
        Book *lib_update_1 = filter_books("./data/books.csv", search_str, row,
                                          "a", MAX_BOOKS, &num_books_found);
        if (lib_update_1 == NULL) {
          continue;
        }
        update_books(win, lib_update_1, num_books_found);
        library = lib_update_1;
        wattron(win, A_REVERSE | A_BOLD);
        mvwprintw(win, y - 1, 0, "FILTER AUTHORS BY: %-*ls", COLS, search_str);
        wattroff(win, A_REVERSE | A_BOLD);
        highlight = -1;
        prev_highlight = -1;
        break;
      case 'R':
        remove_book(win, library[highlight]);
        library = filter_books("./data/books.csv", search_str, row, "a",
                               MAX_BOOKS, &num_books_found);
        if (library == NULL) {
          continue;
        }
        update_books(win, library, num_books_found);
        wattron(win, A_REVERSE | A_BOLD);
        mvwprintw(win, y - 1, 0, "FILTER AUTHORS BY: %-*ls", COLS, search_str);
        wattroff(win, A_REVERSE | A_BOLD);
        highlight = -1;
        prev_highlight = -1;
        break;
      case 'E':
        if (highlight == -1) {
          continue;
        }
        edit_book(win, library[highlight]);
        library = filter_books("./data/books.csv", search_str, row, "b",
                               MAX_BOOKS, &num_books_found);
        if (library == NULL) {
          continue;
        }
        update_books(win, library, num_books_found);
        wattron(win, A_REVERSE | A_BOLD);
        mvwprintw(win, y - 1, 0, "FILTER AUTHORS BY: %-*ls", COLS, search_str);
        wattroff(win, A_REVERSE | A_BOLD);
        break;
      case 27:
        return;
      case ':':
        command_mode_admin(win, user, logged_in);
        break;
      case 'q':
      case 'Q':
        endwin();
      default:
        continue;
      }
      update_highlight(win, highlight, prev_highlight, library);
    }
    free(search_str);
  }

  if ((cmd[0] == 'a' && cmd[1] == 'b') ||
      (cmd[0] == 'b' && cmd[1] == 'a') && cmd[2] == '/') {

    int len = wcslen(cmd) - 3;
    wchar_t *search_str = (wchar_t *)malloc((len + 1) * sizeof(wchar_t));
    wcsncpy(search_str, cmd + 3, len);
    search_str[len] = '\0';
    int row = 1;
    int MAX_BOOKS = getmaxy(win) - 2;
    int num_books_found = 0;
    Book *library = filter_books("./data/books.csv", search_str, row, "ab",
                                 MAX_BOOKS, &num_books_found);
    update_books(win, library, num_books_found);
    wattron(win, A_REVERSE | A_BOLD);
    mvwprintw(win, y - 1, 0, "FILTER BOOKS & AUTHORS BY: %-*ls", COLS,
              search_str);
    wattroff(win, A_REVERSE | A_BOLD);
    wnoutrefresh(win);
    doupdate();
    int highlight = -1;
    int prev_highlight = -1;
    int ch;

    int page_no = 1;
    long long *pagination = (long long *)malloc(page_no * sizeof(long long));
    pagination[0] = 1;
    noecho();
    while (1) {
      ch = wgetch(win);
      prev_highlight = highlight;
      switch (ch) {
      case 'k':
      case KEY_UP:
        if (highlight <= 0) {
          highlight = num_books_found - 1;
        } else {
          highlight--;
        }
        break;
      case 'j':
      case KEY_DOWN:
        if (highlight == num_books_found - 1) {
          highlight = 0;
        } else {
          highlight++;
        }
        break;
      case 'n':
        row = library[num_books_found - 1].id + 1;
        Book *lib_update = filter_books("./data/books.csv", search_str, row,
                                        "ab", MAX_BOOKS, &num_books_found);
        if (lib_update == NULL) {
          row = pagination[page_no - 1];
          continue;
        }
        page_no++;
        pagination = (long long *)realloc(pagination, page_no * sizeof(long));
        pagination[page_no - 1] = row;
        update_books(win, lib_update, num_books_found);
        library = lib_update;
        wattron(win, A_REVERSE | A_BOLD);
        mvwprintw(win, y - 1, 0, "FILTER BOOKS & AUTHORS BY: %-*ls", COLS,
                  search_str);
        wattroff(win, A_REVERSE | A_BOLD);
        highlight = -1;
        prev_highlight = -1;
        break;
      case 'N':
        page_no--;
        if (page_no == 0) {
          page_no = 1;
        }
        row = pagination[page_no - 1];
        Book *lib_update_1 = filter_books("./data/books.csv", search_str, row,
                                          "ab", MAX_BOOKS, &num_books_found);
        if (lib_update_1 == NULL) {
          continue;
        }
        update_books(win, lib_update_1, num_books_found);
        library = lib_update_1;
        wattron(win, A_REVERSE | A_BOLD);
        mvwprintw(win, y - 1, 0, "FILTER BOOKS & AUTHORS BY: %-*ls", COLS,
                  search_str);
        wattroff(win, A_REVERSE | A_BOLD);
        highlight = -1;
        prev_highlight = -1;
        break;
      case 'R':
        remove_book(win, library[highlight]);
        library = filter_books("./data/books.csv", search_str, row, "ab",
                               MAX_BOOKS, &num_books_found);
        if (library == NULL) {
          continue;
        }
        update_books(win, library, num_books_found);
        wattron(win, A_REVERSE | A_BOLD);
        mvwprintw(win, y - 1, 0, "FILTER BOOKS & AUTHORS BY: %-*ls", COLS,
                  search_str);
        wattroff(win, A_REVERSE | A_BOLD);
        break;
        highlight = -1;
        prev_highlight = -1;
      case 'E':
        if (highlight == -1) {
          continue;
        }
        edit_book(win, library[highlight]);
        library = filter_books("./data/books.csv", search_str, row, "ab",
                               MAX_BOOKS, &num_books_found);
        if (library == NULL) {
          continue;
        }
        update_books(win, library, num_books_found);
        wattron(win, A_REVERSE | A_BOLD);
        mvwprintw(win, y - 1, 0, "FILTER AUTHORS & BOOKS BY: %-*ls", COLS,
                  search_str);
        wattroff(win, A_REVERSE | A_BOLD);
        break;
      case 27:
        return;
      case ':':
        command_mode_admin(win, user, logged_in);
        break;
      case 'q':
      case 'Q':
        endwin();
      default:
        continue;
      }
      update_highlight(win, highlight, prev_highlight, library);
    }
    free(search_str);
  }
  wattroff(win, A_REVERSE | A_BOLD);
  print_footer(win, user, logged_in);
  free(cmd);
}

void login_tui(WINDOW *win, int *logged_in, char *user, int *user_type) {
  echo();
  wattron(win, A_REVERSE | A_BOLD);
  curs_set(1);
  int x, y;
  getmaxyx(stdscr, y, x);
  mvwprintw(win, y - 1, 0, " %-*s ", x - 2, "Username: ");
  char *username = (char *)malloc((x - 3) * sizeof(char));
  mvwscanw(win, y - 1, 10, "%s", username);
  mvwprintw(win, y - 1, 0, " %-*s ", x - 2, "Password: ");
  char *password = (char *)malloc((x - 3) * sizeof(char));
  wattron(win, A_INVIS);
  mvwscanw(win, y - 1, 10, "%s", password);
  wattroff(win, A_INVIS);
  wattroff(win, A_REVERSE | A_BOLD);
  curs_set(0);
  if (login(username, password, user_type) == 0) {
    wattron(win, A_REVERSE | A_BOLD);
    mvwprintw(win, y - 1, 0, " %-*s ", x - 2, "Login Successful");
    wattroff(win, A_REVERSE | A_BOLD);
    wrefresh(win);
    strcpy(user, username);
    *logged_in = 1;
    sleep(1);
  } else if (login(username, password, user_type) == -1) {

    wattron(win, A_REVERSE | A_BOLD);
    mvwprintw(win, y - 1, 0, " %-*s ", x - 2, "User not found.");
    wattroff(win, A_REVERSE | A_BOLD);
    wrefresh(win);
    *logged_in = 0;
    sleep(1);
  } else {
    wattron(win, A_REVERSE | A_BOLD);
    mvwprintw(win, y - 1, 0, " %-*s ", x - 2, "Incorrect password.");
    wattroff(win, A_REVERSE | A_BOLD);
    wrefresh(win);
    *logged_in = 0;
    sleep(1);
  }
  print_footer(win, user, *logged_in);
  noecho();
}

void log_out(WINDOW *win, char *user, int *logged_in, int *user_type) {
  strcpy(user, "");
  *logged_in = 0;
  print_footer(win, user, *logged_in);
}

void signup_tui(WINDOW *win, char *user, int *logged_in, int *user_type) {
  echo();
  wattron(win, A_REVERSE | A_BOLD);
  curs_set(1);
  int x, y;
  getmaxyx(stdscr, y, x);
  mvwprintw(win, y - 1, 0, " %-*s ", x - 2, "New Username: ");
  char *username = (char *)malloc((x - 3) * sizeof(char));
  mvwscanw(win, y - 1, 14, "%s", username);
  mvwprintw(win, y - 1, 0, " %-*s ", x - 2,
            "Type of user (0: student; 1: faculty; 2: Admin): ");
  mvwscanw(win, y - 1, 51, "%1d", user_type);
  mvwprintw(win, y - 1, 0, " %-*s ", x - 2, "New Password: ");
  char *password = (char *)malloc((x - 3) * sizeof(char));
  wattron(win, A_INVIS);
  mvwscanw(win, y - 1, 14, "%s", password);
  wattroff(win, A_INVIS);
  curs_set(0);
  wattron(win, A_REVERSE | A_BOLD);
  if (signup(username, password, *user_type) == 0) {
    mvwprintw(win, y - 1, 0, " %-*s ", x - 2, "New user added.");
  } else {
    mvwprintw(win, y - 1, 0, " %-*s ", x - 2, "Username already there.");
  }
  wattroff(win, A_REVERSE | A_BOLD);
  wrefresh(win);
  sleep(1);
  print_footer(win, user, *logged_in);
  noecho();
}

void issue_tui(Book book, char *username, int user_type, WINDOW *win,
               int logged_in) {
  char str[MAX_ISBN_LENGTH];
  sprintf(str, "%010lld", book.isbn);
  int num_issued_books;
  issuedbyuser(username, &num_issued_books);
  int status;
  if (num_issued_books < 5)
    status = issuebook(username, user_type, str);
  else
    status = 0;

  if (status == 0 && num_issued_books < 5) {
    wattron(win, A_REVERSE | A_BOLD);
    mvwprintw(win, LINES - 1, 0, " Book: %ls issued successfully %-*s ",
              book.title, COLS, " ");
    wattroff(win, A_REVERSE | A_BOLD);
    wrefresh(win);
    sleep(1);
  } else if (status == -2) {
    wattron(win, A_REVERSE | A_BOLD);
    mvwprintw(win, LINES - 1, 0, " Book: %ls already issued to user %-*s ",
              book.title, COLS, " ");
    wattroff(win, A_REVERSE | A_BOLD);
    wrefresh(win);
    sleep(1);
  } else if (status == -5) {
    wattron(win, A_REVERSE | A_BOLD);
    mvwprintw(win, LINES - 1, 0, " %-*s ", COLS, "Insufficient copies present");
    wattroff(win, A_REVERSE | A_BOLD);
    wrefresh(win);
    sleep(1);
  } else if (num_issued_books >= 5) {
    wattron(win, A_REVERSE | A_BOLD);
    mvwprintw(win, LINES - 1, 0, " %-*s ", COLS,
              "Cannot issue more than 5 books");
    wattroff(win, A_REVERSE | A_BOLD);
    wrefresh(win);
    sleep(1);
  } else {
    wattron(win, A_REVERSE | A_BOLD);
    mvwprintw(win, LINES - 1, 0, " ERROR CODE: %d for isbn %-*s ", status, COLS,
              str);
    wattroff(win, A_REVERSE | A_BOLD);
    wrefresh(win);
    sleep(1);
  }
  print_footer(win, username, logged_in);
}

void add_books(WINDOW *win) {
  echo();
  wattron(win, A_REVERSE | A_BOLD);
  curs_set(1);
  int x, y;
  getmaxyx(stdscr, y, x);
  mvwprintw(win, y - 1, 0, " %-*s ", x - 2, " Please add details of the book ");
  mvwprintw(win, y - 1, 0, " %-*s ", x - 2, " TITLE: ");
  char *title = (char *)malloc((MAX_TITLE_LENGTH * sizeof(char)));
  mvwscanw(win, y - 1, 9, "%s", title);
  mvwprintw(win, y - 1, 0, " %-*s ", x - 2, " AUTHOR: ");
  char *author = (char *)malloc(MAX_AUTHOR_LENGTH * sizeof(char));
  mvwscanw(win, y - 1, 10, "%s", author);
  mvwprintw(win, y - 1, 0, " %-*s ", x - 2, " ISBN: ");
  char *isbn = (char *)malloc(MAX_ISBN_LENGTH * sizeof(char));
  mvwscanw(win, y - 1, 8, "%s", isbn);
  mvwprintw(win, y - 1, 0, " %-*s ", x - 2, " COPIES ");
  int copies;
  mvwscanw(win, y - 1, 9, "%d", &copies);
  if (book_add(isbn, title, author, copies) == 0) {
    mvwprintw(win, y - 1, 0, " %-*s ", x - 2, " Book added successfully. ");
  }
  wattroff(win, A_REVERSE | A_BOLD);
  curs_set(0);
}

void remove_book(WINDOW *win, Book book) {
  char str[MAX_ISBN_LENGTH];
  sprintf(str, "%010lld", book.isbn);
  wattron(win, A_REVERSE | A_BOLD);
  if (book_remove(str) == 0) {
    mvwprintw(win, LINES - 1, 0, " %-*s ", COLS - 2,
              " Book Removed Successfully. ");
  } else {
    mvwprintw(win, LINES - 1, 0, " %-*s ", COLS - 2, " Book REmove Error. ");
  }
  sleep(2);
  wattroff(win, A_REVERSE | A_BOLD);
  wrefresh(win);
}

void edit_book(WINDOW *win, Book book) {
  echo();
  wattron(win, A_REVERSE | A_BOLD);
  curs_set(1);
  int x, y;
  getmaxyx(stdscr, y, x);
  mvwprintw(win, y - 1, 0, " %-*s ", x - 2, " Please add details of the book ");
  mvwprintw(win, y - 1, 0, " %-*s ", x - 2, " NEW TITLE: ");
  char *title = (char *)malloc((MAX_TITLE_LENGTH * sizeof(char)));
  mvwgetnstr(win, y - 1, 13, title, MAX_TITLE_LENGTH - 1);
  mvwprintw(win, y - 1, 0, " %-*s ", x - 2, " NEW AUTHOR: ");
  char *author = (char *)malloc(MAX_AUTHOR_LENGTH * sizeof(char));
  mvwgetnstr(win, y - 1, 14, author, MAX_AUTHOR_LENGTH - 1);
  mvwprintw(win, y - 1, 0, " %-*s ", x - 2, " NEW ISBN: ");
  char *isbn = (char *)malloc(MAX_ISBN_LENGTH * sizeof(char));
  mvwgetnstr(win, y - 1, 12, isbn, MAX_ISBN_LENGTH - 1);
  mvwprintw(win, y - 1, 0, " %-*s ", x - 2, " NEW COPIES ");
  int copies;
  mvwscanw(win, y - 1, 13, "%d", &copies);
  char str[MAX_ISBN_LENGTH];
  sprintf(str, "%010lld", book.isbn);
  if (book_remove(str) != 0) {
    mvwprintw(win, y - 1, 0, " %-*s ", x - 2, " Editing unsuccessful ");
  } else if (book_add(isbn, title, author, copies) != 0) {
    mvwprintw(win, y - 1, 0, " %-*s ", x - 2, " Editing Unsucceesful ");
  } else {
    mvwprintw(win, y - 1, 0, " %-*s ", x - 2, " Editing succeesful ");
  }
  wattroff(win, A_REVERSE | A_BOLD);
  curs_set(0);
}

void admin_mode(char *x, char *user, int user_type) {
  if (*x != 'a') {
    *x = 'b';
    return;
  }
  int highlight = -1;
  int prev_highlight = -1;
  int ch;
  int logged_in = 0;

  WINDOW *table_win = newwin(LINES, COLS, 0, 0);
  keypad(table_win, TRUE);
  int MAX_BOOKS = getmaxy(table_win) - 2;
  int row = 1;
  Book *library = window(1, MAX_BOOKS);
  display_books_admin(table_win, library, MAX_BOOKS, user, logged_in);
  // int MAX_BOOKS = getmaxy(table_win) - 2;
  char admin = 'b';
  while (1 && *x == 'a') {
    ch = wgetch(table_win);
    prev_highlight = highlight;
    switch (ch) {
    case 'k':
    case KEY_UP:
      if (highlight <= 0) {
        highlight = MAX_BOOKS - 1;
      } else {
        highlight--;
      }
      break;
    case 'j':
    case KEY_DOWN:
      if (highlight == MAX_BOOKS - 1) {
        highlight = 0;
      } else {
        highlight++;
      }
      break;
    case 'n':
      row = MAX_BOOKS + row;
      library = window(row, MAX_BOOKS);
      display_books_admin(table_win, library, MAX_BOOKS, user, logged_in);
      highlight = -1;
      prev_highlight = -1;
      break;
    case 'N':
      if (row - MAX_BOOKS <= 0) {
        break;
      }
      row = row - MAX_BOOKS;
      library = window(row, MAX_BOOKS);
      display_books_admin(table_win, library, MAX_BOOKS, user, logged_in);
      highlight = -1;
      prev_highlight = -1;
      break;
    case 'A':
      add_books(table_win);
      library = window(1, MAX_BOOKS);
      display_books_admin(table_win, library, MAX_BOOKS, user, logged_in);
      highlight = -1;
      prev_highlight = -1;
      break;
    case 'R':
      remove_book(table_win, library[highlight]);
      library = window(row, MAX_BOOKS);
      display_books_admin(table_win, library, MAX_BOOKS, user, logged_in);
      break;
    case 'E':
      if (highlight == -1) {
        continue;
      }
      edit_book(table_win, library[highlight]);
      library = window(row, MAX_BOOKS);
      display_books_admin(table_win, library, MAX_BOOKS, user, logged_in);
      break;
    case 'X':
      *x = 'b';
      return;
      break;
    case ':':
      command_mode_admin(table_win, user, logged_in);
      library = window(row, MAX_BOOKS);
      display_books_admin(table_win, library, MAX_BOOKS, user, logged_in);
      highlight = -1;
      prev_highlight = -1;
      break;
    default:
      continue;
    }
    update_highlight(table_win, highlight, prev_highlight, library);
  }
}

void chat(WINDOW *win, char *user) {
  wattron(win, A_REVERSE | A_BOLD);
  int x, y;
  getmaxyx(stdscr, y, x);
  mvwprintw(win, y - 1, 0, " %-*s ", x - 2, " Please query the chat bot. ");
  wrefresh(win);
  sleep(1);
  mvwprintw(win, y - 1, 0, " %-*s ", x - 2, "  ");
  char *query = (char *)malloc((MAX_STRINGS * sizeof(char)));
  echo();
  curs_set(1);
  mvwgetnstr(win, y - 1, 1, query, MAX_STRINGS - 1);
  sleep(1);
  mvwprintw(win, y - 1, 0, " %-*s ", x - 2, "  ");
  const char *answer = (char *)malloc((MAX_STRINGS * sizeof(char)));
  answer = find_answer(query, knowledge_base);
  mvwprintw(win, y - 1, 0, " %-*s ", x - 2, answer);
  wrefresh(win);
  sleep(2);
  wattroff(win, A_REVERSE | A_BOLD);
}

int main(int argc, char *argv[]) {
  // Set locale to support wide characters
  setlocale(LC_ALL, "");

  // Initialise ncurses
  initscr();
  noecho();
  cbreak();
  curs_set(0);
  start_color();

  int highlight = -1;
  int prev_highlight = -1;
  int ch;
  char *user = (char *)malloc(100 * sizeof(char));
  int user_type;
  int logged_in = 0;

  WINDOW *table_win = newwin(LINES, COLS, 0, 0);
  keypad(table_win, TRUE);
  int MAX_BOOKS = getmaxy(table_win) - 2;
  int row = 1;
  Book *library = window(1, MAX_BOOKS);
  display_books(table_win, library, MAX_BOOKS, user, logged_in);
  // int MAX_BOOKS = getmaxy(table_win) - 2;
  char admin = 'b';
  while (1) {
    ch = wgetch(table_win);
    prev_highlight = highlight;
    switch (ch) {
    case 'k':
    case KEY_UP:
      if (highlight <= 0) {
        highlight = MAX_BOOKS - 1;
      } else {
        highlight--;
      }
      break;
    case 'j':
    case KEY_DOWN:
      if (highlight == MAX_BOOKS - 1) {
        highlight = 0;
      } else {
        highlight++;
      }
      break;
    case 'n':
      row = MAX_BOOKS + row;
      library = window(row, MAX_BOOKS);
      display_books(table_win, library, MAX_BOOKS, user, logged_in);
      highlight = -1;
      prev_highlight = -1;
      break;
    case 'N':
      if (row - MAX_BOOKS <= 0) {
        break;
      }
      row = row - MAX_BOOKS;
      library = window(row, MAX_BOOKS);
      display_books(table_win, library, MAX_BOOKS, user, logged_in);
      highlight = -1;
      prev_highlight = -1;
      break;
    case 'i':
      if (logged_in == 0) {
        wattron(table_win, A_REVERSE | A_BOLD);
        mvwprintw(table_win, LINES - 1, 0, " %-*s ", COLS - 2, "Not Logged In");
        wattroff(table_win, A_REVERSE | A_BOLD);
        wrefresh(table_win);
        sleep(1);
        print_footer(table_win, user, logged_in);
        continue;
      } else if (highlight != -1) {
        issue_tui(library[highlight], user, user_type, table_win, logged_in);
        print_footer(table_win, user, logged_in);
        library = window(row, MAX_BOOKS);
        display_books(table_win, library, MAX_BOOKS, user, logged_in);
        update_highlight(table_win, highlight, prev_highlight, library);
      }
      break;
    case 'l':
      login_tui(table_win, &logged_in, user, &user_type);
      break;
    case '?':
      chat(table_win, user);
      print_footer(table_win, user, logged_in);
      library = window(row, MAX_BOOKS);
      display_books(table_win, library, MAX_BOOKS, user, logged_in);
      update_highlight(table_win, highlight, prev_highlight, library);
      break;
    case 'a':
      if (logged_in == 0) {
        wattron(table_win, A_REVERSE | A_BOLD);
        mvwprintw(table_win, LINES - 1, 0, " %-*s ", COLS - 2, "Not Logged In");
        wattroff(table_win, A_REVERSE | A_BOLD);
        wrefresh(table_win);
        sleep(1);
        print_footer(table_win, user, logged_in);
        continue;
      } else if (user_type != 2) {
        wattron(table_win, A_REVERSE | A_BOLD);
        mvwprintw(table_win, LINES - 1, 0, " %-*s ", COLS - 2, "Not Admin");
        mvwprintw(table_win, LINES - 1, 0, " %d ", user_type);
        wattroff(table_win, A_REVERSE | A_BOLD);
        wrefresh(table_win);
        sleep(1);
        print_footer(table_win, user, logged_in);
        continue;
      } else {
        admin = 'a';
        admin_mode(&admin, user, user_type);
      }
      library = window(row, MAX_BOOKS);
      display_books(table_win, library, MAX_BOOKS, user, logged_in);
      break;
    case 'A':
      signup_tui(table_win, user, &logged_in, &user_type);
      break;
    case 'X':
      log_out(table_win, user, &logged_in, &user_type);
      break;
    case ':':
      command_mode(table_win, user, logged_in, user_type);
      library = window(row, MAX_BOOKS);
      display_books(table_win, library, MAX_BOOKS, user, logged_in);
      highlight = -1;
      prev_highlight = -1;
      break;
    case 'Y':
      if (logged_in == 0) {
        wattron(table_win, A_REVERSE | A_BOLD);
        mvwprintw(table_win, LINES - 1, 0, " %-*s ", COLS - 2, "Not Logged In");
        wattroff(table_win, A_REVERSE | A_BOLD);
        wrefresh(table_win);
        sleep(1);
        continue;
      } else {
        int iss_highlight = -1;
        int iss_prev_highlight = -1;
        int status_issue;
        int c;
        IssuedBook *library_issued;
        int num_issued_books;
        library_issued = issuedbyuser(user, &num_issued_books);
        if (num_issued_books == 0) {

          wattron(table_win, A_REVERSE | A_BOLD);
          mvwprintw(table_win, LINES - 1, 0, " %-*s ", COLS - 2,
                    "No Books issued");
          wattroff(table_win, A_REVERSE | A_BOLD);
          wrefresh(table_win);
          sleep(1);
          library = window(row, MAX_BOOKS);
          display_books(table_win, library, MAX_BOOKS, user, logged_in);
          break;
        } else {
          display_issued_books(table_win, library_issued, num_issued_books,
                               user, logged_in);
          while (1) {
            if (num_issued_books == 0) {
              break;
            }
            c = wgetch(table_win);
            iss_prev_highlight = iss_highlight;
            if (c == 27) {
              break;
            }
            switch (c) {
            case 'k':
            case KEY_UP:
              if (iss_highlight <= 0) {
                iss_highlight = num_issued_books - 1;
              } else {
                iss_highlight--;
              }
              break;
            case 'j':
            case KEY_DOWN:
              if (iss_highlight == num_issued_books - 1) {
                iss_highlight = 0;
              } else {
                iss_highlight++;
              }
              break;
            case 'r':
              status_issue =
                  returnbook(user, library_issued[iss_highlight].isbn);
              if (status_issue == 0) {

                library_issued = issuedbyuser(user, &num_issued_books);
                display_issued_books(table_win, library_issued,
                                     num_issued_books, user, logged_in);
                iss_highlight = -1;
                iss_prev_highlight = -1;
              }

            default:
              continue;
            }
            update_issued_highlight(table_win, iss_highlight,
                                    iss_prev_highlight, library_issued);
          }
        }
      }
      library = window(row, MAX_BOOKS);
      display_books(table_win, library, MAX_BOOKS, user, logged_in);
      break;
    case 'q':
    case 'Q':
      endwin();
      return 0;
    default:
      continue;
    }
    update_highlight(table_win, highlight, prev_highlight, library);
  }

  delwin(table_win);
  endwin();
  return EXIT_SUCCESS;
}
