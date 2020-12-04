#pragma once

namespace esp32_thermostat {
  /// Enum for display modes/screens/pages
  enum DisplayPageEnum : uint8_t {
    MAIN_SCREEN = 0,
    TEMPS_SCREEN = 1,
    SETPOINT_SCREEN = 2,
    MODE_SCREEN = 3,
    SENSORS_SCREEN = 4
  };

  typedef struct displaymode_t {
    DisplayPage* page;
    uint8_t numItems;
  } displaymode_t;

  std::vector<displaymode_t> display_pages = {
    {main_screen, 0},
    {temps_screen, 0},
    {setpoint_screen, 2},
    {mode_screen, 2},
    {sensors_screen, 0}
  };

  const std::vector<climate::ClimateMode> supported_modes = {
      climate::CLIMATE_MODE_OFF,
      climate::CLIMATE_MODE_AUTO,
      climate::CLIMATE_MODE_COOL,
      climate::CLIMATE_MODE_HEAT,
      climate::CLIMATE_MODE_FAN_ONLY
  };
  const std::vector<climate::ClimateFanMode> supported_fan_modes = {
      climate::CLIMATE_FAN_ON,
      climate::CLIMATE_FAN_AUTO
  };
  const uint8_t num_modes = supported_modes.size();
  const uint8_t num_fan_modes = supported_fan_modes.size();
  const uint8_t last_page_number = display_pages.size();
  const float   lower_temp_boundary = (61 - 32) * 5 / 9;
  const float   upper_temp_boundary = (90 - 32) * 5 / 9;
  const float   second_stage_activation_delta = 2 * 5 / 9;
  const uint8_t max_missed_online_updates = 60 * (60 / 15);
  const uint8_t max_missed_offline_updates = 2 * (60 / 15);
  const uint8_t encoder_step_size = 1;

  void refresh_display_pages() {
    display_pages[MAIN_SCREEN].page = main_screen;
    display_pages[TEMPS_SCREEN].page = temps_screen;
    display_pages[SETPOINT_SCREEN].page = setpoint_screen;
    display_pages[MODE_SCREEN].page = mode_screen;
    display_pages[SENSORS_SCREEN].page = sensors_screen;
  }

  void set_display_page(uint8_t page) {
    if (page < esp32_thermostat::last_page_number) {
      id(selected_display_page) = page;
      // if any page in the array is set to nullptr, refresh them to get valid pointers
      if (display_pages[id(selected_display_page)].page == nullptr)
        refresh_display_pages();
      // finally, set/show the new page
      id(main_lcd).show_page(display_pages[id(selected_display_page)].page);
    }
  }

  void mode_button_click() {
    if (id(main_lcd).is_on()) {
      uint8_t mode_selected = 0;
      // set mode_selected to current thermostat climate mode
      for (uint8_t i = 0; i < num_modes; i++) {
          if (supported_modes[i] == id(esp_thermostat).mode)
            mode_selected = i;
      }
      // increment mode_selected, resetting it if it overflowed
      if (++mode_selected >= num_modes)
          mode_selected = 0;
      // set the new climate mode and refresh the thermostat to fire triggers
      id(esp_thermostat).mode = supported_modes[mode_selected];
      id(esp_thermostat).refresh();
    }
  }

  void encoder_button_click() {
    if (id(main_lcd).is_on()) {
      // "zero" the encoder, if it has a value
      if (!isnan(id(esp_thermostat_encoder).state))
        id(encoder_value) = id(esp_thermostat_encoder).state;
      // increment selected_display_item and, if it overflows, zero it and...
      if (++id(selected_display_item) >= display_pages[id(selected_display_page)].numItems) {
        id(selected_display_item) = 0;
        // ...increment selected_display_page and zero it if it overflows
        if (++id(selected_display_page) >= esp32_thermostat::last_page_number)
          id(selected_display_page) = 0;
      }
      set_display_page(id(selected_display_page));
    }
  }

