#ifndef VILICI_EVENT_HPP_
#define VILICI_EVENT_HPP_

#include <queue>

#include "key_codes.hpp"

namespace event {
extern std::queue<unsigned> keys;

bool init();
bool term();

void poll();

} // namespace event

#endif // VILICI_EVENT_HPP_
