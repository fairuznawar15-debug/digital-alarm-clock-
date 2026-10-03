
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// =====================================================
// OLED
// =====================================================

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// =====================================================
// JOYSTICK
// =====================================================

#define JOY_X  A0
#define JOY_Y  A1
#define JOY_SW 2

// =====================================================
// BUZZER
// =====================================================

#define BUZZER 9

// =====================================================
// JOYSTICK THRESHOLDS
// =====================================================

#define JOY_LOW  350
#define JOY_HIGH 700

// =====================================================
// CLOCK
// Internal clock remains 24-hour.
// Display is 12-hour AM/PM.
// =====================================================

int currentSecond = 0;
int currentMinute = 0;
int currentHour   = 12;

// Date = 02/10/2026
int currentDay   = 2;
int currentMonth = 10;
int currentYear  = 2026;

// =====================================================
// ALARM
// Internal alarm hour is also 24-hour.
// =====================================================

int alarmHour   = 12;
int alarmMinute = 0;

bool alarmEnabled = false;
bool alarmRinging = false;

// =====================================================
// TIMER
// =====================================================

unsigned long timerSeconds = 0;
unsigned long timerStartMillis = 0;

bool timerRunning = false;

unsigned long lastTimerBeep = 999999;

// =====================================================
// STOPWATCH
// =====================================================

unsigned long stopwatchStartMillis = 0;
unsigned long stopwatchElapsed = 0;

bool stopwatchRunning = false;

// =====================================================
// MODES
// =====================================================

#define MENU_MODE       0
#define CLOCK_MODE      1
#define ALARM_MODE      2
#define TIMER_MODE      3
#define STOPWATCH_MODE  4

int currentMode = MENU_MODE;

// =====================================================
// JOYSTICK BUTTON
// =====================================================

bool lastButtonState = HIGH;
unsigned long lastButtonTime = 0;

// =====================================================
// CLOCK TIMING
// =====================================================

unsigned long lastClockMillis = 0;

// =====================================================
// MENU
// =====================================================

int menuSelection = 0;

const char* menuItems[] = {
  "Clock",
  "Alarm",
  "Timer",
  "Stopwatch"
};

// =====================================================
// CLOCK SETTING
// =====================================================

bool settingClock = false;

int clockField = 0;

/*
  0 = Hour
  1 = Minute
  2 = Second
  3 = Day
  4 = Month
  5 = Year
*/

// =====================================================
// FUNCTIONS
// =====================================================

// -----------------------------------------------------
// Joystick X
// -----------------------------------------------------

int readX() {
  return analogRead(JOY_X);
}

// -----------------------------------------------------
// Joystick Y
// -----------------------------------------------------

int readY() {
  return analogRead(JOY_Y);
}

// -----------------------------------------------------
// Vertical joystick
//
//  1 = UP
// -1 = DOWN
//  0 = CENTER
// -----------------------------------------------------

int joystickVertical() {

  int y = readY();

  if (y < JOY_LOW) {
    return 1;
  }

  if (y > JOY_HIGH) {
    return -1;
  }

  return 0;
}

// -----------------------------------------------------
// Horizontal joystick
//
// -1 = LEFT
//  1 = RIGHT
//  0 = CENTER
// -----------------------------------------------------

int joystickHorizontal() {

  int x = readX();

  if (x < JOY_LOW) {
    return -1;
  }

  if (x > JOY_HIGH) {
    return 1;
  }

  return 0;
}

// -----------------------------------------------------
// Joystick press
// -----------------------------------------------------

bool joystickPressed() {

  bool currentState = digitalRead(JOY_SW);

  if (currentState == LOW && lastButtonState == HIGH) {

    if (millis() - lastButtonTime > 200) {

      lastButtonTime = millis();
      lastButtonState = currentState;

      return true;
    }
  }

  lastButtonState = currentState;

  return false;
}

// =====================================================
// BEEP
// =====================================================

void beep(int frequency, int duration) {

  tone(BUZZER, frequency, duration);

  delay(duration);

  noTone(BUZZER);
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

  if (month == 2) {

    if (isLeapYear(year))
      return 29;

    return 28;
  }

  if (month == 4 ||
      month == 6 ||
      month == 9 ||
      month == 11) {

    return 30;
  }

  return 31;
}

