/*
 * TinySAAI: console access to every menu item, keypad parameter and measurement.
 *
 * Included at the end of ui.c, so it can use ui.c's static functions and tables;
 * it adds no code paths of its own to the instrument, it drives the existing menu
 * callbacks and keypad handling exactly as the touch screen does.
 *
 *   ui                 list the top menu
 *   ui ls [key]        list a menu: key|kind|state|value|label per item
 *   ui tree            every menu reachable from the top, depth first
 *   ui get <key>       one item
 *   ui <key> [values...] [confirm]
 *                      press an item; values answer the keypad prompts it opens
 *   param              every keypad parameter: name|class|value
 *   param <name> [<value> [confirm]]
 *   measure            the measurement in use
 *   measure <name> [values...]
 *
 * Keys are the menu labels in lower case with '_' between words, joined by '.',
 * e.g. display.waterfall, level.trigger.single, frequency.rbw.rbw_3.
 *
 * Copyright (c) 2026 the TinySAAI contributors. GPL-3.0-or-later, as the rest
 * of the tinySA firmware.
 */

#define AI_QUEUE_MAX   8
#define AI_KEY_LEN    96
#define AI_DEPTH_MAX   8

static const char *ai_queue[AI_QUEUE_MAX];
static int ai_queue_len, ai_queue_pos;
static bool ai_queue_active;            // a TinySAAI command answers the keypad prompts
static const char *ai_missing;          // the first prompt that found no value

// ui_process_keypad() calls this for a keypad opened from the console
static void ai_keypad_answer(void)
{
  if (!ai_queue_active)                 // plain "menu": upstream behaviour
    return;
  if (ai_queue_pos < ai_queue_len) {
    ui_set_keypad_text(ai_queue[ai_queue_pos++]);
    uistat.value = my_atof(kp_buf);
    uistat.freq_value = my_atoui(kp_buf);
    set_numeric_value();
  } else if (ai_missing == NULL) {
    ai_missing = kp_help_text ? kp_help_text : keypads_mode_tbl[keypad_mode].name;
  }
  // Close the keypad exactly as ui_process_keypad() does after a touch entry; this
  // also lets a second prompt for the same parameter (OIP3's left and right
  // frequencies) open instead of being skipped
  kp_help_text = NULL;
  redraw_frame();
  if (current_menu_is_form()) {
    ui_mode_menu();
    selection = -1;
  } else
    ui_mode_normal();
}

static void ai_queue_start(int argc, char *argv[])
{
  ai_queue_len = argc < AI_QUEUE_MAX ? argc : AI_QUEUE_MAX;
  for (int i = 0; i < ai_queue_len; i++)
    ai_queue[i] = argv[i];
  ai_queue_pos = 0;
  ai_missing = NULL;
  ai_queue_active = true;
}

// Report a prompt left without a value, or values nobody asked for; true if all is well
static bool ai_queue_finish(const char *what)
{
  ai_queue_active = false;
  if (ai_missing) {
    char text[40];
    plot_printf(text, sizeof text, "%s", ai_missing);
    for (char *p = text; *p; p++) if (*p == '\n') *p = ' ';
    shell_printf("error: %s needs a value for \"%s\"\r\n", what, text);
    return false;
  }
  if (ai_queue_pos < ai_queue_len) {
    shell_printf("error: %s used %d of %d values; it was pressed with those\r\n", what, ai_queue_pos, ai_queue_len);
    return false;
  }
  return true;
}

// ---------------------------------------------------------------- keypad parameters

