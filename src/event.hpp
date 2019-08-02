#ifndef VILICI_EVENT_HPP_
#define VILICI_EVENT_HPP_

#include <libinput.h>
#include <libudev.h>

#include <bitset>
#include <cerrno>
#include <queue>

namespace event {
extern std::queue<unsigned> keys;
extern std::bitset<255> key_state_;
extern bool caps_lock_;

bool init();
bool term();

unsigned scan_to_key(const unsigned &scancode);

void handle_key_hold_down();
void handle_events();

void poller();

} // namespace event

#endif // VILICI_EVENT_HPP_
