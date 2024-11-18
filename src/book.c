// Copyright
// Gautam Singh

#include "../include/book.h"
#include <locale.h>
#include <stdio.h>
#include <string.h>

// Function to read a CSV file from line a to line b and store in an array of
// structs
int read_csv_range(const char *filename, int a, int b, Book **books,
                   int *num_records) {
  // Set the locale to support wide characters
  setlocale(LC_ALL, "");

  FILE *file = fopen(filename, "r");
  if (file == NULL) {
    perror("Unable to open file");
    return -1;
  }

  // Validate input line numbers
  if (a < 1 || b < a) {
    fprintf(stderr, "Invalid line range specified\n");
    fclose(file);
    return -1;
  }

  wchar_t line[MAX_LINE_LENGTH];
  int current_line = 0;
  int count = 0;

  // Skip lines until we reach the desired starting line
  while (current_line < a && fgetws(line, MAX_LINE_LENGTH, file)) {
    current_line++;
  }

  // Calculate the number of lines to read
  int lines_to_read = b - a + 1;

  // Allocate memory for the books array
  *books = malloc(lines_to_read * sizeof(Book));
  if (*books == NULL) {
    perror("Memory allocation failed");
    fclose(file);
    return -1;
  }

  // Read lines from line a to line b
  while (current_line <= b && fgetws(line, MAX_LINE_LENGTH, file)) {
    current_line++;

    // Remove newline character if present
    line[wcslen(line) - 1] =
        (line[wcslen(line) - 1] == L'\n') ? L'\0' : line[wcslen(line) - 1];

    // Temporary variables to hold parsed data
    long long isbn;
    wchar_t title[300];
    wchar_t author[150];
    int copies;

    // Split the line using comma delimiter
    wchar_t *token;
    wchar_t *context;

    token = wcstok(line, L",", &context);
    if (token == NULL)
      continue;
    isbn = wcstoll(token, NULL, 10);

    token = wcstok(NULL, L",", &context);
    if (token == NULL)
      continue;
    wcscpy(title, token);

    token = wcstok(NULL, L",", &context);
    if (token == NULL)
      continue;
    wcscpy(author, token);

    token = wcstok(NULL, L",", &context);
    if (token == NULL)
      continue;
    copies = wcstol(token, NULL, 10);

    // Store the parsed data in the books array
    (*books)[count].isbn = isbn;
    wcscpy((*books)[count].title, title);
    wcscpy((*books)[count].author, author);
    (*books)[count].copies = copies;
    (*books)[count].id = current_line - 1;
    count++;
  }

  fclose(file);

  *num_records = count; // Update the count of records read
  return 0;
}

int contains_substring(const wchar_t *str, const wchar_t *substr) {
  return wcsstr(str, substr) != NULL;
}

Book *filter_books(const char *filename, const wchar_t *search_str, int row,
                   const char *mode, int MAX_BOOKS, int *num_books_found) {
  setlocale(LC_ALL, "");

  FILE *file = fopen(filename, "r");
  if (file == NULL) {
    perror("Unable to open file");
    return NULL;
  }

  wchar_t line[MAX_LINE_LENGTH];
  int current_line = 0;
  int count = 0;

  // Allocate memory for the books array
  Book *books = malloc(MAX_BOOKS * sizeof(Book));
  if (books == NULL) {
    perror("Memory allocation failed");
    fclose(file);
    return NULL;
  }

  // Skip lines until we reach the specified starting row
  while (current_line < row && fgetws(line, MAX_LINE_LENGTH, file)) {
    current_line++;
  }

  // Read the file line by line
  while (fgetws(line, MAX_LINE_LENGTH, file) && count < MAX_BOOKS) {
    current_line++;

    // Remove newline character if present
    size_t len = wcslen(line);
    if (len > 0 && line[len - 1] == L'\n') {
      line[len - 1] = L'\0';
    }

    // Temporary variables to hold parsed data
    long long isbn = 0;
    wchar_t title[300] = L"";
    wchar_t author[150] = L"";
    int copies = 0;

    // Parse the line into tokens
    wchar_t *token;
    wchar_t *context;

    token = wcstok(line, L",", &context);
    if (token != NULL)
      isbn = wcstoll(token, NULL, 10);

    token = wcstok(NULL, L",", &context);
    if (token != NULL)
      wcsncpy(title, token, 299);

    token = wcstok(NULL, L",", &context);
    if (token != NULL)
      wcsncpy(author, token, 149);

    token = wcstok(NULL, L",", &context);
    if (token != NULL)
      copies = wcstol(token, NULL, 10);

    // Check for substring based on the mode
    int match_found = 0;
    if (strcmp(mode, "b") == 0) {
      match_found = contains_substring(title, search_str);
    } else if (strcmp(mode, "a") == 0) {
      match_found = contains_substring(author, search_str);
    } else if (strcmp(mode, "ab") == 0 || strcmp(mode, "ba") == 0) {
      match_found = (contains_substring(title, search_str) ||
                     contains_substring(author, search_str));
    }

    // If a match is found, add the book to the array
    if (match_found) {
      books[count].isbn = isbn;
      wcsncpy(books[count].title, title, 299);
      wcsncpy(books[count].author, author, 149);
      books[count].copies = copies;
      books[count].id = current_line;
      count++;
    }

    // Stop if we have reached MAX_BOOKS
    if (count >= MAX_BOOKS) {
      break;
    }
  }

  fclose(file);

  // Update the count of books found
  *num_books_found = count;

  // If no books were found, free the memory and return NULL

  if (count == 0) {
    free(books);
    return NULL;
  }

  return books;
}

Book *window(long long start, int n) {
  const char *filename = "./data/books.csv";
  int a = start;
  int b = start + n - 1;
  Book *library = NULL;
  int num_records = 0;
  read_csv_range(filename, a, b, &library, &num_records);

  return library;
}