// Parameters that write the stored configuration or retune the synthesizer
#define AI_CONFIG 1
static const struct { uint8_t mode; uint8_t flags; const char *name; } ai_params[] = {
  {KM_START, 0, "start"}, {KM_STOP, 0, "stop"}, {KM_CENTER, 0, "center"}, {KM_SPAN, 0, "span"},
  {KM_CW, 0, "cw"}, {KM_REFLEVEL, 0, "reflevel"}, {KM_SCALE, 0, "scale"},
  {KM_ATTENUATION, 0, "attenuation"}, {KM_ACTUALPOWER, AI_CONFIG, "actualpower"},
  {KM_IF, AI_CONFIG, "if"}, {KM_SAMPLETIME, 0, "sampletime"}, {KM_LOWOUTLEVEL, 0, "lowoutlevel"},
  {KM_DECAY, 0, "decay"}, {KM_NOISE, 0, "noise"}, {KM_FREQ_CORR, AI_CONFIG, "freq_corr"},
  {KM_REPEAT, 0, "repeat"}, {KM_EXT_GAIN, 0, "ext_gain"}, {KM_TRIGGER, 0, "trigger"},
  {KM_LEVELSWEEP, 0, "levelsweep"}, {KM_SWEEP_TIME, 0, "sweep_time"},
  {KM_OFFSET_DELAY, 0, "offset_delay"}, {KM_FAST_SPEEDUP, 0, "fast_speedup"},
  {KM_FASTER_SPEEDUP, 0, "faster_speedup"}, {KM_GRIDLINES, 0, "gridlines"},
  {KM_MARKER, 0, "marker"}, {KM_MODULATION, 0, "modulation"}, {KM_HIGHOUTLEVEL, 0, "highoutlevel"},
  {KM_COR_AM, AI_CONFIG, "cor_am"}, {KM_COR_WFM, AI_CONFIG, "cor_wfm"}, {KM_COR_NFM, AI_CONFIG, "cor_nfm"},
  {KM_DEVIATION, 0, "deviation"}, {KM_DEPTH, 0, "depth"}, {KM_IF2, AI_CONFIG, "if2"},
  {KM_R, AI_CONFIG, "r"}, {KM_MOD, AI_CONFIG, "mod"}, {KM_CP, AI_CONFIG, "cp"},
  {KM_ATTACK, 0, "attack"}, {KM_ULTRA_START, AI_CONFIG, "ultra_start"},
  {KM_HARM_START, AI_CONFIG, "harm_start"}, {KM_EXP_AVER, 0, "exp_aver"}, {KM_LEVEL, 0, "level"},
  {KM_LIMIT_FREQ, 0, "limit_freq"}, {KM_LIMIT_LEVEL, 0, "limit_level"},
  {KM_BAND_START, 0, "band_start"}, {KM_BAND_END, 0, "band_end"}, {KM_BAND_CENTER, 0, "band_center"},
  {KM_BAND_SPAN, 0, "band_span"}, {KM_BAND_LEVEL, 0, "band_level"}, {KM_BAND_NAME, 0, "band_name"},
  {KM_MARKER_TIME, 0, "marker_time"}, {KM_VAR, 0, "var"}, {KM_NF, AI_CONFIG, "nf"},
  {KM_LINEAR_SCALE, 0, "linear_scale"}, {KM_DIRECT_START, AI_CONFIG, "direct_start"},
  {KM_DIRECT_STOP, AI_CONFIG, "direct_stop"}, {KM_OVERCLOCK, AI_CONFIG, "overclock"},
  {KM_RTC_DATE, AI_CONFIG, "rtc_date"}, {KM_RTC_TIME, AI_CONFIG, "rtc_time"},
  {KM_INTERVAL, 0, "interval"}, {KM_TRIGGER_GRID, 0, "trigger_grid"},
};
#define AI_PARAMS (int)(sizeof ai_params / sizeof ai_params[0])

static int ai_param_flags(uint8_t mode)
{
  for (int i = 0; i < AI_PARAMS; i++)
    if (ai_params[i].mode == mode) return ai_params[i].flags;
  return 0;
}

// Copy text without control bytes, '|' or line breaks
static void ai_print_clean(const char *s)
{
  char out[64];
  int n = 0;
  for (; s && *s && n < (int)sizeof out - 1; s++) {
    unsigned char c = *s;
    if (c == '\n') { if (n < (int)sizeof out - 3) { out[n++] = ' '; out[n++] = '/'; out[n++] = ' '; } continue; }
    if (c < 0x20 || c > 0x7e || c == '|') c = ' ';
    if (c == ' ' && (n == 0 || out[n-1] == ' ')) continue;
    out[n++] = c;
  }
  while (n > 0 && out[n-1] == ' ') n--;
  out[n] = 0;
  shell_printf("%s", out);
}

