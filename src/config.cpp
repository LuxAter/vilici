#include "config.h"

#include <cstdio>
#include <cstring>

#include "color.h"

namespace config {
Config cfg;
} // namespace config

bool config::load(const char *config_path) {
  printf(">>%s<<\n", config_path);
  FILE *config_file = std::fopen(config_path, "r");
  if (config_path == NULL) {
    cfg = Config{{0x000000, 0xcd0000, 0x00cd00, 0xcdcd00, 0x0000ee, 0xcd00cd,
                  0x00cdcd, 0xe5e5e5, 0x7f7f7f, 0xff0000, 0x00ff00, 0xffff00,
                  0x5c5cff, 0xff00ff, 0x00ffff, 0xffffff},
                 0x000000,
                 0xffffff,
                 "terminal",
                 12};
    return true;
  }
  char *line;
  size_t line_size = 0;
  ssize_t line_count = 0;
  Config::Sections section = Config::Sections::NONE;
  line_count = getline(&line, &line_size, config_file);
  while (line_count > 0) {
    size_t pos = 0;
    if (line[pos] == '[') {
      pos++;
      char sec[255];
      size_t id = 0;
      while (pos < line_size && line[pos] != ']') {
        sec[id] = line[pos];
        id++;
        pos++;
      }
      sec[id] = 0;
      if (strncmp(sec, "color", 5) == 0) {
        section = Config::Sections::COLOR;
      } else if (strncmp(sec, "font", 4) == 0) {
        section = Config::Sections::FONT;
      } else {
        section = Config::Sections::NONE;
      }
    } else {
      char key[255];
      size_t id = 0;
      while (pos < line_size && line[pos] != '=' && line[pos] != '\n') {
        key[id] = line[pos];
        id++;
        pos++;
      }
      pos++;
      key[id] = 0;
      id = 0;
      char val[255];
      while (pos < line_size && line[pos] != '=' && line[pos] != '\n') {
        val[id] = line[pos];
        id++;
        pos++;
      }
      val[id] = 0;

      switch (section) {
      case Config::Sections::COLOR: {
        if (strncmp(key, "background", 10) == 0) {
          cfg.background = rgb(val);
        } else if (strncmp(key, "foreground", 10) == 0) {
          cfg.foreground = rgb(val);
        } else if (strncmp(key, "color", 5) == 0 && strlen(key) == 5) {
          cfg.foreground = rgb(val);
        } else if (strlen(key) > 5) {
          int idx = 0;
          sscanf(key, "color%d", &idx);
          cfg.colors[idx] = rgb(val);
        }
        break;
      }
      case Config::Sections::FONT: {
        if (strncmp(key, "family", 6) == 0) {
          cfg.font_name = val;
        } else if (strncmp(key, "size", 4) == 0 ||
                   ((strncmp(key, "px", 2) == 0 ||
                     strncmp(key, "pt", 2) == 0) &&
                    strlen(key) == 0)) {
          cfg.font_size = atol(val);
        }
        break;
      }
      default: {
        break;
      }
      }
    }
    line_count = getline(&line, &line_size, config_file);
  }
  std::fclose(config_file);
  return true;
}
