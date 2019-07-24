#ifndef VILICI_FB_HPP_
#define VILICI_FB_HPP_

#include <cstdint>
#include <linux/fb.h>

namespace fb {
extern uint32_t *fb, *buff;
extern fb_var_screeninfo info;

inline double horiz_dpi() { return info.xres / (0.0393701 * info.width); }
inline double vert_dpi() { return info.yres / (0.0393701 * info.height); }

bool init();
bool term();

void swap();
void clear();

inline void px(unsigned long x, unsigned long y, uint32_t c) {
  buff[x + info.xres * y] = c;
}

} // namespace fb

#endif // VILICI_FB_HPP_
