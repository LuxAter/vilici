#include "config.hpp"
#include "event.hpp"
#include "fb.hpp"
#include "font.hpp"
#include "log.hpp"

#include <iostream>

int main(int argc, char *argv[]) {
  logger::init();
  fb::init();
  config::load();
  font::init();
  event::init();

  fb::clear();

  std::cout << config::cfg.as<std::string>() << '\n';

  // fb::swap();
  // font::render(0, 0, "Hello World!");
  // fb::swap();
  // while (true) {
  //   event::poll();
  //   while (event::keys.size() != 0) {
  //     std::cout << "KEY:" << static_cast<char>(event::keys.front()) << ":"
  //               << static_cast<unsigned>(event::keys.front()) << "\n";
  //     event::keys.pop();
  //   }
  // }

  event::term();
  font::term();
  fb::term();
  logger::term();
  return 0;
}
