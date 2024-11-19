#include "../include/issuing.h"
#include "../include/book.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

void get_current_date(char *date_str, size_t max_len) {
  time_t t = time(NULL);
  struct tm *tm_info = localtime(&t);
  strftime(date_str, max_len, "%Y-%m-%d", tm_info);
}

IssuedBook *issuedbyuser(const char *username, int *count) {
  const char *filename = "./data/issued.csv";
  char line[MAX_LINE_LENGTH];
  IssuedBook *issued_list = NULL; // Dynamically allocated list of issued books
  *count = 0;

  // Open the file for reading
  FILE *file = fopen(filename, "r");
  if (file == NULL) {
    perror("Error opening file");
    return NULL;
  }

  // Parse each line in the CSV
  while (fgets(line, sizeof(line), file)) {
    char uname[MAX_USERNAME_LENGTH];
    char isbn[MAX_ISBN_LENGTH];
    char date[MAX_DATE_LENGTH];

    // Tokenize the line
    char *token = strtok(line, ",");
    if (token)
      strncpy(uname, token, sizeof(uname) - 1);
    uname[sizeof(uname) - 1] = '\0';

    token = strtok(NULL, ",");
    if (token)
      strncpy(isbn, token, sizeof(isbn) - 1);
    isbn[sizeof(isbn) - 1] = '\0';

    token = strtok(NULL, "\n");
    if (token)
      strncpy(date, token, sizeof(date) - 1);
    date[sizeof(date) - 1] = '\0';

    // Check if the username matches
    if (strcmp(uname, username) == 0) {
      // Allocate memory for a new IssuedBook
      issued_list = realloc(issued_list, (*count + 1) * sizeof(IssuedBook));
      if (issued_list == NULL) {
        perror("Error reallocating memory");
        fclose(file);
        return NULL;
      }

      // Store the ISBN and date in the list
      strncpy(issued_list[*count].isbn, isbn,
              sizeof(issued_list[*count].isbn) - 1);
      issued_list[*count].isbn[sizeof(issued_list[*count].isbn) - 1] = '\0';

      strncpy(issued_list[*count].date, date,
              sizeof(issued_list[*count].date) - 1);
      issued_list[*count].date[sizeof(issued_list[*count].date) - 1] = '\0';

      (*count)++;
    }
  }
  fclose(file);

  return issued_list;
}

// Function to issue a book
int issuebook(const char *username, int user_type, const char *targetisbn) {
  const char *issued_filename = "./data/issued.csv";
  const char *books_filename = "./data/books.csv";
  char line[MAX_LINE_LENGTH];
  char date[MAX_DATE_LENGTH];
  int found_in_issued = 0;

  // Get today's date
  get_current_date(date, sizeof(date));

  // Check if the user already issued the target ISBN
  FILE *issued_file = fopen(issued_filename, "r");
  if (issued_file == NULL) {
    /*perror("Error opening issued.csv");*/
    return -1;
  }

  while (fgets(line, sizeof(line), issued_file)) {
    char uname[MAX_USERNAME_LENGTH];
    char isbn[MAX_ISBN_LENGTH];

    char *token = strtok(line, ",");
    if (token)
      strncpy(uname, token, sizeof(uname) - 1);
    uname[sizeof(uname) - 1] = '\0';

    token = strtok(NULL, ",");
    if (token)
      strncpy(isbn, token, sizeof(isbn) - 1);
    isbn[sizeof(isbn) - 1] = '\0';

    if (strcmp(uname, username) == 0 && strcmp(isbn, targetisbn) == 0) {
      found_in_issued = 1;
      break;
    }
  }
  fclose(issued_file);

  if (found_in_issued) {
    // printf("Error: User '%s' has already issued the book with ISBN '%s'.\n",
    // username, targetisbn);
    /*perror("The book has been issued by this user already.");*/
    return -2;
  }

  // Find the book in books.csv and check the number of copies
  FILE *books_file = fopen(books_filename, "r");
  if (books_file == NULL) {
    /*perror("Error opening books.csv");*/
    return -3;
  }

  FILE *temp_file = fopen("./data/.books_temp.csv", "w");
  if (temp_file == NULL) {
    /*perror("Error creating temporary file");*/
    fclose(books_file);
    return -4;
  }

  int book_found = 0;
  int book_issued = 0;

  int title_line = 0;
  while (fgets(line, sizeof(line), books_file)) {
    char isbn[MAX_ISBN_LENGTH];
    char title[MAX_LINE_LENGTH], author[MAX_LINE_LENGTH];
    int copies;

    // Parse the line
    char *token = strtok(line, ",");
    if (token)
      strncpy(isbn, token, sizeof(isbn) - 1);
    isbn[sizeof(isbn) - 1] = '\0';

    token = strtok(NULL, ",");
    if (token)
      strncpy(title, token, sizeof(title) - 1);
    title[sizeof(title) - 1] = '\0';

    token = strtok(NULL, ",");
    if (token)
      strncpy(author, token, sizeof(author) - 1);
    author[sizeof(author) - 1] = '\0';

    token = strtok(NULL, "\n");
    if (token && title_line == 1)
      copies = atoi(token);

    if (strcmp(isbn, targetisbn) == 0) {
      book_found = 1;

      // Check the conditions for issuing the book
      if (copies > 3 || ((0 < copies && copies <= 3) && user_type > 0)) {
        copies--; // Decrement the available copies
        book_issued = 1;
      } else {
        // printf("Error: Book with ISBN '%s' cannot be issued due to
        // insufficient copies.\n", targetisbn);
        /*perror("Insufficient copies present.");*/
        return -5;
      }
    }

    // Write the (possibly updated) line to the temp file
    if (title_line == 0) {
      fprintf(temp_file, "%s,%s,%s,copies\n", isbn, title, author);
      title_line++;
    } else
      fprintf(temp_file, "%s,%s,%s,%d\n", isbn, title, author, copies);
  }

  fclose(books_file);
  fclose(temp_file);

  if (!book_found) {
    // printf("Error: Book with ISBN '%s' not found in books.csv.\n",
    // targetisbn);
    /*perror("Book not found.");*/
    remove("./data/.books_temp.csv");
    return -6;
  }

  if (!book_issued) {
    remove("./data/.books_temp.csv");
    return -7;
  }

  // Replace books.csv with the updated file
  // remove(books_filename);
  rename(books_filename, "./data/.old_books.csv");
  rename("./data/.books_temp.csv", books_filename);

  // Add a new row to issued.csv
  issued_file = fopen(issued_filename, "a");
  if (issued_file == NULL) {
    /*perror("Error opening issued.csv for appending");*/
    return -8;
  }

  fprintf(issued_file, "%s,%s,%s\n", username, targetisbn, date);
  fclose(issued_file);

  // printf("Book with ISBN '%s' successfully issued to user '%s'.\n",
  // targetisbn, username);
  return 0;
}

