#pragma once
#include <ctime>
#include <unordered_map>
#include <vector>
#include <ida.hpp>
#include <idp.hpp>
#include <loader.hpp>
#include <hexrays.hpp>
#include <kernwin.hpp>

ssize_t idaapi HandleMatchingBrace(void* ud, hexrays_event_t event, va_list va);
ssize_t idaapi HandleComeBack(void* ud, hexrays_event_t event, va_list va);

struct MarkedLine {
  unsigned up, down, color;
  MarkedLine(unsigned u, unsigned d, unsigned c) : up(u), down(d), color(c) {}
};

class MatchingBrace {
public:
  MatchingBrace(const MatchingBrace& other) = delete;
  MatchingBrace& operator=(const MatchingBrace& other) = delete;

  static MatchingBrace& GetInstance() {
    static MatchingBrace instance;
    return instance;
  }

  bool IsFuncNeedScan(const ea_t key) {
    auto& vec = matched_[key];
    if (vec.empty()) {
      return false;
    }
    return true;
  }

  std::vector<MarkedLine>& GetHilghtLines(const ea_t key) {
    return matched_[key];
  }

  bool IsLineExist(const ea_t key, const int n) {
    auto& vec = matched_[key];
    for (auto& it : vec) {
      if (n == it.up || n == it.down) {
        return true;
      }
    }
    return false;
  }
  void AddMatched(const ea_t key, const unsigned pre, const unsigned back, const unsigned color) {
    auto& vec = matched_[key];
    vec.emplace_back(pre, back, color);
  }
  std::pair<unsigned, unsigned> DeleteMatched(const ea_t key, int n) {
    std::pair<unsigned, unsigned> result;
    auto& vec = matched_[key];
    for (auto it = vec.begin(); it != vec.end();) {
      if (n == it->up || n == it->down) {
        result.first = it->up;
        result.second = it->down;
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
  std::unordered_map<ea_t, std::vector<MarkedLine>> matched_;
};

