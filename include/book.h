// Copyright [2024]
// Gautam Singh
#include <locale.h>
#include <string.h>
#include <wchar.h>

#pragma once
#ifndef INCLUDE_INCLUDE_BOOK_H_
#define INCLUDE_INCLUDE_BOOK_H_

#define EXIT_SUCCEESS 1

typedef struct {
  long long id;
  long long isbn;
  wchar_t title[300];
  wchar_t author[150];
  int copies;
} Book;

Book *window(long long start, int n);

// temporary
extern Book library[10];
#endif
