#include <openssl/sha.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define MAX_LINE_LENGTH 256
#define SALT_LENGTH 10
#define HASH_LENGTH 65 // SHA256 hash (64 chars) + '\0'

// Function to generate a random string of given length
void generate_random_string(char *str, size_t length) {
  const char charset[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
  size_t charset_size = sizeof(charset) - 1;

  for (size_t i = 0; i < length; ++i) {
    str[i] = charset[rand() % charset_size];
  }
  str[length] = '\0';
}

// Function to compute the SHA256 hash of the salted password
void compute_sha256(const char *input, char *output) {
  unsigned char hash[SHA256_DIGEST_LENGTH];
  SHA256((unsigned char *)input, strlen(input), hash);

  for (int i = 0; i < SHA256_DIGEST_LENGTH; ++i) {
    sprintf(output + (i * 2), "%02x", hash[i]);
  }
  output[HASH_LENGTH - 1] = '\0';
}

// Main signup function
int signup(const char *username, const char *password, int user_type) {
  const char *filename = "./data/users.csv";
  char line[MAX_LINE_LENGTH];
  int user_exists = 0;

  srand((unsigned)time(NULL));

  // Open file for reading
  FILE *file = fopen(filename, "r");
  if (file == NULL) {
    perror("Error opening the users database.");
    return -1;
  }

  // Check if the username already exists
  while (fgets(line, sizeof(line), file)) {
    char *token = strtok(line, ",");
    if (token && strcmp(token, username) == 0) {
      user_exists = 1;
      break;
    }
  }
  fclose(file);

  if (user_exists) {
    /*perror("Username already exists.");*/
    return -1;
  }

  // Generate the salt
  char salt[SALT_LENGTH + 1];
  generate_random_string(salt, SALT_LENGTH);

  // Generate the hash of the salted password
  char combined[81]; // password + salt
  snprintf(combined, sizeof(combined), "%s%s", password, salt);

  char hash[HASH_LENGTH];
  compute_sha256(combined, hash);

  // Open file for appending
  file = fopen(filename, "a");
  if (file == NULL) {
    /*perror("Error opening the users database");*/
    return -1;
  }

  // Append new user data
  fprintf(file, "%s,%s,%s,%d\n", username, salt, hash, user_type);
  fclose(file);

  return 0;
}

int login(const char *username, const char *password, int *user_type) {
  const char *filename = "./data/users.csv";
  char line[MAX_LINE_LENGTH];
  char file_username[71], salt[SALT_LENGTH + 1], spash[HASH_LENGTH];
  int user_found = 0;
  int found_user_type = -1;

  // Open file for reading
  FILE *file = fopen(filename, "r");
  if (file == NULL) {
    perror("Error opening the users database");
    return -1;
  }

  // Search for the username in the CSV
  while (fgets(line, sizeof(line), file)) {
    char *token = strtok(line, ",");
    if (token) {
      strncpy(file_username, token, sizeof(file_username) - 1);
      file_username[sizeof(file_username) - 1] = '\0';

      if (strcmp(file_username, username) == 0) {
        user_found = 1;
        token = strtok(NULL, ","); // Get salt
        if (token)
          strncpy(salt, token, SALT_LENGTH);
        salt[SALT_LENGTH] = '\0';

        token = strtok(NULL, "\n"); // Get spash (hash of the salted password)
        if (token)
          strncpy(spash, token, HASH_LENGTH);
        spash[HASH_LENGTH - 1] = '\0';
        break;
      }
    }
  }
  fclose(file);

  if (!user_found) {
    /*perror("Error: Username does not exist.");*/
    return -1;
  }

  // Append salt to the password
  char combined[81]; // password + salt
  snprintf(combined, sizeof(combined), "%s%s", password, salt);

  // Compute the hash
  char hash[HASH_LENGTH];
  compute_sha256(combined, hash);

  // Compare computed hash with stored hash
  if (strcmp(hash, spash) == 0) {
    if (user_type != NULL) {
      *user_type = found_user_type;
    }
    return 0;
  } else {
    /*perror("Error: Incorrect password.\n");*/
    return -2;
  }
}

int main() {

  char username[71], password[71];

  printf("Enter username: ");
  scanf("%70s", username);
  printf("Enter password: ");
  scanf("%70s", password);

  // int sneed;
  // printf("Enter type: ");
  // scanf("%d", &sneed);
  // signup(username, password, sneed);

  int sneed = 34;
  login(username, password, &sneed);
  printf("\n%d\n", sneed);

  return 0;
}
