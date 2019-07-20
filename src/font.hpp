#ifndef VILICI_FONT_HPP_
#define VILICI_FONT_HPP_

#include <fontconfig/fontconfig.h>
#include <ft2build.h>
#include FT_FREETYPE_H

#include <map>
#include <string>

namespace font {
enum FontStyle {
  REGULAR = 0b1000,
  BOLD = 0b0100,
  ITALIC = 0b0010,
  BOLD_ITALIC = 0b0001
};

extern std::map<std::string, std::string> font_files_;
extern FT_Library ft_library_;
extern FT_Face regular_, bold_, ital_, bold_ital_;

bool init();

void render(unsigned &cx, unsigned &cy, const std::string &text);
} // namespace font

#endif // VILICI_FONT_HPP_
