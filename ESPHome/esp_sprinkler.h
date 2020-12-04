#pragma once

namespace esp_sprinkler {
  /// Enum for display modes/screens/pages
  enum DisplayPageEnum : uint8_t {
    MAIN_SCREEN = 0,
    SETUP_SCREEN = 1
  };

  typedef struct displaymode_t {
    DisplayPage* page;
    uint8_t numItems;
  } displaymode_t;

  std::vector<displaymode_t> display_pages = {
    {main_screen, 0},
    {setup_screen, 9}
  };

  std::vector<template_::TemplateSwitch*> zone_enable_switches = {
    esp_sprinkler_controller_zone_1_enable,
    esp_sprinkler_controller_zone_2_enable,
    esp_sprinkler_controller_zone_3_enable,
    esp_sprinkler_controller_zone_4_enable,
    esp_sprinkler_controller_zone_5_enable,
  };

  std::vector<template_::TemplateSwitch*> zone_switches = {
    esp_sprinkler_controller_zone_1,
    esp_sprinkler_controller_zone_2,
    esp_sprinkler_controller_zone_3,
    esp_sprinkler_controller_zone_4,
    esp_sprinkler_controller_zone_5,
  };

  std::vector<uint16_t> zone_run_times = {0, 0, 0, 0, 0};

  // Color color_mode(0, 0, 0, 0.5);
  // Color color_action(0, 0, 0, 1);
  // Color color_footer(0, 0, 0, 0.2);
  // Color color_time(0, 0, 0, 0.08);
  // Color color_highlight(0, 0, 0, 0.9);
  // Color color_lowlight(0, 0, 0, 0.08);

  const uint8_t last_page_number = display_pages.size();
  const uint8_t last_zone_number = zone_switches.size();
    
  void refresh_display_pages() {
    display_pages[MAIN_SCREEN].page = main_screen;
    display_pages[SETUP_SCREEN].page = setup_screen;
  }

  void refresh_zone_enable_switches() {
    zone_enable_switches[0] = esp_sprinkler_controller_zone_1_enable;
    zone_enable_switches[1] = esp_sprinkler_controller_zone_2_enable;
    zone_enable_switches[2] = esp_sprinkler_controller_zone_3_enable;
    zone_enable_switches[3] = esp_sprinkler_controller_zone_4_enable;
    zone_enable_switches[4] = esp_sprinkler_controller_zone_5_enable;
  }

  void refresh_zone_switches() {
    zone_switches[0] = esp_sprinkler_controller_zone_1;
    zone_switches[1] = esp_sprinkler_controller_zone_2;
    zone_switches[2] = esp_sprinkler_controller_zone_3;
    zone_switches[3] = esp_sprinkler_controller_zone_4;
    zone_switches[4] = esp_sprinkler_controller_zone_5;
  }

  void refresh_zone_run_times() {
    zone_run_times[0] = id(zone_1_run_time);
    zone_run_times[1] = id(zone_2_run_time);
    zone_run_times[2] = id(zone_3_run_time);
    zone_run_times[3] = id(zone_4_run_time);
    zone_run_times[4] = id(zone_5_run_time);
  }

  void set_display_page(uint8_t page) {
    if (page < esp_sprinkler::last_page_number) {
      id(selected_display_page) = page;
      // if any page in the array is set to nullptr, refresh them to get valid pointers
      if (display_pages[id(selected_display_page)].page == nullptr)
        refresh_display_pages();
      // finally, set/show the new page
      id(main_lcd).show_page(display_pages[id(selected_display_page)].page);
    }
  }

  void mode_button_click() {
  }

  void encoder_button_click() {
    if (id(main_lcd).is_on()) {
      if (!isnan(id(esp_sprinkler_controller_encoder).state))
        id(encoder_value) = id(esp_sprinkler_controller_encoder).state;

      switch (id(selected_display_page)) {
        case MAIN_SCREEN:
          esp_sprinkler::set_display_page(esp_sprinkler::DisplayPageEnum::SETUP_SCREEN);
          break;
        case SETUP_SCREEN:
          switch(id(selected_display_item)) {
            case 0:
              id(esp_sprinkler_controller_zone_1_enable).toggle();
              break;
            case 1:
              id(esp_sprinkler_controller_zone_2_enable).toggle();
              break;
            case 2:
              id(esp_sprinkler_controller_zone_3_enable).toggle();
              break;
            case 3:
              id(esp_sprinkler_controller_zone_4_enable).toggle();
              break;
            case 4:
              id(esp_sprinkler_controller_zone_5_enable).toggle();
              break;
            case 5:
              id(esp_sprinkler_controller_auto_advance).toggle();
              break;
            case 6:
              id(esp_sprinkler_controller_master).turn_on();
              break;
            case 7:
              // enable schedule
              break;
            default:
              esp_sprinkler::set_display_page(esp_sprinkler::DisplayPageEnum::MAIN_SCREEN);
              break;
          }
          break;        
        default:
          break;
      }
    }
  }

