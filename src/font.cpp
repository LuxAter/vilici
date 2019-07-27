#include "font.hpp"

#include <fontconfig/fontconfig.h>
#include <ft2build.h>
#include FT_FREETYPE_H

#include <array>
#include <codecvt>
#include <iostream>
#include <map>
#include <string>

#include "config.hpp"
#include "fb.hpp"
#include "log.hpp"

namespace font {
std::map<std::string, std::string> font_files_;
FT_Library ft_library_;
FT_Face regular_, bold_, ital_, bold_ital_;
static unsigned styles_, offset_ = 0;
} // namespace font

bool font::init() {
  std::string name = config::cfg["font"]["name"].as<std::string>();
  FcConfig *cfg = FcInitLoadConfigAndFonts();
  FcPattern *pat =
      FcPatternBuild(0, FC_FAMILY, FcTypeString, name.c_str(), (char *)0);
  FcObjectSet *os =
      FcObjectSetBuild(FC_FAMILY, FC_STYLE, FC_LANG, FC_FILE, (char *)0);
  FcFontSet *fs = FcFontList(cfg, pat, os);
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
  FT_Error error;
  if ((error = FT_Init_FreeType(&ft_library_))) {
    error("Failed to init freetype");
    return false;
  }
  if (font_files_.find(config::cfg["font"]["regular"].as<std::string>()) ==
      font_files_.end()) {
    error("Font family {} does not have style {}", name,
          config::cfg["font"]["regular"].as<std::string>());
    return false;
  }
  styles_ &= REGULAR;

  std::string regular_path =
      font_files_[config::cfg["font"]["regular"].as<std::string>()];
  if ((error = FT_New_Face(ft_library_, regular_path.c_str(), 0, &regular_)) ==
      FT_Err_Unknown_File_Format) {
    error("Font {} is an unsupported format", regular_path);
    return false;
  }
  std::map<std::string, std::string>::iterator it;
  if ((it = font_files_.find(config::cfg["font"]["bold"].as<std::string>())) !=
      font_files_.end()) {
    if ((error = FT_New_Face(ft_library_, it->second.c_str(), 0, &bold_)) ==
        FT_Err_Unknown_File_Format) {
      error("Font {} is an unsupported format", it->second);
    } else {
      styles_ &= BOLD;
    }
  } else {
    warning("No Bold font style found");
  }
  if ((it =
           font_files_.find(config::cfg["font"]["italic"].as<std::string>())) !=
      font_files_.end()) {
    if ((error = FT_New_Face(ft_library_, it->second.c_str(), 0, &ital_)) ==
        FT_Err_Unknown_File_Format) {
      error("Font {} is an unsupported format", it->second);
    } else {
      styles_ &= ITALIC;
    }
  } else {
    warning("No Italic font style found");
  }
  if ((it = font_files_.find(
           config::cfg["font"]["bold italic"].as<std::string>())) !=
      font_files_.end()) {
    if ((error = FT_New_Face(ft_library_, it->second.c_str(), 0,
                             &bold_ital_)) == FT_Err_Unknown_File_Format) {
      error("Font {} is an unsupported format", it->second);
    } else {
      styles_ &= BOLD_ITALIC;
    }
  } else {
    warning("No Bold Italic font style found");
  }

  if (config::cfg["font"].has("px")) {
    unsigned px_height = config::cfg["font"]["px"].as<unsigned>();
    if (FT_Set_Pixel_Sizes(regular_, 0, px_height)) {
      warning("Failed to set Regular font face to {}px size", px_height);
    }
    if (((BOLD & styles_) == BOLD) && FT_Set_Pixel_Sizes(bold_, 0, px_height)) {
      warning("Failed to set Bold font face to {}px size", px_height);
    }
    if (((ITALIC & styles_) == ITALIC) &&
        FT_Set_Pixel_Sizes(ital_, 0, px_height)) {
      warning("Failed to set Italic font face to {}px size", px_height);
    }
    if (((BOLD_ITALIC & styles_) == BOLD_ITALIC) &&
        FT_Set_Pixel_Sizes(bold_ital_, 0, px_height)) {
      warning("Failed to set Bold Italic font face to {}px size", px_height);
    }
  } else if (config::cfg["font"].has("px height")) {
    unsigned px_height = config::cfg["font"]["px"].as<unsigned>();
    unsigned px_width = 0;
    if (config::cfg["font"].has("px width")) {
      px_width = config::cfg["font"]["px width"].as<unsigned>();
    }
    if (FT_Set_Pixel_Sizes(regular_, px_width, px_height)) {
      warning("Failed to set Regular font face to {}x{}px size", px_width,
              px_height);
    }
    if (((BOLD & styles_) == BOLD) &&
        FT_Set_Pixel_Sizes(bold_, px_width, px_height)) {
      warning("Failed to set Bold font face to {}x{}px size", px_width,
              px_height);
    }
    if (((ITALIC & styles_) == ITALIC) &&
        FT_Set_Pixel_Sizes(ital_, px_width, px_height)) {
      warning("Failed to set Italic font face to {}x{}px size", px_width,
              px_height);
    }
    if (((BOLD_ITALIC & styles_) == BOLD_ITALIC) &&
        FT_Set_Pixel_Sizes(bold_ital_, px_width, px_height)) {
      warning("Failed to set Bold Italic font face to {}x{}px size", px_width,
              px_height);
    }
  } else if (config::cfg["font"].has("pt")) {
    long pt = static_cast<long>(config::cfg["font"]["pt"].as<double>() * 64);
    unsigned horiz_dpi = static_cast<unsigned>(fb::horiz_dpi());
    unsigned vert_dpi = static_cast<unsigned>(fb::vert_dpi());
    std::cout << pt << ':' << horiz_dpi << 'x' << vert_dpi << '\n';
    if (FT_Set_Char_Size(regular_, 0, pt, horiz_dpi, vert_dpi)) {
      warning("Failed to set Regular font face to {}pt size", pt);
    }
    if (((BOLD & styles_) == BOLD) &&
        FT_Set_Char_Size(bold_, 0, pt, horiz_dpi, vert_dpi)) {
      warning("Failed to set Bold font face to {}pt size", pt);
    }
    if (((ITALIC & styles_) == ITALIC) &&
        FT_Set_Char_Size(ital_, 0, pt, horiz_dpi, vert_dpi)) {
      warning("Failed to set Italic font face to {}pt size", pt);
    }
    if (((BOLD_ITALIC & styles_) == BOLD_ITALIC) &&
        FT_Set_Char_Size(bold_ital_, 0, pt, horiz_dpi, vert_dpi)) {
      warning("Failed to set Bold Italic font face to {}pt size", pt);
    }
  }

  offset_ = 0;
  for (uint32_t ch = 32; ch < 127; ++ch) {
    FT_Error error = FT_Load_Char(regular_, ch, FT_LOAD_RENDER);
    if (error) {
      continue;
    }
    offset_ = static_cast<unsigned>(
        std::fmax(offset_, regular_->glyph->metrics.horiBearingY >> 6));
  }

  return true;
}

