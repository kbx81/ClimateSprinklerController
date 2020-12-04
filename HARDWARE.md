# kbx's Climate and Sprinkler Controller

![Climate and Sprinkler Controller](images/front-small.jpg "Climate and Sprinkler Controller")

If you want to dive right in to prepare your own build, skip down to the [Design Overview](#Design-Overview) section.

This project started as a thermostat, but, application aside, it's just a processor with some switches, sensors and maybe a display connected to it. There are footprints on the PCB for up to eleven switches and up to six sensors, to be exact. It's primarily aimed at controlling 24 volt AC loads, although its three relays will allow switching a range DC voltages if required. Therefore, it should be able to operate more or less anything over a reasonably wide range of voltages and currents, from HVAC systems to electric sprinkler valves to security system hardware and variety of other things.

## The Basics

**If you are not planning to use this as a thermostat and/or are already familiar with HVAC system control and wiring, skip to the next section.**

### What is a Thermostat?

A thermostat is a switch. It is a switch that happens to be controlled by temperature. The switch is either on or it is off.

Yup. That's it. It's literally just a temperature controlled switch -- I'm not over-simplifying. That's really all it is.

If your thermostat happens to control your heating system, it does exactly two things:
 1. When the temperature drops below the temperature you set on it, the switch turns on.
 1. When the temperature rises above the temperature you set on it, the switch turns off.

You thought it was more complicated than that? Sorry if I've disappointed you! That's really about it. Don't believe me? Here's a [lovely video](https://youtu.be/ZZC0SP02PqY) if you'd like a closer look!

Ok, ok..._some modern systems_ allow control that is more advanced than just that. Still, it's reasonably safe to say that most (residential) systems that are either more than about ten years old or are "small" (for example, those found in single or two bedroom homes/apartments that just don't require a lot of power to heat or cool them) do little more than that. For larger homes or (commercial) buildings, the control may be more complex and, as such, may require a more complex controller. Those are probably out of the scope of what's here.

### Getting Connected

I'm not going to reinvent the wheel here. There are a number of guides that explain [thermostat wiring](https://lmgtfy.com/?q=thermostat+wiring) if you are interested in learning more. Where I live, forced-air systems that both heat and cool are most common and, as such, most of my experience is with them. To quickly summarize, our thermostats are usually connected to these systems with four to five wires:
 - `R`: a red wire, which is "live" with 24 volts AC
 - `G`: a green wire, which controls the system's main blower fan. The thermostat internally shorts this to the `R` wire to activate the fan.
 - `W`: a white wire, which controls the system's heating capability. The thermostat internally shorts this to the `R` wire to activate heating.
 - `Y`: a yellow wire, which controls the system's cooling capability. The thermostat internally shorts this to the `R` wire to activate cooling.
 - `C`: a blue wire, which provides the return path from the `R` wire (above) to complete the (24 volt AC) circuit. **This is the wire that is usually missing and brings about the "not enough wires" problem I described in the [Preface](PREFACE.md).** Without this wire, there is no reliable way for the thermostat to draw power from the system and consequently it will not work because it simply has no power. This is the reason thermostats with batteries in them are so common here now -- their old mechanical predecessors simply didn't require power to function as modern, electronic thermostats do. As a cost-saving measure, installers often ran cables that did not include this wire.

### Even More Connected

More modern systems may have additional wires, such as a `W2` (second stage heating), `Y2` (second stage cooling), `RH` (`R` for heating system), or `RC` (`R` for cooling system). These are arguably only found in large(r) homes or buildings where extra cooling or heating capacity might be needed to adjust the indoor temperature when the outdoor temperature is more extreme. Still, the controller you've found here can support these particular connections (and then some) if they are available!

In addition to heating and cooling, some systems have the ability to humidify and/or dehumidify. Here, it is common to use the air conditioning system for dehumidification, while humidification is accomplished with a separate unit when needed (typically in the winter season). If available, you can connect (de)humidifiers to any of this controller's relays or "MISCx" TRIACs -- confirm the unit's power requirements before doing so!

## Design Overview

When I design devices such as this, I tend to focus on flexibility -- cost is usually an afterthought. This device is no exception; it can be built at least three different ways...but probably isn't the cheapest possible DIY option.

There are five key components:
 - Power supply
 - Processor (ESP32)
 - Sensors
 - Display
 - I/O Expander and switching hardware:
   - TRIACs, opto-isolators, snubber circuits
   - Relays, FETs, kick-back diodes

I can probably go without saying that the power supply and processor are always required if you want it to function at all. While (the) sensor(s) may be omitted, this is not recommended if it is to be used as a climate control device (I'll explain why later). With these items in mind, it is otherwise possible to omit:
 - the display, or
 - the I/O expander and switching hardware

Any combination of TRIACs and/or relays may be installed; in other words, it is not necessary to install _all_ relays and/or _all_ TRIACs. Just install the number of TRIACs and/or relays that you need for your use case. For example, if the application requires a single DC circuit and two AC circuits, only a single relay and two TRIACs could be installed. As an alternative in this situation, three relays could be used (and no TRIACs). This is entirely at the discretion of the builder. (Note that a benefit of the TRIACs is that they are not subject to contact wear as relays are -- in theory, they should last forever. Relay contacts wear out and will stop working after some time.)

Based on the above two build configurations, I've come to think of these devices in two ways:
 - A `controller`, which is a unit that has the switching hardware and may or may not have a display and encoder
 - A `satellite`, which has a display and encoder but no switching hardware

Both build configurations have at least one sensor.

I use a couple `satellite` units to monitor and report to Home Assistant the temperature in remote (with respect to the wired-in `controller` build) rooms. This data can then be used to guide the `controller` that is physically connected to the HVAC system. It's easy to aggregate the data from any number of `satellite` units and, with the right automations in place, use it to control the system. In addition, `satellite` units can provide alternate points of control for the system; for example, put one next to your bed and you could adjust the temperature without using your smartphone or voice control (and potentially waking up your SO in the process).

`Controller` units are builds that have (some) switching hardware but may or may not have a display. If built with a display, you can probably mount it on the wall as a direct replacement for your existing thermostat. With no display, the controller unit can still control your HVAC system (or whatever else you've wired up to it), you just won't be able to interact with it locally (at least, not easily). While building it this way might seem like a silly idea, _this is the solution to the "not enough wires" problem._

### Solving the "Not Enough Wires" Problem

_How does building it without a display solve the "not enough wires problem"?_

Imagine, for a moment, that your HVAC system is located in the basement of your house. Your thermostat is installed on a wall on the main floor. The cable to your thermostat from your HVAC system runs up, through the floor, around a corner, down the hall and finally up the wall to where your thermostat is mounted. You need five wires to install your new thermostat, but the system installer ran a cable that has only four wires. What do you do?

(At this point, I can assure you that your HVAC professional will install a small white plastic box with a tiny LCD on it. It will require a battery (or two) that you'll have to change periodically, probably once or twice a year. The battery will always fail at an inopportune time.)

By building one of these as a `controller` (and perhaps without a display and encoder), you can:
 1. Mount your `controller` (very) near your HVAC system -- for example, on the wall directly behind it...or hanging on the side of it
 1. Get a short cable--say, one to two yards/meters or so--that _does_ have enough wires (or you could recycle a couple pieces of the cable you're about to disconnect in the next step!)
 1. Disconnect the original cable from your HVAC system to your old thermostat, then
 1. Connect the new short cable from your HVAC system to the new `controller` that's now right next to your system.

What you have now is a switching device that can still _tell your HVAC system what to do_ -- you'll just need to feed it data from remote sensors (in this example, sensors that are located upstairs). These remote sensors could be the "satellite" units I described above -- or they could be something even more simple, such as [these sensor boards](https://github.com/kbx81/TempHumSensWithESP01) which I also designed. There also exists a super cheap [DHT22 + ESP-01 module](https://www.amazon.com/gp/product/B07L6CYFT9/)...although, in practice, I _really_ don't recommend DHT modules for this application -- they're too inconsistent. It's up to you!

So, to summarize:
 - The `controller` is installed (very) near your HVAC system and is connected to it with a short, easily-installed cable that _has enough wires_.
 - `Satellite` sensors located in areas to be climate controlled report data to Home Assistant.
 - (An) automation(s) in Home Assistant parse and feed this data back to the `controller` so that it can control your system as appropriate.

Ok, ok...I admit, it's more of a workaround than a "real solution". Even so, it works great!

### But What If...

I'm going to assume that, since you're here and are reading this, you're at least somewhat technically inclined and are asking "What if my Home Assistant goes down??? What if my Wi-Fi goes down??? This won't work then! It's not _really_ a solution!" You may recall that, above, I stated: "While (the) sensor(s) may be omitted, this is not recommended if it is to be used as a climate control device." In the [ESPHome](https://esphome.io) [configuration](ESPHome/) that I built for my devices, two events are monitored:
 1. The API connection state (Am I connected to [Home Assistant](https://www.home-assistant.io)?)
 1. The time since the [ESPHome](https://esphome.io) [template sensor](https://esphome.io/components/sensor/template.html) was last updated (by (an automation in) [Home Assistant](https://www.home-assistant.io))

There is a timer for each of these two events; each event's respective timer is reset each time the event occurs (such a mechanism is also often called a "watchdog"). If the device either 1. loses its connection to the [Home Assistant](https://www.home-assistant.io) API ([Home Assistant](https://www.home-assistant.io) is down, Wi-Fi is down) or 2. does not receive an update to the [template sensor](https://esphome.io/components/sensor/template.html) (your automation that updates it broke or otherwise stopped running), after a duration of your choosing, it will stop using the [template sensor](https://esphome.io/components/sensor/template.html) and switch to using its local sensor (BME280/BME680/SHTC3/TMP117/DHT22/Thermistor...note that in my [configurations](ESPHome/) I revert to using the BME280 in this case). While the data from its local sensor(s) won't necessarily be consistent with the sensors located in the areas of interest, it _will_ provide the controller with _some_ data so that it can continue to function on some level. It may not be ideal, but it can keep the system from becoming totally useless (and, as such, your house doesn't freeze/turn into a sauna).

It's not a perfect solution to the original problem and I'm sure that it won't work for everybody. Still, it should offer a level of flexibility that I think many home automation enthusiasts can work with. My experience has proven that [Home Assistant](https://www.home-assistant.io), [ESPHome](https://esphome.io) and my Wi-Fi/home network are extremely reliable and that they can handle this task with no trouble at all. They break _only when I break them_.

## I Want (to Build) One

Awesome! Read on for build-related information, tips, etc.

### Before You Build

If you've read through the previous section, you may have already begun thinking about how you'd like to build yours. _But,_ just in case you skipped that part, I'll outline the key points you need to consider here:
 1. How many units do you need? Are you simply replacing a single existing thermostat or do you desire/plan to extend the capabilities of your system, as well?
 1. How many control channels do you need for your intended application? For example, if you're only controlling a heating system, you likely only need a single channel. If you have a more complex system that can (for example) heat, cool, fan, and humidify, you need at least four channels/circuits. This determines how many TRIACs and/or relays you will need.
 1. If you are replacing an existing thermostat, _do you have enough wires?_ (My favorite problem!) If not, you may want to build a `controller` (see above) version as well as a `satellite` version (see above). As an alternative, you could build a complete version and power it with an external source through the barrel jack on the side. It is recommended to _not_ install the barrel jack if your HVAC system will provide power through the `R` and `C` wires.
 1. Which sensor(s) do you wish to use? Are you comfortable soldering tiny parts such as the BME280/BME680, SHTC3, or TMP117 (you need a hot air station or (toaster) oven for these!)? If not, is a DHT22 or thermistor accurate/consistent enough for your liking/needs? If not, consider a BME280, SHTC3, or TMP117 module from [Adafruit](https://www.adafruit.com/product/2652) or [Sparkfun](https://www.sparkfun.com/products/13676)! They can be plugged into one of the headers along the edges of the board. (My personal favorite is the BME280.)
 1. Do you want/need a display? If so, which display do you want to use? On the board there are headers for two different displays from Adafruit -- the [SSD1325](https://www.adafruit.com/product/2674) (grayscale, big) and the [SSD1351](https://www.adafruit.com/product/1431) (color, not big). It's absolutely possible to use a variety of other types of displays (even touch screens!) but the specifics of doing so are outside the scope of what I'll cover here.
 1. Do you want the display to be motion-activated? This is especially important if it is an OLED-type of display -- they have a relatively short lifespan! I used an [RCWL-0516 module](https://www.amazon.com/gp/product/B07MTWZDQZ/) to accomplish this, although a PIR sensor is suitable, as well. This may be attached to J7 or, if a DHT22 is not installed, the `U6` header could be used, as well (bridge pins 3 and 4 for easy mounting). (Pro-tip: I've installed a short jumper cable with a dupont-style connector and mounted the [RCWL-0516 module](https://www.amazon.com/gp/product/B07MTWZDQZ/) into the back side of the front cover of the enclosure. This arrangement distances it from the PCB which allows it to function more reliably.)

Once you have the PCBs and parts in-hand, you're ready to begin building!

### Construction

The bill of materials (BoM) ([v2/WROOM](https://octopart.com/bom-tool/Y3Xc4zFl), v3/WROVER coming soon!) contains not only the list of parts, but also the component references -- it can be used to determine what parts go where on the PCB. This should be everything you need to complete assembly.

The PCB is designed in a way that it has parts on both sides. I recommend starting with the front-facing side of the board, which is the side that has the ESP32, I/O expander, and sensors on it. This side has no "heavy" or otherwise particularly large parts on it, so assembling it first will make assembling the opposite side later on easier. I use and recommend a stencil with solder paste and a hot air soldering station, so having a board that doesn't wobble (much) when it's on the desk is important.

As you're assembling, you'll notice a number of solder jumpers on the board. This is a good point to discuss what they do so you can determine which ones you should bridge and how.

#### Solder Jumpers

##### v2/WROOM

- JP1 to JP3: U2 (MCP23017) I2C address selection. These are used to modify the I2C address of the MCP23017. (*See the note below.)
- JP4: Voltage selection for pin 3 of J4. Not connected by default; create a solder bridge to the voltage of your choosing if connecting an external device. **Not required for ESP32 programming.**
- JP5: J4 pin 6 to ~ESP_RESET -- connects through C4 to the ~RESET line of the ESP32 to allow resetting from a connected serial adapter.
- JP6: Connects the ESP32's GPIO26 pin to J4 pin 2.
- JP7: Connects the ESP32's GPIO26 pin to J5 pin 6.
- JP8: Voltage selection for U2 (MCP23017). This IC can operate with either 3.3 or 5 volts. _Use 5 volts with caution_ as the interrupt lines may put 5 volts onto the ESP32's GPIO 36 and/or 39 pins! This may cause undefined behavior and/or damage the ESP32. If using 5 volts, these pins must be configured to open drain mode on the MCP23017.
- JP9: U3 (LMR36520) PG to ~ESP_RESET -- allows the LMR36520 to hold the ESP32 in "reset" state until the power from the buck converter has stabilized.
- JP10: TMP117 Alert to MCP23017 B7. Not used if the TMP117 is not installed.
- JP11: ESP32 GPIO26 to J10, SD_CS pin -- may be used if the [SSD1351](https://www.adafruit.com/product/1431) is installed and the SD card is to be used.
- JP12: TMP117 A0 -- allows changing the I2C address of the TMP117. (*See the note below.)
- JP13: SD_CD to MCP23017 B6 pin -- may be used if the [SSD1351](https://www.adafruit.com/product/1431) is installed and the SD card is to be used.
- JP14: DHT22 Q2 Bypass. Shorts the drain and source pins of Q2. This transistor can be used to power-cycle the DHT22 if required by the application (they sometimes stop responding). Not used if the DHT22 is not installed. Note that this transistor could also be used to drive an LED, if required.
- JP15: BME280 A0 -- allows changing the I2C address of the BME280. (*See the note below.)

*Note: This jumper is shorted to `0` by default; if you wish to modify it, use a razor knife to cut the trace between the center and `0` pads on the board and then create a solder bridge between the center pad and the `1` pad. Use your DMM to confirm proper/expected conductivity.

After you're done assembling but **before powering it on for the first time**, always check for shorts -- particularly across power tracks. I use my DMM to measure between ground, the 3.3 volt and 5 volt rails to be sure they are not shorted together. If any of them are, you'll need to carefully check your soldering work and look for solder bridges. If there are no shorts, it's probably safe to power it on and load the firmware image onto the ESP32.

#### One More Thing!

There is a header labeled `J2` near the lower left on the back of the PCB. This header must be fitted with a jumper to select where the board should draw power from. There are four options: one of three possible AC circuits (`RC`/`C`, `RH`/`C`, or `RMISC`/`C`) as well as `J1` which is the barrel jack. In the event that your HVAC system does not have separate/individual `RC` and/or `RH` wires, it is possible to jumper them together by placing jumpers on _both_ `RC` _and_ `RH`. This effectively turns them into a single `R` terminal. `RMISC` may also be added into this mix, as needed. This may be done if only a single `R` wire is available/connected.

Note that the barrel jack may be an AC or DC source and it is not polarity sensitive because of D1. It must provide a minimum of 7.5 volts and a maximum of 40 volts DC or 30 volts AC (because RMS, yada yada). A common 12-volt DC power brick will likely work best if the board is to be powered this way. Depending on exactly how it is built and the input voltage, its current requirement will vary. Even with a low input voltage (say, nine volts or so) it shouldn't need more than about half of an amp (500 mA) unless you've attached some extra hardware (sensors, LEDs, etc.). Even so, keep in mind that a higher voltage source will mean a lower current draw because of the buck converter. The rating of the polyfuse `F1` should ideally be adjusted accordingly.

For safety reasons, do not connect more than one power source at a time.

### Initial Firmware Flashing

To invoke the ESP32's bootloader, hold down the "Mode" pushbutton (which is connected to the ESP32's GPIO 0 pin) as you either power it on or tap the adjacent reset button, then release the "Mode" button. With the bootloader invoked, you can use J4 (labeled "SERIAL") to connect your serial adapter and flash the ESP32. There are plenty of guides on flashing ESP32s out on the web, so I'm not going to reinvent the wheel here. If you are having difficulty doing so, however, use your DMM and check for shorts across the transmit (TX) and receive (RX) lines. It's possible one or the other is shorted to ground, Vcc, or another adjacent track. In some cases, it may be necessary to hold down the encoder button (connected to GPIO 2) _as well as_ the "Mode" button (GPIO 0) to properly invoke the bootloader -- try this first if you're having any trouble.

With the firmware flashed onto the ESP32, you're ready to begin using your Climate and Sprinkler Controller(s)! If you made it this far, nice work!

## Installation and Usage

Installing the controller is pretty straight forward -- the terminal strips are labeled just like a typical thermostat. _Power off your HVAC system before (dis)connecting anything!_ Connect the wires as you would any other thermostat and you should be ready to go. The [esp_thermostat_nook.yaml](ESPHome/esp_thermostat_nook.yaml) configuration in the [ESPHome](https://esphome.io) [directory](ESPHome/) of this repository aims to function just like a "real thermostat". (The others are aimed at being `satellite` units used to enhance control of the system.)

Once all the little wires are connected, turn your HVAC system's power back on and your new controller should start up. If you are using the [esp_thermostat_nook.yaml](ESPHome/esp_thermostat_nook.yaml) configuration from this repository, pressing the encoder button down will cycle through the various screens/pages of the thermostat. Rotating the encoder changes the highlighted value (when present). I recommend clicking through the various screens to familiarize yourself with them prior to testing.

To test the thermostat:

- If your system supports heating, set the thermostat mode to either `Auto` or `Heat` mode and turn the temperature up to a value at least one degree above the currently displayed room/air temperature. Heating should commence.
- If your system supports cooling, set the thermostat mode to either `Auto` or `Cool` mode and turn the temperature down to a value at least one degree below the currently displayed room/air temperature. Cooling should commence.

**Important note:** the [ESPHome](https://esphome.io) [configurations](ESPHome/) I've provided have timers built into them that deliberately prevent the physical outputs (TRIACs, in this case) from changing states too quickly. For example, AC compressors are sensitive to being switched on/off too quickly (this is called "short cycling"). Because of this, you may find that cooling/heating will not stop despite that the temperature has been reset to a value that triggers the "idle" state/behavior. The display (and HA) will reflect this, but the outputs won't. Don't worry -- once the timer expires, the respective output will switch off, as expected. Note that these delays may be modified by changing the configuration to your liking -- just be careful, as making them too short will shorten the life of or even damage your system!

If all this works, you have a new thermostat! Nice work!

## Legal stuff and License

Everything you see here is provided to you free of charge. You are free to use it however you please within the constraints of the license -- but you do so at your own risk. I accept absolutely no liability if you cause damage to your property, HVAC system, or any other hardware you choose to connect to this device.

The circuit schematics, PCBs and enclosures found here are licensed under the
 [Creative Commons Attribution-ShareAlike 4.0 International License](http://creativecommons.org/licenses/by-sa/4.0/).

![Creative Commons License badge](https://i.creativecommons.org/l/by-sa/4.0/88x31.png)

_Happy building!_