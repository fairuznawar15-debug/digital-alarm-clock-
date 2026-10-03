# digital-alarm-clock-
An arduino project that will show time , date and also work as a timer, stopwatch . Later on, Maybe it will have global clock version . 
#Connections:
| Component   | Pin        | Arduino |
| ----------- | ---------- | ------- |
| OLED        | VCC        | 5V      |
| OLED        | GND        | GND     |
| OLED        | SDA        | A4      |
| OLED        | SCL        | A5      |
| Buzzer      | +          | D9      |
| Buzzer      | −          | GND     |
| MENU button | one side   | D2      |
| UP button   | one side   | D3      |
| DOWN button | one side   | D4      |
| OK button   | one side   | D5      |
| All buttons | other side | GND     |

#Code: For Simple Interface [ Time+Date+Day ] 
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// =====================================================
// OLED
// =====================================================

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1

Adafruit_SSD1306 display(
  SCREEN_WIDTH,
  SCREEN_HEIGHT,
  &Wire,
  OLED_RESET
);


// =====================================================
// PINS
// =====================================================

#define JOY_X A0
#define JOY_Y A1
#define JOY_SW 2

#define BUZZER 9


// =====================================================
// JOYSTICK SETTINGS
// =====================================================

#define JOY_LOW 350
#define JOY_HIGH 700


// =====================================================
// MENU MODES
// =====================================================

enum Mode {
  CLOCK_MODE,
  ALARM_MODE,
  TIMER_MODE,
  STOPWATCH_MODE
};

Mode currentMode = CLOCK_MODE;

int menuSelection = 0;


// =====================================================
// CLOCK
// =====================================================

int currentSecond = 0;
int currentMinute = 0;
int currentHour = 12;

int currentDay = 2;
int currentMonth = 10;
int currentYear = 2026;

unsigned long lastClockUpdate = 0;


// =====================================================
// ALARM
// =====================================================

int alarmHour = 7;
int alarmMinute = 0;

bool alarmEnabled = false;
bool alarmRinging = false;

unsigned long lastAlarmBeep = 0;

int lastAlarmTriggeredMinute = -1;


// =====================================================
// TIMER
// =====================================================

unsigned long timerSetSeconds = 60;
unsigned long timerRemaining = 0;

bool timerRunning = false;

unsigned long lastTimerUpdate = 0;


// =====================================================
// STOPWATCH
// =====================================================

bool stopwatchRunning = false;

unsigned long stopwatchStart = 0;
unsigned long stopwatchElapsed = 0;


// =====================================================
// CLOCK SETTING
// =====================================================

bool settingClock = false;

int settingField = 0;

// 0 = hour
// 1 = minute
// 2 = second
// 3 = day
// 4 = month
// 5 = year


// =====================================================
// JOYSTICK DEBOUNCE
// =====================================================

unsigned long lastJoystickAction = 0;


// =====================================================
// SETUP
// =====================================================

void setup() {

  pinMode(JOY_SW, INPUT_PULLUP);

  pinMode(BUZZER, OUTPUT);

  Serial.begin(9600);


  // Start OLED

  if (!display.begin(
        SSD1306_SWITCHCAPVCC,
        0x3C)) {

    while (true);
  }


  display.clearDisplay();

  display.setTextColor(SSD1306_WHITE);

  display.setTextSize(2);

  display.setCursor(10, 10);
  display.println("DIGITAL");

  display.setCursor(25, 32);
  display.println("CLOCK");

  display.display();

  delay(1500);


  lastClockUpdate = millis();
}


// =====================================================
// MAIN LOOP
// =====================================================

void loop() {

  updateClock();

  checkAlarm();


  // ---------------------------------------------------
  // If alarm is ringing
  // ---------------------------------------------------

  if (alarmRinging) {

    showAlarmRinging();

    return;
  }


  // ---------------------------------------------------
  // If setting clock
  // ---------------------------------------------------

  if (settingClock) {

    clockSettingScreen();

    return;
  }


  // ---------------------------------------------------
  // Main menu
  // ---------------------------------------------------

  mainMenu();
}


// =====================================================
// JOYSTICK BUTTON
// =====================================================