  void encoder_value_change() {
    int8_t encoder_offset = 0;

    // we only need to do stuff if we are on a page that would allow something to be changed
    if (id(selected_display_page) == SETUP_SCREEN) {
      if (!isnan(id(esp_sprinkler_controller_encoder).state)) {
          encoder_offset = id(esp_sprinkler_controller_encoder).state - id(encoder_value);
          id(encoder_value) = id(esp_sprinkler_controller_encoder).state;
      }

      id(selected_display_item) += encoder_offset;
      if (id(selected_display_item) >= display_pages[id(selected_display_page)].numItems)
        id(selected_display_item) = display_pages[id(selected_display_page)].numItems - 1;
      if (id(selected_display_item) < 0)
        id(selected_display_item) = 0;
    }
  }

  bool there_is_an_active_zone() {
    return ((id(active_zone) >= 0) && (id(active_zone) < last_zone_number));
  }

  bool is_a_valid_zone(const int8_t zone) {
    return ((zone >= 0) && (zone < last_zone_number));
  }

  template_::TemplateSwitch* zone_switch(const uint8_t zone) {
    if (is_a_valid_zone(zone)) {
      if (zone_switches[zone] == nullptr) {
        refresh_zone_switches();
      }
      return zone_switches[zone];
    }
    return nullptr;
  }

  template_::TemplateSwitch* zone_enable_switch(const uint8_t zone) {
    if (is_a_valid_zone(zone)) {
      if (zone_enable_switches[zone] == nullptr) {
        refresh_zone_enable_switches();
      }
      return zone_enable_switches[zone];
    }
    return nullptr;
  }

  template_::TemplateSwitch* active_zone_switch() {
    if (there_is_an_active_zone()) {
      return zone_switch(id(active_zone));
    }
    return nullptr;
  }

  template_::TemplateSwitch* active_zone_enable_switch() {
    if (there_is_an_active_zone()) {
      return zone_enable_switch(id(active_zone));
    }
    return nullptr;
  }

  int8_t next_zone(const int8_t first_zone) {
    if (is_a_valid_zone(first_zone) || (first_zone == -1)) {
      for (int8_t zone = first_zone + 1; zone < last_zone_number; zone++) {
        if (zone_enable_switch(zone)->state) {
          return zone;
        }
      }
      for (int8_t zone = 0; zone < first_zone; zone++) {
        if (zone_enable_switch(zone)->state) {
          return zone;
        }
      }
    }
    return -1;
  }

  template_::TemplateSwitch* next_zone_switch() {
    if (there_is_an_active_zone() || (id(active_zone) == -1)) {
      if (next_zone(id(active_zone)) >= 0) {
        return zone_switch(next_zone(id(active_zone)));
      }
    }
    return nullptr;
  }

  template_::TemplateSwitch* next_zone_enable_switch() {
    if (there_is_an_active_zone() || (id(active_zone) == -1)) {
      if (next_zone(id(active_zone)) >= 0) {
        return zone_enable_switch(next_zone(id(active_zone)));
      }
    }
    return nullptr;
  }

  bool any_zone_is_enabled() {
    for (uint8_t zone = 0; zone < last_zone_number; zone++) {
      if (zone_enable_switch(zone)->state == true)
        return true;
    }
    return false;
  }

  uint16_t zone_run_time(const uint8_t zone) {
    if (is_a_valid_zone(zone)) {
      refresh_zone_run_times();
      return zone_run_times[zone];
    }
    return 0;
  }

  void run_time_count_down() {
    if (there_is_an_active_zone()) {
      id(seconds_remaining) -= 1;
      if (id(seconds_remaining) < 0) {
        if (esp_sprinkler_controller_auto_advance->state == true) {
          active_zone_enable_switch()->turn_off();
        }
        if (next_zone(id(active_zone)) >= 0) {
          next_zone_switch()->turn_on();
        } else {
          esp_sprinkler_controller_master->turn_off();
        }
      }
    }
  }