void ai_cmd_param(int argc, char *argv[])
{
  if (argc == 0) {
    for (int i = 0; i < AI_PARAMS; i++) {
      uistat.text[0] = 0;
      fetch_numeric_target(ai_params[i].mode);
      shell_printf("%s|%s|", ai_params[i].name, ai_params[i].flags & AI_CONFIG ? "config" : "setting");
      ai_print_clean(uistat.text);
      shell_printf("\r\n");
    }
    return;
  }
  int i;
  for (i = 0; i < AI_PARAMS; i++)
    if (strcmp(argv[0], ai_params[i].name) == 0) break;
  if (i == AI_PARAMS || argc > 3) {
    shell_printf("usage: param [name [value [confirm]]]\r\n");
    return;
  }
  if (argc >= 2) {
    bool confirmed = argc == 3 && strcmp(argv[2], "confirm") == 0;
    if (argc == 3 && !confirmed) {
      shell_printf("usage: param [name [value [confirm]]]\r\n");
      return;
    }
    if ((ai_params[i].flags & AI_CONFIG) && !confirmed) {
      shell_printf("error: %s changes the stored configuration; add confirm\r\n", ai_params[i].name);
      return;
    }
    keypad_mode = ai_params[i].mode;
    ui_set_keypad_text(argv[1]);
    uistat.value = my_atof(kp_buf);
    uistat.freq_value = my_atoui(kp_buf);
    set_numeric_value();
    redraw_request |= REDRAW_CAL_STATUS | REDRAW_AREA | REDRAW_FREQUENCY;
  }
  uistat.text[0] = 0;
  fetch_numeric_target(ai_params[i].mode);
  shell_printf("%s|%s|", ai_params[i].name, ai_params[i].flags & AI_CONFIG ? "config" : "setting");
  ai_print_clean(uistat.text);
  shell_printf("\r\n");
}

// ---------------------------------------------------------------- menu items

// Items that wait for a person at the screen, or erase the configuration: never from the console
static bool ai_item_refused(const menuitem_t *m)
{
  const void *r = m->reference;
  int t = MT_MASK(m->type);
  if (t == MT_CALLBACK && r == (const void *)menu_config_cb)
    return m->data != CONFIG_MENUITEM_CALIBRATE;      // touch cal/test, notepad, self test, version
  if (t == MT_CALLBACK && (r == (const void *)menu_clearconfig_cb
#ifdef __USE_USB_MSC__
                           || r == (const void *)menu_usb_disk_cb
#endif
                           || r == (const void *)menu_sdcard_browse_cb))
    return true;
  if (t == MT_ADV_CALLBACK && (r == (const void *)menu_brightness_acb
                               || r == (const void *)menu_connection_acb       // would cut the USB console
                               || r == (const void *)menu_serial_speed_acb))
    return true;
  if ((t & 0x7) == MT_CALLBACK && r == (const void *)menu_calibrate_cb && m->data != 2)
    return true;                                      // ends in a touch wait: use "calibrate"
  return false;
}

// Items that change calibration or correction data: need "confirm"
static bool ai_item_guarded(const menuitem_t *m)
{
  const void *r = m->reference;
  int t = MT_MASK(m->type);
  if (t == MT_KEYPAD)
    return ai_param_flags(m->data) & AI_CONFIG;
  if (t == MT_CALLBACK && (r == (const void *)menu_input_curve_prepare_cb
                           || r == (const void *)menu_output_curve_prepare_cb
                           || r == (const void *)menu_curve_confirm_cb))
    return true;
  if (t == MT_CALLBACK && r == (const void *)menu_calibrate_cb)
    return true;                                      // RESET CALIBRATION
  if (t == MT_ADV_CALLBACK && (r == (const void *)menu_audio_agc_acb      // preferences saved to flash
                               || r == (const void *)menu_config_mode_acb
                               || r == (const void *)menu_direct_acb
                               || r == (const void *)menu_flip_acb
                               || r == (const void *)menu_harmonic_acb
                               || r == (const void *)menu_hide_21MHz_acb
                               || r == (const void *)menu_save_state_acb
                               || r == (const void *)menu_sd_icon_save_capture_acb
                               || r == (const void *)menu_sd_icon_save_traces_acb
                               || r == (const void *)menu_ultra_acb))
    return true;
  if (t == MT_ADV_CALLBACK && (r == (const void *)menu_output_level_acb
                               || r == (const void *)menu_output_level2_acb
                               || r == (const void *)menu_output_level3_acb
                               || r == (const void *)menu_curve_acb))
    return true;
  return false;
}