bool joystickPressed() {

  if (digitalRead(JOY_SW) == LOW) {

    if (millis() - lastJoystickAction > 250) {

      lastJoystickAction = millis();

      while (digitalRead(JOY_SW) == LOW) {
        delay(5);
      }

      return true;
    }
  }

  return false;
}


// =====================================================
// JOYSTICK UP
// =====================================================

bool joystickUp() {

  int y = analogRead(JOY_Y);

  if (y > JOY_HIGH) {

    if (millis() - lastJoystickAction > 180) {

      lastJoystickAction = millis();

      return true;
    }
  }

  return false;
}


// =====================================================
// JOYSTICK DOWN
// =====================================================

bool joystickDown() {

  int y = analogRead(JOY_Y);

  if (y < JOY_LOW) {

    if (millis() - lastJoystickAction > 180) {

      lastJoystickAction = millis();

      return true;
    }
  }

  return false;
}


// =====================================================
// JOYSTICK LEFT
// =====================================================

bool joystickLeft() {

  int x = analogRead(JOY_X);

  if (x < JOY_LOW) {

    if (millis() - lastJoystickAction > 180) {

      lastJoystickAction = millis();

      return true;
    }
  }

  return false;
}


// =====================================================
// JOYSTICK RIGHT
// =====================================================

bool joystickRight() {

  int x = analogRead(JOY_X);

  if (x > JOY_HIGH) {

    if (millis() - lastJoystickAction > 180) {

      lastJoystickAction = millis();

      return true;
    }
  }

  return false;
}


// =====================================================
// LEAP YEAR
// =====================================================

bool isLeapYear(int year) {

  if (year % 400 == 0)
    return true;

  if (year % 100 == 0)
    return false;

  if (year % 4 == 0)
    return true;

  return false;
}


// =====================================================
// DAYS IN MONTH
// =====================================================

int daysInMonth(int month, int year) {

  switch (month) {

    case 1:
      return 31;

    case 2:
      if (isLeapYear(year))
        return 29;
      else
        return 28;

    case 3:
      return 31;

    case 4:
      return 30;

    case 5:
      return 31;

    case 6:
      return 30;

    case 7:
      return 31;

    case 8:
      return 31;

    case 9:
      return 30;

    case 10:
      return 31;

    case 11:
      return 30;

    case 12:
      return 31;
  }

  return 30;
}


// =====================================================
// UPDATE CLOCK
// =====================================================

void updateClock() {

  unsigned long now = millis();

  if (now - lastClockUpdate >= 1000) {

    lastClockUpdate += 1000;

    currentSecond++;


    // Seconds

    if (currentSecond >= 60) {

      currentSecond = 0;
      currentMinute++;
    }


    // Minutes

    if (currentMinute >= 60) {

      currentMinute = 0;
      currentHour++;
    }


    // Hours

    if (currentHour >= 24) {

      currentHour = 0;
      currentDay++;
    }


    // Day

    if (currentDay >
        daysInMonth(currentMonth, currentYear)) {

      currentDay = 1;
      currentMonth++;
    }


    // Month

    if (currentMonth > 12) {

      currentMonth = 1;
      currentYear++;
    }
  }
}


// =====================================================
// MAIN MENU
// =====================================================

void mainMenu() {

  display.clearDisplay();

  display.setTextColor(SSD1306_WHITE);

  display.setTextSize(1);

  display.setCursor(28, 0);

  display.println("DIGITAL CLOCK");


  const char* menuItems[] = {

    "Clock",
    "Alarm",
    "Timer",
    "Stopwatch"
  };


  for (int i = 0; i < 4; i++) {

    display.setCursor(15, 13 + i * 12);

    if (i == menuSelection)
      display.print("> ");
    else
      display.print("  ");

    display.println(menuItems[i]);
  }


  display.display();


  // UP

  if (joystickUp()) {

    menuSelection--;

    if (menuSelection < 0)
      menuSelection = 3;
  }


  // DOWN

  if (joystickDown()) {

    menuSelection++;

    if (menuSelection > 3)
      menuSelection = 0;
  }


  // SELECT

  if (joystickPressed()) {

    currentMode = (Mode)menuSelection;
  }


  // Draw selected screen

  if (currentMode == CLOCK_MODE) {

    clockScreen();
  }

  else if (currentMode == ALARM_MODE) {

    alarmScreen();
  }

  else if (currentMode == TIMER_MODE) {

    timerScreen();
  }

  else if (currentMode == STOPWATCH_MODE) {

    stopwatchScreen();
  }


  delay(50);
}


