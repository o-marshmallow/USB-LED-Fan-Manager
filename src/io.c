#include "io.h"

/**
 * Function that reads the data from the fan and stores it into
 * rom. As the fan's memory size is 2K, rom has to have at least
 * this size
 */
int read_rom_from_usb(hid_device* handle, uint8_t* rom, ssize_t size,
                      progress_callback progress){
  if(size < (PACKET_NUMBER * 7)){
    return BUFFER_TOO_SMALL;
  }
  // Tell the USB fan to send its data
  uint8_t buffer[BUFSIZE] = {0, 0x55, 0x05, 0, 0, 0, 0, 0, 0};
  /* The buffer will be used for reading each packet sent by the fan */
  int res = hid_send_feature_report(handle, buffer, 8);
  if (res < 0)
    return ERROR_SENDING_REPORT;
  int i, j, rom_index = 0;
  for(i = 0; i < PACKET_NUMBER; i++){
    usleep(25000); // 25 ms

    /* The first byte of the buffer is used to tell which feature
     * we want to send to the device (in our case 0) 
     * The eight remaining byte will be given by the device */
    memset(buffer, 0, BUFSIZE);
    res = hid_get_feature_report(handle, buffer, BUFSIZE);
    if(res < 0)
      return ERROR_FEATURE;

    /* The sixth byte is always 0 and should NOT be saved,
     * it's not part of the ROM */
    for(j = 1; j < BUFSIZE; j++){
      if(j != 6)
        rom[rom_index++] = buffer[j];
    }
    if(progress)
      progress(i+1, PACKET_NUMBER);
  }
  return rom_index;
}

/**
 * Function that write the data from the buffer to the fan
 */
int write_rom_to_usb(hid_device* handle, uint8_t* rom, ssize_t size,
                      progress_callback progress){
  if(size < (PACKET_NUMBER * 7)){
    return BUFFER_TOO_SMALL;
  }

  // Tell the USB fan we are going to send the data
  uint8_t buffer[BUFSIZE] = {0, 0x55, 1, 1, 0, 0, 0xff, 7, 0x5d};
#ifndef DEBUG
  int res = hid_send_feature_report(handle, buffer, BUFSIZE);
  if (res < 0)
    return ERROR_SENDING_REPORT;

  // Get the response from the fan
  res = hid_get_feature_report(handle, buffer, BUFSIZE);
  if (res < 0)
    return ERROR_FEATURE;
#endif

  // Start sending data
  buffer[0] = 0;
  const int modulo  = (size%8);
  const int to_write = (size/8) + !!modulo;
  int i;
  for(i = 0; i < to_write; i++){
    usleep(25000); // 25 ms

    /* The first byte of the buffer is used to tell which feature
     * we want to send to the device (in our case 0) 
     * The eight remaining byte are the byte to send
     * Let's check whether we reached the end of the buffer of not*/
    if(i == (to_write - 1) && modulo){
      memset(buffer, 0, BUFSIZE);
      memcpy(buffer+1, rom+i*8, modulo);
    } else {
      memcpy(buffer+1, rom+i*8, BUFSIZE-1);
    }

#ifndef DEBUG
    res = hid_send_feature_report(handle, buffer, BUFSIZE);
    if(res < 0)
      return ERROR_FEATURE;
#else
    for(j = 1; j < BUFSIZE; j++)
      printf("%02x, ", buffer[j]);
    printf("\n");
#endif

    if(progress)
      progress(i+1, to_write);
  }
  return i*8;
}
