# Padfoot Tracker
Padfoot is a activity tracking application for four-legged friends, developed for my [Ada Developers Academy](https://www.adadevelopersacademy.org/) capstone project.

You can find a copy of my presentation, including a demo, [here](https://docs.google.com/presentation/d/194CYs_QVgdxTuEOwFOGOsLW1hw3vMLnqcob7OLrou4g/edit?usp=sharing). 

This repository contains the source code and instructions for building the Padfoot pedometer. Its [sister repository](https://github.com/jacquelynoelle/padfoot) contains the code and instructions for building the accompanying Android app.

## Building the Hardware

### Supplies
- 1 [Adafruit Feather nRF52 Bluefruit LE board](https://www.adafruit.com/product/3406)
- 1 [ADXL 337 accelerometer](https://www.sparkfun.com/products/12786)
- 1 [3.7V lithium ion battery](https://www.adafruit.com/product/1578) (must be compatible with the plug on the Adafruit board)
- A soldering iron with at least 25 watts
- Solder (a small amount will do)
- 6 wires (half inch or shorter)
- A microUSB cord that will connect the board to your computer

### Connecting Components

First, you need to establish a reliable connection between the accelerometer and the board's analog pins as follows:

| Accelerometer | Board       |
| ------------- | ----------- |
| GND           | A5          |
| 3.3V          | A4          |
| X             | A3          |
| Y             | A2          |
| Z             | A1          |
| ST            | A0          |

You'll need to first solder one end of each of the six wires to the accelerometer, then solder the other ends as diagrammed to the board. Please refer to [Adafruit's guide to soldering](https://learn.adafruit.com/adafruit-guide-excellent-soldering/tools) for further instruction. Remember, its dangerous - you're melting metal!

Last but not least, you can plug in the battery to the white socket on the board.

## Programming the Board
To install the Padfoot tracker program, you'll need to:
1. [Download the Arduino IDE](https://www.arduino.cc/en/main/software)
2. Clone this repository using 'git clone https://github.com/jacquelynoelle/padfoot_tracker.git'
3. Open the Arduino IDE and connect your board to your computer ([follow these instructions from Adafruit](https://learn.adafruit.com/bluefruit-nrf52-feather-learning-guide/arduino-board-setup))
4. In the Tools menu, click Manage libraries, and install the `Time` library by Michael Margolis (version 1.5.0) and the `Adafruit BluefruitLE nRF51` library by Adafruit.
5. Select "Open" from the menu, then choose the padfoot_tracker.ino file from the folder where you cloned the repo
6. Click the check mark button to verify the code, then click upload to add the sketch to your hardware
7. Click the magnifying glass to open the Serial Monitor
8. Get the current time in seconds from [here](http://www.onlineconversion.com/unix_time.htm)
9. Type in T plus the current time into the serial monitor -- the second you hit enter, you're starting the board's clock at that time
10. Note that the pedometer won't start counting steps until the time is set
11. Unplug the device -- it should now be ready to send step data over Bluetooth LE!

## Testing the Tracker
Besides downloading and running the Padfoot Android app, you can also use an existing application like [LightBlue](https://play.google.com/store/apps/details?id=com.punchthrough.lightblueexplorer&hl=en_US) to simply test out the Bluetooth LE data stream.
