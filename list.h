#ifndef _LIST_H
#define _LIST_H

struct node {
    char *key;
    void *value;
    struct node *next;
};

struct list {
    struct node *head;
    struct node *tail;
    unsigned int length;
};

struct node *node_new(char *key, void *value);
struct list *list_new();
void list_free(struct list *list);
void list_add(struct list *list, struct node *node);
struct node *list_addval(struct list *list, char *key, void *value);
struct node *list_find(struct list *list, char *key);
void *list_findval(struct list *list, char *key);

#endif