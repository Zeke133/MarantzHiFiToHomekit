# MarantzHiFiToHomekit

## Description

Marantz PM4000 integration to Apple HomeKit based on ESP8266

## Details

How to control vintage Marantz PM4000 HiFi amplifier with Apple Homekit from iPad/iPhone?

There is a try to integrate a vintage [Marantz PM4000](https://www.hifiengine.com/manual_library/marantz/pm4000.shtml) HiFi amplifier to Apple HomeKit for a remote controlling.
As a hardware base I used ESP8266 NodeMCU board which found in my workshop.

Amplifier has an IR remote control and TTL-level Input/Output on a back panel to transmit/receive control signal by wire as well.
The last one I decided to utilize by connection to ESP8266 board.
One of ESP GPIO lines is connected via opto-isolator to amplifier control input.
Another one is planed to be connected in the same way to the ampifier stand-by relay output for monitoring of amplifier power state.

Internall amplifier control IC power supply is based on small separate transformer and linear 7805 voltage stabilizer without a heatsink.
Therefore I decided to add dedicated AC220/DC5 power supply for ESP and Bluetooth adapter used.

Here is a related projects which helped me or were utilized here:
- https://github.com/samm-git/marantz-rc-esp32.
- https://github.com/Mixiaoxiao/Arduino-HomeKit-ESP8266.
