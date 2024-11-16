// Copyright
// Gautam Singh

#include <iconv.h>
#include <locale.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <wchar.h>

typedef struct {
  long long slno;
  long long ISBN;
  wchar_t title[300];
  wchar_t author[150];
  int yearPub;
  wchar_t publisher[150];
  int copies;
} Book;

// Since the library is a csv database
// count the number of \n characters to determine the number
// of of lines, which will give us number of books.

int lines() {
  setlocale(LC_ALL, "en_US.UTF-8");

  // Open the file in read mode
  FILE *file = fopen("final_books.csv", "r");
  if (file == NULL) {
    perror("Error opening library database.");
    return EXIT_FAILURE;
  }

  // Set up iconv for converting ISO-8859-1 to UTF-8
  iconv_t cd = iconv_open("UTF-8", "ISO-8859-1");
  if (cd == (iconv_t)-1) {
    perror("Error opening iconv conversion");
    fclose(file);
    return EXIT_FAILURE;
  }

  int count = 0;
  char inbuf[1024], outbuf[1024];
  size_t inbytesleft, outbytesleft;

  while (fgets(inbuf, sizeof(inbuf), file)) {
    inbytesleft = strlen(inbuf);
    outbytesleft = sizeof(outbuf);

    char *inptr = inbuf;
    char *outptr = outbuf;

    // Perform encoding conversion from ISO-8859-1 to UTF-8
    size_t ret = iconv(cd, &inptr, &inbytesleft, &outptr, &outbytesleft);
    if (ret == (size_t)-1) {
      perror("Error converting encoding");
      continue;
    }

    // Now outbuf contains the UTF-8 version of the line
    // Process this line to count newlines
    for (int i = 0; outbuf[i] != '\0'; i++) {
      if (outbuf[i] == '\n') {
        count++;
      }
    }
  }

  // Close iconv and file
  iconv_close(cd);
  fclose(file);
  return count;
}

int readFile(Book *library, int numBooks) {
  FILE *file = fopen("final_books.csv", "r, ccs=UTF-8");
  if (file == NULL) {
    perror("Error opening library database.");
    return EXIT_FAILURE;
  }

  wchar_t line[1024];
  int i = 0;

  // Skip the header row
  fgetws(line, sizeof(line) / sizeof(wchar_t), file);

  while (fgetws(line, sizeof(line) / sizeof(wchar_t), file) != NULL &&
         i < numBooks) {
    // Remove trailing newline characters
    line[wcslen(line) - 1] =
        (line[wcslen(line) - 1] == L'\n') ? L'\0' : line[wcslen(line) - 1];
    line[wcslen(line) - 1] =
        (line[wcslen(line) - 1] == L'\r') ? L'\0' : line[wcslen(line) - 1];

    // Debug: print the line read from the file

    wchar_t *token;
    wchar_t *saveptr;

    // Use wcstok to parse each field
    token = wcstok(line, L",", &saveptr);
    if (token != NULL)
      library[i].slno = wcstoll(token, NULL, 10);

    token = wcstok(NULL, L",", &saveptr);
    if (token != NULL)
      library[i].ISBN = wcstoll(token, NULL, 10);

    token = wcstok(NULL, L",", &saveptr);
    if (token != NULL)
      wcsncpy(library[i].title, token, 299);
    library[i].title[299] = L'\0'; // Ensure null termination

    token = wcstok(NULL, L",", &saveptr);
    if (token != NULL)
      wcsncpy(library[i].author, token, 149);
    library[i].author[149] = L'\0';

    token = wcstok(NULL, L",", &saveptr);
    if (token != NULL)
      library[i].yearPub = wcstol(token, NULL, 10);

    token = wcstok(NULL, L",", &saveptr);
    if (token != NULL)
      wcsncpy(library[i].publisher, token, 149);
    library[i].publisher[149] = L'\0';

    token = wcstok(NULL, L",", &saveptr);
    if (token != NULL)
      library[i].copies = wcstol(token, NULL, 10);

    // Move to the next book entry
    i++;
  }

  fclose(file);
  return EXIT_SUCCESS;
}

void print_books(Book *books, int num_books) {
  // Ensure the locale is set for wide characters
  setlocale(LC_ALL, "en_US.UTF-8");

  // Print the table header with appropriate column widths
  wprintf(L"| %-5ls | %-13ls | %-50ls | %-30ls | %-4ls | %-30ls | %-7ls |\n",
          L"ID", L"ISBN", L"Title", L"Author", L"Year", L"Publisher",
          L"Copies");

  // Print the separator line
  wprintf(
      L"|-------|---------------|--------------------------------------------"
      L"--------|--------------------------------|------|------------------"
      L"--------------|---------|\n");

  // Print each book's details
  for (int i = 0; i < num_books; i++) {
    // Ensure that long strings are truncated to fit within the columns
    wchar_t truncated_title[51];     // Allow up to 50 characters for title
    wchar_t truncated_author[31];    // Allow up to 30 characters for author
    wchar_t truncated_publisher[31]; // Allow up to 30 characters for publisher

    // Truncate and null-terminate
    wcsncpy(truncated_title, books[i].title, 50);
    truncated_title[50] = L'\0';

    wcsncpy(truncated_author, books[i].author, 30);
    truncated_author[30] = L'\0';

    wcsncpy(truncated_publisher, books[i].publisher, 30);
    truncated_publisher[30] = L'\0';

    // Print the book data row using %ls for wide strings
    wprintf(L"| %-5lld | %-13lld | %-50ls | %-30ls | %-4d | %-30ls | %-7d |\n",
            books[i].slno, books[i].ISBN, truncated_title, truncated_author,
            books[i].yearPub, truncated_publisher, books[i].copies);
  }

  // Print the bottom border of the table
  wprintf(
      L"|-------|---------------|--------------------------------------------"
      L"--------|--------------------------------|------|------------------"
      L"--------------|---------|\n");
}

int main(int argc, char *argv[]) {
  int numBooks = lines();
  Book *library = malloc(numBooks * sizeof(Book));
  if (library == NULL) {
    perror("Memory allocation failed for library storage.");
    return EXIT_FAILURE;
  }

  if (readFile(library, numBooks) == EXIT_FAILURE) {
    free(library);
    perror("Failed to read library database.");
    return EXIT_FAILURE;
  }

  print_books(library, numBooks);
  free(library);

  return EXIT_SUCCESS;
}
