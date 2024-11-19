#include "../include/bookrecords.h"
#include "../include/book.h"
#include "../include/issuing.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Function to remove a book from books.csv by ISBN
int book_remove(const char *removeisbn) {
  const char *books_filename = "./data/books.csv";
  char line[MAX_LINE_LENGTH];
  int found = 0;

  // Open the original file for reading
  FILE *books_file = fopen(books_filename, "r");
  if (books_file == NULL) {
    perror("Error opening books.csv");
    return -1;
  }

  // Create a temporary file for writing updated data
  FILE *temp_books_file = fopen("./data/.books_temp.csv", "w");
  if (temp_books_file == NULL) {
    perror("Error creating temporary file for books.csv");
    fclose(books_file);
    return -1;
  }

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

    if (strcmp(isbn, removeisbn) == 0)
      found = 1;

    // Write the updated or unchanged line to the temp file

    if (title_line == 0) {
      fprintf(temp_books_file, "%s,%s,%s,copies\n", isbn, title, author);
      title_line++;
    } else if (found == 1)
      found = 0;
    else
      fprintf(temp_books_file, "%s,%s,%s,%d\n", isbn, title, author, copies);
  }

  fclose(books_file);
  fclose(temp_books_file);

  // Replace the original books.csv with the updated file
  rename(books_filename, "./data/.old_books.csv");
  rename("./data/.books_temp.csv", books_filename);

  return 0;
}

// Function to add a new book to books.csv
int book_add(const char *addisbn, const char *title, const char *author,
             int copies) {
  const char *books_filename = "./data/books.csv";

  // Open the books.csv file for appending
  FILE *books_file = fopen(books_filename, "a");
  if (books_file == NULL) {
    perror("Error opening books.csv for appending");
    return -1;
  }

  // Write the new book details to the file
  fprintf(books_file, "%s,%s,%s,%d\n", addisbn, title, author, copies);
  fclose(books_file);

  return 0;
}

int change_copies(const char *copiesisbn, int newcopies) {
  const char *books_filename = "./data/books.csv";
  char line[MAX_LINE_LENGTH];
  int found = 0;

  // Open the original file for reading
  FILE *books_file = fopen(books_filename, "r");
  if (books_file == NULL) {
    perror("Error opening books.csv");
    return -1;
  }

  // Create a temporary file for writing updated data
  FILE *temp_books_file = fopen("./data/.books_temp.csv", "w");
  if (temp_books_file == NULL) {
    perror("Error creating temporary file for books.csv");
    fclose(books_file);
    return -1;
  }

  // Read and update the file line by line
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

    // If this is the row to update, modify the copies value
    if (strcmp(isbn, copiesisbn) == 0) {
      copies = newcopies;
      found = 1;
    }

    // Write the updated or unchanged line to the temp file

    if (title_line == 0) {
      fprintf(temp_books_file, "%s,%s,%s,copies\n", isbn, title, author);
      title_line++;
    } else
      fprintf(temp_books_file, "%s,%s,%s,%d\n", isbn, title, author, copies);
  }

  fclose(books_file);
  fclose(temp_books_file);

  if (!found) {
    perror("Book not found.");
    remove("./data/.books_temp.csv");
    return -1;
  }

  // Replace the original books.csv with the updated file
  rename(books_filename, "./data/.old_books.csv");
  rename("./data/.books_temp.csv", books_filename);

  return 0;
}

/*
int main() {
    int choice;
    printf("Select an option:\n");
    printf("1. Remove a book by ISBN\n");
    printf("2. Add a new book\n");
    printf("Enter your choice: ");
    scanf("%d", &choice);

    if (choice == 1) {
        char removeisbn[MAX_ISBN_LENGTH];
        printf("Enter ISBN of the book to remove: ");
        scanf("%10s", removeisbn);

        if (book_remove(removeisbn) == 0) {
            printf("Book removed successfully.\n");
        } else {
            printf("Failed to remove book.\n");
        }
    } else if (choice == 2) {
        char addisbn[MAX_ISBN_LENGTH];
        char title[MAX_TITLE_LENGTH];
        char author[MAX_AUTHOR_LENGTH];
        int copies;

        printf("Enter ISBN of the book to add: ");
        scanf("%10s", addisbn);
        getchar();
        printf("Enter title of the book: ");
        fgets(title, sizeof(title), stdin);
        title[strcspn(title, "\n")] = '\0';
        printf("Enter author of the book: ");
        fgets(author, sizeof(author), stdin);
        author[strcspn(author, "\n")] = '\0';
        printf("Enter number of copies: ");
        scanf("%d", &copies);

        if (book_add(addisbn, title, author, copies) == 0) {
            printf("Book added successfully.\n");
        } else {
            printf("Failed to add book.\n");
        }
    } else {
        printf("Invalid choice.\n");
    }

    return 0;
}
*/

/*
int main() {
    char isbn[MAX_ISBN_LENGTH];
    int newcopies;

    printf("Enter ISBN of the book to update copies: ");
    scanf("%10s", isbn);
    printf("Enter new number of copies: ");
    scanf("%d", &newcopies);

    if (change_copies(isbn, newcopies) == 0) {
        printf("Copies updated successfully.\n");
    } else {
        printf("Failed to update copies.\n");
    }

    return 0;
}

*/
