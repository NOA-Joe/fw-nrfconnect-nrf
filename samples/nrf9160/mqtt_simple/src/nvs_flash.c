#include "nvs_flash.h"



#include <zephyr.h>
#include <drivers/flash.h>
#include <device.h>
#include <stdio.h>
#include <fs/nvs.h>
#include <string.h>

#define TESTSTRING  "NOA Labs\0"

void nvs_main(void)
{
  int err;	
  static struct nvs_fs fs;

  struct flash_pages_info info;

  printk("Flash test \n");
  fs.offset = DT_FLASH_AREA_STORAGE_OFFSET-4096*8;
  err = flash_get_page_info_by_offs(device_get_binding(DT_FLASH_DEV_NAME),
      fs.offset, &info);
  if (err) {
      printk("Unable to get page info");
  }
  fs.sector_size = info.size;
  fs.sector_count = 8U;
  err = nvs_init(&fs, DT_FLASH_DEV_NAME);
  if (err) {
      printk("Flash Init failed\n");
  }else printk("Flash Init success, sector_size:%d,write_block_size:%d\n",fs.sector_size,fs.write_block_size);

  char wbuf[16];
  char rbuf[16];
  strcpy(wbuf, TESTSTRING);
  ssize_t bytes_written = nvs_write(&fs, 1, &wbuf, strlen(wbuf)+1);
  printk("Bytes written to nvs: %d\n", bytes_written);
  ssize_t bytes_read = nvs_read(&fs, 1, &rbuf, sizeof(rbuf));
  printk("Bytes Read to nvs: %d:%s\n", bytes_read,rbuf);


}