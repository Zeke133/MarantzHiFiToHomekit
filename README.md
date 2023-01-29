# MarantzHiFiToHomekit

## Description

Marantz PM4000 integration to Apple HomeKit based on ESP8266

## Details

How to control vintage Marantz PM4000 HiFi amplifier with Apple Homekit from iPad/iPhone?

There is a try to integrate a vintage [Marantz PM4000](https://www.hifiengine.com/manual_library/marantz/pm4000.shtml) HiFi amplifier to Apple HomeKit for a remote controlling.
As a hardware base I used ESP8266 NodeMCU board which found in my workshop.

Amplifier has an IR remote control and TTL-level Input/Output on a back panel to transmit/receive control signal by wire as well.
The last one I decided to utilize by connection to ESP8266 board.

Here is another project helped me on start - https://github.com/samm-git/marantz-rc-esp32.
There is a library to connect ESP8266 with Homekit I utilized - https://github.com/Mixiaoxiao/Arduino-HomeKit-ESP8266.