// Names for items whose label is computed at run time
static const char *ai_item_name(const menuitem_t *m)
{
  const void *r = m->reference;
  if (r == (const void *)menu_spur_acb) return "spur";
  if (r == (const void *)menu_atten_acb) return "auto";
  if (r == (const void *)menu_waterfall_acb) return "waterfall";
  if (r == (const void *)menu_smodulation_acb) return "modulation";
  if (r == (const void *)menu_sweep_acb) return "sweep";
  if (r == (const void *)menu_restart_acb) return "restart";
  if (r == (const void *)menu_trigger_level_acb) return "trigger_level";
  if (r == (const void *)menu_rbw_acb) return "rbw";
  if (r == (const void *)menu_vbw_acb) return "vbw";
  if (r == (const void *)menu_reffer_acb) return "cal_output";
  if (r == (const void *)menu_store_preset_acb || r == (const void *)menu_load_preset_acb) return "preset";
  if (r == (const void *)menu_store_trace_acb) return "to_trace";
  if (r == (const void *)menu_subtract_trace_acb) return "subtract";
  if (r == (const void *)menu_traces_acb) return m->data == 3 ? "calc" : m->data == 5 ? "math" : NULL;
  if (r == (const void *)menu_modulation_acb && m->data == MO_NONE) return "none";
  if (r == (const void *)menu_points_acb) return "points";
  if (r == (const void *)menu_serial_speed_acb) return "speed";
  if (r == (const void *)menu_curve_acb) return "point";
  if (r == (const void *)menu_mixer_drive_acb) return "drive";
  if (r == (const void *)menu_marker_select_acb || r == (const void *)menu_marker_ref_select_acb) return "marker";
  if (r == (const void *)menu_trace_acb || r == (const void *)menu_marker_trace_acb) return "trace";
  if (MT_MASK(m->type) == MT_KEYPAD && m->data == KM_CENTER) return "center";
  return NULL;
}

// Lower case words joined by '_', printf conversions dropped
static void ai_slug(const char *label, char *out, int size)
{
  int n = 0;
  bool gap = false;
  for (const char *p = label; p && *p; p++) {
    char c = *p;
    if (c == '%') {
      while (p[1] && !strchr("diouxXeEfFgGcsQqDUbp%", p[1])) p++;
      if (p[1]) p++;
      gap = true;
      continue;
    }
    if (c >= 'A' && c <= 'Z') c += 'a' - 'A';
    if ((c >= 'a' && c <= 'z') || (c >= '0' && c <= '9')) {
      if (gap && n > 0 && n < size - 1) out[n++] = '_';
      gap = false;
      if (n < size - 1) out[n++] = c;
    } else
      gap = true;
  }
  out[n] = 0;
}

// An ordinary item is named by its label; a repeated group, or an item whose label
// is computed at run time, by the name table
static void ai_item_base(const menuitem_t *m, char *out, int size)
{
  const char *name = ai_item_name(m);
  out[0] = 0;
  if (!(m->type & MT_REPEATS) && m->label != MT_CUSTOM_LABEL)
    ai_slug(m->label, out, size);
  if (out[0] == 0 && name)
    plot_printf(out, size, "%s", name);
  if (out[0] == 0 && m->label != MT_CUSTOM_LABEL)
    ai_slug(m->label, out, size);
  if (out[0] == 0)
    plot_printf(out, size, "%s", MT_MASK(m->type) == MT_CANCEL ? "back" : "item");
}

// The key of item number pos of menu: base, the repeat number, and a suffix if an
// earlier item has the same base
static void ai_item_key(const menuitem_t *menu, int pos, char *out, int size)
{
  const menuitem_t *m = menu;
  int sub = 0, i = 0, same = 0;
  char base[32], other[32];
  const menuitem_t *target = NULL;
  int target_sub = 0;
  for (; m; m = menu_next_item(m, &sub), i++)
    if (i == pos) { target = m; target_sub = sub; break; }
  if (!target) { out[0] = 0; return; }
  ai_item_base(target, base, sizeof base);
  if (target->type & MT_REPEATS) {
    plot_printf(out, size, "%s_%d", base, target_sub + 1);
    return;
  }
  m = menu; sub = 0; i = 0;
  for (; m && i < pos; m = menu_next_item(m, &sub), i++) {
    if (m->type & MT_REPEATS) continue;
    ai_item_base(m, other, sizeof other);
    if (strcmp(base, other) == 0) same++;
  }
  if (same)
    plot_printf(out, size, "%s_%d", base, same + 1);
  else
    plot_printf(out, size, "%s", base);
}

static const menuitem_t *ai_item_at(const menuitem_t *menu, int pos, int *sub)
{
  const menuitem_t *m = menu;
  *sub = 0;
  for (int i = 0; m && i < pos; i++) m = menu_next_item(m, sub);
  return m;
}