// =====================================================
// CLOCK SCREEN
// =====================================================

void clockScreen() {

  display.clearDisplay();


  display.setTextSize(2);

  display.setCursor(15, 3);

  printTwoDigits(currentHour);

  display.print(":");

  printTwoDigits(currentMinute);

  display.print(":");

  printTwoDigits(currentSecond);


  // Date

  display.setTextSize(1);

  display.setCursor(22, 27);

  printTwoDigits(currentDay);

  display.print("/");

  printTwoDigits(currentMonth);

  display.print("/");

  display.print(currentYear);


  // Alarm status

  display.setCursor(10, 41);

  display.print("Alarm: ");


  if (alarmEnabled) {

    printTwoDigits(alarmHour);

    display.print(":");

    printTwoDigits(alarmMinute);

  }

  else {

    display.print("OFF");
  }


  display.setCursor(5, 56);

  display.print("Press=Set  Left=Back");


  display.display();


  // Press joystick = set clock

  if (joystickPressed()) {

    settingClock = true;

    settingField = 0;
  }


  // Left = menu

  if (joystickLeft()) {

    currentMode = CLOCK_MODE;
  }


  delay(50);
}


// =====================================================
// CLOCK SETTING
// =====================================================

void clockSettingScreen() {

  display.clearDisplay();


  display.setTextSize(1);

  display.setCursor(27, 0);

  display.println("SET CLOCK");


  display.setTextSize(2);

  display.setCursor(15, 13);

  printTwoDigits(currentHour);

  display.print(":");

  printTwoDigits(currentMinute);

  display.print(":");

  printTwoDigits(currentSecond);


  display.setTextSize(1);

  display.setCursor(20, 35);

  printTwoDigits(currentDay);

  display.print("/");

  printTwoDigits(currentMonth);

  display.print("/");

  display.print(currentYear);


  display.setCursor(5, 47);

  display.print("Field: ");


  switch (settingField) {

    case 0:
      display.print("HOUR");
      break;

    case 1:
      display.print("MINUTE");
      break;

    case 2:
      display.print("SECOND");
      break;

    case 3:
      display.print("DAY");
      break;

    case 4:
      display.print("MONTH");
      break;

    case 5:
      display.print("YEAR");
      break;
  }


  display.setCursor(5, 58);

  display.print("UP/DOWN  Press=Next");


  display.display();


  // UP

  if (joystickUp()) {

    changeClockValue(1);
  }


  // DOWN

  if (joystickDown()) {

    changeClockValue(-1);
  }


  // PRESS = next field

  if (joystickPressed()) {

    settingField++;


    if (settingField > 5) {

      settingClock = false;

      settingField = 0;

      lastClockUpdate = millis();
    }
  }


  // LEFT = cancel

  if (joystickLeft()) {

    settingClock = false;

    settingField = 0;
  }


  delay(50);
}


// =====================================================
// CHANGE CLOCK VALUE
// =====================================================