  void encoder_value_change() {
    static const float temp_step_size = esp32_thermostat::encoder_step_size * (5.0 / 9.0);
    int encoder_offset = 0, mode_selected = 0, fan_mode_selected = 0;
    float high_set_point = id(esp_thermostat).target_temperature_high,
          low_set_point  = id(esp_thermostat).target_temperature_low;
    // we only need to do stuff if we are on a page that would allow something to be changed
    if ((id(selected_display_page) == SETPOINT_SCREEN) || (id(selected_display_page) == MODE_SCREEN)) {
      // set mode_selected to current thermostat climate mode
      for (uint8_t i = 0; i < num_modes; i++) {
          if (supported_modes[i] == id(esp_thermostat).mode)
            mode_selected = i;
      }
      // set fan_mode_selected to current thermostat fan mode
      for (uint8_t i = 0; i < num_fan_modes; i++) {
          if (supported_fan_modes[i] == id(esp_thermostat).fan_mode)
            fan_mode_selected = i;
      }
      // determine the encoder offset (how much whatever it is should move)
      if (!isnan(id(esp_thermostat_encoder).state)) {
          encoder_offset = id(esp_thermostat_encoder).state - id(encoder_value);
          id(encoder_value) = id(esp_thermostat_encoder).state;
      } else {
          encoder_offset = 0;
      }

      switch (id(selected_display_page)) {
        case SETPOINT_SCREEN:
          switch (id(selected_display_item)) {
            case 0:   // heat
              low_set_point += (encoder_offset * temp_step_size);
              if (low_set_point < esp32_thermostat::lower_temp_boundary)
                low_set_point = esp32_thermostat::lower_temp_boundary;
              else if (low_set_point > esp32_thermostat::upper_temp_boundary - temp_step_size)
                low_set_point = esp32_thermostat::upper_temp_boundary - temp_step_size;
              if (low_set_point >= high_set_point)
                high_set_point = low_set_point + temp_step_size;
              break;
            case 1:   // cool
              high_set_point += (encoder_offset * temp_step_size);
              if (high_set_point < esp32_thermostat::lower_temp_boundary +  temp_step_size)
                high_set_point = esp32_thermostat::lower_temp_boundary +  temp_step_size;
              else if (high_set_point > esp32_thermostat::upper_temp_boundary)
                high_set_point = esp32_thermostat::upper_temp_boundary;
              if (high_set_point <= low_set_point)
                low_set_point = high_set_point - temp_step_size;
              break;
            default:
              break;
          }
          break;
        case MODE_SCREEN:
          switch (id(selected_display_item)) {
            case 0:   // mode
              mode_selected += encoder_offset;
              if (mode_selected >= num_modes)
                  mode_selected = num_modes - 1;
              else if (mode_selected < 0)
                  mode_selected = 0;
              break;
            case 1:   // fan mode
              fan_mode_selected += encoder_offset;
              if (fan_mode_selected >= num_fan_modes)
                  fan_mode_selected = num_fan_modes - 1;
              else if (fan_mode_selected < 0)
                  fan_mode_selected = 0;
              break;
            default:
              break;
          }
          break;
        default:
          break;
      }

      id(esp_thermostat).target_temperature_low = low_set_point;
      id(esp_thermostat).target_temperature_high = high_set_point;
      id(esp_thermostat).mode = supported_modes[mode_selected];
      id(esp_thermostat).fan_mode = supported_fan_modes[fan_mode_selected];
      id(esp_thermostat).refresh();
    }
  }

float thermostat_sensor_update() {
    bool  template_sensor_valid = (id(current_temperature) >= esp32_thermostat::lower_temp_boundary) && (id(current_temperature) <= esp32_thermostat::upper_temp_boundary);
    float sensor_value = id(esp_thermostat_bme280_temperature).state;
    int   max_missed_updates = esp32_thermostat::max_missed_offline_updates;

    if (id(esp_thermostat_api_status).state)
      max_missed_updates = esp32_thermostat::max_missed_online_updates;

    id(missed_update_count) += 1;

    if (id(missed_update_count) > max_missed_updates)
      id(on_board_sensor_active) = true;
    else
      id(on_board_sensor_active) = false;

    id(esp_thermostat_thermistor_vcc).turn_on();
    id(adc_sensor_thermistor).update();
    id(esp_thermostat_thermistor_vcc).turn_off();

    if (id(esp_thermostat_cool_1).state
        && (id(esp_thermostat_temperature_sensor).state - id(esp_thermostat).target_temperature_high >= esp32_thermostat::second_stage_activation_delta))
      id(esp_thermostat_cool_2).turn_on();

    if (id(esp_thermostat_heat_1).state
        && (id(esp_thermostat).target_temperature_low - id(esp_thermostat_temperature_sensor).state >= esp32_thermostat::second_stage_activation_delta))
      id(esp_thermostat_heat_2).turn_on();

    if (id(on_board_sensor_active) || !template_sensor_valid) {
      if (!isnan(sensor_value)) {
        id(sensor_ready) = true;
        return sensor_value;
      } else {
        return (id(esp_thermostat).target_temperature_low + id(esp_thermostat).target_temperature_high) / 2;
      }
    } else {
      id(sensor_ready) = true;
      return id(current_temperature);
    }
  }

