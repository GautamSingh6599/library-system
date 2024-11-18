#include <openssl/sha.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#pragma once
#ifndef INCLUDE_INCLUDE_ACCOUNTS_H_
#define INCLUDE_INCLUDE_ACCOUNTS_H_

int signup(const char *username, const char *password, int user_type);
int login(const char *username, const char *password, int *user_type);

#endif