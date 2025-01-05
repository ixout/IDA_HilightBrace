#include "MatchingBrace.hpp"

//must return 0
ssize_t idaapi HandleMatchingBrace(void* ud, hexrays_event_t event, va_list va) {
  vdui_t* vdui = va_arg(va, vdui_t*);
  int shift_state = va_arg(va, int);
  strvec_t& codes = vdui->cfunc->sv;
  ea_t start = vdui->cfunc->entry_ea;
  bool needRefresh = false;
  int now_line = vdui->cpos.lnnum;
  int match_line = now_line;
  if (now_line < codes.size()) {
    qstring line;
    tag_remove(&line, codes[now_line].line);
    const char* pos = strpbrk(line.c_str(), "{}");
    if (pos != NULL) {
      const char* double_pos = strpbrk(pos + 1, "{}");
      if (double_pos != nullptr)
        return 0;
      if (!MatchingBrace::GetInstance().IsLineExist(start, now_line)) {
        int npos = pos - line.c_str();
        char brace = *pos;
        bool isFind = false;
        if (brace == '{') {
          for (;match_line < codes.size();) {
            tag_remove(&line, codes[++match_line].line);
            char dst = line.at(npos);
            if (dst == '}') {
              isFind = true;
              break;
            }
          }
        }
        else {
          for (; match_line < codes.size();) {
            tag_remove(&line, codes[--match_line].line);
            char dst = line.at(npos);
            if (dst == '{') {
              isFind = true;
              break;
            }
          }
        }
        if (!isFind) {
          return 0;
        }
        unsigned color = MatchingBrace::GetColor();
        codes[now_line].bgcolor = color;
        codes[match_line].bgcolor = color;
        MatchingBrace::GetInstance().AddMatched(start, now_line, match_line, color);
      }
      else {
        auto [x, y] = MatchingBrace::GetInstance().DeleteMatched(start, now_line);
        codes[x].bgcolor = 0xffffffff;
        codes[y].bgcolor = 0xffffffff;
      }
      needRefresh = true;
    }
  }
  if (needRefresh) {
    refresh_custom_viewer(vdui->ct);
  }
  return 0;
}

ssize_t idaapi HandleComeBack(void* ud, hexrays_event_t event, va_list va) {
  cfunc_t* cfunc = va_arg(va, cfunc_t*);
  ea_t start = cfunc->entry_ea;
  strvec_t& codes = cfunc->sv;
  if (MatchingBrace::GetInstance().IsFuncNeedScan(start)) {
    std::vector<MarkedLine>& Lines = MatchingBrace::GetInstance().GetHilghtLines(start);
    for (auto& it : Lines) {
     unsigned color = MatchingBrace::GetColor();
     codes[it.up].bgcolor = it.color;
     codes[it.down].bgcolor = it.color;
    }
  }
  return 0;
}