Pomodoro
========

A custom built timer for the [pomodoro technique](http://pomodorotechnique.com/).

Use it
------

1. One click starts a 25 minutes _work_ counter. Five yellow LED indicates how
  many slices of five minutes are left. There is a discrete beep when the timer
  expires. The LED then flashes to remind you to go into _rest_ mode.
2. A second click starts a 5 minutes _rest_ counter. A **red** LED is turned on.
  There is a discrete beep when it expires. Then the timer turns off.
3. If you wish the turn the timer off during _rest_ mode, click a third time the button.

Build it
--------

### Bill Of Material

* 1x ATtiny85
* 5x LED yellow, SMD 0805
* 1x LED red, SMD 0805
* 3x 50 Ohm Resistor, SMD 0805
* 2x 1K Ohm Resistor, SMD 0805
* 1x PNP BPJ transistor, SOT-23 (BC846CMTF)
* 1x 1 mH Inductor, SMD 1007
* 1x Buzzer, SMD [mouser](http://www.mouser.com/ProductDetail/Kobitone/254-PB501-ROX/?qs=%2fha2pyFaduio21Wb3%2fEgDw7Itv2u2RCUk%252bCSGDldQmgXvX6wRiKaqw%3d%3d)
* 1x CR2032 coin cell holder, SMD [mouser](http://www.mouser.com/ProductDetail/Linx-Technologies/BAT-HLD-001/?qs=sGAEpiMZZMupuRtfu7GC%252bdEIlVvqTbq%252bJLMEoC1V420%3d)
* 1x Tactile switch, SMD EVQ-Q2 [mouser](http://www.mouser.com/ProductDetail/Panasonic/EVQ-Q2K03W/?qs=sGAEpiMZZMsgGjVA3toVBA4puZS2llK5hCEf4xZ%2f9rI%3d)

### Schematic

![](./Pomodoro_sch.png)

### Etch the PCB

The PCB can be etched using the [mask](Pomodoro_mask.eps) provided in EPS format.
Here is a picture of the layout.

![](./Pomodoro_brd.png)

### Firmware

The firmware is in the `firmware` folder. It can be uploaded using avrdude and
a [usbtiny-isp](https://www.sparkfun.com/products/9825), for example. The idea is
to press a six-pin header againt the PCB directly when programing. Some spring-loaded
pins would be ideal for the job. The square pad is pad number one of a six-pin ISP
header.

To compile and upload (assuming you have avr-gcc and avrdude installed), type the
following in a terminal.

    make avrdude


License
-------

2014 (c) Robin Scheibler aka FakuFaku

    "THE BEER-WARE LICENSE" (Revision 42):
    <fakufaku@gmail.com> wrote this file. As long as you retain this notice you
    can do whatever you want with this stuff. If we meet some day, and you think
    this stuff is worth it, you can buy me a beer in return -- Robin Scheibler

![](https://upload.wikimedia.org/wikipedia/commons/d/d5/BeerWare_Logo.svg)
