#ifndef IO_H
#define IO_H

#include <hidapi/hidapi.h>
#include <stdint.h>
#include <sys/types.h>
#include <string.h>
#include <unistd.h>
#include <stdio.h>

#define BUFSIZE 9
#define PACKET_NUMBER 294

// Errors

#define BUFFER_TOO_SMALL -1
#define ERROR_SENDING_REPORT -2
#define ERROR_FEATURE -3

/* Functions of this type will be called when reading and writing
   to the device */
typedef void* (*progress_callback)(int, int);

int read_rom_from_usb(hid_device*, uint8_t*, ssize_t, progress_callback);

int write_rom_to_usb(hid_device*, uint8_t*, ssize_t, progress_callback);

#endif
