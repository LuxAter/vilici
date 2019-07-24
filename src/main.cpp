#include "config.hpp"
#include "fb.hpp"
#include "font.hpp"
#include "log.hpp"

#include <iostream>

int main(int argc, char *argv[]) {
  logger::init();
  fb::init();
  config::load();
  font::init();
  fb::clear();
  fb::swap();
  unsigned x = 0, y = 0;
  for (std::size_t i = 0; i < fb::info.xres; ++i) {
    fb::px(i, y, 0xffffff);
  }
  font::render(x, y, "Hello World!");
  fb::swap();
  // for (auto &it : font::font_files_) {
  //   std::cout << it.first << "::" << it.second << '\n';
  // }
  // for (auto &it : config::cfg) {
  //   std::cout << it.first << "::\n";
  //   for (auto &kv : it.second) {
  //     std::cout << kv.first << ':' << kv.second.as<std::string>() << '\n';
  //   }
  // }
  fb::term();
  logger::term();
  return 0;
}
