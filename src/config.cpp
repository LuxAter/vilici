#include "config.hpp"

#include <algorithm>
#include <cstdio>
#include <filesystem>
#include <fstream>
#include <string>

#include "color.hpp"

namespace config {
Value cfg;
} // namespace config

std::string trim(const std::string &str) {
  std::string s(str);
  s.erase(s.begin(), std::find_if(s.begin(), s.end(),
                                  [](int ch) { return !std::isspace(ch); }));
  s.erase(std::find_if(s.rbegin(), s.rend(),
                       [](int ch) { return !std::isspace(ch); })
              .base(),
          s.end());
  return s;
}

config::Value config::sub_str(const Value &res, const Value &id) {
  if (id.type_ == Value::STRING) {
    std::string val = id.as<std::string>();
    if (val.size() > 3 && val[0] == '$' && val[1] == '{' && val.back() == '}' &&
        res.has(val.substr(2, val.size() - 3))) {
      return res[val.substr(2, val.size() - 3)];
    }
  }
  return id;
}

config::Value config::parse_str(const std::string &val) {
  if (val.front() == '#' && val.size() == 7) {
    std::istringstream iss(val.substr(1));
    unsigned uval;
    iss >> std::hex >> uval;
    return Value(uval);
  } else if (val[0] == '0' && val[1] == 'x') {
    std::istringstream iss(val.substr(2));
    unsigned uval;
    iss >> std::hex >> uval;
    return Value(uval);
    return Value(uval);
  } else if (val.find_first_not_of("-+0123456789") == std::string::npos) {
    std::istringstream iss(val);
    long lval;
    iss >> lval;
    return Value(lval);
  } else if (val.find_first_not_of("-+0123456789eE.,") == std::string::npos) {
    std::istringstream iss(val);
    double dval;
    iss >> dval;
    return Value(dval);
  } else {
    return Value(val);
  }
}

config::Value config::load_ini(const std::string &config_path) {
  std::fstream file(config_path);
  if (file.is_open()) {
    Value res(Value::TABLE);
    std::string line;
    std::string key = "";
    while (getline(file, line)) {
      line = trim(line);
      if (line.size() == 0 || line.front() == ';' || line.front() == '#')
        continue;
      if (line.front() == '[' && line.back() == ']') {
        key = line.substr(1, line.size() - 2);
        res.append(key, Value(Value::TABLE));
      } else if (line.find('=') != std::string::npos) {
        std::string prop = trim(line.substr(0, line.find('=')));
        std::string val = trim(line.substr(line.find('=') + 1));
        res[key].append(prop, sub_str(res, parse_str(val)));
      }
    }
    return res;
  } else {
    return Value();
  }
}
config::Value config::load_json(const std::string &config_path) {
  std::fstream file(config_path);
  if (file.is_open()) {
    Value val;
    return val;
  } else {
    return Value();
  }
}
config::Value config::load_toml(const std::string &config_path) {
  std::fstream file(config_path);
  if (file.is_open()) {
    Value val;
    return val;
  } else {
    return Value();
  }
}
config::Value config::load_yaml(const std::string &config_path) {
  std::fstream file(config_path);
  if (file.is_open()) {
    Value val;
    return val;
  } else {
    return Value();
  }
}

bool config::load(const std::string &config_path) {
  if (std::filesystem::exists(config_path + ".ini")) {
    cfg = load_ini(config_path + ".ini");
  } else if (std::filesystem::exists(config_path + ".toml")) {
    cfg = load_toml(config_path + ".toml");
  } else if (std::filesystem::exists(config_path + ".yaml")) {
    cfg = load_toml(config_path + ".yaml");
  } else if (std::filesystem::exists(config_path + ".json")) {
    cfg = load_toml(config_path + ".json");
  } else {
    return false;
  }
  cfg["font"].append("regular", "Regular");
  cfg["font"].append("bold", "Bold");
  cfg["font"].append("italic", "Italic");
  cfg["font"].append("bold italic", "Bold Italic");
  return true;
}