// =====================================================
// DAY OF WEEK
//
// Returns:
// 0 = SUN
// 1 = MON
// 2 = TUE
// 3 = WED
// 4 = THU
// 5 = FRI
// 6 = SAT
// =====================================================

int getDayOfWeek(int day, int month, int year) {

  int t[] = {
    0, 3, 2, 5, 0, 3,
    5, 1, 4, 6, 2, 4
  };

  if (month < 3)
    year--;

  return (year +
          year / 4 -
          year / 100 +
          year / 400 +
          t[month - 1] +
          day) % 7;
}

// =====================================================
// DAY NAME
// =====================================================

const char* getDayName(int dayOfWeek) {

  switch (dayOfWeek) {

    case 0:
      return "SUN";

    case 1:
      return "MON";

    case 2:
      return "TUE";

    case 3:
      return "WED";

    case 4:
      return "THU";

    case 5:
      return "FRI";

    case 6:
      return "SAT";
  }

  return "";
}

// =====================================================
// 12-HOUR CONVERSION
// =====================================================

int getDisplayHour(int hour24) {

  int hour12 = hour24 % 12;

  if (hour12 == 0)
    hour12 = 12;

  return hour12;
}

// =====================================================
// AM / PM
// =====================================================

const char* getAMPM(int hour24) {

  if (hour24 < 12)
    return "AM";

  return "PM";
}

// =====================================================
// UPDATE CLOCK
// =====================================================

void updateClock() {

  unsigned long now = millis();

  if (now - lastClockMillis >= 1000) {

    lastClockMillis += 1000;

    currentSecond++;

    if (currentSecond >= 60) {

      currentSecond = 0;
      currentMinute++;

      if (currentMinute >= 60) {

        currentMinute = 0;
        currentHour++;

        if (currentHour >= 24) {

          currentHour = 0;

          currentDay++;

          if (currentDay >
              daysInMonth(currentMonth, currentYear)) {

            currentDay = 1;

            currentMonth++;

            if (currentMonth > 12) {

              currentMonth = 1;
              currentYear++;
            }
          }
        }
      }
    }

    checkAlarm();
  }
}

// =====================================================
// CHECK ALARM
// =====================================================

void checkAlarm() {

  if (!alarmEnabled)
    return;

  if (currentHour == alarmHour &&
      currentMinute == alarmMinute &&
      currentSecond == 0) {

    alarmRinging = true;
  }
}

// =====================================================
// ALARM SOUND
// =====================================================

void handleAlarmSound() {

  if (!alarmRinging)
    return;

  beep(1200, 200);

  delay(150);

  beep(1500, 200);

  delay(150);

  beep(1200, 200);

  alarmRinging = false;
}

// =====================================================
// DRAW MENU
// =====================================================

void drawMenu() {

  display.clearDisplay();

  display.setTextColor(SSD1306_WHITE);

  display.setTextSize(1);

  display.setCursor(30, 0);
  display.println("DIGITAL CLOCK");

  display.drawLine(
    0, 11,
    127, 11,
    SSD1306_WHITE
  );

  for (int i = 0; i < 4; i++) {

    display.setCursor(15, 16 + i * 11);

    if (i == menuSelection)
      display.print("> ");
    else
      display.print("  ");

    display.println(menuItems[i]);
  }

  display.setCursor(5, 58);
  display.print("UP/DOWN  SELECT");

  display.display();
}

// =====================================================
// DRAW CLOCK
// =====================================================