void changeClockValue(int amount) {

  switch (settingField) {


    // HOUR

    case 0:

      currentHour += amount;

      if (currentHour >= 24)
        currentHour = 0;

      if (currentHour < 0)
        currentHour = 23;

      break;


    // MINUTE

    case 1:

      currentMinute += amount;

      if (currentMinute >= 60)
        currentMinute = 0;

      if (currentMinute < 0)
        currentMinute = 59;

      break;


    // SECOND

    case 2:

      currentSecond += amount;

      if (currentSecond >= 60)
        currentSecond = 0;

      if (currentSecond < 0)
        currentSecond = 59;

      break;


    // DAY

    case 3:

      currentDay += amount;

      if (currentDay >
          daysInMonth(currentMonth, currentYear)) {

        currentDay = 1;
      }

      if (currentDay < 1) {

        currentDay =
          daysInMonth(currentMonth, currentYear);
      }

      break;


    // MONTH

    case 4:

      currentMonth += amount;

      if (currentMonth > 12)
        currentMonth = 1;

      if (currentMonth < 1)
        currentMonth = 12;


      if (currentDay >
          daysInMonth(currentMonth, currentYear)) {

        currentDay =
          daysInMonth(currentMonth, currentYear);
      }

      break;


    // YEAR

    case 5:

      currentYear += amount;

      if (currentYear > 2099)
        currentYear = 2000;

      if (currentYear < 2000)
        currentYear = 2099;


      if (currentDay >
          daysInMonth(currentMonth, currentYear)) {

        currentDay =
          daysInMonth(currentMonth, currentYear);
      }

      break;
  }
}


// =====================================================
// ALARM SCREEN
// =====================================================

void alarmScreen() {

  display.clearDisplay();


  display.setTextSize(1);

  display.setCursor(43, 0);

  display.println("ALARM");


  display.setTextSize(2);

  display.setCursor(25, 13);

  printTwoDigits(alarmHour);

  display.print(":");

  printTwoDigits(alarmMinute);


  display.setTextSize(1);

  display.setCursor(40, 36);

  if (alarmEnabled)
    display.println("ON");
  else
    display.println("OFF");


  display.setCursor(5, 48);

  display.println("UP/DOWN = Adjust");


  display.setCursor(5, 59);

  display.println("Press=ON/OFF");


  display.display();


  // UP

  if (joystickUp()) {

    alarmMinute++;

    if (alarmMinute >= 60) {

      alarmMinute = 0;
      alarmHour++;
    }

    if (alarmHour >= 24)
      alarmHour = 0;
  }


  // DOWN

  if (joystickDown()) {

    alarmMinute--;

    if (alarmMinute < 0) {

      alarmMinute = 59;
      alarmHour--;
    }

    if (alarmHour < 0)
      alarmHour = 23;
  }


  // PRESS = enable/disable

  if (joystickPressed()) {

    alarmEnabled = !alarmEnabled;

    tone(BUZZER, 1000, 100);
  }


  // LEFT = back

  if (joystickLeft()) {

    currentMode = CLOCK_MODE;
  }


  delay(50);
}


// =====================================================
// CHECK ALARM
// =====================================================

void checkAlarm() {

  if (!alarmEnabled)
    return;


  // Reset trigger tracking when minute changes

  if (currentMinute != lastAlarmTriggeredMinute &&
      currentSecond != 0) {

    // Do nothing
  }


  // Trigger exactly at alarm time

  if (currentHour == alarmHour &&
      currentMinute == alarmMinute &&
      currentSecond == 0 &&
      lastAlarmTriggeredMinute != currentMinute) {

    alarmRinging = true;

    lastAlarmBeep = 0;

    lastAlarmTriggeredMinute =
      currentMinute;
  }


  // Alarm sound

  if (alarmRinging) {

    if (millis() - lastAlarmBeep >= 500) {

      lastAlarmBeep = millis();

      tone(BUZZER, 1500, 250);
    }
  }
}


// =====================================================
// ALARM RINGING SCREEN
// =====================================================

void showAlarmRinging() {

  display.clearDisplay();


  display.setTextSize(2);

  display.setCursor(20, 12);

  display.println("ALARM!");


  display.setTextSize(1);

  display.setCursor(20, 38);

  display.println("Press joystick");


  display.setCursor(24, 50);

  display.println("to stop");


  display.display();


  // Press joystick = stop alarm

  if (joystickPressed()) {

    alarmRinging = false;

    noTone(BUZZER);
  }


  delay(20);
}


// =====================================================
// TIMER SCREEN
// =====================================================

