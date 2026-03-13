#ifndef _UINPUT_H
#define _UINPUT_H

#include "keymaps.h"

struct libevdev_uinput *uinput_create(int fd, struct keymap *keymap);

#endif      