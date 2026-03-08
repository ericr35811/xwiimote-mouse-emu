#ifndef _EVDEV_HELPERS_H
#define _EVDEV_HELPERS_H

int match_device(struct libevdev **outdev, char* pattern);

int print_event(struct input_event *ev);

#endif