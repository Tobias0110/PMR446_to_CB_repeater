# PMR446_to_CB_repeater
Small Arduino program which allows you to control a CB or PMR446 radio so that they act as repeater together.

The channel of the CB-radio can be controlled via DTMF-Signals. These signals are received by the PMR446-radio. The IC CM8870 is used to decode the DTMF-Signals (you can also use a more modern chip). The IO-chip PCF8574 is used to control the PLL of the CB-radio. It is also possible to deactivate the repeater by using DTMF-Signals.
This program was tested with an Arduino Nano.

For more details visit: http://teramine.no-ip.org/repeater_manual.html
