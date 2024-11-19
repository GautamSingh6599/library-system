#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>


#define MAX_LINE_LENGTH 1050
#define MAX_USERNAME_LENGTH 51
#define MAX_ISBN_LENGTH 11
#define MAX_DATE_LENGTH 11

typedef struct {
    char isbn[MAX_ISBN_LENGTH];
    char date[MAX_DATE_LENGTH];
} IssuedBook;

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
        if (token) strncpy(uname, token, sizeof(uname) - 1);
        uname[sizeof(uname) - 1] = '\0';

        token = strtok(NULL, ",");
        if (token) strncpy(isbn, token, sizeof(isbn) - 1);
        isbn[sizeof(isbn) - 1] = '\0';

        token = strtok(NULL, "\n");
        if (token) strncpy(date, token, sizeof(date) - 1);
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
            strncpy(issued_list[*count].isbn, isbn, sizeof(issued_list[*count].isbn) - 1);
            issued_list[*count].isbn[sizeof(issued_list[*count].isbn) - 1] = '\0';

            strncpy(issued_list[*count].date, date, sizeof(issued_list[*count].date) - 1);
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
        perror("Error opening issued.csv");
        return -1;
    }

    while (fgets(line, sizeof(line), issued_file)) {
        char uname[MAX_USERNAME_LENGTH];
        char isbn[MAX_ISBN_LENGTH];

        char *token = strtok(line, ",");
        if (token) strncpy(uname, token, sizeof(uname) - 1);
        uname[sizeof(uname) - 1] = '\0';

        token = strtok(NULL, ",");
        if (token) strncpy(isbn, token, sizeof(isbn) - 1);
        isbn[sizeof(isbn) - 1] = '\0';

        if (strcmp(uname, username) == 0 && strcmp(isbn, targetisbn) == 0) {
            found_in_issued = 1;
            break;
        }
    }
    fclose(issued_file);

    if (found_in_issued) {
        // printf("Error: User '%s' has already issued the book with ISBN '%s'.\n", username, targetisbn);
        perror("The book has been issued by this user already.");
        return -1;
    }

    // Find the book in books.csv and check the number of copies
    FILE *books_file = fopen(books_filename, "r");
    if (books_file == NULL) {
        perror("Error opening books.csv");
        return -1;
    }

    FILE *temp_file = fopen("./data/.books_temp.csv", "w");
    if (temp_file == NULL) {
        perror("Error creating temporary file");
        fclose(books_file);
        return -1;
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
        if (token) strncpy(isbn, token, sizeof(isbn) - 1);
        isbn[sizeof(isbn) - 1] = '\0';

        token = strtok(NULL, ",");
        if (token) strncpy(title, token, sizeof(title) - 1);
        title[sizeof(title) - 1] = '\0';

        token = strtok(NULL, ",");
        if (token) strncpy(author, token, sizeof(author) - 1);
        author[sizeof(author) - 1] = '\0';

        token = strtok(NULL, "\n");
        if (token && title_line == 1) copies = atoi(token);

        if (strcmp(isbn, targetisbn) == 0) {
            book_found = 1;

            // Check the conditions for issuing the book
            if (copies > 3 || (0 < copies < 3 && user_type > 0)) {
                copies--; // Decrement the available copies
                book_issued = 1;
            } else {
                // printf("Error: Book with ISBN '%s' cannot be issued due to insufficient copies.\n", targetisbn);
                perror("Insufficient copies present.");
            }
        }

        // Write the (possibly updated) line to the temp file
        if (title_line == 0) {
            fprintf(temp_file, "%s,%s,%s,copies\n", isbn, title, author);
            title_line++;
        }
        else
            fprintf(temp_file, "%s,%s,%s,%d\n", isbn, title, author, copies);

    }

    fclose(books_file);
    fclose(temp_file);

    if (!book_found) {
        // printf("Error: Book with ISBN '%s' not found in books.csv.\n", targetisbn);
        perror("Book not found.");
        remove("./data/.books_temp.csv");
        return -1;
    }

    if (!book_issued) {
        remove("./data/.books_temp.csv");
        return -1;
    }

    // Replace books.csv with the updated file
    // remove(books_filename);
    rename(books_filename, "./data/.old_books.csv");
    rename("./data/.books_temp.csv", books_filename);

    // Add a new row to issued.csv
    issued_file = fopen(issued_filename, "a");
    if (issued_file == NULL) {
        perror("Error opening issued.csv for appending");
        return -1;
    }

    fprintf(issued_file, "%s,%s,%s\n", username, targetisbn, date);
    fclose(issued_file);

    // printf("Book with ISBN '%s' successfully issued to user '%s'.\n", targetisbn, username);
    return 0;
}


int main() {
    char username[MAX_USERNAME_LENGTH];
    char targetisbn[MAX_ISBN_LENGTH];
    int user_type;

    printf("Enter username: ");
    scanf("%50s", username);
    printf("Enter user type: ");
    scanf("%d", &user_type);
    printf("Enter the ISBN of the book to issue: ");
    scanf("%10s", targetisbn);

    if (issuebook(username, user_type, targetisbn) == 0) {
        printf("Book issued successfully.\n");
    } else {
        printf("Failed to issue book.\n");
    }

    return 0;
}



/*
int main() {
    char username[MAX_USERNAME_LENGTH];
    printf("Enter username: ");
    scanf("%50s", username);

    int count = 0;
    IssuedBook *issued_list = issuedbyuser(username, &count);

    if (issued_list == NULL) {
        printf("No issued books found for the user.\n");
        return 1;
    }

    printf("Books issued by %s:\n", username);
    for (int i = 0; i < count; i++) {
        printf("  - ISBN: %s, Date: %s\n", issued_list[i].isbn, issued_list[i].date);
    }

    // Free the dynamically allocated memory
    free(issued_list);

    return 0;
}

*/
