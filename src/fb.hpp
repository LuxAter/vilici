#ifndef VILICI_FB_HPP_
#define VILICI_FB_HPP_

#include <cstdint>
#include <linux/fb.h>

namespace fb {
extern uint32_t *fb;
extern fb_var_screeninfo info;

bool init();
bool term();

void swap();
void clear();

} // namespace fb

#endif // VILICI_FB_HPP_
