// Copyright [2024]
// Gautam Singh

#include "../include/tui.h"
#include "../include/book.h"

Book library[10] = {
    {1, 9780131103,
     L"The C "
     L"Prograhbiidkfjsdfnsdjfsndjfnsjfndsjfnsjdfnjdfndjfndfdklsdjfnsdkfmmming "
     L"Language",
     L"Brian W. Kernighan", 5},
    {2, 9780201633, L"Design Patterns", L"Erich Gamma", 3},
    {3, 9780262033, L"Introduction to Algorithms", L"Cormen", 7},
    {4, 9780132350, L"Clean Code", L"Robert C. Martin", 2},
    {5, 9780134494, L"Effective Modern C++", L"Scott Meyers", 4},
    {6, 9780131101, L"The UNIX Programming Environment", L"Rob Pike", 1},
    {7, 9780321751, L"The C++ Programming Language", L"Bjarne Stroustrup", 6},
    {8, 9781491903, L"Fluent Python", L"Luciano Ramalho", 3},
    {9, 9780134685, L"Refactoring", L"Martin Fowler", 2},
    {10, 9978059600, L"Head First Design Patterns", L"Eric Freeman", 8}};

// Function to print the header with dynamic column widths
void print_header(WINDOW *win, int id_width, int isbn_width, int title_width,
                  int author_width, int copies_width) {
  wattron(win, A_REVERSE);
  mvwprintw(win, 0, 0, " %-*s %-*s %-*s %-*.*s %-*.*s ", id_width, "ID",
            isbn_width, "ISBN", copies_width, "Copies", title_width,
            title_width, "Title", author_width, author_width, "Author");
  wattroff(win, A_REVERSE);
}

// Function to print footer
void print_footer(WINDOW *win) {
  wattron(win, A_REVERSE);
  int x, y;
  getmaxyx(stdscr, y, x);
  mvwprintw(win, y - 1, 0, " %-*s ", x - 2, "Exit(q)");
  wattroff(win, A_REVERSE);
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
                int copies_width) {
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

void display_books(WINDOW *win) {
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
  int row = 1;
  /*for (int i = 0; i < LINES - 2; i++) {*/
  for (int i = 0; i < 10; i++) {
    print_book(win, row, library[i], 0, id_width, isbn_width, title_width,
               author_width, copies_width);
    row++;
  }

  print_footer(win);

  wmove(win, 1, 0);
  wrefresh(win);
  doupdate();
}

void update_highlight(WINDOW *win, int highlight, int prev_highlight) {
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

  if (prev_highlight != -1) {
    row = prev_highlight + 1;

    print_book(win, row, library[prev_highlight], 0, id_width, isbn_width,
               title_width, author_width, copies_width);
  }
  // Highlight current book
  mvwprintw(
      win, 11, 0,
      "id = %d, isbn = %d, copies = %d, title = %d, author = %d, cols = %d",
      id_width, isbn_width, copies_width, title_width, author_width,
      getmaxx(win));
  mvwprintw(win, 12, 168, "He");
  // Add highlight to current book

  row = highlight + 1;
  if (row == 0) {
    row = 1;
  }
  print_book(win, row, library[highlight], 1, id_width, isbn_width, title_width,
             author_width, copies_width);
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
  display_books(table_win);
  // int MAX_BOOKS = getmaxy(table_win) - 2;
  int MAX_BOOKS = 10;

  int highlight = -1;
  int prev_highlight = -1;
  int ch;

  while (1) {
    ch = wgetch(table_win);
    prev_highlight = highlight;
    switch (ch) {
    case 'k':
    case KEY_UP:
      if (highlight == 0) {
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
    case 'q':
    case 'Q':
      endwin();
      return 0;
    default:
      continue;
    }
    update_highlight(table_win, highlight, prev_highlight);
  }

  delwin(table_win);
  endwin();
  return EXIT_SUCCESS;
}
