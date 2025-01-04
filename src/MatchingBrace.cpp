#include "MatchingBrace.hpp"

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
        return 1;
      if (!MatchingBrace::GetInstance().IsExist(start, now_line)) {
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
          return 1;
        }
        unsigned color = MatchingBrace::GetColor();
        codes[now_line].bgcolor = color;
        codes[match_line].bgcolor = color;
        MatchingBrace::GetInstance().AddMatched(start, now_line, match_line);
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
  return 1;
}