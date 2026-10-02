# Arduino Digital Alarm Clock Project

## Project Overview

This project is an Arduino-based digital alarm clock with multiple
functions:

-   Display the current time and date.
-   Set an alarm using buttons.
-   Produce a beeping sound when the alarm triggers.
-   Work as a countdown timer.
-   Work as a stopwatch.
-   During the timer countdown, begin warning beeps when 5 seconds
    remain.
-   Produce one beep per second during the final 5 seconds.
-   Stop the timer and/or alarm appropriately when the countdown reaches
    zero.

## Main Features

### 1. Digital Clock

The device should continuously display: - Current time - Current date

### 2. Alarm

The user should be able to: - Set the desired alarm time using
buttons. - Enable/set the alarm. - Receive a beeping alert when the
alarm time is reached.

### 3. Timer

The device should: - Allow the user to set a countdown duration. - Start
the countdown. - Display the remaining time. - When 5 seconds remain,
give a warning beep once per second. - Continue until the countdown
reaches 0. - Give the appropriate final alarm/beep at 0.

### 4. Stopwatch

The device should: - Start counting elapsed time. - Stop/pause the
stopwatch. - Reset it when required.

## Hardware / Equipment

The project is intended to use an Arduino with a display, buttons, and a
buzzer.

Known available components from the project discussion include:

-   Arduino board
-   OLED display
-   Passive buzzer
-   Buttons (for clock/alarm/timer/stopwatch controls)
-   Temperature and humidity sensor (available, but not required for the
    basic alarm-clock version)
-   Sound sensor (available, but not required for the basic alarm-clock
    version)
-   Piezo element (available for other projects, not required for the
    basic alarm-clock version)

A real-time clock (RTC) module may be used if accurate timekeeping is
required while the Arduino is powered off. A suitable RTC such as a
DS3231 is recommended for that purpose.

## Suggested User Interface

The OLED can have a simple menu system such as:

1.  Clock
2.  Alarm
3.  Timer
4.  Stopwatch

Buttons can be assigned to: - Menu/select - Increase - Decrease -
Start/stop - Back/reset

The exact button arrangement can be changed depending on the available
hardware.

## Circuit

The circuit should connect: - Arduino → OLED display - Arduino → passive
buzzer - Arduino → control buttons - Arduino → RTC module (if used)

The exact pin assignments should be documented in the Arduino source
code and circuit diagram.

## Software Structure

The Arduino program should be organized into separate functions/modules
for:

-   Reading and displaying time/date
-   Alarm setup and checking
-   Timer setup and countdown
-   Stopwatch operation
-   Button input/debouncing
-   Buzzer control
-   OLED display/menu handling

The final Arduino `.ino` file should be added to the repository
separately.

## Project Development Notes

This document was created from the ChatGPT discussion for the Arduino
Digital Alarm Clock project. It is intended to give GitHub Copilot
project context without requiring the full conversational transcript.

When changing the project, keep the following goals in mind:

1.  Maintain normal clock operation while other features are available.
2.  Avoid blocking delays where they would interfere with clock, button,
    or timer operation.
3.  Use reliable button handling/debouncing.
4.  Keep the OLED interface simple and readable.
5.  Use a passive buzzer with appropriate tone generation.
6.  Keep alarm, timer, and stopwatch states independent so they can be
    maintained cleanly.
7.  Document all hardware pin connections.

## Repository Suggested Structure

``` text
digital-alarm-clock/
├── README.md
├── DigitalAlarmClock/
│   └── DigitalAlarmClock.ino
├── circuit/
│   └── circuit-diagram.png
└── docs/
    └── project-discussion.md
```

## Future Additions

The following can be added later:

-   Temperature/humidity display
-   Multiple alarms
-   Snooze function
-   12/24-hour mode
-   Adjustable buzzer patterns
-   Automatic brightness control
-   More advanced OLED menus
