#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "list.h" 

struct node *node_new(char *key, void *value) {
    struct node *node;

    node = malloc(sizeof(struct node));
    node->key = key;
    node->value = value;
    node->next = NULL;

    return node;
}

void list_add(struct list *list, struct node *node) {
    if (list->head == NULL) {
        list->head = node;
        list->tail = node;
    } else {
        list->tail->next = node;
        list->tail = node;
    }
    
    list->length++;
}

struct node* list_addval(struct list *list, char *key, void *value) {
    struct node *node;
    node = node_new(key, value);
    list_add(list, node);
    return node;
}

struct list *list_new() {
    struct list *list;

    list = malloc(sizeof(struct list));
    list->head = NULL;
    list->tail = NULL;
    list->length = 0;
    return list;
}

void list_free(struct list *list) {
    struct node *node;
    struct node *next;

    node = list->head;
    while (node) {
        next = node->next;
        free(node);
        node = next;
    };
}

struct node *list_find(struct list *list, char *key) {
    struct node *node;

    node = list->head;
    while (node) {
        if (!strcmp(key, node->key)) {
            return node;
        } else {
            node = node->next;
        }
    }

    return NULL;
}

void *list_findval(struct list *list, char *key) {
    return list_find(list, key)->value;    
}
