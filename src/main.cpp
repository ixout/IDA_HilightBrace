#include <hexrays.hpp>
#include "MatchingBrace.hpp"

ssize_t idaapi MyHexraysCallback(void* ud, hexrays_event_t event, va_list va) {
  if (event == hxe_double_click) {
    HandleMatchingBrace(ud, event, va);
  }
  else if (event == hxe_func_printed) {
    HandleComeBack(ud, event, va);
  }
  return 0;
}

plugmod_t* idaapi init(void)
{
  if (!init_hexrays_plugin()) {
    msg("Hex-Rays decompiler is not available.\n");
    return PLUGIN_SKIP;
  }

  if (!install_hexrays_callback(MyHexraysCallback, nullptr)) {
    msg("Failed to install Hex-Rays callback.\n");
    return PLUGIN_SKIP;
  }

  return PLUGIN_KEEP;
}

void idaapi term(void)
{
  remove_hexrays_callback(MyHexraysCallback, nullptr);
  term_hexrays_plugin();
  return;
}

bool idaapi run(size_t) {
  msg("This plugin works automatically on double-click events.\n");
  return true;
}

static char comment[] = "It's a simple plugin to improve experience!";

plugin_t PLUGIN =
{
  IDP_INTERFACE_VERSION,
  0,
  init,
  term,
  run,
  comment,
  "",
  "",
  ""
};