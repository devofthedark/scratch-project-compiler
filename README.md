# Scratch Project Compiler
A text-based programming language to help students transition from block-based programming to text-based in a familiar environment.

WARNING: This is very much a proof-of-concept, if you wish to use this, you are on your own.

Documentation of the language is coming soon. In the meantime, heres a code snippet to help you get started:

```
fn add(num a, num b) -> num {
    return a + b;
}
num c = add(100, add(10, 7));
```

The following is a binding to the standard scratch blocks provided by default:

```
// Movement category
fn move_steps(num steps) [stdcall] {
    __scratch_stdcall_hook_void("motion_movesteps", "STEPS", steps);
}

fn turn_right_by(num angle) [stdcall] {
    __scratch_stdcall_hook_void("motion_turnright", "DEGREES", angle);
}

fn turn_left_by(num angle) [stdcall] {
    __scratch_stdcall_hook_void("motion_turnleft", "DEGREES", angle);
}

fn move_to(str loc) [stdcall] {
    __scratch_stdcall_hook_void("motion_goto", "FIELD_BEGIN", "_random_", "_mouse_", "FIELD_END", "inputs", "motion_goto_menu", "TO", loc);
}

fn move_to_random() {
    move_to("_random_");
}

fn move_to_mouse() {
    move_to("_mouse_");
}

fn move_to(num x, num y) [stdcall] {
    __scratch_stdcall_hook_void("motion_gotoxy", "X", x, "Y", y);
}

fn glide_to(str loc, num time) [stdcall] {
    __scratch_stdcall_hook_void("motion_glideto", "FIELD_BEGIN", "_random_", "_mouse_", "FIELD_END", "inputs", "motion_glideto_menu", "TO", loc, "SECS", time);
}

fn glide_to_random(num time) {
    glide_to("_random_", time);
}

fn glide_to_mouse(num time) {
    glide_to("_mouse_", time);
}

fn glide_to(num x, num y, num time) [stdcall] {
    __scratch_stdcall_hook_void("motion_glidesecstoxy", "X", x, "Y", y, "SECS", time);
}

fn set_direction(num angle) [stdcall] {
    __scratch_stdcall_hook_void("motion_pointindirection", "DIRECTION", angle);
}

fn change_x_by(num dx) [stdcall] {
    __scratch_stdcall_hook_void("motion_changexby", "DX", dx);
}

fn change_y_by(num dy) [stdcall] {
    __scratch_stdcall_hook_void("motion_changeyby", "DY", dy);
}

fn set_xpos(num x) [stdcall] {
    __scratch_stdcall_hook_void("motion_setx", "X", x);
}

fn set_ypos(num y) [stdcall] {
    __scratch_stdcall_hook_void("motion_sety", "Y", y);
}

fn if_on_edge_bounce() [stdcall] {
    __scratch_stdcall_hook_void("motion_ifonedgebounce");
}

fn set_rotation_style(str style) [stdcall] {
    __scratch_stdcall_hook_void("motion_setrotationstyle", "FIELD_BEGIN", "left-right", "don't rotate", "all around", "FIELD_END", "fields", "STYLE", style);
}

fn xpos() -> num [stdcall] {
    return __scratch_stdcall_hook_return("motion_xposition");
}

fn ypos() -> num [stdcall] {
    return __scratch_stdcall_hook_return("motion_yposition");
}

fn direction() -> num [stdcall] {
    return __scratch_stdcall_hook_return("motion_direction");
}

// Looks category

fn switch_to_next_costume() [stdcall] {
    __scratch_stdcall_hook_void("looks_nextcostume");
}

fn switch_to_next_backdrop() [stdcall] {
    __scratch_stdcall_hook_void("looks_nextbackdrop");
}

fn change_size_by(num dsize) [stdcall] {
    __scratch_stdcall_hook_void("looks_changesizeby", "CHANGE", dsize);
}

fn set_size(num new_size) [stdcall] {
    __scratch_stdcall_hook_void("looks_setsizeto", "SIZE", new_size);
}

fn change_visual_effect_by(str effect, num delta) [stdcall] {
    __scratch_stdcall_hook_void("looks_changeeffectby", "FIELD_BEGIN", "COLOR", "FISHEYE", "WHIRL", "PIXELATE", "MOSAIC", "BRIGHTNESS", "GHOST", "FIELD_END", "fields", "EFFECT", effect, "CHANGE", delta);
}

fn set_visual_effect_to(str effect, num strength) [stdcall] {
    __scratch_stdcall_hook_void("looks_seteffectto", "FIELD_BEGIN", "COLOR", "FISHEYE", "WHIRL", "PIXELATE", "MOSAIC", "BRIGHTNESS", "GHOST", "FIELD_END", "fields", "EFFECT", effect, "VALUE", strength);
}

fn show_sprite() [stdcall] {
    __scratch_stdcall_hook_void("looks_show");
}

fn hide_sprite() [stdcall] {
    __scratch_stdcall_hook_void("looks_hide");
}

fn size() -> num [stdcall] {
    return __scratch_stdcall_hook_return("looks_size");
}

// Sound Category

fn stop_all_sounds() [stdcall] {
    __scratch_stdcall_hook_void("sound_stopallsounds");
}

fn change_sound_effect_by(str effect, num delta) [stdcall] {
    __scratch_stdcall_hook_void("sound_changeeffectby", "FIELD_BEGIN", "PITCH", "PAN", "FIELD_END", "fields", "EFFECT", effect, "VALUE", delta);
}

fn set_sound_effect_to(str effect, num value) [stdcall] {
    __scratch_stdcall_hook_void("sound_seteffectto", "FIELD_BEGIN", "PITCH", "PAN", "FIELD_END", "fields", "EFFECT", effect, "VALUE", value);
}

fn clear_sound_effects() [stdcall] {
    __scratch_stdcall_hook_void("sound_cleareffects");
}

fn change_volume_by(num volume) [stdcall] {
    __scratch_stdcall_hook_void("sound_changevolumeby", "VOLUME", volume);
}

fn set_volume(num volume) [stdcall] {
    __scratch_stdcall_hook_void("sound_setvolumeto", "VOLUME", volume);
}

fn volume() -> num [stdcall] {
    return __scratch_stdcall_hook_return("sound_volume");
}

// Control Category

fn sleep(num secs) [stdcall] {
    __scratch_stdcall_hook_void("control_wait", "DURATION", secs);
}

// Sensing category

fn ask_wait(num question) [stdcall] {
    __scratch_stdcall_hook_void("sensing_askandwait", "QUESTION", question);
}

fn set_drag_mode(str mode) [stdcall] {
    __scratch_stdcall_hook_void("sensing_setdragmode", "FIELD_BEGIN", "draggable", "not draggable", "FIELD_END", "fields", "DRAG_MODE", mode);
}

fn set_not_draggable() {
    set_drag_mode("not draggable");
}

fn set_draggable() {
    set_drag_mode("draggable");
}

fn mouse_x() -> num [stdcall] {
    return __scratch_stdcall_hook_return("sensing_mousex");
}

fn mouse_y() -> num [stdcall] {
    return __scratch_stdcall_hook_return("sensing_mousey");
}

fn loudness() -> num [stdcall] {
    return __scratch_stdcall_hook_return("sensing_loudness");
}

fn timer_time() -> num [stdcall] {
    return __scratch_stdcall_hook_return("sensing_timer");
}

fn reset_timer() [stdcall] {
    __scratch_stdcall_hook_void("sensing_resettimer");
}

fn get_time_property(str property) -> num [stdcall] {
    return __scratch_stdcall_hook_return("sensing_current", "FIELD_BEGIN", "YEAR", "MONTH", "DATE", "DAYOFWEEK", "HOUR", "MINUTE", "SECOND", "FIELD_END", "fields", "CURRENTMENU", property);
}

fn get_year() -> num {
    return get_time_property("YEAR");
}

fn get_month() -> num {
    return get_time_property("MONTH");
}

fn get_day() -> num {
    return get_time_property("DATE");
}

fn get_dayofweek() -> num {
    return get_time_property("DAYOFWEEK");
}

fn get_hour() -> num {
    return get_time_property("HOUR");
}

fn get_minute() -> num {
    return get_time_property("MINUTE");
}

fn get_second() -> num {
    return get_time_property("SECOND");
}

fn time() -> num [stdcall] {
    return __scratch_stdcall_hook_return("sensing_dayssince2000");
}

fn get_username() -> num [stdcall] {
    return __scratch_stdcall_hook_return("sensing_username");
}

// Operators Category

fn random(num a, num b) -> num [stdcall] {
    return __scratch_stdcall_hook_return("operator_random", "FROM", a, "TO", b);
}

fn round(num n) -> num [stdcall] {
    return __scratch_stdcall_hook_return("operator_round", "NUM", n);
}

fn apply_mathop(str operator, num n) -> num [stdcall] {
    return __scratch_stdcall_hook_return("operator_mathop", "FIELD_BEGIN", "abs", "floor", "ceiling", "sqrt", "sin", "cos", "tan", "asin", "acos", "atan", "ln", "log", "e ^", "10 ^", "FIELD_END", "fields", "OPERATOR", operator, "NUM", n);
}

fn abs(num n) -> num {
    return apply_mathop("abs", n);
}

fn floor(num n) -> num {
    return apply_mathop("floor", n);
}

fn ceil(num n) -> num {
    return apply_mathop("ceiling", n);
}

fn sqrt(num n) -> num {
    return apply_mathop("sqrt", n);
}

fn sin(num n) -> num {
    return apply_mathop("sin", n);
}

fn cos(num n) -> num {
    return apply_mathop("cos", n);
}

fn tan(num n) -> num {
    return apply_mathop("tan", n);
}

fn asin(num n) -> num {
    return apply_mathop("asin", n);
}

fn acos(num n) -> num {
    return apply_mathop("acos", n);
}

fn atan(num n) -> num {
    return apply_mathop("atan", n);
}

fn ln(num n) -> num {
    return apply_mathop("ln", n);
}

fn log(num n) -> num {
    return apply_mathop("log", n);
}

fn epow(num n) -> num {
    return apply_mathop("e ^", n);
}

fn tenpow(num n) -> num {
    return apply_mathop("10 ^", n);
}

fn pow(num a, num b) -> num {
    return epow(b * ln(a));
}
```
