// Copyright [2024]
// Gautam Singh
#include <locale.h>
#include <ncurses.h>
#include <wchar.h>

#define MAX_BOOKS 34 // Maximum books on screen

typedef struct {
  int id;
  wchar_t isbn[20];
  wchar_t title[300];
  wchar_t author[150];
  int year;
  wchar_t publisher[150];
  int copies;
} Book;

/*int lines() {*/
/*  setlocale(LC_ALL, "en_US.UTF-8");*/
/**/
/*  // Open the file in read mode*/
/*  FILE *file = fopen("final_books.csv", "r");*/
/*  if (file == NULL) {*/
/*    perror("Error opening library database.");*/
/*    return EXIT_FAILURE;*/
/*  }*/
/**/
/*  // Set up iconv for converting ISO-8859-1 to UTF-8*/
/*  iconv_t cd = iconv_open("UTF-8", "ISO-8859-1");*/
/*  if (cd == (iconv_t)-1) {*/
/*    perror("Error opening iconv conversion");*/
/*    fclose(file);*/
/*    return EXIT_FAILURE;*/
/*  }*/
/**/
/*  int count = 0;*/
/*  char inbuf[1024], outbuf[1024];*/
/*  size_t inbytesleft, outbytesleft;*/
/**/
/*  while (fgets(inbuf, sizeof(inbuf), file)) {*/
/*    inbytesleft = strlen(inbuf);*/
/*    outbytesleft = sizeof(outbuf);*/
/**/
/*    char *inptr = inbuf;*/
/*    char *outptr = outbuf;*/
/**/
/*    // Perform encoding conversion from ISO-8859-1 to UTF-8*/
/*    size_t ret = iconv(cd, &inptr, &inbytesleft, &outptr, &outbytesleft);*/
/*    if (ret == (size_t)-1) {*/
/*      perror("Error converting encoding");*/
/*      continue;*/
/*    }*/
/**/
/*    // Now outbuf contains the UTF-8 version of the line*/
/*    // Process this line to count newlines*/
/*    for (int i = 0; outbuf[i] != '\0'; i++) {*/
/*      if (outbuf[i] == '\n') {*/
/*        count++;*/
/*      }*/
/*    }*/
/*  }*/
/**/
/*  // Close iconv and file*/
/*  iconv_close(cd);*/
/*  fclose(file);*/
/*  return count;*/
/*}*/
/**/
/*int readFile(Book *library, int numBooks) {*/
/*  FILE *file = fopen("final_books.csv", "r, ccs=UTF-8");*/
/*  if (file == NULL) {*/
/*    perror("Error opening library database.");*/
/*    return EXIT_FAILURE;*/
/*  }*/
/**/
/*  wchar_t line[1024];*/
/*  int i = 0;*/
/**/
/*  // Skip the header row*/
/*  fgetws(line, sizeof(line) / sizeof(wchar_t), file);*/
/**/
/*  while (fgetws(line, sizeof(line) / sizeof(wchar_t), file) != NULL &&*/
/*         i < numBooks) {*/
/*    // Remove trailing newline characters*/
/*    line[wcslen(line) - 1] =*/
/*        (line[wcslen(line) - 1] == L'\n') ? L'\0' : line[wcslen(line) - 1];*/
/*    line[wcslen(line) - 1] =*/
/*        (line[wcslen(line) - 1] == L'\r') ? L'\0' : line[wcslen(line) - 1];*/
/**/
/*    // Debug: print the line read from the file*/
/**/
/*    wchar_t *token;*/
/*    wchar_t *saveptr;*/
/**/
/*    // Use wcstok to parse each field*/
/*    token = wcstok(line, L",", &saveptr);*/
/*    if (token != NULL)*/
/*      library[i].slno = wcstoll(token, NULL, 10);*/
/**/
/*    token = wcstok(NULL, L",", &saveptr);*/
/*    if (token != NULL)*/
/*      library[i].ISBN = wcstoll(token, NULL, 10);*/
/**/
/*    token = wcstok(NULL, L",", &saveptr);*/
/*    if (token != NULL)*/
/*      wcsncpy(library[i].title, token, 299);*/
/*    library[i].title[299] = L'\0'; // Ensure null termination*/
/**/
/*    token = wcstok(NULL, L",", &saveptr);*/
/*    if (token != NULL)*/
/*      wcsncpy(library[i].author, token, 149);*/
/*    library[i].author[149] = L'\0';*/
/**/
/*    token = wcstok(NULL, L",", &saveptr);*/
/*    if (token != NULL)*/
/*      library[i].yearPub = wcstol(token, NULL, 10);*/
/**/
/*    token = wcstok(NULL, L",", &saveptr);*/
/*    if (token != NULL)*/
/*      wcsncpy(library[i].publisher, token, 149);*/
/*    library[i].publisher[149] = L'\0';*/
/**/
/*    token = wcstok(NULL, L",", &saveptr);*/
/*    if (token != NULL)*/
/*      library[i].copies = wcstol(token, NULL, 10);*/
/**/
/*    // Move to the next book entry*/
/*    i++;*/
/*  }*/
/**/
/*  fclose(file);*/
/*  return EXIT_SUCCESS;*/
/*}*/