void drawClock() {

  display.clearDisplay();

  display.setTextColor(SSD1306_WHITE);

  // ---------------------------------------------------
  // 12 HOUR TIME
  // ---------------------------------------------------

  int displayHour =
    getDisplayHour(currentHour);

  display.setTextSize(2);

  display.setCursor(5, 0);

  if (displayHour < 10)
    display.print("0");

  display.print(displayHour);

  display.print(":");

  if (currentMinute < 10)
    display.print("0");

  display.print(currentMinute);

  display.print(":");

  if (currentSecond < 10)
    display.print("0");

  display.print(currentSecond);

  // ---------------------------------------------------
  // AM / PM
  // ---------------------------------------------------

  display.setTextSize(1);

  display.setCursor(105, 6);
  display.print(getAMPM(currentHour));

  // ---------------------------------------------------
  // DAY OF WEEK
  // ---------------------------------------------------

  int dayOfWeek =
    getDayOfWeek(
      currentDay,
      currentMonth,
      currentYear
    );

  display.setTextSize(1);

  display.setCursor(51, 22);
  display.print(getDayName(dayOfWeek));

  // ---------------------------------------------------
  // DATE
  // ---------------------------------------------------

  display.setCursor(32, 33);

  if (currentDay < 10)
    display.print("0");

  display.print(currentDay);

  display.print("/");

  if (currentMonth < 10)
    display.print("0");

  display.print(currentMonth);

  display.print("/");

  display.print(currentYear);

  // ---------------------------------------------------
  // ALARM
  // ---------------------------------------------------

  int alarmDisplayHour =
    getDisplayHour(alarmHour);

  display.setCursor(5, 47);

  display.print("Alarm: ");

  if (alarmDisplayHour < 10)
    display.print("0");

  display.print(alarmDisplayHour);

  display.print(":");

  if (alarmMinute < 10)
    display.print("0");

  display.print(alarmMinute);

  display.print(" ");
  display.print(getAMPM(alarmHour));

  if (alarmEnabled)
    display.print(" ON");
  else
    display.print(" OFF");

  // ---------------------------------------------------
  // BOTTOM
  // ---------------------------------------------------

  display.setCursor(35, 58);
  display.print("PRESS = Set");

  display.display();
}

// =====================================================
// DRAW CLOCK SETTING
// =====================================================

void drawClockSetting() {

  display.clearDisplay();

  display.setTextColor(SSD1306_WHITE);

  display.setTextSize(1);

  display.setCursor(32, 0);
  display.println("SET CLOCK");

  // ---------------------------------------------------
  // HOUR
  // ---------------------------------------------------

  display.setCursor(5, 13);

  if (clockField == 0)
    display.print("> ");
  else
    display.print("  ");

  display.print("Hour: ");

  int displayHour =
    getDisplayHour(currentHour);

  display.print(displayHour);

  display.print(" ");
  display.print(getAMPM(currentHour));

  // ---------------------------------------------------
  // MINUTE
  // ---------------------------------------------------

  display.setCursor(5, 23);

  if (clockField == 1)
    display.print("> ");
  else
    display.print("  ");

  display.print("Minute: ");
  display.println(currentMinute);

  // ---------------------------------------------------
  // SECOND
  // ---------------------------------------------------

  display.setCursor(5, 33);

  if (clockField == 2)
    display.print("> ");
  else
    display.print("  ");

  display.print("Second: ");
  display.println(currentSecond);

  // ---------------------------------------------------
  // DAY
  // ---------------------------------------------------

  display.setCursor(5, 43);

  if (clockField == 3)
    display.print("> ");
  else
    display.print("  ");

  display.print("Day: ");
  display.println(currentDay);

  // ---------------------------------------------------
  // MONTH
  // ---------------------------------------------------

  display.setCursor(5, 53);

  if (clockField == 4)
    display.print("> ");
  else
    display.print("  ");

  display.print("Month: ");
  display.println(currentMonth);

  display.display();
}

// =====================================================
// HANDLE CLOCK SETTING
// =====================================================