  void draw_footer(DisplayBuffer* it, bool include_climate = false) {
    it->strftime(it->get_width(), it->get_height(), thermostat_tiny, color_time, TextAlign::BASELINE_RIGHT, "%I:%M:%S %p", id(esptime).now());
    if (id(esp_thermostat_api_status).state) {
      it->strftime(0, it->get_height(), thermostat_tiny, color_time, TextAlign::BASELINE_LEFT, "%m-%d-%Y", id(esptime).now());
    } else {
      it->print(0, it->get_height(), thermostat_tiny, color_highlight, TextAlign::BASELINE_LEFT, "Offline");
    }

    if (include_climate) {
      if (id(on_board_sensor_active)) {
        it->printf(0, it->get_height() - 8, thermostat_tiny, color_mode, TextAlign::BASELINE_LEFT, "On-board sensor in use");
      } else {
        if (!isnan(id(esp_thermostat_bme280_temperature).state)) {
          it->printf(0, it->get_height() - 8, thermostat_tiny, color_footer, TextAlign::BASELINE_LEFT, "Local: %.1f°", id(esp_thermostat_bme280_temperature).state * 1.8 + 32);
        }
        if (!isnan(id(esp_thermostat_bme280_humidity).state)) {
          it->printf(it->get_width() - 1, it->get_height() - 8, thermostat_tiny, color_footer, TextAlign::BASELINE_RIGHT, "%.1f%% RH", id(esp_thermostat_bme280_humidity).state);
        }
      }
    }
  }

