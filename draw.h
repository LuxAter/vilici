#ifndef VILICI_DRAW_H_
#define VILICI_DRAW_H_

#include <stdint.h>
#include "rgb.h"

static void drawRect(uint32_t left, uint32_t top, uint32_t width, uint32_t height, rgb_t rgb);
static void drawFilledRect(uint32_t left, uint32_t top, uint32_t width, uint32_t height, rgb_t rgb);

#endif  // VILICI_DRAW_H_