void handleClockSetting() {

  int vertical =
    joystickVertical();

  int horizontal =
    joystickHorizontal();

  // ---------------------------------------------------
  // RIGHT = NEXT FIELD
  // LEFT = PREVIOUS FIELD
  // ---------------------------------------------------

  if (horizontal != 0) {

    if (horizontal > 0) {

      clockField++;

      if (clockField > 5)
        clockField = 0;
    }

    else {

      clockField--;

      if (clockField < 0)
        clockField = 5;
    }

    delay(180);
  }

  // ---------------------------------------------------
  // UP / DOWN CHANGES VALUE
  // ---------------------------------------------------

  if (vertical != 0) {

    // HOUR
    if (clockField == 0) {

      currentHour += vertical;

      if (currentHour > 23)
        currentHour = 0;

      if (currentHour < 0)
        currentHour = 23;
    }

    // MINUTE
    else if (clockField == 1) {

      currentMinute += vertical;

      if (currentMinute > 59)
        currentMinute = 0;

      if (currentMinute < 0)
        currentMinute = 59;
    }

    // SECOND
    else if (clockField == 2) {

      currentSecond += vertical;

      if (currentSecond > 59)
        currentSecond = 0;

      if (currentSecond < 0)
        currentSecond = 59;
    }

    // DAY
    else if (clockField == 3) {

      currentDay += vertical;

      int maxDay =
        daysInMonth(
          currentMonth,
          currentYear
        );

      if (currentDay > maxDay)
        currentDay = 1;

      if (currentDay < 1)
        currentDay = maxDay;
    }

    // MONTH
    else if (clockField == 4) {

      currentMonth += vertical;

      if (currentMonth > 12)
        currentMonth = 1;

      if (currentMonth < 1)
        currentMonth = 12;

      int maxDay =
        daysInMonth(
          currentMonth,
          currentYear
        );

      if (currentDay > maxDay)
        currentDay = maxDay;
    }

    // YEAR
    else if (clockField == 5) {

      currentYear += vertical;

      if (currentYear > 2099)
        currentYear = 2000;

      if (currentYear < 2000)
        currentYear = 2099;

      int maxDay =
        daysInMonth(
          currentMonth,
          currentYear
        );

      if (currentDay > maxDay)
        currentDay = maxDay;
    }

    delay(180);
  }

  // ---------------------------------------------------
  // PRESS = SAVE
  // ---------------------------------------------------

  if (joystickPressed()) {

    settingClock = false;

    currentMode = CLOCK_MODE;

    lastClockMillis = millis();

    beep(1000, 80);
  }
}

// =====================================================
// DRAW ALARM
// =====================================================

void drawAlarm() {

  display.clearDisplay();

  display.setTextColor(SSD1306_WHITE);

  display.setTextSize(2);

  display.setCursor(27, 2);
  display.println("ALARM");

  // ---------------------------------------------------
  // ALARM TIME
  // ---------------------------------------------------

  int displayHour =
    getDisplayHour(alarmHour);

  display.setCursor(20, 23);

  if (displayHour < 10)
    display.print("0");

  display.print(displayHour);

  display.print(":");

  if (alarmMinute < 10)
    display.print("0");

  display.print(alarmMinute);

  display.setTextSize(1);

  display.setCursor(94, 29);
  display.print(getAMPM(alarmHour));

  // ---------------------------------------------------
  // STATUS
  // ---------------------------------------------------

  display.setCursor(35, 43);

  if (alarmEnabled)
    display.println("STATUS: ON");
  else
    display.println("STATUS: OFF");

  // ---------------------------------------------------
  // INSTRUCTIONS
  // ---------------------------------------------------

  display.setCursor(7, 56);
  display.println("UP/DOWN Adjust");

  display.display();
}

// =====================================================
// HANDLE ALARM
// =====================================================

void handleAlarm() {

  int vertical =
    joystickVertical();

  if (vertical != 0) {

    alarmMinute += vertical;

    if (alarmMinute > 59) {

      alarmMinute = 0;

      alarmHour++;

      if (alarmHour > 23)
        alarmHour = 0;
    }

    if (alarmMinute < 0) {

      alarmMinute = 59;

      alarmHour--;

      if (alarmHour < 0)
        alarmHour = 23;
    }

    delay(180);
  }

  // PRESS = ON/OFF

  if (joystickPressed()) {

    alarmEnabled =
      !alarmEnabled;

    beep(1000, 100);
  }
}

// =====================================================
// DRAW TIMER
// =====================================================

