#include "keyboard_prompt.h"

/*
============================================================================
                               Shared Routines                              
============================================================================
*/

INLINED static char *keyboard_prompt_generate_usage(keyboard_prompt_state_t *state, char *cmd)
{
  char *usage = NULL;
  if (htable_fetch(state->usages, cmd, (void **) &usage) == HTABLE_SUCCESS)
    return strclone(usage);
 return strfmt_direct("Could not find a usage for " QUOTSTR "!\n", cmd);
}

INLINED static char *keyboard_prompt_parse_key_color(char *key, char *color, keyboard_key_color_t **out)
{
  // Parse the key
  keyboard_key_t k;
  if (keyboard_key_value(key, &k) != ENUMLUT_SUCCESS)
    return strfmt_direct("Invalid key: " QUOTSTR "\n", key);

  // Parse key color
  keyboard_color_t kcol = { 0x00, 0x00, 0x00 };
  long color_l;
  if (longp(&color_l, color, 16) != LONGP_SUCCESS)
    return strfmt_direct("Invalid color hexadecimal number: %s\n", color);
  keyboard_color_apply_number(&kcol, color_l);

  // Create key color and write to output buffer
  scptr keyboard_key_color_t *k_color = keyboard_key_color_make(k, kcol);
  *out = mman_ref(k_color);

  // No error occurred
  return NULL;
}

INLINED static char *keyboard_prompt_generate_keylist()
{
  scptr char *buf = (char *) mman_alloc(sizeof(char), 128, NULL);
  size_t buf_offs = 0;
  
  strfmt(&buf, &buf_offs, "Available keys:\n");

  // Fill in available keys from enum
  size_t len = keyboard_key_length();
  for (size_t i = 0; i < len; i++)
  {
    keyboard_key_t tar;
    if (keyboard_key_by_index(i, &tar) == ENUMLUT_SUCCESS)
      strfmt(&buf, &buf_offs, "- %s\n", keyboard_key_name(tar));
  }

  strfmt(&buf, &buf_offs, "\n");
  return mman_ref(buf);
}

INLINED static void keyboard_prompt_write_targets(char **buf, size_t *buf_offs)
{
  // Fill in available targets from enum
  size_t len = keyboard_ctl_frame_target_length();
  for (size_t i = 0; i < len; i++)
  {
    keyboard_ctl_frame_target_t tar;
    if (keyboard_ctl_frame_target_by_index(i, &tar) == ENUMLUT_SUCCESS)
      strfmt(buf, buf_offs, "%s%s", i == 0 ? "" : "/", keyboard_ctl_frame_target_name(tar));
  }
}

INLINED static void keyboard_prompt_write_bootmode(char **buf, size_t *buf_offs)
{
  // Fill in available bootmodes from enum
  size_t len = keyboard_boot_mode_length();
  for (size_t i = 0; i < len; i++)
  {
    keyboard_boot_mode_t tar;
    if (keyboard_boot_mode_by_index(i, &tar) == ENUMLUT_SUCCESS)
      strfmt(buf, buf_offs, "%s%s", i == 0 ? "" : "/", keyboard_boot_mode_name(tar));
  }
}

INLINED static void keyboard_prompt_kill_animation(keyboard_prompt_state_t *state)
{
  // Quit previous animation
  if (state->curr_anim)
  {
    keyboard_animation_quit(state->curr_anim);
    mman_dealloc(state->curr_anim);
    state->curr_anim = NULL;
  }
}

/*
============================================================================
                               Command "LIST"                               
============================================================================
*/

INLINED static char *keyboard_prompt_list(char *cmd, char *args, keyboard_prompt_state_t *state)
{
  return keyboard_devman_list();
}

/*
============================================================================
                               Command "SELECT"                             
============================================================================
*/

