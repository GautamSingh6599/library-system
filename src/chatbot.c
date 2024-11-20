#include "../include/chatbot.h"
#include <ctype.h>
#include <stdio.h>
#include <string.h>

const char *knowledge_base[MAX_SUBLISTS][MAX_STRINGS] = {
    {"You navigate using vim keybindings.", "How do I move around?",
     "How do I navigate?", "How to scroll?", "How do I scroll?"},
    {"You can issue up to five books at a time.", "How many books can I issue?",
     "How many books can I borrow?"},
    {"A student can only issue a book that has more than three copies "
     "available.",
     "Why can't I borrow this book?", "Why am I unable to borrow this book?",
     "What privileges do I have?"},
    {"You can ask me anything about how to use the library interface.",
     "What can I ask you?", "What do you do?", "What can you do?"},
    {"You press colon and then type `a/foo` or `b/foo` or `ab/foo` to search in authors, books or both.",
    "How do I search for a book?", "How do I find a book?", "How do find an author?",
    "How do I search for an author?"}

};

void strip_string(const char *input, char *output) {
  int j = 0;
  for (int i = 0; input[i] != '\0'; i++) {
    if (isalnum(input[i]) || isspace(input[i])) {
      output[j++] = tolower(input[i]);
    }
  }
  output[j] = '\0';
}

int tokenize(const char *str, char tokens[MAX_WORDS][MAX_WORD_LEN]) {
  int count = 0;
  char temp[strlen(str) + 1];
  strcpy(temp, str);

  char *token = strtok(temp, " ");
  while (token != NULL && count < MAX_WORDS) {
    strcpy(tokens[count++], token);
    token = strtok(NULL, " ");
  }
  return count;
}

// The degree of similarity is defined to be the ratio of common words to total
// words
double compute_similarity(char tokens1[MAX_WORDS][MAX_WORD_LEN], int len1,
                          char tokens2[MAX_WORDS][MAX_WORD_LEN], int len2) {
  int common_words = 0;

  for (int i = 0; i < len1; i++) {
    for (int j = 0; j < len2; j++) {
      if (strcmp(tokens1[i], tokens2[j]) == 0) {
        common_words++;
        break;
      }
    }
  }

  return (double)common_words /
         len2; // Ratio of common words to total words in the question
}

const char *find_answer(const char *input,
                        const char *knowledge_base[MAX_SUBLISTS][MAX_STRINGS]) {
  char stripped_input[MAX_WORD_LEN * MAX_WORDS];
  strip_string(input, stripped_input);

  char input_tokens[MAX_WORDS][MAX_WORD_LEN];
  int input_len = tokenize(stripped_input, input_tokens);

  double max_similarity = -1.0;
  const char *best_answer = NULL;

  int num_sublists = 0;
  while (knowledge_base[num_sublists][0] != NULL) {
    num_sublists++;
  }

  for (int i = 0; i < num_sublists; i++) {
    const char *answer =
        knowledge_base[i][0]; // The first element is the answer

    for (int j = 1; knowledge_base[i][j] != NULL; j++) {
      char stripped_question[MAX_WORD_LEN * MAX_WORDS];
      strip_string(knowledge_base[i][j], stripped_question);

      char question_tokens[MAX_WORDS][MAX_WORD_LEN];
      int question_len = tokenize(stripped_question, question_tokens);

      double similarity = compute_similarity(input_tokens, input_len,
                                             question_tokens, question_len);
      if (similarity > max_similarity) {
        max_similarity = similarity;
        best_answer = answer;
      }
    }
  }

  return best_answer;
}

/*
int main() {


    // const char *input = "How do I navigate?";

    char input[MAX_WORD_LEN * MAX_WORDS];
    printf("Enter your question: ");
    scanf("%[^\n]%*c", input);

    const char *answer = find_answer(input, knowledge_base);

    printf("Input: %s\n", input);
    printf("Answer: %s\n", answer);

    return 0;
}
*/