// The menu the screen starts from: the generator form in output modes
static const menuitem_t *ai_root(void)
{
  if (setting.mode == M_GENLOW)  return menu_lowoutputmode;
  if (setting.mode == M_GENHIGH) return menu_highoutputmode;
  return menu_top;
}

// The submenu an item opens: its own for MT_SUBMENU, or the one its callback pushes
// without changing anything first. Callbacks that edit state while their menu is
// drawn (multi band, limit tables) are left out, so listing stays read only.
static const menuitem_t *ai_submenu_of(const menuitem_t *m)
{
  const void *r = m->reference;
  int t = MT_MASK(m->type);
  if (t == MT_SUBMENU) return (const menuitem_t *)r;
  if (t != MT_CALLBACK && t != MT_ADV_CALLBACK && t != MT_TITLE) return NULL;
  if (r == (const void *)menu_sweep_acb) return menu_sweep;
  if (r == (const void *)menu_smodulation_acb) return menu_modulation;
  if (r == (const void *)menu_sreffer_acb) return menu_reffer;
  if (r == (const void *)menu_config_cb && m->data == CONFIG_MENUITEM_CALIBRATE) return menu_calibrate;
  if (t != MT_TITLE && m->data == 0) {               // the firmware pushes only for data 0
    if (r == (const void *)menu_calibrate_normal_acb) return menu_calibrate_normal;
    if (r == (const void *)menu_calibrate_harmonic_acb) return menu_calibrate_harmonic;
  }
  if (r == (const void *)menu_traces_acb)
    return m->data == 0 ? menu_trace : m->data == 3 ? menu_average : m->data == 5 ? menu_subtract_trace : NULL;
  if (r == (const void *)menu_trigger_level_acb && m->data == 254) return menu_trigger_level;
  if (r == (const void *)menu_marker_modify_acb)
    return m->data == 0 ? menu_marker_select : m->data == M_STORED ? menu_marker_trace : NULL;
  if (r == (const void *)menu_lowoutput_settings_acb && m->data == 255)
    return max2871 ? menu_lowoutput_settings_max : menu_lowoutput_settings;
  if (r == (const void *)menu_internals_acb) return menu_settings2;        // asks for code 5432
  return NULL;
}

// Resolve a dotted key. Fills the position of each level; returns the item, the
// menu it is in in *menu and the depth in *depth, or NULL.
static const menuitem_t *ai_path_item[AI_DEPTH_MAX];   // the item pressed at each level

static const menuitem_t *ai_resolve(const char *key, const menuitem_t **menu, int *path, int *depth)
{
  const menuitem_t *cur = ai_root(), *found = NULL;
  char seg[40], k[40];
  *depth = 0;
  while (*key) {
    int n = 0;
    while (*key && *key != '.' && n < (int)sizeof seg - 1) seg[n++] = *key++;
    seg[n] = 0;
    if (*key == '.') key++;
    if (found) {                                       // descend
      if (!ai_submenu_of(found) || *depth >= AI_DEPTH_MAX) return NULL;
      cur = ai_submenu_of(found);
    }
    found = NULL;
    int sub = 0, pos = 0;
    for (const menuitem_t *m = cur; m; m = menu_next_item(m, &sub), pos++) {
      ai_item_key(cur, pos, k, sizeof k);
      if (strcmp(k, seg) == 0) { found = m; ai_path_item[*depth] = m; path[(*depth)++] = pos; break; }
    }
    if (!found) return NULL;
  }
  *menu = cur;
  return found;
}

static const char *ai_kind(const menuitem_t *m)
{
  switch (MT_MASK(m->type)) {
  case MT_SUBMENU:      return "menu";
  case MT_CALLBACK:     return "action";
  case MT_ADV_CALLBACK: return "setting";
  case MT_CANCEL:       return "back";
  case MT_TITLE:        return "title";
  case MT_KEYPAD:       return "value";
  }
  return "?";
}

