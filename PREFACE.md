# kbx's Climate and Sprinkler Controller

![Climate and Sprinkler Controller](images/Nextion/front-small.jpg "Climate and Sprinkler Controller - Nextion 7\"")
![Climate and Sprinkler Controller](images/OLED/front-small.jpg "Climate and Sprinkler Controller - OLED")

This document outlines some of the thoughts I had that went into the design of this device.

## Why?

I started this project due to frustration with the thermostats I had at the time (more on this later). It also came about due to my general dislike of the typical cloud-based thermostats that frequently replace the old round dial or the white plastic box with (tiny, gray) LCD. I don't want a "dumb" thermostat, I don't want a battery-powered thermostat, and I don't want to be at the mercy of my ISP or ABC Corp. when they decide to turn off their web service(s) because they don't want to pay for hardware/software/servers/bandwidth anymore. I was also looking for more...flexibility...than the commercial solutions provide; I'm less interested in a thermostat and more interested in a climate control _system_.

Another issue that really bothers me and that I've seen surface quite often with respect to thermostats/HVAC control is wires. "I don't have enough wires to my thermostat to use the [insert electronic (cloud) thermostat here]." As I'm writing this, it's mid-2020; we have more really great technology now than ever before. It absolutely _blows my mind_ that, in light of this, wires are an obstacle to connecting a new thermostat. Now, I'm not saying that wires are not necessary -- of course, it's got to connect to your system somehow. I get that. But why do they need to stretch across your house, through walls and floors and ceilings where you can't easily get to them?...and why on earth do you need more than two? I often see "solutions" to the "wires problem" in the form of add-on hardware you can spend extra money on...although I've also gathered that these are less than trouble-free. Or you can just concede and use a cheap mechanical option...or a battery-powered option...but then of course you'll have to swap batteries every few months, which definitely works but it is not something I'm willing to settle for. Regardless, it's quite clear that "not enough wires" is a problem lots of folks run into when installing these things. I don't know why it bothers me so much; it just seems like a problem that should have been put to rest and completely solved for everybody everywhere 20+ years ago.

As a home automation enthusiast, I've already used at least a couple different "smart" thermostats. One was an old Honeywell IAQ, which featured a basic touchscreen interface and a four-event-per-day scheduling system, which was nice, but...eh. Over time I began installing Insteon hardware all around the house and, in sticking with the platform, replaced the Honeywell thermostat with the Insteon thermostats. Yes, plural, _thermostats_. The Insteon thermostats are actually pretty cool -- there is a wired one which you connect (yes, with wires, assuming you have enough of them) directly to your HVAC system and it can "pair" with up to two wireless units which are battery-powered and, as such, are actually portable. You can take them from room to room as you like, or they can be wall-mounted. One is designated the "master" and the temperature the master reads determines what the HVAC system will do (heat, cool, etc.); the master can be changed on-the-fly with a button on the front. Set the one in your bedroom as the master while you're sleeping, then change the other to be the master during the day/evening when you're in the living room watching TV. Sounds great...but now I'll tell you that they're tragically unreliable. Specifically, the wired unit has a terrible habit of jumping to a different mode when the HVAC system starts. For example, let's pretend it's winter and you have the system set to "heat" mode. The house gets cold, the thermostat sees the temperature drop, so it turns on the heat. A few seconds after switching on the heat, the thermostat magically switches itself into "cool" mode -- so the heat promptly turns off. Sure, it's not like the AC switches on, but it's not heating. Now your house just continues to get colder. Sometimes they even change the set points for you. I keep it at 72 (low)/75 (high) degrees F, but sometimes I look and it has suddenly switched itself to 60/80. No idea where those numbers came from, but I assume they're some kind of default values it randomly loads up for reasons unknown. So...overall...neat idea, terrible implementation. Not recommended. (I solved the erratic behavior with automations in Home Assistant -- the mode/temperature changes _are_ reported, so HA would see them and correct them as needed. Hacky solution, but it works great.)

