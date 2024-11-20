#pragma once
#ifndef INCLUDE_INCLUDE_ACCOUNTS_H_
#define INCLUDE_INCLUDE_ACCOUNTS_H_
#include <openssl/sha.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

void generate_random_string(char *str, size_t length);
void compute_sha256(const char *input, char *output);
int signup(const char *username, const char *password, int user_type);
int login(const char *username, const char *password, int *user_type);

#endif