  void start_master() {
    if (!there_is_an_active_zone()) {
      esp_sprinkler_controller_auto_advance->turn_on();
      // if no zones are enabled, enable them all so that auto-advance can work
      if (!any_zone_is_enabled()) {
        for (uint8_t zone = 0; zone < last_zone_number; zone++) {
          zone_enable_switch(zone)->turn_on();
        }
      }
      next_zone_switch()->turn_on();
    }
  }

  void stop_master() {
    if (there_is_an_active_zone()) {
      id(active_zone) = -1;
      id(seconds_remaining) = -1;
    }
  }

  void start_zone(const uint8_t zone) {
    if (is_a_valid_zone(zone)) {
      id(active_zone) = zone;
      // zone run times are set in minutes -- we multiply by 60 for the time in seconds
      id(seconds_remaining) = zone_run_time(zone) * id(zone_run_time_multiplier) * 60;
    }
  }

  void stop_zone(const uint8_t zone) {
    if (is_a_valid_zone(zone)) {
      if (zone == id(active_zone)) {
        id(active_zone) = -1;
        id(seconds_remaining) = -1;
      }
    }
  }

  void draw_footer(DisplayBuffer* it, const bool include_climate = false) {
    it->strftime(it->get_width(), it->get_height(), controller_tiny, color_time, TextAlign::BASELINE_RIGHT, "%I:%M:%S %p", id(esptime).now());
    if (id(esp_sprinkler_controller_api_status).state) {
      it->strftime(0, it->get_height(), controller_tiny, color_time, TextAlign::BASELINE_LEFT, "%m-%d-%Y", id(esptime).now());
    } else {
      it->print(0, it->get_height(), controller_tiny, color_highlight, TextAlign::BASELINE_LEFT, "Offline");
    }

    if (include_climate) {
      if (!isnan(id(esp_sprinkler_controller_bme280_temperature).state)) {
        it->printf(0, it->get_height() - 8, controller_tiny, color_footer, TextAlign::BASELINE_LEFT, "Local: %.1f°", id(esp_sprinkler_controller_bme280_temperature).state * 1.8 + 32);
      }
      if (!isnan(id(esp_sprinkler_controller_bme280_humidity).state)) {
        it->printf(it->get_width() - 1, it->get_height() - 8, controller_tiny, color_footer, TextAlign::BASELINE_RIGHT, "%.1f%% RH", id(esp_sprinkler_controller_bme280_humidity).state);
      }
    }
  }

  void draw_main_screen(DisplayBuffer* it) {
    const uint8_t h_remaining = id(seconds_remaining) / (60 * 60);
    const uint8_t m_remaining = (id(seconds_remaining) - (h_remaining * 60 * 60)) / 60;
    const uint8_t s_remaining = id(seconds_remaining) - ((h_remaining * 60 * 60) + (m_remaining * 60));

    esp_sprinkler::draw_footer(it, true);

    if (id(seconds_remaining) >= 0) {
      it->printf(0, 0, controller_small, color_lowlight, TextAlign::TOP_LEFT, "Time remaining: %d:%02d:%02d", h_remaining, m_remaining, s_remaining);
    } else {
      it->printf(0, 0, controller_small, color_lowlight, TextAlign::TOP_LEFT, "No scheduled runs.");
    }

    if (id(esp_sprinkler_controller_zone_1).state == true) {
      it->printf((it->get_width() / 2), (it->get_height() / 2), controller_large, color_action, TextAlign::CENTER, "Parkway");
    } else if (id(esp_sprinkler_controller_zone_2).state == true) {
      it->printf((it->get_width() / 2), (it->get_height() / 2), controller_large, color_action, TextAlign::CENTER, "Front yard");
    } else if (id(esp_sprinkler_controller_zone_3).state == true) {
      it->printf((it->get_width() / 2), (it->get_height() / 2), controller_large, color_action, TextAlign::CENTER, "East side");
    } else if (id(esp_sprinkler_controller_zone_4).state == true) {
      it->printf((it->get_width() / 2), (it->get_height() / 2), controller_large, color_action, TextAlign::CENTER, "Back yard");
    } else if (id(esp_sprinkler_controller_zone_5).state == true) {
      it->printf((it->get_width() / 2), (it->get_height() / 2), controller_large, color_action, TextAlign::CENTER, "West side");
    } else {
      it->printf((it->get_width() / 2), (it->get_height() / 2), controller_large, color_lowlight, TextAlign::CENTER, "Off");
    }
  }

