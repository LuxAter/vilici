#ifndef VILICI_RGB_H_
#define VILICI_RGB_H_

#include <stdint.h>
#include <assert.h>
#include <math.h>

typedef struct rgb_s{
  uint8_t r, g, b;
} rgb_t;

static uint32_t getColor(rgb_t color)
{
  return ((uint32_t)color.r * 65536) + ((uint32_t)color.g * 256) + (uint32_t)color.b;
}
static rgb_t createRGBd(double r, double g, double b)
{
  return (rgb_t){ (uint8_t)(r * 255), (uint8_t)(g * 255), (uint8_t)(b * 255) };
}
static rgb_t createRGBi(uint8_t r, uint8_t g, uint8_t b)
{
  return (rgb_t){ r, g, b };
}

static rgb_t createHSVd(double h, double s, double v)
{
  assert(0 <= h && h < 365);
  assert(0 <= s && s <= 1);
  assert(0 <= v && v <= 1);
  double c = v * s;
  double x = c * (1 - fabs(fmod((h / 60.0), 2.0) - 1));
  double m = v - c;
  double rp = 0.0, gp = 0.0, bp = 0.0;
  if (h < 60) {
    rp = c;
    gp = x;
    bp = 0;
  } else if (h < 120) {
    rp = x;
    gp = c;
    bp = 0;
  } else if (h < 180) {
    rp = 0;
    gp = c;
    bp = x;
  } else if (h < 240) {
    rp = 0;
    gp = x;
    bp = c;
  } else if (h < 300) {
    rp = x;
    gp = 0;
    bp = c;
  } else if (h < 360) {
    rp = c;
    gp = 0;
    bp = x;
  }
  return createRGBd(rp + m, gp + m, bp + m);
}

#endif  // VILICI_RGB_H_