// One "key|kind|state|value|label" line, rendered as the screen renders the button
static void ai_print_item(const menuitem_t *menu, int pos, const char *prefix)
{
  int sub;
  const menuitem_t *m = ai_item_at(menu, pos, &sub);
  ui_button_t button;
  char key[40];
  const char *text = m->label;
  const char *value = "";
  button.icon = BUTTON_ICON_NONE;
  button.text[0] = 0;
  button.param_1.u = 0;
  menu_item_modify_attribute(menu, pos, &button);
  if (MT_MASK(m->type) == MT_ADV_CALLBACK || (MT_MASK(m->type) == MT_TITLE && m->reference)) {
    menuaction_acb_t cb = (menuaction_acb_t)m->reference;
    if (cb) (*cb)(pos, (m->type & MT_REPEATS) ? (m->data & 0x0f) + sub : m->data, &button);
    if (m->label != MT_CUSTOM_LABEL)
      plot_printf(button.text, sizeof button.text, m->label, button.param_1.u);
    text = button.text;
  }
  if (MT_MASK(m->type) == MT_KEYPAD) {
    uistat.text[0] = 0;                               // not every mode fills it
    fetch_numeric_target(m->data);
    value = uistat.text;
    plot_printf(button.text, sizeof button.text, m->label, uistat.text);
    text = button.text;
  }
  ai_item_key(menu, pos, key, sizeof key);
  const char *state = "-";
  switch (button.icon) {
  case BUTTON_ICON_CHECK: case BUTTON_ICON_CHECK_AUTO: case BUTTON_ICON_CHECK_MANUAL:
  case BUTTON_ICON_GROUP_CHECKED: state = "on"; break;
  case BUTTON_ICON_NOCHECK: case BUTTON_ICON_GROUP: state = "off"; break;
  }
  if (ai_item_refused(m)) state = "refused";
  else if (ai_item_guarded(m)) state = strcmp(state, "-") ? state : "confirm";
  shell_printf("%s%s|%s|%s|", prefix, key,
               ai_submenu_of(m) && MT_MASK(m->type) != MT_SUBMENU ? "menu" : ai_kind(m), state);
  ai_print_clean(value);
  shell_printf("|");
  ai_print_clean(text && text != MT_CUSTOM_LABEL ? text : "");
  shell_printf("\r\n");
}

// "a.b.c" -> "a.b." (the part before the last key, with its dot)
static void ai_parent_prefix(const char *key, char *out, int size)
{
  const char *dot = strrchr(key, '.');
  int n = dot ? (int)(dot - key) + 1 : 0;
  if (n > size - 1) n = size - 1;
  memcpy(out, key, n);
  out[n] = 0;
}

static void ai_list(const menuitem_t *menu, const char *prefix)
{
  int sub = 0, pos = 0;
  for (const menuitem_t *m = menu; m; m = menu_next_item(m, &sub), pos++)
    ai_print_item(menu, pos, prefix);
}

// Depth first over submenus, without recursion (the sweep thread stack is small)
static void ai_tree(void)
{
  static const menuitem_t *menus[AI_DEPTH_MAX];
  static int next[AI_DEPTH_MAX];
  static char prefix[AI_KEY_LEN];
  static int prefix_len[AI_DEPTH_MAX];
  int level = 0;
  menus[0] = ai_root(); next[0] = 0; prefix[0] = 0; prefix_len[0] = 0;
  while (level >= 0) {
    int sub;
    const menuitem_t *m = ai_item_at(menus[level], next[level], &sub);
    if (!m) { level--; continue; }
    int pos = next[level]++;
    prefix[prefix_len[level]] = 0;
    ai_print_item(menus[level], pos, prefix);
    bool cycle = false;                               // never re-enter a menu on the path
    for (int l = 0; l <= level; l++) if (menus[l] == ai_submenu_of(m)) cycle = true;
    if (ai_submenu_of(m) && !cycle && level + 1 < AI_DEPTH_MAX) {
      char key[40];
      ai_item_key(menus[level], pos, key, sizeof key);
      int len = prefix_len[level];
      plot_printf(prefix + len, sizeof prefix - len, "%s.", key);
      menus[level + 1] = ai_submenu_of(m);
      next[level + 1] = 0;
      prefix_len[level + 1] = strlen(prefix);
      level++;
    }
  }
}