void timerScreen() {


  // ---------------------------------------------
  // TIMER NOT RUNNING
  // ---------------------------------------------

  if (!timerRunning) {


    // UP = +10 seconds

    if (joystickUp()) {

      timerSetSeconds += 10;


      if (timerSetSeconds > 3599)
        timerSetSeconds = 3599;
    }


    // DOWN = -10 seconds

    if (joystickDown()) {

      if (timerSetSeconds >= 10)
        timerSetSeconds -= 10;
    }


    // PRESS = START

    if (joystickPressed()) {

      timerRemaining = timerSetSeconds;

      timerRunning = true;

      lastTimerUpdate = millis();
    }
  }


  // ---------------------------------------------
  // TIMER RUNNING
  // ---------------------------------------------

  if (timerRunning) {

    if (millis() - lastTimerUpdate >= 1000) {

      lastTimerUpdate += 1000;


      if (timerRemaining > 0)
        timerRemaining--;


      // Final 5 seconds

      if (timerRemaining <= 5 &&
          timerRemaining > 0) {

        tone(BUZZER, 1200, 150);
      }


      // Timer finished

      if (timerRemaining == 0) {

        timerRunning = false;


        // Three beeps

        for (int i = 0; i < 3; i++) {

          tone(BUZZER, 1500, 250);

          delay(350);
        }
      }
    }
  }


  // ---------------------------------------------
  // DISPLAY
  // ---------------------------------------------

  display.clearDisplay();


  display.setTextSize(1);

  display.setCursor(44, 0);

  display.println("TIMER");


  unsigned long secondsToDisplay;


  if (timerRunning)
    secondsToDisplay = timerRemaining;
  else
    secondsToDisplay = timerSetSeconds;


  int minutes =
    secondsToDisplay / 60;

  int seconds =
    secondsToDisplay % 60;


  display.setTextSize(2);

  display.setCursor(28, 15);

  printTwoDigits(minutes);

  display.print(":");

  printTwoDigits(seconds);


  display.setTextSize(1);

  display.setCursor(25, 40);


  if (timerRunning)
    display.println("COUNTING...");
  else
    display.println("Press = START");


  display.setCursor(5, 53);

  display.println("UP/DOWN = +/-10s");


  display.display();


  // LEFT = back

  if (joystickLeft() && !timerRunning) {

    currentMode = CLOCK_MODE;
  }


  delay(50);
}


// =====================================================
// STOPWATCH SCREEN
// =====================================================

void stopwatchScreen() {


  // PRESS = START / STOP

  if (joystickPressed()) {

    if (!stopwatchRunning) {

      stopwatchStart =
        millis() - stopwatchElapsed;

      stopwatchRunning = true;
    }

    else {

      stopwatchElapsed =
        millis() - stopwatchStart;

      stopwatchRunning = false;
    }
  }


  // DOWN = RESET

  if (joystickDown()) {

    stopwatchRunning = false;

    stopwatchElapsed = 0;
  }


  // LEFT = BACK

  if (joystickLeft()) {

    currentMode = CLOCK_MODE;
  }


  // Update stopwatch

  if (stopwatchRunning) {

    stopwatchElapsed =
      millis() - stopwatchStart;
  }


  // Convert time

  unsigned long totalSeconds =
    stopwatchElapsed / 1000;

  unsigned long minutes =
    totalSeconds / 60;

  unsigned long seconds =
    totalSeconds % 60;

  unsigned long milliseconds =
    stopwatchElapsed % 1000;


  // Display

  display.clearDisplay();


  display.setTextSize(1);

  display.setCursor(36, 0);

  display.println("STOPWATCH");


  display.setTextSize(2);

  display.setCursor(8, 16);


  if (minutes < 10)
    display.print("0");

  display.print(minutes);

  display.print(":");


  if (seconds < 10)
    display.print("0");

  display.print(seconds);


  display.setTextSize(1);

  display.print(".");


  if (milliseconds < 100)
    display.print("0");

  if (milliseconds < 10)
    display.print("0");

  display.print(milliseconds);


  display.setCursor(15, 43);


  if (stopwatchRunning)
    display.println("Press = STOP");

  else
    display.println("Press = START");


  display.setCursor(15, 55);

  display.println("DOWN = RESET");


  display.display();


  delay(20);
}


// =====================================================
// PRINT TWO DIGITS
// =====================================================

void printTwoDigits(int number) {

  if (number < 10)
    display.print("0");

  display.print(number);
}
