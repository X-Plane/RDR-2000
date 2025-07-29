# RDR-2000 For X-Plane

This plugin is both a demonstration of what kind of weather radar post-processing can be done
using the X-Plane 12.3.0+ SDK, and a ready-to-install weather radar for third-party developers
to include in their aircraft. A copy ships with X-Plane 12.3.0+, in the Beechcraft Baron

## Usage (for Developers)

To use the RDR-2000 plugin in your own aircraft:

1. Copy the rdr2000 folder to your aircraft's `plugins` directory;
2. In PlaneMaker, under `Standard` > `Viewpoint`, in the "Map" panel, set at least one
   weather radar dropdown to "Plugin"; **selecting "X-Plane" instead of "Plugin" will
   result in incorrect weather returns;
3. In your aircraft's model, map the screen of the weather radar display model a custom cockpit
   device with the ID `RDR2000_WXR`, and map the buttons and knobs to the commands and datarefs
   listed below.

## Commands and Datarefs

**Display**

- device ID (use in Blender as "Plugin-based device"): `RDR2000_WXR`

**Buttons**

- Wx: command `rdr2000/mode_wx`
- WxA: command `rdr2000/mode_wxa`
- GND MAP: command `rdr2000/mode_map`

- Range up: command `rdr2000/range_up`
- Range down: command `rdr2000/range_down`
- Stab: command `rdr2000/stab`

**Screen (for popping up)**

command  `rdr2000/popup`

**Knobs**

- Brightness:
    - increase: command `rdr2000/brightness_up`
    - decrease: command `rdr2000/brightness_down`
    - value: dataref `rdr2000/brightness` (0.0 -> 1.0)

- Mode:
    - increase: command `rdr2000/mode_up`
    - decrease: command `rdr2000/mode_down`
    - value: dataref `rdr2000/mode` (0: off, 1: stby, 2: test, 3: on)
    
- Tilt
    - increase: command `rdr2000/tilt_up`
    - decrease: command `rdr2000/tilt_down`
    - value: dataref `rdr2000/tilt` (-15.0 -> 15.0)
    
- Gain
    - increase: command `rdr2000/gain_up`
    - decrease: command `rdr2000/gain_down`
    - value: dataref `rdr2000/gain` (0.0 -> 1.0)

