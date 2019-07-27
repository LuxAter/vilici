#ifndef VILICI_CONFIG_HPP_
#define VILICI_CONFIG_HPP_

#include <exception>
#include <filesystem>
#include <iostream>
#include <map>
#include <memory>
#include <string>
#include <variant>
#include <vector>

namespace config {

class Value {
public:
  enum ValueType { STRING, INTEGER, FLOAT, BOOLEAN, TABLE, ARRAY };
  Value() : type_(STRING), value_(std::string()) {}
  Value(const char *val) : type_(STRING), value_(std::string(val)) {}
  Value(const std::string &val) : type_(STRING), value_(val) {}
  Value(const int &val) : type_(INTEGER), value_(static_cast<long>(val)) {}
  Value(const long &val) : type_(INTEGER), value_(val) {}
  Value(const unsigned &val) : type_(INTEGER), value_(static_cast<long>(val)) {}
  Value(const double &val) : type_(FLOAT), value_(val) {}
  Value(const bool &val) : type_(BOOLEAN), value_(val) {}
  Value(const std::map<std::string, Value> &value_map) : type_(TABLE) {
    std::map<std::string, std::shared_ptr<Value>> ptr_map;
    for (auto &kv : value_map) {
      ptr_map.insert({kv.first, std::make_shared<Value>(Value(kv.second))});
    }
    value_ = ptr_map;
  }
  Value(const std::vector<Value> &value_vec) : type_(ARRAY) {
    std::vector<std::shared_ptr<Value>> ptr_vec;
    for (auto &it : value_vec) {
      ptr_vec.push_back(std::make_shared<Value>(Value(it)));
    }
    value_ = ptr_vec;
  }
  Value(const ValueType &type) : type_(type), value_(std::string()) {
    switch (type_) {
    case STRING:
      value_ = std::string();
      break;
    case INTEGER:
      value_ = long();
      break;
    case FLOAT:
      value_ = double();
      break;
    case BOOLEAN:
      value_ = false;
      break;
    case TABLE:
      value_ = std::map<std::string, std::shared_ptr<Value>>();
      break;
    case ARRAY:
      value_ = std::vector<std::shared_ptr<Value>>();
      break;
    }
  }
  void append(const Value &val) {
    if (type_ == ARRAY) {
      // auto val_array =
      std::get<std::vector<std::shared_ptr<Value>>>(value_).push_back(
          std::make_shared<Value>(val));
      // val_array.push_back(std::make_shared<Value>(val));
      // value_ = val_array;
    }
  }
  void append(const std::string &key, const Value &val) {
    if (type_ == TABLE) {
      // auto table =
      std::get<std::map<std::string, std::shared_ptr<Value>>>(value_).insert(
          {key, std::make_shared<Value>(val)});
      // table.insert({key, std::make_shared<Value>(val)});
      // value_ = table;
    }
  }

