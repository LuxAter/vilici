#include "config.hpp"
#include "fb.hpp"
#include "font.hpp"

#include <iostream>

int main(int argc, char *argv[]) {
  fb::init();
  config::load();
  font::init("SauceCodePro Nerd Font");
  for (auto &it : font::font_files_) {
    std::cout << it.first << ":" << it.second << "\n";
  }
  fb::clear();
  fb::swap();
  // for (auto &it : config::cfg) {
  //   std::cout << it.first << "::\n";
  //   for (auto &kv : it.second) {
  //     std::cout << kv.first << ':' << kv.second.as<std::string>() << '\n';
  //   }
  // }
  // std::cout << config::cfg["color"]["background"].as<unsigned>() << '\n';
  fb::term();
  return 0;
}