int returnbook(const char *username, const char *returnisbn) {
  const char *issued_filename = "./data/issued.csv";
  const char *books_filename = "./data/books.csv";
  char line[MAX_LINE_LENGTH];
  int found_in_issued = 0;

  FILE *issued_file = fopen(issued_filename, "r");
  if (issued_file == NULL) {
    perror("Error opening issued.csv");
    return -1;
  }

  FILE *temp_issued_file = fopen("./data/.issued_temp.csv", "w");
  if (temp_issued_file == NULL) {
    perror("Error creating temporary file to modify issued.csv");
    fclose(issued_file);
    return -1;
  }

  while (fgets(line, sizeof(line), issued_file)) {
    char uname[MAX_USERNAME_LENGTH];
    char isbn[MAX_ISBN_LENGTH];

    char *token = strtok(line, ",");
    if (token)
      strncpy(uname, token, sizeof(uname) - 1);
    uname[sizeof(uname) - 1] = '\0';

    token = strtok(NULL, ",");
    if (token)
      strncpy(isbn, token, sizeof(isbn) - 1);
    isbn[sizeof(isbn) - 1] = '\0';

    // Skip writing the row with the book to be returned into the temp file
    if (strcmp(uname, username) == 0 && strcmp(isbn, returnisbn) == 0) {
      found_in_issued = 1;
      continue;
    }

    // Write all other rows to the temp file
    fprintf(temp_issued_file, "%s,%s,%s\n", uname, isbn, strtok(NULL, "\n"));
  }

  fclose(issued_file);
  fclose(temp_issued_file);

  if (!found_in_issued) {
    // printf("Error: No record found for user '%s' with ISBN '%s' in
    // issued.csv.\n", username, returnisbn);
    perror("Error: User does not have this book issued.");
    remove("issued_temp.csv");
    return -1;
  }

  // Replace the original issued.csv with the updated file
  rename(issued_filename, "./data/.old_issued.csv");
  rename("./data/.issued_temp.csv", issued_filename);

  FILE *books_file = fopen(books_filename, "r");
  if (books_file == NULL) {
    perror("Error opening books.csv");
    return -1;
  }

  FILE *temp_books_file = fopen("./data/.books_temp.csv", "w");
  if (temp_books_file == NULL) {
    perror("Error creating temporary file for modifying books.csv");
    fclose(books_file);
    return -1;
  }

  int book_found = 0;

  int title_line = 0;
  while (fgets(line, sizeof(line), books_file)) {
    char isbn[MAX_ISBN_LENGTH];
    char title[MAX_LINE_LENGTH], author[MAX_LINE_LENGTH];
    int copies;

    // Parse the line
    char *token = strtok(line, ",");
    if (token)
      strncpy(isbn, token, sizeof(isbn) - 1);
    isbn[sizeof(isbn) - 1] = '\0';

    token = strtok(NULL, ",");
    if (token)
      strncpy(title, token, sizeof(title) - 1);
    title[sizeof(title) - 1] = '\0';

    token = strtok(NULL, ",");
    if (token)
      strncpy(author, token, sizeof(author) - 1);
    author[sizeof(author) - 1] = '\0';

    token = strtok(NULL, "\n");
    if (token && title_line == 1)
      copies = atoi(token);

    // If this is the row to update, increment copies
    if (strcmp(isbn, returnisbn) == 0) {
      copies++;
      book_found = 1;
    }

    // Write the (possibly updated) line to the temp file
    if (title_line == 0) {
      fprintf(temp_books_file, "%s,%s,%s,copies\n", isbn, title, author);
      title_line++;
    } else
      fprintf(temp_books_file, "%s,%s,%s,%d\n", isbn, title, author, copies);
  }

  fclose(books_file);
  fclose(temp_books_file);

  if (!book_found) {
    // printf("Error: Book with ISBN '%s' not found in books.csv.\n",
    // returnisbn);
    perror("Book record not found in database.");
    remove("./data/.books_temp.csv");
    return -1;
  }

  // Replace the original books.csv with the updated file
  rename(books_filename, "./data/.old_books.csv");
  rename("./data/.books_temp.csv", books_filename);

  return 0;
}
