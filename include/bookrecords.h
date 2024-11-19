#ifndef INCLUDE_INCLUDE_BOOKRECORDS_H_
#define INCLUDE_INCLUDE_BOOKRECORDS_H_
#define MAX_TITLE_LENGTH 300
#define MAX_AUTHOR_LENGTH 150

int book_remove(const char *removeisbn);
int book_add(const char *addisbn, const char *title, const char *author,
             int copies);

int change_copies(const char *copiesisbn, int newcopies);
#endif // INCLUDE_INCLUDE_BOOKRECORDS_H_
