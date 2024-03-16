/*
Copyright 2019 @foostan
Copyright 2020 Drashna Jaelre <@drashna>

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 2 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include QMK_KEYBOARD_H
#include <stdio.h>

enum custom_keycodes {
  SELECT_LINE = SAFE_RANGE,
  SELECT_WORD,
  SELECT_ALL,
  BROWSER_BACK,
  BROWSER_FORWARD,
  BROWSER_NEXT_TAB,
  BROWSER_PREVIOUS_TAB,
  BROWSER_CLOSE_TAB,
  BROWSER_BOOKMARKS
};

#if defined(POINTING_DEVICE_ENABLE) || defined(PS2_MOUSE_ENABLE)
#define AUTO_TYPING_DELAY 600
#define AUTO_MIN_TIME 200
#define AUTO_DEBOUNCE 200
#define AUTO_ACTIVE_DURATION 6000

#define RIGHT_CLICK KC_U
#define LEFT_CLICK KC_Y

static uint16_t auto_debounce_timer = 0;
static uint16_t auto_typing_timer = 0;
static uint16_t auto_active_timer = 0;
static uint16_t auto_min_timer = 0;
static uint16_t auto_is_active = false;
static uint16_t auto_is_pending = false;

void auto_set_active(void)
{
    auto_active_timer = timer_read();
    auto_is_active = true;
    auto_is_pending = false;
}

void auto_extend_active(void)
{
    auto_active_timer = timer_read();
}

void auto_set_inactive(void)
{
    auto_debounce_timer = timer_read();
    auto_active_timer = 0;
    auto_is_active = false;
    auto_is_pending = false;
}

void auto_handle_non_mouse_key_press(void)
{
    auto_set_inactive();
    auto_typing_timer = timer_read();
}

void auto_handle_mouse_event(void)
{
    if(timer_elapsed(auto_debounce_timer) > AUTO_DEBOUNCE)
    {
        if(!auto_is_active)
        {
            if(timer_elapsed(auto_debounce_timer) > AUTO_DEBOUNCE && timer_elapsed(auto_typing_timer) > AUTO_TYPING_DELAY)
            {
                if(!auto_is_pending)
                {
                    auto_is_pending = true;
                    auto_min_timer = timer_read();
                }
                else if(timer_elapsed(auto_min_timer) > AUTO_MIN_TIME)
                {
                    auto_set_active();
                }
            }
        }
        else
        {
            auto_extend_active();
        }
        auto_debounce_timer = timer_read();
    }
}

void auto_handle_scroll_event(void)
{
    if(timer_elapsed(auto_debounce_timer) > AUTO_DEBOUNCE)
    {
        if(auto_is_active)
        {
            auto_extend_active();
        }
        auto_debounce_timer = timer_read();
    }
}

bool auto_get_is_active(void)
{
    if(timer_elapsed(auto_active_timer) > AUTO_ACTIVE_DURATION)
    {
        auto_set_inactive();
    }
    return auto_is_active;
}
#endif //defined(POINTING_DEVICE_ENABLE) || defined(PS2_MOUSE_ENABLE)

bool process_record_user(uint16_t keycode, keyrecord_t *record) {
    bool result = true;
    switch (keycode) {
        case SELECT_LINE:
            if (record->event.pressed) {
                SEND_STRING(SS_TAP(X_HOME) SS_LSFT(SS_TAP(X_END)));
            } else {
                // when keycode SELECT_LINE is released
            }
            break;
        case SELECT_WORD:
            if (record->event.pressed) {
                SEND_STRING(SS_LCTL(SS_TAP(X_RGHT) SS_LSFT(SS_TAP(X_LEFT))));
            } else {
                // when keycode SELECT_LINE is released
            }
            break;
        case SELECT_ALL:
            if (record->event.pressed) {
                SEND_STRING(SS_LCTL(SS_TAP(X_HOME) SS_LSFT(SS_TAP(X_END))));
            } else {
            }
            break;
        case BROWSER_BACK:
            if (record->event.pressed) {
                SEND_STRING(SS_LALT(SS_TAP(X_LEFT)));
            } else {
            }
            break;
        case BROWSER_FORWARD:
            if (record->event.pressed) {
                SEND_STRING(SS_LALT(SS_TAP(X_RIGHT)));
            } else {
            }
            break;
        case BROWSER_NEXT_TAB:
            if (record->event.pressed) {
                SEND_STRING(SS_LSFT(SS_LCTL(SS_TAP(X_TAB))));
            } else {
            }
            break;
        case BROWSER_PREVIOUS_TAB:
            if (record->event.pressed) {
                SEND_STRING(SS_LCTL(SS_TAP(X_TAB)));
            } else {
            }
            break;
        case BROWSER_CLOSE_TAB:
            if (record->event.pressed) {
                SEND_STRING(SS_LCTL(SS_TAP(X_W)));
            } else {
            }
            break;
        case BROWSER_BOOKMARKS:
            if (record->event.pressed) {
                SEND_STRING(SS_LCTL(SS_TAP(X_B))SS_DELAY(100)SS_TAP(X_TAB));
            } else {
            }
            break;
#if defined(POINTING_DEVICE_ENABLE) || defined(PS2_MOUSE_ENABLE)
        case RIGHT_CLICK:
        case LEFT_CLICK:
            if(auto_get_is_active())
            {
                uint16_t toPress = KC_MS_BTN1;
                if(keycode == RIGHT_CLICK)
                {
                    toPress = KC_MS_BTN2;
                }
                if (!record->event.pressed) {
                    unregister_code(toPress);
                }
                else
                {
                    register_code(toPress);
                }
                auto_handle_mouse_event();
                result = false;
            }
            break;
#endif
        default:
            break;
    }

#if defined(POINTING_DEVICE_ENABLE) || defined(PS2_MOUSE_ENABLE)
    if(record->event.pressed && result && !IS_MOUSEKEY(keycode) && keycode != LT(5, KC_SPACE))
    {
        auto_handle_non_mouse_key_press();
    }
#endif

    return result;
};

const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
  [0] = LAYOUT_split_3x6_3(
  //,-----------------------------------------------------.                    ,-----------------------------------------------------.
       KC_TAB,    KC_Q,    KC_W,    KC_E,    KC_R,    KC_T,                         KC_Y,    KC_U,    KC_I,    KC_O,   KC_P,  KC_BSPC,
  //|--------+--------+--------+--------+--------+--------|                    |--------+--------+--------+--------+--------+--------|
      KC_LCTL,LT(2,KC_A),LALT_T(KC_S),LSFT_T(KC_D),LCTL_T(KC_F),LGUI_T(KC_G),   LGUI_T(KC_H),LCTL_T(KC_J),LSFT_T(KC_K),LALT_T(KC_L), LT(1,KC_SCLN), KC_QUOT,
  //|--------+--------+--------+--------+--------+--------|                    |--------+--------+--------+--------+--------+--------|
      KC_LSFT,    KC_Z,    RALT_T(KC_X),    KC_C,    KC_V,    KC_B,                         KC_N,    KC_M, KC_COMM,  RALT_T(KC_DOT), KC_SLSH,  KC_ESC,
  //|--------+--------+--------+--------+--------+--------+--------|  |--------+--------+--------+--------+--------+--------+--------|
                            LT(8, KC_NO),   LT(3, KC_ESCAPE),  LCAG_T(KC_BACKSPACE),     LT(5, KC_SPACE),   LT(4,KC_ENT), LT(7, KC_NO)
                                      //`--------------------------'  `--------------------------'

  ),

  [1] = LAYOUT_split_3x6_3(
  //,-----------------------------------------------------.                    ,-----------------------------------------------------.
       KC_TAB,    KC_6,    KC_7,    KC_8,    KC_9,    KC_0,                     KC_TRNS,KC_MS_BTN1,KC_TRNS,KC_TRNS,KC_TRNS,KC_TRNS,
  //|--------+--------+--------+--------+--------+--------|                    |--------+--------+--------+--------+--------+--------|
      KC_LCTL,    KC_1,    KC_2,    KC_3,    KC_4,    KC_5,                     KC_TRNS,KC_TRNS,KC_TRNS,KC_TRNS,KC_TRNS,KC_TRNS,
  //|--------+--------+--------+--------+--------+--------|                    |--------+--------+--------+--------+--------+--------|
KC_LBRC,KC_LBRC,KC_RBRC,LSFT(KC_LBRC),LSFT(KC_RBRC), KC_GRV,                    KC_TRNS,KC_TRNS,KC_TRNS,KC_TRNS,KC_TRNS,KC_TRNS,
  //|--------+--------+--------+--------+--------+--------+--------|  |--------+--------+--------+--------+--------+--------+--------|
                                      KC_NO, KC_QUOT, LSFT(KC_QUOT),  KC_MS_BTN1, KC_TRNS, KC_TRNS
                                      //`--------------------------'  `--------------------------'
  ),

  [2] = LAYOUT_split_3x6_3(
  //,-----------------------------------------------------.                    ,-----------------------------------------------------.
       KC_TRNS,KC_TRNS,KC_TRNS,KC_TRNS,KC_TRNS,KC_TRNS,                LSFT(KC_BSLS),LSFT(KC_6),LSFT(KC_7),LSFT(KC_8),LSFT(KC_5),LSFT(KC_5),
  //|--------+--------+--------+--------+--------+--------|                    |--------+--------+--------+--------+--------+--------|
      KC_TRNS,KC_TRNS,KC_TRNS,KC_TRNS,KC_TRNS,KC_TRNS,            KC_BSLS,LSFT(KC_1),LSFT(KC_2),LSFT(KC_3),LSFT(KC_4),LSFT(KC_5),
  //|--------+--------+--------+--------+--------+--------|                    |--------+--------+--------+--------+--------+--------|
      KC_TRNS,KC_TRNS,KC_TRNS,KC_TRNS,KC_TRNS,KC_TRNS,             LSFT(KC_MINS),KC_MINS,LSFT(KC_EQL),KC_EQL,LSFT(KC_GRV),KC_TRNS,
  //|--------+--------+--------+--------+--------+--------+--------|  |--------+--------+--------+--------+--------+--------+--------|
                                          KC_TRNS,   KC_TRNS,  KC_TRNS,  LSFT(KC_9), LSFT(KC_0), KC_TRNS
                                      //`--------------------------'  `--------------------------'
  ),

  [3] = LAYOUT_split_3x6_3(
  //,-----------------------------------------------------.                    ,-----------------------------------------------------.
        KC_TRNS,KC_TRNS,KC_TRNS,KC_TRNS,KC_TRNS,KC_TRNS,                      KC_TRNS, KC_TRNS, KC_TRNS, QK_CAPS_WORD_TOGGLE, KC_PRINT_SCREEN, XXXXXXX,
  //|--------+--------+--------+--------+--------+--------|                    |--------+--------+--------+--------+--------+--------|
      KC_TRNS,KC_TRNS,KC_TRNS,KC_TRNS,KC_TRNS,KC_TRNS,                          KC_LEFT, KC_DOWN, KC_UP, KC_RIGHT, KC_PGUP, XXXXXXX,
  //|--------+--------+--------+--------+--------+--------|                    |--------+--------+--------+--------+--------+--------|
      KC_TRNS,KC_TRNS,KC_TRNS,KC_TRNS,KC_TRNS,KC_TRNS,                      KC_HOME, KC_END, KC_BRK, XXXXXXX, KC_PGDN, XXXXXXX,
  //|--------+--------+--------+--------+--------+--------+--------|  |--------+--------+--------+--------+--------+--------+--------|
                                          KC_NO, KC_TRNS,  KC_TRNS,     KC_TAB, KC_TRNS, KC_NO
                                      //`--------------------------'  `--------------------------'
  ),
  [4] = LAYOUT_split_3x6_3(
  //,-----------------------------------------------------.                    ,-----------------------------------------------------.
      KC_TRNS,   KC_F6,   KC_F7,   KC_F8,   KC_F9,  KC_F10,                      KC_AUDIO_MUTE,KC_AUDIO_VOL_DOWN,KC_AUDIO_VOL_UP,KC_TRNS,KC_TRNS,KC_TRNS,
  //|--------+--------+--------+--------+--------+--------|                    |--------+--------+--------+--------+--------+--------|
      KC_TRNS,   KC_F1,   KC_F2,   KC_F3,   KC_F4,   KC_F5,                     KC_TRNS,KC_TRNS,KC_TRNS,KC_TRNS,KC_TRNS,KC_TRNS,
  //|--------+--------+--------+--------+--------+--------|                    |--------+--------+--------+--------+--------+--------|
       KC_TRNS,   KC_F11,KC_F12,KC_TRNS,KC_LGUI,KC_TRNS,                         KC_TRNS,KC_TRNS,KC_TRNS,KC_TRNS,KC_TRNS,KC_TRNS,
  //|--------+--------+--------+--------+--------+--------+--------|  |--------+--------+--------+--------+--------+--------+--------|
                                          KC_NO, QK_CAPS_WORD_TOGGLE,  KC_DEL,     KC_TRNS, KC_TRNS, KC_NO
                                      //`--------------------------'  `--------------------------'
  ),
  [5] = LAYOUT_split_3x6_3(
  //,-----------------------------------------------------.                    ,-----------------------------------------------------.
     KC_TRNS,LCAG(KC_6),LCAG(KC_7),LCAG(KC_8),LCAG(KC_9),LCAG(KC_0),           KC_F5,BROWSER_NEXT_TAB,BROWSER_PREVIOUS_TAB,BROWSER_CLOSE_TAB,BROWSER_BOOKMARKS,KC_TRNS,
  //|--------+--------+--------+--------+--------+--------|                    |--------+--------+--------+--------+--------+--------|
     KC_TRNS,LCAG(KC_1),LCAG(KC_2),LCAG(KC_3),LCAG(KC_4),LCAG(KC_5),           BROWSER_BACK,KC_DOWN,LSFT_T(KC_UP),BROWSER_FORWARD,KC_TRNS,KC_TRNS,
  //|--------+--------+--------+--------+--------+--------|                    |--------+--------+--------+--------+--------+--------|
     KC_TRNS,LCAG(KC_Z),LCAG(KC_X),LCAG(KC_C),LCAG(KC_V),LCAG(KC_B),           KC_MS_BTN1,KC_MS_BTN1,KC_TRNS,KC_TRNS,KC_TRNS,KC_TRNS,
  //|--------+--------+--------+--------+--------+--------+--------|  |--------+--------+--------+--------+--------+--------+--------|
                                         KC_TRNS,KC_TRNS,KC_TRNS,              LCAG(KC_SPACE),KC_TRNS,KC_TRNS
                                      //`--------------------------'  `--------------------------'
  ),
  [6] = LAYOUT_split_3x6_3(
  //,-----------------------------------------------------.                    ,-----------------------------------------------------.
     KC_TRNS,KC_TRNS,KC_TRNS,KC_TRNS,KC_TRNS,KC_TRNS,                          LCAG(KC_Y),LCAG(KC_U),LCAG(KC_I),LCAG(KC_O),LCAG(KC_P),KC_TRNS,
  //|--------+--------+--------+--------+--------+--------|                    |--------+--------+--------+--------+--------+--------|
     KC_TRNS,KC_TRNS,KC_TRNS,KC_TRNS,KC_TRNS,KC_TRNS,                          LCAG(KC_H),LCAG(KC_J),LCAG(KC_K),LCAG(KC_L),KC_TRNS,KC_TRNS,
  //|--------+--------+--------+--------+--------+--------|                    |--------+--------+--------+--------+--------+--------|
     KC_TRNS,KC_TRNS,KC_TRNS,KC_TRNS,KC_TRNS,KC_TRNS,                          LCAG(KC_N),LCAG(KC_M),LCAG(KC_COMM),LCAG(KC_DOT),LCAG(KC_SLSH),KC_TRNS,
  //|--------+--------+--------+--------+--------+--------+--------|  |--------+--------+--------+--------+--------+--------+--------|
                                         KC_TRNS,KC_TRNS, KC_TRNS,             LCAG(KC_SPACE),LCAG(KC_ENT),KC_TRNS
                                      //`--------------------------'  `--------------------------'
  ),
  [7] = LAYOUT_split_3x6_3(
  //,-----------------------------------------------------.                    ,-----------------------------------------------------.
     KC_TRNS,LCAG(KC_F6),LCAG(KC_F7),LCAG(KC_F8),LCAG(KC_F9),LCAG(KC_F10),     KC_TRNS,KC_TRNS,KC_TRNS,KC_TRNS,KC_TRNS,KC_TRNS,
  //|--------+--------+--------+--------+--------+--------|                    |--------+--------+--------+--------+--------+--------|
     KC_TRNS,LCAG(KC_F1),LCAG(KC_F2),LCAG(KC_F3),LCAG(KC_F4),LCAG(KC_F5),      KC_TRNS,KC_TRNS,KC_TRNS,KC_TRNS,KC_TRNS,KC_TRNS,
  //|--------+--------+--------+--------+--------+--------|                    |--------+--------+--------+--------+--------+--------|
     KC_TRNS,SELECT_LINE,SELECT_WORD,SELECT_ALL,DB_TOGG,QK_BOOT,                          KC_TRNS,KC_TRNS,KC_TRNS,KC_TRNS,KC_TRNS,KC_TRNS,
  //|--------+--------+--------+--------+--------+--------+--------|  |--------+--------+--------+--------+--------+--------+--------|
                                         KC_TRNS,KC_TRNS,KC_TRNS,              KC_SPACE,KC_TRNS,KC_TRNS
                                      //`--------------------------'  `--------------------------'
  ),
  [8] = LAYOUT_split_3x6_3(
  //,-----------------------------------------------------.                    ,-----------------------------------------------------.
     KC_TRNS,KC_TRNS,KC_TRNS,KC_TRNS,KC_TRNS,KC_TRNS,                          LCAG(KC_F18),LCAG(KC_F19),LCAG(KC_F20),LCAG(21),LCAG(22),KC_TRNS,
  //|--------+--------+--------+--------+--------+--------|                    |--------+--------+--------+--------+--------+--------|
     KC_TRNS,KC_TRNS,KC_TRNS,KC_TRNS,KC_TRNS,KC_TRNS,                          LCAG(KC_F13),LCAG(KC_F14),LCAG(KC_F15),LCAG(KC_F16),LCAG(KC_F17),KC_TRNS,
  //|--------+--------+--------+--------+--------+--------|                    |--------+--------+--------+--------+--------+--------|
     KC_TRNS,KC_TRNS,KC_TRNS,KC_TRNS,KC_TRNS,KC_TRNS,                          KC_TRNS,KC_TRNS,KC_TRNS,KC_TRNS,KC_TRNS,KC_TRNS,
  //|--------+--------+--------+--------+--------+--------+--------|  |--------+--------+--------+--------+--------+--------+--------|
                                         KC_TRNS,KC_TRNS,KC_TRNS,              KC_SPACE,KC_TRNS,KC_TRNS
                                      //`--------------------------'  `--------------------------'
  ),
  [9] = LAYOUT_split_3x6_3(
  //,-----------------------------------------------------.                    ,-----------------------------------------------------.
     KC_TRNS,KC_TRNS,KC_TRNS,KC_TRNS,KC_TRNS,KC_TRNS,                          KC_MS_BTN1,KC_MS_BTN2,KC_TRNS,KC_TRNS,KC_TRNS,KC_TRNS,
  //|--------+--------+--------+--------+--------+--------|                    |--------+--------+--------+--------+--------+--------|
     KC_TRNS,KC_TRNS,KC_TRNS,KC_TRNS,KC_TRNS,KC_TRNS,                          KC_TRNS,KC_TRNS,KC_TRNS,KC_TRNS,KC_TRNS,KC_TRNS,
  //|--------+--------+--------+--------+--------+--------|                    |--------+--------+--------+--------+--------+--------|
     KC_TRNS,KC_TRNS,KC_TRNS,KC_TRNS,KC_TRNS,KC_TRNS,                          KC_TRNS,KC_TRNS,KC_TRNS,KC_TRNS,KC_TRNS,KC_TRNS,
  //|--------+--------+--------+--------+--------+--------+--------|  |--------+--------+--------+--------+--------+--------+--------|
                                         KC_TRNS,KC_TRNS,KC_TRNS,              KC_TRNS,KC_TRNS,KC_TRNS
                                      //`--------------------------'  `--------------------------'
  )
};

uint16_t get_tapping_term(uint16_t keycode, keyrecord_t *record) {
    switch (keycode) {
        case LSFT_T(KC_G):
        case LGUI_T(KC_H):
            return TAPPING_TERM + 200;
        case LSFT_T(KC_K):
        case LSFT_T(KC_D):
        case LT(2,KC_A):
        case LT(2,KC_SCLN):
            return 150;
        default:
            return 200;
    }
}

uint8_t current_layer = 0;
layer_state_t layer_state_set_user(layer_state_t state) {
    current_layer = get_highest_layer(state);
    return state;
}

enum mouse_layers
{
    NONE = 0,
    SCROLL = 5,
    SCROLL2 = 8,
    _VOLUME = 4,
};

#ifdef POINTING_DEVICE_ENABLE
#define SCROLL_DIVISOR_V 8.0

float scroll_accumulated_v = 0;

static uint16_t mouse_debounce_timer = 0;

float slow_movement_threshold = 8.0;
float mid_movement_threshold = 16.0;
float fast_movement_threshold = 25.0;

report_mouse_t process_mouse_report(report_mouse_t mouse_report)
{
    switch(current_layer)
    {
        case NONE:
            {
                float distance_moved = sqrt((mouse_report.x * mouse_report.x) + (mouse_report.y * mouse_report.y));

                if (distance_moved <= slow_movement_threshold)
                {
                    mouse_report.x = 0.4 * mouse_report.x;
                    mouse_report.y = 0.4 * mouse_report.y;
                }
                else if(distance_moved <= mid_movement_threshold)
                {
                    mouse_report.x = .8 * mouse_report.x;
                    mouse_report.y = .8 * mouse_report.y;
                }
                else
                {
                    mouse_report.x = 1.2 * mouse_report.x;
                    mouse_report.y = 1.2 * mouse_report.y;
                }

                if(mouse_report.buttons & 0x01)
                {
                    if(timer_elapsed(mouse_debounce_timer) > 500)
                    {
                        tap_code16(KC_AUDIO_MUTE);
                        mouse_debounce_timer = timer_read();
                    }
                    mouse_report.buttons = 0;
                }

                if(mouse_report.x != 0 || mouse_report.y != 0)
                {
                    auto_handle_mouse_event();
                }
            }
            break;
        case SCROLL:
        case SCROLL2:
            {
                scroll_accumulated_v += (float)mouse_report.y / SCROLL_DIVISOR_V;

                scroll_accumulated_v = -scroll_accumulated_v;

                mouse_report.h = 0;
                mouse_report.v = (int8_t)scroll_accumulated_v;

                scroll_accumulated_v -= (int8_t)scroll_accumulated_v;

                mouse_report.x = 0;
                mouse_report.y = 0;

                auto_handle_scroll_event();
            }
            break;
        case _VOLUME:
            if(mouse_report.y > 3)
            {
                tap_code16(KC_AUDIO_VOL_DOWN);
            }
            else if(mouse_report.y < -3)
            {
                tap_code16(KC_AUDIO_VOL_UP);
            }
            break;
        default:
            break;
    }

    if(current_layer != NONE)
    {
        mouse_report.x = mouse_report.y = 0;
    }

    return mouse_report;
}

report_mouse_t pointing_device_task_user(report_mouse_t mouse_report)
{
    return process_mouse_report(mouse_report);
}
#endif

#ifdef PS2_MOUSE_ENABLE
void ps2_mouse_moved_user(report_mouse_t *mouse_report)
{
#ifdef CONSOLE_ENABLE
    uprintf("PS2 report before: x:%u y:%u v:%u h:%u layer:%u\n", mouse_report->x, mouse_report->y, mouse_report->v, mouse_report->h, current_layer);
#endif
    switch(current_layer)
    {
        case NONE:
            if(mouse_report->x != 0 || mouse_report->y != 0)
            {
                auto_handle_mouse_event();
            }
            break;
        case SCROLL:
        case SCROLL2:
            if(mouse_report->y != 0)
            {
                mouse_report->v = -mouse_report->y * .4;
                auto_handle_scroll_event();
            }
            break;
        case _VOLUME:
            if(mouse_report->y > 3)
            {
                tap_code16(KC_AUDIO_VOL_DOWN);
            }
            else if(mouse_report->y < -3)
            {
                tap_code16(KC_AUDIO_VOL_UP);
            }
            break;
        default:
            break;
    }

    if(current_layer != NONE)
    {
        mouse_report->x = 0;
        mouse_report->y = 0;
    }
#ifdef CONSOLE_ENABLE
    uprintf("PS2 report after: x:%u y:%u v:%u h:%u\n", mouse_report->x, mouse_report->y, mouse_report->v, mouse_report->h);
#endif
}
#endif

#ifdef OLED_ENABLE
oled_rotation_t oled_init_user(oled_rotation_t rotation) {
  if (!is_keyboard_master()) {
    return OLED_ROTATION_180;  // flips the display 180 degrees if offhand
  }
  return rotation;
}

#define L_BASE 0
#define L_LOWER 2
#define L_RAISE 4
#define L_ADJUST 8

void oled_render_layer_state(void) {
    oled_write_P(PSTR("Layer: "), false);
    switch (layer_state) {
        case L_BASE:
            oled_write_ln_P(PSTR("Default"), false);
            break;
        case L_LOWER:
            oled_write_ln_P(PSTR("Lower"), false);
            break;
        case L_RAISE:
            oled_write_ln_P(PSTR("Raise"), false);
            break;
        case L_ADJUST:
        case L_ADJUST|L_LOWER:
        case L_ADJUST|L_RAISE:
        case L_ADJUST|L_LOWER|L_RAISE:
            oled_write_ln_P(PSTR("Adjust"), false);
            break;
    }
}


char keylog_str[24] = {};

const char code_to_name[60] = {
    ' ', ' ', ' ', ' ', 'a', 'b', 'c', 'd', 'e', 'f',
    'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o', 'p',
    'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z',
    '1', '2', '3', '4', '5', '6', '7', '8', '9', '0',
    'R', 'E', 'B', 'T', '_', '-', '=', '[', ']', '\\',
    '#', ';', '\'', '`', ',', '.', '/', ' ', ' ', ' '};

void set_keylog(uint16_t keycode, keyrecord_t *record) {
  char name = ' ';
    if ((keycode >= QK_MOD_TAP && keycode <= QK_MOD_TAP_MAX) ||
        (keycode >= QK_LAYER_TAP && keycode <= QK_LAYER_TAP_MAX)) { keycode = keycode & 0xFF; }
  if (keycode < 60) {
    name = code_to_name[keycode];
  }

  // update keylog
  snprintf(keylog_str, sizeof(keylog_str), "%dx%d, k%2d : %c",
           record->event.key.row, record->event.key.col,
           keycode, name);
}

void oled_render_keylog(void) {
    oled_write(keylog_str, false);
}

void render_bootmagic_status(bool status) {
    /* Show Ctrl-Gui Swap options */
    static const char PROGMEM logo[][2][3] = {
        {{0x97, 0x98, 0}, {0xb7, 0xb8, 0}},
        {{0x95, 0x96, 0}, {0xb5, 0xb6, 0}},
    };
    if (status) {
        oled_write_ln_P(logo[0][0], false);
        oled_write_ln_P(logo[0][1], false);
    } else {
        oled_write_ln_P(logo[1][0], false);
        oled_write_ln_P(logo[1][1], false);
    }
}

