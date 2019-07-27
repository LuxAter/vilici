#include "event.hpp"

#include <iostream>
#include <queue>
#include <sys/ioctl.h>
#include <termios.h>

#include "key_codes.hpp"
#include "log.hpp"

namespace event {
std::queue<unsigned> keys;
} // namespace event

bool event::init() {

  struct termios term;
  tcgetattr(0, &term);
  term.c_lflag &= ~ECHO;
  term.c_lflag &= ~ICANON;
  tcsetattr(0, TCSANOW, &term);
  printf("\033[?25l\n");
  return true;
}

bool event::term() {
  struct termios term;
  tcgetattr(0, &term);
  term.c_lflag |= ECHO;
  term.c_lflag |= ICANON;
  tcsetattr(0, TCSANOW, &term);
  printf("\033[?25h\n");
  return true;
}

void event::poll() {
  int bytes = 0, pending_res = 0;
  pending_res = ioctl(0, FIONREAD, &bytes);
  if (pending_res != 0 || bytes <= 0) {
    return;
  }
  while (bytes > 0) {
    unsigned ch = getchar();
    bytes--;
    // keys.push(ch);
    if (ch != 27 || bytes == 0) {
      keys.push(ch);
    } else {
      switch ((ch = getchar())) {
      case '[':
        switch ((ch = getchar())) {
        case '1':
          switch ((ch = getchar())) {
          case '5':
            keys.push(KEY_F5);
            getchar();
            bytes--;
            break;
          case '7':
            keys.push(KEY_F6);
            getchar();
            bytes--;
            break;
          case '8':
            keys.push(KEY_F7);
            getchar();
            bytes--;
            break;
          case '9':
            keys.push(KEY_F8);
            getchar();
            bytes--;
            break;
          default:
            break;
          }
          bytes--;
          break;
        case '2':
          switch ((ch = getchar())) {
          case '0':
            keys.push(KEY_F9);
            getchar();
            bytes--;
            break;
          case '1':
            keys.push(KEY_F10);
            getchar();
            bytes--;
            break;
          case '4':
            keys.push(KEY_F12);
            getchar();
            bytes--;
            break;
          default:
            break;
          }
          bytes--;
          break;
        case '3':
          keys.push(KEY_DELETE);
          getchar();
          bytes--;
          break;
        case 'A':
          keys.push(KEY_UP);
          break;
        case 'B':
          keys.push(KEY_DOWN);
          break;
        case 'C':
          keys.push(KEY_RIGHT);
          break;
        case 'D':
          keys.push(KEY_LEFT);
          break;
        }
        bytes--;
        break;
      case 'O':
        switch ((ch = getchar())) {
        case 'P':
          keys.push(KEY_F1);
          break;
        case 'Q':
          keys.push(KEY_F2);
          break;
        case 'R':
          keys.push(KEY_F3);
          break;
        case 'S':
          keys.push(KEY_F4);
          break;
        default:
          break;
        }
        bytes--;
        break;
      default:
        break;
      }
      bytes--;
    }
  }
}
