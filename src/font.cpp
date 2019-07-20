#include "font.hpp"

#include <fontconfig/fontconfig.h>
#include <ft2build.h>
#include FT_FREETYPE_H

#include <iostream>
#include <map>
#include <string>

namespace font {
std::map<std::string, std::string> font_files_;
} // namespace font

bool font::init(const std::string &name) {
  FcConfig *config = FcInitLoadConfigAndFonts();
  FcPattern *pat =
      FcPatternBuild(0, FC_FAMILY, FcTypeString, name.c_str(), (char *)0);
  FcObjectSet *os =
      FcObjectSetBuild(FC_FAMILY, FC_STYLE, FC_LANG, FC_FILE, (char *)0);
  FcFontSet *fs = FcFontList(config, pat, os);
  if (fs->nfont == 0) {
    if (fs)
      FcFontSetDestroy(fs);
    return false;
  }
  for (int i = 0; fs && i < fs->nfont; ++i) {
    FcPattern *font = fs->fonts[i];
    FcChar8 *file, *style, *family;
    if (FcPatternGetString(font, FC_FILE, 0, &file) == FcResultMatch &&
        FcPatternGetString(font, FC_FAMILY, 0, &family) == FcResultMatch &&
        FcPatternGetString(font, FC_STYLE, 0, &style) == FcResultMatch) {
      font_files_[std::string(reinterpret_cast<char *>(style))] =
          std::string(reinterpret_cast<char *>(file));
    }
  }
  if (fs)
    FcFontSetDestroy(fs);
  return true;
}
