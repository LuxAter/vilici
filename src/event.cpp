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
#include <chrono>
#include <cmath>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <ctime>

#include "config.hpp"
#include "log.hpp"

namespace event {
std::queue<unsigned> keys;
std::bitset<255> key_state_;
bool caps_lock_ = false, capital_ = false;
std::size_t tick_count_ = 0, repeat_delay_ = 50;
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

  if (config::cfg.has("input.repeat delay")) {
    repeat_delay_ = config::cfg["input"]["repeat delay"].as<std::size_t>();
  }

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
  switch (scancode) {
  case KEY_ESC:
    return VILICI_KEY_ESCAPE;
  case KEY_1:
    return capital_ ? '!' : '1';
  case KEY_2:
    return capital_ ? '@' : '2';
  case KEY_3:
    return capital_ ? '#' : '3';
  case KEY_4:
    return capital_ ? '$' : '4';
  case KEY_5:
    return capital_ ? '%' : '5';
  case KEY_6:
    return capital_ ? '^' : '6';
  case KEY_7:
    return capital_ ? '&' : '7';
  case KEY_8:
    return capital_ ? '*' : '8';
  case KEY_9:
    return capital_ ? '(' : '9';
  case KEY_0:
    return capital_ ? ')' : '0';
  case KEY_MINUS:
    return capital_ ? '_' : '-';
  case KEY_EQUAL:
    return capital_ ? '+' : '=';
  case KEY_BACKSPACE:
    return VILICI_KEY_BACKSPACE;
  case KEY_TAB:
    return VILICI_KEY_TAB;
  case KEY_Q:
    return capital_ ? 'Q' : 'q';
  case KEY_W:
    return capital_ ? 'W' : 'w';
  case KEY_E:
    return capital_ ? 'E' : 'e';
  case KEY_R:
    return capital_ ? 'R' : 'r';
  case KEY_T:
    return capital_ ? 'T' : 't';
  case KEY_Y:
    return capital_ ? 'Y' : 'y';
  case KEY_U:
    return capital_ ? 'U' : 'u';
  case KEY_I:
    return capital_ ? 'I' : 'i';
  case KEY_O:
    return capital_ ? 'O' : 'o';
  case KEY_P:
    return capital_ ? 'P' : 'p';
  case KEY_LEFTBRACE:
    return capital_ ? '{' : '[';
  case KEY_RIGHTBRACE:
    return capital_ ? '}' : ']';
  case KEY_BACKSLASH:
    return capital_ ? '|' : '\\';
  case KEY_ENTER:
    return VILICI_KEY_ENTER;
  case KEY_A:
    return capital_ ? 'A' : 'a';
  case KEY_S:
    return capital_ ? 'S' : 's';
  case KEY_D:
    return capital_ ? 'D' : 'd';
  case KEY_F:
    return capital_ ? 'F' : 'f';
  case KEY_G:
    return capital_ ? 'G' : 'g';
  case KEY_H:
    return capital_ ? 'H' : 'h';
  case KEY_J:
    return capital_ ? 'J' : 'j';
  case KEY_K:
    return capital_ ? 'K' : 'k';
  case KEY_L:
    return capital_ ? 'L' : 'l';
  case KEY_SEMICOLON:
    return capital_ ? ':' : ';';
  case KEY_APOSTROPHE:
    return capital_ ? '"' : '\'';
  case KEY_GRAVE:
    return capital_ ? '~' : '`';
  case KEY_Z:
    return capital_ ? 'Z' : 'z';
  case KEY_X:
    return capital_ ? 'X' : 'x';
  case KEY_C:
    return capital_ ? 'C' : 'c';
  case KEY_V:
    return capital_ ? 'V' : 'v';
  case KEY_B:
    return capital_ ? 'B' : 'b';
  case KEY_N:
    return capital_ ? 'N' : 'n';
  case KEY_M:
    return capital_ ? 'M' : 'm';
  case KEY_COMMA:
    return capital_ ? '<' : ',';
  case KEY_DOT:
    return capital_ ? '>' : '.';
  case KEY_SLASH:
    return capital_ ? '?' : '/';
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
  case KEY_INSERT:
    return VILICI_KEY_INSERT;
  case KEY_SYSRQ:
    return VILICI_KEY_PRINT_SCREEN;
  case KEY_RIGHTMETA:
  case KEY_LEFTMETA:
    return VILICI_KEY_META;
  case KEY_LEFTCTRL:
  case KEY_RIGHTCTRL:
    return VILICI_KEY_CTRL;
  case KEY_LEFTALT:
  case KEY_RIGHTALT:
    return VILICI_KEY_ALT;
  case KEY_HOME:
    return VILICI_KEY_HOME;
  case KEY_END:
    return VILICI_KEY_END;
  case KEY_PAGEUP:
    return VILICI_KEY_PAGE_UP;
  case KEY_PAGEDOWN:
    return VILICI_KEY_PAGE_DOWN;
  case KEY_LEFTSHIFT:
  case KEY_RIGHTSHIFT:
  case KEY_CAPSLOCK:
    return VILICI_KEY_ERR;
  case KEY_MUTE:
    return VILICI_KEY_MUTE;
  case KEY_VOLUMEDOWN:
    return VILICI_KEY_VOLUME_DOWN;
  case KEY_VOLUMEUP:
    return VILICI_KEY_VOLUME_UP;
  case KEY_NEXTSONG:
    return VILICI_KEY_NEXT_SONG;
  case KEY_PLAYPAUSE:
    return VILICI_KEY_PAUSE_PLAY;
  case KEY_PREVIOUSSONG:
    return VILICI_KEY_PREVIOUS_SONG;
  case KEY_BRIGHTNESSDOWN:
    return VILICI_KEY_BRIGHTNESS_DOWN;
  case KEY_BRIGHTNESSUP:
    return VILICI_KEY_BRIGHTNESS_UP;
  default:
    return scancode + 1000;
  }
}

void event::handle_key_hold_down() {
  if (key_state_.any()) {
    for (std::size_t i = 0; i < key_state_.size(); ++i) {
      unsigned keycode = VILICI_KEY_ERR;
      if (key_state_[i] && (keycode = scan_to_key(i)) != VILICI_KEY_ERR) {
        keys.push(keycode);
      }
    }
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
        unsigned keycode = scan_to_key(scan);
        if (keycode != VILICI_KEY_ERR) {
          keys.push(keycode);
        } else {
          capital_ = (key_state_[KEY_LEFTSHIFT] ||
                      key_state_[KEY_RIGHTSHIFT]) != caps_lock_;
        }
        tick_count_ = 0;
      } else {
        switch (libinput_event_keyboard_get_key(kb)) {
        case KEY_LEFTSHIFT:
        case KEY_RIGHTSHIFT:
        case KEY_CAPSLOCK:
          key_state_.reset(libinput_event_keyboard_get_key(kb));
          capital_ = (key_state_[KEY_LEFTSHIFT] ||
                      key_state_[KEY_RIGHTSHIFT]) != caps_lock_;
          break;
        default:
          key_state_.reset(libinput_event_keyboard_get_key(kb));
          break;
        }
      }
      break;
    }
    default:
      break;
    }
    libinput_event_destroy(ev);
    libinput_dispatch(li_);
  }
  if (tick_count_ > config::cfg["input"]["repeat delay"].as<std::size_t>())
    handle_key_hold_down();
  tick_count_++;
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