void ai_cmd_ui(int argc, char *argv[])
{
  const menuitem_t *menu, *m;
  int path[AI_DEPTH_MAX], depth;
  if (argc == 0) { ai_list(ai_root(), ""); return; }
  if (strcmp(argv[0], "tree") == 0 && argc == 1) { ai_tree(); return; }
  if (strcmp(argv[0], "ls") == 0 && argc <= 2) {
    if (argc == 1) { ai_list(ai_root(), ""); return; }
    m = ai_resolve(argv[1], &menu, path, &depth);
    if (!m || !ai_submenu_of(m)) { shell_printf("error: no menu %s\r\n", argv[1]); return; }
    static char prefix[AI_KEY_LEN];   // static: the sweep thread stack is small
    plot_printf(prefix, sizeof prefix, "%s.", argv[1]);
    ai_list(ai_submenu_of(m), prefix);
    return;
  }
  if (strcmp(argv[0], "get") == 0 && argc == 2) {
    m = ai_resolve(argv[1], &menu, path, &depth);
    if (!m) { shell_printf("error: no item %s\r\n", argv[1]); return; }
    static char prefix[AI_KEY_LEN];   // static: the sweep thread stack is small
    ai_parent_prefix(argv[1], prefix, sizeof prefix);
    ai_print_item(menu, path[depth - 1], prefix);
    return;
  }
  // press an item
  m = ai_resolve(argv[0], &menu, path, &depth);
  if (!m) { shell_printf("error: no item %s\r\n", argv[0]); return; }
  int values = argc - 1;
  bool confirmed = values > 0 && strcmp(argv[argc - 1], "confirm") == 0;
  if (confirmed) values--;
  if (ai_item_refused(m)) {
    shell_printf("error: %s is refused from the console (it waits for a person at the screen, "
                 "cuts the USB console or erases the configuration; for level calibration use calibrate)\r\n", argv[0]);
    return;
  }
  if (ai_item_guarded(m) && !confirmed) {
    shell_printf("error: %s changes the stored configuration or calibration; add confirm\r\n", argv[0]);
    return;
  }
  if (values > AI_QUEUE_MAX) { shell_printf("error: at most %d values\r\n", AI_QUEUE_MAX); return; }
  ui_mode_normal();
  menu_current_level = 0;
  in_menu_command = true;
  if (ai_root() != menu_top)                          // generator: its form is the root
    menu_push_submenu(ai_root());
  ai_queue_start(values, &argv[1]);
  for (int i = 0; i < depth; i++) {
    menu_invoke(path[i]);
    // A press on the way must have opened the menu the next key is in (INTERNALS
    // opens only with its code); otherwise the next position is in another menu
    if (i < depth - 1 && menu_stack[menu_current_level] != ai_submenu_of(ai_path_item[i])) {
      in_menu_command = false;
      ai_queue_active = false;
      ui_mode_normal();
      shell_printf("error: %s did not open; stopped before pressing anything inside it\r\n", argv[0]);
      return;
    }
  }
  in_menu_command = false;
  bool ok = ai_queue_finish(argv[0]);
  if (!ai_submenu_of(m) && !current_menu_is_form())
    ui_mode_normal();
  redraw_request |= REDRAW_CAL_STATUS | REDRAW_AREA | REDRAW_FREQUENCY;
  if (!ok) return;
  static char prefix[AI_KEY_LEN];     // static: the sweep thread stack is small
  ai_parent_prefix(argv[0], prefix, sizeof prefix);
  shell_printf("ok ");
  ai_print_item(menu, path[depth - 1], prefix);
}

// ---------------------------------------------------------------- measurements

static const char * const ai_measure_names[] = {
  "off", "harmonic", "oip3", "phase_noise", "snr", "pass_band", "occupied_width", "am", "fm",
  "thd", "channel_power", "nf_tinysa", "nf_store", "nf_validate", "nf_amplifier", "deconv", "linearity",
};

void ai_cmd_measure(int argc, char *argv[])
{
  int n = sizeof ai_measure_names / sizeof ai_measure_names[0];
  if (argc == 0) {
    shell_printf("measure %s\r\n", setting.measurement < n ? ai_measure_names[setting.measurement] : "unknown");
    return;
  }
  int i;
  for (i = 0; i < n; i++)
    if (strcmp(argv[0], ai_measure_names[i]) == 0) break;
#ifndef __FFT_DECONV__
  if (i == M_DECONV) i = n;                           // not built
#endif
  if (i == n || i >= M_MAX) {
    shell_printf("usage: measure [off|harmonic|oip3|phase_noise|snr|pass_band|occupied_width|am|fm|thd|"
                 "channel_power|nf_tinysa|nf_store|nf_validate|nf_amplifier|linearity] [values...]\r\n");
    return;
  }
  int values = argc - 1;
  bool confirmed = values > 0 && strcmp(argv[argc - 1], "confirm") == 0;
  if (confirmed) values--;
  if (i == M_NF_STORE && !confirmed) {
    shell_printf("error: nf_store saves the noise figure to the stored configuration; add confirm\r\n");
    return;
  }
  if (values > AI_QUEUE_MAX) { shell_printf("error: at most %d values\r\n", AI_QUEUE_MAX); return; }
  ui_mode_normal();
  menu_current_level = 0;
  in_menu_command = true;
  ai_queue_start(values, &argv[1]);
  menu_measure_acb(0, i, NULL);
  in_menu_command = false;
  if (!ai_queue_finish(argv[0])) {
    in_menu_command = true;                           // undo a half set up measurement
    menu_measure_acb(0, M_OFF, NULL);
    in_menu_command = false;
    ui_mode_normal();
    return;
  }
  ui_mode_normal();
  shell_printf("measure %s\r\n", ai_measure_names[setting.measurement < n ? setting.measurement : 0]);
}

