#include "fb.hpp"

#include <fcntl.h>
#include <linux/fb.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <sysexits.h>
#include <unistd.h>

#include <cassert>
#include <cmath>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <ctime>

#include "config.hpp"

namespace fb {
uint32_t *fb = nullptr;
uint32_t *buff = nullptr;
fb_var_screeninfo info;
static int fb_file = 0;
} // namespace fb

bool fb::init() {
  fb_file = open("/dev/fb0", O_RDWR);
  assert(fb_file > 0);
  assert(ioctl(fb_file, FBIOGET_VSCREENINFO, &info) == 0);
  size_t len = 4 * info.xres * info.yres;
  fb = static_cast<uint32_t *>(
      mmap(NULL, len, PROT_READ | PROT_WRITE, MAP_SHARED, fb_file, 0));
  assert(fb != MAP_FAILED);
  buff =
      static_cast<uint32_t *>(malloc(info.xres * info.yres * sizeof(uint32_t)));
  return true;
}

bool fb::term() {
  free(buff);
  fb = nullptr;
  return close(fb_file);
}

void fb::swap() {
  std::memcpy(fb, buff, info.xres * info.yres * sizeof(uint32_t));
}

void fb::clear() {
  // std::fill_n(buff, info.xres * info.yres,
  //             config::cfg["color"]["background"].as<uint32_t>());
}
