#include "event.hpp"

#include <libevdev/libevdev.h>
#include <libinput.h>
#include <libudev.h>
#include <termios.h>

#include <cerrno>
#include <fcntl.h>
#include <fnmatch.h>
#include <getopt.h>
#include <poll.h>
#include <queue>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

#include "key_codes.hpp"
#include "log.hpp"

#include <fcntl.h>
#include <linux/fb.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <sysexits.h>
#include <unistd.h>

#include <cassert>
#include <cmath>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <ctime>

namespace event {
std::queue<unsigned> keys;
std::bitset<255> key_state_;
bool caps_lock_ = false;
} // namespace event

static int open_restricted(const char *path, int flags, void *user_data) {
  bool *grab = reinterpret_cast<bool *>(user_data);
  int fd = open(path, flags);

  if (fd < 0)
    fprintf(stderr, "Failed to open %s (%s)\n", path, strerror(errno));
  else if (grab && *grab && ioctl(fd, EVIOCGRAB, (void *)1) == -1)
    fprintf(stderr, "Grab requested, but failed for %s (%s)\n", path,
            strerror(errno));

  return fd < 0 ? -errno : fd;
}

static void close_restricted(int fd, void *user_data) { close(fd); }

struct libinput *li_;
static const struct libinput_interface interface_ = {
    .open_restricted = open_restricted,
    .close_restricted = close_restricted,
};

bool event::init() {
  struct udev *udev = udev_new();
  bool grab = false;
  if (!udev) {
    fatal("Failed to initalize udev");
    return false;
  }
  li_ = libinput_udev_create_context(&interface_, &grab, udev);
  if (!li_) {
    fatal("Failed to initalizer context form udev");
    udev_unref(udev);
    return false;
  }
  if (libinput_udev_assign_seat(li_, "seat0")) {
    fatal("Failed to set seat");
    libinput_unref(li_);
    li_ = NULL;
    udev_unref(udev);
    return false;
  }
  udev_unref(udev);
  struct termios term;
  tcgetattr(0, &term);
  term.c_lflag &= ~ECHO;
  term.c_lflag &= ~ICANON;
  tcsetattr(0, TCSANOW, &term);
  printf("\033[?25l\n");
  return true;
}

bool event::term() {
  libinput_unref(li_);
  li_ = NULL;
  struct termios term;
  tcgetattr(0, &term);
  term.c_lflag |= ECHO;
  term.c_lflag |= ICANON;
  tcsetattr(0, TCSANOW, &term);
  printf("\033[?25h\n");
  return true;
}

