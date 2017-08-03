#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "io.h"

void main_loop(hid_device*);

int main(int argc, char* argv[])
{
  
  int res;
  hid_device *handle;

  // Open the device using the VID, PID,
  // and optionally the Serial number.
  hid_init();
  handle = hid_open(0x1d57, 0xac01, NULL);
  if(handle == NULL){
    printf("Cannot get handle, try running as sudo\n");
    return 1;
  }

  main_loop(handle);
  return 0;
  
  uint8_t rom[2058];
  res = read_rom_from_usb(handle, rom, 2058, NULL);
  if(res < 0)
    printf("Error\n");    
  
  printf("\nWhich file to save into ?\n");
  char filename[100];
  scanf("%s", filename);
  printf("Writing rom to %s\n", filename);
  int fd = open(filename, O_WRONLY | O_CREAT | O_TRUNC, 0644);
  if(fd < 0){
    perror("Couldn't open the file");
    return 2;
  }
  int wr = write(fd, rom, 2058);
  if(wr < 0){
    perror("Couldn't write to the file");
  }
  close(fd);
  printf("File written !\n");
  
  const uint8_t towrite[] = {1, 1, 0x91, 0, 0x15, 0xff, 0x57, 0xe7};
  memcpy(rom+0x10, towrite, sizeof(towrite));

  res = write_rom_to_usb(handle, rom, 2058, NULL);
  printf("Final result: %d\n", res);
    
  return 0;
}
