#ifndef _KEYMAPS_H
#define _KEYMAPS_H

#include "ini.h" 

enum special_keys {
    SKEY_ERR = -1,
    SKEY_UNBOUND = KEY_MAX+1,
    SKEY_SCROLL,
};

struct keymap {
    int a;
    int b;
    int home;
    int minus;
    int plus;
    int up;
    int down;
    int left;
    int right;
    int one;
    int two;
};

int keymap_translate(struct keymap *keymap, int code);
void keymap_populate_err(struct keymap *keymap, struct list *store);
int key_str_to_int_err(char *str);
int key_str_to_int(char *str);


#endif