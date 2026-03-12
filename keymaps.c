#include <libevdev/libevdev.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "keymaps.h"

void keymap_populate_err(struct keymap *keymap, struct list *store) {
    keymap->a     = key_str_to_int_err(ini_get_str(store, "ButtonMapping", "A"     ));
    keymap->b     = key_str_to_int_err(ini_get_str(store, "ButtonMapping", "B"     ));
    keymap->home  = key_str_to_int_err(ini_get_str(store, "ButtonMapping", "Home"  ));
    keymap->minus = key_str_to_int_err(ini_get_str(store, "ButtonMapping", "Minus" ));
    keymap->plus  = key_str_to_int_err(ini_get_str(store, "ButtonMapping", "Plus"  ));
    keymap->up    = key_str_to_int_err(ini_get_str(store, "ButtonMapping", "Up"    ));
    keymap->down  = key_str_to_int_err(ini_get_str(store, "ButtonMapping", "Down"  ));
    keymap->left  = key_str_to_int_err(ini_get_str(store, "ButtonMapping", "Left"  ));
    keymap->right = key_str_to_int_err(ini_get_str(store, "ButtonMapping", "Right" ));
    keymap->one   = key_str_to_int_err(ini_get_str(store, "ButtonMapping", "1"     ));
    keymap->two   = key_str_to_int_err(ini_get_str(store, "ButtonMapping", "2"     ));
}

int keymap_translate(struct keymap *keymap, int code) {
    int k;

    switch (code) {
        case BTN_A:        k = keymap->a;     break;
        case BTN_B:        k = keymap->b;     break;
        case BTN_MODE:     k = keymap->home;  break;
        case KEY_PREVIOUS: k = keymap->minus; break;
        case KEY_NEXT:     k = keymap->plus;  break;
        case KEY_UP:       k = keymap->up;    break;
        case KEY_DOWN:     k = keymap->down;  break;
        case KEY_LEFT:     k = keymap->left;  break;
        case KEY_RIGHT:    k = keymap->right; break;
        case BTN_1:        k = keymap->one;   break;
        case BTN_2:        k = keymap->two;   break;
        default:           k = SKEY_ERR;      break;
    }

    return k;
}

int key_str_to_int_err(char *str) {
    int code = key_str_to_int(str);

    if (code == SKEY_ERR) {
        fprintf(stderr, "Invalid button mapping: \"");
        fprintf(stderr, str);
        fprintf(stderr, "\"\n");
        exit(1);
    } else {
        return code;
    }
}


int key_str_to_int(char *str) {
    int code = libevdev_event_code_from_name(EV_KEY, str);

    if (code == SKEY_ERR) {
        if (!strcmp(str, "%SCROLL")) {
            code = SKEY_SCROLL;
        } else if (!strcmp(str, "")) {
            code = SKEY_UNBOUND;
        }
    }

    return code;
}