unsigned event::scan_to_key(const unsigned &scancode) {
  bool capital =
      key_state_[KEY_LEFTSHIFT] || key_state_[KEY_RIGHTSHIFT] || caps_lock_;
  switch (scancode) {
  case KEY_ESC:
    return VILICI_KEY_ESCAPE;
  case KEY_1:
    return capital ? '!' : '1';
  case KEY_2:
    return capital ? '@' : '2';
  case KEY_3:
    return capital ? '#' : '3';
  case KEY_4:
    return capital ? '$' : '4';
  case KEY_5:
    return capital ? '%' : '5';
  case KEY_6:
    return capital ? '^' : '6';
  case KEY_7:
    return capital ? '&' : '7';
  case KEY_8:
    return capital ? '*' : '8';
  case KEY_9:
    return capital ? '(' : '9';
  case KEY_0:
    return capital ? ')' : '0';
  case KEY_MINUS:
    return capital ? '_' : '-';
  case KEY_BACKSPACE:
    return VILICI_KEY_BACKSPACE;
  case KEY_TAB:
    return VILICI_KEY_TAB;
  case KEY_Q:
    return capital ? 'Q' : 'q';
  case KEY_W:
    return capital ? 'W' : 'w';
  case KEY_E:
    return capital ? 'E' : 'e';
  case KEY_R:
    return capital ? 'R' : 'r';
  case KEY_T:
    return capital ? 'T' : 't';
  case KEY_Y:
    return capital ? 'Y' : 'y';
  case KEY_U:
    return capital ? 'U' : 'u';
  case KEY_I:
    return capital ? 'I' : 'i';
  case KEY_O:
    return capital ? 'O' : 'o';
  case KEY_P:
    return capital ? 'P' : 'p';
  case KEY_LEFTBRACE:
    return capital ? '{' : '[';
  case KEY_RIGHTBRACE:
    return capital ? '}' : ']';
  case KEY_ENTER:
    return VILICI_KEY_ENTER;
  case KEY_A:
    return capital ? 'A' : 'a';
  case KEY_S:
    return capital ? 'S' : 's';
  case KEY_D:
    return capital ? 'D' : 'd';
  case KEY_F:
    return capital ? 'F' : 'f';
  case KEY_G:
    return capital ? 'G' : 'g';
  case KEY_H:
    return capital ? 'H' : 'h';
  case KEY_J:
    return capital ? 'J' : 'j';
  case KEY_K:
    return capital ? 'K' : 'k';
  case KEY_L:
    return capital ? 'L' : 'l';
  case KEY_SEMICOLON:
    return capital ? ':' : ';';
  case KEY_APOSTROPHE:
    return capital ? '"' : '\'';
  case KEY_GRAVE:
    return capital ? '~' : '`';
  case KEY_Z:
    return capital ? 'Z' : 'z';
  case KEY_X:
    return capital ? 'X' : 'x';
  case KEY_C:
    return capital ? 'C' : 'c';
  case KEY_V:
    return capital ? 'V' : 'v';
  case KEY_B:
    return capital ? 'B' : 'b';
  case KEY_N:
    return capital ? 'N' : 'n';
  case KEY_M:
    return capital ? 'M' : 'm';
  case KEY_COMMA:
    return capital ? '<' : ',';
  case KEY_DOT:
    return capital ? '>' : '.';
  case KEY_SLASH:
    return capital ? '?' : '/';
  case KEY_SPACE:
    return ' ';
  case KEY_F1:
    return VILICI_KEY_F1;
  case KEY_F2:
    return VILICI_KEY_F2;
  case KEY_F3:
    return VILICI_KEY_F3;
  case KEY_F4:
    return VILICI_KEY_F4;
  case KEY_F5:
    return VILICI_KEY_F5;
  case KEY_F6:
    return VILICI_KEY_F6;
  case KEY_F7:
    return VILICI_KEY_F7;
  case KEY_F8:
    return VILICI_KEY_F8;
  case KEY_F9:
    return VILICI_KEY_F9;
  case KEY_F10:
    return VILICI_KEY_F10;
  case KEY_F11:
    return VILICI_KEY_F11;
  case KEY_F12:
    return VILICI_KEY_F12;
  case KEY_DELETE:
    return VILICI_KEY_DELETE;
  case KEY_UP:
    return VILICI_KEY_UP;
  case KEY_LEFT:
    return VILICI_KEY_LEFT;
  case KEY_RIGHT:
    return VILICI_KEY_RIGHT;
  case KEY_DOWN:
    return VILICI_KEY_DOWN;
  default:
    return scancode + 255;
  }
}

void event::handle_events() {
  struct libinput_event *ev;
  libinput_dispatch(li_);
  while ((ev = libinput_get_event(li_))) {
    switch (libinput_event_get_type(ev)) {
    case LIBINPUT_EVENT_KEYBOARD_KEY: {
      struct libinput_event_keyboard *kb =
          libinput_event_get_keyboard_event(ev);
      if (libinput_event_keyboard_get_key_state(kb) ==
          LIBINPUT_KEY_STATE_PRESSED) {
        int scan = libinput_event_keyboard_get_key(kb);
        key_state_.set(scan);
        switch (scan) {
        case KEY_CAPSLOCK:
          caps_lock_ = !caps_lock_;
        default: {
          unsigned keycode = scan_to_key(scan);
          if (keycode != VILICI_KEY_ERR) {
            keys.push(keycode);
          }
        }
        }
      } else {
        key_state_.reset(libinput_event_keyboard_get_key(kb));
      }
      break;
    }
    default:
      break;
    }
    libinput_event_destroy(ev);
    libinput_dispatch(li_);
  }
}

void event::poller() {
  struct pollfd fds;
  fds.fd = libinput_get_fd(li_);
  fds.events = POLLIN;
  fds.revents = 0;
  libinput_dispatch(li_);
  if (poll(&fds, 1, -1) > -1) {
    handle_events();
  }
}
