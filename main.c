#include <libevdev/libevdev.h>  // libevdev_*()
#include <stdlib.h>             // exit()
#include <stdio.h>              // printf(), fprintf()
#include <unistd.h>             // close()
#include <errno.h>              // EAGAIN
#include "evdev_helpers.h"      // match_device()

int main() {
  int rc, fd;
  struct libevdev *dev = NULL;

  rc = match_device(&dev, "^Nintendo Wii Remote Motion Plus$"); // will error out and exit if a device is not found

  if (rc != 0) {
    return 1;
  }

  printf("%s\n", libevdev_get_name(dev));


  // check device capabilities
  // if (!(
  //   libevdev_has_event_type(dev, EV_ABS) &&
  //   libevdev_has_event_code(dev, EV_ABS, ABS_X) &&
  //   libevdev_has_event_code(dev, EV_ABS, ABS_Y) &&
  //   libevdev_has_event_code(dev, EV_ABS, ABS_Z)
  // )) {
  //   fprintf(stderr, "Device %s does not have the required capabilities for this program.", libevdev_get_name(dev));
  //   exit(1);
  // }


  do {
    struct input_event ev;
    rc = libevdev_next_event(dev, LIBEVDEV_READ_FLAG_NORMAL|LIBEVDEV_READ_FLAG_BLOCKING, &ev);
    if (rc == LIBEVDEV_READ_STATUS_SUCCESS) {
      print_event(&ev);
    }
  } while (rc == LIBEVDEV_READ_STATUS_SUCCESS || rc == -EAGAIN);

  // check for termination reasons here?
  
  // cleanup
  fd = libevdev_get_fd(dev);
  libevdev_free(dev);
  close(fd);
  
  return 0;
}

