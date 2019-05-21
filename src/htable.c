#include "htable.h"

unsigned int
htable_hash(const char *str)
{
    unsigned int h=0;
    const unsigned char *p;
    for(p=(const unsigned char*)str; *p!='\0'; p++)
        h = 65599*h + *p;
    return h % HTABLE_SIZE;
}

void
htable_init(htable *table)
{
    memset(table->lists, 0, sizeof(*(table->lists)) * HTABLE_SIZE);
}

void
htable_free(htable *table)
{
    for (int i = 0; i < HTABLE_SIZE; i++) {
        htable_free_list(table->lists[i]);
    }
}

void
htable_free_list(struct htable_item *list)
{
    struct htable_item *iter = list, *tmp = list;

    while(iter) {
        iter = iter->next;
        free(tmp);
        tmp = iter;
    }
}

RETCODE
htable_insert(htable *table, char *key)
{
    struct htable_item* record = htable_get_record(table, key);
    if (record) {
        record->data.occurence++;
    } else {
        unsigned int index = htable_hash(key);
        record = malloc(sizeof(*record));
        if (!record) {
            return INT_ERR;
        }
        record->data.occurence = 1;
        record->data.msg = key;
        record->next = table->lists[index];
        table->lists[index] = record;
    }

    return SUCCES;
}

struct htable_item*
htable_get_record(htable *table, char *key)
{
    unsigned int index = htable_hash(key);
    struct htable_item *iter = table->lists[index];

    while (iter) {
        if (strcmp(iter->data.msg, key)) {
            return iter;
        }
        iter = iter->next;
    }

    return NULL;
}

char *
htable_get_most_frequented(htable *table)
{
    struct htable_item *record = NULL;
    struct htable_data most_frequented = {0, NULL};

    for (int i = 0; i < HTABLE_SIZE; i++) {
        record = table->lists[i];
        while(record) {
            if (record->data.occurence > most_frequented.occurence) {
                most_frequented = record->data;
            }
            record = record->next;
        }
    }

    if (most_frequented.occurence == 0) {
        return NULL;
    }

    return (most_frequented.occurence == 0) ? NULL : most_frequented.msg;
}
