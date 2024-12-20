#pragma once
#ifndef INCLUDE_INCLUDE_BOOK_H_
#define INCLUDE_INCLUDE_BOOK_H_

#include <locale.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <wchar.h>
#include <wctype.h>
#define MAX_LINE_LENGTH 1050

typedef struct {
  long long id;
  long long isbn;
  wchar_t title[300];
  wchar_t author[150];
  int copies;
} Book;

wchar_t *to_lowercase(const wchar_t *str);
Book *window(long long start, int n);
int read_csv_range(const char *filename, int a, int b, Book **books,
                   int *num_records);

int contains_substring(const wchar_t *str, const wchar_t *substr);
Book *filter_books(const char *filename, const wchar_t *search_str, int row,
                   const char *mode, int MAX_BOOKS, int *num_books_found);
#endif