// ---------------------------------------------------------------- bands and limits

#ifdef __BANDS__
// band            every band: n|on/off|active|start|end|level|name
// band <n>        select band n and enable it (as pressing it in the band menu)
// band <n> off    disable band n (the last enabled band stays)
// Then param band_start/band_end/band_center/band_span/band_level/band_name set it.
void ai_cmd_band(int argc, char *argv[])
{
  if (argc >= 1) {
    int n = (int)my_atoui(argv[0]) - 1;
    if (n < 0 || n >= BANDS_MAX || argc > 2 || (argc == 2 && strcmp(argv[1], "off") != 0)) {
      shell_printf("usage: band [1..%d [off]]\r\n", BANDS_MAX);
      return;
    }
    if (argc == 2) {
      int count = 0;
      for (int i = 0; i < BANDS_MAX; i++) if (setting.bands[i].enabled) count++;
      if (count == 1 && setting.bands[n].enabled) {
        shell_printf("error: at least one band stays enabled\r\n");
        return;
      }
      setting.bands[n].enabled = false;
      setting.bands[n].name[0] = 0;
    } else {
      active_band = n;
      setting.bands[n].enabled = true;
    }
    dirty = true;
    update_frequencies();
    update_grid();
    redraw_request |= REDRAW_CAL_STATUS | REDRAW_AREA | REDRAW_FREQUENCY;
  }
  for (int i = 0; i < BANDS_MAX; i++) {
    band_t *b = &setting.bands[i];
    shell_printf("%d|%s|%s|%U|%U|%.1f|", i + 1, b->enabled ? "on" : "off",
                 i == active_band ? "active" : "-", b->start, b->end, b->level);
    ai_print_clean(b->name);
    shell_printf("\r\n");
  }
  shell_printf("multi_band %s\r\n", setting.multi_band ? "on" : "off");
}
#endif

// limit           the limit lines of the selected trace: n|on/off|active|frequency|level
// limit <n>       select limit n and enable it
// limit <n> off   disable limit n (the last enabled one stays)
// Select the trace with ui trace.trace.trace_<t>; then param limit_freq/limit_level.
void ai_cmd_limit(int argc, char *argv[])
{
  if (argc >= 1) {
    int n = (int)my_atoui(argv[0]) - 1;
    if (n < 0 || n >= LIMITS_MAX || argc > 2 || (argc == 2 && strcmp(argv[1], "off") != 0)) {
      shell_printf("usage: limit [1..%d [off]]\r\n", LIMITS_MAX);
      return;
    }
    if (argc == 2) {
      int count = 0;
      for (int i = 0; i < LIMITS_MAX; i++) if (setting.limits[current_trace][i].enabled) count++;
      if (count == 1 && setting.limits[current_trace][n].enabled) {
        shell_printf("error: at least one limit stays enabled\r\n");
        return;
      }
      setting.limits[current_trace][n].enabled = false;
    } else {
      active_limit = n;
      setting.limits[current_trace][n].enabled = true;
    }
    dirty = true;
    limits_update();
    redraw_request |= REDRAW_AREA;
  }
  shell_printf("trace %d\r\n", current_trace + 1);
  for (int i = 0; i < LIMITS_MAX; i++) {
    limit_t *l = &setting.limits[current_trace][i];
    shell_printf("%d|%s|%s|%U|%.2f\r\n", i + 1, l->enabled ? "on" : "off",
                 i == active_limit ? "active" : "-", l->frequency, l->level);
  }
}