  Value &operator[](const std::size_t &id) {
    if (type_ == ARRAY) {
      return *std::get<std::vector<std::shared_ptr<Value>>>(value_)[id];
    }
    throw std::invalid_argument(
        "numeric index is only available for array types");
  }
  Value &operator[](const std::string &id) {
    std::string key, sub_key;
    if (id.find('.') != std::string::npos) {
      key = id.substr(0, id.find('.'));
      sub_key = id.substr(key.size() + 1);
    } else {
      key = id;
    }
    if (type_ == TABLE) {
      if (sub_key == std::string()) {
        return *std::get<std::map<std::string, std::shared_ptr<Value>>>(
            value_)[key];
      } else {
        return (*std::get<std::map<std::string, std::shared_ptr<Value>>>(
            value_)[key])[sub_key];
      }
    } else if (type_ == ARRAY &&
               key.find_first_not_of("0123456789") == std::string::npos) {
      if (sub_key == std::string()) {
        return *std::get<std::vector<std::shared_ptr<Value>>>(
            value_)[std::stol(key)];
      } else {

        return (*std::get<std::vector<std::shared_ptr<Value>>>(
            value_)[std::stol(key)])[sub_key];
      }
    }
    throw std::invalid_argument(
        "string index is only available for table types");
  }
  Value operator[](const std::string &id) const {
    std::string key, sub_key;
    if (id.find('.') != std::string::npos) {
      key = id.substr(0, id.find('.'));
      sub_key = id.substr(key.size() + 1);
    } else {
      key = id;
    }
    if (type_ == TABLE) {
      if (sub_key == std::string()) {
        return *std::get<std::map<std::string, std::shared_ptr<Value>>>(value_)
                    .at(key);
      } else {
        return (
            *std::get<std::map<std::string, std::shared_ptr<Value>>>(value_).at(
                key))[sub_key];
      }
    } else if (type_ == ARRAY &&
               key.find_first_not_of("0123456789") == std::string::npos) {
      if (sub_key == std::string()) {
        return *std::get<std::vector<std::shared_ptr<Value>>>(
            value_)[std::stol(key)];
      } else {

        return (*std::get<std::vector<std::shared_ptr<Value>>>(
            value_)[std::stol(key)])[sub_key];
      }
    }
    throw std::invalid_argument(
        "string index is only available for table types");
  }
  bool has(const std::string &id) const {
    std::string key, sub_key;
    if (id.find('.') != std::string::npos) {
      key = id.substr(0, id.find('.'));
      sub_key = id.substr(key.size() + 1);
    } else {
      key = id;
    }
    if (type_ == TABLE) {
      std::map<std::string, std::shared_ptr<Value>> val_table =
          std::get<std::map<std::string, std::shared_ptr<Value>>>(value_);
      if (sub_key == std::string()) {
        return val_table.find(key) != val_table.end();
      } else if (val_table.find(key) != val_table.end()) {
        return val_table.find(key)->second->has(sub_key);
      }
      return false;
    } else if (type_ == ARRAY &&
               key.find_first_not_of("0123456789") == std::string::npos) {
      if (sub_key == std::string()) {
        return std::get<std::vector<std::shared_ptr<Value>>>(value_).size() >
               std::stoul(key);
      } else if (std::get<std::vector<std::shared_ptr<Value>>>(value_).size() >
                 std::stoul(key)) {
        return std::get<std::vector<std::shared_ptr<Value>>>(
                   value_)[std::stoul(key)]
            ->has(sub_key);
      }
      return false;
    }
    return false;
  }
  template <typename _T>
  typename std::enable_if<std::is_arithmetic<_T>::value, _T>::type as() const {
    switch (type_) {
    case Value::INTEGER:
      return _T(std::get<long>(value_));
    case Value::FLOAT:
      return _T(std::get<double>(value_));
    case Value::BOOLEAN:
      return _T(std::get<bool>(value_));
    default:
      return _T();
    };
  }
  template <typename _T>
  typename std::enable_if<std::is_same<_T, std::string>::value,
                          std::string>::type
  as() const {
    switch (type_) {
    case Value::STRING:
      return std::get<std::string>(value_);
    case Value::INTEGER:
      return std::to_string(std::get<long>(value_));
    case Value::FLOAT:
      return std::to_string(std::get<double>(value_));
    case Value::BOOLEAN:
      return std::get<bool>(value_) ? "true" : "false";
    case Value::TABLE: {
      std::string res = "{";
      for (auto &kv :
           std::get<std::map<std::string, std::shared_ptr<Value>>>(value_)) {
        res += kv.first + "=" + kv.second->as<std::string>() + ',';
      }
      if (std::get<std::map<std::string, std::shared_ptr<Value>>>(value_)
              .size() != 0)
        res.pop_back();
      return res + "}";
    }
    case Value::ARRAY: {
      std::string res = "[";
      for (auto &it : std::get<std::vector<std::shared_ptr<Value>>>(value_)) {
        res += it->as<std::string>() + ',';
      }
      if (std::get<std::vector<std::shared_ptr<Value>>>(value_).size() != 0)
        res.pop_back();
      return res + "]";
    }
    default:
      return std::string();
    };
  }
  template <typename _T>
  typename std::enable_if<!std::is_arithmetic<_T>::type &&
                              !std::is_same<_T, std::string>::value,
                          _T>::type
  as() const {
    return _T();
  }

  ValueType type_;

private:
  std::variant<std::string, long, double, bool,
               std::vector<std::shared_ptr<Value>>,
               std::map<std::string, std::shared_ptr<Value>>>
      value_;
};

typedef std::map<std::string, Value> Table;
typedef std::vector<Value> Array;

extern Value cfg;

Value sub_str(const Value &res, const Value &id);
Value parse_str(const std::string &val);

Value load_ini(const std::string &config_path);
Value load_json(const std::string &config_path);
Value load_toml(const std::string &config_path);
Value load_yaml(const std::string &config_path);

#ifdef DEBUG
bool load(const std::string &config_path = "./resources/config");
#else
bool load(const std::string &config_path = "~/.config/vilici/config");
#endif
} // namespace config

#endif // VILICI_CONFIG_HPP_
