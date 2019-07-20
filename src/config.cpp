#include "config.hpp"

#include <algorithm>
#include <cstdio>
#include <fstream>
#include <string>

#include "color.hpp"

namespace config {
std::map<std::string, std::map<std::string, Value>> cfg;
} // namespace config

void trimwhitespace(char *str) {
  while (str[0] == ' ')
    for (size_t i = strlen(str) - 1; i > 0; --i) {
      str[i] = str[i - 1];
    }
}

bool config::load(const std::string &config_path) {
  cfg = {{"color",
          {{"color0", "#000000"},
           {"color1", "#cd0000"},
           {"color2", "#00cd00"},
           {"color3", "#cdcd00"},
           {"color4", "#0000ee"},
           {"color5", "#cd00cd"},
           {"color6", "#00cdcd"},
           {"color7", "#e5e5e5"},
           {"color8", "#7f7f7f"},
           {"color9", "#ff0000"},
           {"color10", "#00ff00"},
           {"color11", "#ffff00"},
           {"color12", "#5c5cff"},
           {"color13", "#ff00ff"},
           {"color14", "#00ffff"},
           {"color15", "#ffffff"},
           {"foreground", "#ffffff"},
           {"background", "#000000"}}},
         {"font", {{"family", "Terminus"}, {"size", "12"}}}};
  std::fstream file(config_path);
  if (file.is_open()) {
    std::string line;
    std::string section = "";
    while (std::getline(file, line)) {
      line.erase(line.begin(),
                 std::find_if(line.begin(), line.end(),
                              [](int ch) { return !std::isspace(ch); }));
      line.erase(std::find_if(line.rbegin(), line.rend(),
                              [](int ch) { return !std::isspace(ch); })
                     .base(),
                 line.end());
      if (line.front() == '[' && line.back() == ']') {
        line = line.substr(1, line.size() - 2);
        section = line;
        if (cfg.find(section) == cfg.end()) {
          cfg[section] = std::map<std::string, Value>();
        }
      } else {
        std::size_t pos = line.find('=');
        std::string key = line.substr(0, pos);
        std::string val = line.substr(pos + 1);
        key.erase(std::find_if(key.rbegin(), key.rend(),
                               [](int ch) { return !std::isspace(ch); })
                      .base(),
                  key.end());
        val.erase(val.begin(), std::find_if(val.begin(), val.end(), [](int ch) {
                    return !std::isspace(ch);
                  }));
        cfg[section][key] = Value{val};
      }
    }
  }
  return true;
}