INLINED static char *keyboard_prompt_select(char *cmd, char *args, keyboard_prompt_state_t *state)
{
  // Check that no selections are present already
  if (state->kb)
    return strfmt_direct("Already selected a device!\n");

  size_t args_offs = 0;
  scptr char *vid = partial_strdup(args, &args_offs, " ", false);
  scptr char *pid = partial_strdup(args, &args_offs, " ", false);
  scptr char *ser = partial_strdup(args, &args_offs, " ", false);

  if (!vid | !pid)
    return keyboard_prompt_generate_usage(state, cmd);

  // Parse vendor id number
  long vid_l;
  if (longp(&vid_l, vid, 16) != LONGP_SUCCESS)
    return strfmt_direct("Invalid VID hexadecimal number: " QUOTSTR "\n", vid);

  // Parse product id number
  long pid_l;
  if (longp(&pid_l, pid, 16) != LONGP_SUCCESS)
    return strfmt_direct("Invalid PID hexadecimal number: " QUOTSTR "\n", pid);

  // Search device
  scptr keyboard_t *device = keyboard_devman_find(vid_l, pid_l, ser);
  if (!device)
    return strfmt_direct("Could not find the device %s:%s!\n", vid, pid);

  // Try to open the device
  scptr char *err = NULL;
  if (!keyboard_open(device, &err))
    return strfmt_direct("Could not open the device %s:%s (%s)!\n", vid, pid, err);

  // Apply state change
  state->kb = mman_ref(device);
  return strfmt_direct("Successfully selected the device %s:%s!\n", vid, pid);
}

/*
============================================================================
                              Command "UNSELECT"                            
============================================================================
*/

INLINED static char *keyboard_prompt_unselect(char *cmd, char *args, keyboard_prompt_state_t *state)
{
  if (!state->kb)
    return strfmt_direct("No device selected!\n");

  // Kill any existing animation
  keyboard_prompt_kill_animation(state);

  mman_dealloc(state->kb);
  state->kb = NULL;
  return strfmt_direct("Device unselected!\n");
}

/*
============================================================================
                                Command "WHAT"                              
============================================================================
*/

INLINED static char *keyboard_prompt_what(char *cmd, char *args, keyboard_prompt_state_t *state)
{
  if (!state->kb)
    return strfmt_direct("No device selected!\n");

  // Print info about selected device
  keyboard_t kb = *(state->kb);
  return strfmt_direct(
    "Currently selected: "
    "VID=%04X, PID=%04X, SER=" QUOTSTR ", PATH=" QUOTSTR "\n",
    kb.vendor_id, kb.product_id, kb.serial_str, kb.path
  );
}

/*
============================================================================
                               Command "EFFECT"                             
============================================================================
*/

INLINED static char *keyboard_prompt_effect_gen_usage()
{
  scptr char *buf = (char *) mman_alloc(sizeof(char), 128, NULL);
  size_t buf_offs = 0;
  strfmt(&buf, &buf_offs, "Usage: effect <list/apply> <effect> [<");
  keyboard_prompt_write_targets(&buf, &buf_offs);
  strfmt(&buf, &buf_offs, "> <time in ms> [color as hex]]");
  return mman_ref(buf);
}

