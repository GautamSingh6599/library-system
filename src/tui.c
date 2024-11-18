#include "../include/tui.h"
#include "../include/accounts.h"
#include "../include/book.h"
#include <ncurses.h>
#include <string.h>

// Function to print the header with dynamic column widths
void print_header(WINDOW *win, int id_width, int isbn_width, int title_width,
                  int author_width, int copies_width) {
  wattron(win, A_REVERSE | A_BOLD);
  mvwprintw(win, 0, 0, " %-*s %-*s %-*s %-*.*s %-*.*s ", id_width, "ID",
            isbn_width, "ISBN", copies_width, "Copies", title_width,
            title_width, "Title", author_width, author_width, "Author");
  wattroff(win, A_REVERSE | A_BOLD);
}

// Function to print footer
void print_footer(WINDOW *win, char *user, int logged_in) {
  wattron(win, A_REVERSE | A_BOLD);
  int x, y;
  getmaxyx(stdscr, y, x);
  mvwprintw(win, y - 1, 0, " %-*s ", x - 2,
            "EXIT[q] NEXTPAGE[n] PREVPAGE[N] SCROLLUP[k/UP] "
            "SCROLLDOWN[j/DOWN]SEARCH[:] LOGIN[l]");
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

void command_mode(WINDOW *win, char *user, int logged_in) {
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
      case 27:
        return;
      case ':':
        command_mode(win, user, logged_in);
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
      case 27:
        return;
      case ':':
        command_mode(win, user, logged_in);
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
      case 27:
        return;
      case ':':
        command_mode(win, user, logged_in);
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
    case 'l':
      login_tui(table_win, &logged_in, user, &user_type);
      break;
    case ':':
      command_mode(table_win, user, logged_in);
      int row = 1;
      Book *library = window(1, MAX_BOOKS);
      display_books(table_win, library, MAX_BOOKS, user, logged_in);
      int hioghlight = -1;
      int prev_highlight = -1;
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
