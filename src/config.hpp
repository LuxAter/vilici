#ifndef VILICI_CONFIG_HPP_
#define VILICI_CONFIG_HPP_

#include <map>
#include <string>

namespace config {
struct Value {
  Value() : value("") {}
  Value(const char *val) : value(val) {}
  Value(const std::string &val) : value(val) {}
  template <typename _T>
  typename std::enable_if<std::is_floating_point<_T>::value, _T>::type
  as() const {
    return static_cast<_T>(stold(value));
  }
  template <typename _T>
  typename std::enable_if<std::is_unsigned<_T>::value, _T>::type as() const {
    if (value[0] == '#') {
      return static_cast<_T>(stoull(value.substr(1), nullptr, 16));
    }
    return static_cast<_T>(stoull(value));
  }
  template <typename _T>
  typename std::enable_if<
      std::is_integral<_T>::value && !std::is_unsigned<_T>::value, _T>::type
  as() const {
    return static_cast<_T>(stoll(value));
  }
  template <typename _T>
  typename std::enable_if<std::is_same<_T, std::string>::value, _T>::type
  as() const {
    return value;
  }
  template <typename _T>
  typename std::enable_if<
      !std::is_floating_point<_T>::value && !std::is_integral<_T>::value &&
          !std::is_unsigned<_T>::value && !std::is_same<_T, std::string>::value,
      _T>::type
  as() const {
    return _T(value);
  }
  std::string value;
};

extern std::map<std::string, std::map<std::string, Value>> cfg;

#ifdef DEBUG
bool load(const std::string &config_path = "./resources/config.ini");
#else
bool load(const std::string &config_path = "~/.config/vilici/config.ini");
#endif
} // namespace config

#endif // VILICI_CONFIG_HPP_
