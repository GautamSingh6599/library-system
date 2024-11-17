// Copyright [2024]
// Gautam Singh

#include "../include/tui.h"
#include "../include/book.h"
#include <ncurses.h>

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
void print_footer(WINDOW *win) {
  wattron(win, A_REVERSE | A_BOLD);
  int x, y;
  getmaxyx(stdscr, y, x);
  mvwprintw(win, y - 1, 0, " %-*s ", x - 2,
            "EXIT[q] NEXTPAGE[n] PREVPAGE[N] SCROLLUP[k/UP] SCROLLDOWN[j/DOWN] "
            "SEARCH[/] LOGIN[l]");
  wattroff(win, A_REVERSE | A_BOLD);
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

void display_books(WINDOW *win, Book *library, int MAX_BOOKS) {
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

  /*mvwprintw(*/
  /*    win, 11, 0,*/
  /*    "id = %d, isbn = %d, copies = %d, title = %d, author = %d, cols = %d",*/
  /*    id_width, isbn_width, copies_width, title_width, author_width,*/
  /*    getmaxx(win));*/
  /*for (int i = 0; i < LINES - 2; i++) {*/
  int row = 1;
  for (int i = 0; i < MAX_BOOKS; i++) {
    print_book(win, row, library[i], 0, id_width, isbn_width, title_width,
               author_width, copies_width, library);
    row++;
  }

  print_footer(win);

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
  // Highlight current book
  /*mvwprintw(*/
  /*    win, 11, 0,*/
  /*    "id = %d, isbn = %d, copies = %d, title = %d, author = %d, cols = %d",*/
  /*    id_width, isbn_width, copies_width, title_width, author_width,*/
  /*    getmaxx(win));*/
  /*mvwprintw(win, 12, 168, "He");*/
  // Add highlight to current book

  row = highlight + 1;
  print_book(win, row, library[highlight], 1, id_width, isbn_width, title_width,
             author_width, copies_width, library);
  // Highlight current book
  // Refresh the screen efficiently
  wnoutrefresh(win);
  doupdate();
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

  WINDOW *table_win = newwin(LINES, COLS, 0, 0);
  keypad(table_win, TRUE);
  int MAX_BOOKS = getmaxy(table_win) - 2;
  int row = 1;
  Book *library = window(1, MAX_BOOKS);
  display_books(table_win, library, MAX_BOOKS);
  // int MAX_BOOKS = getmaxy(table_win) - 2;

  int highlight = -1;
  int prev_highlight = -1;
  int ch;

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
      display_books(table_win, library, MAX_BOOKS);
      highlight = -1;
      prev_highlight = -1;
      break;
    case 'N':
      if (row - MAX_BOOKS <= 0) {
        break;
      }
      row = row - MAX_BOOKS;
      library = window(row, MAX_BOOKS);
      display_books(table_win, library, MAX_BOOKS);
      highlight = -1;
      prev_highlight = -1;
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
