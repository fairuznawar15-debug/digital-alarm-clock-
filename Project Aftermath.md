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

#Code:
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

#define BTN_MENU  2
#define BTN_UP    3
#define BTN_DOWN  4
#define BTN_OK    5

#define BUZZER    9

// =====================================================
// CLOCK VARIABLES
// =====================================================

int currentHour   = 12;
int currentMinute = 0;
int currentSecond = 0;

int currentDay   = 2;
int currentMonth = 10;
int currentYear  = 2026;

unsigned long lastClockUpdate = 0;

// =====================================================
// ALARM
// =====================================================

int alarmHour = 7;
int alarmMinute = 30;

bool alarmEnabled = false;
bool alarmRinging = false;

int lastAlarmDay = -1;

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
// SOUND TYPES
// =====================================================

enum SoundType {
  SOUND_CLICK,
  SOUND_BEEP,
  SOUND_DOUBLE,
  SOUND_MELODY
};

SoundType selectedSound = SOUND_CLICK;

// =====================================================
// MENU
// =====================================================

enum Screen {
  SCREEN_CLOCK,
  SCREEN_MENU,
  SCREEN_SET_CLOCK,
  SCREEN_ALARM,
  SCREEN_TIMER,
  SCREEN_STOPWATCH,
  SCREEN_SOUND
};

Screen screen = SCREEN_CLOCK;

int menuPosition = 0;

const int MENU_ITEMS = 6;

// =====================================================
// SET CLOCK VARIABLES
// =====================================================

int setHour;
int setMinute;
int setDay;
int setMonth;
int setYear;

int clockEditField = 0;

// =====================================================
// ALARM EDIT
// =====================================================

int alarmEditField = 0;

// =====================================================
// BUTTON DEBOUNCE
// =====================================================

unsigned long lastButtonPress = 0;

bool buttonPressed(int pin) {

  if (digitalRead(pin) == LOW) {

    if (millis() - lastButtonPress > 180) {

      lastButtonPress = millis();

      while (digitalRead(pin) == LOW) {
        delay(5);
      }

      return true;
    }
  }

  return false;
}

// =====================================================
// SOUND FUNCTIONS
// =====================================================

void playClick() {

  tone(BUZZER, 1800, 40);
}


void playBeep() {

  tone(BUZZER, 1200, 180);
}


void playDoubleBeep() {

  tone(BUZZER, 1200, 120);
  delay(160);

  tone(BUZZER, 1200, 120);
}


void playMelody() {

  tone(BUZZER, 1000, 120);
  delay(150);

  tone(BUZZER, 1300, 120);
  delay(150);

  tone(BUZZER, 1600, 200);
}


void playSelectedSound() {

  switch (selectedSound) {

    case SOUND_CLICK:
      playClick();
      break;

    case SOUND_BEEP:
      playBeep();
      break;

    case SOUND_DOUBLE:
      playDoubleBeep();
      break;

    case SOUND_MELODY:
      playMelody();
      break;
  }
}

// =====================================================
// ALARM SOUND
// =====================================================

void playAlarmSound() {

  tone(BUZZER, 1600, 250);
  delay(280);

  tone(BUZZER, 1200, 250);
  delay(280);
}

// =====================================================
// DAYS IN MONTH
// =====================================================

