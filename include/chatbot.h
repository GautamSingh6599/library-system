#pragma once
#ifndef INCLUDE_INCLUDE_CHATBOT_H_
#define INCLUDE_INCLUDE_CHATBOT_H_

#include "../include/chatbot.h"
#include <ctype.h>
#include <stdio.h>
#include <string.h>
#define MAX_STRINGS 200
#define MAX_WORDS 200
#define MAX_WORD_LEN 50
#define MAX_SUBLISTS 50

const char *find_answer(const char *input,
                        const char *knowledge_base[MAX_SUBLISTS][MAX_STRINGS]);
void strip_string(const char *input, char *output);
const extern char *knowledge_base[MAX_SUBLISTS][MAX_STRINGS];
int tokenize(const char *str, char tokens[MAX_WORDS][MAX_WORD_LEN]);
double compute_similarity(char tokens1[MAX_WORDS][MAX_WORD_LEN], int len1,
                          char tokens2[MAX_WORDS][MAX_WORD_LEN], int len2);

#endif // INCLUDE_INCLUDE_CHATBOT_H_