bool font::term() { return true; }

std::array<unsigned, 2> font::render(const unsigned &cx, const unsigned &cy,
                                     const std::string &text) {
  std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
  std::wstring wtext = converter.from_bytes(text);
  uint32_t base_fg = config::cfg["color"]["foreground"].as<uint32_t>();
  uint32_t base_bg = config::cfg["color"]["background"].as<uint32_t>();
  bool kerning = FT_HAS_KERNING(regular_);
  uint32_t prev = 0;
  FT_Face *face = &regular_;
  unsigned x = cx;
  unsigned y = cy;
  for (uint32_t i = 0; i < wtext.size(); ++i) {
    uint32_t index = FT_Get_Char_Index(*face, wtext[i]);
    if (kerning && prev && index) {
      FT_Vector delta;
      FT_Get_Kerning(*face, prev, index, FT_KERNING_DEFAULT, &delta);
      x += delta.x >> 6;
    }
    prev = index;
    FT_Error error = FT_Load_Glyph(*face, index, FT_LOAD_RENDER);
    if (error)
      continue;
    uint32_t offset = offset_ - ((*face)->glyph->metrics.horiBearingY >> 6);
    FT_Bitmap *bitmap = &(*face)->glyph->bitmap;
    for (uint16_t by = 0; by < bitmap->rows; ++by) {
      for (uint16_t bx = 0; bx < bitmap->width; ++bx) {
        if (bitmap->buffer[by * bitmap->width + bx] != 0) {
          double dr = ((base_fg >> 16) & 0xFF) - ((base_bg >> 16) & 0xFF);
          double dg = ((base_fg >> 8) & 0xFF) - ((base_bg >> 8) & 0xFF);
          double db = ((base_fg)&0xFF) - ((base_bg)&0xFF);
          uint8_t red = (base_bg >> 16) & 0xFF;
          uint8_t green = (base_bg >> 8) & 0xFF;
          uint8_t blue = base_bg & 0xFF;
          double perc = bitmap->buffer[by * bitmap->width + bx] / 255.0;
          uint32_t color = (static_cast<uint32_t>((dr * perc) + red) << 16) |
                           (static_cast<uint32_t>((dg * perc) + green) << 8) |
                           (static_cast<uint32_t>((db * perc) + blue) << 0);
          fb::buff[x + bx + (y + offset + by) * fb::info.xres] = color;
        }
      }
    }
    x += (*face)->glyph->advance.x >> 6;
  }
  return {x - cx, y - cy};
}
