#ifndef VILICI_FONT_HPP_
#define VILICI_FONT_HPP_

#include <fontconfig/fontconfig.h>
#include <ft2build.h>
#include FT_FREETYPE_H

#include <map>
#include <string>

namespace font {
extern std::map<std::string, std::string> font_files_;
bool init(const std::string &name);
} // namespace font

#endif // VILICI_FONT_HPP_
