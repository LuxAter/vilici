#ifndef VILICI_COLOR_HPP_
#define VILICI_COLOR_HPP_

#include <cmath>
#include <cstdint>
#include <cstring>
#include <string>

inline uint32_t rgb(const double &r, const double &g, const double &b) {
  return (static_cast<uint32_t>(r * 255) << 16) |
         (static_cast<uint32_t>(g * 255) << 8) |
         (static_cast<uint32_t>(b * 255) << 0);
}
inline uint32_t rgb(const char *hex) {
  bool pound = (hex[0] == '#');
  char rs[3], gs[3], bs[3];
  rs[2] = 0;
  gs[2] = 0;
  gs[2] = 0;
  strncpy(rs, hex + 0 + pound, 2);
  strncpy(gs, hex + 2 + pound, 2);
  strncpy(bs, hex + 4 + pound, 2);
  return (strtoul(rs, NULL, 16) << 16) | (strtoul(gs, NULL, 16) << 8) |
         (strtoul(bs, NULL, 16) << 0);
}
inline uint32_t rgb(const std::string &hex) {
  bool pound = (hex[0] == '#');
  char rs[3], gs[3], bs[3];
  strncpy(rs, hex.c_str() + 0 + pound, 2);
  strncpy(gs, hex.c_str() + 2 + pound, 2);
  strncpy(bs, hex.c_str() + 4 + pound, 2);
  return (strtoul(rs, NULL, 16) << 16) | (strtoul(gs, NULL, 16) << 8) |
         (strtoul(bs, NULL, 16) << 0);
}

#endif // VILICI_COLOR_HPP_