INLINED static char *keyboard_prompt_effect(char *cmd, char *args, keyboard_prompt_state_t *state)
{
  if (!state->kb)
    return strfmt_direct("No device selected!\n");

  size_t args_offs = 0;
  scptr char *action = partial_strdup(args, &args_offs, " ", false);
  scptr char *effect = partial_strdup(args, &args_offs, " ", false);
  scptr char *target = partial_strdup(args, &args_offs, " ", false);
  scptr char *time = partial_strdup(args, &args_offs, " ", false);
  scptr char *color = partial_strdup(args, &args_offs, " ", false);
  scptr char *store = partial_strdup(args, &args_offs, " ", false);

  if (!action)
    return keyboard_prompt_generate_usage(state, cmd);

  // List all available effects
  if (strcasecmp(action, "list") == 0)
  {
    scptr char *buf = (char *) mman_alloc(sizeof(char), 128, NULL);
    size_t buf_offs = 0;
    
    strfmt(&buf, &buf_offs, "Available effects:\n");

    // Print out all known effect values
    size_t len = keyboard_effect_length();
    for (size_t i = 0; i < len; i++)
    {
      keyboard_effect_t tar;
      if (keyboard_effect_by_index(i, &tar) == ENUMLUT_SUCCESS)
        strfmt(&buf, &buf_offs, "- %s%s", keyboard_effect_name(tar), i == len - 1 ? "" : "\n");
    }

    strfmt(&buf, &buf_offs, "\n");
    return mman_ref(buf);
  }

  // Apply a selected effect
  if (strcasecmp(action, "apply") == 0)
  {
    // Allow for null-colors and null-store-bools
    if (!effect || !target || !time)
      return keyboard_prompt_generate_usage(state, cmd);

    // Parse effect from string
    keyboard_effect_t eff;
    if (keyboard_effect_value(effect, &eff) != ENUMLUT_SUCCESS)
      return strfmt_direct("Unknown effect: " QUOTSTR "\n", effect);

    // Parse target from string
    keyboard_ctl_frame_target_t targ;
    if (keyboard_ctl_frame_target_value(target, &targ) != ENUMLUT_SUCCESS)
      return strfmt_direct("Unknown target: " QUOTSTR "\n", target);

    // Parse time from string
    long time_l;
    if (longp(&time_l, time, 10) != LONGP_SUCCESS)
      return strfmt_direct("Invalid time decimal number: %s\n", time);

    // Parse key color, fallback to off
    keyboard_color_t kcol = { 0x00, 0x00, 0x00 };
    if (color)
    {
      long color_l;
      if (longp(&color_l, color, 16) != LONGP_SUCCESS)
        return strfmt_direct("Invalid color hexadecimal number: %s\n", color);

      keyboard_color_apply_number(&kcol, color_l);
    }

    // Parse store boolean, fallback to false
    bool store_b = false;
    if (store && strcasecmp(store, "true"))
      store_b = true;

    // Kill any existing animation
    keyboard_prompt_kill_animation(state);

    // Create parameterized effect frame and send
    scptr uint8_t *data = keyboard_ctl_frame_make(TYPE_EFFECT);
    keyboard_ctl_frame_effect_apply(data, eff, time_l, kcol, store_b);
    keyboard_ctl_frame_target_apply(data, targ);
    if (!keyboard_transmit(state->kb, data, mman_fetch_meta(data)->num_blocks))
      return strfmt_direct("Could not transmit data to the device!\n");

    return strfmt_direct("Applied effect!\n");
  }

  return keyboard_prompt_generate_usage(state, cmd);
}

/*
============================================================================
                              Command "BOOTMODE"                            
============================================================================
*/

INLINED static char *keyboard_prompt_bootmode_gen_usage()
{
  scptr char *buf = (char *) mman_alloc(sizeof(char), 128, NULL);
  size_t buf_offs = 0;
  strfmt(&buf, &buf_offs, "Usage: bootmode <");
  keyboard_prompt_write_bootmode(&buf, &buf_offs);
  strfmt(&buf, &buf_offs, ">");
  return mman_ref(buf);
}

INLINED static char *keyboard_prompt_bootmode(char *cmd, char *args, keyboard_prompt_state_t *state)
{
  if (!state->kb)
    return strfmt_direct("No device selected!\n");

  size_t args_offs = 0;
  scptr char *bootmode = partial_strdup(args, &args_offs, " ", false);

  if (!bootmode)
    return keyboard_prompt_generate_usage(state, cmd);

  // Parse bootmode from string
  keyboard_boot_mode_t mode;
  if (keyboard_boot_mode_value(bootmode, &mode) != ENUMLUT_SUCCESS)
    return strfmt_direct("Unknown bootmode: " QUOTSTR "\n", bootmode);
  
  // Create parameterized deactivate frame and send
  scptr uint8_t *data = keyboard_ctl_frame_make(TYPE_BOOT_MODE);
  keyboard_ctl_frame_boot_mode_apply(data, mode);
  if (!keyboard_transmit(state->kb, data, mman_fetch_meta(data)->num_blocks))
    return strfmt_direct("Could not transmit data to the device!\n");

  return strfmt_direct("Set bootmode to " QUOTSTR "!\n", keyboard_boot_mode_name(mode));
}

/*
============================================================================
                             Command "DEACTIVATE"                           
============================================================================
*/

INLINED static char *keyboard_prompt_deactivate_gen_usage()
{
  scptr char *buf = (char *) mman_alloc(sizeof(char), 128, NULL);
  size_t buf_offs = 0;
  strfmt(&buf, &buf_offs, "Usage: deactivate <");
  keyboard_prompt_write_targets(&buf, &buf_offs);
  strfmt(&buf, &buf_offs, ">");
  return mman_ref(buf);
}

