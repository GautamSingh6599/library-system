// Copyright [2024]
// Gautam Singh
#include <locale.h>
#include <stdlib.h>
#include <string.h>
#include <wchar.h>

#pragma once
#ifndef INCLUDE_INCLUDE_BOOK_H_
#define INCLUDE_INCLUDE_BOOK_H_

#define MAX_LINE_LENGTH 1024

typedef struct {
  long long id;
  long long isbn;
  wchar_t title[300];
  wchar_t author[150];
  int copies;
} Book;

Book *window(long long start, int n);
int read_csv_range(const char *filename, int a, int b, Book **books,
                   int *num_records);

int contains_substring(const wchar_t *str, const wchar_t *substr);
Book *filter_books(const char *filename, const wchar_t *search_str, int row,
                   const char *mode, int MAX_BOOKS, int *num_books_found);
#endif
