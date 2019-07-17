#include "fb.h"

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
#include <ctime>

namespace fb {
uint32_t *fb = nullptr;
fb_var_screeninfo info;
static int fb_file = 0;
} // namespace fb

bool fb::init() {
  fb_file = open("/dev/fb0", O_RDWR);
  assert(fb_file > 0);
  assert(ioctl(fb_file, FBIOGET_VSCREENINFO, &info) == 0);
  size_t len = 4 * info.xres * info.yres;
  fb = (uint32_t *)mmap(NULL, len, PROT_READ | PROT_WRITE, MAP_SHARED, fb_file,
                        0);
  assert(fb != MAP_FAILED);
  return true;
}

bool fb::term() {
  fb = nullptr;
  return close(fb_file);
}
