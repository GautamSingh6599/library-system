#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define MAX_USERNAME_LENGTH 51
#define MAX_ISBN_LENGTH 11
#define MAX_DATE_LENGTH 11

typedef struct {
  char isbn[MAX_ISBN_LENGTH];
  char date[MAX_DATE_LENGTH];
} IssuedBook;

void get_current_date(char *date_str, size_t max_len);
IssuedBook *issuedbyuser(const char *username, int *count);
int returnbook(const char *username, const char *returnisbn);
int issuebook(const char *username, int user_type, const char *targetisbn);
