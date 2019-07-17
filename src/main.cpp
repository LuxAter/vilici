#include "config.h"
#include "fb.h"

#include <iostream>

int main(int argc, char *argv[]) {
  fb::init();
  config::load();
  std::cout << config::cfg.background << "\n";
  printf("%x<<\n", config::cfg.background);
  fb::term();
  return 0;
}