int daysInMonth(int month, int year) {

  if (month == 2) {

    if ((year % 4 == 0 && year % 100 != 0) ||
        (year % 400 == 0)) {

      return 29;

    } else {

      return 28;
    }
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
// UPDATE CLOCK
// =====================================================

void updateClock() {

  if (millis() - lastClockUpdate >= 1000) {

    lastClockUpdate += 1000;

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

          if (currentDay > daysInMonth(
                currentMonth,
                currentYear)) {

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
      currentSecond == 0 &&
      lastAlarmDay != currentDay) {

    alarmRinging = true;

    lastAlarmDay = currentDay;
  }

  if (alarmRinging) {

    playAlarmSound();

    display.clearDisplay();

    display.setTextSize(2);

    display.setCursor(25, 5);
    display.println("ALARM!");

    display.setTextSize(1);

    display.setCursor(30, 32);
    display.println("WAKE UP!");

    display.setCursor(15, 50);
    display.println("Press OK to stop");

    display.display();

    if (buttonPressed(BTN_OK)) {

      alarmRinging = false;

      noTone(BUZZER);

      playClick();
    }
  }
}

// =====================================================
// CLOCK SCREEN
// =====================================================

void showClock() {

  display.clearDisplay();

  display.setTextColor(SSD1306_WHITE);

  // Time
  display.setTextSize(2);

  display.setCursor(10, 3);

  if (currentHour < 10)
    display.print("0");

  display.print(currentHour);
  display.print(":");

  if (currentMinute < 10)
    display.print("0");

  display.print(currentMinute);
  display.print(":");

  if (currentSecond < 10)
    display.print("0");

  display.print(currentSecond);

  // Date
  display.setTextSize(1);

  display.setCursor(35, 27);

  if (currentDay < 10)
    display.print("0");

  display.print(currentDay);
  display.print("/");

  if (currentMonth < 10)
    display.print("0");

  display.print(currentMonth);
  display.print("/");

  display.print(currentYear);

  // Alarm
  display.setCursor(15, 42);

  display.print("Alarm: ");

  if (alarmHour < 10)
    display.print("0");

  display.print(alarmHour);
  display.print(":");

  if (alarmMinute < 10)
    display.print("0");

  display.print(alarmMinute);

  display.print(" ");

  if (alarmEnabled)
    display.print("ON");
  else
    display.print("OFF");

  display.setCursor(28, 55);
  display.print("MENU = Menu");

  display.display();
}

// =====================================================
// MAIN MENU
// =====================================================

void showMenu() {

  const char* items[] = {

    "Clock",
    "Set Clock",
    "Alarm",
    "Timer",
    "Stopwatch",
    "Sound"
  };

  display.clearDisplay();

  display.setTextSize(1);

  display.setCursor(48, 0);
  display.println("MENU");

  for (int i = 0; i < MENU_ITEMS; i++) {

    int y = 10 + i * 9;

    display.setCursor(8, y);

    if (i == menuPosition)
      display.print("> ");
    else
      display.print("  ");

    display.println(items[i]);
  }

  display.display();
}

// =====================================================
// MENU HANDLER
// =====================================================

void handleMenu() {

  if (buttonPressed(BTN_UP)) {

    menuPosition--;

    if (menuPosition < 0)
      menuPosition = MENU_ITEMS - 1;

    playClick();
  }

  if (buttonPressed(BTN_DOWN)) {

    menuPosition++;

    if (menuPosition >= MENU_ITEMS)
      menuPosition = 0;

    playClick();
  }

  if (buttonPressed(BTN_OK)) {

    playClick();

    switch (menuPosition) {

      case 0:
        screen = SCREEN_CLOCK;
        break;

      case 1:

        setHour = currentHour;
        setMinute = currentMinute;
        setDay = currentDay;
        setMonth = currentMonth;
        setYear = currentYear;

        clockEditField = 0;

        screen = SCREEN_SET_CLOCK;

        break;

      case 2:

        alarmEditField = 0;

        screen = SCREEN_ALARM;

        break;

      case 3:

        screen = SCREEN_TIMER;

        break;

      case 4:

        screen = SCREEN_STOPWATCH;

        break;

      case 5:

        screen = SCREEN_SOUND;

        break;
    }
  }
}

// =====================================================
// SET CLOCK SCREEN
// =====================================================

void showSetClock() {

  display.clearDisplay();

  display.setTextSize(1);

  display.setCursor(35, 0);
  display.println("SET CLOCK");

  display.setTextSize(2);

  display.setCursor(18, 13);

  if (setHour < 10)
    display.print("0");

  display.print(setHour);
  display.print(":");

  if (setMinute < 10)
    display.print("0");

  display.print(setMinute);

  display.setTextSize(1);

  display.setCursor(25, 34);

  if (setDay < 10)
    display.print("0");

  display.print(setDay);
  display.print("/");

  if (setMonth < 10)
    display.print("0");

  display.print(setMonth);
  display.print("/");

  display.print(setYear);

  display.setCursor(5, 48);

  if (clockEditField == 0)
    display.print("> Hour");
  else if (clockEditField == 1)
    display.print("> Minute");
  else if (clockEditField == 2)
    display.print("> Day");
  else if (clockEditField == 3)
    display.print("> Month");
  else
    display.print("> Year");

  display.setCursor(5, 58);
  display.print("UP/DN=Change OK=Next");

  display.display();
}

// =====================================================
// HANDLE CLOCK SETTING
// =====================================================

void handleSetClock() {

  if (buttonPressed(BTN_UP)) {

    switch (clockEditField) {

      case 0:

        setHour++;

        if (setHour >= 24)
          setHour = 0;

        break;

      case 1:

        setMinute++;

        if (setMinute >= 60)
          setMinute = 0;

        break;

      case 2:

        setDay++;

        if (setDay >
            daysInMonth(setMonth, setYear))

          setDay = 1;

        break;

      case 3:

        setMonth++;

        if (setMonth > 12)
          setMonth = 1;

        if (setDay >
            daysInMonth(setMonth, setYear))

          setDay = daysInMonth(
              setMonth,
              setYear);

        break;

      case 4:

        setYear++;

        if (setYear > 2099)
          setYear = 2026;

        break;
    }

    playClick();
  }

  if (buttonPressed(BTN_DOWN)) {

    switch (clockEditField) {

      case 0:

        setHour--;

        if (setHour < 0)
          setHour = 23;

        break;

      case 1:

        setMinute--;

        if (setMinute < 0)
          setMinute = 59;

        break;

      case 2:

        setDay--;

        if (setDay < 1)
          setDay =
            daysInMonth(
              setMonth,
              setYear);

        break;

      case 3:

        setMonth--;

        if (setMonth < 1)
          setMonth = 12;

        if (setDay >
            daysInMonth(setMonth, setYear))

          setDay = daysInMonth(
              setMonth,
              setYear);

        break;

      case 4:

        setYear--;

        if (setYear < 2026)
          setYear = 2099;

        break;
    }

    playClick();
  }

  if (buttonPressed(BTN_OK)) {

    clockEditField++;

    playClick();

    if (clockEditField > 4) {

      currentHour = setHour;
      currentMinute = setMinute;
      currentSecond = 0;

      currentDay = setDay;
      currentMonth = setMonth;
      currentYear = setYear;

      lastClockUpdate = millis();

      screen = SCREEN_CLOCK;
    }
  }

  if (buttonPressed(BTN_MENU)) {

    screen = SCREEN_MENU;

    playClick();
  }
}

// =====================================================
// ALARM SCREEN
// =====================================================

void showAlarm() {

  display.clearDisplay();

  display.setTextSize(1);

  display.setCursor(45, 0);
  display.println("ALARM");

  display.setTextSize(2);

  display.setCursor(25, 12);

  if (alarmHour < 10)
    display.print("0");

  display.print(alarmHour);

  display.print(":");

  if (alarmMinute < 10)
    display.print("0");

  display.print(alarmMinute);

  display.setTextSize(1);

  display.setCursor(35, 35);

  if (alarmEnabled)
    display.println("STATUS: ON");
  else
    display.println("STATUS: OFF");

  display.setCursor(5, 47);

  if (alarmEditField == 0)
    display.print("> Hour");
  else
    display.print("> Minute");

  display.setCursor(5, 57);
  display.print("UP/DN Change OK Next");

  display.display();
}

// =====================================================
// HANDLE ALARM
// =====================================================

void handleAlarm() {

  if (buttonPressed(BTN_UP)) {

    if (alarmEditField == 0) {

      alarmHour++;

      if (alarmHour >= 24)
        alarmHour = 0;

    } else {

      alarmMinute++;

      if (alarmMinute >= 60)
        alarmMinute = 0;
    }

    playClick();
  }

  if (buttonPressed(BTN_DOWN)) {

    if (alarmEditField == 0) {

      alarmHour--;

      if (alarmHour < 0)
        alarmHour = 23;

    } else {

      alarmMinute--;

      if (alarmMinute < 0)
        alarmMinute = 59;
    }

    playClick();
  }

  if (buttonPressed(BTN_OK)) {

    if (alarmEditField == 0) {

      alarmEditField = 1;

    } else {

      alarmEnabled = !alarmEnabled;
      alarmEditField = 0;
    }

    playClick();
  }

  if (buttonPressed(BTN_MENU)) {

    screen = SCREEN_MENU;

    playClick();
  }
}

// =====================================================
// TIMER SCREEN
// =====================================================

void showTimer() {

  unsigned long seconds;

  if (timerRunning)
    seconds = timerRemaining;
  else
    seconds = timerSetSeconds;

  int minutes = seconds / 60;
  int secs = seconds % 60;

  display.clearDisplay();

  display.setTextSize(1);

  display.setCursor(45, 0);
  display.println("TIMER");

  display.setTextSize(2);

  display.setCursor(25, 16);

  if (minutes < 10)
    display.print("0");

  display.print(minutes);

  display.print(":");

  if (secs < 10)
    display.print("0");

  display.print(secs);

  display.setTextSize(1);

  display.setCursor(15, 40);

  if (timerRunning)
    display.println("RUNNING");
  else
    display.println("UP/DOWN = +/-10 sec");

  display.setCursor(20, 54);

  if (timerRunning)
    display.println("OK = Stop");
  else
    display.println("OK = Start");

  display.display();
}

// =====================================================
// TIMER HANDLER
// =====================================================

void handleTimer() {

  if (!timerRunning) {

    if (buttonPressed(BTN_UP)) {

      timerSetSeconds += 10;

      if (timerSetSeconds > 3599)
        timerSetSeconds = 3599;

      playClick();
    }

    if (buttonPressed(BTN_DOWN)) {

      if (timerSetSeconds >= 10)
        timerSetSeconds -= 10;

      playClick();
    }

    if (buttonPressed(BTN_OK)) {

      timerRemaining = timerSetSeconds;

      timerRunning = true;

      lastTimerUpdate = millis();

      playClick();
    }

  } else {

    if (buttonPressed(BTN_OK)) {

      timerRunning = false;

      playClick();
    }
  }

  if (buttonPressed(BTN_MENU)) {

    timerRunning = false;

    screen = SCREEN_MENU;

    playClick();
  }
}

// =====================================================
// UPDATE TIMER
// =====================================================

void updateTimer() {

  if (!timerRunning)
    return;

  if (millis() - lastTimerUpdate >= 1000) {

    lastTimerUpdate += 1000;

    if (timerRemaining > 0) {

      timerRemaining--;

      // Final 5 seconds
      if (timerRemaining <= 5 &&
          timerRemaining > 0) {

        playSelectedSound();
      }
    }

    // Reached zero
    if (timerRemaining == 0) {

      timerRunning = false;

      // Finished sound
      playMelody();
      delay(100);

      playMelody();
    }
  }
}

// =====================================================
// STOPWATCH SCREEN
// =====================================================

void showStopwatch() {

  unsigned long elapsed;

  if (stopwatchRunning)
    elapsed = millis() - stopwatchStart;
  else
    elapsed = stopwatchElapsed;

  unsigned long totalSeconds = elapsed / 1000;

  unsigned long minutes =
      totalSeconds / 60;

  unsigned long seconds =
      totalSeconds % 60;

  unsigned long hundredths =
      (elapsed % 1000) / 10;

  display.clearDisplay();

  display.setTextSize(1);

  display.setCursor(35, 0);
  display.println("STOPWATCH");

  display.setTextSize(2);

  display.setCursor(8, 17);

  if (minutes < 10)
    display.print("0");

  display.print(minutes);

  display.print(":");

  if (seconds < 10)
    display.print("0");

  display.print(seconds);

  display.setTextSize(1);

  display.print(".");

  if (hundredths < 10)
    display.print("0");

  display.print(hundredths);

  display.setCursor(15, 43);

  if (stopwatchRunning)
    display.println("OK = Stop");
  else
    display.println("OK = Start");

  display.setCursor(15, 55);
  display.println("DOWN = Reset");

  display.display();
}

// =====================================================
// STOPWATCH HANDLER
// =====================================================

void handleStopwatch() {

  if (buttonPressed(BTN_OK)) {

    if (!stopwatchRunning) {

      stopwatchStart =
          millis() - stopwatchElapsed;

      stopwatchRunning = true;

    } else {

      stopwatchElapsed =
          millis() - stopwatchStart;

      stopwatchRunning = false;
    }

    playClick();
  }

  if (buttonPressed(BTN_DOWN)) {

    stopwatchRunning = false;

    stopwatchElapsed = 0;

    playClick();
  }

  if (buttonPressed(BTN_MENU)) {

    screen = SCREEN_MENU;

    playClick();
  }
}

// =====================================================
// SOUND SCREEN
// =====================================================

void showSound() {

  const char* sounds[] = {

    "Click",
    "Beep",
    "Double Beep",
    "Melody"
  };

  display.clearDisplay();

  display.setTextSize(1);

  display.setCursor(42, 0);
  display.println("SOUND");

  for (int i = 0; i < 4; i++) {

    display.setCursor(15, 12 + i * 11);

    if (i == selectedSound)
      display.print("> ");
    else
      display.print("  ");

    display.println(sounds[i]);
  }

  display.setCursor(5, 56);
  display.println("UP/DN Select OK Test");

  display.display();
}

// =====================================================
// SOUND HANDLER
// =====================================================

void handleSound() {

  if (buttonPressed(BTN_UP)) {

    selectedSound =
        (SoundType)((selectedSound + 3) % 4);

    playClick();
  }

  if (buttonPressed(BTN_DOWN)) {

    selectedSound =
        (SoundType)((selectedSound + 1) % 4);

    playClick();
  }

  if (buttonPressed(BTN_OK)) {

    playSelectedSound();
  }

  if (buttonPressed(BTN_MENU)) {

    screen = SCREEN_MENU;

    playClick();
  }
}

// =====================================================
// SETUP
// =====================================================

void setup() {

  pinMode(BTN_MENU, INPUT_PULLUP);
  pinMode(BTN_UP, INPUT_PULLUP);
  pinMode(BTN_DOWN, INPUT_PULLUP);
  pinMode(BTN_OK, INPUT_PULLUP);

  pinMode(BUZZER, OUTPUT);

  Serial.begin(9600);

  // Start OLED
  if (!display.begin(
        SSD1306_SWITCHCAPVCC,
        0x3C)) {

    while (1);
  }

  display.clearDisplay();

  display.setTextColor(SSD1306_WHITE);

  display.setTextSize(2);

  display.setCursor(15, 15);
  display.println("DIGITAL");

  display.setCursor(20, 38);
  display.println("CLOCK");

  display.display();

  playMelody();

  delay(1500);

  lastClockUpdate = millis();
}

// =====================================================
// MAIN LOOP
// =====================================================

void loop() {

  // Keep clock running
  updateClock();

  // Keep timer running
  updateTimer();

  // Check alarm
  checkAlarm();

  // Don't process normal screens
  // while alarm is ringing
  if (alarmRinging)
    return;

  switch (screen) {

    // -------------------------
    // CLOCK
    // -------------------------

    case SCREEN_CLOCK:

      showClock();

      if (buttonPressed(BTN_MENU)) {

        screen = SCREEN_MENU;

        playClick();
      }

      break;


    // -------------------------
    // MENU
    // -------------------------

    case SCREEN_MENU:

      showMenu();

      handleMenu();

      break;


    // -------------------------
    // SET CLOCK
    // -------------------------

    case SCREEN_SET_CLOCK:

      showSetClock();

      handleSetClock();

      break;


    // -------------------------
    // ALARM
    // -------------------------

    case SCREEN_ALARM:

      showAlarm();

      handleAlarm();

      break;


    // -------------------------
    // TIMER
    // -------------------------

    case SCREEN_TIMER:

      showTimer();

      handleTimer();

      break;


    // -------------------------
    // STOPWATCH
    // -------------------------

    case SCREEN_STOPWATCH:

      showStopwatch();

      handleStopwatch();

      break;


    // -------------------------
    // SOUND
    // -------------------------

    case SCREEN_SOUND:

      showSound();

      handleSound();

      break;
  }

  delay(20);
}