INLINED static char *keyboard_prompt_deactivate(char *cmd, char *args, keyboard_prompt_state_t *state)
{
  if (!state->kb)
    return strfmt_direct("No device selected!\n");

  size_t args_offs = 0;
  scptr char *target = partial_strdup(args, &args_offs, " ", false);

  if (!target)
    return keyboard_prompt_generate_usage(state, cmd);

  // Parse target from string
  keyboard_ctl_frame_target_t targ;
  if (keyboard_ctl_frame_target_value(target, &targ) != ENUMLUT_SUCCESS)
    return strfmt_direct("Unknown target: " QUOTSTR "\n", target);

  // Kill any existing animation
  if (targ == TARG_KEYS)
    keyboard_prompt_kill_animation(state);
  
  // Create parameterized deactivate frame and send
  scptr uint8_t *data = keyboard_ctl_frame_make(TYPE_DEACTIVATE);
  keyboard_ctl_frame_target_apply(data, targ);
  if (!keyboard_transmit(state->kb, data, mman_fetch_meta(data)->num_blocks))
    return strfmt_direct("Could not transmit data to the device!\n");

  return strfmt_direct("Deactivated " QUOTSTR "!\n", keyboard_ctl_frame_target_name(targ));
}

/*
============================================================================
                           Command "STATUSCOLOR"                           
============================================================================
*/

INLINED static char *keyboard_prompt_statuscolor(char *cmd, char *args, keyboard_prompt_state_t *state)
{
  if (!state->kb)
    return strfmt_direct("No device selected!\n");

  size_t args_offs = 0;
  scptr char *color = partial_strdup(args, &args_offs, " ", false);

  if (!color)
    return keyboard_prompt_generate_usage(state, cmd);

  // Parse status color
  keyboard_color_t kcol = { 0x00, 0x00, 0x00 };
  long color_l;
  if (longp(&color_l, color, 16) != LONGP_SUCCESS)
    return strfmt_direct("Invalid color hexadecimal number: %s\n", color);
  keyboard_color_apply_number(&kcol, color_l);

  // Create items frame and both available status colors
  scptr uint8_t *data = keyboard_ctl_frame_make(TYPE_KEYS);
  scptr keyboard_key_color_t *stat_backl = keyboard_key_color_make(KEY_STATUS_BACKLIGHT, kcol);
  scptr keyboard_key_color_t *stat_game = keyboard_key_color_make(KEY_STATUS_GAME, kcol);
  keyboard_key_color_t *keys_arr[] = {stat_backl, stat_game};

  // Transmit keys frame
  size_t num_keys = sizeof(keys_arr) / sizeof(keyboard_key_color_t *), statuses_offs = 0;
  keyboard_ctl_frame_key_list_apply(data, keys_arr, num_keys, KGA_STATUS, &statuses_offs);
  if (!keyboard_transmit(state->kb, data, mman_fetch_meta(data)->num_blocks))
    return strfmt_direct("Could not transmit data to the device!\n");

  // Commit changes and thus make them visible
  scptr uint8_t *data_comm = keyboard_ctl_frame_make(TYPE_COMMIT);
  if (!keyboard_transmit(state->kb, data_comm, mman_fetch_meta(data_comm)->num_blocks))
    return strfmt_direct("Could not transmit data to the device!\n");

  return strfmt_direct("Applied statuscolor %s!\n", color);
}

/*
============================================================================
                                Command "KEY"                               
============================================================================
*/

