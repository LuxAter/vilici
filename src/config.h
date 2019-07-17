#ifndef VILICI_CONFIG_H_
#define VILICI_CONFIG_H_

#include <cstdint>
#include <string>

namespace config {
struct Config {
  enum Sections { COLOR, FONT, NONE };
  uint32_t colors[16];
  uint32_t background, foreground;
  std::string font_name;
  uint16_t font_size;
};

extern Config cfg;

#ifdef DEBUG
bool load(const char *config_path = "./resources/config.ini");
#else
bool load(const char *config_path = "~/.config/vilici/config.ini");
#endif
} // namespace config

#endif // VILICI_CONFIG_H_
