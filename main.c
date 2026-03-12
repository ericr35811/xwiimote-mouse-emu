#include <libevdev/libevdev.h>  // libevdev_*()
#include <libevdev/libevdev-uinput.h>
#include <stdlib.h>             // exit()
#include <stdio.h>              // printf(), fprintf()
#include <unistd.h>             // close()
#include <errno.h>              // EAGAIN
#include <fcntl.h>
#include "evdev_helpers.h"      // match_device()
#include "uinput.h"

#define ANG_VEL_THRESHOLD 50
#define N_SUBPIXELS 900

struct motionplus {
  // acceleration for each vxis, raw from the device
  int vx;
  int vy;
  int vz;
};

/*  scale the velocity according to the number of subpixels (MotionPlus units) per
    pixel of mouse movement.
    also accumulate the remainder into a variable, and send a 1-pixel pulse of 
    movement when it wraps (when it totals 1 subpixel.)
*/
int scale_and_accumulate_remainder(int *acc, int vel, int ang_vel_threshold, int n_subpixels) {
  int r, v;

  // check if this movement is large enough to register
  if (vel > ang_vel_threshold || vel < -ang_vel_threshold) {
    r = vel % n_subpixels;  // integer scaling
    v = vel / n_subpixels;  // remaining subpixel value
    
    *acc += r;

    // if acc wraps in the positive direction
    if (*acc > n_subpixels) {
      // reset acc and add positive velocity
      *acc = 0;
      v += 1;
    // if acc wraps in the negative direction
    } else if (*acc < -n_subpixels) {
      // reset acc and add negative velocity
      *acc = 0;
      v += -1;
    }
  } else {
    // movements that are too small are ignored
    v = 0;
  }

  return v;
}

int main() {
  int rc, fd;
  int vx = 0, vy = 0, accx = 0, accy = 0;

  struct libevdev *dev = NULL;
  struct libevdev_uinput *ui = NULL;
  struct input_event ev;
  struct motionplus mp = {
    .vx = 0,
    .vy = 0,
    .vz = 0,
  };

  rc = match_device(&dev, "^Nintendo Wii Remote Motion Plus$"); // will error out and exit if a device is not found

  if (rc != 0) 
    return 1; 

  fd = open("/dev/uinput", O_RDWR | O_TRUNC);
  if (fd == -1)
    return 1;
  
  printf("%d\n", fd);

  ui = uinput_create(fd);

  if (ui == NULL)
    return 1;

  printf("%s\n", libevdev_get_name(dev));

  do {
    rc = libevdev_next_event(dev, LIBEVDEV_READ_FLAG_NORMAL|LIBEVDEV_READ_FLAG_BLOCKING, &ev);
    if (rc == LIBEVDEV_READ_STATUS_SUCCESS) {
      //print_event(&ev);

      if (ev.type == EV_ABS) {
        if      (ev.code == ABS_RX) {
          mp.vx = ev.value;
        }
        else if (ev.code == ABS_RY) {
          mp.vy = ev.value;
        }
        else if (ev.code == ABS_RZ) {
          mp.vz = ev.value;
        }
      }
      else if (ev.type == EV_SYN) {
        vx = (mp.vx-100);
        vy = -(mp.vz-450);

        vx = scale_and_accumulate_remainder(&accx, vx, ANG_VEL_THRESHOLD, N_SUBPIXELS);
        vy = scale_and_accumulate_remainder(&accy, vy, ANG_VEL_THRESHOLD, N_SUBPIXELS);

        libevdev_uinput_write_event(ui, EV_REL, REL_X, vx);
        libevdev_uinput_write_event(ui, EV_REL, REL_Y, vy);
        libevdev_uinput_write_event(ui, EV_SYN, SYN_REPORT, 0);

        printf("VX: %5d VY: %5d\n", vx, vy); fflush(stdout);

      }
    }

  } while (rc == LIBEVDEV_READ_STATUS_SUCCESS || rc == -EAGAIN);

  // check for termination reasons here?
  
  // cleanup
  close(fd);

  fd = libevdev_get_fd(dev);
  libevdev_free(dev);
  close(fd);
  
  return 0;
}

