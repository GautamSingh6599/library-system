#ifndef INCLUDE_INCLUDE_CHATBOT_H_
#define INCLUDE_INCLUDE_CHATBOT_H_

#define MAX_STRINGS 200
#define MAX_WORDS 200
#define MAX_WORD_LEN 50
#define MAX_SUBLISTS 50

const char *find_answer(const char *input,
                        const char *knowledge_base[MAX_SUBLISTS][MAX_STRINGS]);

const extern char *knowledge_base[MAX_SUBLISTS][MAX_STRINGS];

#endif // INCLUDE_INCLUDE_CHATBOT_H_
