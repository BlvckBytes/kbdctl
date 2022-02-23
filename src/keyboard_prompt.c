#include "keyboard_prompt.h"

/*
============================================================================
                                  Commands                                  
============================================================================
*/

INLINED static char *keyboard_prompt_list(char *args, keyboard_prompt_state_t *state)
{
  return keyboard_devman_list();
}

INLINED static char *keyboard_prompt_select(char *args, keyboard_prompt_state_t *state)
{
  // Check that no selections are present already
  if (state->kb)
    return strfmt_direct("Already selected a device!\n");

  size_t args_offs = 0;
  scptr char *vid = partial_strdup(args, &args_offs, " ", false);
  scptr char *pid = partial_strdup(args, &args_offs, " ", false);
  scptr char *ser = partial_strdup(args, &args_offs, " ", false);

  if (!vid | !pid)
    return strfmt_direct("Usage: select <VID> <PID> [SER]\n");

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

INLINED static char *keyboard_prompt_unselect(char *args, keyboard_prompt_state_t *state)
{
  if (!state->kb)
    return strfmt_direct("No device selected!\n");

  mman_dealloc(state->kb);
  state->kb = NULL;
  return strfmt_direct("Device unselected!\n");
}

INLINED static char *keyboard_prompt_what(char *args, keyboard_prompt_state_t *state)
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

INLINED static char *keyboard_prompt_effect_gen_usage()
{
  scptr char *buf = (char *) mman_alloc(sizeof(char), 128, NULL);
  size_t buf_offs = 0;
  strfmt(&buf, &buf_offs, "Usage: effect <list/apply> <effect> [<");

  // Fill in available targets from enum
  size_t len = keyboard_ctl_frame_target_length();
  for (size_t i = 0; i < len; i++)
  {
    keyboard_ctl_frame_target_t tar;
    if (keyboard_ctl_frame_target_by_index(i, &tar) == ENUMLUT_SUCCESS)
      strfmt(&buf, &buf_offs, "%s%s", i == 0 ? "" : "/", keyboard_ctl_frame_target_name(tar));
  }

  strfmt(&buf, &buf_offs, "> <time in ms> [color as hex]]\n");
  return mman_ref(buf);
}

INLINED static char *keyboard_prompt_effect(char *args, keyboard_prompt_state_t *state)
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
    return keyboard_prompt_effect_gen_usage();

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
      return keyboard_prompt_effect_gen_usage();

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

    // Create parameterized effect frame and send
    scptr uint8_t *data = keyboard_ctl_frame_make(TYPE_EFFECT);
    keyboard_ctl_frame_effect_apply(data, eff, time_l, kcol, store_b);
    keyboard_ctl_frame_target_apply(data, targ);
    if (!keyboard_transmit(state->kb, data, mman_fetch_meta(data)->num_blocks))
      return strfmt_direct("Could not transmit data to the device!\n");

    return strfmt_direct("Applied effect!\n");
  }

  return keyboard_prompt_effect_gen_usage();
}

INLINED static char *keyboard_prompt_exit(char *args, keyboard_prompt_state_t *state)
{
  // Invoke unselect routine and dealloc it's result, as it's not needed
  mman_dealloc(keyboard_prompt_unselect(NULL, state));

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
  if (htable_fetch(state->commands, cmd, (void **) &pc) != HTABLE_SUCCESS)
  {
    scptr char **cmds = NULL;
    htable_list_keys(state->commands, &cmds);

    scptr char *buf = (char *) mman_alloc(sizeof(char), 128, NULL);
    size_t buf_offs = 0;

    // Add all available commands from the table
    strfmt(&buf, &buf_offs, "Unknown, available commands:\n");
    for (char **c = cmds; *c; c++)
      strfmt(&buf, &buf_offs, "- %s\n", *c);

    return mman_ref(buf);
  }

  return pc(args, state);
}

static void keyboard_prompt_state_cleanup(mman_meta_t *meta)
{
  keyboard_prompt_state_t *state = (keyboard_prompt_state_t *) meta->ptr;

  // Deallocate the keyboard wrapper and the command table
  mman_dealloc(state->kb);
  mman_dealloc(state->commands);
}

INLINED static htable_t *keyboard_prompt_build_command_table()
{
  scptr htable_t *cmds = htable_make(32, NULL);

  // TODO: Statuscolor
  // TODO: Bootmode
  // TODO: Deactivate
  // TODO: Set key color individually
  // TODO: Run animations

  htable_insert(cmds, "list", keyboard_prompt_list);
  htable_insert(cmds, "select", keyboard_prompt_select);
  htable_insert(cmds, "what", keyboard_prompt_what);
  htable_insert(cmds, "unselect", keyboard_prompt_unselect);
  htable_insert(cmds, "effect", keyboard_prompt_effect);
  htable_insert(cmds, "exit", keyboard_prompt_exit);

  return mman_ref(cmds);
}

keyboard_prompt_state_t *keyboard_prompt_state_make()
{
  scptr keyboard_prompt_state_t *state = mman_alloc(sizeof(keyboard_prompt_state_t), 1, keyboard_prompt_state_cleanup);

  // Set defaults
  state->kb = NULL;
  state->prompting = true;

  // Initialize command table
  state->commands = keyboard_prompt_build_command_table();

  return mman_ref(state);
}