Those aside...why not just get a Nest or Ecobee? I'm just not a fan of cloud-dependent devices; you're at the mercy of the manufacturer. See also: "Works with Nest". I gather that Ecobee allows at least some local control and isn't entirely dependent on the cloud, but, even so, I had some other ideas in mind...

"What...other...ideas?" you might ask.

The ideas I had are probably best expressed as a list of requirements. That's what engineers tend work with, right? So, here's what I defined as primary objectives for this project:
 - It must be "connected" so that it may be controlled and monitored remotely
 - It must not have any dependency on "the cloud". Absolutely none. At all. Whatsoever. Period.
 - It must support at a minimum heating, cooling, and fan-only modes of operation
 - It must be extensible such that any number of additional sensors may be added to monitor the temperature in locations other than where the thermostat is physically installed
 - It must be able to _act_ on data from other sensors and, if desired, _completely ignore_ its local sensor (aka the sensor on the device)
 - It must offer a solution to the "not enough wires" problem
 - It must not require batteries
 - It must be able to be locally controlled (like, with buttons), even when in a disconnected/offline state
 - It must function regardless of its state of connectivity to other sensors and/or to its point of remote control

...and some secondary ("nice to have") objectives (some of which spin out of the primary objectives):
 - In addition to heating, cooling, and fan-only modes, it should support two-stage heating and cooling, as well as humidification and dehumidification
 - It should offer at least some support for non-24-volt AC systems, such as millivolt systems
 - It must be able to be constructed in a "headless" manner (more on this later)
 - It should have a clear, easy-to-read display and a simple human interface
 - It should be able to display readings from other, remote sensors
 - It must be able to be powered from a source other than 24 volt AC
 - It should be reasonably easy to connect and install
 - It should be able to be constructed with the minimum required parts given its use case
 - It should be reasonably maker-friendly
 - It should be self-documenting as much as possible

## The Solution

