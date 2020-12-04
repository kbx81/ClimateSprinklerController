# kbx's Climate and Sprinkler Controller

![Climate and Sprinkler Controller](images/front-small.jpg "Climate and Sprinkler Controller")

## A DIY Thermostat and/or Sprinkler (Valve) Controller

Here you'll find plans for an ESP32-based controller designed with [ESPHome](https://esphome.io) in mind. It is
 primarily designed for switching 24-volt AC circuits but it is flexible by design and should be adaptable to other
  types of electrical workloads.

In the [Preface](PREFACE.md) you will find background information about this project such as how it came to be and
 some personal thoughts and experiences I've had with home automation through the years.

In the [Hardware](HARDWARE.md) document you will find a general explanation of the theory of operation of a thermostat
 as well as a design overview of this device, some handy tips (should you choose to build one), construction, usage, 
 and installation detail.

## TLDR

The schematic and PCB were drawn in [KiCAD](https://www.kicad-pcb.org). There are two versions of the device and the
 project files for each can be found it its respective directory:
 - [Version 2 (WROOM)](WROOM/)
 - [Version 3 (WROVER)](WROVER/) (still a WIP and untested!)

I had PCBs fabricated by [Elecrow](https://www.elecrow.com) but nearly any PCB fabricator should be able to manufacture these without any issues.

On [Octopart](https://octopart.com) I have posted a bill of materials (BoM):
 - [Version 2 (WROOM)](https://octopart.com/bom-tool/Y3Xc4zFl)
 - Version 3 (WROVER) (still a WIP so the BoM isn't here yet!)

**Save money** by reading through the [build guide](HARDWARE.md) **before** you order any parts, as you likely _do not need **everything** in the linked BoMs!_

The [ESPHome](https://esphome.io) [directory](ESPHome/) contains YAML configuration files that I use for my installation with [ESPHome](https://esphome.io).

## Legal stuff and License

Everything you see here is provided to you free of charge. You are free to use it however you please within the constraints of the license -- but you do so at your own risk. I accept absolutely no liability if you cause damage to your property, HVAC system, or any other hardware you choose to connect to this device.

The circuit schematics, PCBs and enclosures found here are licensed under the
 [Creative Commons Attribution-ShareAlike 4.0 International License](http://creativecommons.org/licenses/by-sa/4.0/).

![Creative Commons License badge](https://i.creativecommons.org/l/by-sa/4.0/88x31.png)

_Thanks for visiting!_