#include <libevdev/libevdev.h>  // (struct libevdev), libevdev_*()
#include <stdio.h>              // printf(), fprintf(), perror(), stderr, 
#include <stdlib.h>             // exit(), EXIT_FAILURE
//#include <errno.h>              
#include <string.h>             // strerror(), strcpy(), strcat()
#include <dirent.h>             // (DIR), DT_CHR
#include <fcntl.h>              // O_RDONLY, O_NONBLOCK
#include <unistd.h>             // close()
#include <regex.h>              // (regex_t), regcomp(), regcopy()

#define DIR_DEV_INPUT "/dev/input/"

int match_device(struct libevdev **outdev, char* pattern) {
  regex_t regex;

  char pathbuf[256];

  DIR *dir;
  struct dirent *ent;

  int fd;
  struct libevdev *dev = NULL;

  int rc;

  // compile the provided regex and exit if it fails
  if (rc = regcomp(&regex, pattern, 0) != 0) {
    fprintf(stderr, "Regex compile error for \"%s\"\n", pattern);
    //exit(EXIT_FAILURE);
    return 1;
  }

  // open /dev/input directory
  if ((dir = opendir(DIR_DEV_INPUT)) != NULL) {
    // loop through each file/directory in /dev/input
    while ((ent = readdir(dir)) != NULL) {
      // if current file is a character device:
      if (ent->d_type == DT_CHR) {
        // combine the "/dev/input/" path with the filename
        strcpy(pathbuf, DIR_DEV_INPUT);
        strcat(pathbuf, ent->d_name);

        printf("%s\n", pathbuf);

        fd = open(pathbuf, O_RDONLY);

        // if a libevdev device can successfully be inited:
        if ((rc = libevdev_new_from_fd(fd, &dev)) >= 0) {
          printf("Input device name: \"%s\"\n", libevdev_get_name(dev));

          // compare the device name to the regex
          rc = regexec(&regex, libevdev_get_name(dev), 0, NULL, 0);

          if (rc == REG_NOERROR) {
            // assign the new device to the outdev argument
            closedir(dir);
            *outdev = dev;
            return 0;
          }
        } else {
          // failed to init a valid libevdev device, which is fine in this case
          fprintf(stderr, "Failed to init libevdev (%s)\n", strerror(-rc));
        }
        close(fd);
      }
    }
    // close the directory and give up, returning error
    closedir(dir);
    fprintf(stderr, "No devices matching \"%s\" found\n", pattern);
    return 1;
  } else {
    // /dev/input does not exist
    perror(DIR_DEV_INPUT);
    //exit(EXIT_FAILURE);
    return 1;
  }
}

// https://gitlab.freedesktop.org/libevdev/libevdev/blob/master/tools/libevdev-events.c
int print_event(struct input_event *ev)
{
	if (ev->type == EV_SYN)
		printf("Event: time %ld.%06ld, ++++++++++++++++++++ %s +++++++++++++++\n",
				ev->input_event_sec,
				ev->input_event_usec,
				libevdev_event_type_get_name(ev->type));
	else
		printf("Event: time %ld.%06ld, type %d (%s), code %d (%s), value %d\n",
			ev->input_event_sec,
			ev->input_event_usec,
			ev->type,
			libevdev_event_type_get_name(ev->type),
			ev->code,
			libevdev_event_code_get_name(ev->type, ev->code),
			ev->value);
	return 0;
}
