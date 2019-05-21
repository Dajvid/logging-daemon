#ifndef HTABLE_H
#define HTABLE_H

#include "common.h"
#include <string.h>

#define HTABLE_SIZE 256

struct htable_data {
    int occurence;
    char *msg;
};

struct htable_item {
    struct htable_item *next;
    struct htable_data data;
};

typedef struct {
    struct htable_item *lists[HTABLE_SIZE];
} htable;


void htable_init(htable *table);

void htable_free(htable *table);

void htable_free_list(struct htable_item *list);

RETCODE htable_insert(htable *table, char *key);

unsigned int htable_hash(const char *key);

struct htable_item *htable_get_record(htable *table, char *key);

char * htable_get_most_frequented(htable *table);

#endif //HTABLE_H