void drawTimer() {

  display.clearDisplay();

  display.setTextColor(SSD1306_WHITE);

  display.setTextSize(2);

  display.setCursor(30, 2);
  display.println("TIMER");

  unsigned long remaining =
    timerSeconds;

  if (timerRunning) {

    unsigned long elapsed =
      (millis() - timerStartMillis) / 1000;

    if (elapsed >= timerSeconds) {

      remaining = 0;
    }
    else {

      remaining =
        timerSeconds - elapsed;
    }
  }

  int minutes =
    remaining / 60;

  int seconds =
    remaining % 60;

  display.setCursor(30, 25);

  if (minutes < 10)
    display.print("0");

  display.print(minutes);

  display.print(":");

  if (seconds < 10)
    display.print("0");

  display.print(seconds);

  display.setTextSize(1);

  if (timerRunning) {

    display.setCursor(22, 48);
    display.println("RUNNING");
  }
  else {

    display.setCursor(13, 48);
    display.println("PRESS = START");
  }

  display.setCursor(12, 58);
  display.print("UP/DOWN = +/-10s");

  display.display();
}

// =====================================================
// HANDLE TIMER
// =====================================================

void handleTimer() {

  // ---------------------------------------------------
  // TIMER RUNNING
  // ---------------------------------------------------

  if (timerRunning) {

    unsigned long elapsed =
      (millis() - timerStartMillis) / 1000;

    if (elapsed >= timerSeconds) {

      timerRunning = false;

      timerSeconds = 0;

      lastTimerBeep = 999999;

      beep(1500, 300);

      delay(100);

      beep(1500, 300);

      return;
    }

    unsigned long remaining =
      timerSeconds - elapsed;

    // Final 5 seconds
    if (remaining <= 5 &&
        remaining > 0 &&
        remaining != lastTimerBeep) {

      lastTimerBeep = remaining;

      beep(1200, 150);
    }

    return;
  }

  // ---------------------------------------------------
  // TIMER STOPPED
  // ---------------------------------------------------

  int vertical =
    joystickVertical();

  if (vertical != 0) {

    if (vertical > 0) {

      timerSeconds += 10;
    }

    else {

      if (timerSeconds >= 10)
        timerSeconds -= 10;
      else
        timerSeconds = 0;
    }

    delay(180);
  }

  // ---------------------------------------------------
  // PRESS = START
  // ---------------------------------------------------

  if (joystickPressed()) {

    if (timerSeconds > 0) {

      timerStartMillis =
        millis();

      timerRunning = true;

      lastTimerBeep = 999999;

      beep(1000, 80);
    }
  }
}

// =====================================================
// DRAW STOPWATCH
// =====================================================

void drawStopwatch() {

  display.clearDisplay();

  display.setTextColor(SSD1306_WHITE);

  display.setTextSize(2);

  display.setCursor(12, 2);
  display.println("STOPWATCH");

  unsigned long elapsed =
    stopwatchElapsed;

  if (stopwatchRunning) {

    elapsed =
      stopwatchElapsed +
      (millis() - stopwatchStartMillis);
  }

  unsigned long totalSeconds =
    elapsed / 1000;

  int hours =
    totalSeconds / 3600;

  int minutes =
    (totalSeconds % 3600) / 60;

  int seconds =
    totalSeconds % 60;

  display.setTextSize(2);

  display.setCursor(20, 25);

  if (hours < 10)
    display.print("0");

  display.print(hours);

  display.print(":");

  if (minutes < 10)
    display.print("0");

  display.print(minutes);

  display.print(":");

  if (seconds < 10)
    display.print("0");

  display.print(seconds);

  display.setTextSize(1);

  if (stopwatchRunning) {

    display.setCursor(45, 47);
    display.println("RUNNING");
  }
  else {

    display.setCursor(38, 47);
    display.println("STOPPED");
  }

  display.setCursor(10, 58);
  display.print("PRESS Start/Stop");

  display.display();
}

// =====================================================
// HANDLE STOPWATCH
// =====================================================