INLINED static char *keyboard_prompt_key(char *cmd, char *args, keyboard_prompt_state_t *state)
{
  if (!state->kb)
    return strfmt_direct("No device selected!\n");

  size_t args_offs = 0;
  scptr char *action = partial_strdup(args, &args_offs, " ", false);

  if (!action)
    return keyboard_prompt_generate_usage(state, cmd);

  if (strcasecmp(action, "list") == 0)
    return keyboard_prompt_generate_keylist();

  if (strcasecmp(action, "apply") == 0)
  {
    scptr char *key = partial_strdup(args, &args_offs, " ", false);
    scptr char *color = partial_strdup(args, &args_offs, " ", false);

    // No key color left in the arguments buffer
    if (!key || !color)
      return keyboard_prompt_generate_usage(state, cmd);

    // Parse key and key color
    scptr keyboard_key_color_t *k_color = NULL;
    scptr char *parse_err = keyboard_prompt_parse_key_color(key, color, &k_color);

    // Error during parsing
    if (parse_err)
      return mman_ref(parse_err);

    // Map key if applicable
    scptr char *keymap_lang = partial_strdup(args, &args_offs, " ", false);
    if (keymap_lang)
      k_color->key = keyboard_keymapper_lookup(state->mappings, keymap_lang, k_color->key);

    // Kill any existing animation
    keyboard_prompt_kill_animation(state);

    // Make parameterized items frame
    size_t keys_offs = 0;
    scptr uint8_t *data_keys = keyboard_ctl_frame_make(TYPE_KEYS);
    keyboard_ctl_frame_key_list_apply(data_keys, &k_color, 1, KGA_KEY, &keys_offs);

    // Transmit frame
    if (!keyboard_transmit(state->kb, data_keys, mman_fetch_meta(data_keys)->num_blocks))
      return strfmt_direct("Could not transmit data to the device!\n");

    // Commit changes and thus make them visible
    scptr uint8_t *data_comm = keyboard_ctl_frame_make(TYPE_COMMIT);
    if (!keyboard_transmit(state->kb, data_comm, mman_fetch_meta(data_comm)->num_blocks))
      return strfmt_direct("Could not transmit data to the device!\n");

    return strfmt_direct("Applied color %s to key %s!\n", color, key);
  }

  return keyboard_prompt_generate_usage(state, cmd);
}

/*
============================================================================
                                Command "KEYS"                              
============================================================================
*/

INLINED static char *keyboard_prompt_keys(char *cmd, char *args, keyboard_prompt_state_t *state)
{
  if (!state->kb)
    return strfmt_direct("No device selected!\n");

  size_t args_offs = 0;
  scptr char *action = partial_strdup(args, &args_offs, " ", false);

  if (!action)
    return keyboard_prompt_generate_usage(state, cmd);

  if (strcasecmp(action, "list") == 0)
    return keyboard_prompt_generate_keylist();

  if (strcasecmp(action, "apply") == 0)
  {
    // Get the desired keymap language
    scptr char *keymap_lang = partial_strdup(args, &args_offs, " ", false);
    if (!keymap_lang)
      return keyboard_prompt_generate_usage(state, cmd);

    scptr dynarr_t *keys = dynarr_make(32, keyboard_key_length(), mman_dealloc_nr);

    // Loop as long as keys are available
    while (true)
    {
      scptr char *key = partial_strdup(args, &args_offs, " ", false);
      scptr char *color = partial_strdup(args, &args_offs, " ", false);
      if (!key || !color) break;

      // Parse key and key color
      scptr keyboard_key_color_t *k_color = NULL;
      scptr char *parse_err = keyboard_prompt_parse_key_color(key, color, &k_color);

      // Error during parsing
      if (parse_err)
        return mman_ref(parse_err);

      // Map key if applicable
      if (keymap_lang)
        k_color->key = keyboard_keymapper_lookup(state->mappings, keymap_lang, k_color->key);

      // Insert key to list and quit if the array is full
      if (dynarr_push(keys, mman_ref(k_color), NULL) != dynarr_SUCCESS)
        break;
    }

    // Get all key-color entries
    scptr keyboard_key_color_t **key_arr = NULL;
    size_t num_keys = dynarr_as_array(keys, (void ***) &key_arr);

    // Make items frame
    scptr uint8_t *data_keys = keyboard_ctl_frame_make(TYPE_KEYS);

    // Append all keys and send, may take multiple frames as one frame has limited capacity
    size_t keys_offs = 0;
    while (num_keys != keys_offs)
    {
      keyboard_ctl_frame_key_list_apply(data_keys, key_arr, num_keys, KGA_KEY, &keys_offs);
      if (!keyboard_transmit(state->kb, data_keys, mman_fetch_meta(data_keys)->num_blocks))
        return strfmt_direct("Could not transmit data to the device!\n");
      usleep(1000 * 10);
    }

    // Commit changes and thus make them visible
    scptr uint8_t *data_comm = keyboard_ctl_frame_make(TYPE_COMMIT);
    if (!keyboard_transmit(state->kb, data_comm, mman_fetch_meta(data_comm)->num_blocks))
      return strfmt_direct("Could not transmit data to the device!\n");

    return strfmt_direct("Applied %lu key-colors!\n", num_keys);
  }

  return keyboard_prompt_generate_usage(state, cmd);
}

