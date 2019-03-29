# Short introduction

This is project work in progress and is not ready for production. Please be free to use and/or contribute to this project.

The project goal is in first place to monitor a water towers level and consumption. In a later phase, automatic control of the pumping might be added.


# Plan

Test two transducers to monitor the water level.

# Status

The project has been tested in the lab, but not yet in real circumstances.

# Parts

* Arduino uno R3 (https://www.jumia.cm/arduino-carte-uno-avec-sortie-micro-usb-151883.html)
* ESP-01/ESP8266 wifi module (https://www.jumia.cm/arduino-carte-wifi-esp-01-152365.html)
* WAVGAT Waterproof Ultrasonic Module JSN-SR04T  (https://www.aliexpress.com/item/Wavgat-SN-SR04T/32561534054.html)
* 30 psi DV5V pressure transducer (https://www.aliexpress.com/item/DC5V-30psi-Pressure-Transducer/32868688713.html)
* 3V3 regulator, capacitors, resistors, jumpers, wires, 
* Power supply. Solar?
* Stripboard

See the fritzing sketch for details. Jumpers for switching the wifi module between pin 2,3 for normal operation and 0,1 for flashing of the ESP is not drawn but added to the actual circuit.

![Hardware](Circuit/stripboard.jpg?raw=true "Hardware")

# Software

* Arduino
* thingsboard for receiving data


![Thingsboard](thingsboard/dashboard.PNG?raw=true "Thingsboard")