void handleStopwatch() {

  if (joystickPressed()) {

    if (stopwatchRunning) {

      stopwatchElapsed +=
        millis() -
        stopwatchStartMillis;

      stopwatchRunning = false;
    }

    else {

      stopwatchStartMillis =
        millis();

      stopwatchRunning = true;
    }

    beep(1000, 80);
  }

  // LEFT = RESET

  if (joystickHorizontal() < 0) {

    stopwatchRunning = false;

    stopwatchElapsed = 0;

    beep(800, 80);

    delay(300);
  }
}

// =====================================================
// HANDLE MENU
// =====================================================

void handleMenu() {

  int vertical =
    joystickVertical();

  if (vertical != 0) {

    if (vertical > 0) {

      menuSelection--;

      if (menuSelection < 0)
        menuSelection = 3;
    }

    else {

      menuSelection++;

      if (menuSelection > 3)
        menuSelection = 0;
    }

    beep(1000, 50);

    delay(180);
  }

  // PRESS = SELECT

  if (joystickPressed()) {

    if (menuSelection == 0)
      currentMode = CLOCK_MODE;

    else if (menuSelection == 1)
      currentMode = ALARM_MODE;

    else if (menuSelection == 2)
      currentMode = TIMER_MODE;

    else if (menuSelection == 3)
      currentMode = STOPWATCH_MODE;

    beep(1200, 80);
  }
}

// =====================================================
// RETURN TO MENU
// Hold LEFT for about 0.7 second
// =====================================================

void checkBackToMenu() {

  static unsigned long leftStart = 0;

  int horizontal =
    joystickHorizontal();

  if (horizontal < 0) {

    if (leftStart == 0)
      leftStart = millis();

    if (millis() - leftStart > 700) {

      currentMode = MENU_MODE;

      settingClock = false;

      leftStart = 0;

      beep(800, 80);

      delay(300);
    }
  }

  else {

    leftStart = 0;
  }
}

// =====================================================
// SETUP
// =====================================================

void setup() {

  pinMode(JOY_SW, INPUT_PULLUP);

  pinMode(BUZZER, OUTPUT);

  // ===================================================
  // EXACT SAME OLED INITIALIZATION
  // AS THE WORKING TEST
  // ===================================================

  if (!display.begin(
        SSD1306_SWITCHCAPVCC,
        0x3C)) {

    while (1);
  }

  // ===================================================
  // STARTUP SCREEN
  // ===================================================

  display.clearDisplay();

  display.setTextColor(SSD1306_WHITE);

  display.setTextSize(2);

  display.setCursor(10, 10);
  display.println("DIGITAL");

  display.setCursor(25, 32);
  display.println("CLOCK");

  display.display();

  delay(1500);

  lastClockMillis =
    millis();
}

// =====================================================
// MAIN LOOP
// =====================================================

void loop() {

  // Clock always runs
  updateClock();

  // ---------------------------------------------------
  // MENU
  // ---------------------------------------------------

  if (currentMode == MENU_MODE) {

    drawMenu();

    handleMenu();
  }

  // ---------------------------------------------------
  // CLOCK
  // ---------------------------------------------------

  else if (currentMode == CLOCK_MODE) {

    if (settingClock) {

      drawClockSetting();

      handleClockSetting();
    }

    else {

      drawClock();

      if (joystickPressed()) {

        settingClock = true;

        clockField = 0;

        beep(1000, 80);
      }
    }
  }

  // ---------------------------------------------------
  // ALARM
  // ---------------------------------------------------

  else if (currentMode == ALARM_MODE) {

    drawAlarm();

    handleAlarm();
  }

  // ---------------------------------------------------
  // TIMER
  // ---------------------------------------------------

  else if (currentMode == TIMER_MODE) {

    drawTimer();

    handleTimer();
  }

  // ---------------------------------------------------
  // STOPWATCH
  // ---------------------------------------------------

  else if (currentMode == STOPWATCH_MODE) {

    drawStopwatch();

    handleStopwatch();
  }

  // ---------------------------------------------------
  // BACK TO MENU
  // ---------------------------------------------------

  if (currentMode != MENU_MODE) {

    checkBackToMenu();
  }

  // ---------------------------------------------------
  // ALARM SOUND
  // ---------------------------------------------------

  if (alarmRinging) {

    handleAlarmSound();
  }

  delay(20);
}