void oled_render_logo(void) {
    static const char PROGMEM crkbd_logo[] = {
        0x80, 0x81, 0x82, 0x83, 0x84, 0x85, 0x86, 0x87, 0x88, 0x89, 0x8a, 0x8b, 0x8c, 0x8d, 0x8e, 0x8f, 0x90, 0x91, 0x92, 0x93, 0x94,
        0xa0, 0xa1, 0xa2, 0xa3, 0xa4, 0xa5, 0xa6, 0xa7, 0xa8, 0xa9, 0xaa, 0xab, 0xac, 0xad, 0xae, 0xaf, 0xb0, 0xb1, 0xb2, 0xb3, 0xb4,
        0xc0, 0xc1, 0xc2, 0xc3, 0xc4, 0xc5, 0xc6, 0xc7, 0xc8, 0xc9, 0xca, 0xcb, 0xcc, 0xcd, 0xce, 0xcf, 0xd0, 0xd1, 0xd2, 0xd3, 0xd4,
        0};
    oled_write_P(crkbd_logo, false);
}

bool oled_task_user(void) {
    if (is_keyboard_master()) {
        oled_render_layer_state();
        oled_render_keylog();
    } else {
        oled_render_logo();
    }
    return false;
}
#endif // OLED_ENABLE

/* void keyboard_post_init_user(void) { */
/*   // Customise these values to desired behaviour */
/*   debug_enable=true; */
/*   debug_matrix=true; */
/*   debug_keyboard=true; */
/*   debug_mouse=true; */
/* } */
