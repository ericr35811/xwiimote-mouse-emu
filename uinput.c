#include <libevdev/libevdev.h> 
#include <libevdev/libevdev-uinput.h>
#include <stdlib.h>
#include <stdio.h>

struct libevdev_uinput *uinput_create(int fd) {
    int rc;
    struct libevdev *dev;
    struct libevdev_uinput *ui;


    dev = libevdev_new();
    libevdev_set_name(dev, "test device");
    
    libevdev_enable_event_type(dev, EV_REL);
    libevdev_enable_event_code(dev, EV_REL, REL_X, NULL);
    libevdev_enable_event_code(dev, EV_REL, REL_Y, NULL);

    // this is required for it to be seen as a mouse i guess
    libevdev_enable_event_type(dev, EV_KEY);
    libevdev_enable_event_code(dev, EV_KEY, BTN_LEFT, 0);

    libevdev_enable_event_type(dev, EV_SYN);
    libevdev_enable_event_code(dev, EV_SYN, SYN_REPORT, NULL);

    rc = libevdev_uinput_create_from_device(
        dev, fd, &ui
    );

    if (rc != 0)
        return NULL;
    else
        return ui;
}