  void draw_main_screen(DisplayBuffer* it) {
    float high_set_point = id(esp_thermostat).target_temperature_high * 1.8 + 32,
          low_set_point  = id(esp_thermostat).target_temperature_low  * 1.8 + 32,
          current_temperature = id(esp_thermostat_temperature_sensor).state * 1.8 + 32,
          current_humidity = id(esp_thermostat_humidity_sensor).state;
    
    esp32_thermostat::draw_footer(it, true);

    if(id(sensor_ready) == false) {
      it->printf(0, 1, thermostat_small, color_time, TextAlign::TOP_LEFT, "Waiting for sensor data...");
    } else {
      switch (id(esp_thermostat).action) {
        case CLIMATE_ACTION_OFF:
        case CLIMATE_ACTION_IDLE:
          switch (id(esp_thermostat).mode) {
            case CLIMATE_MODE_OFF:
              it->printf(0, 0, thermostat_small, color_lowlight, TextAlign::TOP_LEFT, "System is off.");
              break;

            case CLIMATE_MODE_AUTO:
              it->printf(0, 0, thermostat_small, color_mode, TextAlign::TOP_LEFT, "Maintain %.0f° to %.0f°.", low_set_point, high_set_point);
              break;

            case CLIMATE_MODE_COOL:
              it->printf(0, 0, thermostat_small, color_mode, TextAlign::TOP_LEFT, "Cool to %.0f°.", high_set_point);
              break;

            case CLIMATE_MODE_HEAT:
              it->printf(0, 0, thermostat_small, color_mode, TextAlign::TOP_LEFT, "Heat to %.0f°.", low_set_point);
              break;

            case CLIMATE_MODE_FAN_ONLY:
              it->printf(0, 0, thermostat_small, color_mode, TextAlign::TOP_LEFT, "Fan only above %.0f°.", high_set_point);
              break;

            case CLIMATE_MODE_DRY:
              it->printf(0, 0, thermostat_small, color_mode, TextAlign::TOP_LEFT, "Dry only");
              break;
          }
          break;

        case CLIMATE_ACTION_COOLING:
          if (id(esp_thermostat_cool_2).state == true) {
            it->printf(0, 0, thermostat_small, color_action, TextAlign::TOP_LEFT, "Cooling to %.0f°, 2-stage.", high_set_point);
          } else {
            it->printf(0, 0, thermostat_small, color_action, TextAlign::TOP_LEFT, "Cooling to %.0f°.", high_set_point);
          }
          break;

        case CLIMATE_ACTION_HEATING:
          if (id(esp_thermostat_heat_2).state == true) {
            it->printf(0, 0, thermostat_small, color_action, TextAlign::TOP_LEFT, "Heating to %.0f°, 2-stage.", low_set_point);
          } else {
            it->printf(0, 0, thermostat_small, color_action, TextAlign::TOP_LEFT, "Heating to %.0f°.", low_set_point);
          }
          break;

        case CLIMATE_ACTION_DRYING:
          it->printf(0, 0, thermostat_small, color_action, TextAlign::TOP_LEFT, "Drying.");
          break;

        case CLIMATE_ACTION_FAN:
          it->printf(0, 0, thermostat_small, color_action, TextAlign::TOP_LEFT, "Faning to %.0f°.", high_set_point);
          break;
      }

      switch (id(esp_thermostat).fan_mode) {
        case CLIMATE_FAN_ON:
          it->printf(0, 9, thermostat_small, color_action, TextAlign::TOP_LEFT, "Fan on");
          break;
        case CLIMATE_FAN_OFF:
          break;
        case CLIMATE_FAN_AUTO:
          break;
        case CLIMATE_FAN_LOW:
          it->printf(0, 9, thermostat_small, color_action, TextAlign::TOP_LEFT, "Fan low");
          break;
        case CLIMATE_FAN_MEDIUM:
          it->printf(0, 9, thermostat_small, color_action, TextAlign::TOP_LEFT, "Fan medium");
          break;
        case CLIMATE_FAN_HIGH:
          it->printf(0, 9, thermostat_small, color_action, TextAlign::TOP_LEFT, "Fan high");
          break;
        case CLIMATE_FAN_MIDDLE:
          it->printf(0, 9, thermostat_small, color_action, TextAlign::TOP_LEFT, "Fan middle");
          break;
        case CLIMATE_FAN_FOCUS:
          it->printf(0, 9, thermostat_small, color_action, TextAlign::TOP_LEFT, "Fan focus");
          break;
        case CLIMATE_FAN_DIFFUSE:
          it->printf(0, 9, thermostat_small, color_action, TextAlign::TOP_LEFT, "Fan diffuse");
          break;
      }

      it->printf((it->get_width() / 2), (it->get_height() / 2), thermostat_large, color_temp, TextAlign::CENTER, "%.1f°", current_temperature);
    }
  }

  void draw_setpoint_screen(DisplayBuffer* it) {
    auto highlight_cool = color_lowlight, highlight_heat = color_lowlight;
    float high_set_point = id(esp_thermostat).target_temperature_high * 1.8 + 32,
          low_set_point  = id(esp_thermostat).target_temperature_low  * 1.8 + 32;

    esp32_thermostat::draw_footer(it, false);

    it->printf(0, 0, thermostat_small, color_mode, TextAlign::TOP_LEFT, "Adjust setpoints:");

    switch (id(selected_display_item)) {
      case 0:   // heat
        highlight_heat = color_highlight;
        break;
      case 1:   // cool
        highlight_cool = color_highlight;
        break;
      default:
        break;
    }

    it->printf(((it->get_width() / 3) * 1) - 10, 20, thermostat_small, highlight_heat, TextAlign::CENTER, "Heat");
    it->printf(((it->get_width() / 3) * 1) - 10, (it->get_height() / 2) + 5, thermostat_medium, highlight_heat, TextAlign::CENTER, "%.0f°", low_set_point);

    it->printf(((it->get_width() / 3) * 2) + 10, 20, thermostat_small, highlight_cool, TextAlign::CENTER, "Cool");
    it->printf(((it->get_width() / 3) * 2) + 10, (it->get_height() / 2) + 5, thermostat_medium, highlight_cool, TextAlign::CENTER, "%.0f°", high_set_point);
  }

