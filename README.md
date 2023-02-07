# Marantz PM4000 integration to Apple HomeKit based on ESP8266

## Description

How to control vintage Marantz PM4000 HiFi amplifier with Apple Homekit from iPad/iPhone?

There is a try to integrate a vintage [Marantz PM4000](https://www.hifiengine.com/manual_library/marantz/pm4000.shtml) HiFi amplifier to Apple HomeKit for a remote controlling.

(for the audio signal transmission I utilized existing 3rd party Bluetooth to RCA adaptor)

## Hardware solution

As a hardware base I used ESP8266 NodeMCU board which found in my workshop.

Amplifier has an IR remote control and +5V TTL Input/Output on a back panel to transmit/receive control signal by wire as well.
This Remote-In socket was used to connect with ESP8266 board GPIO line via opto-isolator due to ESP 3.3V logic.

Internall amplifier control IC +5V power supply is based on small separate transformer and linear 7805 voltage stabilizer without any heatsink.
Unfortunately even with added heatsink 7805 is not enough to supply both ESP8266 board and Bluetooth adaptor therefore dedicated AC220/DC5 power supply with USB sockets I going to add.

Furthermore I added 3.5 Jack to the back pannel with signals from 7805 stabilized and power relay control which are planed to be connected to ESP GPIO for monitoring of amplifier power state.

One line there is constant +5V whenever amplifier is in Stand-by or Power-On state. This one used with opto-transistor to modulate control signal.

Second one is +5V activation Power-On relay to supply main amplifier board which could be used via opto-isolator to identify Power-On state.

For more details on implementation [please follow here](misc/misc.md)

## Projects used

- Arduino with ESP8266 board support
- HomeKit Arduino library for ESP8266 - https://github.com/Mixiaoxiao/Arduino-HomeKit-ESP8266.
- Here is a similar project inspired me on start - https://github.com/samm-git/marantz-rc-esp32.
