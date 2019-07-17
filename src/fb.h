#ifndef VILICI_FB_H_
#define VILICI_FB_H_

#include <cstdint>
#include <linux/fb.h>

namespace fb {
extern uint32_t *fb;
extern fb_var_screeninfo info;

bool init();
bool term();

} // namespace fb

#endif // VILICI_FB_H_
