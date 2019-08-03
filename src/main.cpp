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
  fb::swap();
  unsigned x = 0, y = 0;
  fb::swap();
  while (true) {
    event::poller();
    while (event::keys.size() != 0) {
      std::cout << "KEY:" << static_cast<char>(event::keys.front()) << ":"
                << static_cast<unsigned>(event::keys.front()) << "\n";
      if (event::keys.front() >= 32 && event::keys.front() <= 126) {
        // auto step = font::render(
        //     x, y, std::string(1, static_cast<char>(event::keys.front())));
        // x += step[0];
        // y += step[1];
      }
      // std::cout << x << ',' << y << "\n";
      event::keys.pop();
      // fb::swap();
    }
  }

  event::term();
  font::term();
  fb::term();
  logger::term();
  return 0;
}