  void draw_mode_screen(DisplayBuffer* it) {
    auto highlight_fan_mode = color_lowlight, highlight_mode = color_lowlight;

    esp32_thermostat::draw_footer(it, false);

    it->printf(0, 0, thermostat_small, color_mode, TextAlign::TOP_LEFT, "Adjust modes:");

    switch (id(selected_display_item)) {
      case 0:   // mode
        highlight_mode = color_highlight;
        break;
      case 1:   // fan mode
        highlight_fan_mode = color_highlight;
        break;
      default:
        break;
    }

    it->printf(((it->get_width() / 3) * 1) - 10, 20, thermostat_small, highlight_mode, TextAlign::CENTER, "Mode");
    switch (id(esp_thermostat).mode) {
      case climate::CLIMATE_MODE_OFF:
        it->printf(((it->get_width() / 3) * 1) - 10, (it->get_height() / 2) + 5, thermostat_medium, highlight_mode, TextAlign::CENTER, "Off");
        break;
      case climate::CLIMATE_MODE_AUTO:
        it->printf(((it->get_width() / 3) * 1) - 10, (it->get_height() / 2) + 5, thermostat_medium, highlight_mode, TextAlign::CENTER, "Auto");
        break;
      case climate::CLIMATE_MODE_COOL:
        it->printf(((it->get_width() / 3) * 1) - 10, (it->get_height() / 2) + 5, thermostat_medium, highlight_mode, TextAlign::CENTER, "Cool");
        break;
      case climate::CLIMATE_MODE_HEAT:
        it->printf(((it->get_width() / 3) * 1) - 10, (it->get_height() / 2) + 5, thermostat_medium, highlight_mode, TextAlign::CENTER, "Heat");
        break;
      case climate::CLIMATE_MODE_FAN_ONLY:
        it->printf(((it->get_width() / 3) * 1) - 10, (it->get_height() / 2) + 5, thermostat_medium, highlight_mode, TextAlign::CENTER, "Fan");
        break;
      case climate::CLIMATE_MODE_DRY:
        it->printf(((it->get_width() / 3) * 1) - 10, (it->get_height() / 2) + 5, thermostat_medium, highlight_mode, TextAlign::CENTER, "Dry");
        break;
    }

    it->printf(((it->get_width() / 3) * 2) + 10, 20, thermostat_small, highlight_fan_mode, TextAlign::CENTER, "Fan Mode");
    switch (id(esp_thermostat).fan_mode) {
      case climate::CLIMATE_FAN_ON:
        it->printf(((it->get_width() / 3) * 2) + 10, (it->get_height() / 2) + 5, thermostat_medium, highlight_fan_mode, TextAlign::CENTER, "On");
        break;
      case climate::CLIMATE_FAN_OFF:
        it->printf(((it->get_width() / 3) * 2) + 10, (it->get_height() / 2) + 5, thermostat_medium, highlight_fan_mode, TextAlign::CENTER, "Off");
        break;
      case climate::CLIMATE_FAN_AUTO:
        it->printf(((it->get_width() / 3) * 2) + 10, (it->get_height() / 2) + 5, thermostat_medium, highlight_fan_mode, TextAlign::CENTER, "Auto");
        break;
      case climate::CLIMATE_FAN_LOW:
        it->printf(((it->get_width() / 3) * 2) + 10, (it->get_height() / 2) + 5, thermostat_medium, highlight_fan_mode, TextAlign::CENTER, "Low");
        break;
      case climate::CLIMATE_FAN_MEDIUM:
        it->printf(((it->get_width() / 3) * 2) + 10, (it->get_height() / 2) + 5, thermostat_medium, highlight_fan_mode, TextAlign::CENTER, "Medium");
        break;
      case climate::CLIMATE_FAN_HIGH:
        it->printf(((it->get_width() / 3) * 2) + 10, (it->get_height() / 2) + 5, thermostat_medium, highlight_fan_mode, TextAlign::CENTER, "High");
        break;
      case climate::CLIMATE_FAN_MIDDLE:
        it->printf(((it->get_width() / 3) * 2) + 10, (it->get_height() / 2) + 5, thermostat_medium, highlight_fan_mode, TextAlign::CENTER, "Middle");
        break;
      case climate::CLIMATE_FAN_FOCUS:
        it->printf(((it->get_width() / 3) * 2) + 10, (it->get_height() / 2) + 5, thermostat_medium, highlight_fan_mode, TextAlign::CENTER, "Focus");
        break;
      case climate::CLIMATE_FAN_DIFFUSE:
        it->printf(((it->get_width() / 3) * 2) + 10, (it->get_height() / 2) + 5, thermostat_medium, highlight_fan_mode, TextAlign::CENTER, "Diffuse");
        break;
    }
  }
}