As I tend to do, I designed and built something that's arguably a bit overkill. It meets all of the design goals I listed above and then some. The result is:
 - An ESP32-WROOM-32 (v2) or ESP32-WROVER-32 (v3 and Nextion versions) module as the "brain" with an MCP23017 (I2C) I/O expander for switching the...
 - Up to 11 control channels -- up to eight opto-isolated TRIAC-based switches and up to three SPDT relays
 - Indicator LEDs for all on-board control channels
 - Display support:
   - v2/v3: Connectors for two different (Adafruit) OLED displays, or...
   - Nextion: A dedicated serial header
 - Local conotrol:
   - v2/v3: An encoder with a button
   - Nextion: these are touchscreens!
 - Support (footprints) for up to eight different sensors (you don't need them all!):
   - BME280
   - BME680 (Nextion versions only)
   - SHTC3
   - TMP117
   - DHT22
   - Up to two thermistors (internal/external)
   - SGP40 (WROVER/v3 and Nextion versions only)
 - The ability to draw power from one of three available AC circuits (RH/C, RC/C, or RMISC/C) or from an on-board barrel jack that supports both AC and DC
 - Ports that break out all I/O capability, including serial, I2C, SPI, encoder (v2/v3), and the on-board I/O expander

As a home automation enthusiast, I'm already using [Home Assistant](https://www.home-assistant.io) and as an electronics hobbyist, I was already using [ESPHome](https://esphome.io); it seemed a logical solution for this device and, as such, I designed it with [ESPHome](https://esphome.io) in mind. Still, I expect it will work just fine with any code that will run on the ESP32.

## The Journey

There aren't many hardware projects I do that do _not_ involve me writing at least some software -- this one was no exception. Prior to this, I was a more casual [ESPHome](https://esphome.io) user; I'd set up a device to control some LED lights in my 3-D printer's enclosure as well as another device as an IR blaster (see the [kbxIRBlaster](https://github.com/kbx81/kbxIRBlaster)) for my home AV system. This idea came to me and I began experimenting with some hardware on a breadboard. It was easy to get going, although I realized that some of the existing [ESPHome](https://esphome.io) components would need some tweaking to really get it to work to my liking.

Long story short, this led to me submitting a number of PRs to the [ESPHome](https://esphome.io) project. This is the first open source project I've contributed to! (Although I'm no stranger to GitHub -- I regularly use it at my day job.) With the help of several kind folks on the [ESPHome](https://esphome.io) Discord server, my PRs eventually got merged into their `dev` branch and are now generally available! I more or less rewrote the `bang-bang` component, although, after some discussion, we decided to make this a new component, which is cleverly named `thermostat`. I added support for two new displays and, as a part of one of them, also played a large part in adding color (and grayscale) support to the platform. With these changes, I was able to create a wonderful little device that should keep my place (and yours!) comfy for a long time to come.

## The Aftermath

Of course, just designing and building a controller such as this isn't enough. After you finish it, you have to do something with it. Why not _actually use it_? That's what it's for, right? ...Right?

That was kind of the whole point of this -- to make something better to replace what I had. And, as it turns out, it is a great solution. For one, temperature data is much higher resolution, both in terms of reported temperature and time. While not terribly useful, a resolution of (about) one-tenth of a degree F makes charts much more interesting, as you can see how (small) actions (turning on the TV and a couple lamps, or taking a shower, for example) quickly begin to affect the temperature and humidity.

I set up three of them -- one physically connected (with wires!) to the HVAC system and two satellite units (more on this [here](HARDWARE.md)). One satellite resides in the bedroom while the other is in the living room. I built automations that keep them all in sync so that a change on one of them is reflected on all of them (since it's really just a single heating system). The sensor data is averaged among them and the average is used to control the system. It is a great setup and it works beautifully!

## The Technical Stuff

Please see the [build guide](HARDWARE.md) for specific detail regarding the hardware and how to build one!

## Closing Thoughts

At the time I'm writing this, I've been using these devices for over a year and have found them to be very reliable -- significantly more so than what I was using before. The sensors are very accurate and provide a high-resolution picture of the climate/environment. They offer a lot of flexibility and can be built-to-order. They are very hackable and anyone familiar with ESP devices should be able to pick one up and run with it. You can extend them by plugging extra hardware into the break-outs along the edges of the boards. I really hope others will find them useful and have fun with them!

Some folks may be concerned about reliability. In my experience, this is simply not an issue, both with respect to this device itself as well as Home Assistant and ESPHome. Home Assistant and ESPHome are quite stable at this point and, if that isn't your experience, I might suggest looking more closely at your hardware setup and/or how you are managing it. A little troubleshooting and/or optimizing can go a long way. I've been using these platforms for over two years at this point (in two houses!) and I have had zero unexpected crashes. Now, _I have broken it_, but it has always been because of something I did and, as such, I was fully aware of it and easily recovered. Occasionally I'll stumble across an update that brings about a breaking change -- so I search the release notes for anything related to the device(s) that broke, I adjust my config as needed and move on. Breaking changes are an opportunity to learn and improve. I've got over a hundred connected devices and a plethora of automations at each location so _it all gets used_. When the software is set up correctly and the hardware is built well, it should be as reliable as the old mechanical light switch you used to use before you installed that smart one. ;)

Thank you for sticking around!

## Legal stuff and License

Everything you see here is provided to you free of charge. You are free to use it however you please within the constraints of the license -- but you do so at your own risk. I accept absolutely no liability if you cause damage to your property, HVAC system, or any other hardware you choose to connect to this device.

The circuit schematics, PCBs and enclosures found here are licensed under the
 [Creative Commons Attribution-ShareAlike 4.0 International License](http://creativecommons.org/licenses/by-sa/4.0/).

![Creative Commons License badge](https://i.creativecommons.org/l/by-sa/4.0/88x31.png)

_Happy building!_