/*
============================================================================
                             Command "ANIMATION"                            
============================================================================
*/

INLINED static char *keyboard_prompt_animation(char *cmd, char *args, keyboard_prompt_state_t *state)
{
  if (!state->kb)
    return strfmt_direct("No device selected!\n");

  size_t args_offs = 0;
  scptr char *name = partial_strdup(args, &args_offs, " ", false);

  if (!name)
    return keyboard_prompt_generate_usage(state, cmd);

  // Parse animation from file
  scptr char *anim_err = NULL;
  scptr char *anim_path = strfmt_direct("%s/animations/%s.ini", state->kbdctl_dir, name);
  scptr keyboard_animation_t *anim = keyboard_animation_load(anim_path, &anim_err);
  if (!anim)
    return strfmt_direct("Could not parse the animation at " QUOTSTR ": %s\n", anim_path, anim_err);

  keyboard_prompt_kill_animation(state);

  // Launch animation
  if (!keyboard_animation_launch(anim, state->mappings, state->kb))
    return strfmt_direct("Could not launch the animation " QUOTSTR "!\n", name);

  state->curr_anim = mman_ref(anim);

  return strfmt_direct("Launched animation " QUOTSTR "!\n", name);
}

/*
============================================================================
                                Command "EXIT"                              
============================================================================
*/

INLINED static char *keyboard_prompt_exit(char *cmd, char *args, keyboard_prompt_state_t *state)
{
  // Kill any existing animation
  keyboard_prompt_kill_animation(state);

  // Invoke unselect routine and dealloc it's result, as it's not needed
  mman_dealloc(keyboard_prompt_unselect(NULL, NULL, state));

  // Stop prompting and return
  state->prompting = false;
  return strfmt_direct("Quitting application!\n");
}

/*
============================================================================
                              Command Processor                             
============================================================================
*/

char *keyboard_prompt_process(char *input, keyboard_prompt_state_t *state)
{
  size_t input_offs = 0;
  scptr char *cmd = partial_strdup(input, &input_offs, " ", false);
  scptr char *args = partial_strdup(input, &input_offs, "\0", false);

  keyboard_prompt_command_t pc;
  if (!cmd || htable_fetch(state->commands, cmd, (void **) &pc) != HTABLE_SUCCESS)
  {
    scptr char **cmds = NULL;
    size_t nkeys = htable_list_keys(state->commands, &cmds);

    scptr char *buf = (char *) mman_alloc(sizeof(char), 128, NULL);
    size_t buf_offs = 0;

    // Add all available commands from the table
    strfmt(&buf, &buf_offs, "=======================< Helpscreen >=======================\n");
    for (char **c = cmds; *c; c++)
    {
      scptr char *c_usage = keyboard_prompt_generate_usage(state, *c);
      strfmt(&buf, &buf_offs, "- %s\n", *c);
      strfmt(&buf, &buf_offs, "%s%s", c_usage, (c - cmds != nkeys - 1) ? "\n" : "");
    }
    strfmt(&buf, &buf_offs, "=======================< Helpscreen >=======================\n");
    return mman_ref(buf);
  }

  return pc(cmd, args, state);
}

/*
============================================================================
                                Command Table                               
============================================================================
*/

