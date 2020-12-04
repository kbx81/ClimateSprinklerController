# kbx's Climate and Sprinkler Controller

![Climate and Sprinkler Controller](../images/front-small.jpg "Climate and Sprinkler Controller")

Note that all of the configurations I've shared here are built for controllers with the [SSD1325](https://www.adafruit.com/product/2674) display.

## Example YAML Configuration Files for a Climate Control System

This directory contains the YAML configuration files I use at home where I have my controllers installed.

- [esp_thermostat.h](esp_thermostat.h) contains the majority of the controller UI functionality as well as some C++ constants used within the code/lambdas.
- [esp_thermostat_nook.yaml](esp_thermostat_nook.yaml) is the configuration file I use for the controller that is physically wired/connected to my HVAC system.
- [esp_thermostat_front_room.yaml](esp_thermostat_front_room.yaml) is a satellite thermostat I keep in my living room area.
- [esp_thermostat_bedroom.yaml](esp_thermostat_bedroom.yaml) is a satellite thermostat I keep in my bedroom.

The beauty here comes in with [Home Assistant's](https://www.home-assistant.io) [automations](https://www.home-assistant.io/docs/automation/). (In my mind, this is where it stops being a thermostat and becomes a "climate control system".) For example, your automation could use different sensors based on time of day -- at night, the bedroom thermostat's sensor is used while during the day, the living room thermostat's sensor is used. If you install into your build(s) either a PIR sensor or the [RCWL-0516 module](https://www.amazon.com/gp/product/B07MTWZDQZ/) I suggested, motion sensing could play a part in this. I have an automation that takes the readings from all of my sensors, performs a simple averaging of them and then feeds this back to the [template sensor](https://esphome.io/components/sensor/template.html) in the configurations. This drives the heating/cooling in my living space. That said, I have a unique situation in that the wall-mounted "nook" controller is on a wall that has behind it a large boiler unit that heats the upstairs areas -- this boiler warms the wall when it runs, skewing the sensor readings of the "nook" controller. Because of this, I've placed one of [these sensor boards](https://github.com/kbx81/TempHumSensWithESP01) in the boiler room and, when the temperature it senses rises above 85 degrees F, my automation discards the reading from the sensors in the wall-mounted "nook" controller and just uses the other two (bedroom and front room). If I don't do this, my living space gets colder than it should be based on the thermostat's setting. (You can probably surmise now why I am _very dependent_ on the remote sensors...particularly in the winter season.)

In addition to what I described above, I've also created automations that keep the [ESPHome](https://esphome.io) [thermostat](https://esphome.io/components/climate/thermostat.html) components synchronized across all of the controllers; a change on one is reflected on all of them.

(Sidebar: I use [Node-RED](https://nodered.org) to build my automations. It's super cool. If you haven't tried it, this might be a good time to do so.)

The controller UI I've created for these is quite simple. Pressing/"clicking" the encoder advances to the next screen on the display. When available, a highlighted value may be modified by rotating the encoder.

The main screen displays the temperature indicated by the [template sensor](https://esphome.io/components/sensor/template.html) -- note that this is a computed value that's normally fed to the controller by the [Home Assistant](https://www.home-assistant.io) [automation](https://www.home-assistant.io/docs/automation/) that I've built. (This is the temperature that controls what the system ultimately does.) This screen also displays the temperature and humidity from a local sensor (specifically, the BME280) as well as the system's current mode or action ("heat"/"heating") and the time and date.

The second screen displays the values of other sensors my automations feed to the controller. This includes the temperatures and humidities from the other units as well as the outside temperature and humidity.

The third screen displays the set points and allows them to be adjusted.

The fourth screen displays the system's mode as well as the system's fan mode.

The final screen displays data from all installed sensors (there are several, in my case...because I can).

When the [RCWL-0516 module](https://www.amazon.com/gp/product/B07MTWZDQZ/) senses motion, the display is activated. It turns off after one minute.

## Example YAML Configuration Files for a Sprinkler System

- [esp_sprinkler.h](esp_sprinkler.h) contains the majority of the controller UI functionality as well as some C++ constants used within the code/lambdas.
- [esp_sprinkler_controller.yaml](esp_sprinkler_controller.yaml) is a configuration file that can be used to control electric valves that are a part of a sprinkler/irrigation system.

This configuration is still a WIP, however at this point all of the critical functionality is in place.

The [esp_sprinkler_controller.yaml](esp_sprinkler_controller.yaml) configuration implements a simple sprinkler valve controller. It is configured so that "zone 8" (aka `A7`) is a "master valve" (one that controls the water supply to the other individual zone valves). _The master valve is on when any other valve/zone is on_. In this configuration, five other valves/zones are defined. Zone 1 (`A0`) is the parkway zone, zone 2 (`A1`) is the front yard area, zone 3 (`A2`) is one side of the house, zone 4 (`A3`) is the back yard area and zone 5 (`A4`) is the other side of the house.

### Wiring

For this configuration, you'll want to install all of the jumpers on `J2`. Power must be supplied by the sprinkler valve power supply/transformer which _should_ be a 24-volt AC supply. This power may be fed into the controller through the barrel jack.

#### If using the barrel jack:

  - The common wire from the valve manifold would connect to the `C` terminal on the controller.
  - The remaining numbered terminals (`A0` - `A7`) would connect to the individual sprinkler zone valves (given this configuration `A7` would connect to the master valve).

#### If **not** using the barrel jack:

 - The common wire from the valve manifold would connect to the `C` terminal on the controller as well as one of the wires from the sprinkler valve power transformer.
 - The other wire from the the transformer would connect to one of the three `R` terminals on the controller (recall that all of the `R` terminals are now jumpered together because `J2`).
 - The remaining numbered terminals (`A0` - `A7`) would connect to the individual sprinkler zone valves (given this configuration `A7` would connect to the master valve).

### Usage

Default run durations for each sprinkler/valve zone are determined by global variables defined near the top of the [configuration file](esp_sprinkler_controller.yaml). A multiplier is also available to extend or reduce the run time for all zones. All of these values may be adjusted arbitrarily through the API (also defined in the configuration) from within [Home Assistant](https://www.home-assistant.io).

The [configuration](esp_sprinkler_controller.yaml) defines a switch for each zone, a switch to "enable" each zone, a switch for the master valve and an "auto-advance" switch. (At this time the various other sensors are also present; remove them if you like as they really are not necessary.)

Zones that are "enabled" will run in the next cycle _if "auto-advance" is on/enabled_.

Turning on a single zone switch will cause that zone to run for its configured duration. If "auto-advance" is on/enabled, the controller will continue to iterate through any other enabled zones (in a circular fashion).

Turning on the master valve will cause the controller to iterate through all "enabled" zones; it will start with the lowest-numbered enabled zone. Each zone will run for its configured duration, then it will advance to the next zone. This continues until all enabled zones have run.

Turning on the master valve _with no zones enabled_ will _enable all zones and turn on auto-advance_. This effectively starts a full cycle of the system.

Locally (on the controller itself), press the encoder button to toggle between the main screen and the "setup" screen. The latter allows you to enable and disable the various individual zones by rotating the encoder wheel to select and then pressing/"clicking" to toggle them. Selecting and clicking on "Start" from this screen will start a cycle. The behavior is the same as described above with respect to zones being enabled/disabled, etc.

When the [RCWL-0516 module](https://www.amazon.com/gp/product/B07MTWZDQZ/) senses motion, the display is activated. It turns off after one minute.

Future plans include a feature to trigger the controller to start a cycle at a specified time of day. While this is not currently available, it can easily be achieved with an [automation](https://www.home-assistant.io/docs/automation/) that may be built in [Home Assistant](https://www.home-assistant.io).

## Legal stuff and License

The circuit schematics and PCB found here are licensed under the
 [Creative Commons Attribution-ShareAlike 4.0 International License](http://creativecommons.org/licenses/by-sa/4.0/).

![Creative Commons License badge](https://i.creativecommons.org/l/by-sa/4.0/88x31.png)

_Thanks for visiting!_