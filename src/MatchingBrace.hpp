#pragma once
#include <ctime>
#include <ida.hpp>
#include <idp.hpp>
#include <loader.hpp>
#include <hexrays.hpp>
#include <kernwin.hpp>
#include <unordered_map>
#include <vector>

ssize_t idaapi HandleMatchingBrace(void* ud, hexrays_event_t event, va_list va);

class MatchingBrace {
public:
  MatchingBrace(const MatchingBrace& other) = delete;
  MatchingBrace& operator=(const MatchingBrace& other) = delete;

  static MatchingBrace& GetInstance() {
    static MatchingBrace instance;
    return instance;
  }

  bool IsExist(const ea_t key, const int n) {
    auto& vec = matched_[key];
    for (auto& it : vec) {
      if (n == it.first || n == it.second) {
        return true;
      }
    }
    return false;
  }
  void AddMatched(const ea_t key, const int pre, const int back) {
    auto& vec = matched_[key];
    vec.emplace_back(pre, back );
  }
  std::pair<int, int> DeleteMatched(const ea_t key, int n) {
    std::pair<int, int> result;
    auto& vec = matched_[key];
    for (auto it = vec.begin(); it != vec.end();) {
      if (n == it->first || n == it->second) {
        result = *it;
        it = vec.erase(it);
        break;
      }
      ++it;
    }
    return result;
  }
  static unsigned int GetColor() {
    static int colors_[10] = { 0xCD853F, 0xFFD700, 0xFFFF00, 0x7FFF00, 0xA5BF8E, 0x00BFFF, 0x8A2BE2, 0xFFEFD5, 0xFFC0CB, 0xD8BFD8 };
    srand(time(NULL));
    return colors_[rand() % 10];
  }
private:
  MatchingBrace() = default;
  std::unordered_map<ea_t, std::vector<std::pair<int, int>>> matched_;
};