INLINED static void keyboard_prompt_build_command_table(keyboard_prompt_state_t *state)
{
  scptr htable_t *cmds = htable_make(32, NULL);
  scptr htable_t *usages = htable_make(32, mman_dealloc_nr);

  htable_insert(cmds, "list", keyboard_prompt_list);
  htable_insert(usages, "list", strfmt_direct(
    "%s\n%s\n",
    "Usage: list",
    "List all available USB devices"
  ));

  htable_insert(cmds, "select", keyboard_prompt_select);
  htable_insert(usages, "select", strfmt_direct(
    "%s\n%s\n",
    "Usage: select <VID> <PID> [SER]",
    "Select a USB device by it's vendor- and productid and an optional serial"
  ));

  htable_insert(cmds, "what", keyboard_prompt_what);
  htable_insert(usages, "what", strfmt_direct(
    "%s\n%s\n",
    "Usage: what",
    "Check what device is currently selected"
  ));

  htable_insert(cmds, "unselect", keyboard_prompt_unselect);
  htable_insert(usages, "unselect", strfmt_direct(
    "%s\n%s\n",
    "Usage: unselect",
    "Unselect and thus close the currently selected device"
  ));

  htable_insert(cmds, "effect", keyboard_prompt_effect);
  scptr char *effect_usage = keyboard_prompt_effect_gen_usage();
  htable_insert(usages, "effect", strfmt_direct(
    "%s\n%s\n",
    effect_usage,
    "Apply a built-in, parameterized effect to the keyboard"
  ));

  htable_insert(cmds, "deactivate", keyboard_prompt_deactivate);
  scptr char *deactivate_usage = keyboard_prompt_deactivate_gen_usage();
  htable_insert(usages, "deactivate", strfmt_direct(
    "%s\n%s\n",
    deactivate_usage,
    "Deactivate a group of keys by turning them off"
  ));

  htable_insert(cmds, "bootmode", keyboard_prompt_bootmode);
  scptr char *bootmode_usage = keyboard_prompt_bootmode_gen_usage();
  htable_insert(usages, "bootmode", strfmt_direct(
    "%s\n%s\n",
    bootmode_usage,
    "Set the mode the keyboard boots into"
  ));

  htable_insert(cmds, "statuscolor", keyboard_prompt_statuscolor);
  htable_insert(usages, "statuscolor", strfmt_direct(
    "%s\n%s\n",
    "Usage: statuscolor <color as hex>",
    "Set the color of status-keys"
  ));

  htable_insert(cmds, "animation", keyboard_prompt_animation);
  htable_insert(usages, "animation", strfmt_direct(
    "%s\n%s\n",
    "Usage: animation <name>",
    "Launch an animation by it's .ini-file name"
  ));

  htable_insert(cmds, "key", keyboard_prompt_key);
  htable_insert(usages, "key", strfmt_direct(
    "%s\n%s\n",
    "Usage: key <list/apply> <key> <color> [keymap-lang]",
    "Set an individual key's color"
  ));

  htable_insert(cmds, "keys", keyboard_prompt_keys);
  htable_insert(usages, "keys", strfmt_direct(
    "%s\n%s\n",
    "Usage: keys <list/apply> <keymap-lang> (<key> <color>)+",
    "Set multiple key colors at once"
  ));

  htable_insert(cmds, "exit", keyboard_prompt_exit);
  htable_insert(usages, "exit", strfmt_direct(
    "%s\n%s\n",
    "Usage: exit"
    "Exit the program and thus close any open devices"
  ));

  state->commands = mman_ref(cmds);
  state->usages = mman_ref(usages);
}

/*
============================================================================
                                 Prompt State                               
============================================================================
*/

static void keyboard_prompt_state_cleanup(mman_meta_t *meta)
{
  keyboard_prompt_state_t *state = (keyboard_prompt_state_t *) meta->ptr;

  // Deallocate the keyboard wrapper and the command table
  mman_dealloc(state->kb);
  mman_dealloc(state->commands);
  mman_dealloc(state->mappings);
  mman_dealloc(state->usages);
  mman_dealloc(state->curr_anim);
  mman_dealloc(state->kbdctl_dir);
}

keyboard_prompt_state_t *keyboard_prompt_state_make(const char *kbdctl_dir, char **err)
{
  scptr keyboard_prompt_state_t *state = mman_alloc(sizeof(keyboard_prompt_state_t), 1, keyboard_prompt_state_cleanup);

  // Set defaults
  state->kb = NULL;
  state->prompting = true;
  state->curr_anim = NULL;
  state->mappings = NULL;
  state->commands = NULL;

  // Set kbdctl base-path
  state->kbdctl_dir = strfmt_direct("%s", kbdctl_dir);

  // Initialize command table
  keyboard_prompt_build_command_table(state);

  // Initialize mappings table
  scptr char *keymap_err = NULL;
  scptr char *keymap_floc = strfmt_direct("%s/keymap.ini", kbdctl_dir);
  scptr htable_t *keymap = keyboard_keymapper_load(keymap_floc, &keymap_err);
  if (!keymap)
  {
    if (err)
      *err = strfmt_direct("ERROR: Could not parse the keymap at " QUOTSTR ": %s\n", keymap_floc, keymap_err);
    return NULL;
  }

  state->mappings = mman_ref(keymap);
  return mman_ref(state);
}