#include <assert.h>
#include <fcntl.h>
#include <linux/fb.h>
#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <sysexits.h>
#include <time.h>
#include <unistd.h>

#include "draw.h"
#include "rgb.h"

uint32_t* buf;
struct fb_var_screeninfo info;

static void renderRectangle(uint32_t left, uint32_t top, uint32_t width, uint32_t height, rgb_t rgb)
{
  uint32_t color = getColor(rgb);
  for (uint32_t y = 0; y < height; ++y) {
    for (uint32_t x = 0; x < width; ++x) {
      buf[(top + y) * info.xres + left + x] = color;
    }
  }
}

static void renderChar(uint32_t left, uint32_t top, char c)
{
  /* uint8_t *glyph = gyphs[c]; */
  /* uint32_t stride = header.charsize / header.height; */
  /* uint32_t stride = 8; */
  uint32_t color = rand() % 0xFFFFFF;
  for (uint32_t y = 0; y < 16; ++y) {
    for (uint32_t x = 0; x < 9; ++x) {
      buf[(top + y) * info.xres + left + x] = color;
    }
  }
}

static void renderStr(uint32_t left, uint32_t top, const char* str)
{
  for (; *str; ++str) {
    renderChar(left, top, *str);
    left += 9;
  }
}

int main(int argc, char* argv[])
{
  int fb = open("/dev/fb0", O_RDWR);
  assert(fb > 0);
  assert(0 == ioctl(fb, FBIOGET_VSCREENINFO, &info));
  size_t len = 4 * info.xres * info.yres;
  buf = mmap(NULL, len, PROT_READ | PROT_WRITE, MAP_SHARED, fb, 0);
  assert(buf != MAP_FAILED);
  for (;;) {
    double h = 0;
    for (uint32_t p = 0; p < info.xres; p += 2) {
      renderRectangle(p, 50, 2, 10, createHSVd(h, 1.0, 1.0));
      h += 1.0;
      if (h >= 360){
        h = 0;
      }
    }
    /* time_t t = time(NULL); */
    /* const struct tm* local = localtime(&t); */
    /* char* str; */
    /* #<{(| size_t len = strftime(str, sizeof(str), "%H:%M", local); |)}># */
    /* size_t len = 12; */
    /* str = "Hello World!"; */
    /* assert(len); */
    /* for (int i = 0; i < (60 - local->tm_sec); ++i) { */
    /*   renderStr(info.xres - 9 * len, 0, str); */
    /*   sleep(1); */
    /* } */
  }
  return 0;
}