  void draw_setup_screen(DisplayBuffer* it) {
    const int line_height = 10;
    auto zone_1_highlight = color_lowlight,
          zone_2_highlight = color_lowlight,
          zone_3_highlight = color_lowlight,
          zone_4_highlight = color_lowlight,
          zone_5_highlight = color_lowlight,
          auto_highlight = color_lowlight,
          start_highlight = color_lowlight,
          schedule_highlight = color_lowlight,
          back_highlight = color_lowlight;

    switch(id(selected_display_item)) {
      case 0:
        zone_1_highlight = color_highlight;
        break;
      case 1:
        zone_2_highlight = color_highlight;
        break;
      case 2:
        zone_3_highlight = color_highlight;
        break;
      case 3:
        zone_4_highlight = color_highlight;
        break;
      case 4:
        zone_5_highlight = color_highlight;
        break;
      case 5:
        auto_highlight = color_highlight;
        break;
      case 6:
        start_highlight = color_highlight;
        break;
      case 7:
        schedule_highlight = color_highlight;
        break;
      case 8:
        back_highlight = color_highlight;
        break;
      default:
        break;
    }

    it->printf(0, line_height * 0, controller_small, color_mode, TextAlign::TOP_LEFT, "Select zones to run:");
    it->printf(7, line_height * 1, controller_small, zone_1_highlight, TextAlign::TOP_LEFT, "Parkway");
    it->printf(7, line_height * 2, controller_small, zone_2_highlight, TextAlign::TOP_LEFT, "Front yard");
    it->printf(7, line_height * 3, controller_small, zone_3_highlight, TextAlign::TOP_LEFT, "East side");
    it->printf(7, line_height * 4, controller_small, zone_4_highlight, TextAlign::TOP_LEFT, "Back yard");
    it->printf(7, line_height * 5, controller_small, zone_5_highlight, TextAlign::TOP_LEFT, "West side");
    it->printf(it->get_width(), line_height * 1, controller_small, auto_highlight, TextAlign::TOP_RIGHT, "Auto-Adv");
    it->printf(it->get_width(), line_height * 2, controller_small, start_highlight, TextAlign::TOP_RIGHT, "Start");
    it->printf(it->get_width(), line_height * 3, controller_small, schedule_highlight, TextAlign::TOP_RIGHT, "Schedule");
    it->printf(it->get_width(), line_height * 4, controller_small, back_highlight, TextAlign::TOP_RIGHT, "Back");
    
    if (id(esp_sprinkler_controller_zone_1_enable).state == true)
      it->printf(1, line_height * 1 - 1, controller_small, zone_1_highlight, TextAlign::TOP_LEFT, "+");
    else
      it->printf(2, line_height * 1 - 0, controller_small, zone_1_highlight, TextAlign::TOP_LEFT, "-");
    if (id(esp_sprinkler_controller_zone_2_enable).state == true)
      it->printf(1, line_height * 2 - 1, controller_small, zone_2_highlight, TextAlign::TOP_LEFT, "+");
    else
      it->printf(2, line_height * 2 - 0, controller_small, zone_2_highlight, TextAlign::TOP_LEFT, "-");
    if (id(esp_sprinkler_controller_zone_3_enable).state == true)
      it->printf(1, line_height * 3 - 1, controller_small, zone_3_highlight, TextAlign::TOP_LEFT, "+");
    else
      it->printf(2, line_height * 3 - 0, controller_small, zone_3_highlight, TextAlign::TOP_LEFT, "-");
    if (id(esp_sprinkler_controller_zone_4_enable).state == true)
      it->printf(1, line_height * 4 - 1, controller_small, zone_4_highlight, TextAlign::TOP_LEFT, "+");
    else
      it->printf(2, line_height * 4 - 0, controller_small, zone_4_highlight, TextAlign::TOP_LEFT, "-");
    if (id(esp_sprinkler_controller_zone_5_enable).state == true)
      it->printf(1, line_height * 5 - 1, controller_small, zone_5_highlight, TextAlign::TOP_LEFT, "+");
    else
      it->printf(2, line_height * 5 - 0, controller_small, zone_5_highlight, TextAlign::TOP_LEFT, "-");
    if (id(esp_sprinkler_controller_auto_advance).state == true)
      it->printf(75, line_height * 1 - 1, controller_small, auto_highlight, TextAlign::TOP_LEFT, "+");
    else
      it->printf(76, line_height * 1 - 0, controller_small, auto_highlight, TextAlign::TOP_LEFT, "-");
  }
}