Book library[MAX_BOOKS] = {
    {1, L"978-0131103627",
     L"The C "
     L"Prograhbiidkfjsdfnsdjfsndjfnsjfndsjfnsjdfnjdfndjfndfdklsdjfnsdkfmmming "
     L"Language",
     L"Brian W. Kernighan", 1978, L"Prentice Hall", 5},
    {2, L"978-0201633610", L"Design Patterns", L"Erich Gamma", 1994,
     L"Addison-Wesley", 3},
    {3, L"978-0262033848", L"Introduction to Algorithms", L"Cormen", 2009,
     L"MIT Press", 7},
    {4, L"978-0132350884", L"Clean Code", L"Robert C. Martin", 2008,
     L"Prentice Hall", 2},
    {5, L"978-0134494166", L"Effective Modern C++", L"Scott Meyers", 2014,
     L"O'Reilly Media", 4},
    {6, L"978-0131101630", L"The UNIX Programming Environment", L"Rob Pike",
     1984, L"Prentice Hall", 1},
    {7, L"978-0321751041", L"The C++ Programming Language",
     L"Bjarne Stroustrup", 2013, L"Addison-Wesley", 6},
    {8, L"978-1491903995", L"Fluent Python", L"Luciano Ramalho", 2015,
     L"O'Reilly Media", 3},
    {9, L"978-0134685991", L"Refactoring", L"Martin Fowler", 2018,
     L"Addison-Wesley", 2},
    {10, L"978-0596007126", L"Head First Design Patterns", L"Eric Freeman",
     2004, L"O'Reilly Media", 8}};

// Function to print the header
void print_header(WINDOW *win) {
  wattron(win, A_REVERSE);
  mvwprintw(win, 0, 0, " %-6s  %-14s  %-83s  %-50s  %-4s  %-30s  %-4s ", "ID",
            "ISBN", "Title", "Author", "Year", "Publisher", "Copies");
  wattroff(win, A_REVERSE);
}

// Function to print a single book entry with optional highlighting
void print_book(WINDOW *win, int row, Book book, int highlight) {
  if (highlight) {
    wattron(win, A_STANDOUT); // Use standout mode for clear highlighting
  }

  // Print ID
  mvwprintw(win, row, 0, " %-6d ", book.id);

  // Print ISBN
  mvwprintw(win, row, 8, " %-15ls ", book.isbn);

  // Print Title
  mvwprintw(win, row, 24, " %-83ls ", book.title);

  // Print Author
  mvwprintw(win, row, 109, " %-50ls ", book.author);

  // Print Year
  mvwprintw(win, row, 161, " %-4d ", book.year);

  // Print Publisher
  mvwprintw(win, row, 167, " %-30ls ", book.publisher);

  // Print Copies
  mvwprintw(win, row, 199, " %-6d ", book.copies);

  if (highlight) {
    wattroff(win, A_STANDOUT);
  }
}

// Function to display all books
void display_books(WINDOW *win) {
  int i;
  int row = 1;

  // Print the header row
  print_header(win);

  // Print all books
  for (i = 0; i < MAX_BOOKS; i++) {
    print_book(win, row, library[i], 0); // No highlighting initially
    row++;
  }

  // Move the cursor to the first row (positioning it after all the books are
  // printed)
  wmove(win, 1, 0); // Move cursor to row 1, column 0

  // Refresh the screen after printing all books
  wnoutrefresh(win);
  doupdate();
}

// Function to update the display with the highlighted book
void update_highlight(WINDOW *win, int highlight, int previous_highlight) {
  int row;

  // Remove highlight from previous book
  if (previous_highlight != -1) {
    row = previous_highlight + 1;
    print_book(win, row, library[previous_highlight], 0); // No highlight
  }

  // Add highlight to current book
  row = highlight + 1;
  print_book(win, row, library[highlight], 1); // Highlight current book

  // Refresh the screen efficiently
  wnoutrefresh(win);
  doupdate();
}

int main() {
  // Set locale to support wide characters
  setlocale(LC_ALL, "");

  // Initialize ncurses
  initscr();
  noecho();
  cbreak();
  curs_set(0); // Hide the default cursor
  start_color();

  // Create a new window covering the entire screen
  WINDOW *table_win = newwin(LINES, COLS, 0, 0);
  keypad(table_win, TRUE); // Enable arrow keys

  int highlight = -1;          // Index of the currently highlighted book
  int previous_highlight = -1; // Track the previously highlighted book
  int ch;

  // Display all books initially
  display_books(table_win);

  // Main loop for user input
  while (1) {
    ch = wgetch(table_win); // Wait for user input

    previous_highlight = highlight;

    switch (ch) {
    case 'k':
      if (highlight <= 0) {
        highlight = MAX_BOOKS - 1;
      } else {
        highlight--; // Move cursor up
      }
      break;
    case KEY_UP:
      if (highlight <= 0) {
        highlight = MAX_BOOKS - 1;
      } else {
        highlight--; // Move cursor up
      }
      break;
    case 'j':
      if (highlight < MAX_BOOKS - 1) {
        highlight++; // Move cursor down
      } else {
        highlight = 0;
      }
      break;
    case KEY_DOWN:
      if (highlight < MAX_BOOKS - 1) {
        highlight++; // Move cursor down
      } else {
        highlight = 0;
      }
      break;
    case 'q':
    case 'Q':
      endwin(); // Exit ncurses mode
      return 0;
    default:
      if (highlight <= 0) {
        highlight = previous_highlight;
      }
    }

    // Update display with new highlight
    update_highlight(table_win, highlight, previous_highlight);
  }

  // Clean up ncurses
  delwin(table_win);
  endwin();
  return 0;
}
